/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 *
 * fetchXMLname
 *
 * Authors: I. Vatton
 *          L. Carcone
 *
 */

#define THOT_EXPORT extern
#include "amaya.h"
#include "parser.h"
#include "HTMLnames.h"
#include "MathMLnames.h"
#include "SVGnames.h"
#include "XLinknames.h"
#include "Templatename.h"

/* define some pointers to let other parser functions access the local table */
int               HTML_ENTRIES = (sizeof(XHTMLElemMappingTable) / sizeof(ElemMapping));
ElemMapping      *pHTMLGIMapping = XHTMLElemMappingTable;
AttributeMapping *pHTMLAttributeMapping = XHTMLAttributeMappingTable;

XmlEntity        *pXhtmlEntityTable = XhtmlEntityTable;
XmlEntity        *pMathEntityTable = MathEntityTable;

#include "fetchXMLname_f.h"
#ifdef TEMPLATES
#include "templates.h"
#include "templates_f.h"
#endif /* TEMPLATES */

/* Global variables used by the entity mapping */
static int        XHTMLSup = 0;
static int        MathSup = 0;

/*----------------------------------------------------------------------
  GetXHTMLSSchema returns the XHTML Thot schema for document doc.
  ----------------------------------------------------------------------*/
SSchema GetXHTMLSSchema (Document doc)
{
  SSchema	XHTMLSSchema;

  XHTMLSSchema = TtaGetSSchema ("HTML", doc);
  if (XHTMLSSchema == NULL)
    XHTMLSSchema = TtaNewNature(doc, TtaGetDocumentSSchema(doc), NULL,
                                "HTML", "HTMLP");
  return (XHTMLSSchema);
}

/*----------------------------------------------------------------------
  GetMathMLSSchema returns the MathML Thot schema for document doc.
  ----------------------------------------------------------------------*/
SSchema GetMathMLSSchema (Document doc)
{
  SSchema	MathMLSSchema;

  MathMLSSchema = TtaGetSSchema ("MathML", doc);
  if (MathMLSSchema == NULL)
    MathMLSSchema = TtaNewNature(doc, 
                                 TtaGetDocumentSSchema(doc), NULL,
                                 "MathML", "MathMLP");
  return (MathMLSSchema);
}

/*----------------------------------------------------------------------
  GetSVGSSchema returns the SVG Thot schema for document doc.
  ----------------------------------------------------------------------*/
SSchema GetSVGSSchema (Document doc)
{
  SSchema	SVGSSchema;

  SVGSSchema = TtaGetSSchema ("SVG", doc);
  if (SVGSSchema == NULL)
    SVGSSchema = TtaNewNature(doc, TtaGetDocumentSSchema(doc), NULL,
                              "SVG", "SVGP");
  return (SVGSSchema);
}

/*----------------------------------------------------------------------
  GetXLinkSSchema returns the XLink Thot schema for document doc.
  ----------------------------------------------------------------------*/
SSchema GetXLinkSSchema (Document doc)
{
  SSchema	XLinkSSchema;

  XLinkSSchema = TtaGetSSchema ("XLink", doc);
  if (XLinkSSchema == NULL)
    XLinkSSchema = TtaNewNature(doc, TtaGetDocumentSSchema(doc), NULL,
                                    "XLink", "XLinkP");
  return (XLinkSSchema);
}

/* --------------------------------------------------------------------
   GetTemplateSSchema returns the Template Thot schema for document doc.
   --------------------------------------------------------------------*/
SSchema GetTemplateSSchema (Document doc)
{
  SSchema       TemplateSSchema = NULL;
  
#ifdef TEMPLATES
  TemplateSSchema = TtaGetSSchema ("Template", doc);
  if (TemplateSSchema == NULL)
    {
      if (IsTemplateInstanceDocument(doc))
        TemplateSSchema = TtaNewNature (doc, TtaGetDocumentSSchema(doc), NULL,
                                    "Template", "TemplatePI");
      else
        TemplateSSchema = TtaNewNature (doc, TtaGetDocumentSSchema(doc), NULL,
                                        "Template", "TemplateP");
    }
#endif /* TEMPLATES */
  return (TemplateSSchema);
}
    


