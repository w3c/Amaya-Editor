/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2010
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Set of functions to parse CSS rules:
 * Each ParseCSS function calls one or more Thot style API function.
 *
 * Author: I. Vatton (INRIA)
 *
 */
 
/* Included headerfiles */

#ifdef _WX
#include "wx/wx.h"
#include "wx/utils.h"
#include "wx/file.h"
#endif /* _WX */

#define THOT_EXPORT extern
#include "amaya.h"
#include "parser.h"
#include "css.h"
#include "XLink.h"
#include "MathML.h"
#ifdef _SVG
#include "SVG.h"
#endif /* SVG */
#ifdef TEMPLATES
#include "Template.h"
#include "templates.h"
#include "templateDeclarations_f.h"
#include "templateUtils_f.h"
#endif /* TEMPLATES */
#include "templates_f.h"
#ifdef _WX
#include "message_wx.h"
#endif /* _WX */

static char        *TargetDocumentURL = NULL;
static int          OldWidth;
static int          OldHeight;
#define buflen 50

#include "AHTURLTools_f.h"
#include "css_f.h"
#include "EDITimage_f.h"
#include "EDITstyle_f.h"
#include "EDITORactions_f.h"
#include "fetchHTMLname_f.h"
#include "fetchXMLname_f.h"
#include "SVGbuilder_f.h"
#include "anim_f.h"
#include "HTMLactions_f.h"
#include "HTMLedit_f.h"
#include "HTMLimage_f.h"
#include "HTMLpresentation_f.h"
#include "HTMLtable_f.h"
#include "html2thot_f.h"
#include "init_f.h"
#include "Mathedit_f.h"
#include "MathMLbuilder_f.h"
#include "styleparser_f.h"
#include "XHTMLbuilder_f.h"
#include "XLinkedit_f.h"
#include "tree.h"
#include "interface.h"


#ifdef _WINGUI
#include "wininclude.h"
#endif /* _WINGUI */
#ifdef _WX
#include "wxdialogapi_f.h"
#include "paneltypes_wx.h"
#include "appdialogue_wx.h"
#endif /* _WX */

static ThotBool     Document_state;

/*----------------------------------------------------------------------
  InsertBefore inserts an empty element before the selection
  ----------------------------------------------------------------------*/
void InsertBefore (Document doc, View view)
{
  TtaInsertAnyElement (doc, TRUE);
}

/*----------------------------------------------------------------------
  InsertAfter inserts an empty element after the selection
  ----------------------------------------------------------------------*/
void InsertAfter (Document doc, View view)
{
  TtaInsertAnyElement (doc, FALSE);
}

/*----------------------------------------------------------------------
  GetNoTemplateSibling
  ----------------------------------------------------------------------*/
Element GetNoTemplateSibling (Element el, ThotBool before)
{
  ElementType         elType;

  if (el)
    {
      do
        {
          if (before)
            TtaPreviousSibling (&el);
          else
            TtaNextSibling (&el);
          elType = TtaGetElementType (el);
        }
      while (el && !strcmp (TtaGetSSchemaName (elType.ElSSchema), "Template"));
    }
  return el;
}

/*----------------------------------------------------------------------
  GetNoTemplateChild
  ----------------------------------------------------------------------*/
Element GetNoTemplateChild (Element el, ThotBool first)
{
  ElementType         elType;

  if (el)
    {
      do
        {
          if (first)
            el = TtaGetFirstChild (el);
          else
            el = TtaGetLastChild (el);
          elType = TtaGetElementType (el);
        }
      while (el && !TtaIsLeaf (elType) &&
             !strcmp (TtaGetSSchemaName (elType.ElSSchema), "Template"));
    }
  return el;
}

/*----------------------------------------------------------------------
  GetNoTemplateParent
  ----------------------------------------------------------------------*/
Element GetNoTemplateParent (Element el)
{
  ElementType         elType;

  if (el)
    el = TtaGetParent (el);
  if (el)
    {
      elType = TtaGetElementType (el);
      if (strcmp (TtaGetSSchemaName (elType.ElSSchema), "Template"))
        return el;
    }
  return NULL;
}

/*----------------------------------------------------------------------
  GetNextNode
  Return the next node in the tree, using a complete traversal algorithm.
  ----------------------------------------------------------------------*/
Element GetNextNode (Element curr)
{
  Element             el;
  ElementType         elType;

  if (!curr)
    return NULL;

  /* get the next child */
  el = GetNoTemplateChild (curr, TRUE);
  if (el == NULL)
    {
      el = curr;
      do
        {
          /* get the next siblign, or parent
             if there was no other sibling */
          el = TtaGetSuccessor (el);
          elType = TtaGetElementType (el);
        }
      while (el && !strcmp (TtaGetSSchemaName (elType.ElSSchema), "Template"));
    }
  return el;
}

/*----------------------------------------------------------------------
  SetTargetContent
  Set the new value of Target.                  
  ----------------------------------------------------------------------*/
void SetTargetContent (Document doc, Attribute attrNAME)
{
  int                 length;

  /* the document that issues the command Set target becomes the target doc */
  if (TargetDocumentURL)
    TtaFreeMemory (TargetDocumentURL);
  if (doc)
    {
      length = strlen (DocumentURLs[doc]);
      TargetDocumentURL = (char *)TtaGetMemory (length + 1);
      strcpy (TargetDocumentURL, DocumentURLs[doc]);
    }

  if (TargetName)
    {
      TtaFreeMemory (TargetName);
      TargetName = NULL;
    }
  if (attrNAME)
    {
      /* get a buffer for the NAME */
      length = TtaGetTextAttributeLength (attrNAME);
      TargetName = (char *)TtaGetMemory (length + 1);
      /* copy the NAME attribute into TargetName */
      TtaGiveTextAttributeValue (attrNAME, TargetName, &length);
    }
}

/*----------------------------------------------------------------------
  LinkToPreviousTarget
  If current selection is within an anchor, change that link, otherwise
  create a link.
  ----------------------------------------------------------------------*/
void LinkToPreviousTarget (Document doc, View view)
{
  Element             el;
  Attribute           attr;
  int                 firstSelectedChar, i;

  if (!TtaGetDocumentAccessMode (doc))
    /* the document is in ReadOnly mode */
    return;
  if ((TargetName == NULL || TargetName[0] == EOS) &&
      (TargetDocumentURL == NULL || TargetDocumentURL[0] == EOS))
    /* no target available */
    return;
  if ((TargetName == NULL || TargetName[0] == EOS) &&
      !strcmp (TargetDocumentURL, DocumentURLs[doc]))
    /* it's not a valid target */
    return;

  TtaGiveFirstSelectedElement (doc, &el, &firstSelectedChar, &i);
  if (TtaIsReadOnly (el))
    /* the selected element is read-only */
    return;

  if (el != NULL)
    {
      UseLastTarget = TRUE;
      AttrHREFundoable = TRUE;
      /* Look if there is an enclosing anchor element */
      el = SearchAnchor (doc, el, &attr, TRUE);
      if (el == NULL)
        {
          /* The link element is a new created one */
          IsNewAnchor = TRUE;
          /* no anchor element, create a new link */
          CreateAnchor (doc, view, TRUE);
        }
      else
        {
          /* The link element already exists */
          IsNewAnchor = FALSE;
          /* There is an anchor. Just points to the last created target */
          SetREFattribute (el, doc, TargetDocumentURL, TargetName);
        }
    }
}


/*----------------------------------------------------------------------
  AddAccessKey
  -----------------------------------------------------------------------*/
void AddAccessKey (NotifyAttribute *event)
{
  char              buffer[2];
  int                 length;

  /* get the access key */
  length = 1;
  TtaGiveTextAttributeValue (event->attribute, buffer, &length);
  if (length > 0)
    TtaAddAccessKey (event->document, (unsigned int)buffer[0], event->element);
}


/*----------------------------------------------------------------------
  RemoveAccessKey
  -----------------------------------------------------------------------*/
ThotBool RemoveAccessKey (NotifyAttribute *event)
{
  char              buffer[2];
  int                 length;

  /* get the access key */
  length = 2;
  TtaGiveTextAttributeValue (event->attribute, buffer, &length);
  if (length > 0)
    TtaRemoveAccessKey (event->document, (int)buffer[0]);
  return FALSE; /* let Thot perform normal operation */
}


/*----------------------------------------------------------------------
  RemoveLink
  Destroy the link element (HTML) or PI element (others) and 
  remove CSS rules when the link points to a CSS file.
  ----------------------------------------------------------------------*/
void RemoveLink (Element el, Document doc)
{
  Element	          elText;
  ElementType	      elType;
  AttributeType     attrType;
  Attribute         attr;
  char             *s, *ptr = NULL, *end = NULL;
  char              buffer[MAX_LENGTH], cssname[MAX_LENGTH];
  char              pathname[MAX_LENGTH], documentname[MAX_LENGTH];   
  int               length, piNum;
  Language          lang;

  /* Search the "nature" of the link */
  elType = TtaGetElementType (el);
  s = TtaGetSSchemaName (elType.ElSSchema);
  if ((strcmp (s, "HTML") == 0) &&
      (elType.ElTypeNum != HTML_EL_XMLPI))
    {
      /* (X)HTML document, well, we search within a hlink element */
      attrType.AttrSSchema = elType.ElSSchema;
      if (IsCSSLink (el, doc))
        {
          /* it's a link to a style sheet. Remove that style sheet */
          attrType.AttrTypeNum = HTML_ATTR_HREF_;
          attr = TtaGetAttribute (el, attrType);
          if (attr)
            {
              /* copy the HREF attribute into the buffer */
              length = MAX_LENGTH;
              TtaGiveTextAttributeValue (attr, buffer, &length);
              NormalizeURL (buffer, doc, pathname, documentname, NULL);
              RemoveStyle (pathname, doc, TRUE, TRUE, NULL, CSS_EXTERNAL_STYLE);
            }
        }
    }
  else
    {
      if (strcmp (s, "HTML") == 0)
        piNum = HTML_EL_XMLPI;
      else if (strcmp (s, "MathML") == 0)
        piNum = MathML_EL_XMLPI;
      else if (strcmp (s, "SVG") == 0)
        piNum = SVG_EL_XMLPI;
      else
        piNum = XML_EL_xmlpi;
      if (elType.ElTypeNum == piNum)
        {
          /* We search within an xml PI element */
          elType.ElTypeNum = 1;
          elText = TtaSearchTypedElement (elType, SearchInTree, el);
          if (elText != NULL)
            {
              length = MAX_LENGTH - 1;
              TtaGiveTextContent (elText, (unsigned char *)buffer, &length, &lang);
              buffer[length++] = EOS;
              /* Search the name of the stylesheet */
              ptr = strstr (buffer, "href");
              if (ptr != NULL)
                {
                  ptr = strstr (ptr, "\"");
                  ptr++;
                }
              if (ptr != NULL)
                {
                  end = strstr (ptr, "\"");
                  *end = EOS;
                  strcpy (cssname, ptr);
                }		
              NormalizeURL (cssname, doc, pathname, documentname, NULL);
              RemoveStyle (pathname, doc, TRUE, TRUE, NULL, CSS_EXTERNAL_STYLE);
            }
        }
    }
  return;
}

/*----------------------------------------------------------------------
  DeleteLink                                              
  ----------------------------------------------------------------------*/
