/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2002
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

#include "css_f.h"
#include "fetchXMLname_f.h"
#include "fetchHTMLname_f.h"
#include "UIcss_f.h"
#include "styleparser_f.h"
#include "Xml2thot_f.h"
#include "HTMLedit_f.h"

/* maximum length of a Thot structure schema name */
#define MAX_SS_NAME_LENGTH 32

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

/*----------------------------------------------------------------------
  XmlAttributeComplete
  Complete XML elements. Check its attributes and its contents.
  ----------------------------------------------------------------------*/
void XmlAttributeComplete (Attribute attr, Element el, Document doc)

{
  return;
}

/*----------------------------------------------------------------------
   CreateXmlAttribute
   create an attribute of type attrType for the element el.
  ----------------------------------------------------------------------*/
void CreateXmlAttribute (Element       el,
			 AttributeType attrType,
			 char*         text,
			 ThotBool      isInvalid,
			 Document      doc,
			 Attribute    *lastAttribute,
			 Element      *lastAttrElement)
     
{
}

/*----------------------------------------------------------------------
   MapXmLAttributeValue
   Search in the Attribute Value Mapping Table the entry for the attribute
   ThotAtt and its value AttrVal. Returns the corresponding Thot value.
  ----------------------------------------------------------------------*/
void MapXmlAttributeValue (char *AttrVal, AttributeType  attrType,
			   int *value)

{

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
  Document  doc;   
  Element   lastChild;

  doc = context->doc;
  if (el && doc)
    {
      /* get the last child of the closed element */
      lastChild = TtaGetLastChild (el);
      
      if (lastChild == NULL)
	/* This is an empty element */
	/* Add a specific presentation rule */
	TtaAddEmptyBox (el);
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
void  MapGenericXmlElement (char *XMLName, ElementType *elType,
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
Element InsertCssInXml (Document doc, View view)
{
  Element      piEl, root, el;
  ElementType  elType;

  /* Check the Thot abstract tree against the structure schema. */
  TtaSetStructureChecking (0, doc);

  /* Create an xmlpi element */
  el = NULL;
  elType.ElSSchema = TtaGetDocumentSSchema (doc);
  elType.ElTypeNum = XML_EL_xmlpi;
  piEl = TtaNewElement (doc, elType);
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
      elType.ElTypeNum = XML_EL_xmlpi_line;
      el = TtaNewElement (doc, elType);
      if (el != NULL)
	{
	  TtaInsertFirstChild (&el, piEl, doc);
	  /* Select a new destination */
	  SelectDestination (doc, el, FALSE);
	}
    }
  TtaSetStructureChecking (1, doc);
 
  return (el);
}