/*----------------------------------------------------------------------
  GetTextSSchema returns the TextFile Thot schema for document doc.
  (this is not XML, but its useful to have this function here).
  ----------------------------------------------------------------------*/
SSchema GetTextSSchema (Document doc)
{
  SSchema	XLinkSSchema;

  XLinkSSchema = TtaGetSSchema ("TextFile", doc);
  if (XLinkSSchema == NULL)
    XLinkSSchema = TtaNewNature(doc, TtaGetDocumentSSchema(doc), NULL,
                                "TextFile", "TextFileP");
  return (XLinkSSchema);
}

/*----------------------------------------------------------------------
  GetGenericXMLSSchema
  Returns the XML Thot schema of name schemaName for the document doc.
  ----------------------------------------------------------------------*/
SSchema GetGenericXMLSSchema (const char *schemaName, Document doc)
{
  SSchema	XMLSSchema;

  XMLSSchema = TtaGetSSchema (schemaName, doc);
  if (XMLSSchema == NULL)
    XMLSSchema = TtaNewNature(doc, TtaGetDocumentSSchema(doc), NULL,
                              "XML", "XMLP");
  return (XMLSSchema);
}

/*----------------------------------------------------------------------
  GetGenericXMLSSchemaByUri
  Returns the XML Thot schema for the document doc.
  ----------------------------------------------------------------------*/
SSchema GetGenericXMLSSchemaByUri (const char *uriName, Document doc, ThotBool *isnew)
{
  SSchema	XMLSSchema;

  if (uriName == NULL)
    XMLSSchema = TtaGetSSchemaByUri ("Default_Uri", doc);
  else
    XMLSSchema = TtaGetSSchemaByUri (uriName, doc);
  if (XMLSSchema == NULL)
    {
      XMLSSchema = TtaNewNature(doc, TtaGetDocumentSSchema(doc), uriName,
                                "XML", "XMLP");
      *isnew = TRUE;
    }
  return (XMLSSchema);
}


/*----------------------------------------------------------------------
  GetXMLSSchema returns the XML Thot schema for document doc.
  ----------------------------------------------------------------------*/
SSchema GetXMLSSchema (int XMLtype, Document doc)
{
  if (XMLtype == XHTML_TYPE)
    return GetXHTMLSSchema (doc);
  else if (XMLtype == MATH_TYPE)
    return GetMathMLSSchema (doc);
  else if (XMLtype == SVG_TYPE)
    return GetSVGSSchema (doc);
  else if (XMLtype == XLINK_TYPE)
    return GetXLinkSSchema (doc);
  else if (XMLtype == Template_TYPE)
    return GetTemplateSSchema (doc);
  else
    return NULL;
}

/*--------------------------------------------------------------------------
  HasNatures
  Check if there are MathML and/or SVG natures
  --------------------------------------------------------------------------*/
void HasNatures (Document document, ThotBool *useMathML, ThotBool *useSVG)
{
  SSchema         nature;
  char           *ptr;

  /* look for a MathML or SVG nature within the document */
  nature = NULL;
  *useMathML = FALSE;
  *useSVG = FALSE;
  if (DocumentMeta[document] && DocumentMeta[document]->compound)
    do
      {
        TtaNextNature (document, &nature);
        if (nature)
          {
            ptr = TtaGetSSchemaName (nature);
            if (!strcmp (ptr, "MathML"))
              *useMathML = TRUE;
            if (!strcmp (ptr, "SVG"))
              *useSVG = TRUE;
          }
      }
    while (nature);
}