ThotBool DeleteLink (NotifyElement *event)
{
  RemoveLink (event->element, event->document);
  UpdateStyleList (event->document, 1);
  return FALSE;		/* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
  CheckMandatory
  Check whether the attribute could be removed.
  ----------------------------------------------------------------------*/
ThotBool CheckMandatory (NotifyAttribute *event)
{
  AttributeType       attrType;
  ElementType         elType;

  attrType.AttrSSchema = event->attributeType.AttrSSchema;
  attrType.AttrTypeNum = event->attributeType.AttrTypeNum;
  elType = TtaGetElementType (event->element);
  if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML") &&
      elType.ElTypeNum == HTML_EL_Anchor)
    /* it's an anchor. It should have at least a name or href attribute */
    {
      if (attrType.AttrTypeNum == HTML_ATTR_HREF_)
        /* the user wants to remove the href attribute.
           check if there is a name or an id attribute */
        {
          attrType.AttrTypeNum = HTML_ATTR_NAME;
          if (!TtaGetAttribute (event->element, attrType))
            /* there is no name attribute. Check id */
            {
              attrType.AttrTypeNum = HTML_ATTR_ID;
              if (!TtaGetAttribute (event->element, attrType))
                /* no id attribute. Refuse. */
                return TRUE;
            }
        }
      else
        /* the user wants to remove the name attribute.
           check if there is a href or id attribute */
        {
          attrType.AttrTypeNum = HTML_ATTR_HREF_;
          if (!TtaGetAttribute (event->element, attrType))
            /* there is no href attribute. Check id */
            {
              attrType.AttrTypeNum = HTML_ATTR_ID;
              if (!TtaGetAttribute (event->element, attrType))
                /* no id attribute. Refuse. */
                return TRUE;
            }
        }
    }
  return FALSE;		/* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
  GenerateInlinechildren generates the newType inline element as children
  of block elements.
  -----------------------------------------------------------------------*/
static Element GenerateInlinechildren (Element el, ElementType newType, Document doc)
{
  ElementType	    elType, childType;
  Element         child, added, next, last, in_line, prev;

  /* generate a strong, etc. into the block element? */
  elType = TtaGetElementType (el);
  if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML"))
    {
      child = TtaGetFirstChild (el);
      childType = TtaGetElementType (child);
      last = TtaGetLastChild (el);
      if (child == NULL)
        {
          /* create a in_line element */
          in_line = TtaNewElement (doc, newType);
          prev = NULL;
          if (elType.ElTypeNum == HTML_EL_Basic_Elem ||
              elType.ElTypeNum == HTML_EL_Element)
            {
              // this is a temporary element
              child = el;
              el = TtaGetParent (child);
              prev = child;
              TtaPreviousSibling (&prev);
              TtaRegisterElementDelete (child, doc);
              TtaDeleteTree (child, doc);
            }
          // generate a text
          newType.ElTypeNum = HTML_EL_TEXT_UNIT;
          child = TtaNewElement (doc, newType);
          TtaInsertFirstChild (&child, in_line, doc);
          if (prev)
            TtaInsertSibling (in_line, prev, FALSE, doc);
          else
            TtaInsertFirstChild (&in_line, el, doc);
          TtaRegisterElementCreate (in_line, doc);
          // the HTML_EL_Basic_Elem was replaced by in_line
          el = in_line;
        }
      else if (child && IsCharacterLevelElement (child) &&
               (last != child ||
                childType.ElSSchema != newType.ElSSchema ||
                childType.ElTypeNum != newType.ElTypeNum))
        {
          /* create a in_line element */
          in_line = TtaNewElement (doc, newType);
          added = NULL;
          while (child)
            {
              next = child;
              TtaNextSibling (&next);
              TtaRegisterElementDelete (child, doc);
              TtaRemoveTree (child, doc);
              // add at the end of the in_line
              if (added)
                TtaInsertSibling (child, added, FALSE, doc);
              else
                {
                  added = child;
                  TtaInsertFirstChild (&added, in_line, doc);
                }
              added = child;
              child = next;
            }
          // insert into the new created in_line
          TtaInsertFirstChild (&in_line, el, doc);
          TtaRegisterElementCreate (in_line, doc);
          in_line = NULL;
        }
      else
        {
          while (child)
            {
              GenerateInlinechildren (child, newType, doc);
              TtaNextSibling (&child);
            }
        }
    }
  return el;
}

/*----------------------------------------------------------------------
  AttributeChange generates an aType attribute with data value on the
  current selection from the attribute menu.
  -----------------------------------------------------------------------*/
void AttributeChange (int aType, char * data)
{
  GenerateInlineElement (HTML_EL_Span, NULL, aType, data, TRUE);
}

/*----------------------------------------------------------------------
  UpdateAttribute
  Add the CSS rule (property:value) contained in data to the content of
  attribute attr (a style attribute). If the property to be added is already
  present in attr, replace the existing rule by the new one.
  -----------------------------------------------------------------------*/
static void UpdateAttribute (Attribute attr, char * data, Element el, Document doc)
{
  char     *buffer, *property, *start, *end, *current;
  char      value[50];
  int       lg, val = 0, inc;
  ThotBool  margin, found;

  lg = TtaGetTextAttributeLength (attr);
  property = strstr (data, ":");
  if (lg && property)
    {
      // look for the property in the initial content of attr
      buffer = (char *)TtaGetMemory (lg + strlen (data) + 2);
      TtaGiveTextAttributeValue (attr, buffer, &lg);
      *property = EOS; // isolate the property name in the new CSS rule
      lg = strlen (data);   // length of the property name
      margin = !strcmp (data, "margin-left");
      found = FALSE;
      current = buffer;
      do
	{
	  start = strstr (current, data);
	  if (start)
	    {
	      /* the name of the property is present in the initial content */
	      /* check with the previous and following character that it is
		 really the property name we are looking for */
	      if ((start == buffer || start[-1] == SPACE || start[-1] == ';') &&
		  (start[lg] == SPACE || start[lg] == ':'))
		/* the string is at the beginning of the buffer or it is
		   preceded by a space or a semi-colon, and it is followed by a
		   space or a colon. It's really the property name we are
		   looking for */
		found = TRUE;
	      else
		{
		  /* it's not the full name we are looking for. For instance,
		     it could be the substring "color" of property
		     "background-color" when we are looking for property
		     "color". Check further. */
		  current = start + 1;
		}
	    }
	}
      while (!found && start);
      *property = ':';
      if (start && found)
	/* the property was already present. Remove the rule. */
        {
	  // get the end of the rule
          end = start;
          while (*end != EOS && *end != ';')
            end++;
          if (*end == EOS)
            {
              // the rule is the last one in the buffer
	      if (margin)
		val = GetEmValue (&start[12], el, doc);
              *start = EOS;
            }
          else
            {
              // there are other rules after this one
	      if (margin)
		{
		  *end = EOS;
		  val = GetEmValue (&start[12], el, doc);
		  *end = ';';
		}
	      // copy the rest of the buffer over the rule to be removed
              while (*end != EOS)
                {
                  end++;
                  *start = *end;
                  start++;
                }
            }
        }
      else
        // no margin update
        margin = FALSE;
      lg = strlen (buffer);
      if (lg && buffer[lg-1] != ';')
        strcat (buffer, ";");
      // append the new rule
      if (margin)
        {
          // increment or decrement the existing value
          inc = atoi (&property[1]);
          if (inc == 0 && val != 0)
            val -= 2;
          else if (inc == 2)
            val += 2;
          sprintf ((char *)value, "margin-left:%dem", (int)val);
          strcat (buffer, value);
          ParseHTMLSpecificStyle (el, value, doc, 1000, FALSE);
        }
      else
        {
          strcat (buffer, data);
	  // apply the new rule
          ParseHTMLSpecificStyle (el, data, doc, 1000, FALSE);
        }
      // update the style attribute
      TtaSetAttributeText (attr, buffer, el, doc);
      TtaFreeMemory (buffer);
    }
  else
    /* there was no initial content in attr. Just put data */
    TtaSetAttributeText (attr, data, el, doc);
}

/*----------------------------------------------------------------------
  GenerateInlineElement
  Generates the elType inline element around the selection with the
  attribute (if aType is not 0) and sets the value data to the attribute.
  If a selected element is not inline, adds the attribute if possible.
  If the selection is within a style element and the element type is
  a span adds the data into the style element.
  -----------------------------------------------------------------------*/
ThotBool GenerateInlineElement (int eType, SSchema eSchema, int aType,
                                const char * data, ThotBool replace)
{
  Element         el, firstSel, lastSel, next, in_line, sibling, child;
  Element         last, parent, enclose, selected, lastChild;
  ElementType	  elType, parentType, newType, childType;
  Attribute       newAttr, attr;
  AttributeType   attrType;
  Document        doc;
  Language        lang;
  CHAR_T         *buffer;
  DisplayMode     dispMode;
  ThotBool	      doit, split, before, charlevel, inside, done, removed;
  ThotBool        lastChanged, parse, open, selpos, isPict = FALSE, skip;
  SSchema         templateSSchema;
  int             i, j, firstchar, lastchar, lg, min, max;
  char           *name;
  char           *tmpdata;

  doc = TtaGetSelectedDocument();
  done = FALSE;
  if (doc)
    {
      if (DocumentTypes[doc] == docText ||
          DocumentTypes[doc] == docCSS ||
          DocumentTypes[doc] == docSource)
        {
          TtaPasteFromBuffer ((unsigned char*)data, strlen(data), TtaGetDefaultCharset ());
          done = TRUE;
        }
      else
        {
          /* give current position */
          TtaGiveFirstSelectedElement (doc, &firstSel, &firstchar, &i);
          TtaGiveLastSelectedElement (doc, &lastSel, &j, &lastchar);
          if (TtaIsReadOnly (firstSel))
            {
              /* the selected element is read-only */
              TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_READONLY);
              return FALSE;
            }
         
#ifdef TEMPLATES
          /* Verify if template allow this element.*/
          templateSSchema = TtaGetSSchema ("Template", doc);
          if (templateSSchema)
            {
              parent = GetFirstTemplateParentElement(firstSel);
              elType.ElSSchema = TtaGetSSchema ("HTML", doc);
              elType.ElTypeNum = eType;
              if (parent)
                {
                  parentType = TtaGetElementType(parent);
                  if (parentType.ElSSchema == templateSSchema &&
                      parentType.ElTypeNum==Template_EL_bag)
                    {
                      if (!Template_CanInsertElementInBagElement(doc, elType, parent))
                        {
                          TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_NOT_ALLOWED);
                          return FALSE;
                        }
                    }
                }
            }
#endif /* TEMPLATES */
          
          /* register this element in the editing history */
          elType = TtaGetElementType (firstSel);
          if (eSchema)
            newType.ElSSchema = eSchema;
          else
            newType.ElSSchema = elType.ElSSchema;
          newType.ElTypeNum = 0;
          attrType.AttrSSchema = elType.ElSSchema;
          attrType.AttrTypeNum = aType;
          name = TtaGetSSchemaName (elType.ElSSchema);
          el = NULL;
          parse = (aType == HTML_ATTR_Style_);
          // check if it's within a style element
          parentType.ElSSchema = elType.ElSSchema;
          if (!strcmp (name, "HTML"))
            {
              parentType.ElTypeNum = HTML_EL_STYLE_;
              el = TtaGetTypedAncestor (firstSel, parentType);
              newType.ElTypeNum = eType;
            }
#ifdef _SVG
          else if (!strcmp (name, "SVG"))
            {
              parentType.ElTypeNum = SVG_EL_style__;
              el = TtaGetTypedAncestor (firstSel, parentType);
              if (eType == HTML_EL_Anchor)
                newType.ElTypeNum = SVG_EL_a;
              else
                newType.ElTypeNum = SVG_EL_tspan;
              if (aType == HTML_ATTR_HREF_)
                attrType.AttrTypeNum = SVG_ATTR_xlink_href;
              else if (aType == HTML_ATTR_Class)
                attrType.AttrTypeNum = SVG_ATTR_class;
              else if (aType == HTML_ATTR_Style_)
                attrType.AttrTypeNum = SVG_ATTR_style_;
            }
#endif /* _SVG */
          else if (!strcmp (name, "MathML"))
            {
              if (aType == HTML_ATTR_Class)
                attrType.AttrTypeNum = MathML_ATTR_class;
              else if (aType == HTML_ATTR_Style_)
                attrType.AttrTypeNum = MathML_ATTR_style_;
           }

          if (el && parse)
            TtaPasteFromBuffer ((unsigned char*)data, strlen(data),
                                TtaGetDefaultCharset ());
          else if (!strcmp(name, "HTML"))
            {
              // check if the selection is within the head
              parentType.ElTypeNum = HTML_EL_HEAD;
              el = TtaGetTypedAncestor (firstSel, parentType);
#ifdef TEMPLATES
              if(el)
                {
                  // check if selection is within the xt:head
                  parentType.ElSSchema = templateSSchema;
                  parentType.ElTypeNum = Template_EL_head;
                  if(TtaGetTypedAncestor (firstSel, parentType)!=NULL)
                    el = NULL;
                }
#endif /* TEMPLATES */
            }
          
          if (el == NULL)
            {
              open = TtaHasUndoSequence (doc);
              if (!open)
                TtaOpenUndoSequence (doc, firstSel, lastSel, firstchar, lastchar);
              /* hide the selection before modifications */
              TtaClearViewSelections ();
              /* Need to force a redisplay */
              dispMode = TtaGetDisplayMode (doc);
              if (dispMode == DisplayImmediately)
                TtaSetDisplayMode (doc, DeferredDisplay);
              in_line = NULL;
              el = firstSel;
              parent = NULL;
              // don't insert inside the current selection by default
              inside = FALSE;
              // check the last selected element first
              elType = TtaGetElementType (lastSel);
              name = TtaGetSSchemaName (elType.ElSSchema);
              lg =  TtaGetElementVolume (lastSel);
              lastChanged = FALSE;
              selected = firstSel;
              selpos = TtaIsSelectionEmpty ();
              isPict = elType.ElTypeNum == HTML_EL_PICTURE_UNIT;
              if ((selpos || firstSel == lastSel) && isPict)
                {
                  // this is a picture element
                  // attach the attribute to the parent element
                  parent = TtaGetParent (lastSel);
                  el = parent;
                  elType = TtaGetElementType (el);
                  lastSel = el;
                }
              else if ((selpos || firstSel == lastSel) &&
                  (aType == HTML_ATTR_ID || aType == HTML_ATTR_Language ||
                   aType == HTML_ATTR_Class || aType == HTML_ATTR_Style_) &&
                  !strcmp(name, "HTML") &&
                  (elType.ElTypeNum == HTML_EL_Basic_Elem ||
                   elType.ElTypeNum == HTML_EL_Element))
                {
                  // this is a temporary element
                  // attach the attribute to the parent element
                  parent = TtaGetParent (lastSel);
                  el = parent;
                  elType = TtaGetElementType (el);
                  lastSel = el;
                }
              else if ((selpos || firstSel == lastSel) &&
                       !strcmp(name, "HTML") &&
                       elType.ElTypeNum == HTML_EL_Element)
                {
                  // this is a temporary element
                  // generate an enclosing paragraph
                  elType.ElTypeNum = HTML_EL_Paragraph;
                  el = TtaNewElement (doc, elType);
                  TtaInsertFirstChild (&el, firstSel, doc);
                  TtaRegisterElementCreate (el, doc);
                  parent = el;
                  inside = TRUE;
                  done = TRUE; // action done
                }
              else if (aType == 0 &&
                       !strcmp(name, "HTML") &&
                       !IsCharacterLevelElement (el))
                {
                  // a block level is selected and a in-line element is required
                  // normalize the selection
                  childType = TtaGetElementType (firstSel);
                  while (!strcmp(TtaGetSSchemaName (childType.ElSSchema), "HTML") &&
                         !IsCharacterLevelElement (firstSel))
                    {
                      child = GetNoTemplateChild (firstSel, TRUE);
                      childType = TtaGetElementType (child);
                      if (child)
                        {
                          firstSel = child;
                          parent = el;
                          el = firstSel;
                        }
                    }
                  elType = TtaGetElementType (lastSel);
                  while (!strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") &&
                         !IsCharacterLevelElement (lastSel))
                    {
                      // keep all children
                      child = TtaGetLastChild (lastSel);
                      elType = TtaGetElementType (child);
                      if (!strcmp(TtaGetSSchemaName (elType.ElSSchema), "Template"))
                        {
                          child = GetNoTemplateChild (child, FALSE);
                          elType = TtaGetElementType (child);
                        }
                      if (child)
                        {
                          lastSel = child;
                          lastChanged = TRUE;
                        }
                    }

                  if (firstSel == lastSel &&
                      (childType.ElTypeNum == HTML_EL_Paragraph ||
                       childType.ElTypeNum == HTML_EL_Pseudo_paragraph))
                    {
                      parent = el;
                      // select the block content
                      firstSel = TtaGetFirstChild (parent);
                      if (firstSel)
                        {
                          lastSel = TtaGetLastChild (parent);
                          el = firstSel;
                          elType = TtaGetElementType (el);
                          firstchar = 1;
                          i = TtaGetElementVolume (el)+1;
                        }
                      else
                        {
                          // empty paragraph
                          firstSel = lastSel = el;
                          firstchar = 1;
                          i = TtaGetElementVolume (el)+1;
                          inside = TRUE;
                        }
                    }
                }

              if (el && firstSel != lastSel &&
                  elType.ElTypeNum == HTML_EL_TEXT_UNIT &&
                  (lastchar == 0 || lastchar > lg))
                {
                  // the whole last element is included
                  enclose = TtaGetParent (lastSel);
                  if (!strcmp (name, "HTML") &&
                      IsCharacterLevelElement (enclose) &&
                      lastSel == TtaGetFirstChild (enclose) &&
                      lastSel == TtaGetLastChild (enclose))
                    {
                      lastSel = enclose;
                      lastChanged = TRUE;
                    }
                }
              else if (el && firstSel != lastSel &&
                       aType != HTML_ATTR_Style_ &&
                       elType.ElTypeNum == HTML_EL_TEXT_UNIT)
                {
                  enclose = TtaGetExactTypedAncestor (firstSel, newType);
                  if (enclose)
                    {
                      el = enclose;
                      firstSel = el;
                    }
                  enclose = TtaGetExactTypedAncestor (lastSel, newType);
                  if (enclose)
                    {
                      lastSel = enclose;
                      lastChanged = TRUE;
                    }
                }

              while (el)
                {
                  elType = TtaGetElementType (el);
                  skip = FALSE;
                  if (aType != HTML_ATTR_Style_ && aType != HTML_ATTR_Class
                      && aType != HTML_ATTR_ID)
                    {
                      // generate inline element
                      while (!strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") &&
                             !IsCharacterLevelElement (el))
                        {
                          // move down in block element
                          child = TtaGetFirstChild (el);
                          if (el == firstSel)
                            firstSel = child;
                          if (el == lastSel)
                            lastSel = child;
                          el = child;
                          elType = TtaGetElementType (el);
                        }
                    }
                  // remove the current inline element to extend it
                  name = TtaGetSSchemaName (elType.ElSSchema);
                  if (!strcmp (name, "HTML"))
                    removed = (aType != HTML_ATTR_Style_  &&
                               elType.ElTypeNum == newType.ElTypeNum &&
                               elType.ElSSchema == newType.ElSSchema);
#ifdef _SVG
                  else if (!strcmp (name, "SVG"))
                    removed = (aType != SVG_ATTR_style_  &&
                               elType.ElTypeNum == newType.ElTypeNum &&
                               elType.ElSSchema == newType.ElSSchema);
#endif /* _SVG */
                  if (removed)
                    {
                      attr = NULL;
                      TtaNextAttribute (el, &attr);
                      if (attr)
                        {
                          // don't remove an element with attributes
                          TtaChangeTypeOfElement (el, doc, HTML_EL_Span);
                          TtaRegisterElementTypeChange (el, elType.ElTypeNum, doc);
                          removed = FALSE;
                          done = TRUE; // action done
                        }
                    }
                  lg =  TtaGetElementVolume (el);
                  split = ((el == firstSel || el == lastSel) &&
                           !strcmp(name, "HTML") &&
                           elType.ElTypeNum == HTML_EL_TEXT_UNIT &&
                           ((firstchar > 1 && firstchar <= lg) ||
                            (i > 0 && i <= lg && i >= firstchar)));
                  // check the next selected element
                  if (el == lastSel)
                    // only one element selected
                    next = NULL;
                  else
                    {
                      if (el)
                        next = el;
                      if (next != selected && TtaIsAncestor (next, selected))
                        {
                          // get next sibling
                          child = next;
                          TtaNextSibling (&next);
                          while (selected && next == NULL)
                            {
                              next = TtaGetParent (child);
                              child = next;
                              if (next != selected)
                                TtaNextSibling (&next);
                              else
                                {
                                  TtaGiveNextSelectedElement (doc, &next, &j, &lastchar);
                                  selected = next;
                                }
                            }
                        }
                      else
                        {
                          // next in the selection
                          TtaGiveNextSelectedElement (doc, &next, &j, &lastchar);
                          selected = next;
                        }
                      if (lastChanged && TtaIsAncestor (next, lastSel))
                        next = lastSel;
                      // adjust the first selection
                      if (el == firstSel && TtaIsLeaf (elType))
                        {
                          parent = TtaGetParent (el);
                          if (!strcmp (name, "HTML") &&
                              IsCharacterLevelElement (parent) &&
                              el == TtaGetFirstChild (parent) &&
                              el == TtaGetLastChild (parent) &&
                              firstchar <= 1)
                            {
                              el = parent;
                              firstSel = el;
                              elType = TtaGetElementType (parent);
                              parent = NULL; // keep the whole parent
                            }
                        }
                    }

                  if (!TtaIsReadOnly (el))
                    {
                      /* the selected element is not read-only */
                      if (!split && el == firstSel && firstSel != lastSel &&
                          elType.ElTypeNum == HTML_EL_TEXT_UNIT &&
                          firstchar <= 1)
                        {
                          // the whole first element is included
                          parent = TtaGetParent (el);
                          if (IsCharacterLevelElement (parent))
                            {
                              // the parent is a in-line element
                              if (!strcmp (name, "HTML") &&
                                  el == TtaGetFirstChild (parent) &&
                                  el == TtaGetLastChild (parent))
                                {
                                  // include the parent in-line
                                  el = parent;
                                  firstSel = el;
                                  elType = TtaGetElementType (parent);
                                  parent = NULL; // keep the whole parent
                                }
                            }
                        }

                      if (!strcmp(name, "HTML"))
                        {
                          if (!selpos && firstSel == lastSel &&
                              (aType == HTML_ATTR_ID || aType == HTML_ATTR_Language ||
                               aType == HTML_ATTR_Class || aType == HTML_ATTR_Style_))
                            // accept to set the attribute to inline element
                            charlevel = TtaIsLeaf (elType);
                          else
                            charlevel = IsCharacterLevelElement (el);
                        }
                      else
                        charlevel = TtaIsLeaf (elType);
                      if (!charlevel)
                        // the element is probably a parent element
                        parent = el;

                      if (split && in_line == NULL)
                        doit = TRUE;
                      else if (elType.ElSSchema == newType.ElSSchema &&
                               elType.ElTypeNum == newType.ElTypeNum &&
                               in_line == NULL && removed)
                        {
                          // we already have a such element
                          doit = FALSE;
                          charlevel = FALSE;
                          skip = TRUE;
                        }
                      else if (in_line == NULL && !strcmp(name, "HTML"))
                        // if a strong, em is requested it should be created
                        doit = (charlevel || aType == 0);
                      else
                        doit = FALSE;

                      // generate a first in_line
                      if (doit && newType.ElTypeNum != 0)
                        /* create a in_line element */
                        in_line = TtaNewElement (doc, newType);
                      
                      sibling = el;
                      before = FALSE;
                      if (!skip)
                        {
                          if (split)
                            {
                              /* exclude trailing spaces from the in_line */
                              if (lg > 0)
                                {
                                  buffer = (CHAR_T *)TtaGetMemory ((lg+1) * sizeof(CHAR_T) );
                                  TtaGiveBufferContent (el, buffer, lg, &lang);
                                  if (i == 0)
                                    i = lg + 1;
                                  if (el == lastSel && i <= lg)
                                    {
                                      min = firstchar;
                                      if (selpos)
                                        i++;
                                      else
                                        {
                                          // not a position
                                          while (i > min && buffer[i - 2] == SPACE)
                                            i--;
                                        }
                                      if (i > 1)
                                        {
                                          TtaRegisterElementReplace (el, doc);
                                          TtaSplitText (el, i, doc);
                                          sibling = el;
                                          TtaNextSibling (&sibling);
                                          TtaRegisterElementCreate (sibling, doc);
                                          // the first piece of el element should be moved
                                          before = TRUE;
                                        }
                                      else
                                        {
                                          // don't manage this element
                                          charlevel = FALSE;
                                          if (el == lastSel)
                                            lastSel = in_line;
                                          el = NULL;
                                        }
                                    }
                                  if (selpos)
                                    {
                                      // empty selection -> generate a text
                                      elType.ElTypeNum = HTML_EL_TEXT_UNIT;
                                      child = TtaNewElement (doc, elType);
                                      TtaInsertFirstChild (&child, in_line, doc);
                                      firstSel = child;
                                      lastSel = child;
                                    }
                                  else if (el == firstSel && firstchar > 1)
                                    {
                                      max = i;
                                      if (firstSel != lastSel || i >= firstchar)
                                        {
                                          // not a position
                                          while (firstchar <= max &&
                                                 buffer[firstchar - 1] == SPACE)
                                            firstchar++;
                                        }
                                      if (firstchar <= i && in_line)
                                        /* split the first string */
                                        {
                                          // prepare the future selection
                                          if (el == firstSel && in_line)
                                            firstSel = in_line;
                                          if (el == lastSel && in_line)
                                            lastSel = in_line;
                                          TtaRegisterElementReplace (el, doc);
                                          TtaSplitText (el, firstchar, doc);
                                          TtaNextSibling (&el);
                                          TtaRemoveTree (el, doc);
                                          if (doit)
                                            // insert into the new created in_line
                                            TtaInsertFirstChild (&el, in_line, doc);
                                          else
                                            {
                                              // add at the end of the in_line
                                              sibling = TtaGetLastChild (in_line);
                                              TtaInsertSibling (el, sibling, FALSE, doc);
                                            }
                                        }
                                      else
                                        {
                                          // don't manage this element
                                          charlevel = FALSE;
                                          if (el == firstSel && next)
                                            {
                                              // the selection starts with the next element
                                              firstSel = next;
                                              TtaRemoveTree (in_line, doc);
                                              in_line = NULL;
                                              parent = NULL;
                                            }
                                          else
                                            {
                                              // become an empty selection -> generate a text
                                              elType.ElTypeNum = HTML_EL_TEXT_UNIT;
                                              child = TtaNewElement (doc, elType);
                                              TtaInsertFirstChild (&child, in_line, doc);
                                              firstSel = child;
                                              lastSel = child;
                                            }
                                          el = in_line;
                                        }
                                    }
                                  else if (before && in_line)
                                    {
                                      // prepare the future selection
                                      if (el == firstSel)
                                        firstSel = in_line;
                                      if (el == lastSel)
                                        lastSel = in_line;
                                      TtaRemoveTree (el, doc);
                                      if (doit)
                                        // insert into the new created in_line
                                        TtaInsertFirstChild (&el, in_line, doc);
                                      else
                                        {
                                          // add at the end of the in_line
                                          sibling = TtaGetLastChild (in_line);
                                          TtaInsertSibling (el, sibling, FALSE, doc);
                                        }
                                    }
                                  TtaFreeMemory (buffer);
                                }
                            }
                          else if (in_line)
                            {
                              if (doit)
                                {
                                  // should we change the selection
                                  if (el == lastSel)
                                    lastSel = in_line;
                                  if (el == firstSel)
                                    firstSel = in_line;
                                }
                              if (doit && (selpos || inside))
                                {
                                  // empty selection or block selection
                                  // -> generate a text within the new in-line element
                                  if (charlevel)
                                    sibling = el;
                                  else
                                    sibling = NULL;
                                  if (isPict)
                                    before = (firstchar == 0);
                                  else if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
                                    before = (firstchar <= 1);
                                  elType.ElTypeNum = HTML_EL_TEXT_UNIT;
                                  child = TtaNewElement (doc, elType);
                                  TtaInsertFirstChild (&child, in_line, doc);
                                  firstSel = child;
                                  lastSel = child;
                                }
                              else if (el == parent || removed)
                                {
                                  // add children into the new in_line
                                  enclose = el;
                                  child = TtaGetFirstChild (el);
                                  // is there a sibling element of this child?
                                  el = child;
                                  last = TtaGetLastChild(in_line); // last inserted child
                                  while (child)
                                    {
                                      TtaNextSibling (&el);
                                      if (el)
                                        next = el;
                                      TtaRegisterElementDelete (child, doc);
                                      TtaRemoveTree (child, doc);
                                      if (last)
                                        TtaInsertSibling (child, last, FALSE, doc);
                                      else
                                        TtaInsertFirstChild (&child, in_line, doc);
                                      last = child;
                                      // get next sibling of moved 
                                      child = el;
                                    }
                                  el = NULL;
                                  if (removed)
                                    {
                                      // remove the enclose element
                                      parent = TtaGetParent (enclose);
                                      sibling = enclose;
                                      TtaPreviousSibling (&sibling);
                                      TtaRegisterElementDelete (enclose, doc);
                                      TtaDeleteTree (enclose, doc);
                                    }
                                  else
                                    // in_line will be inserted into the parent element
                                    sibling = NULL;
                                }
                              else
                                {
                                  // add the element into the new in_line
                                  if (doit)
                                    {
                                      // the in_line element must be inserted
                                      if (parent == NULL)
                                        parent = TtaGetParent (el);
                                      // get the previous sibling
                                      sibling = el;
                                      TtaPreviousSibling (&sibling);
                                       before = FALSE;
                                    }
                                  TtaRegisterElementDelete (el, doc);
                                  TtaRemoveTree (el, doc);
                                  child = TtaGetLastChild (in_line);
                                  if (child)
                                    TtaInsertSibling (el, in_line, FALSE, doc);
                                  else
                                    TtaInsertFirstChild (&el, in_line, doc);
                                  if (!doit)
                                    TtaRegisterElementCreate (el, doc);
                                }
                            }

                          if (doit && in_line)
                            {
                              if (sibling)
                                TtaInsertSibling (in_line, sibling, before, doc);
                              else if (parent)
                                TtaInsertFirstChild (&in_line, parent, doc);
                              /* generate the attribute */
                              if (attrType.AttrTypeNum != 0)
                                {
                                  if (aType == HTML_ATTR_ID)
                                    /* generate id and/or name, but do not register
                                       these attributes in the undo queue, as the
                                       in_line element itself will be registered
                                       with its attributes */
                                    CreateTargetAnchor (doc, in_line, FALSE, FALSE, FALSE);
                                  else if ((aType == HTML_ATTR_Style_ ||
                                            aType == HTML_ATTR_Class) &&
                                           data &&
                                           data[0] != EOS)
                                    {
                                      newAttr = TtaNewAttribute (attrType);
                                      TtaAttachAttribute (in_line, newAttr, doc);
                                      TtaSetAttributeText (newAttr, data, in_line, doc);
                                    }
                                }
                              TtaRegisterElementCreate (in_line, doc);
                              done = TRUE; // action done
                              if (parse && data && data[0] != EOS)
                                {
                                  // apply CSS properties
                                  tmpdata = TtaStrdup(data);
                                  ParseHTMLSpecificStyle (in_line, tmpdata, doc, 1000, FALSE);
                                  TtaFreeMemory(tmpdata);
                                }
                            }
                          else if (in_line && charlevel)
                            {
                              if (el)
                                {
                                  TtaRegisterElementDelete (el, doc);
                                  TtaRemoveTree (el, doc);
                                  lastChild = TtaGetLastChild (in_line);
                                  TtaInsertSibling (el, lastChild, FALSE, doc);
                                  TtaRegisterElementCreate (el, doc);
                                  done = TRUE; // action done
                                  if (el == lastSel)
                                    lastSel = in_line;
                                }
                            }
                          else
                            {
                              if (charlevel)
                                {
                                  in_line = TtaGetParent(el);
                                  // prepare the future selection
                                  if (el == firstSel)
                                    firstSel = in_line;
                                  if (el == lastSel)
                                    lastSel = in_line;
                                  // apply the style to the enclosing element
                                  el = in_line;
                                }
                              else if ((eType != HTML_EL_Anchor || aType == HTML_ATTR_HREF_) &&
                                       eType != HTML_EL_Span)
                                el = GenerateInlinechildren (el, newType, doc);

                              if (el && attrType.AttrTypeNum != 0)
                                {
                                  // generate an attribute to element or its children
                                  child = last = el;
                                  elType = TtaGetElementType (el);
                                  if (TtaHasHiddenException (elType))
                                    {
                                      // don't generate a style to hidden elements
                                      last = TtaGetLastChild (el);
                                      child = TtaGetFirstChild (el);
                                    }
                                  while (child)
                                    {
                                      elType = TtaGetElementType (child);
                                      if (TtaHasHiddenException (elType))
                                        // skip hidden elements
                                        child = TtaGetFirstChild (child);
                                      if (aType == HTML_ATTR_ID)
                                        // generate id and/or name
                                        CreateTargetAnchor (doc, child, FALSE, FALSE, TRUE);
                                      else
                                        {
                                          newAttr = TtaGetAttribute (child, attrType);
                                          if (newAttr == NULL)
                                            {
                                              newAttr = TtaNewAttribute (attrType);
                                              TtaAttachAttribute (child, newAttr, doc);
                                              TtaSetAttributeText (newAttr, data, child, doc);
                                              TtaRegisterAttributeCreate (newAttr, child, doc);
                                              done = TRUE; // action done
                                              if (parse)
                                                {
                                                  // apply CSS properties
                                                  tmpdata = TtaStrdup(data);
                                                  ParseHTMLSpecificStyle (child, tmpdata, doc, 1000, FALSE);
                                                  TtaFreeMemory(tmpdata);
                                                }
                                            }
                                          else
                                            {
                                              done = FALSE;
                                              if (parse && replace)
                                                {
                                                  // remove style rules
                                                  lg = TtaGetTextAttributeLength (newAttr) + 1;
                                                  name = (char *)TtaGetMemory (lg);
                                                  TtaGiveTextAttributeValue (newAttr, name, &lg);
                                                  ParseHTMLSpecificStyle (child, name, doc, 2000, TRUE);
                                                  TtaFreeMemory (name);
                                                  // CSS properties should be ended by ;
                                                  name = TtaStrdup (data);
                                                }
                                              else if (!parse)
                                                {
                                                  // several class names
                                                  lg = TtaGetTextAttributeLength (newAttr);
                                                  if (lg && !replace)
                                                    {
                                                      int     ic, id, ld;
                                                      ld = strlen (data);
                                                      name = (char *)TtaGetMemory (lg + ld + 3);
                                                      TtaGiveTextAttributeValue (newAttr, name, &lg);
                                                      // check if the name is already there
                                                      ic = id = 0;
                                                      while (!done && ic <= lg && id <= ld)
                                                        {
                                                          if (data[id] == EOS &&
                                                              (name[ic] == SPACE || name[ic] == EOS))
                                                            done = TRUE;
                                                          else if (name[ic] != data[id])
                                                            {
                                                              while (name[ic] != SPACE &&
                                                                     name[ic] != EOS)
                                                                ic++;
                                                              id = 0;
                                                              ic++;
                                                            }
                                                          else
                                                            {
                                                              ic++;
                                                              id++;
                                                            }
                                                        }
                                                      if (!done)
                                                        {
                                                          strcat (name, " ");
                                                          strcat (name, data);
                                                        }
                                                    }
                                                  else
                                                    name = TtaStrdup (data);
                                                }
                                              else
                                                name = TtaStrdup (data);
                                              if (!done)
                                                {
                                                  if (name[0] == EOS)
                                                    {
                                                      Element firstC, lastC;
                                                      // the attribute is now empty
                                                      TtaRegisterAttributeDelete (newAttr, child, doc);
                                                      TtaRemoveAttribute (child, newAttr, doc);
                                                      DeleteSpanIfNoAttr (child, doc, &firstC, &lastC);
                                                    }
                                                  else
                                                    {
                                                      TtaRegisterAttributeReplace (newAttr, child, doc);
                                                      if (replace)
                                                        {
                                                          TtaSetAttributeText (newAttr, name, child, doc);
                                                          if (parse)
                                                            // apply CSS properties
                                                            ParseHTMLSpecificStyle (child, name, doc, 1000, FALSE);
                                                        }
                                                      else
                                                        UpdateAttribute (newAttr, name, child, doc);
                                                    }
                                                  done = TRUE; // action done
                                                }
                                              else if (parse)
                                                // apply CSS properties
                                                ParseHTMLSpecificStyle (child, name, doc, 1000, FALSE);
                                              TtaFreeMemory (name);
                                            }
                                        }
                                      if (child == last)
                                        child = NULL;
                                      else
                                        TtaNextSibling (&child);
                                    }
                                }
                            }

                          // check the next element
                          if (in_line)
                            {
                              sibling = in_line;
                              TtaNextSibling (&sibling);
                              if (sibling != next)
                                {
                                // cannot extent the in_line to the next element
                                in_line = NULL;
                                parent = NULL;
                                }
                            }
                        }
                    }
                  el = next;
                  firstchar = j;
                  i = lastchar;
                }
              TtaSetDisplayMode (doc, dispMode);
              TtaSelectElement (doc, firstSel);
              if (lastSel != firstSel)
                TtaExtendSelection (doc, lastSel, TtaGetElementVolume (lastSel) + 1);
              /* mark the document as modified */
              TtaSetDocumentModified (doc);
              if (!open)
                TtaCloseUndoSequence (doc);
              // As the selection can be changed, update the Attribute list
              TtaUpdateAttrMenu (doc);
            }
        }
    }
  return done;
  //if (!done)
  //  TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_NOT_ALLOWED);
}


