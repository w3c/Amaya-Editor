/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2002
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

/* define some pointers to let other parser functions access the local table */
int               HTML_ENTRIES = (sizeof(XHTMLElemMappingTable) / sizeof(ElemMapping));
ElemMapping      *pHTMLGIMapping = XHTMLElemMappingTable;
AttributeMapping *pHTMLAttributeMapping = XHTMLAttributeMappingTable;

XmlEntity        *pXhtmlEntityTable = XhtmlEntityTable;
XmlEntity        *pMathEntityTable = MathEntityTable;

#include "fetchXMLname_f.h"

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
       XHTMLSSchema = TtaNewNature(doc, TtaGetDocumentSSchema(doc),
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
				  TtaGetDocumentSSchema(doc),
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
    SVGSSchema = TtaNewNature(doc,
				  TtaGetDocumentSSchema(doc),
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
    XLinkSSchema = TtaNewNature(doc, TtaGetDocumentSSchema(doc),
				"XLink", "XLinkP");
  return (XLinkSSchema);
}

/*----------------------------------------------------------------------
   GetGenericXMLSSchema returns the XML Thot schema for the document doc.
  ----------------------------------------------------------------------*/
SSchema GetGenericXMLSSchema (Document doc)

{
  SSchema	XMLSSchema;

  XMLSSchema = TtaGetSSchema ("XML", doc);
  if (XMLSSchema == NULL)
      XMLSSchema = TtaNewNature(doc, TtaGetDocumentSSchema(doc),
				"XML", "XMLP");
  return (XMLSSchema);
}

/*----------------------------------------------------------------------
   GetTextSSchema returns the TextFile Thot schema for document doc.
   (this is not XML, but its useful to have this function here).
  ----------------------------------------------------------------------*/
SSchema         GetTextSSchema (Document doc)

{
  SSchema	XLinkSSchema;

  XLinkSSchema = TtaGetSSchema ("TextFile", doc);
  if (XLinkSSchema == NULL)
    XLinkSSchema = TtaNewNature(doc, TtaGetDocumentSSchema(doc),
				"TextFile", "TextFileP");
  return (XLinkSSchema);
}


/*----------------------------------------------------------------------
   GetGenericXMLSSchemaByUri returns the XML Thot schema for the document doc.
  ----------------------------------------------------------------------*/
SSchema GetGenericXMLSSchemaByUri (char *uriName, Document doc, ThotBool *isnew)

{
  SSchema	XMLSSchema;

  XMLSSchema = TtaGetSSchemaByUri (uriName, doc);
  if (XMLSSchema == NULL)
    {
      XMLSSchema = TtaNewNature(doc, TtaGetDocumentSSchema(doc),
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
  else
    return NULL;
}

/*----------------------------------------------------------------------
  MapXMLElementType
  Generic function which searchs in the Element Mapping table, selected
  by the parameter XMLtype, the entry XMLname and returns the corresponding
  Thot element type.
  Returns:
    - ElTypeNum and ElSSchema into elType  ElTypeNum = 0 if not found.
    - content information about this entry
    - checkProfile TRUE if the entry is valid for the current Doc profile.
  ----------------------------------------------------------------------*/
void MapXMLElementType (int XMLtype, char *XMLname, ElementType *elType,
			char **mappedName, char *content,
			ThotBool *checkProfile, Document doc)
{
  ElemMapping        *ptr;
  char                c;
  int                 i;

  /* Initialize variables */
  *mappedName = NULL;
  *checkProfile = TRUE;
  elType->ElTypeNum = 0;

  /* Select the right table */
  if (XMLtype == XHTML_TYPE)
    ptr = XHTMLElemMappingTable;
  else if (XMLtype == MATH_TYPE)
    {
      if (TtaGetDocumentProfile(doc) == L_Basic &&
	  DocumentTypes[doc] == docHTML)
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
      if (TtaGetDocumentProfile(doc) == L_Basic &&
	  DocumentTypes[doc] == docHTML)
	{
	  /* Graphics are not allowed in this document */
	  ptr = NULL;
	  *checkProfile = FALSE;
	}
      else
	ptr = SVGElemMappingTable;
    }
  else
    ptr = NULL;
   
  if (ptr)
    {
      /* search in the ElemMappingTable */
      i = 0;
      /* case insensitive for HTML */
      if (ptr == XHTMLElemMappingTable)
	c = tolower (XMLname[0]);
      else
	c = XMLname[0];
      /* look for the first concerned entry in the table */
      while (ptr[i].XMLname[0] < c && ptr[i].XMLname[0] != EOS)
	i++;     
      /* look at all entries starting with the right character */
      do
	if (ptr == XHTMLElemMappingTable && strcasecmp (ptr[i].XMLname, XMLname))
	  /* it's not the tag */
	  i++;
	else if (ptr != XHTMLElemMappingTable && strcmp (ptr[i].XMLname, XMLname))
	  /* it's not the tag */
	  i++;
	else if (TtaGetDocumentProfile(doc) != L_Other &&
		 !(ptr[i].Level & TtaGetDocumentProfile(doc)))
	  {
	    /* this tag is not valid in the document profile */
	    *checkProfile = FALSE;
	    i++;
	  }
	else
	  {
	    elType->ElTypeNum = ptr[i].ThotType;
	    if (elType->ElSSchema == NULL)
	      elType->ElSSchema = GetXMLSSchema (XMLtype, doc);
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
char *GetXMLElementName (ElementType elType, Document doc)
{
  ElemMapping  *ptr;
  char         *name;
  int           i;
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
      else
	ptr = XHTMLElemMappingTable;
      
      if (ptr)
	do
	  {
	    if (ptr[i].ThotType == elType.ElTypeNum)
	      {
		if (doc == 0 || 
		    TtaGetDocumentProfile(doc) == L_Other ||
		    (ptr[i].Level & TtaGetDocumentProfile(doc)))
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
   MapMathMLAttributeValue
   Search in the Attribute Value Mapping Table the entry for the attribute
   ThotAtt and its value attVal. Returns the corresponding Thot value.
  ----------------------------------------------------------------------*/
void MapXMLAttributeValue (int XMLtype, char *attVal, AttributeType attrType,
			      int *value)
{
  AttrValueMapping   *ptr;
  int                 i;

  /* Select the right table */
  if (XMLtype == XHTML_TYPE)
    ptr = XHTMLAttributeMappingTable;
  else if (XMLtype == MATH_TYPE)
    ptr = MathMLAttrValueMappingTable;
  else if (XMLtype == SVG_TYPE)
    ptr = SVGAttributeMappingTable;
  else if (XMLtype == XLINK_TYPE)
    ptr = XLinkAttributeMappingTable;
  else
    ptr = NULL;
  
  *value = 0;
  i = 0;
  if (ptr == NULL)
    return;
  while (ptr[i].ThotAttr != attrType.AttrTypeNum && ptr[i].ThotAttr != 0)
    i++;
  if (ptr[i].ThotAttr == attrType.AttrTypeNum)
    do
      if (!strcmp (ptr[i].XMLattrValue, attVal))
	*value = ptr[i].ThotAttrValue;
      else
	i++;
    while (*value == 0 && ptr[i].ThotAttr == attrType.AttrTypeNum);
}


/*----------------------------------------------------------------------
   MapXMLAttribute
   Generic function which searchs in the Attribute Mapping Table (table)
   the entry attrName associated to the element elementName.
   Returns the corresponding entry or -1.
  ----------------------------------------------------------------------*/
int MapXMLAttribute (int XMLtype, char *attrName, char *elementName,
		     ThotBool *checkProfile, Document doc, int *thotType)
{
  AttributeMapping   *ptr;
  char                c;
  int                 i;

  /* Initialization */
  *checkProfile = TRUE;
  i = 1;
  *thotType = 0;
  
  /* Select the right table */
  if (XMLtype == XHTML_TYPE)
    ptr = XHTMLAttributeMappingTable;
  else if (XMLtype == MATH_TYPE)
    ptr = MathMLAttributeMappingTable;
  else if (XMLtype == SVG_TYPE)
    ptr = SVGAttributeMappingTable;
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
  if (ptr == XHTMLAttributeMappingTable)
    c = tolower (attrName[0]);
  else
    c = attrName[0];
  /* look for the first concerned entry in the table */
  while (ptr[i].XMLattribute[0] < c &&  ptr[i].XMLattribute[0] != EOS)
    i++;
  while (ptr[i].XMLattribute[0] == c)
    {
      if (ptr == XHTMLAttributeMappingTable &&
	  (strcasecmp (ptr[i].XMLattribute, attrName) ||
	   (ptr[i].XMLelement[0] != EOS &&
	    strcasecmp (ptr[i].XMLelement, elementName))))
	i++;
      else if (ptr != XHTMLAttributeMappingTable &&
	  (strcmp (ptr[i].XMLattribute, attrName) ||
	   (ptr[i].XMLelement[0] != EOS &&
	    strcmp (ptr[i].XMLelement, elementName))))
	i++;
      else if (TtaGetDocumentProfile(doc) != L_Other &&
	       !(ptr[i].Level & TtaGetDocumentProfile(doc)))
	{
	  *checkProfile = FALSE;
	  i++;
	}
      else
	{
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
char *GetXMLAttributeName (AttributeType attrType, ElementType elType,
			   Document doc)
{
  AttributeMapping   *ptr;
  char               *name, *tag;
  int                 i;
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
      else if (strcmp ("SVG", name) == 0)
	ptr = SVGAttributeMappingTable;
      else if (strcmp ("XLink", name) == 0)
	ptr = XLinkAttributeMappingTable;
      else
	ptr = XHTMLAttributeMappingTable;
      
      if (ptr)
	do
	  {
	    if (ptr[i].ThotAttribute == attrType.AttrTypeNum &&
		(ptr[i].XMLelement[0] == EOS ||
		 !strcmp (ptr[i].XMLelement, tag)))
	      {
		if (doc != 0 &&
		    TtaGetDocumentProfile(doc) != L_Other &&
		    !(ptr[i].Level & TtaGetDocumentProfile(doc)))
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
   MapXMLEntity
   Generic function which searchs in the Entity Mapping Table (table)
   the entry entityName and give the corresponding decimal value.
   Returns FALSE if entityName is not found.
  ----------------------------------------------------------------------*/
ThotBool      MapXMLEntity (int XMLtype, char *entityName, int *entityValue)
{
  XmlEntity  *ptr;
  ThotBool    found;
  int         inf, sup, med, rescomp;

  /* Initialization */
  found = FALSE;
  sup = 0;

  /* Select the right table */
  if (XMLtype == XHTML_TYPE)
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
  
  if (ptr == NULL)
    return found;

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
  return found;
 }

/*----------------------------------------------------------------------
   MapEntityByCode
   Generic function which searchs in the Entity Mapping Table (table)
   the entry with code entityValue and give the corresponding name.
   Returns FALSE if entityValue is not found.
  ----------------------------------------------------------------------*/
void MapEntityByCode (int entityValue, char **entityName)
{
  XmlEntity  *ptr;
  ThotBool    found;
  int         i;

  /* Select the right table */
  ptr = XhtmlEntityTable;
  if (ptr)
    {
      /* look for in the HTML entities table */
      found = FALSE;
      while (ptr && !found)
	{
	  for (i = 0; ptr[i].charCode >= 0 && !found; i++)
	    found = (ptr[i].charCode == entityValue);
  
	  if (found)
	    {
	      /* entity value found */
	      i--;
	      *entityName = (char *) (ptr[i].charName);
	    }
	  else if (ptr != MathEntityTable)
	    /* look for in the Math entities table */
	    ptr = MathEntityTable;
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
