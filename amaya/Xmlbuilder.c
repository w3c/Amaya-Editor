/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2001.
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
#include "tree.h"

#include "css_f.h"
#include "fetchXMLname_f.h"
#include "fetchHTMLname_f.h"
#include "UIcss_f.h"
#include "styleparser_f.h"
#include "Xml2thot_f.h"

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
  XmlElementComplete
  Complete XML elements. Check its attributes and its contents.
  ----------------------------------------------------------------------*/
void           XmlElementComplete (Element el, Document doc, int *error)

{
  return;
}

/*----------------------------------------------------------------------
   CreateXmlAttribute
   create an attribute of type attrType for the element el.
  ----------------------------------------------------------------------*/
void           CreateXmlAttribute (Element       el,
				   AttributeType attrType,
				   char*       text,
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
void    MapXmlAttributeValue (char        *AttrVal,
			      AttributeType  attrType,
			      int           *value)

{

}

/*----------------------------------------------------------------------
  MapGenericXmlType
  ----------------------------------------------------------------------*/
void         MapGenericXmlType (STRING XMLname, ElementType *elType,
				STRING *mappedName, char *content,
				ThotBool *highEnoughLevel, Document doc)

{
#ifdef XML_GEN
  int        i;

  printf ("\n MapGenericXmlType\n");
  /* Initialize variables */
  *mappedName = NULL;
  *highEnoughLevel = TRUE;
  elType->ElTypeNum = 0;

  /* Search for the element name in the structure schema */

  if (1)
    {
      /* that element already exists */
    }
  else
    {
      /* Create a new rule for that element */
    }
#endif  /* XML_GEN */
}
