/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 2000.
 *  Please first read the full copyright statement in file COPYRIGHT.
 * 
 */

/*
 * rdfparse.c : parses an annotation RDF file  and intializes
 *              the corresponding memory elements. 
 *              We first build a memory XML tree from the RDF file and
 *              then we browse it using the thotlib's DOM-like functions.
 *
 * Author: J. Kahan (W3C/INRIA)
 *
 */

/* Fonction: ajouter du text (CDATA)
   mettre le element + le tag
*/

#include "annotlib.h"
#include "XmlStr.h"
#include "xmlparse.h"

/* size of the file input buffer */
#define CHUNK_SIZE 8192

/* information about the Thot document under construction */
static Document     ParsedDocument = 0;   /* the Thot document */
static Language     CurrentLanguage;      /* language used in the document */
static SSchema      XmlSSchema = NULL;    /* the HTML structure schema */
static Element      RootElement;          /* root element of the document */

static Element CurrentElement = NULL;
static Element CurrentAttribute = NULL;
static ElementType ContentsElementType;
static ElementType ComponentElementType;
static ElementType PcDataElementType;
static ElementType TextElementType;
static ElementType TagElementType;
static ElementType AttributeElementType;
static ElementType AttributesElementType;
static ElementType AttrNameElementType;
static ElementType AttrValueElementType;
static AttributeType GiAttributeType;

/*----------------------------------------------------------------------
   CloseElement     Closes the element elem
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool        CloseElement (Element elem)
#else
static ThotBool        CloseElement (elem)
Element elem;
#endif

{
  if (CurrentElement != NULL)
    CurrentElement = TtaGetParent (CurrentElement);
  while (CurrentElement != NULL &&
         !TtaSameTypes (TtaGetElementType (CurrentElement), TagElementType) &&
         CurrentElement != RootElement)
    CurrentElement = TtaGetParent (CurrentElement);
  return (CurrentElement != NULL);
}

/*----------------------------------------------------------------------
   ProcessStartGI  An HTML GI has been read in a start tag.
   Create the corresponding  attribute,
   ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ProcessStartGI (char *GIname)
#else
static void         ProcessStartGI (GIname)
char               *GIname;

#endif
{
  Attribute giAttr;
 
  giAttr = TtaGetAttribute (CurrentElement, GiAttributeType);
  if (giAttr == NULL)
    {
      giAttr = TtaNewAttribute (GiAttributeType);
      TtaAttachAttribute (CurrentElement, giAttr, ParsedDocument);
    }
  TtaSetAttributeText (giAttr, GIname, CurrentElement, ParsedDocument);
}

/*----------------------------------------------------------------------
   InsertElementName   Inserts an Element Name
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         InsertElementName (char *el)
#else
static void         InsertElementName (el)
char                *el;
#endif
{
  Element elContents, elAttributes, newElement, elPrevious;

  /* choose where to add the element */
  if (CurrentElement != NULL)
    {
      elContents = TtaGetLastChild (CurrentElement);
      if (elContents == NULL)
	{
	  elContents = TtaNewElement (ParsedDocument, ContentsElementType);
	  TtaInsertFirstChild (&elContents, CurrentElement, ParsedDocument);
	}
      if (!TtaSameTypes (TtaGetElementType (elContents), ContentsElementType))
	{
	  elAttributes = elContents;
	  elContents = TtaNewElement (ParsedDocument, ContentsElementType);
	  TtaInsertSibling (elContents, elAttributes, FALSE, ParsedDocument);
	}
      if (elContents != NULL)
	{
	  elPrevious  = TtaGetLastChild (elContents);
	  newElement = TtaNewElement (ParsedDocument, TagElementType);
	  if (newElement != NULL)
	    {
	      if (elPrevious == NULL)
		{
		  elPrevious = TtaNewElement (ParsedDocument, 
					      ComponentElementType);
		  TtaInsertFirstChild (&elPrevious, elContents, 
				       ParsedDocument);
		  TtaInsertFirstChild (&newElement, elPrevious,
				       ParsedDocument);
		}
	      else
		TtaInsertSibling (newElement, elPrevious, FALSE,
				  ParsedDocument);
	      CurrentElement = newElement;
	      CurrentAttribute = NULL;
	    }
	}
    }
  else
    CurrentElement = RootElement;

  /* insert the element name */
  ProcessStartGI (el);
}

