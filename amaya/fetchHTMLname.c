/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2000
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * fetchHTMLname
 *
 * Author: I. Vatton
 *
 */
 
/* This module is used by the html2thot parser and the css parser. */

#define THOT_EXPORT
#include "amaya.h"
#include "fetchHTMLname.h"
#include "parser.h"

#include "fetchHTMLname_f.h"
#include "fetchXMLname_f.h"


/*----------------------------------------------------------------------
  LowercaseGI converts uppercases into lovercases
  GI is the input string
  gi is the output string
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void      LowercaseGI (CHAR_T *GI, CHAR_T *gi)
#else
static void      LowercaseGI (GI, gi)
CHAR_T          *GI;
CHAR_T          *gi;
#endif
{
  int        i;

  for (i = 0; GI[i] != WC_EOS; i++)
    {
      if (GI[i] >= TEXT('A') && GI[i] <= TEXT('Z'))
	gi[i] = (CHAR_T) ((int) GI[i] + 32);
      else
	gi[i] = GI[i];
    }
  gi[i] = WC_EOS;
}


/*----------------------------------------------------------------------
   MapGI
   search in the mapping tables the entry for the element of
   name GI and returns the rank of that entry.
   When returning, schema contains the Thot SSchema that defines that element,
   Returns -1 and schema = NULL if not found.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 MapGI (CHAR_T *gi, SSchema *schema, Document doc)
#else
int                 MapGI (gi, schema, doc)
CHAR_T*             gi;
SSchema*            schema;
Document            doc;
#endif
{
  ElementType     elType;
  CHAR_T*         ptr; 
  CHAR_T          c;
  int             i;
  int             entry;
  ThotBool	  isHTML;

  /* TODO: use NameSpaces to search in the right table */
  entry = -1;
  if (*schema == NULL)
    {
      isHTML = FALSE;
      ptr = NULL;
    }
  else
    {
      ptr = TtaGetSSchemaName (*schema);
      isHTML = !ustrcmp (ptr, TEXT("HTML"));
    }

  i = 0;
  if (*schema == NULL || isHTML)
    {
      /*
	First convert the first char into lower case to locate
	registered tags in the HTML mapping table.
	Entries are registered in upper case and in alphabetic order.
      */

      /* TODO: define a function which works on unicode */
      c = utolower (gi[0]);
      /* look for the first concerned entry in the table */
      while (pHTMLGIMapping[i].XMLname[0] < c
	     && pHTMLGIMapping[i].XMLname[0] != EOS)
	i++;

      /* look at all entries starting with the right character */
      do
	{
	  if (ustrcasecmp (pHTMLGIMapping[i].XMLname, gi))
	    i++;
	  else
	    entry = i;
	}
      while (entry < 0 && pHTMLGIMapping[i].XMLname[0] == c);
    }

  if (entry < 0)
    if (*schema != NULL && isHTML)
      *schema = NULL;
    else
      /* not found. Look at the XML mapping tables */
      {
	elType.ElTypeNum = 0;
	elType.ElSSchema = *schema;

	if (!ptr || !ustrcmp (ptr, TEXT("MathML")))
	  MapXMLElementType (MATH_TYPE, gi, &elType, &ptr, &c, doc);
	if (elType.ElTypeNum == 0 && (!ptr || !ustrcmp (ptr, TEXT("GraphML"))))
	  MapXMLElementType (GRAPH_TYPE, gi, &elType, &ptr, &c, doc);
	if (elType.ElTypeNum == 0)
	  {
            entry = -1;
	    elType.ElSSchema = NULL;
	    *schema = NULL;
	  }
	else
	  {
            entry = elType.ElTypeNum;
            *schema = elType.ElSSchema;
	  }
      }
  return entry;
}

/*----------------------------------------------------------------------
   GIType  search in mapping tables the Element type associated with
   a given GI Name. If not found returns zero.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GIType (CHAR_T* gi, ElementType *elType, Document doc)
#else
void                GIType (gi, elType, doc)
CHAR_T*             gi;
ElementType*        elType;
Document            doc;
#endif
{
  CHAR_T              c;
  CHAR_T*             ptr;
  int                 i;

  /* TODO: use NameSpaces to search in the right table */
  elType->ElSSchema = NULL;
  elType->ElTypeNum = 0;

  /*
    First convert the first char into lower case to locate
    registered tags in the HTML mapping table.
    Entries are registered in upper case and in alphabetic order.
  */

  /* TODO: define a function which works on unicode */
  c = utolower (gi[0]);

  i = 0;
  /* look for the first concerned entry in the table */
  while (pHTMLGIMapping[i].XMLname[0] < c &&
	 pHTMLGIMapping[i].XMLname[0] != EOS)
    i++;
  /* look at all entries starting with the right character */
  do
    {
      if (!ustrcasecmp (pHTMLGIMapping[i].XMLname, gi))
      {
	if (doc != 0)
        elType->ElSSchema = TtaGetSSchema (TEXT("HTML"), doc);
	elType->ElTypeNum = pHTMLGIMapping[i].ThotType;
	return;
      }
      i++;
    }
  while (pHTMLGIMapping[i].XMLname[0] == c);

  /* if not found, look at the XML mapping tables */
  MapXMLElementType (MATH_TYPE, gi, elType, &ptr, &c, doc);
  if (elType->ElTypeNum == 0)
  MapXMLElementType (GRAPH_TYPE, gi, elType, &ptr, &c, doc);
}

/*----------------------------------------------------------------------
   MapAttr search in all AttributeMappingTables the entry for the
   attribute of name Attr and returns a pointer to that entry,
   as well as the corresponding Thot SSchema
  ----------------------------------------------------------------------*/
#ifdef __STDC__
AttributeMapping   *MapAttr (CHAR_T *attrName, SSchema *schema, int elemEntry, Document doc)
#else
AttributeMapping   *MapAttr (attrName, schema, elemEntry, doc)
CHAR_T             *attrName;
SSchema            *schema;
int                 elemEntry;
Document            doc;
#endif
{
  typeName          attr, elem;
  int               i;
  int               thotType;

  *schema = TtaGetDocumentSSchema (doc);
  LowercaseGI (attrName, attr);
  LowercaseGI (pHTMLGIMapping[elemEntry].XMLname, elem);
  i = MapXMLAttribute (XHTML_TYPE, attr, elem, doc, &thotType);
  if (i < 0)
    /* not found */
    return (NULL);
  else
    return (&(pHTMLAttributeMapping[i]));
}


/*----------------------------------------------------------------------
   MapHTMLAttribute
   Search in the Attribute Mapping Table the entry for the attribute
   of name Attr and returns the corresponding Thot attribute type.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
AttributeMapping *MapHTMLAttribute (CHAR_T *attrName,
				    AttributeType *attrType,
				    CHAR_T *elementName,
				    Document doc)
#else
AttributeMapping *MapHTMLAttribute (attrName,
				    attrType,
				    elementName,
				     doc)
CHAR_T*           attrName;
AttributeType*    attrType;
CHAR_T*           elementName;
Document          doc;
#endif
{
  int                 i;

  attrType->AttrSSchema = GetXHTMLSSchema (doc);
  i = MapXMLAttribute (XHTML_TYPE, attrName, elementName, doc, &(attrType->AttrTypeNum));
  if (i < 0)
    return (NULL);
  else
    return (&(pHTMLAttributeMapping[i]));
}

