/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1998-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * Templatebuilder
 *
 * Authors: V. Quint
 *          I. Vatton
 */

#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"
#include "Template.h"
#include "parser.h"
#include "registry.h"
#include "style.h"

#include "containers.h"
#include "Elemlist.h"
#include "insertelem_f.h"


#include "templates.h"
#include "templateDeclarations.h"


#define MaxMsgLength 200
#include "anim_f.h"
#include "animbuilder_f.h"
#include "css_f.h"
#include "fetchXMLname_f.h"
#include "html2thot_f.h"
#include "HTMLactions_f.h"
#include "css_f.h"
#include "templateDeclarations_f.h"
#include "templateUtils_f.h"
#include "templates_f.h"
#include "Xml2thot_f.h"
#include "XHTMLbuilder_f.h"


/*----------------------------------------------------------------------
  TemplateGetDTDName
  Return in DTDname the name of the DTD to be used for parsing the
  content of element named elementName.
  This element type appear with an 'X' in the ElemMappingTable.
  ----------------------------------------------------------------------*/
void TemplateGetDTDName (char *DTDname, char *elementName)
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
void MapTemplateAttributeValue (char *attVal, const AttributeType *attrType,
                                int *value)
{
  MapXMLAttributeValue (Template_TYPE, attVal, attrType, value);
}

/*----------------------------------------------------------------------
  MapTemplateEntity
  Search that entity in the entity table and return the corresponding value.
  ----------------------------------------------------------------------*/
void MapTemplateEntity (char *entityName, char *entityValue, char *script)
{
  entityValue[0] = EOS;
  *script = EOS;
}

/*----------------------------------------------------------------------
  TemplateEntityCreated
  A Template entity has been created by the XML parser.
  ----------------------------------------------------------------------*/
void TemplateEntityCreated (unsigned char *entityValue, Language lang,
                            char *entityName, Document doc)
{

}

/*----------------------------------------------------------------------
  NeedAMenu returns TRUE is a menu must be generated
  and add the currentType is only one type is possible
  ----------------------------------------------------------------------*/