/*----------------------------------------------------------------------
   InsertAttrName   
   A XML attribute has been read. Create the 
   corresponding Thot attribute.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         InsertAttrName (CHAR_T *attrName)
#else
static void         InsertAttrName (attrName)
CHAR_T         *attrName;

#endif
{
  Element newAttribute, elAttributes, nameAttribute, textLeaf;
  
  /* insert the attribute name as a child of the current element */
  if (CurrentElement != NULL)
    {
      newAttribute = TtaNewElement (ParsedDocument, AttributeElementType);
      if (newAttribute != NULL)
        {
	  /* it's the first attribute that we'll add */
          if (CurrentAttribute == NULL)
            {
              elAttributes = TtaNewElement (ParsedDocument, 
					    AttributesElementType);
              if (elAttributes != NULL)
                {
                  TtaInsertFirstChild (&elAttributes, CurrentElement, 
				       ParsedDocument);
                  TtaInsertFirstChild (&newAttribute, elAttributes, 
				       ParsedDocument);
                }
	    }
          else
	    /* a previous attribute already existed */
            TtaInsertSibling (newAttribute, CurrentAttribute, 
			      FALSE,ParsedDocument);

          CurrentAttribute = newAttribute;
          nameAttribute = TtaNewElement (ParsedDocument, AttrNameElementType);
          if (nameAttribute != NULL)
            {
              TtaInsertFirstChild (&nameAttribute, CurrentAttribute, 
				   ParsedDocument);
              textLeaf = TtaNewElement (ParsedDocument, TextElementType);
              if (textLeaf != NULL)
                {
                  TtaInsertFirstChild (&textLeaf, nameAttribute, 
				       ParsedDocument);
                  TtaSetTextContent (textLeaf, attrName, 
				     CurrentLanguage, ParsedDocument);
                }
	    }
	}
    }
}


/*----------------------------------------------------------------------
   InsertAttrValue
   An attribute value has been read from the HTML file.
   Put that value in the current Thot attribute.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         InsertAttrValue (CHAR_T *attrValue)
#else
static void         InsertAttrValue (attrValue)
CHAR_T     *attrValue;

#endif
{
   Element nameAttribute, valueAttribute, textLeaf;

   if (CurrentAttribute != NULL)
     {
       nameAttribute = TtaGetFirstChild (CurrentAttribute);
       valueAttribute = TtaNewElement (ParsedDocument, AttrValueElementType);
       if (valueAttribute != NULL)
         {
           TtaInsertSibling (valueAttribute, nameAttribute, FALSE, 
			     ParsedDocument);
           textLeaf = TtaNewElement (ParsedDocument, TextElementType);
           if (textLeaf != NULL)
	     {
               TtaInsertFirstChild (&textLeaf, valueAttribute,
				    ParsedDocument);
	       TtaSetTextContent (textLeaf, attrValue, CurrentLanguage, 
				  ParsedDocument);
	     }
         }
     }
}


/***************************************************
 **    EXPAT handlers
 **************************************************/
void start_hndl(void *data, const char *el, const char **attr_list)
{
  XML_Parser parser = (XML_Parser) data;

  int nb_attrs;
  CHAR_T *buffer;

  /* add the element name */
  InsertElementName ((CHAR_T *) el);

  /* process the attributes */
  nb_attrs = XML_GetSpecifiedAttributeCount (parser);
  while (*attr_list != NULL)
    {
      /* creation of the attribut into the Thot tree */
      buffer = TtaGetMemory ((strlen (*attr_list)) + 1);
      strcpy (buffer, *attr_list);
      printf (" name : %s ", buffer);
      InsertAttrName (buffer);
      TtaFreeMemory (buffer);

      /* Filling of the attribute value */
      attr_list++;
      if (*attr_list != NULL)
        {
          buffer = TtaGetMemory ((strlen (*attr_list)) + 1);
          strcpy (buffer, *attr_list);
          printf (" value=%s \n", buffer);
          InsertAttrValue (buffer);
          TtaFreeMemory (buffer);
        }
      attr_list++;
    }
}  /* End of start_hndl */

void end_hndl(void *data, const char *el)
{
  if (CurrentElement != NULL)
    CloseElement (CurrentElement);
}  /* End of end_hndl */


