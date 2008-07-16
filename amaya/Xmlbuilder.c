/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Xmlbuilder.c
 * Builds the corresponding abstract tree for a Thot document of type XML.
 *
 * Authors: L. Carcone 
 */

#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"
#include "parser.h"
#include "XML.h"
#include "fetchHTMLname.h"
#include "document.h"
#include "interface.h"

#include "css_f.h"
#include "fetchXMLname_f.h"
#include "fetchHTMLname_f.h"
#include "Mathedit_f.h"
#include "UIcss_f.h"
#include "styleparser_f.h"
#include "Xml2thot_f.h"
#include "HTMLedit_f.h"
#include "HTMLactions_f.h"
#include "AHTURLTools_f.h"

#ifdef _WX
#include "appdialogue_wx.h"
#include "paneltypes_wx.h"
#endif /* _WX */

#define MaxMsgLength 200

/* Mapping table of XML attribute values */
AttrValueMapping XmlAttrValueMappingTable[] =
  {
    /* XML attribute xml:space */
    {HTML_ATTR_xml_space, "default", XML_ATTR_xml_space_VAL_xml_space_default},
    {HTML_ATTR_xml_space, "preserve", XML_ATTR_xml_space_VAL_xml_space_preserve},

    {0, "", 0}			/* Last entry. Mandatory */
  };

/* maximum size of error messages */
#define MaxMsgLength 200

/* Variable used fot the modification of a CSS stylesheet */
static char OldCssName[MAX_LENGTH];

/*----------------------------------------------------------------------
  XmlAttributeComplete
  Complete XML elements. Check its attributes and its contents.
  ----------------------------------------------------------------------*/
void XmlAttributeComplete (Attribute attr, Element el, Document doc)

{
  AttributeType    attrType, attrType1;
  Attribute        intAttr;
  int		    attrKind;

  TtaGiveAttributeType (attr, &attrType, &attrKind);

  switch (attrType.AttrTypeNum)
    {
    case XML_ATTR_Language:
      if (el == TtaGetRootElement (doc))
        /* it's the lang attribute on the root element */
        /* set the RealLang attribute */
        {
          attrType1.AttrSSchema = attrType.AttrSSchema ;
          attrType1.AttrTypeNum = XML_ATTR_RealLang;
          if (!TtaGetAttribute (el, attrType1))
            /* it's not present. Add it */
            {
              intAttr = TtaNewAttribute (attrType1);
              TtaAttachAttribute (el, intAttr, doc);
              TtaSetAttributeValue (intAttr, XML_ATTR_RealLang_VAL_Yes_,
                                    el, doc);
            }
        }
      break;
    default:
      break;
    }
  return;
}

/*----------------------------------------------------------------------
  A new element has been selected. Synchronize selection in source view.      
  ----------------------------------------------------------------------*/
void XmlSelectionChanged (NotifyElement *event)
{
  CheckSynchronize (event);
  /* update the displayed style information */
  SynchronizeAppliedStyle (event);
  UnFrameMath ();
  
  UpdateXmlElementListTool(event->element,event->document);
  TtaSetStatusSelectedElement(event->document, 1, event->element);
  TtaRaiseDoctypePanels(WXAMAYA_DOCTYPE_XML);
}

/*----------------------------------------------------------------------
  CreateXmlAttribute
  create an attribute of type attrType for the element el.
  ----------------------------------------------------------------------*/
void CreateXmlAttribute (Element el, AttributeType attrType,
                         char *text, ThotBool isInvalid,
                         Document doc, Attribute *lastAttribute,
                         Element *lastAttrElement)
     
{
}

/*----------------------------------------------------------------------
  MapXmLAttributeValue
  Search in the Attribute Value Mapping Table the entry for the attribute
  ThotAtt and its value AttrVal. Returns the corresponding Thot value.
  ----------------------------------------------------------------------*/
void MapXmlAttributeValue (char *AttrVal, const AttributeType * attrType,
                           int *value)

{
  *value = 0;

  if (attrType->AttrTypeNum == XML_ATTR_xml_space)
    {
      if (strcmp (AttrVal, "preserve") == 0)
        *value = XML_ATTR_xml_space_VAL_xml_space_preserve;
      else
        *value = XML_ATTR_xml_space_VAL_xml_space_default;
    }

}

/*----------------------------------------------------------------------
  MapGenericXmlAttribute
  ----------------------------------------------------------------------*/