/*----------------------------------------------------------------------
  MapXMLElementType
  Generic function which searchs in the Element Mapping table, selected
  by the parameter XMLtype, the entry XMLname and returns the corresponding
  Thot element type.
  If SSchema is specified (not NULL)in elType, only it is searched.
  Returns:
  - ElTypeNum and ElSSchema into elType  ElTypeNum = 0 if not found.
  - content information about this entry
  - checkProfile TRUE if the entry is valid for the current Doc profile.
  ----------------------------------------------------------------------*/
void MapXMLElementType (int XMLtype, const char *XMLname, ElementType *elType,
                        char **mappedName, char *content,
                        ThotBool *checkProfile, Document doc)
{
  ElemMapping        *ptr;
  char                c;
  int                 i, profile;
  ThotBool            xmlformat;

  /* Initialize variables */
  *mappedName = NULL;
  *checkProfile = TRUE;
  elType->ElTypeNum = 0;
  profile = TtaGetDocumentProfile (doc);
  if (profile == L_Annot)
    profile = L_Other;
  /* case sensitive comparison for xml documents */
  xmlformat = (DocumentMeta[doc] && DocumentMeta[doc]->xmlformat);

  /* Select the right table */
  if (XMLtype == XHTML_TYPE)
    {
      ptr = XHTMLElemMappingTable;
      /* no case sensitive whent there is an explicit "text/html" content_type */
      if (xmlformat && DocumentMeta[doc] && DocumentMeta[doc]->content_type &&
          !strcmp (DocumentMeta[doc]->content_type, "text/html"))
        xmlformat = FALSE;
    }
  else if (XMLtype == MATH_TYPE)
    {
      if (profile == L_Basic && DocumentTypes[doc] == docHTML)
        {
          /* Maths are not allowed in this document */
          ptr = NULL;
          *checkProfile = FALSE;
        }
      else
        ptr = MathMLElemMappingTable;
    }
  else if (XMLtype == SVG_TYPE)
    {
      if (profile == L_Basic && DocumentTypes[doc] == docHTML)
        {
          /* Graphics are not allowed in this document */
          ptr = NULL;
          *checkProfile = FALSE;
        }
      else
        ptr = SVGElemMappingTable;
    }
#ifdef TEMPLATES
  else if (XMLtype == Template_TYPE)
    {
      if (profile == L_Basic && DocumentTypes[doc] == docHTML)
        {
          /* Graphics are not allowed in this document */
          ptr = NULL;
          *checkProfile = FALSE;
        }
      else
        ptr = TemplateElemMappingTable;
    }
#endif /* TEMPLATES */
  else
    ptr = NULL;
   
  if (ptr)
    {
      /* search in the ElemMappingTable */
      i = 0;
      /* case insensitive for HTML */
      if (!xmlformat && ptr == XHTMLElemMappingTable)
        c = tolower (XMLname[0]);
      else
        c = XMLname[0];
      /* look for the first concerned entry in the table */
      while (ptr[i].XMLname[0] < c && ptr[i].XMLname[0] != EOS)
        i++;     
      /* look at all entries starting with the right character */
      do
        if (!xmlformat && ptr == XHTMLElemMappingTable &&
            strcasecmp (ptr[i].XMLname, XMLname))
          /* it's not the tag */
          i++;
        else if ((xmlformat || ptr != XHTMLElemMappingTable) &&
                 strcmp (ptr[i].XMLname, XMLname))
          /* it's not the tag */
          i++;
        else if (XMLtype == XHTML_TYPE && profile != L_Other && !(ptr[i].Level & profile))
          {
            /* this tag is not valid in the document profile */
            *checkProfile = FALSE;
            i++;
          }
        else
          {
            elType->ElTypeNum = ptr[i].ThotType;
            if (elType->ElSSchema == NULL)
              {
                if (XMLtype == Template_TYPE)
                  elType->ElSSchema = GetTemplateSSchema(doc);
                else
                  elType->ElSSchema = GetXMLSSchema (XMLtype, doc);
              }
            *mappedName = ptr[i].XMLname;
            *content = ptr[i].XMLcontents;
          }
      while (elType->ElTypeNum <= 0 && ptr[i].XMLname[0] == c);
    }
}