/*----------------------------------------------------------------------
  SetREFattribute
  Set the HREF or CITE attribute of the element to the concatenation of
  targetURL and targetName expressed in dialog charset.
  ----------------------------------------------------------------------*/
void SetREFattribute (Element element, Document doc, char *targetURL,
                      char *targetName)
{
  ElementType	       elType, piType;
  AttributeType       attrType;
  Attribute           attr;
  Element             piEl;
  LoadedImageDesc   *desc;
  char               *value, *base, *s, *utf8val;
  char                tempURL[MAX_LENGTH];
  char                resname[MAX_LENGTH];
  char                buffer[MAX_LENGTH];
  int                 length, piNum;
  ThotBool            new_, oldStructureChecking;
  ThotBool            isHTML, isSVG, isLib;

  if (element == NULL)
    return;
  attr = 0;
  new_ = FALSE;  
  if (AttrHREFundoable)
    TtaOpenUndoSequence (doc, element, element, 0, 0);

  elType = TtaGetElementType (element);
  attrType.AttrSSchema = elType.ElSSchema;
  s = TtaGetSSchemaName (elType.ElSSchema);
  isHTML = !strcmp (s, "HTML");
  isSVG = !strcmp (s, "SVG");
  isLib = !strcmp (s, "Template");

  if (!LinkAsXmlCSS)
    /* It isn't a link to an xml stylesheet */
    {
      if (isHTML)
        {
          if (elType.ElTypeNum == HTML_EL_Block_Quote ||
              elType.ElTypeNum == HTML_EL_Quotation ||
              elType.ElTypeNum == HTML_EL_INS ||
              elType.ElTypeNum == HTML_EL_DEL)
            attrType.AttrTypeNum = HTML_ATTR_cite;
          else if (elType.ElTypeNum == HTML_EL_FRAME)
            attrType.AttrTypeNum = HTML_ATTR_FrameSrc;
          else if (elType.ElTypeNum == HTML_EL_SCRIPT_)
            attrType.AttrTypeNum = HTML_ATTR_script_src;
          else
            {
            /* The anchor element must have an HREF attribute */
            /* create an attribute PseudoClass = link */
            attrType.AttrTypeNum = HTML_ATTR_PseudoClass;
            attr = TtaGetAttribute (element, attrType);
            if (attr == NULL)
              {
                attr = TtaNewAttribute (attrType);
                TtaAttachAttribute (element, attr, doc);
              }
            TtaSetAttributeText (attr, "link", element, doc);
            attrType.AttrTypeNum = HTML_ATTR_HREF_;
            }
        }
#ifdef _SVG
      else if (isSVG)
          attrType.AttrTypeNum = SVG_ATTR_xlink_href;
#endif /* _SVG */
#ifdef TEMPLATES
      else if (isLib)
          attrType.AttrTypeNum = Template_ATTR_src;
#endif /* TEMPLATES */
       else
        {
          /* the origin of the link is not a HTML element */
          /* create a XLink link */
          attrType.AttrSSchema = TtaGetSSchema ("XLink", doc);
          if (!attrType.AttrSSchema)
            attrType.AttrSSchema = TtaNewNature (doc, TtaGetDocumentSSchema (doc),
                                                 NULL, "XLink", "XLinkP");
          attrType.AttrTypeNum = XLink_ATTR_href_;
          if (TtaIsLeaf (elType))
            element = TtaGetParent (element);
          if (strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML"))
            /* it's not a MathML element (the MathML 2.0 DTD does not use
               the xlink:type attribute) */
            /* create a xlink:type attribute with value "simple" */
            SetXLinkTypeSimple (element, doc, AttrHREFundoable);
        }
      attr = TtaGetAttribute (element, attrType);
      if (attr == NULL)
        {
          /* create an attribute HREF for the element */
          attr = TtaNewAttribute (attrType);
          /* this element may be in a different namespace, so don't check
             validity */
          oldStructureChecking = TtaGetStructureChecking (doc);
          TtaSetStructureChecking (FALSE, doc);
          TtaAttachAttribute (element, attr, doc);
          TtaSetStructureChecking (oldStructureChecking, doc);
          if (!isHTML && !isSVG && !isLib)
            {
              /* Attach the XLink namespace declaration */
              TtaSetUriSSchema (attrType.AttrSSchema, XLink_URI);
              TtaSetANamespaceDeclaration (doc, element, XLink_PREFIX, XLink_URI);
            }
          new_ = TRUE;
        }
      else
        {
          new_ = FALSE;
          if (AttrHREFundoable)
            TtaRegisterAttributeReplace (attr, element, doc);
        }
    }

  /* build the complete target URL */
  if (targetURL && targetURL[0] != EOS && strcmp (targetURL, DocumentURLs[doc]))
    {
      if (targetURL[0] != '.' &&
#ifdef _WINDOWS
          targetURL[1] == ':' &&
#else /* _WINDOWS */
          targetURL[0] == '/' && TtaFileExist (targetURL) &&
#endif /* _WINDOWS */
          IsHTTPPath (DocumentURLs[doc]))
        {
          /* link a local resource to a remote document */
          /* copy the file into the temporary directory of the document */
          TtaExtractName (targetURL, tempURL, resname);
          NormalizeURL (resname, doc, tempURL, resname, NULL);
          AddLocalResource (targetURL, resname, tempURL, doc,  &desc,
                            &LoadedResources, FALSE);
        }
      else
        {
         if (!IsHTTPPath (DocumentURLs[doc]) &&
              strncmp (targetURL, "http", 4))
            {
              // create the file if needed
              TtaExtractName (targetURL, tempURL, resname);
              NormalizeURL (resname, doc, tempURL, resname, NULL);
              if ((LinkAsCSS || LinkAsXmlCSS) && !TtaFileExist (tempURL))
                {
#ifdef _WX
                  wxString tmpFile = TtaConvMessageToWX(tempURL);
                  wxFile (tmpFile, wxFile::write);
#endif /* _WX */
                }
            }
         strcpy (tempURL, targetURL);
        }
    }
  else
    tempURL[0] = EOS;
  if (targetName != NULL)
    {
      strcat (tempURL, "#");
      strcat (tempURL, targetName);
    }

  if (LinkAsXmlCSS)
    {
      if ((tempURL[0] != EOS) && (IsCSSName (targetURL)))
        {
          /* set the relative value or URL in PI */
          base = GetBaseURL (doc);
          value = MakeRelativeURL (tempURL, base);
          LinkAsXmlCSS = FALSE;

          /* Load the CSS style sheet */
          if (elType.ElTypeNum == 1)
            {
              piEl = TtaGetParent (element);
              piEl = TtaGetParent (piEl);
              piType = TtaGetElementType (piEl);
              s = TtaGetSSchemaName (piType.ElSSchema);
              if (strcmp (s, "MathML") == 0)
                piNum = MathML_EL_XMLPI;
              else if (strcmp (s, "SVG") == 0)
                piNum = SVG_EL_XMLPI;
              else
                piNum = XML_EL_xmlpi;
              if (piType.ElTypeNum == piNum)
                {
                  /* The CSS is linked with the PI element */
                  /* don't manage a document used by make book */
                  if (DocumentMeta[doc] == NULL ||
                      DocumentMeta[doc]->method != CE_MAKEBOOK)
                    LoadStyleSheet (targetURL, doc, piEl, NULL, NULL, CSS_ALL, FALSE);
                  /* We use the Latin_Script language to avoid the spell_chekcer */
                  /* to check this element */
                  strcpy (buffer, "xml-stylesheet type=\"text/css\" href=\"");
                  if (*value == EOS)
                    strcat (buffer, "./");
                  else
                    {
                      utf8val = (char *)TtaConvertByteToMbs ((unsigned char *)value,
                                                             TtaGetDefaultCharset ());
                      strcat (buffer, utf8val);
                      TtaFreeMemory (utf8val);
                    }
                  strcat (buffer, "\"");
                  TtaSetTextContent (element, (unsigned char *)buffer, Latin_Script, doc);
                  
                  /* Query update Style List tool panel. */
                  UpdateStyleList (doc, 1);
                }
            }
        }
    }
  else
    {
      if (tempURL[0] == EOS)
        {
          /* get a buffer for the attribute value */
          length = TtaGetTextAttributeLength (attr);
          if (length == 0)
            {
              /* no given value */
              // link to the document itself
              TtaExtractName (targetURL, buffer, tempURL);
              TtaSetAttributeText (attr, tempURL, element, doc);
            }
        }
      else
        {
          /* set the relative value or URL in attribute HREF */
          base = GetBaseURL (doc);
          value = MakeRelativeURL (tempURL, base);
          TtaFreeMemory (base);
          if (*value == EOS)
            TtaSetAttributeText (attr, "./", element, doc);
          else
            {
              utf8val = (char *)TtaConvertByteToMbs ((unsigned char *)value,
                                                     TtaGetDefaultCharset ());
              TtaSetAttributeText (attr, utf8val, element, doc);
              TtaFreeMemory (utf8val);
            }
          TtaFreeMemory (value);
        }

      /* register the new value of the HREF attribute in the undo queue */
      if (AttrHREFundoable && new_)
        TtaRegisterAttributeCreate (attr, element, doc);
    }

  /* is it a html link to a CSS file? */
  if (tempURL[0] != EOS)
    if (elType.ElTypeNum == HTML_EL_LINK && isHTML &&
        (LinkAsCSS || IsCSSName (targetURL)))
      {
        LinkAsCSS = FALSE;
        LoadStyleSheet (targetURL, doc, element, NULL, NULL, CSS_ALL, FALSE);
        attrType.AttrTypeNum = HTML_ATTR_REL;
        attr = TtaGetAttribute (element, attrType);
        if (attr == 0)
          {
            /* create an attribute HREF for the element */
            attr = TtaNewAttribute (attrType);
            TtaAttachAttribute (element, attr, doc);
            new_ = TRUE;
          }
        else
          {
            new_ = FALSE;
            if (AttrHREFundoable)
              TtaRegisterAttributeReplace (attr, element, doc);
          }
        TtaSetAttributeText (attr, "stylesheet", element, doc);
        if (AttrHREFundoable && new_)
          TtaRegisterAttributeCreate (attr, element, doc);

        attrType.AttrTypeNum = HTML_ATTR_Link_type;
        attr = TtaGetAttribute (element, attrType);
        if (attr == 0)
          {
            /* create an attribute HREF for the element */
            attr = TtaNewAttribute (attrType);
            TtaAttachAttribute (element, attr, doc);
            new_ = TRUE;
          }
        else
          {
            new_ = FALSE;
            if (AttrHREFundoable)
              TtaRegisterAttributeReplace (attr, element, doc);
          }
        TtaSetAttributeText (attr, "text/css", element, doc);	   
        if (AttrHREFundoable && new_)
          TtaRegisterAttributeCreate (attr, element, doc);
        
        /* Query update Style List tool panel. */
        UpdateStyleList (doc, 1);
      }
  if (AttrHREFundoable)
    {
      TtaCloseUndoSequence (doc);
      AttrHREFundoable = FALSE;
    }
  TtaSetDocumentModified (doc);
  DisplayUrlAnchor (element, doc);
  //TtaSetStatus (doc, 1, " ", NULL);
  // update the attributes panel
  TtaUpdateAttrMenu (doc);
}


/*----------------------------------------------------------------------
  ChangeTitle displays a form to change the TITLE for the document.
  ----------------------------------------------------------------------*/
void ChangeTitle (Document doc, View view)
{
  ElementType         elType;
  Element             el, child;
  Language            lang;
  int                 length;
  ThotBool            created;

  if (!TtaGetDocumentAccessMode (doc))
    /* the document is in ReadOnly mode */
    return;
  /* search the Title element */
  el = TtaGetRootElement (doc);
  elType.ElSSchema = TtaGetDocumentSSchema (doc);
  if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML"))
    /* it's a HTML document */
    {
      elType.ElTypeNum = HTML_EL_TITLE;
      el = TtaSearchTypedElement (elType, SearchForward, el);
    }
  else if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "SVG"))
    /* it's an SVG document */
    {
      child = TtaGetFirstChild (el);
      elType = TtaGetElementType (child);
      while (child && (elType.ElTypeNum != SVG_EL_title ||
		       strcmp (TtaGetSSchemaName (elType.ElSSchema), "SVG")))
	{
	  TtaNextSibling (&child);
	  elType = TtaGetElementType (child);
	}
      el = child;
    }
  else
    el = NULL;
  if (el)
    {
      child = GetNoTemplateChild (el, TRUE);
      if (child == NULL)
        {
          /* insert the text element */
          elType.ElTypeNum = HTML_EL_TEXT_UNIT;
          child = TtaNewElement (doc, elType);
          TtaInsertFirstChild  (&child, el, doc);
        }
      length = MAX_LENGTH;
      TtaGiveTextContent (child, (unsigned char *)Answer_text, &length, &lang);
      CurrentDocument = doc;
      created = CreateTitleDlgWX ( BaseDialog + TitleForm,
                                   TtaGetViewFrame (doc, view),
                                   Answer_text);
      if (created)
        {
          TtaSetDialoguePosition ();
          TtaShowDialogue (BaseDialog + TitleForm, FALSE, TRUE);
        }
    }   
}

/*----------------------------------------------------------------------
  SetNewTitle stores the new TITLE in the title element and updates
  the windows title.
  ----------------------------------------------------------------------*/
void SetNewTitle (Document doc)
{
  ElementType         elType;
  Element             el, child;
  unsigned char       *title;

  if (!TtaGetDocumentAccessMode (doc))
    /* the document is in ReadOnly mode */
    return;
  /* search the Title element */
  el = TtaGetRootElement (doc);
  elType.ElSSchema = TtaGetDocumentSSchema (doc);
  if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML"))
    {
      elType.ElTypeNum = HTML_EL_TITLE;
      el = TtaSearchTypedElement (elType, SearchForward, el);
    }
  else if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "SVG"))
    {
      child = TtaGetFirstChild (el);
      elType = TtaGetElementType (child);
      while (child && (elType.ElTypeNum != SVG_EL_title ||
		       strcmp (TtaGetSSchemaName (elType.ElSSchema), "SVG")))
	{
	  TtaNextSibling (&child);
	  elType = TtaGetElementType (child);
	}
      el = child;
    }
  else
    el = NULL;
  if (el)
    {
      child = GetNoTemplateChild (el, TRUE);
      if (child)
        {
          TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
          TtaRegisterElementReplace (el, doc);
#ifdef _WX
          /* with _WX returned text is UTF8 everytime ! */
          title = TtaConvertByteToMbs ((unsigned char *)Answer_text, UTF_8);
#else /* _WX */	  
          title = TtaConvertByteToMbs ((unsigned char *)Answer_text, ISO_8859_1);
#endif /* _WX */
          TtaSetTextContent (child, (unsigned char *)title,
                             TtaGetDefaultLanguage (), doc);
          TtaFreeMemory (title);
          TtaCloseUndoSequence (doc);
          TtaSetDocumentModified (doc);
          SetWindowTitle (doc, doc, 0);
          if (DocumentSource[doc])
            SetWindowTitle (doc, DocumentSource[doc], 0);
        }
    }
}

/*----------------------------------------------------------------------
  TitleModified
  The user has modified the contents of element TITLE. Update the    
  the Title field on top of the window.                           
  ----------------------------------------------------------------------*/
void TitleModified (NotifyOnTarget *event)
{
  UpdateTitle (event->element, event->document);
}


/*----------------------------------------------------------------------
  SelectDestination
  Select the destination of the el Anchor.
  The parameter clickFirst is TRUE when the user choose to click the
  target.
  ----------------------------------------------------------------------*/
void SelectDestination (Document doc, Element el, ThotBool withUndo,
                        ThotBool clickFirst)
{
  Element             targetEl;
  ElementType	       elType;
  Document            targetDoc;
  Attribute           attr;
  AttributeType       attrType;
  char               *buffer = NULL, *name;
  int                 length;
  ThotBool            isHTML, created = FALSE;

  if (clickFirst)
    {
      /* ask the user to select target document and target anchor */
      TtaUnmapDialogue (BaseDialog + AttrHREFForm);
      TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_SEL_TARGET), NULL);
      TtaClickElement (&targetDoc, &targetEl);
      if (targetDoc != 0)
        isHTML = !(strcmp (TtaGetSSchemaName (TtaGetDocumentSSchema (targetDoc)),
                           "HTML"));
      else
        isHTML = FALSE;
       
      if (targetDoc && targetEl && DocumentURLs[targetDoc])
        {
          if (isHTML)
            {
              /* get attrName of the enclosing end anchor */
              attr = GetNameAttr (targetDoc, targetEl);
              /* the document becomes the target doc */
              SetTargetContent (targetDoc, attr);
            }
          else
            SetTargetContent (targetDoc, NULL);
        }
      else
        {
          targetDoc = doc;
          SetTargetContent (0, NULL);
        }
    }
   
  AttrHREFelement = el;
  AttrHREFdocument = doc;
  AttrHREFundoable = withUndo;
  if (clickFirst && (doc != targetDoc || TargetName != NULL))
    {
      /* the user has clicked another document or a target element */
      /* create the attribute HREF or CITE */
      SetREFattribute (el, doc, TargetDocumentURL, TargetName);
      IsNewAnchor = FALSE;
      TtaDestroyDialogue (BaseDialog + AttrHREFForm);
      //#ifndef _MACOS
      TtaRaiseView (doc, 1);
      //#endif /* _MACOS */
    }
  else
    /* the user has clicked the same document: pop up a dialogue box
       to allow the user to type the target URI */
    {
      /* Dialogue form to insert HREF name */
      if (el)
        {
          if (!LinkAsXmlCSS)
            {
              /* If the anchor has an HREF attribute, put its value in the form */
              elType = TtaGetElementType (el);
              name = TtaGetSSchemaName (elType.ElSSchema);
              if (!strcmp (name, "HTML"))
                /* it's an HTML element */
                {
                  attrType.AttrSSchema = elType.ElSSchema;
                  /* search the HREF or CITE attribute */
                  if (elType.ElTypeNum == HTML_EL_Quotation ||
                      elType.ElTypeNum == HTML_EL_Block_Quote ||
                      elType.ElTypeNum == HTML_EL_INS ||
                      elType.ElTypeNum == HTML_EL_DEL)
                    attrType.AttrTypeNum = HTML_ATTR_cite;
                  else if (elType.ElTypeNum == HTML_EL_SCRIPT_)
                    attrType.AttrTypeNum = HTML_ATTR_script_src;
                  else 
                    attrType.AttrTypeNum = HTML_ATTR_HREF_;
                }
#ifdef _SVG
              else if (!strcmp (name, "SVG"))
                /* it's an SVG element */
                {
                  attrType.AttrSSchema = elType.ElSSchema;
                  attrType.AttrTypeNum = SVG_ATTR_xlink_href;
                }
#endif /* _SVG */
#ifdef TEMPLATES
              else if (!strcmp (name, "Template"))
                /* it's an SVG element */
                {
                  attrType.AttrSSchema = elType.ElSSchema;
                  attrType.AttrTypeNum = Template_ATTR_src;
                }
#endif /* TEMPLATES */
              else
                {
                  attrType.AttrSSchema = TtaGetSSchema ("XLink", doc);
                  attrType.AttrTypeNum = XLink_ATTR_href_;
                }
              attr = TtaGetAttribute (el, attrType);
              AttrHREFvalue[0] = EOS;
              if (attr != 0)
                {
                  /* get a buffer for the attribute value */
                  length = TtaGetTextAttributeLength (attr);
                  buffer = (char *)TtaGetMemory (length + 1);
                  /* copy the HREF attribute into the buffer */
                  TtaGiveTextAttributeValue (attr, buffer, &length);
                  strcpy (AttrHREFvalue, buffer);
                  TtaFreeMemory (buffer);
                }
            }
        }

      TtaExtractName (DocumentURLs[doc], DirectoryName, DocumentName);

#ifdef _WX
      if (LinkAsXmlCSS || LinkAsCSS)
        /* select a CSS file */
        created = CreateHRefDlgWX (BaseDialog + AttrHREFForm,
                                   TtaGetViewFrame (doc, 1), URL_list,
                                   AttrHREFvalue,
                                   doc, docCSS);
#ifdef TEMPLATES
      else if (LinkAsImport)
        /* select a Javascript file */
        created = CreateHRefDlgWX (BaseDialog + AttrHREFForm,
                                   TtaGetViewFrame (doc, 1), URL_list,
                                   AttrHREFvalue,
                                   doc, docTemplate);
#endif /* TEMPLATES */
      else if (LinkAsJavascript)
        /* select a Javascript file */
        created = CreateHRefDlgWX (BaseDialog + AttrHREFForm,
                                   TtaGetViewFrame (doc, 1), URL_list,
                                   AttrHREFvalue,
                                   doc, docJavascript);
      else
        {
#ifdef IV
          if (AttrHREFvalue[0] == EOS &&
              ((TargetDocumentURL && TargetDocumentURL[0] != EOS) ||
               (TargetName && TargetName[0] != EOS)))
            {
              // by default propose the last created target
              if (TargetDocumentURL && strcmp (TargetDocumentURL, DocumentURLs[doc]))
                strcpy (AttrHREFvalue, TargetDocumentURL);
              if (TargetName && TargetName[0] != EOS)
                {
                  strcat (AttrHREFvalue, "#");
                  strcat (AttrHREFvalue, TargetName);
                }
            }
#endif
          /* select any file */
          created = CreateHRefDlgWX (BaseDialog + AttrHREFForm,
                                     TtaGetViewFrame (doc, 1), URL_list,
                                     AttrHREFvalue,
                                     doc, docText);
        }
      if (created)
        {
          TtaShowDialogue (BaseDialog + AttrHREFForm, TRUE, TRUE);
        }
#endif /* _WX */
    }
}

