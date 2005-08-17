/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1998-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * Templatebuilder
 *
 * Authors: V. Quint
 *          I. Vatton
 *          P. Cheyrou-Lagreze
 */

#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"
#include "Template.h"
#include "parser.h"
#include "registry.h"
#include "style.h"


#define MaxMsgLength 200

#include "anim_f.h"
#include "animbuilder_f.h"
#include "css_f.h"
#include "fetchXMLname_f.h"
#include "html2thot_f.h"
#include "HTMLactions_f.h"
#include "styleparser_f.h"
#include "Xml2thot_f.h"
#include "XHTMLbuilder_f.h"


/*----------------------------------------------------------------------
   TemplateGetDTDName
   Return in DTDname the name of the DTD to be used for parsing the
   content of element named elementName.
   This element type appear with an 'X' in the ElemMappingTable.
  ----------------------------------------------------------------------*/
void      TemplateGetDTDName (char *DTDname, char *elementName)
{
  strcpy (DTDname, "");
}

/*----------------------------------------------------------------------
   MapTemplateAttribute
   Search in the Attribute Mapping Table the entry for the
   attribute of name Attr and returns the corresponding Thot attribute type.
  ----------------------------------------------------------------------*/
void MapTemplateAttribute (char *attrName, AttributeType *attrType,
			   char* elementName, ThotBool *level, Document doc)
{
  attrType->AttrSSchema = GetTemplateSSchema (doc);
  MapXMLAttribute (Template_TYPE, attrName, elementName, level, doc,
		   &(attrType->AttrTypeNum));
}

/*----------------------------------------------------------------------
   MapTemplateAttributeValue
   Search in the Attribute Value Mapping Table the entry for the attribute
   ThotAtt and its value attVal. Returns the corresponding Thot value.
  ----------------------------------------------------------------------*/
void MapTemplateAttributeValue (char* attVal, const AttributeType * attrType, int *value)
{
  MapXMLAttributeValue (Template_TYPE, attVal, attrType, value);
}

/*----------------------------------------------------------------------
   MapTemplateEntity
   Search that entity in the entity table and return the corresponding value.
  ----------------------------------------------------------------------*/
void   MapTemplateEntity (char *entityName, char *entityValue, char *script)
{
  entityValue[0] = EOS;
  *script = EOS;
}

/*----------------------------------------------------------------------
   TemplateEntityCreated
   A Template entity has been created by the XML parser.
  ----------------------------------------------------------------------*/
void    TemplateEntityCreated (unsigned char *entityValue, Language lang,
			      char *entityName, Document doc)
{
}



/*----------------------------------------------------------------------
   TemplateElementComplete
   Check the Thot structure of the Template element el.
  ----------------------------------------------------------------------*/
void TemplateElementComplete (ParserData *context, Element el, int *error)
{
  Document       doc;
  ElementType elType;

  doc = context->doc;
  elType = TtaGetElementType (el);
  switch (elType.ElTypeNum)
    {
    case Template_EL_FREE_STRUCT:
      CheckMandatoryAttribute (el, doc, Template_ATTR_xmlid);
    case Template_EL_FREE_CONTENT:
      CheckMandatoryAttribute (el, doc, Template_ATTR_xmlid);
    }
}

/*----------------------------------------------------------------------
   UnknownTemplateNameSpace
   The element doesn't belong to a supported namespace
  ----------------------------------------------------------------------*/
void               UnknownTemplateNameSpace (ParserData *context,
					Element *unknownEl,
					char* content)
{
}

/*----------------------------------------------------------------------
   TemplateAttributeComplete
   The XML parser has read attribute attr for element el in document doc.
  ----------------------------------------------------------------------*/
void TemplateAttributeComplete (Attribute attr, Element el, Document doc)
{
   AttributeType	attrType;
   int            attrKind;
   TtaGiveAttributeType (attr, &attrType, &attrKind);
   switch (attrType.AttrTypeNum)
     {
     case Template_ATTR_xmlid:
       CheckUniqueName (el, doc, attr, attrType);
       break;
     default:
       break;
     }
}