/*----------------------------------------------------------------------
  GetXMLElementName
  Generic function which searchs in the mapping tables the XML name for
  a given Thot type.
  ----------------------------------------------------------------------*/
const char *GetXMLElementName (ElementType elType, Document doc)
{
  ElemMapping  *ptr;
  char         *name;
  int           i, profile;
  ThotBool      invalid = FALSE;

  if (elType.ElTypeNum > 0)
    {
      i = 0;
      /* Select the table which matches with the element schema */
      name = TtaGetSSchemaName (elType.ElSSchema);
      if (strcmp ("MathML", name) == 0)
        ptr = MathMLElemMappingTable;
      else if (strcmp ("SVG", name) == 0)
        ptr = SVGElemMappingTable;
      else if (strcmp ("HTML", name) == 0)
        ptr = XHTMLElemMappingTable;
#ifdef TEMPLATES
      else if (strcmp ("Template", name) == 0)
        ptr = TemplateElemMappingTable;
#endif /* TEMPLATES */
      else
        ptr = NULL;

      profile = TtaGetDocumentProfile (doc);
      if (profile == L_Annot)
        profile = L_Other;

      if (ptr)
        do
          {
            if (ptr[i].ThotType == elType.ElTypeNum)
              {
                if (doc == 0 || 
                    profile == L_Other || (ptr[i].Level & profile))
                  return ptr[i].XMLname;
                else
                  invalid = TRUE;
              }
            i++;
          }
        while (ptr[i].XMLname[0] != EOS);	  
    }
  if (invalid)
    return "";
  else
    return "???";
}


/*----------------------------------------------------------------------
  IsXMLElementInline
  Generic function which searchs in the mapping tables if a given
  Thot type is an inline character or not
  ----------------------------------------------------------------------*/
ThotBool IsXMLElementInline (ElementType elType, Document doc)
{
  int            i;
  ThotBool       ret = FALSE;
  char          *name;
  ElemMapping   *ptr;

  if (elType.ElTypeNum > 0)
    {
      i = 0;
      /* Select the table which matches with the element schema */
      name = TtaGetSSchemaName (elType.ElSSchema);
      if (strcmp ("MathML", name) == 0)
        ptr = MathMLElemMappingTable;
      else if (strcmp ("SVG", name) == 0)
        ptr = SVGElemMappingTable;
      else if (strcmp ("HTML", name) == 0)
        ptr = XHTMLElemMappingTable;
#ifdef TEMPLATES
      else if (strcmp ("Template", name) == 0)
        return FALSE;
#endif /* TEMPLATES */
      else
        ptr = NULL;
      
      if (ptr)
        {
          while (ptr[i].XMLname[0] != EOS &&
                 ptr[i].ThotType != elType.ElTypeNum)
            i++;
          if (ptr[i].ThotType == elType.ElTypeNum)
            ret = ptr[i].InlineElem;
        }
    }
  return ret;
}

/*----------------------------------------------------------------------
  MapXMLAttributeValue
  Search in the Attribute Value Mapping Table the entry for the attribute
  ThotAtt and its value attVal. Returns the corresponding Thot value.
  ----------------------------------------------------------------------*/
void MapXMLAttributeValue (int XMLtype, char *attVal, const AttributeType *attrType,
                           int *value)
{
  AttrValueMapping   *ptr;
  int                 i;

  /* Select the right table */
  if (XMLtype == XHTML_TYPE)
    ptr = XhtmlAttrValueMappingTable;
  else if (XMLtype == MATH_TYPE)
    ptr = MathMLAttrValueMappingTable;
  else if (XMLtype == SVG_TYPE)
    ptr = SVGAttrValueMappingTable;
#ifdef TEMPLATES
  else if (XMLtype == Template_TYPE)
    ptr = TemplateAttrValueMappingTable;
#endif /* TEMPLATES */
  else
    ptr = NULL;
  
  *value = 0;
  i = 0;
  if (ptr == NULL)
    return;
  while (ptr[i].ThotAttr != attrType->AttrTypeNum && ptr[i].ThotAttr != 0)
    i++;
  if (ptr[i].ThotAttr == attrType->AttrTypeNum)
    do
      if (!strcmp (ptr[i].XMLattrValue, attVal))
        *value = ptr[i].ThotAttrValue;
      else
        i++;
    while (*value == 0 && ptr[i].ThotAttr == attrType->AttrTypeNum);
}


