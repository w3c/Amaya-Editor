/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2000
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 *
 * fetchXMLname
 *
 * Author: I. Vatton
 *
 */

#define THOT_EXPORT extern
#include "amaya.h"
#include "parser.h"
#include "HTMLnames.h"
#include "MathMLnames.h"
#include "GraphMLnames.h"
#include "XLinknames.h"

/* define a pointer to let other parser functions access the local table */
int               HTML_ENTRIES = (sizeof(XHTMLElemMappingTable) / sizeof(ElemMapping));
ElemMapping      *pHTMLGIMapping = XHTMLElemMappingTable;
AttributeMapping *pHTMLAttributeMapping = XHTMLAttributeMappingTable;


#include "fetchXMLname_f.h"

/*----------------------------------------------------------------------
   GetXHTMLSSchema returns the XHTML Thot schema for document doc.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
SSchema            GetXHTMLSSchema (Document doc)
#else
SSchema            GetXHTMLSSchema (doc)
Document	   doc;

#endif
{
  SSchema	XHTMLSSchema;

   XHTMLSSchema = TtaGetSSchema (TEXT("HTML"), doc);
   if (XHTMLSSchema == NULL)
       XHTMLSSchema = TtaNewNature(doc, TtaGetDocumentSSchema(doc),
				    TEXT("HTML"), TEXT("HTMLP"));
   return (XHTMLSSchema);
}

/*----------------------------------------------------------------------
   GetMathMLSSchema returns the MathML Thot schema for document doc.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
SSchema            GetMathMLSSchema (Document doc)
#else
SSchema            GetMathMLSSchema (doc)
Document	   doc;

#endif
{
  SSchema	MathMLSSchema;

  MathMLSSchema = TtaGetSSchema (TEXT("MathML"), doc);
  if (MathMLSSchema == NULL)
     MathMLSSchema = TtaNewNature(doc, 
				  TtaGetDocumentSSchema(doc), TEXT("MathML"),
				  TEXT("MathMLP"));
  return (MathMLSSchema);
}

/*----------------------------------------------------------------------
   GetGraphMLSSchema returns the GraphML Thot schema for document doc.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
SSchema            GetGraphMLSSchema (Document doc)
#else
SSchema            GetGraphMLSSchema (doc)
Document	   doc;

#endif
{
  SSchema	GraphMLSSchema;

  GraphMLSSchema = TtaGetSSchema (TEXT("GraphML"), doc);
  if (GraphMLSSchema == NULL)
    GraphMLSSchema = TtaNewNature(doc,
				  TtaGetDocumentSSchema(doc), TEXT("GraphML"),
				  TEXT("GraphMLP"));
  return (GraphMLSSchema);
}

/*----------------------------------------------------------------------
   GetXLinkSSchema returns the XLink Thot schema for document doc.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
SSchema            GetXLinkSSchema (Document doc)
#else
SSchema            GetXLinkSSchema (doc)
Document	   doc;

#endif
{
  SSchema	XLinkSSchema;

  XLinkSSchema = TtaGetSSchema (TEXT("XLink"), doc);
  if (XLinkSSchema == NULL)
    XLinkSSchema = TtaNewNature(doc, TtaGetDocumentSSchema(doc), TEXT("XLink"),
				TEXT("XLinkP"));
  return (XLinkSSchema);
}

/*----------------------------------------------------------------------
   GetXMLSSchema returns the XML Thot schema for document doc.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
SSchema            GetXMLSSchema (int XMLtype, Document doc)
#else
SSchema            GetXMLSSchema (XMLtype, doc)
Document	   doc;
int                XMLtype;
#endif
{
  if (XMLtype == XHTML_TYPE)
    return GetXHTMLSSchema (doc);
  else if (XMLtype == MATH_TYPE)
    return GetMathMLSSchema (doc);
  else if (XMLtype == GRAPH_TYPE)
    return GetGraphMLSSchema (doc);
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
    - content 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void               MapXMLElementType (int XMLtype,
				      STRING XMLname,
				      ElementType *elType,
				      STRING *mappedName,
				      CHAR_T *content,
				      Document doc)
#else
void               MapXMLElementType (XMLtype,
				      XMLname,
				      elType,
				      mappedName,
				      content,
				      doc)
int                XMLtype;
STRING             XMLname;
ElementType       *elType;
STRING            *mappedName;
CHAR_T  	  *content;
Document           doc;
#endif
{
   int                 i;
   ElemMapping        *ptr;

   /* Select the right table */
   if (XMLtype == XHTML_TYPE)
     ptr = XHTMLElemMappingTable;
   else if (XMLtype == MATH_TYPE)
     ptr = MathMLElemMappingTable;
   else if (XMLtype == GRAPH_TYPE)
     ptr = GraphMLElemMappingTable;
   else
     ptr = NULL;
   *mappedName = NULL;
   elType->ElTypeNum = 0;
   if (ptr != NULL)
     {
       /* search in the ElemMappingTable */
       i = 0;
       /* look for the first concerned entry in the table */
       while (ptr[i].XMLname[0] < XMLname[0] && ptr[i].XMLname[0] != WC_EOS)
	 i++;

       /* look at all entries starting with the right character */
       do
	 if (ustrcmp (ptr[i].XMLname, XMLname) || ptr[i].Level > ParsingLevel[doc])
	   /* it's not the tag or this tag is not valid for the current parsing level */
	   i++;
	 else
	   {
	     elType->ElTypeNum = ptr[i].ThotType;
	     if (elType->ElSSchema == NULL)
	       elType->ElSSchema = GetXMLSSchema (XMLtype, doc);
	     *mappedName = ptr[i].XMLname;
	     *content = ptr[i].XMLcontents;
	   }
       while (elType->ElTypeNum <= 0 && ptr[i].XMLname[0] == XMLname[0]);
     }
}