/*----------------------------------------------------------------------
  GetNameAttr
  Return the NAME attribute of the enclosing Anchor   
  element or the ID attribute of (an ascendant of) the selected element
  or NULL.
  ----------------------------------------------------------------------*/
Attribute GetNameAttr (Document doc, Element selectedElement)
{
  Element             el;
  ElementType         elType;
  AttributeType       attrType;
  Attribute           attr;
  SSchema	       HTMLSSchema;

  attr = NULL;		/* no NAME attribute yet */
  if (selectedElement)
    {
      elType = TtaGetElementType (selectedElement);
      HTMLSSchema = TtaGetSSchema ("HTML", doc);
      attrType.AttrSSchema = HTMLSSchema;
      if (elType.ElSSchema == HTMLSSchema &&
          elType.ElTypeNum == HTML_EL_Anchor)
        el = selectedElement;
      else
        {
          elType.ElSSchema = HTMLSSchema;
          elType.ElTypeNum = HTML_EL_Anchor;
          el = TtaGetTypedAncestor (selectedElement, elType);
        }

      if (el)
        {
          /* the ascending Anchor element has been found */
          /* get the NAME attribute of element Anchor */
          attrType.AttrTypeNum = HTML_ATTR_NAME;
          attr = TtaGetAttribute (el, attrType);
          if (attr == NULL)
            {
              attrType.AttrTypeNum = HTML_ATTR_ID;
              attr = TtaGetAttribute (el, attrType);
            }
        }
      else
        {
          /* no ascending Anchor element */
          /* get the ID attribute of the selected element */
          attrType.AttrTypeNum = HTML_ATTR_ID;
          attr = TtaGetAttribute (selectedElement, attrType);
          if (!attr)
            {
              attrType.AttrSSchema = TtaGetSSchema ("MathML", doc);
              if (attrType.AttrSSchema)
                {
                  attrType.AttrTypeNum = MathML_ATTR_id;
                  attr = TtaGetAttribute (selectedElement, attrType);
                }
            }
#ifdef _SVG
          if (!attr)
            {
              attrType.AttrSSchema = TtaGetSSchema ("SVG", doc);
              if (attrType.AttrSSchema)
                {
                  attrType.AttrTypeNum = SVG_ATTR_id;
                  attr = TtaGetAttribute (selectedElement, attrType);
                }
            }
#endif /* _SVG */
        }
    }
  return (attr);
}

/*----------------------------------------------------------------------
  CreateTargetAnchor
  Create a NAME or ID attribute with a default value for element el.
  If the withUndo parameter is true, we'll register the undo sequence.
  If the forceID parameter is true, we'll always use an ID attribute,
  rather than a NAME one in some cases.
  If the generic parameter is true, the label is used to generate the
  new value.
  ----------------------------------------------------------------------*/
void CreateTargetAnchor (Document doc, Element el, ThotBool forceID,
                         ThotBool generic, ThotBool withUndo)
{
  AttributeType       attrType;
  Attribute           attr;
  ElementType         elType;
  Element             elText;
  SSchema	       HTMLSSchema;
  Language            lang;
  char               *text, *url;
  int                 length, i, space;
  ThotBool            found;
  ThotBool            withinHTML, new_;

  elType = TtaGetElementType (el);
  withinHTML = !strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML");

  /* get a NAME or ID attribute */
  HTMLSSchema = TtaGetSSchema ("HTML", doc);
  attrType.AttrSSchema = HTMLSSchema;
  if (withinHTML && (elType.ElTypeNum == HTML_EL_Anchor ||
                     elType.ElTypeNum == HTML_EL_MAP ||
                     elType.ElTypeNum == HTML_EL_map))
    {
      if (forceID || TtaGetDocumentProfile (doc) ==  L_Xhtml11)
        attrType.AttrTypeNum = HTML_ATTR_ID;
      else
        attrType.AttrTypeNum = HTML_ATTR_NAME;
    }
  else
    {
      if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "MathML") == 0)
        {
          attrType.AttrSSchema = elType.ElSSchema;
          attrType.AttrTypeNum = MathML_ATTR_id;
        }
      else
#ifdef _SVG
        if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "SVG") == 0)
          {
            attrType.AttrSSchema = elType.ElSSchema;
            attrType.AttrTypeNum = SVG_ATTR_id;
          }
        else
#endif /* _SVG */
          attrType.AttrTypeNum = HTML_ATTR_ID;
    }
  attr = TtaGetAttribute (el, attrType);

  if (attr == NULL)
    {
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (el, attr, doc);
      new_ = TRUE;
    }
  else
    {
      /* already done */
      SetTargetContent (doc, attr);
      return;
    }

  /* build a value for the new attribute */
  url = (char *)TtaGetMemory (MAX_LENGTH);
  if (withinHTML &&
      (elType.ElTypeNum == HTML_EL_MAP || elType.ElTypeNum == HTML_EL_map))
    /* mapxxx for a map element */
    strcpy (url, "map");
  else if (withinHTML && elType.ElTypeNum == HTML_EL_LINK)
    /* linkxxx for a link element */
    strcpy (url, "link");
  else if (generic)
    strcpy (url, TtaGetElementLabel (el));
  else
    /* get the content for other elements */
    {
      elType.ElTypeNum = HTML_EL_TEXT_UNIT;
      elText = TtaSearchTypedElement (elType, SearchInTree, el);
      if (elText != NULL)
        {
          /* first word longer than 3 characters */
          length = 50;
          TtaGiveTextContent (elText, (unsigned char *)url, &length, &lang);
          space = 0;
          i = 0;
          found = FALSE;
          url[length++] = EOS;
          while (!found && i < length)
            {
              if (url[i] == ' ' || url[i] == EOS)
                {
                  found = (i - space > 3 || (i != space && url[i] == EOS));
                  if (found)
                    {
                      /* url = the word */
                      if (i > space + 10)
                        /* limit the word length */
                        i = space + 10;
                      url[i] = EOS;
                      if (space != 0)
                        strcpy (url, &url[space]);
                    }
                  i++;
                  space = i;
                }
              else if (url[i] == '_' ||
                       (i-space > 0 && url[i] == '.') ||
                       (i-space > 0 && url[i] == '-') ||
                       ((unsigned int) url[i] >= 65 &&          /*  'A'  */
                        (unsigned int) url[i] <= 90) ||         /*  'Z'  */
                       ((unsigned int) url[i] >= 97 &&          /*  'a'  */
                        (unsigned int) url[i] <= 127) ||        /*  'z'  */
                       (i-space > 0 &&
                        (unsigned int) url[i] >= 48 &&          /*  '0'  */
                        (unsigned int) url[i] <= 57))           /*  '9'  */
                /* valid character for an ID */
                i++;
              else if (i > 2)
                url[i] = ' ';
              else
                /* invalid name for an ID */
                i = length;
            }

          if (!found)
            {
              /* label of the element */
              text = TtaGetElementLabel (el);
              strcpy (url, text);
            }
        }
      else
        {
          /* get the element's label if there is no text */
          text = TtaGetElementLabel (el);
          strcpy (url, text);
        }
    }
  /* copie the text into the NAME attribute */
  TtaSetAttributeText (attr, url, el, doc);
  TtaSetDocumentModified (doc);
  /* Check the attribute value to make sure that it's unique within */
  /* the document */
  MakeUniqueName (el, doc, TRUE, withUndo);
  /* set this new end-anchor as the new target */
  SetTargetContent (doc, attr);
  if (withUndo && new_)
    TtaRegisterAttributeCreate (attr, el, doc);
  TtaFreeMemory (url);
}

/*----------------------------------------------------------------------
  CreateAnchor
  Create a link or target element.                  
  ----------------------------------------------------------------------*/
void CreateAnchor (Document doc, View view, ThotBool createLink)
{
  Element             first, last, el;
  Element             parag, child, anchor, ancestor;
  ElementType         elType, parentType;
  Attribute           attr;
  DisplayMode         dispMode;
#ifdef TEMPLATES
  SSchema             sstempl;
#endif /* TEMPLATES */
  char               *s;
  int                 firstChar, lastChar, i;
  ThotBool            noAnchor, ok;

  if (!TtaGetDocumentAccessMode (doc))
    /* the document is in ReadOnly mode */
    return;

  parag = NULL;
  dispMode = TtaGetDisplayMode (doc);
  anchor = NULL;
  /* get the first and last selected element */
  TtaGiveFirstSelectedElement (doc, &first, &firstChar, &i);
  if (TtaIsReadOnly (first))
    {
      /* the selected element is read-only */
      return;
    }

  TtaGiveLastSelectedElement (doc, &last, &i, &lastChar);
  noAnchor = FALSE;
  /* Check whether the selected elements are a valid content for an anchor */
  elType = TtaGetElementType (first);
  s = TtaGetSSchemaName (elType.ElSSchema);
  parentType.ElSSchema = elType.ElSSchema;
  parentType.ElTypeNum = HTML_EL_HEAD;
  if (!strcmp (s, "HTML") && TtaGetTypedAncestor (first, parentType))
    {
#ifdef TEMPLATES
      sstempl = TtaGetSSchema ("Template", doc);
      if (IsTemplateDocument(doc) && sstempl)
        {
          parentType.ElSSchema = sstempl;
          parentType.ElTypeNum = Template_EL_component;
          if (!TtaGetTypedAncestor (first, parentType))
            {
              TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_INVALID_ANCHOR1);
              return;
            }
        }
#else /* TEMPLATES */
      TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_INVALID_ANCHOR1);
      return;
#endif /* TEMPLATES */
    }
  if ((elType.ElTypeNum == HTML_EL_Anchor ||
       elType.ElTypeNum == HTML_EL_MAP ||
       elType.ElTypeNum == HTML_EL_map) &&
      !strcmp (s, "HTML") &&
      first == last)
    {
      /* add an attribute on the current anchor */
      anchor = first;
      if (!createLink)
        {
          TtaOpenUndoSequence (doc, first, last, firstChar, lastChar);
          CreateTargetAnchor (doc, anchor, FALSE, FALSE, TRUE);
          TtaCloseUndoSequence (doc);
        }
    }
#ifdef _SVG
  else if (elType.ElTypeNum == SVG_EL_a && !strcmp (s, "SVG") &&
           first == last)
    {
      /* add an attribute on the current anchor */
      anchor = first;
      if (!createLink)
        {
          TtaOpenUndoSequence (doc, first, last, firstChar, lastChar);
          CreateTargetAnchor (doc, anchor, FALSE, FALSE, TRUE);
          TtaCloseUndoSequence (doc);
        }
    }
#endif /* _SVG */
  else
    {
      /* check whether the selection is within an anchor */
      if (!strcmp (s, "HTML") || !strcmp (s, "SVG"))
        el = SearchAnchor (doc, first, &attr, !createLink);
      else
        el = NULL;
      if (el)
        {
          /* add an attribute on this anchor */
          anchor = el;
          if (!createLink)
            {
              TtaOpenUndoSequence (doc, first, last, firstChar, lastChar);
              CreateTargetAnchor (doc, anchor, FALSE, FALSE, TRUE);
              TtaCloseUndoSequence (doc);
            }
        }
      else
        {
          el = first;

          while (!noAnchor && el != NULL)
            {
              elType = TtaGetElementType (el);
              s = TtaGetSSchemaName (elType.ElSSchema);
              if (!strcmp (s, "HTML"))
                {
                  if (createLink &&
                      elType.ElTypeNum != HTML_EL_TEXT_UNIT &&
                      elType.ElTypeNum != HTML_EL_Basic_Elem &&
                      elType.ElTypeNum != HTML_EL_Teletype_text &&
                      elType.ElTypeNum != HTML_EL_Italic_text &&
                      elType.ElTypeNum != HTML_EL_Bold_text &&
                      elType.ElTypeNum != HTML_EL_Underlined_text &&
                      elType.ElTypeNum != HTML_EL_Struck_text &&
                      elType.ElTypeNum != HTML_EL_Big_text &&
                      elType.ElTypeNum != HTML_EL_Small_text &&
                      elType.ElTypeNum != HTML_EL_Emphasis &&
                      elType.ElTypeNum != HTML_EL_Strong &&
                      elType.ElTypeNum != HTML_EL_Def &&
                      elType.ElTypeNum != HTML_EL_Code &&
                      elType.ElTypeNum != HTML_EL_Sample &&
                      elType.ElTypeNum != HTML_EL_Keyboard &&
                      elType.ElTypeNum != HTML_EL_Variable_ &&
                      elType.ElTypeNum != HTML_EL_Cite &&
                      elType.ElTypeNum != HTML_EL_ABBR &&
                      elType.ElTypeNum != HTML_EL_ACRONYM &&
                      elType.ElTypeNum != HTML_EL_INS &&
                      elType.ElTypeNum != HTML_EL_DEL &&
                      elType.ElTypeNum != HTML_EL_IMG &&
                      elType.ElTypeNum != HTML_EL_PICTURE_UNIT &&
                      elType.ElTypeNum != HTML_EL_Applet &&
                      elType.ElTypeNum != HTML_EL_Object &&
                      elType.ElTypeNum != HTML_EL_Font_ &&
                      elType.ElTypeNum != HTML_EL_SCRIPT_ &&
                      elType.ElTypeNum != HTML_EL_MAP &&
                      elType.ElTypeNum != HTML_EL_map &&
                      elType.ElTypeNum != HTML_EL_Quotation &&
                      elType.ElTypeNum != HTML_EL_Subscript &&
                      elType.ElTypeNum != HTML_EL_Superscript &&
                      elType.ElTypeNum != HTML_EL_Span &&
                      elType.ElTypeNum != HTML_EL_BDO &&
                      elType.ElTypeNum != HTML_EL_BR &&
                      elType.ElTypeNum != HTML_EL_simple_ruby &&
                      elType.ElTypeNum != HTML_EL_complex_ruby &&
                      elType.ElTypeNum != HTML_EL_ins &&
                      elType.ElTypeNum != HTML_EL_del &&
                      elType.ElTypeNum != HTML_EL_IFRAME)
                    noAnchor = TRUE;
                  else if (!createLink &&
                           elType.ElTypeNum == HTML_EL_Anchor)
                    noAnchor = TRUE;
                }
#ifdef _SVG
              else if (strcmp (s, "SVG") ||
                       elType.ElTypeNum == SVG_EL_SVG)
#else /* _SVG */
              else
#endif /* _SVG */
                /* don't accept to generate an anchor here */
                noAnchor = TRUE;

              if (el == last)
                el = NULL;
              else
                TtaGiveNextSelectedElement (doc, &el, &i, &i);
            }
	  
          if (noAnchor)
            {
              if (createLink || el)
                {
                  elType = TtaGetElementType (first);
                  if (first == last && firstChar == 0 && lastChar == 0 &&
                      createLink && strcmp (s, "HTML") && strcmp (s, "SVG"))
                    /* a single element is selected and it's not a HTML elem
                       nor a SVG element nor a character string */
                    {
                      if (UseLastTarget)
                        /* points to the last created target */
                        SetREFattribute (first, doc, TargetDocumentURL,
                                         TargetName);
                      else
                        /* select the destination */
                        SelectDestination (doc, first, TRUE, FALSE);
                    }
                  else
                    {
                      /* cannot create an anchor here */
                      TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_INVALID_ANCHOR1);
                      return;
                    }
                }
              else
                /* create an ID for target element */
                {
                  TtaOpenUndoSequence (doc, first, last, firstChar, lastChar);
                  CreateTargetAnchor (doc, first, FALSE, FALSE, TRUE);
                  TtaCloseUndoSequence (doc);
                }
              return;
            }
          /* check if the anchor to be created is within an anchor element */
          else
            {
              ok = TRUE;
              ancestor = SearchAnchor (doc, first, &attr, TRUE);
              if (ancestor)
                {
                  elType = TtaGetElementType (ancestor);
                  s = TtaGetSSchemaName (elType.ElSSchema);
                  if (!strcmp (s, "HTML") && elType.ElTypeNum == HTML_EL_Anchor)
                    ok = FALSE;
                }
              if (ok)
                {
                  ancestor = SearchAnchor (doc, last, &attr, TRUE);
                  if (ancestor)
                    {
                      elType = TtaGetElementType (ancestor);
                      s = TtaGetSSchemaName (elType.ElSSchema);
                      if (!strcmp (s, "HTML") &&
                          elType.ElTypeNum == HTML_EL_Anchor)
                        ok = FALSE;
                    }
                }

              elType = TtaGetElementType (first);
              s = TtaGetSSchemaName (elType.ElSSchema);
              if (!ok)
                {
                  TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_INVALID_ANCHOR2);
                  return;
                }
              else if (!strcmp (s, "SVG") &&
                       elType.ElTypeNum == HTML_EL_TEXT_UNIT)
                {
                  /* move the selection to the enclosing text of tspan */
                  first = TtaGetParent (first);
                  if (first == NULL)
                    return;
                  last = first;
                }
            }
          TtaOpenUndoSequence (doc, first, last, firstChar, lastChar);
          if (createLink)
            {
              if(GenerateInlineElement (HTML_EL_Anchor, NULL, HTML_ATTR_HREF_, "", TRUE))
                {
                  // get the created anchor
                  TtaGiveFirstSelectedElement (doc, &anchor, &firstChar, &i);
                  if (anchor)
                    {
                      elType = TtaGetElementType (anchor);
                      s = TtaGetSSchemaName (elType.ElSSchema);
                      while (anchor &&
                             (elType.ElTypeNum != HTML_EL_Anchor || strcmp (s, "HTML")) &&
                             (elType.ElTypeNum != SVG_EL_a || strcmp (s, "SVG")))
                      {
                        anchor = TtaGetParent (anchor);
                        elType = TtaGetElementType (anchor);
                        s = TtaGetSSchemaName (elType.ElSSchema);
                      }
                    }
                }
              else
                {
                  /* ask Thot to display changes made in the document */
                  TtaSetDisplayMode (doc, dispMode);
                  TtaCloseUndoSequence (doc);
                  return;
                }
            }
          else
            {
              if(!GenerateInlineElement (HTML_EL_Anchor, NULL, HTML_ATTR_ID, "", TRUE))
                {
                  /* ask Thot to display changes made in the document */
                  TtaSetDisplayMode (doc, dispMode);
                  TtaCloseUndoSequence (doc);
                  return;
                }
            }
        }
    }

  TtaSetDocumentModified (doc);
  /* ask Thot to display changes made in the document */
  TtaSetDisplayMode (doc, dispMode);
  if (anchor)
    {
      if (TtaGetElementVolume (anchor))
        TtaSelectElement (doc, anchor);
      else
        {
          /* prepare a possible insertion of text */
          child = TtaGetLastChild (anchor);
          TtaSelectString (doc, child, 1, 0);
        }
    }

  if (createLink)
    {
      if (UseLastTarget)
        /* points to the last created target */
        SetREFattribute (anchor, doc, TargetDocumentURL, TargetName);
      else
        /* Select the destination */
        SelectDestination (doc, anchor, FALSE, FALSE);
    }
  TtaCloseUndoSequence (doc);
}

/*----------------------------------------------------------------------
  MakeUniqueName
  Check attribute NAME or ID in order to make sure that its value is unique
  in the document.
  If doIt and the NAME or ID is already used, the function adds a number
  at the end of the value.
  If withUndo, register changes in the undo queue.
  Return TRUE if the initial value was changed or needs to be changed.
  ----------------------------------------------------------------------*/
ThotBool MakeUniqueName (Element el, Document doc, ThotBool doIt,
                         ThotBool withUndo)
{
  ElementType	      elType;
  AttributeType     attrType, attrIDType;
  Attribute         attr, attrID = NULL;
  Element	          image, elFound;
  char             *value, *name;
  char              url[MAX_LENGTH];
  int               length, i;
  ThotBool          change, checkID, checkNAME, checkXMLID;
  ThotBool          result = FALSE;
  
  elType = TtaGetElementType (el);
  attrType.AttrSSchema = elType.ElSSchema;
  attrIDType.AttrSSchema = elType.ElSSchema;
  checkID = checkNAME = checkXMLID = FALSE;
  name = TtaGetSSchemaName (elType.ElSSchema);
  if (!strcmp(name, "HTML"))
    {
      attrIDType.AttrTypeNum = HTML_ATTR_xmlid;
      attrID = TtaGetAttribute (el, attrIDType);
      if (attrID)
        /* the element has a xml:id attribute. Check it too */
        checkXMLID = TRUE;
      /* it's an element from the XHTML namespace */
      if (elType.ElTypeNum == HTML_EL_Anchor ||
          elType.ElTypeNum == HTML_EL_MAP ||
          elType.ElTypeNum == HTML_EL_map)
        /* it's an anchor or a map. Look for a NAME attribute */
        {
          attrType.AttrTypeNum = HTML_ATTR_NAME;
          attr = TtaGetAttribute (el, attrType);
          if (attr)
            /* the element has a NAME attribute. Check it and then check
               if there is an ID too */
            checkID = TRUE;
          else
            {
              /* no NAME. Look for an ID */
              attrType.AttrTypeNum = HTML_ATTR_ID;
              if (TtaGetDocumentProfile (doc) != L_Xhtml11)
                checkNAME = TRUE;
            }
        }
      else
        /* Look for an ID attribute */
        attrType.AttrTypeNum = HTML_ATTR_ID;
    }
  else if (!strcmp(name, "MathML"))
    {
      /* it's an element from the MathML namespace, look for the
         id attribute from the same namespace */
      attrIDType.AttrTypeNum = MathML_ATTR_xmlid;
      attrID = TtaGetAttribute (el, attrIDType);
      if (attrID)
        /* the element has a xml:id attribute. Check it too */
        checkXMLID = TRUE;
      attrType.AttrTypeNum = MathML_ATTR_id;
    }
#ifdef _SVG
  else if (!strcmp(name, "SVG"))
    {
      /* it's an element from the SVG namespace, look for the
         id attribute from the same namespace */
      attrIDType.AttrTypeNum = SVG_ATTR_xmlid;
      attrID = TtaGetAttribute (el, attrIDType);
      if (attrID)
        /* the element has a xml:id attribute. Check it too */
        checkXMLID = TRUE;
      attrType.AttrTypeNum = SVG_ATTR_id;
    }
#endif /* _SVG */
#ifdef TEMPLATES
  else if (!strcmp(name, "Template"))
    {
      /* it's an element from the XTiger namespace, look for the
         id attribute from the same namespace */
      attrIDType.AttrTypeNum = Template_ATTR_name;
      attrID = TtaGetAttribute (el, attrIDType);
      if (attrID)
        /* the element has a name attribute. Check it too */
        checkXMLID = TRUE;
      attrType.AttrTypeNum = Template_ATTR_name;
    }
#endif /* TEMPLATES */
  else
#ifdef XML_GENERIC
    attrType.AttrTypeNum = XML_ATTR_xmlid;
#else /* XML_GENERIC */
  attrType.AttrTypeNum = 0;
#endif /* XML_GENERIC */
  
  if (attrType.AttrTypeNum != 0)
    {
      attr = TtaGetAttribute (el, attrType);
      if (attr)
        /* the element has an attribute NAME or ID. Check it */
        {
          length = TtaGetTextAttributeLength (attr) + 10;
          value = (char *)TtaGetMemory (length);
          change = FALSE;
          if (value)
            {
              TtaGiveTextAttributeValue (attr, value, &length);
              i = 0;
              elFound = SearchNAMEattribute (doc, value, attr, el);
              while (elFound)
                {
                  /* form element or not, the NAME must be changed */
                  change = TRUE;
                  i++;
                  sprintf (&value[length], "%d", i);
                  result = TRUE;
                  /* recheck the new value */
                  elFound = SearchNAMEattribute (doc, value, attr, el);
                }
              
              if (change && doIt)
                {
                  /* copy the element Label into the NAME attribute */
                  TtaSetAttributeText (attr, value, el, doc);
                  if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML") &&
                      elType.ElTypeNum == HTML_EL_MAP)
                    /* it's a MAP element */
                    {
                      /* Search backward the refered image */
                      attrType.AttrTypeNum = HTML_ATTR_USEMAP;
                      TtaSearchAttribute (attrType, SearchBackward, el,
                                          &image, &attr);
                      if (!attr)
                        /* Not found. Search forward the refered image */
                        TtaSearchAttribute (attrType, SearchForward, el,
                                            &image, &attr);
                      if (attr && image)
                        /* referred image found */
                        {
                          i = MAX_LENGTH;
                          TtaGiveTextAttributeValue (attr, url, &i);
                          if (i == length+1 &&
                              !strncmp (&url[0], value, length))
                            {
                              /* Change the USEMAP attribute of the image */
                              attr = TtaGetAttribute (image, attrType);
                              strcpy (&url[0], value);
                              TtaSetAttributeText (attr, url, image, doc);
                            }
                        }
                    }
                }
            }
          if (checkID && doIt)
            {
              /* Change or insert an ID attribute accordingly */
              attrType.AttrTypeNum = HTML_ATTR_ID;
              attr = TtaGetAttribute (el, attrType);
              if (attr == NULL)
                {
                  attr = TtaNewAttribute (attrType);
                  TtaAttachAttribute (el, attr, doc);
                  TtaSetAttributeText (attr, value, el, doc);
                  if (withUndo)
                    TtaRegisterAttributeCreate (attr, el, doc);
                }
              else
                {
                  if (withUndo && change)
                    TtaRegisterAttributeReplace (attr, el, doc);
                  TtaSetAttributeText (attr, value, el, doc);
                }
            }
          else if (checkNAME && doIt)
            {
              /* Change or insert a NAME attribute accordingly */
              attrType.AttrTypeNum = HTML_ATTR_NAME;
              attr = TtaGetAttribute (el, attrType);
              if (attr == NULL)
                {
                  attr = TtaNewAttribute (attrType);
                  TtaAttachAttribute (el, attr, doc);
                  TtaSetAttributeText (attr, value, el, doc);
                  if (withUndo)
                    TtaRegisterAttributeCreate (attr, el, doc);
                }
              else
                {
                  if (withUndo && change)
                    TtaRegisterAttributeReplace (attr, el, doc);
                  TtaSetAttributeText (attr, value, el, doc);
                }
            }
          if (checkXMLID && doIt)
            {
              /* Change or insert an ID attribute accordingly */
              if (withUndo)
                TtaRegisterAttributeReplace (attrID, el, doc);
              TtaSetAttributeText (attrID, value, el, doc);
            }
          TtaFreeMemory (value);
        }
    }
  return result;
}