/*----------------------------------------------------------------------
  MapXMLAttribute
  Generic function which searchs in the Attribute Mapping Table (table)
  the entry attrName associated to the element elementName.
  Returns the corresponding entry or -1.
  ----------------------------------------------------------------------*/
int MapXMLAttribute (int XMLtype, const char *attrName, const char *elementName,
                     ThotBool *checkProfile, Document doc, int *thotType)
{
  AttributeMapping   *ptr;
  char                c;
  int                 i, profile, extraprofile;
  ThotBool            xmlformat;

  /* Initialization */
  *checkProfile = TRUE;
  i = 1;
  *thotType = 0;
  /* case sensitive comparison for xml documents */
  xmlformat = (DocumentMeta[doc] && DocumentMeta[doc]->xmlformat);
  
  /* Select the right table */
  if (XMLtype == XHTML_TYPE)
    {
      ptr = XHTMLAttributeMappingTable;
      /* no case sensitive whent there is an explicit "text/html" content_type */
      if (xmlformat && DocumentMeta[doc] && DocumentMeta[doc]->content_type &&
          !strcmp (DocumentMeta[doc]->content_type, "text/html"))
        xmlformat = FALSE;
    }
  else if (XMLtype == MATH_TYPE)
    ptr = MathMLAttributeMappingTable;
  else if (XMLtype == SVG_TYPE)
    ptr = SVGAttributeMappingTable;
#ifdef TEMPLATES
  else if (XMLtype == Template_TYPE)
    ptr = TemplateAttributeMappingTable;
#endif /* TEMPLATES */
  else if (XMLtype == XLINK_TYPE)
    ptr = XLinkAttributeMappingTable;

  else
    ptr = NULL;
  if (ptr == NULL)
    return -1;
  if (strcmp (attrName, "unknown_attr") == 0)
    {
      *thotType = ptr[0].ThotAttribute;
      return 0;
    }

  /* case insensitive for HTML */
  if (!xmlformat && ptr == XHTMLAttributeMappingTable)
    c = tolower (attrName[0]);
  else
    c = attrName[0];

  profile = TtaGetDocumentProfile (doc);
  extraprofile = TtaGetDocumentExtraProfile (doc);

  if (profile == L_Annot)
    profile = L_Other;
  /* look for the first concerned entry in the table */
  while (ptr[i].XMLattribute[0] < c &&  ptr[i].XMLattribute[0] != EOS)
    i++;
  while (ptr[i].XMLattribute[0] == c)
    {
      if (!xmlformat && ptr == XHTMLAttributeMappingTable &&
          (strcasecmp (ptr[i].XMLattribute, attrName) ||
           (ptr[i].XMLelement[0] != EOS && elementName &&
            strcasecmp (ptr[i].XMLelement, elementName))))
        i++;
      else if ((xmlformat || ptr != XHTMLAttributeMappingTable) &&
               (strcmp (ptr[i].XMLattribute, attrName) ||
                (ptr[i].XMLelement[0] != EOS && elementName &&
                 strcmp (ptr[i].XMLelement, elementName))))
        i++;
      else if (profile != L_Other && extraprofile == 0 && !(ptr[i].Level & profile))
        {
          *checkProfile = FALSE;
          i++;
        }
      else if ((ptr[i].Level == L_RDFaValue) && (extraprofile != L_RDFa))
        {
          *checkProfile = FALSE;
          i++;
        }
      else
        {
          /* Special case for the 'name' attribute for elements 'a' and 'map' in xhtml1.1 profile */
          if ((profile == L_Xhtml11) &&
              !strcmp (attrName, "name") && elementName &&
              (!strcmp (elementName, "a") || !strcmp (elementName, "map")))
            *checkProfile = FALSE;
          /* Special case for the attributes 'rel' and 'rev' for elements 'a' and 'link' */
          else if ((!strcmp (attrName, "rel") || !strcmp (attrName, "rev")) &&
		   elementName && 
                   (strcmp (elementName, "a") && strcmp (elementName, "link")) &&
		   (extraprofile != L_RDFa))
	    *checkProfile = FALSE;
	  else
            *thotType = ptr[i].ThotAttribute;
          return (i);
        }
    }
  return (-1);
}