/*----------------------------------------------------------------------
   GetXMLElementName
   Generic function which searchs in the mapping table the XML name for
   a given Thot type.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
CHAR_T*           GetXMLElementName (ElementType elType, Document doc)
#else
CHAR_T*           GetXMLElementName (elType, doc)
ElementType       elType;
Document          doc;
#endif
{
  ElemMapping        *ptr;
  STRING              name;
  int                 i;
  ThotBool            invalid = FALSE;

  if (elType.ElTypeNum > 0)
    {
      i = 0;
      /* Select the table which matches with the element schema */
      name = TtaGetSSchemaName (elType.ElSSchema);
      if (ustrcmp (TEXT("MathML"), name) == 0)
	ptr = MathMLElemMappingTable;
      else if (ustrcmp (TEXT("GraphML"), name) == 0)
	ptr = GraphMLElemMappingTable;
      else
	ptr = XHTMLElemMappingTable;
      
      if (ptr)
	do
	  {
	    if (ptr[i].ThotType == elType.ElTypeNum)
	      {
		if (doc == 0 || ptr[i].Level <= ParsingLevel[doc])
		  return ptr[i].XMLname;
		else
		  invalid = TRUE;
	      }
	    i++;
	  }
	while (ptr[i].XMLname[0] != WC_EOS);	  
    }
  if (invalid)
    return TEXT("");
  else
    return TEXT("???");
}


/*----------------------------------------------------------------------
   MapXMLAttribute
   Generic function which searchs in the Attribute Mapping Table (table)
   the entry attrName associated to the element elementName.
   Returns the corresponding entry or -1.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int               MapXMLAttribute (int XMLtype,
				   CHAR_T *attrName,
				   CHAR_T *elementName,
				   Document doc,
				   int *thotType)
#else
int               MapXMLAttribute (XMLtype,
				   attrName,
				   elementName,
				   doc,
				   thotType)
int               XMLtype;
CHAR_T           *attrName;
CHAR_T           *elementName;
Document          doc;
int              *thotType;
#endif
{
  int               i;
  AttributeMapping *ptr;

   /* Select the right table */
   if (XMLtype == XHTML_TYPE)
     ptr = XHTMLAttributeMappingTable;
   else if (XMLtype == MATH_TYPE)
     ptr = MathMLAttributeMappingTable;
   else if (XMLtype == GRAPH_TYPE)
     ptr = GraphMLAttributeMappingTable;
   else if (XMLtype == XLINK_TYPE)
     ptr = XLinkAttributeMappingTable;
   else
     ptr = NULL;

  i = 1;
  *thotType = 0;
  if (ptr == NULL)
    return -1;

  /* look for the first concerned entry in the table */
  while (ptr[i].XMLattribute[0] < attrName[0] && ptr[i].XMLattribute[0] != WC_EOS)
    i++;
  while (ptr[i].XMLattribute[0] == attrName[0])
    {
      if (ptr[i].Level > ParsingLevel[doc] ||
	  ustrcmp (ptr[i].XMLattribute, attrName) ||
	  (ptr[i].XMLelement[0] != WC_EOS && ustrcmp (ptr[i].XMLelement, elementName)))
	i++;
      else
	{
	  *thotType = ptr[i].ThotAttribute;
	  return (i);
	}
    }
  return (-1);
}


/*----------------------------------------------------------------------
   GetXMLElementName
   Generic function which searchs in the mapping table the XML name for
   a given Thot type.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
CHAR_T*           GetXMLAttributeName (AttributeType attrType, Document doc)
#else
CHAR_T*           GetXMLAttributeName (attrType, doc)
AttributeType     attrType;
Document          doc;
#endif
{
  AttributeMapping   *ptr;
  STRING              name;
  int                 i;
  ThotBool            invalid = FALSE;

  if (attrType.AttrTypeNum > 0)
    {
      i = 0;
      /* Select the table which matches with the element schema */
      name = TtaGetSSchemaName (attrType.AttrSSchema);
      if (ustrcmp (TEXT("MathML"), name) == 0)
	ptr = MathMLAttributeMappingTable;
      else if (ustrcmp (TEXT("GraphML"), name) == 0)
	ptr = GraphMLAttributeMappingTable;
      else if (ustrcmp (TEXT("XLink"), name) == 0)
	ptr = XLinkAttributeMappingTable;
      else
	ptr = XHTMLAttributeMappingTable;
      
      if (ptr)
	do
	  {
	    if (ptr[i].ThotAttribute == attrType.AttrTypeNum)
	      {
		if (doc == 0 || ptr[i].Level <= ParsingLevel[doc])
		  return ptr[i].XMLattribute;
		else
		  invalid = TRUE;
	      }
	    i++;
	  }
	while (ptr[i].XMLattribute[0] != WC_EOS);	  
    }
  if (invalid)
    return TEXT("");
  else
    return TEXT("???");
}