/*----------------------------------------------------------------------
  SearchTypedElementForward
  Search for a typed element and stops when it finds it or if the
  search reaches the last element.
  ----------------------------------------------------------------------*/
static Element SearchTypedElementForward (ElementType elType_search,
                                          Element curr, Element last)
{
  ElementType elType;
  Element el;

  /* start by getting the next node */
  el = GetNextNode (curr);
  /*  continue browsing until we get an element
      of the searched type or the end condition is
      reached */
  while (el && el != last)
    {
      elType = TtaGetElementType (el);
      if (TtaSameTypes (elType_search, elType))
        break;
      el = GetNextNode (el);
    }

  /* don't return el if it's equal to the last element parameter */
  return ((el != last) ? el : NULL);
}

/*----------------------------------------------------------------------
  CreateRemoveIDAttribute
  For all elements elName of a document, this functions eithers adds or 
  deletes an ID attribute. 
  The createID flag tells which operation must be done.
  The inSelection attribute says if we must apply the operation in the
  whole document or just in the current selection.
  If an element already has an ID attribute, a new one won't be created.
  TO DO: Use the thotmsg functions for the dialogs.
  ----------------------------------------------------------------------*/
void CreateRemoveIDAttribute (char *elName, Document doc, ThotBool createID,
                              ThotBool inSelection)
{
  Element             el, lastEl;
  ElementType         elType;
  AttributeType       attrType;
  Attribute           attr;
  char               *schema_name;
  DisplayMode         dispMode;
  ThotBool            closeUndo;
  int                 i, j;

  /* the user must select something */
  if (inSelection && !TtaIsDocumentSelected (doc))
    {
      strcpy (IdStatus, TtaGetMessage (AMAYA, AM_NO_INSERT_POINT));
      return;
    }
  /* search for the elementType corresponding to the element name given
     by the user */
  GIType (elName, &elType, doc);
  if (elType.ElTypeNum == 0)
    {
      /* element name not found */
      strcpy (IdStatus, TtaGetMessage (AMAYA, AM_INVALID_ELEMENT));
      return;
    }
  /* in function of the target elType, we choose the correct
     ATTR_ID value and schema */
#ifdef XML_GENERIC
  attrType.AttrTypeNum = XML_ATTR_xmlid;
#else /* XML_GENERIC */
  attrType.AttrTypeNum = 0;
#endif /* XML_GENERIC */
  schema_name = TtaGetSSchemaName (elType.ElSSchema);
  if (!strcmp (schema_name, "HTML"))
    {
      /* exception handling... we can't add an ID attribute everywhere
         in HTML documents */
      if (!(elType.ElTypeNum == HTML_EL_HTML
            || elType.ElTypeNum == HTML_EL_HEAD
            || elType.ElTypeNum == HTML_EL_TITLE
            || elType.ElTypeNum == HTML_EL_BASE
            || elType.ElTypeNum == HTML_EL_META
            || elType.ElTypeNum == HTML_EL_SCRIPT_
            || elType.ElTypeNum == HTML_EL_STYLE_))
        attrType.AttrTypeNum = HTML_ATTR_ID;
    }
  else if (!strcmp (schema_name, "MathML"))
    attrType.AttrTypeNum = MathML_ATTR_id;
#ifdef _SVG
  else if (!strcmp (schema_name, "SVG"))
    attrType.AttrTypeNum = SVG_ATTR_id;
#endif /* _SVG */

  /* we didn't find an attribute or we can't put an ID attribute
     in this element */
  if (attrType.AttrTypeNum == 0)
    {
      sprintf (IdStatus, TtaGetMessage (AMAYA, AM_CHANGE_NUMBER), 0);
      return;
    }
  attrType.AttrSSchema = elType.ElSSchema;

  /* prepare the environment before doing the operation */
  dispMode = TtaGetDisplayMode (doc);
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, DeferredDisplay);

  /* set the first and last elements for the search */
  if (inSelection)
    {
      /* get the first and last elements of the selection */
      TtaGiveFirstSelectedElement (doc, &el, &i, &j);
      if (TtaIsSelectionEmpty ())
        lastEl = el;
      else
        TtaGiveLastSelectedElement (doc, &lastEl, &j, &i);
      /* and set the last element as the element just after
         the selection */
      lastEl = TtaGetSuccessor (lastEl);
    }
  else
    {
      el = TtaGetMainRoot (doc);
      lastEl = NULL;
    }

  /*
  ** browse the tree and add the ID if it's missing. Variable i
  ** stores the number of changes we have done.
  */

  /* move to the first element that is of the chosen elementType */
  /* the element where we started is not of the chosen type */
  if (!TtaSameTypes (TtaGetElementType (el), elType))
    el = SearchTypedElementForward (elType, el, lastEl);

  if (TtaHasUndoSequence (doc))
    closeUndo = FALSE;
  else
    {
      closeUndo = TRUE;
      TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
    }
  i = 0;
  while (el)
    {
      /* does the element have an ID attribute already? */
      attr = TtaGetAttribute (el, attrType);
	  if (!TtaIsReadOnly (el))
		{
          /* the selected element is not read-only */
          if (!attr && createID) /* add it */
		  {
            /* we reuse an existing Amaya function */
            CreateTargetAnchor (doc, el, TRUE, FALSE, TRUE);
            i++;
		  }
          else if (attr && !createID) /* delete it */
		  {
            TtaRegisterAttributeDelete (attr, el, doc);
            TtaRemoveAttribute (el, attr, doc);
            i++;
		  }
	  }
      /* get the next element */
      el = SearchTypedElementForward (elType, el, lastEl);
    }

  if (closeUndo)
    TtaCloseUndoSequence (doc);

  /* reset the state of the document */
  if (i)
    TtaSetDocumentModified (doc);
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, dispMode);
  sprintf (IdStatus, TtaGetMessage (AMAYA, AM_CHANGE_NUMBER), i);
#ifdef _WX
  //InitInfo ("", IdStatus);
#endif /* _WX */
}

/*----------------------------------------------------------------------
  CheckPseudoParagraph
  Element el has been created or pasted. If it's a Pseudo_paragraph,
  it is turned into an ordinary Paragraph if it's not the first child
  of its parent.
  If the next sibiling is a Pseudo_paragraph, this sibling is turned into
  an ordinary Paragraph.
  Rule: only the first child of any element can be a Pseudo_paragraph.
  ----------------------------------------------------------------------*/
void CheckPseudoParagraph (Element el, Document doc)
{
  Element		prev, next, parent, sibling, child;
  Attribute             attr;
  ElementType		elType, textType;
  SSchema               htmlSchema;

  elType = TtaGetElementType (el);
  htmlSchema = TtaGetSSchema ("HTML", doc);
  if (!htmlSchema || elType.ElSSchema != htmlSchema)
    /* it's not an HTML element */
    return;

  if (elType.ElTypeNum == HTML_EL_Comment_ ||
      elType.ElTypeNum == HTML_EL_XMLPI)
    /* the new element is a comment or a PI. No need to change anything */
    return;

  if (elType.ElTypeNum == HTML_EL_Pseudo_paragraph)
    /* the new element is a Pseudo_paragraph */
    {
      prev = el;
      TtaPreviousSibling (&prev);
      attr = NULL;
      TtaNextAttribute (el, &attr);
      if (prev || attr)
        /* the Pseudo-paragraph is not the first element among its sibling */
        /* or it has attributes: turn it into an ordinary paragraph */
        TtaChangeElementType (el, HTML_EL_Paragraph);
    }
  else if (elType.ElTypeNum == HTML_EL_Paragraph)
    /* the new element is a Paragraph */
    {
      /* get the previous element that is not a comment or a PI */
      prev = el;
      do
        {
          TtaPreviousSibling (&prev);
          if (prev)
            elType = TtaGetElementType (prev);
        }
      while (prev && elType.ElSSchema == htmlSchema &&
             (elType.ElTypeNum == HTML_EL_Comment_ ||
              elType.ElTypeNum == HTML_EL_XMLPI));

      attr = NULL;
      TtaNextAttribute (el, &attr);
      if (prev == NULL && attr == NULL)
        /* the Paragraph is the first element among its sibling and it has
           no attribute */
        /* turn it into an Pseudo-paragraph if it's in a List_item or a
           table cell. */
        {
          parent = TtaGetParent (el);
          if (parent)
            {
              elType = TtaGetElementType (parent);
              if (elType.ElTypeNum == HTML_EL_List_Item ||
                  elType.ElTypeNum == HTML_EL_Definition ||
                  elType.ElTypeNum == HTML_EL_Data_cell ||
                  elType.ElTypeNum == HTML_EL_Heading_cell ||
                  elType.ElTypeNum == HTML_EL_Object_Content ||
                  elType.ElTypeNum == HTML_EL_map)
                TtaChangeTypeOfElement (el, doc, HTML_EL_Pseudo_paragraph);
            }
        }
    }
  else if (elType.ElTypeNum == HTML_EL_Block)
    /* the new element is a Block */
    {
      /* if this Block is empty and it is the only child of a List_Item,
         change it into a Pseudo_Paragraph */
      if (!TtaGetFirstChild (el))
        /* it is empty */
        if (TtaGetElementType (TtaGetParent (el)).ElTypeNum == HTML_EL_List_Item)
          /* it's a child of a List_Item */
          {
            sibling = el;
            TtaNextSibling (&sibling);
            if (!sibling)
              {
                sibling = el;
                TtaPreviousSibling (&sibling);
                if (!sibling)
                  /* no siblings */
                  {
                    TtaChangeElementType (el, HTML_EL_Pseudo_paragraph);
                    /* insert the text element */
                    textType.ElSSchema = elType.ElSSchema;
                    textType.ElTypeNum = HTML_EL_TEXT_UNIT;
                    child = TtaNewElement (doc, textType);
                    TtaInsertFirstChild  (&child, el, doc);
                  }
              }
          }
    }

  /* get the next element that is not a comment or a PI */
  next = el;
  do
    {
      TtaNextSibling (&next);
      if (next)
        elType = TtaGetElementType (next);
    }
  while (next && elType.ElSSchema == htmlSchema &&
         (elType.ElTypeNum == HTML_EL_Comment_ ||
          elType.ElTypeNum == HTML_EL_XMLPI));

  if (next && elType.ElSSchema == htmlSchema &&
      elType.ElTypeNum == HTML_EL_Pseudo_paragraph)
    /* the next element is a Pseudo-paragraph */
    /* turns it into an ordinary paragraph */
    {
      TtaChangeTypeOfElement (next, doc, HTML_EL_Paragraph);
      TtaRegisterElementTypeChange (next, elType.ElTypeNum, doc);
    }
}

/*----------------------------------------------------------------------
  ElementCreated
  An element has been created in a HTML document.
  ----------------------------------------------------------------------*/
void ElementCreated (NotifyElement *event)
{
  CheckPseudoParagraph (event->element, event->document);
}

/*----------------------------------------------------------------------
  ElementWillBeDeleted
  An HTML element will be deleted.
  Update the namespace declarations associated with that element
  ----------------------------------------------------------------------*/
void TextPasted (NotifyElement *event)
{
  Element         parent;
  ElementType	    elType;
  NotifyOnTarget  style_event;

  parent = TtaGetParent (event->element);
  elType = TtaGetElementType (parent);
  if (elType.ElTypeNum == HTML_EL_STYLE_)
    {
      style_event.element = parent;
      style_event.document = event->document;
      StyleChanged (&style_event);
    }
}

/*----------------------------------------------------------------------
  ElementWillBeDeleted
  An HTML element will be deleted.
  Update the namespace declarations associated with that element
  ----------------------------------------------------------------------*/
ThotBool ElementWillBeDeleted (NotifyElement *event)
{
  ElementType	    elType;
  Element         parent;
  NotifyOnTarget  style_event;
  char           *name;

  elType = TtaGetElementType (event->element);
  name = TtaGetSSchemaName (elType.ElSSchema);
  if (!strcmp (name, "HTML") &&
      (elType.ElTypeNum == HTML_EL_TEXT_UNIT ||
       elType.ElTypeNum == HTML_EL_Inserted_Text))
    {
      /* it could be an input element */
      parent = TtaGetParent (event->element);
      elType = TtaGetElementType (parent);
      if (elType.ElTypeNum == HTML_EL_STYLE_)
        {
          // save the current style content
          style_event.element = parent;
          style_event.document = event->document;
          ChangeStyle (&style_event);
        }

      if (elType.ElTypeNum == HTML_EL_Text_Input ||
          elType.ElTypeNum == HTML_EL_Password_Input ||
          elType.ElTypeNum == HTML_EL_Text_Area ||
          elType.ElTypeNum == HTML_EL_File_Input ||
          elType.ElTypeNum == HTML_EL_Inserted_Text)
        Document_state = TtaIsDocumentModified (event->document);
    }
  else
    TtaFreeElemNamespaceDeclarations (event->document, event->element);
  return FALSE; /* let Thot perform normal operation */
}
 
/*----------------------------------------------------------------------
  ElementDeleted
  An element has been deleted. If it was the only child of element
  BODY, create a first paragraph.
  -----------------------------------------------------------------------*/
void ElementDeleted (NotifyElement *event)
{
  Element	     child, el, parent;
  ElementType	 elType, childType;
  char        *name;
  ThotBool	   empty;

  elType = TtaGetElementType (event->element);
  name = TtaGetSSchemaName (elType.ElSSchema);
  if (strcmp (name, "HTML"))
    return;
  // it could be the parent of a deleted table
  CheckDeleteParentTable (event->element);
  if (elType.ElTypeNum == HTML_EL_TEXT_UNIT ||
       elType.ElTypeNum == HTML_EL_Inserted_Text)
    {
      /* it could be an input element */
      parent = TtaGetParent (event->element);
      elType = TtaGetElementType (parent);
      if ((elType.ElTypeNum == HTML_EL_Text_Input ||
           elType.ElTypeNum == HTML_EL_Password_Input ||
           elType.ElTypeNum == HTML_EL_Text_Area ||
           elType.ElTypeNum == HTML_EL_File_Input ||
           elType.ElTypeNum == HTML_EL_Inserted_Text) &&
          !Document_state)
        {
          TtaSetDocumentUnmodified (event->document);
          /* switch Amaya buttons and menus */
          DocStatusUpdate (event->document, Document_state);
        }
    }
  else if (elType.ElTypeNum == HTML_EL_BODY)
    {
      child = TtaGetFirstChild (event->element);
      empty = TRUE;
      while (empty && child)
        {
          elType = TtaGetElementType (child);
          if (elType.ElTypeNum != HTML_EL_Comment_ &&
              elType.ElTypeNum != HTML_EL_Invalid_element)
            empty = FALSE;
          else
            TtaNextSibling (&child);
        }
      if (empty)
        {
          elType.ElTypeNum = HTML_EL_Paragraph;
          child = TtaNewTree (event->document, elType, "");
          TtaInsertFirstChild (&child, event->element, event->document);
          TtaRegisterElementCreate (child, event->document);
          do
            {
              el = TtaGetFirstChild (child);
              if (el)
                child = el;
            }
          while (el);
          TtaSelectElement (event->document, child);
        }
    }
  /* if the deleted element was the first child of a LI, transform
     the new first child into a Pseudo-Paragraph if it's a Paragraph */
  else if (elType.ElTypeNum == HTML_EL_List_Item)
    {
      /* the parent element is a List_Item */
      if (event->position == 0)
        /* the deleted element was the first child */
        {
          child = TtaGetFirstChild (event->element);
          if (child)
            {
              childType = TtaGetElementType (child);
              if (childType.ElTypeNum == HTML_EL_Paragraph)
                /* the new first child is a Paragraph */
                {
                  TtaRegisterElementReplace (child, event->document);
                  TtaRemoveTree (child, event->document);
                  TtaChangeElementType (child, HTML_EL_Pseudo_paragraph);
                  TtaInsertFirstChild (&child, event->element, event->document);
                }
            }
        }
    }
}


/*----------------------------------------------------------------------
  RegisterURLSavedElements
  Elements from document doc have been saved into the Thot Copy/Cut buffer.
  Save the URL of this document, to allow relative URIs contained
  in these elements to be adapted when they are pasted.
  ----------------------------------------------------------------------*/
void RegisterURLSavedElements (Document doc)
{
  if (SavedDocumentURL)
    TtaFreeMemory (SavedDocumentURL);
  SavedDocumentURL = GetBaseURL (doc);
  /* Paste functions in the table menu could be active now */
  UpdateContextSensitiveMenus (doc, 1);
}

/*----------------------------------------------------------------------
  ChangeURI
  Element el has been pasted in document doc. It comes from document
  originDocument and it has an href attribute (from the HTML or XLink
  namespace) that has to be updated. Update it according to the new
  context.
  ----------------------------------------------------------------------*/
void ChangeURI (Element el, Attribute attr, Document originDocument,
                Document doc)
{
  int      length, i, iName;
  char    *value, *base, *documentURI, *tempURI, *path;

  /* get a buffer for the URI */
  length = TtaGetTextAttributeLength (attr) + 1;
  value = (char *)TtaGetMemory (length);
  if (value)
    {
      tempURI = (char *)TtaGetMemory (MAX_LENGTH);
      if (tempURI)
        {
          iName = 0;
          /* get the URI itself */
          TtaGiveTextAttributeValue (attr, value, &length);
          if (value[0] == '#')
            /* the target is in the original document */
            /* convert the internal link into an external link */
            {
              if (originDocument == 0)
                {
                  /* origin document has been unloaded. Get the saved URL */
                  if (SavedDocumentURL == NULL)
                    RegisterURLSavedElements (doc);
                  strcpy (tempURI, SavedDocumentURL);
                }
              else
                strcpy (tempURI, DocumentURLs[originDocument]);
            }
          else
            {
              /* the target element is in another document */
              documentURI = (char *)TtaGetMemory (MAX_LENGTH);
              if (documentURI)
                {
                  strcpy (documentURI, value);
                  /* looks for a '#' in the value */
                  i = length;
                  while (value[i] != '#' && i > 0)
                    i--;
                  if (i == 0)
                    /* there is no '#' in the URI */
                    value[0] = EOS;
                  else
                    {
                      /* there is a '#' character in the URI */
                      /* separate document name and element name */
                      documentURI[i] = EOS;
                      iName = i;
                    }
                  /* get the complete URI of the referred document */
                  /* Add the base if necessary */
                  path = (char *)TtaGetMemory (MAX_LENGTH);
                  if (path)
                    {
                      if (originDocument == 0)
                        NormalizeURL (documentURI, -1, tempURI, path, NULL);
                      else
                        NormalizeURL (documentURI, originDocument, tempURI, path,NULL);
                      TtaFreeMemory (path);
                    }
                  TtaFreeMemory (documentURI);
                }
            }
          if (value[iName] == '#')
            {
              if (!strcmp (tempURI, DocumentURLs[doc]))
                /* convert external link into internal link */
                strcpy (tempURI, &value[iName]);
              else
                strcat (tempURI, &value[iName]);
            }
          /* set the relative value or URI in attribute HREF */
          base = GetBaseURL (doc);
          if (base)
            {
              TtaFreeMemory (value);
              value = MakeRelativeURL (tempURI, base);
              TtaFreeMemory (base);
            }
          TtaSetAttributeText (attr, value, el, doc);
          TtaFreeMemory (tempURI);
        }
      TtaFreeMemory (value);
    }
}

static void CheckDescendants (Element el, Document doc);

/*----------------------------------------------------------------------
  ElementOKforProfile
  This function is called for each element pasted by the user, and for
  each element within the pasted element.
  Check whether element el is valid in the document profile. If it is
  not, delete the element and return FALSE.
  Check also all attributes associated with the element and remove
  the attributes that are not allowed by the profile.
  ----------------------------------------------------------------------*/