void MapGenericXmlAttribute (char *attrName, AttributeType *attrType,
                             Document doc)
{
  if (attrType->AttrSSchema == NULL)
    return;

  attrType->AttrTypeNum = 0;
  /* Search for the attribute XMLName in the structure schema */
  TtaGetXmlAttributeType (attrName, attrType, doc);

  if (attrType->AttrTypeNum <= 0)
    {
      /* The attribute is not yet present in the tree */
      /* Create a new global attribute */
      TtaAppendXmlAttribute (attrName, attrType, doc);
    }
}

/*----------------------------------------------------------------------
  XmlElementComplete
  Complete XML elements.
  Check its attributes and its contents.
  ----------------------------------------------------------------------*/
void XmlElementComplete (ParserData *context, Element el, int *error)

{
  Document     doc;   
  Element      lastChild;
  ElementType  elType;

  doc = context->doc;
  if (el && doc)
    {
      /* get the last child of the closed element */
      lastChild = TtaGetLastChild (el);
      if (lastChild == NULL)
        {
          /* This is an empty element */
          /* Add a specific presentation rule */
          elType = TtaGetElementType (el);
          if (elType.ElTypeNum != XML_EL_XML_Element)
            {
              TtaAddEmptyBox (el);
              //elType.ElTypeNum == XML_EL_XML_Element;
              //lastChild = TtaNewElement (doc, elType);
              //TtaInsertFirstChild (&lastChild, el, doc);
            }
        }
    }
  return;
}

/*----------------------------------------------------------------------
  CreateXmlLinePRule
  Create a generic 'Line' presentation rule for each element that
  has at least one Text child
  ----------------------------------------------------------------------*/
void  CreateXmlLinePRule (Element elText, Document doc)
  
{
  ElementType  parentType;
  Element      parent;

  parent = TtaGetParent (elText);
  if (parent != NULL)
    {
      parentType = TtaGetElementType (parent);
      TtaSetXmlInLineRule (parentType, doc);
    }
}

/*----------------------------------------------------------------------
  MapGenericXmlType
  ----------------------------------------------------------------------*/
void  MapGenericXmlElement (const char *XMLName, ElementType *elType,
                            char **mappedName, Document doc)
{
  if (elType->ElSSchema == NULL)
    return;

  elType->ElTypeNum = 0;
  /* Search  the element XMLName in the structure schema */
  TtaGetXmlElementType (XMLName, elType, mappedName, doc);

  if (elType->ElTypeNum <= 0)
    {
      /* The element is not yet present in the tree */
      /* Create a new rule in the generic schema */
      TtaAppendXmlElement (XMLName, elType, mappedName, doc);
    }
}

/*----------------------------------------------------------------------
  InsertCssInXml
  Create a processing instruction containing the reference to 
  a CSS stylesheet.
  ----------------------------------------------------------------------*/
void InsertCssInXml (Document doc, View view)
{
  Element      piEl, piLine, root, el;
  ElementType  elType;
  char        *s;
  int          piNum, piLineNum;
  int          j, firstChar, lastChar;
  Element      firstSel, lastSel;

  /* Check the Thot abstract tree against the structure schema. */
  TtaSetStructureChecking (FALSE, doc);

  /* Create an xmlpi element */
  el = NULL;
  elType.ElSSchema = TtaGetDocumentSSchema (doc);

  s = TtaGetSSchemaName (elType.ElSSchema);
  if (strcmp (s, "MathML") == 0)
    {
      piNum = MathML_EL_XMLPI;
      piLineNum = MathML_EL_XMLPI_line;
    }
  else if (strcmp (s, "SVG") == 0)
    {
      piNum = SVG_EL_XMLPI;
      piLineNum = SVG_EL_XMLPI_line;
    }
  else
    {
      piNum = XML_EL_xmlpi;
      piLineNum = XML_EL_xmlpi_line;
    }
  elType.ElTypeNum = piNum;
  /* give current position */
  TtaGiveFirstSelectedElement (doc, &firstSel, &firstChar, &j);
  TtaGiveLastSelectedElement (doc, &lastSel, &j, &lastChar);
  piEl = TtaNewElement (doc, elType);
  /* register this element in the editing history */
  TtaOpenUndoSequence (doc, firstSel, lastSel, firstChar, lastChar);
  TtaRegisterElementCreate (piEl, doc);
  if (piEl != NULL)
    {
      root = TtaGetRootElement (doc);
      TtaPreviousSibling (&root);
      if (root != NULL)
        TtaInsertSibling (piEl, root, FALSE, doc);
      else
        {
          root = TtaGetMainRoot (doc);
          TtaInsertFirstChild (&piEl, root, doc);
        }
      /* Create a xmlpi_line element as the first child of element xmlpi */
      elType.ElTypeNum = piLineNum;
      piLine = TtaNewElement (doc, elType);
      TtaRegisterElementCreate (piLine, doc);
      if (piLine != NULL)
        {
          TtaInsertFirstChild (&piLine, piEl, doc);
          /* Create a text element with the name of that style sheet */
          elType.ElTypeNum = 1;
          el = TtaNewElement (doc, elType);
          TtaRegisterElementCreate (el, doc);
          TtaInsertFirstChild (&el, piLine, doc);
          /* Select a new destination */
          SelectDestination (doc, el, FALSE, FALSE);
        }
    }
  TtaCloseUndoSequence (doc);
  TtaSetStructureChecking (TRUE, doc);
 
  return;
}