/*----------------------------------------------------------------------
  GetXMLAttributeName
  Generic function which searchs in the mapping tables the XML name for
  a given Thot type.
  ----------------------------------------------------------------------*/
const char *GetXMLAttributeName (AttributeType attrType, ElementType elType,
                           Document doc)
{
  AttributeMapping   *ptr;
  char               *name;
  const char         *tag;
  int                 i, profile, extraprofile;
  ThotBool            invalid = FALSE;

  if (attrType.AttrTypeNum > 0)
    {
      /* get the specific element tag */
      if (elType.ElTypeNum > 0)
        tag = GetXMLElementName (elType, doc);
      else
        tag = "";

      i = 0;
      /* Select the table which matches with the element schema */
      name = TtaGetSSchemaName (attrType.AttrSSchema);
      if (strcmp ("MathML", name) == 0)
        ptr = MathMLAttributeMappingTable;
#ifdef _SVG
      else if (strcmp ("SVG", name) == 0)
        ptr = SVGAttributeMappingTable;
#endif /* _SVG */
      else if (strcmp ("XLink", name) == 0)
        ptr = XLinkAttributeMappingTable;
#ifdef TEMPLATES
      else if (strcmp ("Template",name) == 0)
        ptr = TemplateAttributeMappingTable;
#endif /* TEMPLATES */
      else
        ptr = XHTMLAttributeMappingTable;
      
      profile = TtaGetDocumentProfile (doc);
      extraprofile = TtaGetDocumentExtraProfile (doc);
      if (profile == L_Annot)
        profile = L_Other;

      if (ptr)
        do
          {
            if (ptr[i].ThotAttribute == attrType.AttrTypeNum &&
                (ptr[i].XMLelement[0] == EOS || !strcmp (ptr[i].XMLelement, tag)))
              {
                if (doc != 0 && profile != L_Other && extraprofile == L_NoExtraProfile && !(ptr[i].Level & profile))
                  invalid = TRUE;
                else if (doc != 0 && ptr[i].Level == L_RDFaValue && extraprofile != L_RDFa)
                  invalid = TRUE;
		else if ((attrType.AttrTypeNum == HTML_ATTR_REL ||
			  attrType.AttrTypeNum == HTML_ATTR_REV) &&
		 	  elType.ElTypeNum != HTML_EL_Anchor &&
			  elType.ElTypeNum != HTML_EL_LINK &&
			  extraprofile != L_RDFa)
                  invalid = TRUE;
		else
                  return ptr[i].XMLattribute;
              }
            i++;
          }
        while (ptr[i].XMLattribute[0] != EOS);	  
    }
  if (invalid)
    return "";
  else
    return "???";
}

/*----------------------------------------------------------------------
  HasADoctype returns TRUE if the document includes a DocType
  ----------------------------------------------------------------------*/