ThotBool ElementOKforProfile (Element el, Document doc)
{
  ElementType    elType;
  char           *name;
  AttributeType  attrType;
  int            kind, profile;
  Attribute      attr, nextAttr;
  Element        child;
  ThotBool       ok, record;

  ok = TRUE;
  /* handle only HTML elements */
  elType = TtaGetElementType (el);
  profile = TtaGetDocumentProfile (doc);
  if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML"))
    {
      /* it's an element from the HTML namespace */
      if (profile != L_Other)
        /* the document profile accepts only certain elements and attributes */
        {
          name = (char*) GetXMLElementName (elType, doc);
          if (name == NULL || name[0] == EOS)
            /* this element type is not accepted in the document profile */
            ok = FALSE;
          else
            /* the element type is OK for the profile */
            {
              /* check all attributes of the element */
              attr = 0;
              TtaNextAttribute (el, &attr);
              while (attr)
                {
                  nextAttr = attr;  TtaNextAttribute (el, &nextAttr);
                  TtaGiveAttributeType (attr, &attrType, &kind);
                  name = (char*) GetXMLAttributeName (attrType, elType, doc);
                  if (name == NULL || name[0] == EOS)
                    /* this attribute is not valid for this element in the
                       document profile. Delete it */
                    TtaRemoveAttribute (el, attr, doc);
                  attr = nextAttr;
                }
            }
        }
    }
  else if (profile == L_Strict || profile == L_Basic)
    {
      /* cannot insert here */
      ok =  FALSE;
      TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_NOT_ALLOWED), NULL);
    }

  if (!ok)
    /* The element type is not acceptend in the document profile.
       Delete the element but keep its children if they are allowed */
    {
      record = FALSE;
      if (TtaGetLastCreatedElemInHistory (doc) == el)
        /* the last item in the undo history is the creation of this element,
           but this element will be removed. Register its children instead */
        {
          TtaCancelLastRegisteredOperation (doc);
          record = TRUE;
        }
      /* move all children right before the element */
      child = TtaGetFirstChild (el);
      while (child)
        {
          /* check that this child is allowed. If not, delete it */
          if (ElementOKforProfile (child, doc))
            /* this child is allowed. Move it and remove all invalid elements
               and attributes from its decendants */
            {
              TtaRemoveTree (child, doc);
              CheckDescendants (child, doc);
              TtaInsertSibling (child, el, TRUE, doc);
              if (record)
                TtaRegisterElementCreate (child, doc);
            }
          /* take the first child of el, not the sibling of child, as
             child may have been deleted by ElementOKforProfile */
          child = TtaGetFirstChild (el);
        }
      /* the element is now empty. Delete it. */
      TtaDeleteTree (el, doc);
    }
  return ok;
}

/*----------------------------------------------------------------------
  CheckDescendants
  Element el is a valid element in the document profile. Remove the
  invalid elements and attributes from its decendants.
  ----------------------------------------------------------------------*/
static void CheckDescendants (Element el, Document doc)
{
  Element child, nextChild;

  child = TtaGetFirstChild (el);
  while (child)
    {
      nextChild = child;  TtaNextSibling (&nextChild);
      if (ElementOKforProfile (child, doc))
        CheckDescendants (child, doc);
      child = nextChild;
    }
}

/*----------------------------------------------------------------------
  RemoveTextAttributes
  removes all attributes attached to a TEXT_UNIT
  ----------------------------------------------------------------------*/
void RemoveTextAttributes (Element el, Document doc)
{
  Attribute           attr;

  do
    {
      attr = NULL;
      TtaNextAttribute (el, &attr);
      if (attr)
        TtaRemoveAttribute (el, attr, doc);
    }
  while (attr);
}


/*----------------------------------------------------------------------
  CheckPastedElement
  This function is called for each element pasted by the user, and for
  each element within the pasted element.
  Check Pseudo paragraphs.
  If the pasted element has a NAME attribute, change its value if this
  NAME is already used in the document.
  If it's within the TITLE element, update the corresponding field in
  the Formatted window.
  by_ref is TRUE if the pasted element comes from an inclusion reference
  ----------------------------------------------------------------------*/
void CheckPastedElement (Element el, Document doc, int info, int position,
                         ThotBool by_ref)
{
  Document            originDocument;
  Element             anchor, child, previous, nextchild, parent;
  Element             root, ancestor, sibling;
  ElementType         elType;
  AttributeType       attrType;
  Attribute           attr;
  char               *value, *name;
  int                 length;
  ThotBool            oldStructureChecking, ok;
  DisplayMode         dispMode;

  if (!by_ref && !ElementOKforProfile (el, doc))
    return;

  elType = TtaGetElementType (el);
  name = TtaGetSSchemaName (elType.ElSSchema);
  if (elType.ElTypeNum != HTML_EL_Paragraph || strcmp (name, "HTML") ||
      !WithinLastPastedCell (el))
    /* Check pseudo-paragraphs */
    CheckPseudoParagraph (el, doc);

  /* Check attribute NAME or ID in order to make sure that its value */
  /* is unique in the document, except if we are including an external
     document referred by an <object> or <embed> element. In this case, there
     is no need to check IDs as the included document will never be changed
     nor saved */
  if (elType.ElTypeNum > 0)
    MakeUniqueName (el, doc, TRUE, TRUE);

  anchor = NULL;
  if (!strcmp (name, "HTML"))
    {
      if (info == 0 && elType.ElTypeNum == HTML_EL_Anchor)
        anchor = el;
      else if (elType.ElTypeNum == HTML_EL_LINK)
        {
          /* check if it's a CSS link */
          CheckCSSLink (el, doc, elType.ElSSchema);
        }
      else if (elType.ElTypeNum == HTML_EL_STYLE_)
        /* The pasted element is a STYLE element in the HEAD */
        /* Get its content */
        EnableStyleElement (doc, el);
      else if (elType.ElTypeNum == HTML_EL_TEXT_UNIT ||
               elType.ElTypeNum == HTML_EL_Inserted_Text)
        {
          if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
            /* remove all attributes attached to the pasted HTML_EL_TEXT_UNIT */
            RemoveTextAttributes (el, doc);
          parent = TtaGetParent (el);
          elType = TtaGetElementType (parent);
          if (elType.ElTypeNum == HTML_EL_TITLE)
            /* the parent of the pasted text is the TITLE element */
            /* That's probably the result of undoing a change in the TITLE */
            UpdateTitle (parent, doc);
          else if ((elType.ElTypeNum == HTML_EL_Text_Input ||
                    elType.ElTypeNum == HTML_EL_Password_Input ||
                    elType.ElTypeNum == HTML_EL_Text_Area ||
                    elType.ElTypeNum == HTML_EL_File_Input ||
               elType.ElTypeNum == HTML_EL_Inserted_Text) &&
                   !Document_state)
            {
              // The paste could come from undo of an input element
              TtaSetDocumentUnmodified (doc);
              /* switch Amaya buttons and menus */
              DocStatusUpdate (doc, Document_state);
            }
        }
      else if (elType.ElTypeNum == HTML_EL_IMG ||
               elType.ElTypeNum == HTML_EL_Object ||
               elType.ElTypeNum == HTML_EL_Image_Input)
        {
          originDocument = (Document) position;
          if (originDocument > 0)
            {
              /* remove USEMAP attribute */
              attrType.AttrSSchema = elType.ElSSchema;
              attrType.AttrTypeNum = HTML_ATTR_USEMAP;
              attr = TtaGetAttribute (el, attrType);
              if (attr != NULL)
                TtaRemoveAttribute (el, attr, doc);
              /* Change attributes SRC if the element comes from another */
              /* document */
              if (originDocument != doc)
                {
                  /* the image has moved from one document to another */
                  /* get the SRC attribute of element IMAGE */
                  if (elType.ElTypeNum == HTML_EL_Object)
                    attrType.AttrTypeNum = HTML_ATTR_data;
                  else
                    attrType.AttrTypeNum = HTML_ATTR_SRC;
                  attr = TtaGetAttribute (el, attrType);
                  if (attr != NULL)
                    {
                      /* get a buffer for the SRC */
                      length = TtaGetTextAttributeLength (attr);
                      if (length > 0)
                        {
                          value = (char *)TtaGetMemory (MAX_LENGTH);
                          if (value != NULL)
                            {
                              /* get the SRC itself */
                              TtaGiveTextAttributeValue (attr, value, &length);
                              if (!strncmp (value, "internal:", 9))
                                ComputeSRCattribute (el, doc, originDocument,
                                                   attr, &value[9]);
                              else
                                /* update value and SRCattribute */
                                ComputeSRCattribute (el, doc, originDocument,
                                                     attr, value);
                            }
                          TtaFreeMemory (value);
                        }
                    }
                }
            }
        }
    }
#ifdef _SVG
  else if (!strcmp (name, "SVG") && elType.ElTypeNum == SVG_EL_SVG)
    {
      if (DocumentTypes[doc] != docSVG && DocumentMeta[doc])
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
      /* Set the SVG namespace declaration */
      TtaSetUriSSchema (elType.ElSSchema, SVG_URI);
      TtaSetANamespaceDeclaration (doc, el, NULL, SVG_URI);
      // mark the new Coordinate System
      TtaSetElCoordinateSystem (el);
    }
#endif /* _SVG */

  if (anchor)
    /* an anchor element has been pasted. Nested anchors are forbidden in HTML.
       Check if there is an anchor among the ancestors of the pasted element */
    {
      dispMode = TtaGetDisplayMode (doc);
      if (dispMode == DisplayImmediately)
        /* don't set NoComputedDisplay
           -> it breaks down views formatting when Enter generates new elements  */
        TtaSetDisplayMode (doc, DeferredDisplay);
      oldStructureChecking = TtaGetStructureChecking (doc);
      TtaSetStructureChecking (FALSE, doc);
      /* Is there an anchor ancestor? */
      ancestor = TtaGetTypedAncestor (el, elType);
      if (ancestor)
        /* nested anchors */
        {
          ok = FALSE;
          if (ancestor == TtaGetParent (el))
            /* the enclosing anchor is the parent of the pasted element */
            {
              sibling = el; TtaNextSibling (&sibling);
              if (!sibling)
                /* the pasted anchor is the last child of its parent */
                {
                  /* move the pasted anchor as the next sibling of its
                     parent */
                  TtaRemoveTree (el, doc);
                  TtaInsertSibling (el, ancestor, FALSE, doc);
                  ok = TRUE;
                }
              else
                {
                  sibling = el; TtaPreviousSibling (&sibling);
                  if (!sibling)
                    /* the pasted anchor is the first child of its parent */
                    {
                      /* move the pasted anchor as the previous sibling of its
                         parent */
                      TtaRemoveTree (el, doc);
                      TtaInsertSibling (el, ancestor, TRUE, doc);
                      ok = TRUE;
                    }
                }
            }
          if (!ok)
            {
              /* Move anchor children and delete the anchor element */
              child = TtaGetFirstChild (anchor);
              previous = child;
              TtaPreviousSibling (&previous);
              while (child)
                {
                  nextchild = child;
                  TtaNextSibling (&nextchild);
                  TtaRemoveTree (child, doc);
                  TtaInsertSibling (child, anchor, TRUE, doc);
                  /* if anchor is the pasted element, it has been registered
                     in the editing history for the Undo command.  It will be
                     deleted, so its children have to be registered too. */
                  if (anchor == el)
                    TtaRegisterElementCreate (child, doc);
                  child = nextchild;
                }
              TtaDeleteTree (anchor, doc);
              anchor = NULL;
            }
        }
      if (anchor)
        {
          /* the anchor element is allowed here */
          /* Change attributes HREF if the element comes from another */
          /* document */
          originDocument = (Document) position;
          if (originDocument >= 0 && originDocument != doc)
            {
              /* the anchor has moved from one document to another */
              /* get the HREF attribute of element Anchor */
              attrType.AttrSSchema = elType.ElSSchema;
              attrType.AttrTypeNum = HTML_ATTR_HREF_;
              attr = TtaGetAttribute (anchor, attrType);
              if (attr != NULL)
                ChangeURI (anchor, attr, originDocument, doc);
            }
        }
      TtaSetStructureChecking (oldStructureChecking, doc);
      /* Restore the display mode */
      if (dispMode == DisplayImmediately)
        TtaSetDisplayMode (doc, dispMode);
    }
}

/*----------------------------------------------------------------------
  ElementPasted
  This function is called for each element pasted by the user, and for
  each element within the pasted element.
  Check Pseudo paragraphs.
  If the pasted element has a NAME attribute, change its value if this
  NAME is already used in the document.
  If it's within the TITLE element, update the corresponding field in
  the Formatted window.
  ----------------------------------------------------------------------*/
void ElementPasted (NotifyElement * event)
{
  CheckPastedElement (event->element, event->document, event->info,
                      event->position, FALSE);
}

 
/*----------------------------------------------------------------------
  -----------------------------------------------------------------------*/ 
void CheckMaxLength (Element elText, Document doc)
{
  Element	          el;
  ElementType	      elType;
  AttributeType     attrType;
  Attribute         attr;
  CHAR_T           *buffer;
  Language          lang;
  int               length, lg;

  // check the maxlength attribute
  el = elText;
  elType = TtaGetElementType (el);
  while (el &&
         elType.ElTypeNum != HTML_EL_Text_Input &&
         elType.ElTypeNum != HTML_EL_Password_Input &&
         elType.ElTypeNum != HTML_EL_File_Input)
    {
      el = TtaGetParent (el);
      elType = TtaGetElementType (el);
    }
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = HTML_ATTR_MaxLength;
  attr = TtaGetAttribute (el, attrType);
  if (attr)
    {
      // maxlength value
      length = TtaGetAttributeValue (attr);
      lg =  TtaGetElementVolume (elText);
      if (lg > length)
        {
          buffer = (CHAR_T *)TtaGetMemory (lg * sizeof(CHAR_T));
          TtaGiveBufferContent (elText, buffer, lg-1, &lang);
          buffer[length] = EOS;
          TtaSetBufferContent (elText, buffer, lang, doc);
        }
    }
}


/*----------------------------------------------------------------------
  CheckNewLines
  Some new text has been pasted or typed in a text element.
  If it's not in a <pre>, check the NewLine characters and replace them by
  spaces. Replace a sequence of spaces by a single space.
  If its in a paragraph, generate a new paragraph for each newline (if
  mode PasteLineByLine is on), or for each pair of newlines (if this mode
  is off).
  -----------------------------------------------------------------------*/
void CheckNewLines (NotifyOnTarget *event)
{
  Element     ancestor, selEl, leaf, newLeaf, firstLeaf, firstParag;
  Element     el, child, orig, prev, next, parent;
  Document    doc;
  ElementType elType;
  CHAR_T      *content, *sibContent;
  int         firstSelChar, lastSelChar, length, i, j, sibLength, start;
  Language    lang;
  char       *name;
  ThotBool    pre, para, changed, selChanged, newParagraph, undoSeqExtended;
  ThotBool    prevCharEOL, pasteLineByLine, moveNext, insert_nbsp;

  if (!event->target)
    return;
  doc = event->document;

  if (DocumentTypes[doc] == docText || DocumentTypes[doc] == docCSS ||
      DocumentTypes[doc] == docSource || DocumentTypes[doc] == docLog)
    pre = FALSE;
  else
    {
      if (DocumentTypes[doc] == docHTML)
        // check maxlength of input elements
        CheckMaxLength (event->target, doc);
      TtaGetEnvBoolean ("PRESERVE_SPACE", &pre);
      if (pre)
        return;
    }
  leaf = event->target;
  length = TtaGetElementVolume (leaf);
  if (length == 0)
    return;
  firstLeaf = leaf;
  undoSeqExtended = FALSE;

  /* is there a preformatted or paragraph (or equivalent) ancestor? */
  pre = FALSE;
  para = FALSE;
  newParagraph = FALSE;
  firstParag = NULL;
  ancestor = TtaGetParent (leaf);
  while (ancestor && !pre && !para)
    {
      if (TtaIsElementWithSpacePreserve (ancestor))
        pre = TRUE;
      else
        {
          elType = TtaGetElementType (ancestor);
          name = TtaGetSSchemaName (elType.ElSSchema);
          if (!strcmp(name, "HTML"))
            {
              if (elType.ElTypeNum == HTML_EL_STYLE_ ||
                  elType.ElTypeNum == HTML_EL_SCRIPT_ ||
                  elType.ElTypeNum == HTML_EL_Preformatted ||
                  elType.ElTypeNum == HTML_EL_Text_Area)
                pre = TRUE;
              else if (elType.ElTypeNum == HTML_EL_Paragraph ||
                       elType.ElTypeNum == HTML_EL_Pseudo_paragraph)
                {
                  para = TRUE;
                  firstParag = ancestor;
                }
              else
                ancestor = TtaGetParent (ancestor);
            }
          else if (!strcmp(name, "TextFile"))
            {
              TtaGiveFirstSelectedElement (doc, &selEl, &firstSelChar, &lastSelChar);
              if (selEl != leaf)
                /* the current selection is not within this element. Don't care about
                   the selection */
                selEl = NULL;
              content = (CHAR_T *)TtaGetMemory ((length + 1) * sizeof(CHAR_T));
              TtaGiveBufferContent (leaf, content, length, &lang);
              content[length] = EOS;
              j = 0;
              prev = NULL;
              // get information about next leaves
              next = leaf;
              TtaNextSibling (&next);
              moveNext = FALSE;
              for (i = 0; i < length; i++)
                {
                  if (content[i] == EOL)
                    {
                      /* the current character is a newline */
                      content[i] = EOS;
                      TtaSetBufferContent (leaf, &content[j], lang, doc);
                      // generate a new line
                      el = TtaNewTree (doc, elType, "");
                      TtaInsertSibling (el, ancestor, FALSE, doc);
                      if (!undoSeqExtended)
                        {
                          TtaExtendUndoSequence (doc);
                          undoSeqExtended = TRUE;
                        }
                      if (prev)
                        TtaRegisterElementCreate (prev, doc);
                      // next leaves should be moved into the last line
                      moveNext = TRUE;
                      leaf = TtaGetFirstChild (el);
                      prev = ancestor = el;
                      j = i + 1;
                    }
                }

              if (prev)
                {
                  TtaSetBufferContent (leaf, &content[j], lang, doc);
                  TtaRegisterElementCreate (prev, doc);
                  if (moveNext)
                    {
                      /* move next leaves into the last created line */
                      el = leaf;
                      while (next)
                        {
                          prev = next;
                          TtaNextSibling (&next);
                          TtaRegisterElementDelete (prev, doc);
                          TtaRemoveTree (prev, doc);
                          TtaInsertSibling (prev, el, FALSE, doc);
                          TtaRegisterElementCreate (prev, doc);
                          el = prev;
                        }
                    }
                  TtaSelectString (doc, leaf, i, i-1);
                }
              TtaFreeMemory (content);
              return;
            }
          else if (!strcmp(name, "Template"))
            /* just skip ancestors that are template elements */
            ancestor = TtaGetParent (ancestor);
          else
            ancestor = NULL;  /* not an HTML element */
        }
    }
  if (pre)
    /* there is a <PRE> ancestor. Don't change anything */
    return;

  /* get the user value for the Paste-Line-By-Line option */
  TtaGetEnvBoolean ("PASTE_LINE_BY_LINE", &pasteLineByLine);
  /* get the user value for the Insert-nbsp option */
  TtaGetEnvBoolean ("INSERT_NBSP", &insert_nbsp);

  /* replace every new line in the content of the element by a space
     and replace every sequence of spaces by a single space */
  TtaGiveFirstSelectedElement (doc, &selEl, &firstSelChar, &lastSelChar);
  if (selEl != leaf)
    /* the current selection is not within this element. Don't care about
       the selection */
    selEl = NULL;
  elType = TtaGetElementType (leaf);
  if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
    {
      length = TtaGetElementVolume (leaf);
      content = (CHAR_T *)TtaGetMemory ((length + 1) * sizeof(CHAR_T));
      TtaGiveBufferContent (leaf, &content[0], length, &lang);
      content[length] = EOS;
    }
  else
    {
      length = 0;
      content = NULL;
    }
  changed = FALSE;
  selChanged = FALSE;
  prevCharEOL = FALSE;
  /* is there a previous sibling element? */
  prev = leaf;
  TtaPreviousSibling (&prev);
  j = 0;
  for (i = 0; i < length; i++)
    {
      if (content[i] == EOL)
        /* the current character is a newline */
        {
          if (!para)
            /* We are not within a paragraph. Replace the newline by a space */
            {
              content[i] = SPACE;
              changed = TRUE;
            }
          else
            /* The pasted text is within a paragraph */
            {
              if (!pasteLineByLine && !prevCharEOL)
                /* in this mode (!pasteLineByLine), create a paragraph only
                   when there are two newlines (possibly with some spaces) */
                {
                  content[i] = SPACE;
                  changed = TRUE;
                }
              else
                /* Break the current paragraph */
                {
                  /* create new elements to duplicate the ancestors of the
                     leaf until the paragraph (included) */
                  child = NULL; newLeaf = NULL; prev = NULL;
                  orig = leaf;
                  while (orig)
                    {
                      el = TtaCopyElement (orig, doc, doc, TtaGetParent(orig));
                      elType = TtaGetElementType (orig);
                      if (elType.ElTypeNum == HTML_EL_Pseudo_paragraph)
                        TtaChangeElementType (el, HTML_EL_Paragraph);
                      if (orig == leaf)
                        newLeaf = el;
                      if (prev)
                        TtaInsertFirstChild (&prev, el, doc);
                      prev = el;
                      if (orig == ancestor)
                        orig = NULL;
                      else
                        orig = TtaGetParent (orig);
                    }
                  TtaInsertSibling (el, ancestor, FALSE, doc);
                  newParagraph = TRUE;
                  /* extend the previous undo sequence if it's not done */
                  if (!undoSeqExtended)
                    {
                      TtaExtendUndoSequence (doc);
                      undoSeqExtended = TRUE;
                    }
                  TtaRegisterElementCreate (el, doc);
                  if (j > 0 && content[j-1] == SPACE)
                    /* remove trailing space */
                    {
                      j--;
                      if (selEl)
                        if (firstSelChar >= j)
                          /* The selection is after the current position.
                             Update it */
                          {
                            firstSelChar--;
                            selChanged = TRUE;
                          }
                    }
                  content[j] = EOS;
                  content[i] = EOS;
                  TtaSetBufferContent (leaf, content, lang, doc);
                  if (selChanged && firstSelChar <= j)
                    /* update the current selection */
                    {
                      TtaSelectString (doc, leaf, firstSelChar,firstSelChar-1);
                      selChanged = FALSE;
                      selEl = NULL;
                    }
                  else if (selEl)
                    /* the current selection is further in the element */
                    {
                      /* substract the characters moved to the new element and
                         the EOL */
                      firstSelChar-= j; 
                      firstSelChar--;
                      selChanged = TRUE;
                    }
                  leaf = newLeaf;
                  j = 0;
                  changed = TRUE;
                  ancestor = el;
                }
              prevCharEOL = TRUE;
            }
        }
      if (content[i] == SPACE)
        /* this is a space */
        {
          if (j == 0)
            /* beginning of the text element */
            {
              /* if this space is after a newline, remove it */
              if (insert_nbsp && prev && TtaGetLastBufferContent (prev) == SPACE)
                {
                 /* replace the previous space by a nbsp */
                  sibLength = TtaGetElementVolume (prev);
                  sibLength+= 2;
                  sibContent = (CHAR_T *)TtaGetMemory (sibLength * sizeof(CHAR_T));
                  TtaGiveBufferContent (prev, sibContent, sibLength-1, &lang);
                  /* insert a space */
                  if (!undoSeqExtended)
                    {
                      TtaExtendUndoSequence (doc);
                      undoSeqExtended = TRUE;
                    }
                  TtaRegisterElementReplace (prev, doc);
                  sibContent[sibLength-1] = NBSP;
                  TtaSetBufferContent (prev, sibContent, lang, doc);
               }
              else if (prevCharEOL ||
                  (prev && TtaGetLastBufferContent (prev) == SPACE))
                {
                  changed = TRUE;
                  if (selEl)
                    /* the selection is in this piece of text */
                    if (firstSelChar >= j)
                      /* it is after the current position. Update it */
                      {
                        firstSelChar--;
                        selChanged = TRUE;
                      }
                }
              else if (insert_nbsp || prev)
                /* Keep that space */
                {
                  if (i > j)
                    content[j] = content[i];
                  j++;
                }
              else
                // a space is removed
                changed = TRUE;
            }
          else
            {
              if (content[j-1] != SPACE)
                {
                  // the previous character is not a space nor a newline.
                  if (i + 1 < length && content[i+1] == NBSP)
                    {
                    content[j++] = NBSP;
                    changed = TRUE;
                    }
                  else
                    // keep that space
                    content[j++] = SPACE;
                }
              else if (insert_nbsp)
                {
                  /* replace the previous space by a nbsp */
                  content[j-1] = NBSP;
                  content[j++] = SPACE;
                  changed = TRUE;
                }
              else
                /* the previous character is a space or a new line.
                   Remove the current space */
                {
                  changed = TRUE;
                  if (selEl)
                    /* the selection is in this piece of text */
                    if (firstSelChar >= j)
                      /* it is after the current position. Update it */
                      {
                        firstSelChar--;
                        selChanged = TRUE;
                      }
                }
            }
        }
      else
        {
          if (content[i] != EOS)
            /* an ordinary character. Keep it */
            {
              if (i > j)
                /* some characters have been deleted. Move this character */
                content[j] = content[i];
              j++;
              prevCharEOL = FALSE;
            }
        }
    }

  start = 0;
  /* all the content of the modified element has now been processed */
  /* if we are within an inline element and there is a space at the beginning
     or at the end of the text element, tries to move that space to the
     previous (resp. next) text element */
  parent = TtaGetParent (leaf);
  elType = TtaGetElementType (parent);
  if (elType.ElTypeNum == HTML_EL_Anchor ||
      elType.ElTypeNum == HTML_EL_Font_ ||
      elType.ElTypeNum == HTML_EL_Subscript ||
      elType.ElTypeNum == HTML_EL_Superscript ||
      elType.ElTypeNum == HTML_EL_Span ||
      elType.ElTypeNum == HTML_EL_BDO ||
      elType.ElTypeNum == HTML_EL_Teletype_text ||
      elType.ElTypeNum == HTML_EL_Italic_text ||
      elType.ElTypeNum == HTML_EL_Bold_text ||
      elType.ElTypeNum == HTML_EL_Underlined_text ||
      elType.ElTypeNum == HTML_EL_Struck_text ||
      elType.ElTypeNum == HTML_EL_Big_text ||
      elType.ElTypeNum == HTML_EL_Small_text ||
      elType.ElTypeNum == HTML_EL_Emphasis ||
      elType.ElTypeNum == HTML_EL_Strong ||
      elType.ElTypeNum == HTML_EL_Def ||
      elType.ElTypeNum == HTML_EL_Code ||
      elType.ElTypeNum == HTML_EL_Sample ||
      elType.ElTypeNum == HTML_EL_Keyboard ||
      elType.ElTypeNum == HTML_EL_Variable_ ||
      elType.ElTypeNum == HTML_EL_Cite ||
      elType.ElTypeNum == HTML_EL_ABBR ||
      elType.ElTypeNum == HTML_EL_ACRONYM ||
      elType.ElTypeNum == HTML_EL_ins||
      elType.ElTypeNum == HTML_EL_del ||
      elType.ElTypeNum == HTML_EL_rb)
    /* we are within an inline element */
    {
      next = leaf;
      TtaNextSibling (&next);
      if (j > 0 && content[j-1] == SPACE && !next)
        /* the text element has a trailing space and no following sibling */
        /* try to move the trailing space to the beginning of the following
           text leaf */
        {
          next = parent;
          if (elType.ElTypeNum == HTML_EL_rb)
            next = TtaGetParent (parent);
          TtaNextSibling (&next);
          if (next &&
              TtaGetElementType(next).ElTypeNum == HTML_EL_TEXT_UNIT)
            {
              /* the next sibling of the parent element is a character string*/
              /* remove the trailing space */
              j--;
              changed = TRUE;
              if (selEl)
                if (firstSelChar >= j)
                  /* The selection is after the current position. Update it */
                  {
                    firstSelChar--;
                    selChanged = TRUE;
                  }
              /* check the next character string */
              sibLength = TtaGetElementVolume (next);
              sibLength+= 2;
              sibContent = (CHAR_T *)TtaGetMemory (sibLength * sizeof(CHAR_T));
              sibContent[1] = EOS;
              TtaGiveBufferContent (next, &sibContent[1], sibLength-1, &lang);
              if (sibContent[1] != SPACE)
                /* no space at the beginning of the next text element */
                {
                  /* insert a space */
                  if (!undoSeqExtended)
                    {
                      TtaExtendUndoSequence (doc);
                      undoSeqExtended = TRUE;
                    }
                  TtaRegisterElementReplace (next, doc);
                  sibContent[0] = SPACE;
                  TtaSetBufferContent (next, sibContent, lang, doc);
                }
              TtaFreeMemory (sibContent);
            }
        }
      prev = leaf;
      TtaPreviousSibling (&prev);
      if (content[0] == SPACE && !prev)
        /* the text element has a leading space and no previous sibling */
        /* try to move the leading space to the end of the preceding text
           leaf */
        {
          prev = parent;
          if (elType.ElTypeNum == HTML_EL_rb)
            prev = TtaGetParent (parent);
          TtaPreviousSibling (&prev);
          if (prev &&
              TtaGetElementType(prev).ElTypeNum == HTML_EL_TEXT_UNIT)
            {
              /* the previous sibling of the parent element is a character
                 string. Remove the leading space */
              start = 1;
              changed = TRUE;
              if (selEl)
                /* The selection is after the current position.
                   Update it */
                {
                  firstSelChar--;
                  selChanged = TRUE;
                }
              /* check the end of the previous character string */
              sibLength = TtaGetElementVolume (prev);
              sibLength+= 2;
              sibContent = (CHAR_T *)TtaGetMemory (sibLength * sizeof(CHAR_T));
              TtaGiveBufferContent (prev, sibContent, sibLength-1, &lang);
              if (sibLength > 2 && sibContent[sibLength-3] != SPACE)
                {
                  /* insert a space at the end of the previous text element */
                  if (!undoSeqExtended)
                    {
                      TtaExtendUndoSequence (doc);
                      undoSeqExtended = TRUE;
                    }
                  TtaRegisterElementReplace (prev, doc);
                  sibContent[sibLength-2] = SPACE;
                  sibContent[sibLength-1] = EOS;
                  TtaSetBufferContent (prev, sibContent, lang, doc);
                }
              TtaFreeMemory (sibContent);
            }
        }
    }

  if (j < length)
    content[j] = EOS;
  if (changed)
    /* we have made changes in the text buffer, update the element */
    {
      TtaSetBufferContent (leaf, &content[start], lang, doc);
      if (selChanged)
        /* update the current selection */
        TtaSelectString (doc, leaf, firstSelChar, firstSelChar-1);
    }
  TtaFreeMemory (content);

  if (newParagraph)
    /* New paragraphs have been created. Move the content that follows
       the pasted text from the initial paragraph to the last one created */
    {
      orig = firstLeaf;
      prev = leaf;
      while (orig)
        {
          el = orig; 
          TtaNextSibling (&el);
          while (el)
            {
              next = el;
              TtaNextSibling (&next);
              TtaRegisterElementDelete (el, doc);
              TtaRemoveTree (el, doc);
              TtaInsertSibling (el, prev, FALSE, doc);
              prev = el;
              el = next;
            }
          orig = TtaGetParent (orig);
          if (orig == firstParag)
            orig = NULL;
          else
            prev = TtaGetParent (prev);
        }
    }
  /* Close the latest undo sequence if it has been extended */
  if (undoSeqExtended)
    TtaCloseUndoSequence (doc);
}