/*----------------------------------------------------------------------
  IsXmlStyleSheet                                           
  ----------------------------------------------------------------------*/
static Element IsXmlStyleSheet (Element el)
{
  Element	 parent;
  ElementType	 parentType;
  int           piLineNum, piNum;
  char         *s;

  parent = NULL;

  parent = TtaGetParent (el);
  if (parent != NULL)
    {
      parentType = TtaGetElementType (parent);
      s = TtaGetSSchemaName (parentType.ElSSchema);
      if (strcmp (s, "MathML") == 0)
        {
          piLineNum = MathML_EL_XMLPI_line;
          piNum = MathML_EL_XMLPI;
        }
      else if (strcmp (s, "SVG") == 0)
        {
          piLineNum = SVG_EL_XMLPI_line;
          piNum = SVG_EL_XMLPI;
        }
      else
        {
          piLineNum = XML_EL_xmlpi_line;
          piNum = XML_EL_xmlpi;
        }
      if (parentType.ElTypeNum == piLineNum)
        {
          /* We are treating an xml pi */
          parent = TtaGetParent (parent);
          parentType = TtaGetElementType (parent);
          if (parentType.ElTypeNum != piNum)
            parent = NULL;
        }
      else
        parent = NULL;
    }

  return parent;
}

/*----------------------------------------------------------------------
  XmlElementTypeInMenu
  -----------------------------------------------------------------------*/
ThotBool XmlElementTypeInMenu (NotifyElement *event)
{
  return TRUE; /* prevent Thot from putting this element name in the
                  element creation menu */
}

/*----------------------------------------------------------------------
  XmlStyleSheetWillBeModified                                             
  ----------------------------------------------------------------------*/
ThotBool XmlStyleSheetWillBeModified (NotifyOnTarget *event)
{
  char         *ptr = NULL, *end = NULL, *buffer = NULL;
  int           length;
  Language      lang;
    
  if (event->target != NULL)
    {
      OldCssName[0] = EOS;
      length = TtaGetTextLength (event->target) + 1;
      buffer = (char *)TtaGetMemory (length);
      if (buffer != NULL)
        {
          TtaGiveTextContent (event->target, (unsigned char *)buffer, &length, &lang);
          buffer[length++] = EOS;
          /* Is it an xml stylesheet ? */
          ptr = strstr (buffer, "xml-stylesheet");
          if (ptr != NULL)
            {
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
                  strcpy (OldCssName, ptr);
                }
            }
        }
      TtaFreeMemory (buffer);
    }
  return FALSE;
}

/*----------------------------------------------------------------------
  XmlStyleSheetModified                                             
  ----------------------------------------------------------------------*/