Document thotlib_init (char *tmpfile)
{
  Element  el;
  Element  oldel;
  ElementType   elType;
  
  ParsedDocument = TtaNewDocument (TEXT("XmlStr"), tmpfile);
  CurrentLanguage = TtaGetDefaultLanguage ();
#if 0
  elType.ElTypeNum = XmlStr_EL_Component;
  el = TtaNewTree (ParsedDocument, elType, NULL);
#endif

  /* shortcut variables intialization */
  XmlSSchema = TtaGetDocumentSSchema (ParsedDocument);
  ContentsElementType.ElSSchema = XmlSSchema;
  ComponentElementType.ElSSchema = XmlSSchema;
  PcDataElementType.ElSSchema = XmlSSchema;
  TextElementType.ElSSchema = XmlSSchema;
  TagElementType.ElSSchema = XmlSSchema;
  AttributeElementType.ElSSchema = XmlSSchema;
  AttributesElementType.ElSSchema = XmlSSchema;
  AttrNameElementType.ElSSchema = XmlSSchema;
  AttrValueElementType.ElSSchema = XmlSSchema;
  GiAttributeType.AttrSSchema = XmlSSchema;
  
  ContentsElementType.ElTypeNum = XmlStr_EL_Contents;
  ComponentElementType.ElTypeNum = XmlStr_EL_Component;
  PcDataElementType.ElTypeNum = XmlStr_EL_Pc_Data;
  TextElementType.ElTypeNum = XmlStr_EL_TEXT_UNIT;
  TagElementType.ElTypeNum = XmlStr_EL_Tag;
  AttributeElementType.ElTypeNum = XmlStr_EL_Attribute;
  AttributesElementType.ElTypeNum = XmlStr_EL_Attributes;
  AttrNameElementType.ElTypeNum = XmlStr_EL_Attribute_Name;
  AttrValueElementType.ElTypeNum = XmlStr_EL_Attribute_Value;
  GiAttributeType.AttrTypeNum = XmlStr_ATTR_Gi;

  RootElement = TtaGetMainRoot (ParsedDocument);
  TtaSetDisplayMode (ParsedDocument, NoComputedDisplay);

#if 0
  /* delete all element except the root element */
  el = TtaGetFirstChild (RootElement);
  while (el != NULL)
    {
      oldel = el;
      TtaNextSibling (&el);
      TtaDeleteTree (oldel, ParsedDocument);
    }
#endif
  /* disable auto save */
  TtaSetDocumentBackUpInterval (ParsedDocument, 0); 

  return (ParsedDocument);
}

void xmlparser_init (XML_Parser p)
{
  /* expat handlers initialization */
  XML_UseParserAsHandlerArg(p);
  XML_ExternalEntityParserCreate (p, "test", NULL);
  XML_SetElementHandler(p, start_hndl, end_hndl);
}

void ParseFile (XML_Parser p, char *filename)
{
  for (;;) {

    char *buff;
    int len;

    buff = XML_GetBuffer(p, CHUNK_SIZE);
    if (! buff) {
      fprintf(stderr, "Ran out of memory for parse buffer\n");
      exit(-1);
    }

    len = read (0, buff, CHUNK_SIZE);
    if (len < 0) {
      perror("namespace - read error");
      exit(-1);
    }
    
    if (! XML_ParseBuffer (p, len, len == 0)) {
      fprintf(stderr, "Parse error at line %d:\n%s\n",
              XML_GetCurrentLineNumber(p),
              XML_ErrorString(XML_GetErrorCode(p)));
      exit(-1);
    }

    if (len == 0)
      break;
  }
}

void RDFParseFile (char *filename)
{
  Document doc;

  XML_Parser p = XML_ParserCreate(NULL);
  if (!p) {
    fprintf(stderr, "Couldn't allocate memory for parser\n");
    return;
  }

  /* initializate the parsing environment */
  doc = thotlib_init ("rdfparse.tmp");
  xmlparser_init (p);

  /* read the annot rdf into memory as an XML tree */
  ParseFile (p, filename);

  /* (this is just to get an idea of what we parsed) */
  TtaExportDocument (doc, "/tmp/parsedrdf.xml", TEXT("XmlStrT"));

  /* extract what we need from the tree (date, author, ...) */
}









