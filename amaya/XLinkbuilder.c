/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 2000
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * XLinkbuilder
 *
 * Author: V. Quint
 *
 */
 

#define THOT_EXPORT extern
#include "amaya.h"
#include "XLink.h"
#include "parser.h"

static AttributeMapping XLinkAttributeMappingTable[] =
{
   /* The first entry MUST be unknown_attr */
   /* The rest of this table MUST be in alphabetical order */
   {TEXT("unknown_attr"), TEXT(""), 'A', XLink_ATTR_Invalid_attribute, L_Undefined},
   {TEXT("actuate"), TEXT(""), 'A', XLink_ATTR_actuate, L_Undefined},
   {TEXT("from"), TEXT(""), 'A', XLink_ATTR_from, L_Undefined},
   {TEXT("href"), TEXT(""), 'A', XLink_ATTR_href_, L_Undefined},
   {TEXT("role"), TEXT(""), 'A', XLink_ATTR_role, L_Undefined},
   {TEXT("show"), TEXT(""), 'A', XLink_ATTR_show, L_Undefined},
   {TEXT("title"), TEXT(""), 'A', XLink_ATTR_title, L_Undefined},
   {TEXT("to"), TEXT(""), 'A', XLink_ATTR_to, L_Undefined},
   {TEXT("type"), TEXT(""), 'A', XLink_ATTR_type, L_Undefined},
   {TEXT("zzghost"), TEXT(""), 'A', XLink_ATTR_Ghost_restruct, L_Undefined},

   {TEXT(""), TEXT(""), EOS, 0, L_Undefined}		/* Last entry. Mandatory */
};

/* mapping table of attribute values */

static AttrValueMapping XLinkAttrValueMappingTable[] =
{ 
   {XLink_ATTR_actuate, TEXT("onLoad"), XLink_ATTR_actuate_VAL_onLoad},
   {XLink_ATTR_actuate, TEXT("onRequest"), XLink_ATTR_actuate_VAL_onRequest},
   {XLink_ATTR_actuate, TEXT("undefined"), XLink_ATTR_actuate_VAL_undefined},

   {XLink_ATTR_show, TEXT("embed"), XLink_ATTR_show_VAL_embed},
   {XLink_ATTR_show, TEXT("new"), XLink_ATTR_show_VAL_new},
   {XLink_ATTR_show, TEXT("replace"), XLink_ATTR_show_VAL_replace},
   {XLink_ATTR_show, TEXT("undefined"), XLink_ATTR_show_VAL_undefined},

   {XLink_ATTR_type, TEXT("arc"), XLink_ATTR_type_VAL_arc},
   {XLink_ATTR_type, TEXT("extended"), XLink_ATTR_type_VAL_extended},
   {XLink_ATTR_type, TEXT("locator"), XLink_ATTR_type_VAL_locator},
   {XLink_ATTR_type, TEXT("none"), XLink_ATTR_type_VAL_none_},
   {XLink_ATTR_type, TEXT("resource"), XLink_ATTR_type_VAL_resource},
   {XLink_ATTR_type, TEXT("simple"), XLink_ATTR_type_VAL_simple},
   {XLink_ATTR_type, TEXT("title"), XLink_ATTR_type_VAL_title_},

   {0, TEXT(""), 0}			/* Last entry. Mandatory */
};

#include "fetchXMLname_f.h"

/*----------------------------------------------------------------------
   MapXLinkAttribute
   Search in the Attribute Mapping Table the entry for the
   attribute of name Attr and returns the corresponding Thot attribute type.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void           MapXLinkAttribute (CHAR_T* Attr, AttributeType *attrType, CHAR_T* elementName, Document doc)
#else
void           MapXLinkAttribute (Attr, attrType, elementName, doc)
CHAR_T*        Attr;
AttributeType* attrType;
CHAR_T*        elementName;
Document       doc;
#endif
{
   int                 i;

   attrType->AttrTypeNum = 0;
   attrType->AttrSSchema = NULL;
   i = 0;
   do
      if (ustrcasecmp (XLinkAttributeMappingTable[i].XMLattribute, Attr))
	 i++;
      else
	 if (XLinkAttributeMappingTable[i].XMLelement[0] == EOS)
	   {
	   attrType->AttrTypeNum = XLinkAttributeMappingTable[i].ThotAttribute;
	   attrType->AttrSSchema = GetXLinkSSchema (doc);
	   }
	 else if (!ustrcasecmp (XLinkAttributeMappingTable[i].XMLelement,
				elementName))
	   {
	   attrType->AttrTypeNum = XLinkAttributeMappingTable[i].ThotAttribute;
	   attrType->AttrSSchema = GetXLinkSSchema (doc);
	   }
	 else
	   i++;
   while (attrType->AttrTypeNum <= 0 &&
	  XLinkAttributeMappingTable[i].AttrOrContent != EOS);
}

/*----------------------------------------------------------------------
   MapXLinkAttributeValue
   Search in the Attribute Value Mapping Table the entry for the attribute
   and its value AttrVal. Returns the corresponding Thot value.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                MapXLinkAttributeValue (CHAR_T* AttrVal, AttributeType attrType, int *value)
#else
void                MapXLinkAttributeValue (AttrVal, attrType, value)
CHAR_T*             AttrVal;
AttributeType       attrType;
int		   *value;
#endif
{
   int                 i;

   *value = 0;
   i = 0;
   while (XLinkAttrValueMappingTable[i].ThotAttr != attrType.AttrTypeNum &&
	  XLinkAttrValueMappingTable[i].ThotAttr != 0)
      i++;
   if (XLinkAttrValueMappingTable[i].ThotAttr == attrType.AttrTypeNum)
      do
	if (!ustrcasecmp (XLinkAttrValueMappingTable[i].XMLattrValue, AttrVal))
	   *value = XLinkAttrValueMappingTable[i].ThotAttrValue;
	else
	   i++;
      while (*value <= 0 && XLinkAttrValueMappingTable[i].ThotAttr != 0);
}

/*----------------------------------------------------------------------
   XLinkAttributeComplete
   The XML parser has read attribute attr for element el in document doc.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void      XLinkAttributeComplete (Attribute attr, Element el, Document doc)
#else
void      XLinkAttributeComplete (attr, el, doc)
Attribute	attr;
Element		el;
Document	doc;

#endif
{
   AttributeType	attrType;
   int			attrKind;

   TtaGiveAttributeType (attr, &attrType, &attrKind);

   switch (attrType.AttrTypeNum)
     {
     case XLink_ATTR_actuate:
	break;

     default:
	break;
     }
}

/* end of module */
