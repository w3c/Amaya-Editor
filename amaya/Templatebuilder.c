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
  Document		doc;
  ElementType	elType, childType;
  Element		aMenu, last, child, newChild;

  doc = context->doc;
  elType = TtaGetElementType (el);
  switch (elType.ElTypeNum)
    {
    case Template_EL_head:
	  break;

    case Template_EL_component:
      CheckMandatoryAttribute (el, doc, Template_ATTR_name);
	  break;

    case Template_EL_union:
      CheckMandatoryAttribute (el, doc, Template_ATTR_name);
	  break;

    case Template_EL_import:
      CheckMandatoryAttribute (el, doc, Template_ATTR_src);
	  break;

    case Template_EL_useEl:
      //CheckMandatoryAttribute (el, doc, Template_ATTR_id);
	  CheckMandatoryAttribute (el, doc, Template_ATTR_types);
	  //Create the menu
	  elType.ElTypeNum = Template_EL_useMenu;
	  aMenu = TtaNewElement(doc, elType);
	  last = TtaGetLastChild(el);
	  if(last) 
		  TtaInsertSibling(aMenu, last, FALSE, doc);
	  else
		  TtaInsertFirstChild(&aMenu, el, doc);	  	  
	  break;

    case Template_EL_bag:
      //CheckMandatoryAttribute (el, doc, Template_ATTR_id);
	  CheckMandatoryAttribute (el, doc, Template_ATTR_types);
	  //Create the menu
	  elType.ElTypeNum = Template_EL_bagMenu;
	  aMenu = TtaNewElement(doc, elType);
	  last = TtaGetLastChild(el);
	  if(last) 
		  TtaInsertSibling(aMenu, last, FALSE, doc);
	  else
		  TtaInsertFirstChild(&aMenu, el, doc);
	  break;

    case Template_EL_attribute:
      CheckMandatoryAttribute (el, doc, Template_ATTR_name);
	  break;
	case Template_EL_option :
	  //Create the menu
	  elType.ElTypeNum = Template_EL_optionMenu;
	  aMenu = TtaNewElement(doc, elType);
	  last = TtaGetLastChild(el);
	  if(last) 
		  TtaInsertSibling(aMenu, last, FALSE, doc);
	  else
		  TtaInsertFirstChild(&aMenu, el, doc);
	  break;

	case Template_EL_repeat :
	  //If the content is not a XTiger element, we fold it in a folder
	  child = TtaGetFirstChild(el);
	  childType = TtaGetElementType(child);
	  if(strcmp(TtaGetSSchemaName(childType.ElSSchema),"Template")!=0)
	  {
		  elType.ElTypeNum = Template_EL_folder;
		  newChild = TtaNewElement(doc, elType);
		  TtaRemoveTree(child, doc);
		  TtaInsertFirstChild(&child, newChild, doc);
		  TtaInsertFirstChild(&newChild, el, doc);
	  }

	  //Create the menu
	  elType.ElTypeNum = Template_EL_repeatMenu;
	  aMenu = TtaNewElement(doc, elType);
	  last = TtaGetLastChild(el);
	  if(last) 
		  TtaInsertSibling(aMenu, last, FALSE, doc);
	  else
		  TtaInsertFirstChild(&aMenu, el, doc);
	  break;

    default:
      break;
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
	//TODO : The attribute attribute name is not unique!!
	/*
   AttributeType	attrType;
   int            attrKind;
   TtaGiveAttributeType (attr, &attrType, &attrKind);
   switch (attrType.AttrTypeNum)
     {
     case Template_ATTR_id:
       CheckUniqueName (el, doc, attr, attrType);
       break;
     case Template_ATTR_name:
       CheckUniqueName (el, doc, attr, attrType);
       break;
     default:
       break;
     }
   */
}


/*----------------------------------------------------------------------
  TemplateCheckInsert
  ----------------------------------------------------------------------*/
void  TemplateCheckInsert (Element *el, Element parent,
                             Document doc, ThotBool *inserted)
{
  ElementType  elType, parentType;
  char *elName, *parentName;

  elType = TtaGetElementType(*el);
  parentType = TtaGetElementType(parent);

  elName = TtaGetElementTypeName(elType);
  parentName = TtaGetElementTypeName(parentType);

  *inserted = FALSE;

  return;
}