/*----------------------------------------------------------------------
  CreateTarget
  Create a target element.                          
  ----------------------------------------------------------------------*/
void CreateTarget (Document doc, View view)
{
  Element             el;
  int                 firstSelectedChar, i;

  TtaGiveFirstSelectedElement (doc, &el, &firstSelectedChar, &i);
  if (el == NULL)
    TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_NO_INSERT_POINT);
  CreateAnchor (doc, view, FALSE);
}

/*----------------------------------------------------------------------
  UpdateAttrNAME
  A NAME attribute has been created or modified.
  If it's a creation or modification, check that the ID is a unique name
  in the document.
  If it's a deletion for a SPAN element, remove that element if it's
  not needed.
  ----------------------------------------------------------------------*/
void UpdateAttrNAME (NotifyAttribute * event)
{
  ElementType         elType;

  if (DocumentMeta[event->document] && DocumentMeta[event->document]->xmlformat)
    {
      // check valid value
      elType = TtaGetElementType (event->element);
      if (!strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") &&
          (elType.ElTypeNum == HTML_EL_Anchor ||
           elType.ElTypeNum == HTML_EL_MAP ||
           elType.ElTypeNum == HTML_EL_map))
        TtaIsValidID (event->attribute, TRUE);
#ifdef TEMPLATES
      else if (!strcmp(TtaGetSSchemaName (elType.ElSSchema), "Template"))
        TtaIsValidID (event->attribute, TRUE);
#endif /* TEMPLATES */
    }
  //else
    MakeUniqueName (event->element, event->document, TRUE, TRUE);
}

/*----------------------------------------------------------------------
  UpdateAttrID
  An ID attribute has been created, modified or deleted.
  If it's a creation or modification, check that the ID is a unique name
  in the document.
  If it's a deletion for a SPAN element, remove that element if it's
  not needed.
  ----------------------------------------------------------------------*/
void UpdateAttrID (NotifyAttribute * event)
{
  Element	firstChild, lastChild;

  if (event->event == TteAttrDelete)
    /* if the element is a SPAN without any other attribute, remove the SPAN
       element */
    DeleteSpanIfNoAttr (event->element, event->document, &firstChild,
                        &lastChild);
  else
    {
      MakeUniqueName (event->element, event->document, TRUE, TRUE);
      if (event->event == TteAttrCreate)
        /* if the ID attribute is on a text string, create a SPAN element that
           encloses this text string and move the ID attribute to that SPAN
           element */
        AttrToSpan (event->element, event->attribute, event->document);
    }
#ifdef _SVG
  Update_element_id_on_timeline (event); 
#endif /* _SVG */
}


/*----------------------------------------------------------------------
  CoordsModified
  Update x_ccord, y_coord, width, height or polyline according to the
  new coords value.             
  ----------------------------------------------------------------------*/
void CoordsModified (NotifyAttribute * event)
{
  ParseAreaCoords (event->element, event->document);
}


/*----------------------------------------------------------------------
  GraphicsModified
  Update coords attribute value according to the new coord value.
  ----------------------------------------------------------------------*/
void GraphicsModified (NotifyAttribute * event)
{
  Element             el;
  ElementType         elType;
  AttributeType       attrType;
  Attribute	          attr;
  char		            buffer[15];

  el = event->element;
  elType = TtaGetElementType (el);
  if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT)
    {
      /* update the associated map */
      if (event->attributeType.AttrTypeNum == HTML_ATTR_IntWidthPxl)
        {
          UpdateImageMap (el, event->document, OldWidth, -1);
          OldWidth = -1;
          /* update attribute Width__ */
          attrType.AttrSSchema = event->attributeType.AttrSSchema;
          attrType.AttrTypeNum = HTML_ATTR_Width__;
          attr = TtaGetAttribute (el, attrType);
          if (attr)
            {
              sprintf (buffer, "%d",
                       TtaGetAttributeValue (event->attribute));
              TtaSetAttributeText (attr, buffer, el, event->document);
            }
        }
      else if (event->attributeType.AttrTypeNum == HTML_ATTR_IntHeightPxl)
        {
          UpdateImageMap (el, event->document, -1, OldHeight);
          OldHeight = -1;
          /* update attribute Height_ */
          attrType.AttrSSchema = event->attributeType.AttrSSchema;
          attrType.AttrTypeNum = HTML_ATTR_Height_;
          attr = TtaGetAttribute (el, attrType);
          if (attr)
            {
              sprintf (buffer, "%d",
                       TtaGetAttributeValue (event->attribute));
              TtaSetAttributeText (attr, buffer, el, event->document);
            }
        }
    }
  else
    {
      if (elType.ElTypeNum != HTML_EL_AREA)
        {
          el = TtaGetParent (el);
          elType = TtaGetElementType (el);
        }
      if (elType.ElTypeNum == HTML_EL_AREA)
        SetAreaCoords (event->document, el,
                       event->attributeType.AttrTypeNum, NULL);
    }
}

/*----------------------------------------------------------------------
  StoreWidth
  Attribute IntWidthPxl will be changed, store the old value.
  ----------------------------------------------------------------------*/
ThotBool StoreWidth (NotifyAttribute *event)
{
  ElementType	     elType;
  int                h;

  elType = TtaGetElementType (event->element);
  if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT)
    TtaGiveBoxSize (event->element, event->document, 1, UnPixel, &OldWidth,&h);
  else
    OldWidth = -1;
  return FALSE;		/* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
  StoreHeight
  Attribute height_ will be changed, store the old value.
  ----------------------------------------------------------------------*/
ThotBool StoreHeight (NotifyAttribute * event)
{
  ElementType	     elType;
  int                w;

  elType = TtaGetElementType (event->element);
  if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT)
    TtaGiveBoxSize (event->element, event->document, 1, UnPixel, &w,
                    &OldHeight);
  else
    OldHeight = -1;
  return FALSE;		/* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
  AttrHeightDelete
  An attribute Height_ will be deleted.   
  Delete the corresponding attribute IntHeightPercent or   
  IntHeightPxl.                                            
  ----------------------------------------------------------------------*/
