/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2005
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

#define THOT_EXPORT extern
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
static void LowercaseGI (char *GI, char *gi)
{
  int        i;

  for (i = 0; GI[i] != EOS && i < MaxTypeNameLength-1; i++)
    {
      if (GI[i] >= 'A' && GI[i] <= 'Z')
	gi[i] = (char) ((int) GI[i] + 32);
      else
	gi[i] = GI[i];
    }
  gi[i] = EOS;
}


/*----------------------------------------------------------------------
   MapGI
   search in the mapping tables the entry for the element of
   name GI and returns the rank of that entry.
   When returning, schema contains the Thot SSchema that defines that element,
   Returns -1 and schema = NULL if not found.
  ----------------------------------------------------------------------*/
int MapGI (char *gi, SSchema *schema, Document doc)
{
  ElementType     elType;
  char           *ptr; 
  char            c;
  int             i;
  int             entry;
  ThotBool	  isHTML; 
  ThotBool	  level; 

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
      isHTML = !strcmp (ptr, "HTML");
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
      c = tolower (gi[0]);
      /* look for the first concerned entry in the table */
      while (pHTMLGIMapping[i].XMLname[0] < c
	     && pHTMLGIMapping[i].XMLname[0] != EOS)
	i++;

      /* look at all entries starting with the right character */
      do
	{
	  if (strcasecmp (pHTMLGIMapping[i].XMLname, gi))
	    i++;
	  else
	    entry = i;
	}
      while (entry < 0 && pHTMLGIMapping[i].XMLname[0] == c);
    }

  if (entry < 0)
    {
      if (*schema != NULL && isHTML)
	*schema = NULL;
      else
	/* not found. Look at the XML mapping tables */
	{
	  elType.ElTypeNum = 0;
	  elType.ElSSchema = *schema;
	  
	  if (!ptr || !strcmp (ptr, "MathML"))
	    MapXMLElementType (MATH_TYPE, gi, &elType, &ptr, &c, &level, doc);
	  if (elType.ElTypeNum == 0 && (!ptr || !strcmp (ptr, "SVG")))
	    MapXMLElementType (SVG_TYPE, gi, &elType, &ptr, &c, &level, doc);
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
    }
  return entry;
}

/*----------------------------------------------------------------------
   GIType  search in mapping tables the Element type associated with
   a given GI Name. If not found returns zero.
  ----------------------------------------------------------------------*/
void GIType (const char *gi, ElementType *elType, Document doc)
{
  char              c;
  char             *ptr;
  int               i;
  ThotBool	    level; 

  /* TODO: use NameSpaces to search in the right table */
  elType->ElSSchema = NULL;
  elType->ElTypeNum = 0;

  /*
    First convert the first char into lower case to locate
    registered tags in the HTML mapping table.
    Entries are registered in upper case and in alphabetic order.
  */

  /* TODO: define a function which works on unicode */
  c = tolower (gi[0]);

  i = 0;
  /* look for the first concerned entry in the table */
  while (pHTMLGIMapping[i].XMLname[0] < c &&
	 pHTMLGIMapping[i].XMLname[0] != EOS)
    i++;
  /* look at all entries starting with the right character */
  do
    {
      if (!strcasecmp (pHTMLGIMapping[i].XMLname, gi))
      {
	if (doc != 0)
        elType->ElSSchema = TtaGetSSchema ("HTML", doc);
	elType->ElTypeNum = pHTMLGIMapping[i].ThotType;
	return;
      }
      i++;
    }
  while (pHTMLGIMapping[i].XMLname[0] == c);

  /* if not found, look at the XML mapping tables */
  MapXMLElementType (MATH_TYPE, gi, elType, &ptr, &c, &level, doc);
  if (elType->ElTypeNum == 0)
  MapXMLElementType (SVG_TYPE, gi, elType, &ptr, &c, &level, doc);
}

/*----------------------------------------------------------------------
   MapAttr search in all AttributeMappingTables the entry for the
   attribute of name Attr and returns a pointer to that entry,
   as well as the corresponding Thot SSchema
  ----------------------------------------------------------------------*/
AttributeMapping   *MapAttr (char *attrName, SSchema *schema,
			     int elemEntry, ThotBool *level, Document doc)
{
  typeName          attr, elem;
  int               i;
  int               thotType;

  *schema = TtaGetDocumentSSchema (doc);
  LowercaseGI (attrName, attr);
  LowercaseGI (pHTMLGIMapping[elemEntry].XMLname, elem);
  i = MapXMLAttribute (XHTML_TYPE, attr, elem, level, doc, &thotType);
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
AttributeMapping *MapHTMLAttribute (const char *attrName,
				    AttributeType *attrType,
				    const char *elementName,
				    ThotBool *level,
				    Document doc)
{
  int             i;

  attrType->AttrSSchema = GetXHTMLSSchema (doc);
  i = MapXMLAttribute (XHTML_TYPE, attrName, elementName,
		       level, doc, &(attrType->AttrTypeNum));
  if (i < 0)
    return (NULL);
  else
    return (&(pHTMLAttributeMapping[i]));
}