void HasADoctype (Document doc, ThotBool *found, ThotBool *useMath)
{
  Element         el_doc, el_doctype;
  ElementType     elType;
  char           *s;
  ThotBool        useSVG;

  /* Look for a doctype */
  el_doc = TtaGetMainRoot (doc);
  elType = TtaGetElementType (el_doc);
  /* Search the doctype declaration according to the main schema */
  s = TtaGetSSchemaName (elType.ElSSchema);
  if (strcmp (s, "HTML") == 0)
    elType.ElTypeNum = HTML_EL_DOCTYPE;
  else if (strcmp (s, "SVG") == 0)
    elType.ElTypeNum = SVG_EL_DOCTYPE;
  else if (strcmp (s, "MathML") == 0)
    elType.ElTypeNum = MathML_EL_DOCTYPE;
  else
    elType.ElTypeNum = XML_EL_doctype;
  el_doctype = TtaSearchTypedElement (elType, SearchInTree, el_doc);
  *found = (el_doctype != NULL);
  HasNatures (doc, useMath, &useSVG);
}


/*----------------------------------------------------------------------
  MapXMLEntity
  Generic function which searchs in the Entity Mapping Table (table)
  the entry entityName and give the corresponding decimal value.
  Returns FALSE if entityName is not found.
  ----------------------------------------------------------------------*/
ThotBool MapXMLEntity (int XMLtype, char *entityName, int *entityValue)
{
  XmlEntity  *ptr;
  ThotBool    found;
  int         inf, sup, med, rescomp;

  /* Initialization */
  found = FALSE;
  sup = 0;

  /* Select the right table */
  if (XMLtype == XHTML_TYPE || XMLtype == Template_TYPE)
    {
      ptr = XhtmlEntityTable;
      if (XHTMLSup == 0)
        for (XHTMLSup = 0; ptr[XHTMLSup].charCode > 0; XHTMLSup++);
      sup = XHTMLSup;
    }
  else if (XMLtype == MATH_TYPE)
    {
      ptr = MathEntityTable;
      if (MathSup == 0)
        for (MathSup = 0; ptr[MathSup].charCode > 0; MathSup++);
      sup = MathSup;
    }
  else
    ptr = NULL;
  
  if (ptr)
    {
      inf = 0;
      while (sup >= inf && !found)
        /* Dichotomic research */
        {
          med = (sup + inf) / 2;
          rescomp = strcmp (ptr[med].charName, entityName);
          if (rescomp == 0)
            {
              /* entity found */
              *entityValue = ptr[med].charCode;
              found = TRUE;
            }
          else
            {
              if (rescomp > 0)
                sup = med - 1;
              else
                inf = med + 1;
            }
        }
    }
  if (!found && ptr == XhtmlEntityTable)
    // check MathML entities
    return MapXMLEntity (MATH_TYPE, entityName, entityValue);
  return found;
}

/*----------------------------------------------------------------------
  MapEntityByCode
  Generic function which searchs in the Entity Mapping Table (table)
  the entry with code entityValue and give the corresponding name.
  withMath is TRUE when MathML entities are accepted.
  Returns FALSE if entityValue is not found.
  ----------------------------------------------------------------------*/
void MapEntityByCode (int entityValue, Document doc, ThotBool withMath,
                      char **entityName)
{
  XmlEntity  *ptr;
  ThotBool    found;
  int         i;

  /* Select the right table */
  if (withMath)
    /* look for in the Math entities table */
    ptr = MathEntityTable;
  else
    ptr = XhtmlEntityTable;
  if (ptr)
    {
      /* look for in the HTML entities table */
      found = FALSE;
      while (ptr && !found)
        {
          for (i = 0; ptr[i].charCode != 0 && !found; i++)
            found = (ptr[i].charCode == entityValue);
  
          if (found)
            {
              /* entity value found */
              i--;
              *entityName = (char *) (ptr[i].charName);
            }
          else if (withMath && ptr != XhtmlEntityTable)
            /* look for in the Math entities table */
            ptr = XhtmlEntityTable;
          else
            {
              *entityName = NULL;
              ptr = NULL;
            }
        }
    }
  else
    *entityName = NULL;
}