ThotBool AttrHeightDelete (NotifyAttribute *event)
{
  AttributeType       attrType;
  Attribute           attr;
  ElementType         elType, childType;
  Element             el, child;

  StoreHeight (event);
  el = event->element;
  elType = TtaGetElementType (el);
  if (elType.ElTypeNum == HTML_EL_Object)
    /* the height attribute is attached to an Object element */
    {
      child = TtaGetFirstChild (el);
      if (child)
        {
          childType = TtaGetElementType (child);
          if (childType.ElTypeNum == HTML_EL_PICTURE_UNIT)
            /* the Object element is of type image. Apply the height
               attribute to the actual image element */
            el = child;
        }
    }
  attrType = event->attributeType;
  attrType.AttrTypeNum = HTML_ATTR_IntHeightPxl;
  attr = TtaGetAttribute (el, attrType);
  if (attr == NULL)
    {
      attrType.AttrTypeNum = HTML_ATTR_IntHeightPercent;
      attr = TtaGetAttribute (el, attrType);
    }
  if (attr != NULL)
    TtaRemoveAttribute (el, attr, event->document);
  return FALSE;		/* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
  AttrHeightModifed
  An attribute Height_ has been created or modified.
  Create the corresponding attribute IntHeightPercent or IntHeightPxl.
  ----------------------------------------------------------------------*/
void AttrHeightModified (NotifyAttribute *event)
{
  char               *buffer;
  int                 length;

  length = buflen - 1;
  buffer = (char *)TtaGetMemory (buflen);
  TtaGiveTextAttributeValue (event->attribute, buffer, &length);
  CreateAttrHeightPercentPxl (buffer, event->element, event->document,
                              OldHeight);
  TtaFreeMemory (buffer);
  OldHeight = -1;
}

/*----------------------------------------------------------------------
  AttrWidthDelete
  An attribute Width__ will be deleted.   
  Delete the corresponding attribute IntWidthPercent or   
  IntWidthPxl.                                            
  ----------------------------------------------------------------------*/
ThotBool AttrWidthDelete (NotifyAttribute *event)
{
  AttributeType       attrType;
  Attribute           attr;
  ElementType         elType, childType;
  Element             el, child;

  StoreWidth (event);
  el = event->element;
  elType = TtaGetElementType (el);
  if (elType.ElSSchema == TtaGetSSchema ("HTML", event->document))
    {
      if (elType.ElTypeNum == HTML_EL_COL ||
          elType.ElTypeNum == HTML_EL_COLGROUP)
        TransmitWidthToColhead (el, event->document, NULL, -1);
      else if (elType.ElTypeNum == HTML_EL_Object)
        /* the width attribute is attached to an Object element */
        {
          child = TtaGetFirstChild (el);
          if (child)
            {
              childType = TtaGetElementType (child);
              if (childType.ElTypeNum == HTML_EL_PICTURE_UNIT)
                /* the Object element is of type image. Apply the width
                   attribute to the actual image element */
                el = child;
            }
        }
      attrType.AttrSSchema = event->attributeType.AttrSSchema;
      attrType.AttrTypeNum = HTML_ATTR_IntWidthPxl;
      attr = TtaGetAttribute (el, attrType);
      if (attr == NULL)
        {
          attrType.AttrTypeNum = HTML_ATTR_IntWidthPercent;
          attr = TtaGetAttribute (el, attrType);
        }
      if (attr)
        TtaRemoveAttribute (el, attr, event->document);
    }
  return FALSE;		/* let Thot perform normal operation */
}


/*----------------------------------------------------------------------
  AttrWidthModifed
  An attribute Width__ has been created or modified.
  Create the corresponding attribute IntWidthPercent or IntWidthPxl.
  ----------------------------------------------------------------------*/
void AttrWidthModified (NotifyAttribute *event)
{
  ElementType         elType;
  char               *buffer;
  int                 length;

  length = buflen - 1;
  buffer = (char *)TtaGetMemory (buflen);
  TtaGiveTextAttributeValue (event->attribute, buffer, &length);
  CreateAttrWidthPercentPxl (buffer, event->element, event->document,
                             OldWidth);
  elType = TtaGetElementType (event->element);
  if (elType.ElSSchema == TtaGetSSchema ("HTML", event->document) &&
       (elType.ElTypeNum == HTML_EL_COL ||
        elType.ElTypeNum == HTML_EL_COLGROUP))
    TransmitWidthToColhead (event->element, event->document, buffer, -1);
  TtaFreeMemory (buffer);
  OldWidth = -1;
}

/*----------------------------------------------------------------------
  AttrAreaSizeDelete
  An attribute Area_Size (size of an input area) will be deleted.   
  Delete the corresponding attribute IntAreaSize.
  ----------------------------------------------------------------------*/
ThotBool AttrAreaSizeDelete (NotifyAttribute *event)
{
  CreateAttrIntAreaSize (20, event->element, event->document);
  return FALSE;		/* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
  AttrAreaSizeModifed
  An attribute Area_Size (size of an input area) has been created or modified.
  Create the corresponding attribute IntAreaSize.
  ----------------------------------------------------------------------*/
void AttrAreaSizeModified (NotifyAttribute *event)
{
  int                 value;

  value = TtaGetAttributeValue (event->attribute);
  CreateAttrIntAreaSize (value, event->element, event->document);
}

/*----------------------------------------------------------------------
  AttrFontSizeCreated
  an HTML attribute "size" has been created for a Font element.   
  Create the corresponding internal attribute.                    
  ----------------------------------------------------------------------*/
void AttrFontSizeCreated (NotifyAttribute *event)
{
  char               *buffer = (char *)TtaGetMemory (buflen);
  int                 length;
  DisplayMode         dispMode;

  dispMode = TtaGetDisplayMode (event->document);
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (event->document, DeferredDisplay);

  length = buflen - 1;
  TtaGiveTextAttributeValue (event->attribute, buffer, &length);
  CreateAttrIntSize (buffer, event->element, event->document);
  TtaSetDisplayMode (event->document, dispMode);
  TtaFreeMemory (buffer);
}

/*----------------------------------------------------------------------
  AttrFontSizeDelete
  An HTML attribute "size" has been deleted for a Font element.   
  Delete the corresponding internal attribute.                    
  ----------------------------------------------------------------------*/
ThotBool AttrFontSizeDelete (NotifyAttribute * event)
{
  AttributeType       attrType;
  Attribute           attr;

  attrType = event->attributeType;
  attrType.AttrTypeNum = HTML_ATTR_IntSizeIncr;
  attr = TtaGetAttribute (event->element, attrType);
  if (attr == NULL)
    {
      attrType.AttrTypeNum = HTML_ATTR_IntSizeDecr;
      attr = TtaGetAttribute (event->element, attrType);
    }
  if (attr == NULL)
    {
      attrType.AttrTypeNum = HTML_ATTR_IntSizeRel;
      attr = TtaGetAttribute (event->element, attrType);
    }
  if (attr != NULL)
    TtaRemoveAttribute (event->element, attr, event->document);
  return FALSE;		/* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
  AttrColorCreated
  An attribute color, TextColor or BackgroundColor has been       
  created or modified.                                            
  ----------------------------------------------------------------------*/
void AttrColorCreated (NotifyAttribute * event)
{
  char            *value = (char *)TtaGetMemory (buflen);
  int              length;

  value[0] = EOS;
  length = TtaGetTextAttributeLength (event->attribute);
  if (length >= buflen)
    length = buflen - 1;
  if (length > 0)
    TtaGiveTextAttributeValue (event->attribute, value, &length);

  if (event->attributeType.AttrTypeNum == HTML_ATTR_BackgroundColor)
    HTMLSetBackgroundColor (event->document, event->element, 2000, value);
  else if (event->attributeType.AttrTypeNum == HTML_ATTR_background_)
    HTMLSetBackgroundImage (event->document, event->element, REPEAT,
                            2000, value, FALSE);
  else if (event->attributeType.AttrTypeNum == HTML_ATTR_color ||
           event->attributeType.AttrTypeNum == HTML_ATTR_TextColor)
    HTMLSetForegroundColor (event->document, event->element, 2000, value);
  else if (event->attributeType.AttrTypeNum == HTML_ATTR_LinkColor)
    HTMLSetAlinkColor (event->document, event->element, value);
  else if (event->attributeType.AttrTypeNum == HTML_ATTR_VisitedLinkColor)
    HTMLSetAvisitedColor (event->document, event->element, value);
  else if (event->attributeType.AttrTypeNum == HTML_ATTR_ActiveLinkColor)
    HTMLSetAactiveColor (event->document, event->element, value);
  TtaFreeMemory (value);
}


/*----------------------------------------------------------------------
  AttrColorDelete
  An attribute color, TextColor or BackgroundColor is being       
  deleted.                                                        
  ----------------------------------------------------------------------*/
ThotBool AttrColorDelete (NotifyAttribute *event)
{
  if (event->attributeType.AttrTypeNum == HTML_ATTR_BackgroundColor)
    HTMLResetBackgroundColor (event->document, event->element);
  else if (event->attributeType.AttrTypeNum == HTML_ATTR_background_)
    HTMLResetBackgroundImage (event->document, event->element);
  else if (event->attributeType.AttrTypeNum == HTML_ATTR_color ||
           event->attributeType.AttrTypeNum == HTML_ATTR_TextColor)
    HTMLResetForegroundColor (event->document, event->element);
  else if (event->attributeType.AttrTypeNum == HTML_ATTR_LinkColor)
    HTMLResetAlinkColor (event->document, event->element);
  else if (event->attributeType.AttrTypeNum == HTML_ATTR_VisitedLinkColor)
    HTMLResetAvisitedColor (event->document, event->element);
  else if (event->attributeType.AttrTypeNum == HTML_ATTR_ActiveLinkColor)
    HTMLResetAactiveColor (event->document, event->element);
  return FALSE;		/* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
  GlobalAttrInMenu
  Called by Thot when building the Attributes menu.
  Prevent Thot from including a global attribute in the menu if the selected
  element does not accept this attribute.
  ----------------------------------------------------------------------*/
ThotBool GlobalAttrInMenu (NotifyAttribute * event)
{
  ElementType   elType, parentType;
  Element       parent;
  char         *attr;
  ThotBool      edit_xmlid;

  elType = TtaGetElementType (event->element);

  /* don't put any HTML attribute on Thot elements that are not
     HTML elements */
  if (elType.ElTypeNum == HTML_EL_Invalid_element ||
      elType.ElTypeNum == HTML_EL_Unknown_namespace ||
      elType.ElTypeNum == HTML_EL_XHTML_Unknown_namespace ||
      elType.ElTypeNum == HTML_EL_Comment_ ||
      elType.ElTypeNum == HTML_EL_Comment_line ||
      elType.ElTypeNum == HTML_EL_XMLPI ||
      elType.ElTypeNum == HTML_EL_PI_line ||
      elType.ElTypeNum == HTML_EL_DOCTYPE ||
      elType.ElTypeNum == HTML_EL_DOCTYPE_line)
    return TRUE;

  /* don't put any attribute on text fragments that are within DOCTYPE,
     comments, PIs, etc. */
  if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
    {
      parent = TtaGetParent (event->element);
      if (parent)
        {
          parentType = TtaGetElementType (parent);
          if (parentType.ElTypeNum == HTML_EL_Invalid_element ||
              parentType.ElTypeNum == HTML_EL_Unknown_namespace ||
              parentType.ElTypeNum == HTML_EL_XHTML_Unknown_namespace ||
              parentType.ElTypeNum == HTML_EL_Comment_ ||
              parentType.ElTypeNum == HTML_EL_Comment_line ||
              parentType.ElTypeNum == HTML_EL_XMLPI ||
              parentType.ElTypeNum == HTML_EL_PI_line ||
              parentType.ElTypeNum == HTML_EL_DOCTYPE ||
              parentType.ElTypeNum == HTML_EL_DOCTYPE_line)
            return TRUE;
        }
    }

  attr = (char*)GetXMLAttributeName (event->attributeType, elType, event->document);
  if (attr[0] == EOS)
    return TRUE;	/* don't put an invalid attribute in the menu */

  /* do we have to show xml:id in the menu? */
  if (DocumentTypes[event->document] == docXml)
    edit_xmlid = TRUE;
  else
    TtaGetEnvBoolean ("SHOW_XMLID", &edit_xmlid);

  if (!edit_xmlid && event->attributeType.AttrTypeNum == HTML_ATTR_xmlid)
    return TRUE;	/* don't show xml:id in the menu */
  else if ((DocumentMeta[event->document] == NULL ||
            !DocumentMeta[event->document]->xmlformat) &&
           (event->attributeType.AttrTypeNum == HTML_ATTR_xml_space ||
            event->attributeType.AttrTypeNum == HTML_ATTR_xmlid))
    return TRUE;	/* don't put xml attributes in the menu */


  if (event->attributeType.AttrTypeNum == HTML_ATTR_REL ||
      event->attributeType.AttrTypeNum == HTML_ATTR_REV)
    {
      if (TtaGetDocumentExtraProfile (event->document) != L_RDFa)
	/* REL et REV are global for XHTML+RDFa documents only */
	return FALSE;
      else
	{
	  event->restr.RestrEnumVal = TtaStrdup(REL_REV_Attr_Values);
	}
    }
  
  /* handle only Global attributes */
  if (event->attributeType.AttrTypeNum != HTML_ATTR_ID &&
      event->attributeType.AttrTypeNum != HTML_ATTR_Class &&
      event->attributeType.AttrTypeNum != HTML_ATTR_Style_ &&
      event->attributeType.AttrTypeNum != HTML_ATTR_Title &&
      event->attributeType.AttrTypeNum != HTML_ATTR_dir &&
      event->attributeType.AttrTypeNum != HTML_ATTR_onclick &&
      event->attributeType.AttrTypeNum != HTML_ATTR_ondblclick &&
      event->attributeType.AttrTypeNum != HTML_ATTR_onmousedown &&
      event->attributeType.AttrTypeNum != HTML_ATTR_onmouseup &&
      event->attributeType.AttrTypeNum != HTML_ATTR_onmouseover &&
      event->attributeType.AttrTypeNum != HTML_ATTR_onmousemove &&
      event->attributeType.AttrTypeNum != HTML_ATTR_onmouseout &&
      event->attributeType.AttrTypeNum != HTML_ATTR_onkeypress &&
      event->attributeType.AttrTypeNum != HTML_ATTR_onkeydown &&
      event->attributeType.AttrTypeNum != HTML_ATTR_onkeyup &&
      event->attributeType.AttrTypeNum != HTML_ATTR_xml_space)
#ifdef TEMPLATES
    /* it's not a global attribute. Accept it */
    return CheckTemplateAttrInMenu(event);
#else /* TEMPLATES */
  /* it's not a global attribute. Accept it */
    return FALSE;
#endif /* TEMPLATES */
    
    if (strcmp (TtaGetSSchemaName (elType.ElSSchema),"HTML"))
      /* it's not a HTML element */
      return TRUE;
    else
      /* it's a HTML element */
      {
	/* BASE and SCRIPT do not accept any global attribute */
      if (elType.ElTypeNum == HTML_EL_BASE ||
          elType.ElTypeNum == HTML_EL_SCRIPT_ ||
          elType.ElTypeNum == HTML_EL_Element)
        return TRUE;
      /* BASEFONT and PARAM accept only ID */
      else if (elType.ElTypeNum == HTML_EL_BaseFont ||
               elType.ElTypeNum == HTML_EL_Parameter)
        {
          if (event->attributeType.AttrTypeNum == HTML_ATTR_ID)
#ifdef TEMPLATES
            return CheckTemplateAttrInMenu(event);
#else /* TEMPLATES */
            return FALSE;
#endif /* TEMPLATES */
          else
            return TRUE;
        }
      else if (elType.ElTypeNum == HTML_EL_STYLE_ &&
               event->attributeType.AttrTypeNum == HTML_ATTR_Title)
        return FALSE;   
      /* coreattrs */
      else if (event->attributeType.AttrTypeNum == HTML_ATTR_ID ||
               event->attributeType.AttrTypeNum == HTML_ATTR_Class ||
               event->attributeType.AttrTypeNum == HTML_ATTR_Style_ ||
               event->attributeType.AttrTypeNum == HTML_ATTR_Title)
        {
          if (elType.ElTypeNum == HTML_EL_HEAD ||
              elType.ElTypeNum == HTML_EL_TITLE ||
              elType.ElTypeNum == HTML_EL_META ||
              elType.ElTypeNum == HTML_EL_STYLE_ ||
              elType.ElTypeNum == HTML_EL_HTML)
            /* HEAD, TITLE, META, STYLE and HTML don't accept coreattrs */
            return TRUE;
          else
            /* let Thot perform normal operation */
#ifdef TEMPLATES
            return CheckTemplateAttrInMenu(event);
#else /* TEMPLATES */
            return FALSE;
#endif /* TEMPLATES */
        }
      /* i18n */
      else if (event->attributeType.AttrTypeNum == HTML_ATTR_dir ||
               event->attributeType.AttrTypeNum == HTML_ATTR_Language)
        {
          if (elType.ElTypeNum == HTML_EL_BR ||
              elType.ElTypeNum == HTML_EL_Applet ||
              elType.ElTypeNum == HTML_EL_Horizontal_Rule ||
              elType.ElTypeNum == HTML_EL_FRAMESET ||
              elType.ElTypeNum == HTML_EL_FRAME ||
              elType.ElTypeNum == HTML_EL_IFRAME)
            return TRUE;
          else
#ifdef TEMPLATES
            return CheckTemplateAttrInMenu(event);
#else /* TEMPLATES */
            return FALSE;
#endif /* TEMPLATES */
       }
      /* events */
      else if (event->attributeType.AttrTypeNum == HTML_ATTR_onclick ||
               event->attributeType.AttrTypeNum == HTML_ATTR_ondblclick ||
               event->attributeType.AttrTypeNum == HTML_ATTR_onmousedown ||
               event->attributeType.AttrTypeNum == HTML_ATTR_onmouseup ||
               event->attributeType.AttrTypeNum == HTML_ATTR_onmouseover ||
               event->attributeType.AttrTypeNum == HTML_ATTR_onmousemove ||
               event->attributeType.AttrTypeNum == HTML_ATTR_onmouseout ||
               event->attributeType.AttrTypeNum == HTML_ATTR_onkeypress ||
               event->attributeType.AttrTypeNum == HTML_ATTR_onkeydown ||
               event->attributeType.AttrTypeNum == HTML_ATTR_onkeyup)
        {
          if (elType.ElTypeNum == HTML_EL_BDO ||
              elType.ElTypeNum == HTML_EL_Font_ ||
              elType.ElTypeNum == HTML_EL_BR ||
              elType.ElTypeNum == HTML_EL_Applet ||
              elType.ElTypeNum == HTML_EL_FRAMESET ||
              elType.ElTypeNum == HTML_EL_FRAME ||
              elType.ElTypeNum == HTML_EL_IFRAME ||
              elType.ElTypeNum == HTML_EL_HEAD ||
              elType.ElTypeNum == HTML_EL_TITLE ||
              elType.ElTypeNum == HTML_EL_META ||
              elType.ElTypeNum == HTML_EL_STYLE_ ||
              elType.ElTypeNum == HTML_EL_HTML ||
              elType.ElTypeNum == HTML_EL_ISINDEX)
            return TRUE;
        }
      /* xml:space attribute */
      else if (event->attributeType.AttrTypeNum == HTML_ATTR_xml_space)
        {
        /* be careful here -- document may not have been received yet */
        if (DocumentMeta[event->document] == NULL
            || DocumentMeta[event->document]->xmlformat == FALSE)
          return TRUE;
        }
#ifdef TEMPLATES
      return CheckTemplateAttrInMenu(event);
#else /* TEMPLATES */
      return FALSE;
#endif /* TEMPLATES */
    }
  return TRUE;	/* don't put an invalid attribute in the menu */
}

/*----------------------------------------------------------------------
  AttrRELREVinMenu
  Display a list of values for those attributes
  ----------------------------------------------------------------------*/
ThotBool AttrRELREVinMenu (NotifyAttribute * event)
{  
  /* let Thot perform normal operation */
  return FALSE;
}

/*----------------------------------------------------------------------
  AttrNAMEinMenu
  doesn't display NAME in Reset_Input and Submit_Input
  ----------------------------------------------------------------------*/
ThotBool AttrNAMEinMenu (NotifyAttribute * event)
{
  ElementType         elType;

  elType = TtaGetElementType (event->element);
  if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT)
    /* No visivible attributes on a picture element */
    return TRUE;		/* not allowed on standard pictures */
  else
    /* let Thot perform normal operation */
#ifdef TEMPLATES
    return CheckTemplateAttrInMenu(event);
#else /* TEMPLATES */
    /* it's not a global attribute. Accept it */
    return FALSE;
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  AttrScriptLanguageinMenu
  Don't display script_language attribute in menu
  ----------------------------------------------------------------------*/
ThotBool  AttrScriptLanguageinMenu (NotifyAttribute * event)
{
  ElementType         elType;

  elType = TtaGetElementType (event->element);
  if (elType.ElTypeNum == HTML_EL_SCRIPT_)
    return TRUE;
  else
    /* let Thot perform normal operation */
#ifdef TEMPLATES
    return CheckTemplateAttrInMenu(event);
#else /* TEMPLATES */
    /* it's not a global attribute. Accept it */
    return FALSE;
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  SetOnOffEmphasis
  The Emphasis button or menu item has been clicked
  ----------------------------------------------------------------------*/
void SetOnOffEmphasis (Document document, View view)
{
  Element             selectedEl;
  ElementType         elType;
  int                 firstSelectedChar, lastSelectedChar;

  TtaGiveFirstSelectedElement (document, &selectedEl, &firstSelectedChar,
                               &lastSelectedChar);
  if (selectedEl)
    {
      elType = TtaGetElementType (selectedEl);
      if (!strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML"))
        /* it's a HTML element */
        SetCharFontOrPhrase (document, HTML_EL_Emphasis);
      else if (!strcmp(TtaGetSSchemaName (elType.ElSSchema), "MathML"))
        /* it's a MathML element */
        SetMathCharFont (document, MathML_ATTR_fontstyle);
    }
  else
    TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_NO_INSERT_POINT);
}

/*----------------------------------------------------------------------
  SetOnOffStrong
  The Strong button or menu item has been clicked
  ----------------------------------------------------------------------*/
void SetOnOffStrong (Document document, View view)
{
  Element             selectedEl;
  ElementType         elType;
  int                 firstSelectedChar, lastSelectedChar;

  TtaGiveFirstSelectedElement (document, &selectedEl, &firstSelectedChar,
                               &lastSelectedChar);
  if (selectedEl)
    {
      elType = TtaGetElementType (selectedEl);
      if (!strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML"))
        /* it's a HTML element */
        SetCharFontOrPhrase (document, HTML_EL_Strong);
      else if (!strcmp(TtaGetSSchemaName (elType.ElSSchema), "MathML"))
        /* it's a MathML element */
        SetMathCharFont (document, MathML_ATTR_fontweight);
    }
  else
    TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_NO_INSERT_POINT);
}

/*----------------------------------------------------------------------
  SetOnOffCite
  ----------------------------------------------------------------------*/
void SetOnOffCite (Document document, View view)
{
  SetCharFontOrPhrase (document, HTML_EL_Cite);
}


/*----------------------------------------------------------------------
  SetOnOffDefinition
  ----------------------------------------------------------------------*/
void SetOnOffDefinition (Document document, View view)
{
  SetCharFontOrPhrase (document, HTML_EL_Def);
}


/*----------------------------------------------------------------------
  SetOnOffCode
  The Code button or menu item has been clicked
  ----------------------------------------------------------------------*/
void SetOnOffCode (Document document, View view)
{
  Element             selectedEl;
  ElementType         elType;
  int                 firstSelectedChar, lastSelectedChar;

  TtaGiveFirstSelectedElement (document, &selectedEl, &firstSelectedChar,
                               &lastSelectedChar);
  if (selectedEl)
    {
      elType = TtaGetElementType (selectedEl);
      if (!strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML"))
        /* it's a HTML element */
        SetCharFontOrPhrase (document, HTML_EL_Code);
      else if (!strcmp(TtaGetSSchemaName (elType.ElSSchema), "MathML"))
        /* it's a MathML element */
        SetMathCharFont (document, MathML_ATTR_fontfamily);
    }
  else
    TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_NO_INSERT_POINT);
}


/*----------------------------------------------------------------------
  SetOnOffVariable
  ----------------------------------------------------------------------*/
void SetOnOffVariable (Document document, View view)
{
  SetCharFontOrPhrase (document, HTML_EL_Variable_);
}


/*----------------------------------------------------------------------
  SetOnOffSample
  ----------------------------------------------------------------------*/
void SetOnOffSample (Document document, View view)
{
  SetCharFontOrPhrase (document, HTML_EL_Sample);
}


/*----------------------------------------------------------------------
  SetOnOffKeyboard
  ----------------------------------------------------------------------*/
void SetOnOffKeyboard (Document document, View view)
{
  SetCharFontOrPhrase (document, HTML_EL_Keyboard);
}


/*----------------------------------------------------------------------
  SetOnOffAbbr
  ----------------------------------------------------------------------*/
void SetOnOffAbbr (Document document, View view)
{
  SetCharFontOrPhrase (document, HTML_EL_ABBR);
}


/*----------------------------------------------------------------------
  SetOnOffAcronym
  ----------------------------------------------------------------------*/
void SetOnOffAcronym (Document document, View view)
{
  SetCharFontOrPhrase (document, HTML_EL_ACRONYM);
}


/*----------------------------------------------------------------------
  SetOnOffINS
  ----------------------------------------------------------------------*/
void SetOnOffINS (Document document, View view)
{
  SetCharFontOrPhrase (document, HTML_EL_ins);
}


/*----------------------------------------------------------------------
  SetOnOffDEL
  ----------------------------------------------------------------------*/
void SetOnOffDEL (Document document, View view)
{
  SetCharFontOrPhrase (document, HTML_EL_del);
}


/*----------------------------------------------------------------------
  SetOnOffItalic
  ----------------------------------------------------------------------*/
void SetOnOffItalic (Document document, View view)
{
  SetCharFontOrPhrase (document, HTML_EL_Italic_text);
}


/*----------------------------------------------------------------------
  SetOnOffBold
  ----------------------------------------------------------------------*/
void SetOnOffBold (Document document, View view)
{
  SetCharFontOrPhrase (document, HTML_EL_Bold_text);
}


/*----------------------------------------------------------------------
  SetOnOffTeletype
  ----------------------------------------------------------------------*/
void SetOnOffTeletype (Document document, View view)
{
  SetCharFontOrPhrase (document, HTML_EL_Teletype_text);
}


/*----------------------------------------------------------------------
  SetOnOffBig
  ----------------------------------------------------------------------*/
void SetOnOffBig (Document document, View view)
{
  SetCharFontOrPhrase (document, HTML_EL_Big_text);
}


/*----------------------------------------------------------------------
  SetOnOffSmall
  ----------------------------------------------------------------------*/
void SetOnOffSmall (Document document, View view)
{
  SetCharFontOrPhrase (document, HTML_EL_Small_text);
}


/*----------------------------------------------------------------------
  SetOnOffSub
  ----------------------------------------------------------------------*/
void SetOnOffSub (Document document, View view)
{
  SetCharFontOrPhrase (document, HTML_EL_Subscript);
}


/*----------------------------------------------------------------------
  SetOnOffSup
  ----------------------------------------------------------------------*/
void SetOnOffSup (Document document, View view)
{
  SetCharFontOrPhrase (document, HTML_EL_Superscript);
}


/*----------------------------------------------------------------------
  SetOnOffQuotation
  ----------------------------------------------------------------------*/
void SetOnOffQuotation (Document document, View view)
{
  SetCharFontOrPhrase (document, HTML_EL_Quotation);
}


/*----------------------------------------------------------------------
  SetOnOffBDO
  ----------------------------------------------------------------------*/
void SetOnOffBDO (Document document, View view)
{
  SetCharFontOrPhrase (document, HTML_EL_BDO);
}


/*----------------------------------------------------------------------
  Create a SPAN
  ----------------------------------------------------------------------*/
void CreateSpan (Document document, View view)
{
  Element       selectedEl;
  ElementType   elType;
  int           firstSelectedChar, lastSelectedChar;

  TtaGiveFirstSelectedElement (document, &selectedEl, &firstSelectedChar,
                               &lastSelectedChar);
  if (selectedEl)
    {
      elType = TtaGetElementType (selectedEl);
      if (!strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML"))
        /* it's a HTML element */
        SetCharFontOrPhrase (document, HTML_EL_Span);
    }
  else
    TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_NO_INSERT_POINT);
}

/*----------------------------------------------------------------------
  SearchAnchor
  Return the enclosing anchor element.
  If name is true, take into account elements with a name or an ID attribute.
  ----------------------------------------------------------------------*/
Element SearchAnchor (Document doc, Element element, Attribute *HrefAttr,
                      ThotBool name)
{
  AttributeType       attrType;
  Attribute           attr;
  ElementType         elType;
  Element             elAnchor, ancestor;
  SSchema             XLinkSchema;
  ThotBool            found;
  View                activeView;
  Document            activeDoc;
  char               *s;

  elAnchor = NULL;
  *HrefAttr = NULL;
  XLinkSchema = TtaGetSSchema ("XLink", doc);
  /* check the element and its ancestors */
  ancestor = element;
  do
    {
      attr = NULL;
      elType = TtaGetElementType (ancestor);
      s = TtaGetSSchemaName (elType.ElSSchema);
      if (!strcmp (s, "HTML"))
        /* the current element belongs to the HTML namespace */
        {
          attrType.AttrSSchema = elType.ElSSchema;
          if (name &&
              (elType.ElTypeNum == HTML_EL_Anchor ||
               elType.ElTypeNum == HTML_EL_MAP ||
               elType.ElTypeNum == HTML_EL_map))
            /* look for a name attribute */
            {
              attrType.AttrTypeNum = HTML_ATTR_NAME;
              attr = TtaGetAttribute (ancestor, attrType);
              /* stop the research */
              elAnchor = ancestor;
            }
          if (!attr)
            {
              if (elType.ElTypeNum == HTML_EL_LINK ||
                  elType.ElTypeNum == HTML_EL_Anchor ||
                  elType.ElTypeNum == HTML_EL_AREA)
                {
                  /* look for a href attribute */
                  attrType.AttrTypeNum = HTML_ATTR_HREF_;
                  attr = TtaGetAttribute (ancestor, attrType);
                }
              else if (elType.ElTypeNum == HTML_EL_FRAME)
                {
                  /* look for a src attribute */
                  attrType.AttrTypeNum = HTML_ATTR_FrameSrc;
                  attr = TtaGetAttribute (ancestor, attrType);
                }
              else if (elType.ElTypeNum == HTML_EL_Block_Quote ||
                       elType.ElTypeNum == HTML_EL_Quotation ||
                       elType.ElTypeNum == HTML_EL_INS ||
                       elType.ElTypeNum == HTML_EL_DEL)
                /* look for a cite attribute */
                {
                  attrType.AttrTypeNum = HTML_ATTR_cite;
                  attr = TtaGetAttribute (ancestor, attrType);
                }
            }
        }
      else if (!attr && !strcmp (s, "SVG"))
        /* the current element belongs to the SVG namespace */
        {
#ifdef _SVG
          found = FALSE;
          if (elType.ElTypeNum == SVG_EL_a)
            found = TRUE;
          else if (elType.ElTypeNum == SVG_EL_use_ ||
                   elType.ElTypeNum == SVG_EL_tref)
            /* it's a use or tref element. Consider it only if the active
               view is the Structure view, to allow anchors to be activated
               in the main view, even if they contain a use element */
            {
              TtaGetActiveView (&activeDoc, &activeView);
              if (activeDoc == doc && activeView != 0)
                if (!strcmp (TtaGetViewName (doc, activeView), "Structure_view"))
                  found = TRUE;
            }
          if (found)
            /* look for the corresponding href attribute */
            {
              attrType.AttrSSchema = elType.ElSSchema;
              attrType.AttrTypeNum = SVG_ATTR_xlink_href;
              attr = TtaGetAttribute (ancestor, attrType);
            }
#endif /* _SVG */
        }
      else if (!strcmp (s, "XLink"))
        {
          attrType.AttrSSchema = elType.ElSSchema;
          attrType.AttrTypeNum = XLink_ATTR_href_;
          attr = TtaGetAttribute (ancestor, attrType);
        }
	   
      if (!attr && XLinkSchema)
        /* the document uses XLink. Check whether the current element has
           a xlink:href attribute */
        {
          attrType.AttrSSchema = XLinkSchema;
          attrType.AttrTypeNum = XLink_ATTR_href_;
          attr = TtaGetAttribute (ancestor, attrType);
        }

      if (attr)
        {
          elAnchor = ancestor;
          *HrefAttr = attr;
        }
      else
        ancestor = TtaGetParent (ancestor);
    }
  while (elAnchor == NULL && ancestor);
  return elAnchor;
}