void XmlStyleSheetModified (NotifyOnTarget *event)
{
  char         *ptr = NULL, *end = NULL, *buffer = NULL;
  char          cssname[MAX_LENGTH], delimitor;
  char          pathname[MAX_LENGTH], documentname[MAX_LENGTH];   
  int           length;
  Language      lang;
  ThotBool      oldStructureChecking;
    
  if (event->target != NULL)
    {
      length = TtaGetTextLength (event->target) + 1;
      buffer = (char *)TtaGetMemory (length);
      if (buffer != NULL)
        {
          TtaGiveTextContent (event->target, (unsigned char *)buffer, &length, &lang);
          buffer[length++] = EOS;
          /* Is it an xml stylesheet ? */
          ptr = strstr (buffer, "xml-stylesheet");
          if (ptr != NULL)
            {
              /* Search the name of the stylesheet */
              ptr = strstr (buffer, "href");
              if (ptr != NULL)
                {
                  ptr = strstr (ptr, "=");
                  ptr++;
                  while (ptr[0] != EOS && ptr[0] == ' ')
                    ptr++;
                  if (ptr[0] != EOS)
                    {
                      delimitor = ptr[0];
                      strcpy (cssname, &ptr[1]);
                      end = strchr (cssname, delimitor);
                      if (end && end[0] != EOS)
                        *end = EOS;
                      if (OldCssName[0] != EOS && 
                          (strcmp (OldCssName, cssname) != 0))
                        {
                          NormalizeURL (OldCssName, event->document, pathname, documentname, NULL);
                          RemoveStyle (pathname, event->document, TRUE, TRUE, NULL, CSS_EXTERNAL_STYLE);
                          oldStructureChecking = TtaGetStructureChecking (event->document);
                          TtaSetStructureChecking (FALSE, event->document);
                          XmlStyleSheetPi (buffer, event->element);
                          TtaSetStructureChecking (oldStructureChecking, event->document);
                        }
                    }
                }
            }
        }
      TtaFreeMemory (buffer);
    }
  OldCssName[0] = EOS;
  return;
}

/*----------------------------------------------------------------------
  XmlStyleSheetDeleted                                              
  ----------------------------------------------------------------------*/
ThotBool XmlStyleSheetDeleted (NotifyElement * event)
{
  Element	 parent;
  char          buffer[MAX_LENGTH];
  char         *ptr = NULL;
  int           length;
  Language      lang;
 
  parent = IsXmlStyleSheet (event->element);
  if (parent != NULL)
    {
      length = MAX_LENGTH - 1;
      TtaGiveTextContent (event->element, (unsigned char *)buffer, &length, &lang);
      buffer[length++] = EOS;
      /* Is it an xml stylesheet ? */
      ptr = strstr (buffer, "xml-stylesheet");
      if (ptr != NULL)
        RemoveLink (parent, event->document);
    }
  /* let Thot perform normal operation */
  return FALSE;
}

/*----------------------------------------------------------------------
  XmlStyleSheetPasted
  ----------------------------------------------------------------------*/
void XmlStyleSheetPasted (NotifyElement *event)
{
  Element	 parent;
  Language      lang;
  char          buffer[MAX_LENGTH];
  char         *ptr = NULL;
  int           length;
  ThotBool      oldStructureChecking;

  parent = IsXmlStyleSheet (event->element);
  if (parent != NULL)
    {
      length = MAX_LENGTH - 1;
      TtaGiveTextContent (event->element, (unsigned char *)buffer, &length, &lang);
      buffer[length++] = EOS;
      /* Is it an xml stylesheet ? */
      ptr = strstr (buffer, "xml-stylesheet");
      if (ptr != NULL)
        {
          oldStructureChecking = TtaGetStructureChecking (event->document);
          TtaSetStructureChecking (FALSE, event->document);
          XmlStyleSheetPi (buffer, parent);
          TtaSetStructureChecking (oldStructureChecking, event->document);
        }
    }
}

/*----------------------------------------------------------------------
  XmlElementPasted
  ----------------------------------------------------------------------*/
void XmlElementPasted (NotifyElement * event)
{
  ElementType         elType;
  AttributeType       attrType;
  Attribute	          attr;

  elType = TtaGetElementType (event->element);
  if (elType.ElTypeNum == XML_EL_TEXT_UNIT)
    /* remove all attributes attached to the pasted XML_EL_TEXT_UNIT */
    RemoveTextAttributes (event->element, event->document);
  else
    {
      attrType.AttrSSchema = elType.ElSSchema;
      attrType.AttrTypeNum = XML_ATTR_xmlid;
      attr = TtaGetAttribute (event->element, attrType);
      if (attr)
        MakeUniqueName (event->element, event->document, TRUE, TRUE);
    }
}


/*----------------------------------------------------------------------
  CreateXMLElementMenu
  Create an XML element
  ----------------------------------------------------------------------*/
void CreateXMLElementMenu (Document doc, View view)
{
  TtaShowElementMenu (doc, view);
}