ThotBool NeedAMenu (Element el, Document doc)
{
  ElementType	     elType;
  Attribute        att;
  AttributeType    attributeType;
  XTigerTemplate   t;
  Declaration      dec;
  int              size;
  char            *types, *ptr;
  ThotBool         res = FALSE;
  ForwardIterator  iter;

  if (!TtaGetDocumentAccessMode(doc))
    return FALSE;

  // look for the list of types
  elType = TtaGetElementType (el);
  attributeType.AttrSSchema = elType.ElSSchema;
  if (elType.ElTypeNum == Template_EL_component)
    // use the name of the component as a type
    attributeType.AttrTypeNum = Template_ATTR_name;
  else
    attributeType.AttrTypeNum = Template_ATTR_types;
  att = TtaGetAttribute (el, attributeType);
  size = TtaGetTextAttributeLength (att);
  types = (char *) TtaGetMemory (size + 1);	
  TtaGiveTextAttributeValue (att, types, &size);
  ptr = strstr (types, " ");
  if (ptr)
    // there are several types
    res = TRUE;
  else if (!strcmp (types, "string") || !strcmp (types, "number"))
    {
      TtaFreeMemory (types);
      return FALSE;
    }
  else
    {
      t = GetXTigerDocTemplate (doc);
      if (t)
        {
          dec = Template_GetDeclaration (t, types);
          if (dec == NULL && elType.ElTypeNum == Template_EL_useEl)
            res =  (TtaGetFirstChild (el) == NULL);
          else if (dec && dec->nature == UnionNat)
            {
              /* check the extended list of the union */ 
              iter = SearchSet_GetForwardIterator(dec->unionType.include);
              if(ForwardIterator_GetCount(iter) > 1)
                res = TRUE;
              TtaFreeMemory(iter);
            }
        }
      else if (elType.ElTypeNum == Template_EL_useEl)
        res =  TRUE;
    }

  // When only one type is possible add the currentType attribute
  if (elType.ElTypeNum == Template_EL_useEl ||
       elType.ElTypeNum == Template_EL_useSimple)
    {
      attributeType.AttrTypeNum = Template_ATTR_currentType;
      att = TtaGetAttribute (el, attributeType);
      if (att == NULL && !res)
        {
          // there is only one type create this attribute
          att = TtaNewAttribute (attributeType);
          TtaAttachAttribute (el, att, doc);
          TtaSetAttributeText(att, types, el, doc);
        }
      else if (att && ptr == NULL)
        // the use is already instantiated
        res = FALSE;
    }
  TtaFreeMemory (types);
  return res;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void CheckNotEmptyComponent (Element el, Document doc)
{
#ifdef IV
  // not sure it's necessary
  Element          child, parent;
  ElementType	     elType;
  char            *s;

  child = TtaGetFirstChild (el);
  if (child)
    {
      while (child)
        {
          CheckNotEmptyComponent (child, doc);
          TtaNextSibling (&child);
        }
    }
  else
    {
      elType = TtaGetElementType (el);
      if (!TtaIsLeaf (elType))
        {
          // generate a leaf element
          s = TtaGetSSchemaName (elType.ElSSchema);
          parent = el;
          while (!strcmp (s, "Template"))
            {
              parent = TtaGetParent (parent);
              elType = TtaGetElementType (parent);
              s = TtaGetSSchemaName (elType.ElSSchema);
            }
          if (!strcmp (s, "HTML"))
            {
              if (IsCharacterLevelElement (el))
                elType.ElTypeNum = HTML_EL_Basic_Elem;
              else
                elType.ElTypeNum = HTML_EL_Element;
            }
          else if (!strcmp (s, "MathML"))
            elType.ElTypeNum = MathML_EL_TEXT_UNIT;
          else if (!strcmp (s, "SVG"))
            elType.ElTypeNum = SVG_EL_GraphicsElement;
          else if (elType.ElTypeNum == XML_EL_XML_Element)
            return;
          else
            elType.ElTypeNum = XML_EL_XML_Element;
          //printf ("==>Complete component %s:%d\n",s,elType.ElTypeNum);
          child = TtaNewElement (doc, elType);
          TtaInsertFirstChild (&child, el, doc);
        }
    }
#endif
}

/*----------------------------------------------------------------------
  TemplateElementComplete
  Check the Thot structure of the Template element el.
  ----------------------------------------------------------------------*/
void TemplateElementComplete (ParserData *context, Element el, int *error)
{
  Document		     doc;
  ElementType	     elType, otherType;
  Element          ancestor, child, next, prev;
	AttributeType    attType;
  Attribute        att;
  char            *name, *ancestor_name, *ptr, *types;
  char             msgBuffer[MaxMsgLength];
  int              len, option;
  ThotBool         hidden, withmenu = FALSE;

  doc = context->doc;
  elType = TtaGetElementType (el);
  switch  (elType.ElTypeNum)
    {
    case Template_EL_head:
      if (!IsTemplateInstanceDocument (doc))
        CheckMandatoryAttribute (el, doc, Template_ATTR_version);
    case Template_EL_Template:
      SetDocumentAsXTigerTemplate(doc);
      break;

    case Template_EL_component:
      if (!IsTemplateInstanceDocument (doc))
        {
          CheckMandatoryAttribute (el, doc, Template_ATTR_name);
          // check if the component is complete
          CheckNotEmptyComponent (el, doc);
        }
      break;

    case Template_EL_union:
      if (!IsTemplateInstanceDocument (doc))
        CheckMandatoryAttribute (el, doc, Template_ATTR_name);
      break;

    case Template_EL_import:
      if (!IsTemplateInstanceDocument (doc))
        CheckMandatoryAttribute (el, doc, Template_ATTR_src);
      break;

    case Template_EL_useEl:
    case Template_EL_useSimple:
      withmenu = NeedAMenu (el, doc);
      if (!withmenu && elType.ElTypeNum == Template_EL_useEl)
        TtaChangeTypeOfElement (el, doc, Template_EL_useSimple);
      CheckMandatoryAttribute (el, doc, Template_ATTR_title);
      // check if the name is not already 
      attType.AttrSSchema = elType.ElSSchema;
      attType.AttrTypeNum = Template_ATTR_types;
      att = TtaGetAttribute (el, attType);
      if (att)
        {
          name = GetAttributeStringValue (el, att, NULL);
          if (name)
            {
              ancestor = el;
              ancestor_name = GetAncestorComponentName (&ancestor);
              while (ancestor_name)
                {
                  // look for the ancestor name in the current name
                  ptr = strstr (name , ancestor_name);
                  len = strlen (ancestor_name);
                  if (len && ptr && 
                      (ptr == name || ptr[-1] == SPACE) &&
                      (ptr[len] == EOS || ptr[len] == SPACE))
                    {
                      // a loop is detected
                      sprintf (msgBuffer, "Remove type %s for use element",
                               ancestor_name);
                      XmlParseError (errorParsing, (unsigned char *)msgBuffer,
                                     TtaGetElementLineNumber(el));
                      if (ptr[len] == EOS)
                        ptr[0] = EOS;
                      else
                        strcpy (ptr, &ptr[len+1]);
                      TtaSetAttributeText(att, name, el, doc);
                      TtaFreeMemory (ancestor_name);
                      ancestor_name = NULL;
                    }
                  else
                    {
                      TtaFreeMemory (ancestor_name);
                      ancestor_name = GetAncestorComponentName (&ancestor);
                    }
                }
              TtaFreeMemory (name);
            }
          else
            XmlParseError (errorParsing,
                           (unsigned char *)"Missing mandatory attribute types for use element",
                           TtaGetElementLineNumber(el));
        }
      else
        XmlParseError (errorParsing,
                       (unsigned char *)"Missing mandatory attribute types for use element",
                       TtaGetElementLineNumber(el));
      child = TtaGetFirstChild (el);
      if (child == NULL)
        {
          hidden = ElementIsOptional (el);
          if (hidden)
            {
              // check the current option value
              option = GetAttributeIntValueFromNum (el, Template_ATTR_option);
              hidden = (option == Template_ATTR_option_VAL_option_unset);
            }
          // insert almost a pseudo element
          if (withmenu || hidden)
            {
              elType.ElTypeNum = Template_EL_TemplateObject;
              child = TtaNewElement (doc, elType);
              TtaInsertFirstChild (&child, el, doc);
              if (hidden)
                TtaSetAccessRight (child, ReadOnly, doc);
            }
        }
      break;

    case Template_EL_bag:
      CheckMandatoryAttribute (el, doc, Template_ATTR_types);
      CheckMandatoryAttribute (el, doc, Template_ATTR_title);
      // generate a pseudo paragraph if needed
      types = GetAttributeStringValueFromNum (el, Template_ATTR_types, NULL);
      if (types)
        {
          child = TtaGetFirstChild (el);
          otherType = TtaGetElementType (child);
          if (otherType.ElSSchema &&
              // template string or inline html
              (otherType.ElTypeNum == 1 ||
               (!strcmp (TtaGetSSchemaName (otherType.ElSSchema), "HTML") &&
                IsCharacterLevelElement (child))))
            {
              if (otherType.ElSSchema == elType.ElSSchema)
                {
                  // look for the target language
                  ancestor = el;
                  while (otherType.ElSSchema == elType.ElSSchema)
                    {
                      ancestor = TtaGetParent (ancestor);
                      otherType = TtaGetElementType (ancestor);
                    }
                  if (otherType.ElSSchema &&
                      strcmp (TtaGetSSchemaName (otherType.ElSSchema), "HTML"))
                    otherType.ElSSchema = NULL;
                }
              if (otherType.ElSSchema)
                {
                  // generate a pseudo paragraph
                  otherType.ElTypeNum = HTML_EL_Pseudo_paragraph;
                  ancestor = TtaNewElement (doc, otherType);
                  TtaInsertSibling (ancestor, child, TRUE, doc);
                  prev = NULL;
                  while (child)
                    {
                      next = child;
                      TtaNextSibling (&next);
                      TtaRemoveTree (child, doc);
                      if (prev)
                        TtaInsertSibling (child, prev, FALSE, doc);
                      else
                        TtaInsertFirstChild (&child, ancestor, doc);
                      prev = child;
                      child = next;
                    }
                }
            }
          TtaFreeMemory (types);
        }
      break;

    case Template_EL_attribute:
      CheckMandatoryAttribute (el, doc, Template_ATTR_ref_name);
      break;

    case Template_EL_repeat :
      // children must be use elements
      CheckMandatoryAttribute (el, doc, Template_ATTR_title);
      // Children must be components or use elements
      child = TtaGetFirstChild (el);
      while (child)
        {
          otherType = TtaGetElementType (child);
          if (otherType.ElSSchema != elType.ElSSchema ||
              (otherType.ElTypeNum != Template_EL_useSimple &&
               otherType.ElTypeNum != Template_EL_useEl &&
               otherType.ElTypeNum != Template_EL_component &&
               otherType.ElTypeNum != Template_EL_Comment_))
            // report an error
            XmlParseError (errorParsing,
                           (unsigned char *)"Invalid child of repeat element",
                           TtaGetElementLineNumber(child));
          TtaNextSibling (&child);
        }
      break;
    default:
      break;
    }
}

/*----------------------------------------------------------------------
  UnknownTemplateNameSpace
  The element doesn't belong to a supported namespace
  ----------------------------------------------------------------------*/
void UnknownTemplateNameSpace (ParserData *context, Element *unknownEl,
                               char* content)
{
}

/*----------------------------------------------------------------------
  TemplateAttributeComplete
  The XML parser has read attribute attr for element el in document doc.
  ----------------------------------------------------------------------*/
void TemplateAttributeComplete (Attribute attr, Element el, Document doc)
{
  AttributeType	 attrType;
  char          *checked, *string, *number;
  int            attrKind;

  TtaGiveAttributeType  (attr, &attrType, &attrKind);
  switch (attrType.AttrTypeNum)
    {
    case Template_ATTR_name:
      CheckUniqueName (el, doc, attr, attrType);
      break;
    case Template_ATTR_types:
      // check the validity of block or inline level
      checked = GetAttributeStringValueFromNum(el, Template_ATTR_includeAt, NULL);
      if (checked)
        {
          string = strstr (checked, "string");
          number = strstr (checked, "number");
          if ((string || number) && strstr (checked, " "))
            {
              // invalid association
              XmlParseError (errorParsing,
                             (unsigned char *)"string or number not allowed there",
                             TtaGetElementLineNumber(el));
            }
          TtaFreeMemory (checked);
        }
      break;
    default:
      break;
    }
}


/*----------------------------------------------------------------------
  TemplateCheckInsert
  ----------------------------------------------------------------------*/
void  TemplateCheckInsert (Element *el, Element parent,
                           Document doc, ThotBool *inserted)
{
  ElementType  elType, parentType;
  char        *elName, *parentName;

  elType = TtaGetElementType (*el);
  parentType = TtaGetElementType (parent);

  elName = TtaGetElementTypeName (elType);
  parentName = TtaGetElementTypeName (parentType);

  *inserted = FALSE;
  return;
}


