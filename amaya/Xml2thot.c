/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2001
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Xml2thot.c
 *
 * Initializes and launches Expat parser and processes all the events it sent
 * Builds the Thot abstract tree corresponding to the XML document.
 *
 * Authors: L. Carcone
 *          V. Quint 
 */

#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"
#include "parser.h"
#include "zlib.h"
#include "MathML.h"
#include "fetchHTMLname.h"
#ifdef XML_GENERIC
#include "tree.h"
#endif /* XML_GENERIC */

#include "HTMLactions_f.h"
#include "HTMLedit_f.h"
#include "HTMLimage_f.h"
#include "HTMLtable_f.h"
#include "HTMLimage_f.h"
#include "css_f.h"
#include "UIcss_f.h"
#include "fetchHTMLname_f.h"
#include "fetchXMLname_f.h"
#include "html2thot_f.h"
#include "Xml2thot_f.h"
#include "init_f.h"
#include "styleparser_f.h"
#include "XHTMLbuilder_f.h"
#include "MathMLbuilder_f.h"
#include "GraphMLbuilder_f.h"
#ifdef XML_GENERIC
#include "Xmlbuilder_f.h"
#endif /* XML_GENERIC */
#include "XLinkbuilder_f.h"

#include "xmlparse.h"
#define NS_SEP '|'
/* maximum length of a Thot structure schema name */
#define MAX_SS_NAME_LENGTH 32

/* elements that cannot contain text as immediate children.
   When some text is present in the HTML file it must be surrounded
   by a Thot Paragraph (or Pseudo_paragraph) element */
static int          NoTextChild[] =
{
   HTML_EL_HTML, HTML_EL_HEAD, HTML_EL_BODY,
   HTML_EL_Definition_List, HTML_EL_Block_Quote, HTML_EL_Directory,
   HTML_EL_Form, HTML_EL_Menu, HTML_EL_FIELDSET,
   HTML_EL_Numbered_List, HTML_EL_Option_Menu,
   HTML_EL_Unnumbered_List, HTML_EL_Definition, HTML_EL_List_Item,
   HTML_EL_MAP, HTML_EL_Applet,
   HTML_EL_Object, HTML_EL_IFRAME, HTML_EL_NOFRAMES,
   HTML_EL_Division, HTML_EL_Center, HTML_EL_NOSCRIPT,
   HTML_EL_Data_cell, HTML_EL_Heading_cell,
   0};


/* ---------------------- static variables ---------------------- */

/* Expat parser identifier */
static XML_Parser  Parser = NULL;

/* global data used by the HTML parser */
static ParserData  XMLcontext = {0, UTF_8, 0, NULL, 0, FALSE, FALSE, FALSE, FALSE, FALSE};

/* a parser context. It describes the specific actions to be executed
when parsing an XML document fragment according to a given DTD */
typedef struct _XMLparserContext *PtrParserCtxt;
typedef struct _XMLparserContext
  {
    char	  *UriName;		/* URI of namespaces for that DTD */
    PtrParserCtxt  NextParserCtxt;	/* next parser context */
    char	  *SSchemaName;		/* name of Thot structure schema */
    SSchema	   XMLSSchema;		/* the Thot structure schema */
    int            XMLtype;             /* indentifier used by fetchname */
    Proc	   MapAttribute;	/* returns the Thot attribute corresp.
					   to an XML attribute name */
    Proc	   MapAttributeValue;	/* returns the Thot value corresp. to
					   the name of an XML attribute value */    
    Proc	   EntityCreated;	/* action to be called when an entity
					   has been parsed  */
    Proc	   CheckContext;        /* action to be called to verify if an
					   element is allowed in the current
					   structural context */
    Proc	   CheckInsert;         /* action to be called to insert an
					   element in the abstract tree */
    Proc	   ElementComplete;	/* action to be called when an element
					   has been generated completely */
    Proc	   AttributeComplete;	/* action to be called when an
					   attribute has been generated */
    Proc	   GetDTDName;		/* returns the name of the DTD to be
					   used for parsing the contents of an
					   element that uses a different DTD */
    ThotBool       DefaultLineBreak;    /* default treatment for white-space */
    ThotBool       DefaultLeadingSpace;   
    ThotBool       DefaultTrailingSpace;  
    ThotBool       DefaultContiguousSpace;

                                        /* preserve treatment for white-space */
    ThotBool       PreserveLineBreak;    
    ThotBool       PreserveLeadingSpace;   
    ThotBool       PreserveTrailingSpace;  
    ThotBool       PreserveContiguousSpace;
  }
XMLparserContext;

/* information about XML languages */
/* All parser contexts describing known XML DTDs constitute a chain */
/* first context in the chain*/
static PtrParserCtxt	firstParserCtxt = NULL;
/* current context */
static PtrParserCtxt	currentParserCtxt = NULL;
/* Generic XML context */
static PtrParserCtxt	XmlGenericParserCtxt = NULL;

#define MAX_URI_NAME_LENGTH  60
#define XHTML_URI            "http://www.w3.org/1999/xhtml"
#define MathML_URI           "http://www.w3.org/1998/Math/MathML"
#define GraphML_URI          "http://www.w3.org/2000/svg"
#define XLink_URI            "http://www.w3.org/1999/xlink"
#define NAMESPACE_URI        "http://www.w3.org/XML/1998/namespace"


/* Parser Stack */
	                /* maximum stack height */
#define MAX_STACK_HEIGHT   100
                        /* XML element name */
static char         *nameElementStack[MAX_STACK_HEIGHT]; 
                        /* element in the Thot abstract tree */
static Element       elementStack[MAX_STACK_HEIGHT];
                        /* element language */
static Language	     languageStack[MAX_STACK_HEIGHT];
                        /* is space preserved for that element */
static char	     spacePreservedStack[MAX_STACK_HEIGHT];
                        /* context of the element */
static PtrParserCtxt parserCtxtStack[MAX_STACK_HEIGHT];
                        /* first free element on the stack */
static int           stackLevel = 0;

                        /*  */
static gzFile        stream = 0;
                        /* path or URL of the document */
static char         *docURL = NULL;

/* information about the Thot document under construction */
                        /* Document structure schema */
static SSchema       DocumentSSchema = NULL;
                        /* root element of the document */
static Element       RootElement;
                        /* name of the root element */
static char          XMLRootName[100];
                        /* root element is closed */
static ThotBool	     XMLrootClosed = FALSE;
                        /* the last start tag encountered is invalid */
static ThotBool      UnknownTag = FALSE;
                        /* last attribute created */
static Attribute     currentAttribute = NULL;
                        /* last created attribute is "unknown_attr"*/
static ThotBool      UnknownAttr;
                        /* element with which the last */
                        /* attribute has been associated */
static Element       lastAttrElement = NULL;
                        /* entry in the AttributeMappingTable */
		        /* of the attribute being created */
static AttributeMapping* lastMappedAttr = NULL;

static ThotBool	    ParsingSubTree = FALSE;
static ThotBool	    ImmediatelyAfterTag = FALSE;
static ThotBool	    HTMLStyleAttribute = FALSE;
static ThotBool	    XMLSpaceAttribute = FALSE;
static ThotBool     ParsingCDATA = FALSE;
static ThotBool     IgnoreCommentAndPi = FALSE;;
static char	    currentElementContent = ' ';
static char	    currentElementName[40];

/* Global variable to handle white-space in XML documents */
static ThotBool     RemoveLineBreak = FALSE;
static ThotBool     RemoveLeadingSpace = FALSE;   
static ThotBool     RemoveTrailingSpace = FALSE;
static ThotBool     RemoveContiguousSpace = FALSE;

/* "Extra" counters for the characters and the lines read */
static int          extraLineRead = 0;
static int          extraOffset = 0;
static int          htmlLineRead = 0;
static int          htmlCharRead = 0;

/* Virtual DOCTYPE Declaration */
#define DECL_DOCTYPE "<!DOCTYPE html PUBLIC \"\" \"\">\n"
#define DECL_DOCTYPE_LEN 29
#define DECL_XML "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n"
#define DECL_XML_LEN 44

/* Virtual root for XML sub-tree */
#define SUBTREE_ROOT "amaya-root-subtree"
static  int         SUBTREE_ROOT_LEN = 20;

/* maximum size of error messages */
#define MaxMsgLength 200

static void   StartOfXmlStartElement (char *uriName, char *elementName);
static void   DisableExpatParser ();
static void   XhtmlCheckInsert (Element *el, Element parent, Document doc, ThotBool *inserted);
static void   XmlCheckInsert (Element *el, Element parent, Document doc, ThotBool *inserted);
static void   XhtmlCheckContext (char *elName, ElementType elType, ThotBool *isAllowed);
static void   XmlCheckContext (char *elName, ElementType elType, ThotBool *isAllowed);

/*----------------------------------------------------------------------
   ChangeXmlParserContextDTD
   Get the parser context correponding to a given DTD
  ----------------------------------------------------------------------*/
static void     ChangeXmlParserContextDTD (char *DTDname)

{
  currentParserCtxt = firstParserCtxt;
  while (currentParserCtxt != NULL &&
	 strcmp (DTDname, currentParserCtxt->SSchemaName))
    currentParserCtxt = currentParserCtxt->NextParserCtxt;

  /* initialize the corresponding entry */
  if (currentParserCtxt != NULL &&
      currentParserCtxt != XmlGenericParserCtxt &&
      currentParserCtxt->XMLSSchema == NULL) 
    currentParserCtxt->XMLSSchema = 
      GetXMLSSchema (currentParserCtxt->XMLtype, XMLcontext.doc);
}

/*----------------------------------------------------------------------
   ChangeXmlParserContextUri
   Get the parser context correponding to a given uri
  ----------------------------------------------------------------------*/
static void     ChangeXmlParserContextUri (char *uriName)

{
  currentParserCtxt = firstParserCtxt;
  while (currentParserCtxt != NULL &&
	 strcmp (uriName, currentParserCtxt->UriName))
    currentParserCtxt = currentParserCtxt->NextParserCtxt;

  /* Return the corresponding Thot schema */
  if (currentParserCtxt != NULL &&
      currentParserCtxt != XmlGenericParserCtxt &&
      currentParserCtxt->XMLSSchema == NULL) 
    currentParserCtxt->XMLSSchema = 
      GetXMLSSchema (currentParserCtxt->XMLtype, XMLcontext.doc);
}

#ifdef LC
/*----------------------------------------------------------------------
   ChangeXmlParserContextRootName
   Get the parser context correponding to a given tag
  ----------------------------------------------------------------------*/
static void     ChangeXmlParserContextTagName (char *tagName)

{
  if (!strcmp (tagName, "math"))
    ChangeXmlParserContextDTD ("MathML");
  else
    if (!strcmp (tagName, "svg") ||
	!strcmp (tagName, "xmlgraphics"))
      ChangeXmlParserContextDTD ("GraphML");
    else
      if (!strcmp (tagName, "html"))
	ChangeXmlParserContextDTD ("HTML");
      else
	currentParserCtxt = NULL;
}
#endif /* LC */

/*----------------------------------------------------------------------
   InitXmlParserContexts
   Create the chain of parser contexts decribing all recognized XML DTDs
  ----------------------------------------------------------------------*/
static void    InitXmlParserContexts (void)

{
   PtrParserCtxt   ctxt, prevCtxt;

   firstParserCtxt = NULL;
   prevCtxt = NULL;
   ctxt = NULL;

   /* create and initialize a context for XHTML */
   ctxt = TtaGetMemory (sizeof (XMLparserContext));
   if (prevCtxt == NULL)
      firstParserCtxt = ctxt;
   else
      prevCtxt->NextParserCtxt = ctxt;
   ctxt->NextParserCtxt = NULL;
   ctxt->SSchemaName = TtaGetMemory (MAX_SS_NAME_LENGTH);
   strcpy (ctxt->SSchemaName, "HTML");
   ctxt->UriName = TtaGetMemory (MAX_URI_NAME_LENGTH);
   strcpy (ctxt->UriName, XHTML_URI);
   ctxt->XMLSSchema = NULL;
   ctxt->XMLtype = XHTML_TYPE;
   ctxt->MapAttribute = (Proc) MapHTMLAttribute;
   ctxt->MapAttributeValue = (Proc) MapHTMLAttributeValue;
   ctxt->EntityCreated = (Proc) XhtmlEntityCreated;
   ctxt->CheckContext = (Proc) XhtmlCheckContext;
   ctxt->CheckInsert = (Proc) XhtmlCheckInsert;
   ctxt->ElementComplete = (Proc) XhtmlElementComplete;
   ctxt->AttributeComplete = NULL;
   ctxt->GetDTDName = NULL;
   ctxt->DefaultLineBreak = TRUE;
   ctxt->DefaultLeadingSpace = TRUE;   
   ctxt->DefaultTrailingSpace = TRUE;  
   ctxt->DefaultContiguousSpace = TRUE;
   ctxt->PreserveLineBreak = FALSE;    
   ctxt->PreserveLeadingSpace = FALSE;   
   ctxt->PreserveTrailingSpace = FALSE;  
   ctxt->PreserveContiguousSpace = FALSE;
   prevCtxt = ctxt;

   /* create and initialize a context for MathML */
   ctxt = TtaGetMemory (sizeof (XMLparserContext));
   if (prevCtxt == NULL)
      firstParserCtxt = ctxt;
   else
      prevCtxt->NextParserCtxt = ctxt;
   ctxt->NextParserCtxt = NULL;
   ctxt->SSchemaName = TtaGetMemory (MAX_SS_NAME_LENGTH);
   strcpy (ctxt->SSchemaName, "MathML");
   ctxt->UriName = TtaGetMemory (MAX_URI_NAME_LENGTH);
   strcpy (ctxt->UriName, MathML_URI);
   ctxt->XMLSSchema = NULL;
   ctxt->XMLtype = MATH_TYPE;
   ctxt->MapAttribute = (Proc) MapMathMLAttribute;
   ctxt->MapAttributeValue = (Proc) MapMathMLAttributeValue;
   ctxt->EntityCreated = (Proc) MathMLEntityCreatedWithExpat;
   ctxt->CheckContext = (Proc) XmlCheckContext;
   ctxt->CheckInsert = (Proc) XmlCheckInsert;
   ctxt->ElementComplete = (Proc) MathMLElementComplete;
   ctxt->AttributeComplete = (Proc) MathMLAttributeComplete;
   ctxt->GetDTDName = (Proc) MathMLGetDTDName;
   ctxt->DefaultLineBreak = TRUE;
   ctxt->DefaultLeadingSpace = TRUE;   
   ctxt->DefaultTrailingSpace = TRUE;  
   ctxt->DefaultContiguousSpace = TRUE;
   ctxt->PreserveLineBreak = FALSE;    
   ctxt->PreserveLeadingSpace = FALSE;   
   ctxt->PreserveTrailingSpace = FALSE;  
   ctxt->PreserveContiguousSpace = FALSE;
   prevCtxt = ctxt;

   /* create and initialize a context for SVG */
   ctxt = TtaGetMemory (sizeof (XMLparserContext));
   if (prevCtxt == NULL)
      firstParserCtxt = ctxt;
   else
      prevCtxt->NextParserCtxt = ctxt;
   ctxt->NextParserCtxt = NULL;
   ctxt->SSchemaName = TtaGetMemory (MAX_SS_NAME_LENGTH);
   strcpy (ctxt->SSchemaName, "GraphML");
   ctxt->UriName = TtaGetMemory (MAX_URI_NAME_LENGTH);
   strcpy (ctxt->UriName, GraphML_URI);
   ctxt->XMLSSchema = NULL;
   ctxt->XMLtype = GRAPH_TYPE;
   ctxt->MapAttribute = (Proc) MapGraphMLAttribute;
   ctxt->MapAttributeValue = (Proc) MapGraphMLAttributeValue;
   ctxt->EntityCreated = (Proc) GraphMLEntityCreatedWithExpat;
   ctxt->CheckContext = (Proc) XmlCheckContext;
   ctxt->CheckInsert = (Proc) XmlCheckInsert;
   ctxt->ElementComplete = (Proc) GraphMLElementComplete;
   ctxt->AttributeComplete = (Proc) GraphMLAttributeComplete;
   ctxt->GetDTDName = (Proc) GraphMLGetDTDName;
   ctxt->DefaultLineBreak = TRUE;
   ctxt->DefaultLeadingSpace = TRUE;   
   ctxt->DefaultTrailingSpace = TRUE;  
   ctxt->DefaultContiguousSpace = TRUE;
   ctxt->PreserveLineBreak = TRUE;    
   ctxt->PreserveLeadingSpace = FALSE;   
   ctxt->PreserveTrailingSpace = FALSE;  
   ctxt->PreserveContiguousSpace = FALSE;
   prevCtxt = ctxt;

   /* create and initialize a context for XLink */
   ctxt = TtaGetMemory (sizeof (XMLparserContext));
   if (prevCtxt == NULL)
      firstParserCtxt = ctxt;
   else
      prevCtxt->NextParserCtxt = ctxt;
   ctxt->NextParserCtxt = NULL;	/* last context */
   ctxt->SSchemaName = TtaGetMemory (MAX_SS_NAME_LENGTH);
   strcpy (ctxt->SSchemaName, "XLink");
   ctxt->UriName = TtaGetMemory (MAX_URI_NAME_LENGTH);
   strcpy (ctxt->UriName, XLink_URI);
   ctxt->XMLSSchema = NULL;
   ctxt->XMLtype = XLINK_TYPE;
   ctxt->MapAttribute = (Proc) MapXLinkAttribute;
   ctxt->MapAttributeValue = (Proc) MapXLinkAttributeValue;
   ctxt->EntityCreated = NULL;
   ctxt->CheckContext = (Proc) XmlCheckContext;
   ctxt->CheckInsert = (Proc) XmlCheckInsert;
   ctxt->ElementComplete = NULL;
   ctxt->AttributeComplete = (Proc) XLinkAttributeComplete;
   ctxt->GetDTDName = NULL;
   ctxt->DefaultLineBreak = TRUE;
   ctxt->DefaultLeadingSpace = TRUE;   
   ctxt->DefaultTrailingSpace = TRUE;  
   ctxt->DefaultContiguousSpace = TRUE;
   ctxt->PreserveLineBreak = FALSE;    
   ctxt->PreserveLeadingSpace = FALSE;   
   ctxt->PreserveTrailingSpace = FALSE;  
   ctxt->PreserveContiguousSpace = FALSE;
   prevCtxt = ctxt;

#ifdef XML_GENERIC
   /* create and initialize a context for generic XML */
   ctxt = TtaGetMemory (sizeof (XMLparserContext));
   if (prevCtxt == NULL)
      firstParserCtxt = ctxt;
   else
      prevCtxt->NextParserCtxt = ctxt;
   ctxt->NextParserCtxt = NULL;	/* last context */
   ctxt->SSchemaName = TtaGetMemory (MAX_SS_NAME_LENGTH);
   strcpy (ctxt->SSchemaName, "XML");
   ctxt->UriName = TtaGetMemory (MAX_URI_NAME_LENGTH);
   ctxt->UriName[0] = EOS;
   ctxt->XMLSSchema = NULL;
   ctxt->XMLtype = XML_TYPE;
   ctxt->MapAttribute = NULL;
   ctxt->MapAttributeValue = (Proc) MapXmlAttributeValue;
   ctxt->EntityCreated = NULL;
   ctxt->CheckContext =  (Proc) XmlCheckContext;
   ctxt->CheckInsert = (Proc) XmlCheckInsert;
   ctxt->ElementComplete = (Proc) XmlElementComplete;
   ctxt->AttributeComplete = (Proc) XmlAttributeComplete;
   ctxt->GetDTDName = NULL;
   ctxt->DefaultLineBreak = TRUE;
   ctxt->DefaultLeadingSpace = TRUE;   
   ctxt->DefaultTrailingSpace = TRUE;  
   ctxt->DefaultContiguousSpace = TRUE;
   ctxt->PreserveLineBreak = FALSE;    
   ctxt->PreserveLeadingSpace = FALSE;   
   ctxt->PreserveTrailingSpace = FALSE;  
   ctxt->PreserveContiguousSpace = FALSE;
   prevCtxt = ctxt;
   XmlGenericParserCtxt = ctxt;
#endif /* XML_GENERIC */

   currentParserCtxt = NULL;

}

/*----------------------------------------------------------------------
   XmlSetElemLineNumber
   Assigns the current line number (number given by EXPAT parser.
  ----------------------------------------------------------------------*/
void XmlSetElemLineNumber (Element el)

{
  int     lineNumber;

  if (ParsingSubTree)
    lineNumber = 0;
  else
    lineNumber = XML_GetCurrentLineNumber (Parser) + htmlLineRead - extraLineRead;
  TtaSetElementLineNumber (el, lineNumber);
}

/*----------------------------------------------------------------------
   XmlParseError
   print the error message msg on stderr.
   When the line is 0 ask to expat the current line number
  ----------------------------------------------------------------------*/
void  XmlParseError (ErrorType type, unsigned char *msg, int line)
{
  if (!ErrFile)
    if (OpenParsingErrors (XMLcontext.doc) == FALSE)
      return;
  
  if (docURL != NULL)
    {
      fprintf (ErrFile, "*** Errors/warnings in %s\n", docURL);
      TtaFreeMemory (docURL);
      docURL = NULL;
    }
  
  switch (type)
    {
    case errorEncoding: 
      fprintf (ErrFile, "  %s\n", msg);
      XMLNotWellFormed = TRUE;
      break;
    case errorNotWellFormed:
      if (line == 0)
	{
	  fprintf (ErrFile, "  line %d, char %d: %s\n",
		   XML_GetCurrentLineNumber (Parser) + htmlLineRead -  extraLineRead,
		   XML_GetCurrentColumnNumber (Parser),
		   msg);
	}
      else
	fprintf (ErrFile, "  line %d: %s\n", line, msg); 
      XMLNotWellFormed = TRUE;
      break;
    case errorCharacterNotSupported:
      if (line == 0)
	{
	  fprintf (ErrFile, "  line %d, char %d: %s\n",
		   XML_GetCurrentLineNumber (Parser) + htmlLineRead -  extraLineRead,
		   XML_GetCurrentColumnNumber (Parser),
		   msg);
	}
      else
	fprintf (ErrFile, "  line %d: %s\n", line, msg); 
      XMLCharacterNotSupported = TRUE;
      break;
    case errorParsing:
      if (line == 0)
	{
	  fprintf (ErrFile, "  line %d, char %d: %s\n",
		   XML_GetCurrentLineNumber (Parser) + htmlLineRead -  extraLineRead,
		   XML_GetCurrentColumnNumber (Parser),
		   msg);
	}
      else
	fprintf (ErrFile, "  line %d: %s\n", line, msg); 
      XMLErrorsFound = TRUE;
      break;
    case errorParsingProfile:
      if (line == 0)
	{
	  fprintf (ErrFile, "  line %d, char %d: %s\n",
		   XML_GetCurrentLineNumber (Parser) + htmlLineRead -  extraLineRead,
		   XML_GetCurrentColumnNumber (Parser),
		   msg);
	}
      else
	fprintf (ErrFile, "  line %d: %s\n", line, msg); 
      XMLErrorsFoundInProfile = TRUE;
      break;
    case undefinedEncoding:
      fprintf (ErrFile, "  %s\n", msg);
      break;
    }
}

/*----------------------------------------------------------------------
  IsParsingCSS 
  Returns the value of ParsingCSS boolean.
  ----------------------------------------------------------------------*/
ThotBool  IsParsingCSS ()

{
   return XMLcontext.parsingCSS;
}

/*----------------------------------------------------------------------
  SetParsingCSS 
  Sets the value of ParsingCSS boolean.
  ----------------------------------------------------------------------*/
void  SetParsingCSS (ThotBool value)

{
   XMLcontext.parsingCSS = value;
}

/*----------------------------------------------------------------------
  SetParsingTextArea
  Sets the value of ParsingTextArea boolean.
  ----------------------------------------------------------------------*/
void  SetParsingTextArea (ThotBool value)

{
   XMLcontext.parsingTextArea = value;
}

/*----------------------------------------------------------------------
  SetLanguagInXmlStack
  Sets the value of the language.
  ----------------------------------------------------------------------*/
void  SetLanguagInXmlStack (Language lang)

{
  languageStack[stackLevel - 1] = lang;
}

/*----------------------------------------------------------------------
  IsWithinTable 
  Returns the value of WithinTable integer.
  ----------------------------------------------------------------------*/
int  IsWithinTable ()

{
   return XMLcontext.withinTable;
}

/*----------------------------------------------------------------------
  SubWithinTable
  ----------------------------------------------------------------------*/
void  SubWithinTable ()

{
   XMLcontext.withinTable--;
}

/*----------------------------------------------------------------------
   XmlWhiteSpaceHandling
   Is there an openend element with a xml:space attribute ?
  ----------------------------------------------------------------------*/
static void  XmlWhiteSpaceHandling ()

{
  int        i;
  ThotBool   found;

  if (currentParserCtxt == NULL)
    return; 

  found = FALSE;
  i = stackLevel - 1;
  while (i > 0 && !found)
    {
      if (spacePreservedStack[i] == ' ')
	i--;
      else
	{
	  found = TRUE;
	  if (spacePreservedStack[i] == 'D')
	    {
	      RemoveLineBreak = currentParserCtxt->DefaultLineBreak;
	      RemoveLeadingSpace = currentParserCtxt->DefaultLeadingSpace;   
	      RemoveTrailingSpace = currentParserCtxt->DefaultTrailingSpace;  
	      RemoveContiguousSpace = currentParserCtxt->DefaultContiguousSpace;
	    }
	  else
	    {
	      RemoveLineBreak = currentParserCtxt->PreserveLineBreak;
	      RemoveLeadingSpace = currentParserCtxt->PreserveLeadingSpace;   
	      RemoveTrailingSpace = currentParserCtxt->PreserveTrailingSpace;  
	      RemoveContiguousSpace = currentParserCtxt->PreserveContiguousSpace;
	    }
	}
    }
  
  if (!found)
    {
      RemoveLineBreak = currentParserCtxt->DefaultLineBreak;
      RemoveLeadingSpace = currentParserCtxt->DefaultLeadingSpace;   
      RemoveTrailingSpace = currentParserCtxt->DefaultTrailingSpace;  
      RemoveContiguousSpace = currentParserCtxt->DefaultContiguousSpace;
    }
}

/*----------------------------------------------------------------------
   XmlWhiteInStack
   The last element in stack has a xml:space attribute
   (or it is a PRE, STYLE or SCRIPT element in XHTML)
  ----------------------------------------------------------------------*/
static void    XmlWhiteSpaceInStack (char  *attrValue)

{
  if (attrValue == NULL)
      spacePreservedStack[stackLevel-1] = 'P';
  else
    {
      if ((strcmp (attrValue, "default") == 0))
	spacePreservedStack[stackLevel-1] = 'D';
      else
	spacePreservedStack[stackLevel-1] = 'P';
    }
}

/*----------------------------------------------------------------------
  XmlWithinStack  
  Checks if an element of type ThotType is in the stack.
  ----------------------------------------------------------------------*/
static ThotBool  XmlWithinStack (int ThotType, SSchema ThotSSchema)

{
   ThotBool       ret;
   int            i;
   ElementType    elType;

   ret = FALSE;
   i = stackLevel - 1;

   while (i >= 0 && !ret)
     {
       if (elementStack[i] != NULL)
	 {
	   elType = TtaGetElementType (elementStack[i]);
	   if (elType.ElTypeNum == ThotType &&
	       elType.ElSSchema == ThotSSchema)
	       ret = TRUE;
	 }
       i--;
     }
   return ret;
}

/*----------------------------------------------------------------------
   InsertSibling
   Return TRUE if the new element must be inserted in the Thot document
   as a sibling of lastElement;
   Return FALSE if it must be inserted as a child.
  ----------------------------------------------------------------------*/
static ThotBool     InsertSibling ()
{
   if (stackLevel == 0)
       return FALSE;
   else
     if (XMLcontext.lastElementClosed ||
	 TtaIsLeaf (TtaGetElementType (XMLcontext.lastElement)))
       return TRUE;
     else
       return FALSE;

}

/*----------------------------------------------------------------------
  XmlGetFallbackCharacter
  Try to find a fallback character
  ----------------------------------------------------------------------*/
static void  XmlGetFallbackCharacter (wchar_t wcharRead)

{
   Language       lang;
   char           fallback[5];
   char           bufName[10];
   char           buffer[10];
   ElementType    elType;
   Element        elLeaf;
   AttributeType  attrType;
   Attribute	  attr;
   int            len, i, j;

   GetFallbackCharacter ((int) wcharRead, fallback, &lang);
   if (fallback[0] == '?')
     {
       /* Character not found in the fallback table */
       /* Create a symbol leaf */
       elType = TtaGetElementType (XMLcontext.lastElement);
       elType.ElTypeNum = 3;
       elLeaf = TtaNewElement (XMLcontext.doc, elType);
       XmlSetElemLineNumber (elLeaf);
       InsertXmlElement (&elLeaf);
       XMLcontext.lastElementClosed = TRUE;
       /* Put the symbol '?' into the new symbol leaf */
       TtaSetGraphicsShape (elLeaf, fallback[0], XMLcontext.doc);
       /* Changes the wide char code associated with that symbol */
       TtaSetSymbolCode (elLeaf, wcharRead, XMLcontext.doc);
       /* Make that leaf read-only */
       TtaSetAccessRight (elLeaf, ReadOnly, XMLcontext.doc);
       XMLcontext.mergeText = FALSE;
     }
   else
     {
       /* Character found in the fallback table */
       /* Create a new text leaf */
       elType = TtaGetElementType (XMLcontext.lastElement);
       elType.ElTypeNum = 1;
       elLeaf = TtaNewElement (XMLcontext.doc, elType);
       XmlSetElemLineNumber (elLeaf);
       InsertXmlElement (&elLeaf);
       XMLcontext.lastElementClosed = TRUE;
       /* Put the fallback character into the new text leaf */
       TtaSetTextContent (elLeaf, fallback, lang, XMLcontext.doc);
       XMLcontext.mergeText = FALSE;
       /* Associate an attribute EntityName with the new text leaf */
       attrType.AttrSSchema = elType.ElSSchema;
       if (strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML") == 0)
	 attrType.AttrTypeNum = MathML_ATTR_EntityName;
       else if (strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
	 attrType.AttrTypeNum = HTML_ATTR_EntityName;
       else
	 attrType.AttrTypeNum = HTML_ATTR_EntityName;
       attr = TtaNewAttribute (attrType);
       TtaAttachAttribute (elLeaf, attr, XMLcontext.doc);
       len = sprintf (buffer, "%d", (int) wcharRead);
       i = 0;
       bufName[i++] = (char) START_ENTITY;
       bufName[i++] = '#';
       for (j = 0; j < len; j++)
	 bufName[i++] = buffer[j];
       bufName[i++] = ';';
       bufName[i] = EOS;
       TtaSetAttributeText (attr, bufName, elLeaf, XMLcontext.doc);
     }
}

/*----------------------------------------------------------------------
   XhtmlCannotContainText 
   Return TRUE if element el is a block element.
  ----------------------------------------------------------------------*/
static ThotBool   XhtmlCannotContainText (ElementType elType)

{
   int            i;
   ThotBool       ret;

   if (strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML"))
      /* not an HTML element */
      ret = TRUE;
   else
     {
       ret = FALSE;
       i = 0;
       while (NoTextChild[i] > 0 && NoTextChild[i] != elType.ElTypeNum)
           i++;
       if (NoTextChild[i] == elType.ElTypeNum)
           ret = TRUE;
     }
   return ret;
}

/*----------------------------------------------------------------------
   XmlCheckInsert
   Inserts a Pseudo_paragraph element in the abstract tree if el 
   is a math within a XHTML element 
  ----------------------------------------------------------------------*/
static void  XmlCheckInsert (Element *el, Element parent,
			     Document doc, ThotBool *inserted)

{
   ElementType  newElType, elType, prevType;
   Element      newEl, ancestor, prev, prevprev;
   
   if (parent == NULL)
       return;

   elType = TtaGetElementType (*el);
   if (elType.ElTypeNum == MathML_EL_MathML &&
       strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML") == 0)
     {
       ancestor = parent;
       elType = TtaGetElementType (ancestor);
       if (strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML"))
	 return;

       while (ancestor != NULL &&
	      IsXMLElementInline (elType))
	 {
	   ancestor = TtaGetParent (ancestor);
	   elType = TtaGetElementType (ancestor);
	 }
       
       if (ancestor != NULL)
	 {
	   elType = TtaGetElementType (ancestor);
	   if (XhtmlCannotContainText (elType) &&
	       !XmlWithinStack (HTML_EL_Option_Menu, DocumentSSchema))
	     {
	       /* Element ancestor cannot contain math directly. Create a */
	       /* Pseudo_paragraph element as the parent of the math element */
	       newElType.ElSSchema = DocumentSSchema;
	       newElType.ElTypeNum = HTML_EL_Pseudo_paragraph;
	       newEl = TtaNewElement (doc, newElType);
	       XmlSetElemLineNumber (newEl);
	       /* insert the new Pseudo_paragraph element */
	       InsertXmlElement (&newEl); 
	       BlockInCharLevelElem (newEl);
	       if (newEl != NULL)
		 {
		   /* insert the Text element in the tree */
		   TtaInsertFirstChild (el, newEl, doc);
		   *inserted = TRUE;
		   
		   /* if previous siblings of the new Pseudo_paragraph element
		      are inline elements, move them within the new
		      Pseudo_paragraph element */
		     prev = newEl;
		     TtaPreviousSibling (&prev);
		     while (prev != NULL)
		       {
			 prevType = TtaGetElementType (prev);
			 if (IsXMLElementInline (prevType))
			   {
			     prevprev = prev;  TtaPreviousSibling (&prevprev);
			     TtaRemoveTree (prev, doc);
			     TtaInsertFirstChild (&prev, newEl, doc);
			     prev = prevprev;
			   }
			 else
			   prev = NULL;
		       }
		 }
	     }
	 }
     }
   return;
}

/*----------------------------------------------------------------------
   XhtmlCheckInsert
   Inserts an element Pseudo_paragraph in the abstract tree of
   the Thot document if el is a leaf and is not allowed to be
   a child of element parent.
   If element *el is not a character level element and parent is
   a Pseudo_paragraph, insert *el as a sibling of element parent.

   Return TRUE if element *el has been inserted in the tree.
  ----------------------------------------------------------------------*/
static void  XhtmlCheckInsert (Element *el, Element  parent,
			       Document doc, ThotBool *inserted)

{
   ElementType   parentType, newElType, elType, prevType, ancestorType;
   Element       newEl, ancestor, prev, prevprev;

   if (parent == NULL)
     return;
   
   elType = TtaGetElementType (*el);

   if (elType.ElTypeNum == HTML_EL_TEXT_UNIT || 
       elType.ElTypeNum == HTML_EL_BR ||
       elType.ElTypeNum == HTML_EL_PICTURE_UNIT ||
       elType.ElTypeNum == HTML_EL_Input ||
       elType.ElTypeNum == HTML_EL_Text_Area)
     {
       /* the element to be inserted is a character string */
       /* Search the ancestor that is not a character level element */
       ancestor = parent;
       ancestorType = TtaGetElementType (ancestor);
       while (ancestor != NULL &&
	      IsXMLElementInline (ancestorType))
	 {
	   ancestor = TtaGetParent (ancestor);
	   ancestorType = TtaGetElementType (ancestor);
	 }
       
       if (ancestor != NULL)
	 {
	   elType = TtaGetElementType (ancestor);
	   if (XhtmlCannotContainText (elType) &&
	       !XmlWithinStack (HTML_EL_Option_Menu, DocumentSSchema))
	     {
	       /* Element ancestor cannot contain text directly. Create a */
	       /* Pseudo_paragraph element as the parent of the text element */
	       newElType.ElSSchema = DocumentSSchema;
	       newElType.ElTypeNum = HTML_EL_Pseudo_paragraph;
	       newEl = TtaNewElement (doc, newElType);
	       XmlSetElemLineNumber (newEl);
	       /* insert the new Pseudo_paragraph element */
	       InsertXmlElement (&newEl);
	       if (newEl != NULL)
		 {
		   /* insert the Text element in the tree */
		   TtaInsertFirstChild (el, newEl, doc);
		   BlockInCharLevelElem (newEl);
		   *inserted = TRUE;
		   
		   /* if previous siblings of the new Pseudo_paragraph element
		      are character level elements, move them within the new
		      Pseudo_paragraph element */
		   prev = newEl;
		   TtaPreviousSibling (&prev);
		   while (prev != NULL)
		     {
		       prevType = TtaGetElementType (prev);
		       if (!IsXMLElementInline (prevType))
			 prev = NULL;
		       else
			 {
			   prevprev = prev; 
			   TtaPreviousSibling (&prevprev);
			   TtaRemoveTree (prev, doc);
			   TtaInsertFirstChild (&prev, newEl, doc);
			   prev = prevprev;
			 }
		     }
		 }
	     }
	 }
     }
   else
     if (!IsXMLElementInline (elType) &&
	 elType.ElTypeNum != HTML_EL_Comment_ &&
	 elType.ElTypeNum != HTML_EL_XMLPI)
       /* it is not a character level element nor a comment or a PI */
       /* don't insert it as a child of a Pseudo_paragraph, but as a sibling */
       {
	 parentType = TtaGetElementType (parent);
	 if (parentType.ElTypeNum == HTML_EL_Pseudo_paragraph)
	   {
	     TtaInsertSibling (*el, parent, FALSE, doc);
	      *inserted = TRUE;
	   }
       }

   if (!*inserted)
     if (elType.ElTypeNum == HTML_EL_TEXT_UNIT ||
         (elType.ElTypeNum != HTML_EL_Inserted_Text &&
	  IsXMLElementInline (TtaGetElementType (*el))))
       {
         /* it is a character level element */
	 parentType = TtaGetElementType (parent);
	 if (parentType.ElTypeNum == HTML_EL_Text_Area)
	   {
	     /* A basic element cannot be a child of a Text_Area */
	     /* create a Inserted_Text element as a child of Text_Area */
	     newElType.ElSSchema = DocumentSSchema;
	     newElType.ElTypeNum = HTML_EL_Inserted_Text;
	     newEl = TtaNewElement (doc, newElType);
	     XmlSetElemLineNumber (newEl);
	     InsertXmlElement (&newEl);
	     if (newEl != NULL)
	       {
		 TtaInsertFirstChild (el, newEl, doc);
		 *inserted = TRUE;
	       }
	   }
       }

   return;
}

/*---------------------------------------------------------------------------
   InsertXmlElement   
   Inserts an element el in the Thot abstract tree , at the current position.
  ---------------------------------------------------------------------------*/
void     InsertXmlElement (Element *el)

{
  Element   parent;
  ThotBool  inserted = FALSE;

  if (currentParserCtxt != NULL)
    {
      if (InsertSibling ())
	{
	  if (XMLcontext.lastElement == NULL)
	    parent = NULL;
	  else
	    parent = TtaGetParent (XMLcontext.lastElement);
	  (*(currentParserCtxt->CheckInsert))
	    (el, parent, XMLcontext.doc, &inserted);
	  if (!inserted)
	    {
	      if (parent != NULL)
		TtaInsertSibling (*el, XMLcontext.lastElement,
				  FALSE, XMLcontext.doc);
	      else
		{
		  TtaDeleteTree (*el, XMLcontext.doc);
		  *el = NULL;
		}
	    }
	}
      else
	{
	  (*(currentParserCtxt->CheckInsert))
	    (el, XMLcontext.lastElement, XMLcontext.doc, &inserted);
	  if (!inserted)
	    TtaInsertFirstChild (el, XMLcontext.lastElement, XMLcontext.doc);
	}

      if (*el != NULL)
	{
	  XMLcontext.lastElement = *el;
	  XMLcontext.lastElementClosed = FALSE;
	}
    }
}

/*----------------------------------------------------------------------
   XmlLastLeafInElement
   return the last leaf element in element el.
  ----------------------------------------------------------------------*/
Element      XmlLastLeafInElement (Element el)

{
   Element  child, lastLeaf;

   child = el;
   lastLeaf = NULL;
   while (child != NULL)
     {
       child = TtaGetLastChild (child);
       if (child != NULL)
	   lastLeaf = child;
     }
   return lastLeaf;
}

#ifdef LC
/*----------------------------------------------------------------------
   RemoveEndingSpaces
   If element el is a block-level element, remove all spaces contained
   at the end of that element.
   Return TRUE if spaces have been removed.
  ----------------------------------------------------------------------*/
static ThotBool     RemoveEndingSpaces (Element el)

{
   int              length, nbspaces;
   ElementType      elType;
   Element          lastLeaf;
   char             lastChar[2];
   ThotBool         endingSpacesDeleted;

   endingSpacesDeleted = FALSE;
   if (IsBlockElement (el))
      /* it's a block element. */
      {
	   /* Search the last leaf in the element's tree */
	   lastLeaf = XmlLastLeafInElement (el);
	   if (lastLeaf != NULL)
	     {
	       elType = TtaGetElementType (lastLeaf);
	       if (elType.ElTypeNum == 1)
		 /* the las leaf is a TEXT element */
		 {
		   length = TtaGetTextLength (lastLeaf);
		   if (length > 0)
		     {
		       /* count ending spaces */
		       nbspaces = 0;
		       do
			 {
			   TtaGiveSubString (lastLeaf, lastChar, length, 1);
			   if (lastChar[0] == SPACE)
			     {
			       length--;
			       nbspaces++;
			     }
			 }
		       while (lastChar[0] == SPACE && length > 0);
		       if (nbspaces > 0)
			 {
			 if (length == 0)
			   /* empty TEXT element */
			   TtaDeleteTree (lastLeaf, XMLcontext.doc);
			 else
			   /* remove the ending spaces */
			   TtaDeleteTextContent (lastLeaf, length + 1,
						 nbspaces, XMLcontext.doc);
			 }
		     }
		 }
	     }
	   endingSpacesDeleted = TRUE;
      }
   return endingSpacesDeleted;
}
#endif /* LC */

/*----------------------------------------------------------------------
   RemoveTrailingSpaces
  ----------------------------------------------------------------------*/
static void      RemoveTrailingSpaces (Element el)

{
   int           length, nbspaces;
   ElementType   elType;
   Element       lastLeaf;
   AttributeType attrType;
   Attribute     attr = NULL;
   char          lastChar[2];

   /* Search the last leaf in the element's tree */
   lastLeaf = XmlLastLeafInElement (el);
   if (lastLeaf != NULL)
     {
       elType = TtaGetElementType (lastLeaf);
       if (elType.ElTypeNum == 1)
	 /* the last leaf is a TEXT element */
	 {
	   length = TtaGetTextLength (lastLeaf);
	   if (length > 0)
	     {
	       nbspaces = 0;
	       /* Search for an Entity attribute  */
	       attrType.AttrSSchema = elType.ElSSchema;
	       if (strcmp (currentParserCtxt->SSchemaName, "HTML") == 0)
		 attrType.AttrTypeNum = HTML_ATTR_EntityName;
	       else if (strcmp (currentParserCtxt->SSchemaName, "MathML") == 0)
		 attrType.AttrTypeNum = MathML_ATTR_EntityName;
	       else
		 {
		   attrType.AttrTypeNum = 0;
		   attr = NULL;
		 }
	       if (attrType.AttrTypeNum != 0)
		 attr = TtaGetAttribute (lastLeaf, attrType);

	       /* Don't suppress trailing spaces for an entity element */
	       if (attr == NULL)
		 {
		   if (RemoveTrailingSpace)
		     {
		       do
			 {
			   TtaGiveSubString (lastLeaf, lastChar, length, 1);
			   if (lastChar[0] == SPACE)
			     {
			       length--;
			       nbspaces++;
			     }
			 }
		       while (lastChar[0] == SPACE && length > 0);
		     }
		   else
		     {
		       TtaGiveSubString (lastLeaf, lastChar, length, 1);
		       if (lastChar[0] == CR || lastChar[0] == EOL)
			 {
			   length--;
			   nbspaces++;
			 }
		     }
		 }
	       
	       if (nbspaces > 0)
		 {
		   if (length == 0)
		     /* empty TEXT element */
		     TtaDeleteTree (lastLeaf, XMLcontext.doc);
		   else
		     /* remove the ending spaces */
		     TtaDeleteTextContent (lastLeaf, length + 1,
					   nbspaces, XMLcontext.doc);
		 }
	     }
	 }
     }
}

/*----------------------------------------------------------------------
   XmlCloseElement
   Terminate the corresponding Thot element.
  ----------------------------------------------------------------------*/
static ThotBool     XmlCloseElement (char *mappedName)
{
   int              i, error;
   Element          el, parent;
   ElementType      parentType;
   ThotBool         ret, spacesDeleted;

   ret = FALSE;

   if (stackLevel > 0)
     {
       el = XMLcontext.lastElement;
       if (XMLcontext.lastElementClosed)
	  el = TtaGetParent (el);
       i = stackLevel - 1;

       if (i >= 0 && mappedName == nameElementStack[i])
	 /* element found in the stack */
	 {
	   /* This element and its whole subtree are closed */
	   stackLevel = i;
	   XMLcontext.lastElement = elementStack[i];
	   XMLcontext.lastElementClosed = TRUE;
	   ret = TRUE;
	 }
       else
	 {
	   /* element not found in the stack */
	   if (XMLcontext.lastElement != NULL)
	     {
	       /* implicit close. Check the parent of current element */
	       if (InsertSibling ())
		 parent = TtaGetParent (XMLcontext.lastElement);
	       else
		 parent = XMLcontext.lastElement;
	       if (parent != NULL)
		 {
		   parentType = TtaGetElementType (parent);
		   if (parentType.ElTypeNum == HTML_EL_Pseudo_paragraph)
		     {
		       XMLcontext.lastElement = parent;
		       XMLcontext.lastElementClosed = TRUE;
		       ret = TRUE;
		     }
		 }
	     }
	 }

       if (ret)
	 /* successful close */
	 {
	   /* remove closed elements from the stack */
	   while (i > 0)
	     if (elementStack[i] == XMLcontext.lastElement)
	       {
		 stackLevel = i;
		 i = 0;
	       }
	     else
	       {
		 if (TtaIsAncestor (elementStack[i], XMLcontext.lastElement))
	             stackLevel = i;
	         i--;
	       }

	   /* complete all closed elements */
	   if (el != XMLcontext.lastElement)
	       if (!TtaIsAncestor(el, XMLcontext.lastElement))
	           el = NULL;
	   
	   spacesDeleted = FALSE;
	   while (el != NULL)
	     {
	       /* If the element closed is a block-element, remove */
	       /* spaces contained at the end of that element */
	       /*
	       if (!spacesDeleted)
	          spacesDeleted = RemoveEndingSpaces (el);
	       */

	       /* Remove the trailing spaces of that element */
	       RemoveTrailingSpaces (el);

	       (*(currentParserCtxt->ElementComplete)) (el, XMLcontext.doc,
							&error);

	       if (el == XMLcontext.lastElement)
		 el = NULL;
	       else
		 el = TtaGetParent (el);
	     }

	   if (stackLevel > 1)
	     {
	       XMLcontext.language = languageStack[stackLevel - 1];
	       currentParserCtxt = parserCtxtStack[stackLevel - 1];
	       /* Is there a space attribute in the stack ? */
	       XmlWhiteSpaceHandling ();
	     }
	 }
     }
   return ret;
}

/*--------------------  StartElement  (start)  ---------------------*/

/*----------------------------------------------------------------------
   XmlCheckContext
   Verifies if the element elName is allowed to occur in the current
   structural context.
  ----------------------------------------------------------------------*/
static void   XmlCheckContext (char *elName,
			       ElementType elType,
			       ThotBool *isAllowed)

{
  *isAllowed = TRUE;
  return;
}

/*----------------------------------------------------------------------
   XhtmlCheckContext
   Verifies if the XHTML element elName is allowed to occur 
   in the current structural context.
  ----------------------------------------------------------------------*/
static void   XhtmlCheckContext (char *elName,
				 ElementType elType,
				 ThotBool *isAllowed)

{
   if (stackLevel <= 1 || nameElementStack[stackLevel - 1] == NULL)
     {
       *isAllowed = TRUE;
       return;
     }
   else
     {
       *isAllowed = TRUE;
       /* only TH and TD elements are allowed as children of a TR element */
       if (!strcmp (nameElementStack[stackLevel - 1], "tr"))
	 if (strcmp (elName, "th") &&
	     strcmp (elName, "td"))
	   *isAllowed = FALSE;

       if (*isAllowed &&
	   !strcmp (nameElementStack[stackLevel - 1], "table"))
	 /* only CAPTION, THEAD, TFOOT, TBODY, COLGROUP, COL and TR are */
	 /* allowed as children of a TABLE element */
	 {
	   if (strcmp (elName, "caption")  &&
	       strcmp (elName, "thead")    &&
	       strcmp (elName, "tfoot")    &&
	       strcmp (elName, "tbody")    &&
	       strcmp (elName, "colgroup") &&
	       strcmp (elName, "col")      &&
	       strcmp (elName, "tr"))
	     {
	       if (!strcmp (elName, "td") ||
		   !strcmp (elName, "th"))
		 /* Table cell within a table, without a tr. Assume tr */
		 {
		   /* simulate a <TR> tag */
		   StartOfXmlStartElement (NULL, "tr");
		 }
	       else
		 *isAllowed = FALSE;
	     }
	 }

       if (*isAllowed)
	 /* CAPTION, THEAD, TFOOT, TBODY, COLGROUP are allowed only as
	    children of a TABLE element */
	 if (strcmp (elName, "caption")  == 0 ||
	     strcmp (elName, "thead")    == 0 ||
	     strcmp (elName, "tfoot")    == 0 ||
	     strcmp (elName, "tbody")    == 0 ||
	     strcmp (elName, "colgroup") == 0)
	     if (strcmp (nameElementStack[stackLevel - 1], "table") != 0)
		 *isAllowed = FALSE;

       if (*isAllowed)
	 {
	   /* only TR is allowed as a child of a THEAD, TFOOT or TBODY element */
	   if (!strcmp (nameElementStack[stackLevel - 1], "thead") ||
	       !strcmp (nameElementStack[stackLevel - 1], "tfoot") ||
	       !strcmp (nameElementStack[stackLevel - 1], "tbody"))
	     {
	       if (strcmp (elName, "tr"))
		 {
	         if (!strcmp (elName, "td") ||
		     !strcmp (elName, "th"))
		   /* Table cell within a thead, tfoot or tbody without a tr. */
		   /* Assume tr */
		   {
		     /* simulate a <tr> tag */
		     StartOfXmlStartElement (NULL, "tr");
		   }
		 else
		   *isAllowed = FALSE;
		 }
	     }
	 }

       if (*isAllowed)
	 {
	   /* Block elements are not allowed within an anchor */
	   if (!strcmp (nameElementStack[stackLevel - 1], "a") &&
	       (!IsXMLElementInline (elType)))
	       *isAllowed = FALSE;
	 }
       
       if (*isAllowed &&
	   strcmp (elName, "body") == 0 &&
	   XmlWithinStack (HTML_EL_BODY, DocumentSSchema))
	 /* refuse BODY within BODY */
	 *isAllowed = FALSE;
       
       if (*isAllowed)
	 /* refuse HEAD within HEAD */
	 if (strcmp (elName, "head") == 0)
	   if (XmlWithinStack (HTML_EL_HEAD, DocumentSSchema))
	     *isAllowed = FALSE;

       if (*isAllowed)
	 /* refuse STYLE within STYLE */
	 if (strcmp (elName, "style") == 0)
	   if (XmlWithinStack (HTML_EL_STYLE_, DocumentSSchema))
	     *isAllowed = FALSE;
       
       return;
     }
}

/*----------------------------------------------------------------------
   GetXmlElType
   Search in the mapping tables the entry for the element type of
   name Xmlname and returns the corresponding Thot element type.
  ----------------------------------------------------------------------*/
static void   GetXmlElType (char *uriName,
			    char *elementName,
			    ElementType *elType,
			    char **mappedName,
			    char *content,
			    ThotBool *level)

{
#ifdef XML_GENERIC
  ThotBool    isnew;
#endif /* XML_GENERIC */

 /* initialize all parser contexts if not done yet */
  if (firstParserCtxt == NULL)
    InitXmlParserContexts ();

  /* Look at the current context if there is one */
  if (currentParserCtxt != NULL)
    {
#ifdef XML_GENERIC
      if (strcmp (currentParserCtxt->SSchemaName, "XML") == 0)
	{
	  if (XMLRootName[0] == EOS)
	    {
	      /* This is the first parsed element */
	      elType->ElSSchema = GetGenericXMLSSchema (XMLcontext.doc);
	      /* We instanciate the XML schema with the element name */
	      /* (except for the elements 'Comments' and 'PI') */
	      if (strcmp (elementName, "XMLcomment") &&
		  strcmp (elementName, "XMLcomment_line") &&
		  strcmp (elementName, "XMLPI") &&
		  strcmp (elementName, "XMLPI_line"))
		TtaChangeGenericSchemaNames (uriName, elementName, XMLcontext.doc);
	    }
	  else
	    {
	      if (uriName != NULL)
		{
		  isnew = FALSE;
		  elType->ElSSchema = GetGenericXMLSSchemaByUri (uriName, XMLcontext.doc, &isnew);
		  if (isnew)
		    TtaChangeGenericSchemaNames (uriName, elementName, XMLcontext.doc);
		}
	      else
		  *elType = TtaGetElementType (RootElement);
	      *level = TRUE;
	      *content = SPACE;
	    }
	  MapGenericXmlElement (elementName, elType,
				mappedName, XMLcontext.doc);
	}
      else
	{
	  elType->ElSSchema = currentParserCtxt->XMLSSchema;
	  MapXMLElementType (currentParserCtxt->XMLtype, elementName, elType,
			     mappedName, content, level, XMLcontext.doc);
	}
#else /* XML_GENERIC */
      elType->ElSSchema = currentParserCtxt->XMLSSchema;
      MapXMLElementType (currentParserCtxt->XMLtype, elementName, elType,
			 mappedName, content, level, XMLcontext.doc);
#endif /* XML_GENERIC */
    }
  else
    {
      /* not found */
      elType->ElTypeNum = 0;
      elType->ElSSchema = NULL;
    }
}

/*----------------------------------------------------------------------
   StartOfXmlStartElement  
   The name of an element type has been read from a start tag.
   Create the corresponding Thot element according to the mapping table.
  ----------------------------------------------------------------------*/
static void       StartOfXmlStartElement (char *uriName, char *elementName)
{
  ElementType     elType;
  Element         newElement;
  char            msgBuffer[MaxMsgLength];
  char            schemaName[MAX_SS_NAME_LENGTH];
  char           *mappedName = NULL;
  ThotBool        elInStack = FALSE;
  ThotBool        highEnoughLevel = TRUE;
  ThotBool        isAllowed = TRUE;
      
  UnknownTag = FALSE;

  /* ignore tag <P> within PRE for Xhtml elements */
  if (currentParserCtxt != NULL &&
      (strcmp (currentParserCtxt->SSchemaName, "HTML") == 0) &&
      (XmlWithinStack (HTML_EL_Preformatted, currentParserCtxt->XMLSSchema)) &&
      (strcasecmp (elementName, "p") == 0))
    {
       UnknownTag = TRUE;
       return;
    }

  if (stackLevel == MAX_STACK_HEIGHT)
    {
      XmlParseError (errorNotWellFormed, "**FATAL** Too many XML levels", 0);
      UnknownTag = TRUE;
      return;
    }

  /* search the XML element name in the corresponding mapping table */
  elType.ElSSchema = NULL;
  elType.ElTypeNum = 0;
  currentElementName[0] = EOS;
  GetXmlElType (uriName, elementName, &elType, &mappedName,
		&currentElementContent, &highEnoughLevel);

  if (mappedName == NULL)
    {
      if (strcmp (currentParserCtxt->SSchemaName, "HTML") == 0)
	strcpy (schemaName, "XHTML");
      else if (strcmp (currentParserCtxt->SSchemaName, "GraphML") == 0)
	strcpy (schemaName, "SVG");
      else
	strcpy (schemaName, currentParserCtxt->SSchemaName);
      if (highEnoughLevel)
	{
	  /* element not found in the corresponding DTD */
	  /* don't process that element */
	  sprintf (msgBuffer, "Unknown %s element %s",
		   schemaName , elementName);
	  XmlParseError (errorParsing, msgBuffer, 0);
	  UnknownTag = TRUE;
 	}
      else
	{
	  /* element invalid for the current profile */
	  /* doesn't process that element */
	  sprintf (msgBuffer,
		    "Unknown %s element %s for the current profile",
		    schemaName, elementName);
	  XmlParseError (errorParsingProfile, msgBuffer, 0);
	  UnknownTag = TRUE;
	}
      return;
   }
  else
    strcpy (currentElementName, mappedName);
  
  /* Element found in the corresponding DTD */
  if (currentParserCtxt != NULL)
    (*(currentParserCtxt->CheckContext))(mappedName, elType, &isAllowed);
  if (!isAllowed)
    /* Element not allowed in the current structural context */
    {
      sprintf (msgBuffer,
		"The XML element %s is not allowed here", elementName);
      XmlParseError (errorParsing, msgBuffer, 0);
      UnknownTag = TRUE;
      elInStack = FALSE;
    }
  else
    {
      newElement = NULL;
      /* create a Thot element */
      if (currentElementContent == 'E')
	/* empty XML element. Create all children specified */
	/* in the Thot structure schema */
	newElement = TtaNewTree (XMLcontext.doc, elType, "");
      else
	/* the HTML element may have children. Create only */
	/* the corresponding Thot element, without any child */
	newElement = TtaNewElement (XMLcontext.doc, elType);
      
      XmlSetElemLineNumber (newElement);
      InsertXmlElement (&newElement);

      if (newElement != NULL && elType.ElTypeNum == 1)
	/* If an empty Text element has been created, the */
	/* following character data must go to that element */
	XMLcontext.mergeText = TRUE;

      elementStack[stackLevel] = newElement;
      nameElementStack[stackLevel] = mappedName;
      elInStack = TRUE;
    }
  
  if (elInStack)
    {
      languageStack[stackLevel] = XMLcontext.language;
      parserCtxtStack[stackLevel] = currentParserCtxt;
      spacePreservedStack[stackLevel] = ' ';
      stackLevel++;
    }
}

/*----------------------------------------------------------------------
   EndOfXmlStartElement
   Function called at the end of a start tag.
  ----------------------------------------------------------------------*/
static void       EndOfXmlStartElement (char *elementName)
{
  ElementType     elType;
  AttributeType   attrType;
  Attribute       attr;
  int             length;
  char           *text;

  if (UnknownTag)
    return;

  if (XMLcontext.lastElement != NULL && currentElementName[0] != EOS)
    {
      elType = TtaGetElementType (XMLcontext.lastElement);
      if (strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
	{
	  if (!strcmp (nameElementStack[stackLevel - 1], "pre")   ||
	      !strcmp (nameElementStack[stackLevel - 1], "style") ||
	      !strcmp (nameElementStack[stackLevel - 1], "script"))
	    /* a <PRE>, <STYLE> or <SCRIPT> tag has been read */
	    XmlWhiteSpaceInStack (NULL);
	  else
	    if (!strcmp (nameElementStack[stackLevel - 1], "table"))
	      /* <TABLE> has been read */
	      XMLcontext.withinTable++;
	  
	  /* if it's a LI element, creates its IntItemStyle attribute
	     according to surrounding elements */
	  SetAttrIntItemStyle (XMLcontext.lastElement, XMLcontext.doc);
	  
	  /* if it's an AREA element, computes its position and size */
	  ParseAreaCoords (XMLcontext.lastElement, XMLcontext.doc);
	  
	  /* if it's a STYLE element in CSS notation, activate the CSS */
	  /* parser for parsing the element content */
	  if (elType.ElTypeNum == HTML_EL_STYLE_)
	    {
	      /* Search the Notation attribute */
	      attrType.AttrSSchema = elType.ElSSchema;
	      attrType.AttrTypeNum = HTML_ATTR_Notation;
	      attr = TtaGetAttribute (XMLcontext.lastElement, attrType);
	      if (attr == NULL)
		/* No Notation attribute. Assume CSS by default */
		XMLcontext.parsingCSS = TRUE;
	      else
		/* the STYLE element has a Notation attribute */
		/* get its value */
		{
		  length = TtaGetTextAttributeLength (attr);
		  text = TtaGetMemory (length + 1);
		  TtaGiveTextAttributeValue (attr, text, &length);
		  if (!strcasecmp (text, "text/css"))
		    XMLcontext.parsingCSS = TRUE;
		  TtaFreeMemory (text);
		}
	    }
	  else
	    if (elType.ElTypeNum == HTML_EL_Text_Area)
	      {
		/* we have to read the content as a simple text unit */
		XMLcontext.parsingTextArea = TRUE;
	      }
	}
    }
  XmlWhiteSpaceHandling ();
}

/*----------------------  StartElement  (end)  -----------------------*/


/*----------------------  EndElement  (start)  -----------------------*/

/*----------------------------------------------------------------------
   EndOfXmlElement
   Terminate all corresponding Thot elements.
  ----------------------------------------------------------------------*/
static void       EndOfXmlElement (char *uriName, char *elementName)
{
   ElementType    elType;
   char           msgBuffer[MaxMsgLength];
   char           schemaName[MAX_SS_NAME_LENGTH];
   char          *mappedName = NULL;
   ThotBool       highEnoughLevel = TRUE;

   if (XMLcontext.parsingTextArea)
     if (strcasecmp (elementName, "textarea") != 0)
       /* We are parsing the contents of a textarea element. */
       /* The end tag is not the one closing the current textarea, */
       /* consider it as plain text */
       return;
   
   /* Ignore the virtual root of a XML sub-tree */
   if (strcmp (elementName, SUBTREE_ROOT) == 0)
     return;

   /* search the XML element name in the corresponding mapping table */
   elType.ElSSchema = NULL;
   elType.ElTypeNum = 0;
   currentElementName[0] = EOS;
   GetXmlElType (uriName, elementName, &elType, &mappedName,
		 &currentElementContent, &highEnoughLevel);

   if (mappedName == NULL)
     {
       if (strcmp (currentParserCtxt->SSchemaName, "HTML") == 0)
	 strcpy (schemaName, "XHTML");
       else if (strcmp (currentParserCtxt->SSchemaName, "GraphML") == 0)
	 strcpy (schemaName, "SVG");
       else
	 strcpy (schemaName, currentParserCtxt->SSchemaName);
       if (highEnoughLevel)
	 {
	   /* element not found in the corresponding DTD */
	   /* don't process that element */
	   sprintf (msgBuffer, "Unknown %s element %s",
		     schemaName, elementName);
	   XmlParseError (errorParsing, msgBuffer, 0);
	   UnknownTag = TRUE;
	 }
       else
	 {
	   /* element invalid for the current profile */
	   /* doesn't process that element */
	   sprintf (msgBuffer,
		     "Unknown %s element %s for the current profile",
		     schemaName, elementName);
	   XmlParseError (errorParsingProfile, msgBuffer, 0);
	   UnknownTag = TRUE;
	 }
     }
   else
     {
       /* element found in the corresponding DTD */
       if (!XmlCloseElement (mappedName))
	 {
	   /* the end tag does not close any current element */
	   sprintf (msgBuffer, "Unexpected end tag %s", elementName);
	   XmlParseError (errorParsing, msgBuffer, 0);
	 }
     }
}
/*---------------------  EndElement  (end)  --------------------------*/


/*----------------------  Data  (start)  -----------------------------*/

/*----------------------------------------------------------------------
   IsLeadingSpaceUseless
  ----------------------------------------------------------------------*/
static ThotBool  IsLeadingSpaceUseless ()
{
   ElementType   elType, lastElType, ancestorType;
   Element       parent, ancestor, prev;
   ThotBool      removeLeadingSpaces;

   if (InsertSibling ())
     /* There is a previous sibling (XMLcontext.lastElement) 
	for the new Text element */
     {
       parent = TtaGetParent (XMLcontext.lastElement);
       if (parent == NULL)
	 parent = XMLcontext.lastElement;
       elType = TtaGetElementType (parent);
       lastElType = TtaGetElementType (XMLcontext.lastElement);
       removeLeadingSpaces = TRUE;
       if (IsXMLElementInline (lastElType))
	 {
	   if (strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML") != 0)
	     removeLeadingSpaces = FALSE;
	   else
	     {
	       if (elType.ElTypeNum != HTML_EL_Option_Menu &&
		   elType.ElTypeNum != HTML_EL_OptGroup)
		 {
		   removeLeadingSpaces = FALSE;
		   if (lastElType.ElTypeNum == HTML_EL_BR)
		     removeLeadingSpaces = TRUE;
		 }
	     }
	 }
       else
	 {
	   if ((strcmp (TtaGetSSchemaName (lastElType.ElSSchema), "HTML") == 0) &&
	       ((lastElType.ElTypeNum == HTML_EL_Comment_) ||
		(lastElType.ElTypeNum == HTML_EL_XMLPI)))
	     removeLeadingSpaces = FALSE;
	 }
     }
   else
     /* the new Text element should be the first child 
	of the latest element encountered */
     {
       parent = XMLcontext.lastElement;
       elType = TtaGetElementType (XMLcontext.lastElement);
       removeLeadingSpaces = TRUE;
       if ((strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML") != 0) ||
	   ((strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML")  == 0) &&
	    elType.ElTypeNum != HTML_EL_Option_Menu &&
	    elType.ElTypeNum != HTML_EL_OptGroup))
	 {
	   ancestor = parent;
	   ancestorType = TtaGetElementType (ancestor);
	   while (removeLeadingSpaces &&
		  IsXMLElementInline (ancestorType))
	     {
	       prev = ancestor;
	       TtaPreviousSibling (&prev);
	       if (prev == NULL)
		 {
		   ancestor = TtaGetParent (ancestor);
		   ancestorType = TtaGetElementType (ancestor);
		 }
	       else
		 removeLeadingSpaces = FALSE;
	     }
	 }
     }
   return removeLeadingSpaces;
}

/*----------------------------------------------------------------------
   PutInXmlElement
  ----------------------------------------------------------------------*/
void            PutInXmlElement (char *data)

{
   ElementType  elType;
   Element      elText;
   Language     lang;
   char        *buffer, *bufferws, *buffertext;
   int          i = 0;
   int          length;
   int          i1, i2 = 0, i3 = 0;
   ThotBool     uselessSpace = FALSE;

   i = 0;
   /* Immediately after a start tag, treatment of the leading spaces */
   /* If RemoveLeadingSpace = TRUE, we suppress all leading white-space */
   /* characters, otherwise, we only suppress the first line break*/
   if (ImmediatelyAfterTag)
     {
       if (RemoveLeadingSpace)
	 {
	   while (data[i] == EOL || data[i] == CR ||
		   data[i] == TAB || data[i] == SPACE)
	     i++;
	   if (data[i] != EOS)
	     ImmediatelyAfterTag = FALSE;
	 }
       else
	 {
	   if (data[0] == EOL || data[0] == CR)
	     i = 1;
	   ImmediatelyAfterTag = FALSE;
	 }
     }
 
   if (data[i] == EOS)
     return;

   length = strlen (&(data[i]));
   bufferws = TtaGetMemory (length+1);
   strcpy (bufferws, &(data[i]));
   
   /* Convert line-break or tabs into space character */
   i = 0;
   if (RemoveLineBreak)
     {
       while (bufferws[i] != EOS)
	 {
	   if (bufferws[i] == EOL || bufferws[i] == CR ||
	       bufferws[i] == TAB)
	     bufferws[i]= SPACE;
	   i++;
	 }
     }

   if (XMLcontext.lastElement != NULL)
     i = 0;
     {
       /* Suppress the leading spaces in Inline elements */
       uselessSpace = IsLeadingSpaceUseless ();
       if (RemoveLeadingSpace && uselessSpace)
	 /* suppress leading spaces */
	   while (bufferws[i] == SPACE)
	     i++;
       
       /* Collapse contiguous spaces */ 
       if (bufferws[i] != EOS)
	 {
	   length = strlen (bufferws);
	   buffer = TtaGetMemory (length+1);
	   if (RemoveContiguousSpace)
	     {
	       for (i1 = i; i1 <= length; i1++)
		 {
		   if ((unsigned int)bufferws[i1] <= SPACE && bufferws[i1] != EOS)
		     i3++;
		   else
		     i3 = 0;
		   if (i3 <= 1)
		     buffer[i2++] = bufferws[i1];
		 }
	     }
	   else
	     strcpy (buffer, bufferws);
	   i1 = 0;
	   
	   /* Filling of the element value */
	   elType = TtaGetElementType (XMLcontext.lastElement);
	   if (elType.ElTypeNum == 1 && XMLcontext.mergeText)
	     {
	       if ((buffer[i1] == SPACE) && RemoveContiguousSpace)
		 {
		   /* Is the last character of text element a space */
		   length = TtaGetTextLength (XMLcontext.lastElement) + 1;
		   buffertext = TtaGetMemory (length);
		   TtaGiveTextContent (XMLcontext.lastElement,
				       buffertext, &length, &lang);
		   /* Remove leading space if last content was finished by a space */
		   if ((buffertext[length-1] == SPACE))
		     TtaAppendTextContent (XMLcontext.lastElement,
					   &(buffer[i1+1]), XMLcontext.doc);
		   else
		     TtaAppendTextContent (XMLcontext.lastElement,
					   &(buffer[i1]), XMLcontext.doc);
		   TtaFreeMemory (buffertext);
		 }
	       else
		 TtaAppendTextContent (XMLcontext.lastElement,
				       &(buffer[i1]), XMLcontext.doc);
	     }
	   else
	     {
	       /* create a TEXT element */
	       /* elType.ElSSchema = currentParserCtxt->XMLSSchema; */
	       elType.ElTypeNum = 1;
	       elText = TtaNewElement (XMLcontext.doc, elType);
	       XmlSetElemLineNumber (elText);
	       InsertXmlElement (&elText);
	       XMLcontext.lastElementClosed = TRUE;
	       XMLcontext.mergeText = TRUE;
	       /* put the content of the input buffer into the TEXT element */
	       if (elText != NULL)
		 TtaSetTextContent (elText, &(buffer[i1]),
				    XMLcontext.language, XMLcontext.doc);
	     }
	   TtaFreeMemory (buffer);
	 }
       TtaFreeMemory (bufferws);
     }
}
/*----------------------  Data  (end)  ---------------------------*/


/*--------------------  Attributes  (start)  ---------------------*/

/*----------------------------------------------------------------------
   EndOfXhtmlAttributeName   
   End of a XHTML attribute
  ----------------------------------------------------------------------*/
static void          EndOfXhtmlAttributeName (char *attrName,
					      Element el,
					      Document doc)

{
 AttributeMapping*   mapAttr;
 AttributeType       attrType;
 ElementType         elType;
 Attribute           attr;
 char                translation;
 ThotBool            highEnoughLevel = TRUE;
 char                msgBuffer[MaxMsgLength];


   UnknownAttr = FALSE;
   attrType.AttrTypeNum = 0;

   mapAttr = MapHTMLAttribute (attrName, &attrType,
			       currentElementName, &highEnoughLevel, doc);

   if (attrType.AttrTypeNum <= 0)
     {
       /* this attribute is not in the mapping table */
       if (strcasecmp (attrName, "xml:lang") == 0)
	 /* attribute xml:lang is not considered as invalid, but it is ignored */
	 lastMappedAttr = NULL;
       else
	 {
	   if (highEnoughLevel)
	     {
	       sprintf (msgBuffer, "Unknown XHTML attribute %s", attrName);
	       XmlParseError (errorParsing, msgBuffer, 0);
	     }
	   else
	     {
	       sprintf (msgBuffer,
			 "Unknown XHTML attribute %s for the current profile",
			 attrName);
	       XmlParseError (errorParsingProfile, msgBuffer, 0);
	     }
	   /* attach an Invalid_attribute to the current element */
	   mapAttr = MapHTMLAttribute ("unknown_attr", &attrType,
				       currentElementName,
				       &highEnoughLevel, doc);
	   UnknownAttr = TRUE;
	 }
     }
   
   if (attrType.AttrTypeNum > 0 && el != NULL &&
       (!XMLcontext.lastElementClosed ||
	(XMLcontext.lastElement != RootElement)))
     {
       lastMappedAttr = mapAttr;
       translation = lastMappedAttr->AttrOrContent;       

       switch (translation)
	 {
	 case 'C':	/* Content */
	   /* Nothing to do yet: wait for attribute value */
	   break;
	   
	 case 'A':
	   /* create an attribute for current element */
	   CreateHTMLAttribute (el, attrType, attrName, UnknownAttr,
				doc, &currentAttribute, &lastAttrElement);
	   if (attrType.AttrTypeNum == HTML_ATTR_HREF_)
	     {
	       elType = TtaGetElementType (el);
	       if (elType.ElTypeNum == HTML_EL_Anchor)
		 /* attribute HREF for element Anchor */
		 /* create attribute PseudoClass = link */
		 {
		   attrType.AttrTypeNum = HTML_ATTR_PseudoClass;
		   attr = TtaNewAttribute (attrType);
		   TtaAttachAttribute (el, attr, doc);
		   TtaSetAttributeText (attr, "link", el, doc);
		 }
	     }
	   else
	     if (attrType.AttrTypeNum == HTML_ATTR_Checked)
	       {
		 /* create Default-Checked attribute */
		 attrType.AttrTypeNum = HTML_ATTR_DefaultChecked;
		 attr = TtaNewAttribute (attrType);
		 TtaAttachAttribute (el, attr, doc);
		 TtaSetAttributeValue (attr, HTML_ATTR_DefaultChecked_VAL_Yes_,
				       el, doc);
	       }
	     else 
	       if (attrType.AttrTypeNum == HTML_ATTR_Selected)
		 {
		   /* create Default-Selected attribute */
		   attrType.AttrTypeNum = HTML_ATTR_DefaultSelected;
		   attr = TtaNewAttribute (attrType);
		   TtaAttachAttribute (el, attr, doc);
		   TtaSetAttributeValue (attr, HTML_ATTR_DefaultSelected_VAL_Yes_,
					 el, doc);
		 }
	   break;
	   
	 case SPACE:
	   /* nothing to do */
	   break;
	   
	 default:
	   break;
	 }
     }
}

/*----------------------------------------------------------------------
   XmlEndOfAttrName
   End of a XML attribute that doesn't belongs to the XHTML DTD
  ----------------------------------------------------------------------*/
static void         EndOfXmlAttributeName (char *attrName,
					   char *uriName,
					   Element  el,
					   Document doc)
     
{
   AttributeType    attrType;
   Attribute        attr;
   char             msgBuffer[MaxMsgLength];
   ThotBool         level = TRUE;
   char             schemaName[MAX_SS_NAME_LENGTH];
#ifdef XML_GENERIC   
   ElementType      elType;
   ThotBool         isnew;
#endif /* XML_GENERIC */

   attrType.AttrTypeNum = 0;

#ifdef XML_GENERIC   
   if (strcmp (currentParserCtxt->SSchemaName, "XML") == 0)
     {
       if (uriName != NULL)
	 {
	   isnew = FALSE;
	   attrType.AttrSSchema = GetGenericXMLSSchemaByUri (uriName, XMLcontext.doc, &isnew);
	   if (isnew)
	     TtaChangeGenericSchemaNames (uriName, NULL, XMLcontext.doc);
	 }
       else
	 {
	   elType = TtaGetElementType (XMLcontext.lastElement);
	   attrType.AttrSSchema = elType.ElSSchema;
	 }
       MapGenericXmlAttribute (attrName, &attrType, doc);
     }
   else
     { 
       if (currentParserCtxt->MapAttribute)
	 (*(currentParserCtxt->MapAttribute)) (attrName, &attrType,
					       currentElementName,
					       &level, doc);
     }
#else /* XML_GENERIC */
       if (currentParserCtxt->MapAttribute)
	 (*(currentParserCtxt->MapAttribute)) (attrName, &attrType,
					       currentElementName,
					       &level, doc);
#endif /* XML_GENERIC */

   if (attrType.AttrTypeNum <= 0)
     /* this attribute is not in a mapping table */
     {
       if (strcmp (currentParserCtxt->SSchemaName, "GraphML") == 0)
	 strcpy (schemaName, "SVG");
       else
	 strcpy (schemaName, currentParserCtxt->SSchemaName);
       sprintf (msgBuffer, "Unknown %s attribute %s",
		schemaName, attrName);
       XmlParseError (errorParsing, msgBuffer, 0);
     }
   else
     {
       attr = TtaGetAttribute (el, attrType);
       if (!attr)
	 {
	   attr = TtaNewAttribute (attrType);
	   TtaAttachAttribute (el, attr, doc);
	 }
       currentAttribute = attr;
       if (strcasecmp (attrName, "style") == 0)
	 HTMLStyleAttribute = TRUE;
     }
}

/*----------------------------------------------------------------------
  EndOfAttributeName   
  A XML attribute has been read. 
  Create the corresponding Thot attribute.
  ----------------------------------------------------------------------*/
static void    EndOfAttributeName (char *xmlName)
     
{
   char     *buffer;
   char     *attrName;
   char     *ptr = NULL;

   currentAttribute = NULL;
   lastMappedAttr = NULL;
   HTMLStyleAttribute = FALSE;
   XMLSpaceAttribute = FALSE;
 
   if (UnknownTag)
     return;

   /* look for a NS_SEP in the tag name (namespaces) */ 
   /* and ignore the prefix if there is one */
   buffer = TtaGetMemory (strlen (xmlName) + 1);
   strcpy (buffer, (char*) xmlName);
   if ((ptr = strrchr (buffer, NS_SEP)) != NULL)
     {
       *ptr = EOS;
       ptr++;

       /* Specific treatment to get round a bug in EXPAT parser */
       /* This one replaces first "xml:" prefix by the namespaces URI */
       if (strcmp (buffer, NAMESPACE_URI) == 0)
	 {
	   attrName = TtaGetMemory (strlen (ptr) + 5);
	   strcpy (attrName, "xml:");
	   strcat (attrName, ptr);
	 }
       else
	 {
	   attrName = TtaGetMemory (strlen (ptr) + 1);
	   strcpy (attrName, ptr);
	   if (currentParserCtxt != NULL &&
	       strcmp (buffer, currentParserCtxt->UriName))
	     ChangeXmlParserContextUri (buffer);
	 }
     }
   else
     {
       attrName = TtaGetMemory (strlen (buffer) + 1);
       strcpy (attrName, buffer);
     }
   
#ifdef XML_GENERIC
   /* We assign the generic XML context by default */ 
   if (currentParserCtxt == NULL)
     currentParserCtxt = XmlGenericParserCtxt;
#endif /* XML_GENERIC */
   
   /* Is it a xml:space attribute */
   if (strncmp (attrName, "xml:space", 9) == 0)
     XMLSpaceAttribute = TRUE;
   
   /* the attribute xml:lang is replaced by the attribute "lang" */
   if (strncmp (attrName, "xml:lang", 8) == 0)
     strcpy (attrName, "lang");
   
   if (currentParserCtxt != NULL)
     {
       if (strcmp (currentParserCtxt->SSchemaName, "HTML") == 0)
	 EndOfXhtmlAttributeName (attrName,
				  XMLcontext.lastElement, XMLcontext.doc);
       else
	 EndOfXmlAttributeName (attrName, ptr,
				XMLcontext.lastElement, XMLcontext.doc);
     }
   
   TtaFreeMemory (buffer);
   TtaFreeMemory (attrName);
   return;
}

/*----------------------------------------------------------------------
   EndOfXmlAttributeValue
   An attribute value has been read for a element that
   doesn't belongs to XHTML DTD
  ----------------------------------------------------------------------*/
static void         EndOfXmlAttributeValue (char *attrValue)

{
   AttributeType    attrType;
   int		    attrKind, val;
   unsigned char    msgBuffer[MaxMsgLength];

   TtaGiveAttributeType (currentAttribute, &attrType, &attrKind);
   switch (attrKind)
     {
     case 0:       /* enumerate */
       (*(currentParserCtxt->MapAttributeValue)) (attrValue, attrType, &val);
       if (val <= 0)
	 {
	   sprintf (msgBuffer, "Unknown attribute value %s", attrValue);
	   XmlParseError (errorParsing, msgBuffer, 0);	
	 }
       else
	 TtaSetAttributeValue (currentAttribute, val,
			       XMLcontext.lastElement, XMLcontext.doc);
       break;
     case 1:       /* integer */
       sscanf (attrValue, "%d", &val);
       TtaSetAttributeValue (currentAttribute, val,
			     XMLcontext.lastElement, XMLcontext.doc);
       break;
     case 2:       /* text */
       TtaSetAttributeText (currentAttribute, attrValue,
			    XMLcontext.lastElement, XMLcontext.doc);
       if (HTMLStyleAttribute)
	 ParseHTMLSpecificStyle (XMLcontext.lastElement, attrValue,
				 XMLcontext.doc, 1, FALSE);
       break;
     case 3:       /* reference */
       break;
     }
   
   if (currentParserCtxt != NULL && !HTMLStyleAttribute)
     (*(currentParserCtxt->AttributeComplete)) (currentAttribute,
						XMLcontext.lastElement,
						XMLcontext.doc);
}

/*----------------------------------------------------------------------
   EndOfAttributeValue
   An attribute value has been read from the HTML file.
   Put that value in the current Thot attribute.
  ----------------------------------------------------------------------*/
static void       EndOfAttributeValue (unsigned char *attrValue,
				       unsigned char *attrName)
{

   unsigned char *buffer;
   unsigned char *srcbuf;
   wchar_t        wcharRead;
   unsigned char  charRead;
   int            nbBytesRead = 0;
   int            i = 0, j = 0;
   int            length;
   char           tmpbuf[10];
   int            tmplen;
   int            k, l, m;
   char          *entityName;
   int            entityValue;	
   ThotBool       found, end;

   if (lastMappedAttr != NULL  || currentAttribute != NULL) 
     {
       if (currentParserCtxt != NULL)
	 {
	   length = strlen (attrValue);
	   buffer = TtaGetMemory (2*length + 1);
	   buffer[j] = EOS;
	   entityName = TtaGetMemory (length + 1);

	   while (i < length)
	     {
	       srcbuf = (unsigned char *) &attrValue[i];
	       nbBytesRead = TtaGetNextWideCharFromMultibyteString (&wcharRead,
								    &srcbuf,
								    UTF_8);
	       i += nbBytesRead;
	       if (wcharRead <= 255)
		 {
		   /* It's an ISO-Latin1 character */
		   charRead = (unsigned char) wcharRead;
		   if (charRead == START_ENTITY)
		     {
		       /* Maybe it is the beginning of an entity */
		       l = 0;
		       end = FALSE;
		       entityName[l++] = START_ENTITY;
		       for (k = i; k < length && !end; k++)
			 {
			   if (attrValue[k] == '&')
			     {
			       /* An '&' inside an other '&' ?? We suppose */
			       /* the first one doesn't belong to an entity */
			       k = length;
			       buffer [j++] = (char) START_ENTITY;
			     }
			   else if (attrValue[k] == ';')
			     {
			       /* End of the entity */
			       end = TRUE;
			       i = k + 1;
			       entityName[l] = EOS;
			       found = MapXMLEntity (currentParserCtxt->XMLtype,
						     &entityName[1], &entityValue);
			       if (found && entityValue <= 255)
				 {
				   /* It is an ISO latin1 character */
				   buffer [j++] = (char) entityValue;
				 }
			       else
				 {
				   for (m = 0; entityName[m] != EOS; m++)
				     buffer[j++] = entityName[m];
				   buffer[j++] = ';';
				 }
			     }
			   else
			     entityName[l++] = attrValue[k];
			 }
		     }
		   else
		     buffer[j++] = charRead;
		 }
	       else
		 {
		   /* It's not an ISO-Latin1 character */
		   tmplen = sprintf (tmpbuf, "%d", (int) wcharRead);
		   buffer[j++] = (char) START_ENTITY;
		   buffer[j++] = '#';
		   for (k = 0; k < tmplen; k++)
		       buffer[j++] = tmpbuf[k];
		   buffer[j++] = ';';
		 }
	     }

	   if (buffer[0] != EOS)
	     buffer[j] = EOS; 
	   if (XMLSpaceAttribute)
	     XmlWhiteSpaceInStack (buffer);
	   if (strcmp (currentParserCtxt->SSchemaName, "HTML") == 0)
	     EndOfHTMLAttributeValue (buffer, lastMappedAttr,
				      currentAttribute, lastAttrElement,
				      UnknownAttr, &XMLcontext, TRUE);
	   else
	     EndOfXmlAttributeValue (buffer);

	   TtaFreeMemory (buffer);
	   TtaFreeMemory (entityName);
	 }
     }
   
   currentAttribute = NULL;
   HTMLStyleAttribute = FALSE;
   XMLSpaceAttribute = FALSE;
}

/*--------------------  Attributes  (end)  ---------------------*/


/*--------------------  Entities  (start)  ---------------------*/

/*----------------------------------------------------------------------
   CreateXmlEntity
   End of a XML entity. 
   Search that entity in the corresponding entity table and 
   put the corresponding character in the corresponding element.
  ----------------------------------------------------------------------*/
static void     CreateXmlEntity (char *data, int length)
{
   char         schemaName[MAX_SS_NAME_LENGTH];
   char         msgBuffer[MaxMsgLength];
   char        *buffer;
   int          entityValue, i;	
   ThotBool     found;
      
  /* Name of the entity without '&' and ';' */
   buffer = TtaGetMemory (length);
   for (i = 0; i < length-1; i++)
       buffer[i] = data[i+1];
   if (buffer[i-1] == ';')
     buffer[i-1] = EOS;
   else
     buffer[i] = EOS;

  /* Look at the current context if there is one */
   if (currentParserCtxt != NULL)
     {
       found = MapXMLEntity (currentParserCtxt->XMLtype,
			     buffer, &entityValue);
       if (found)
	 {
	   /* Creation of the entity */
	   (*(currentParserCtxt->EntityCreated)) (entityValue, buffer,
						  TRUE, &XMLcontext);
	 }
       else
 	 {
	   /* Entity not supported */
	   if (strcmp (currentParserCtxt->SSchemaName, "HTML") == 0)
	     strcpy (schemaName, "XHTML");
	   else if (strcmp (currentParserCtxt->SSchemaName, "GraphML") == 0)
	     strcpy (schemaName, "SVG");
	   else
	     strcpy (schemaName, currentParserCtxt->SSchemaName);
	   sprintf (msgBuffer, "%s entity not supported : &%s;",
		     schemaName, buffer);
	   XmlParseError (errorParsing, msgBuffer, 0);
	   (*(currentParserCtxt->EntityCreated)) (entityValue, buffer,
						  FALSE, &XMLcontext);
	 }
    }
   else
     {
       /* not found */
       sprintf (msgBuffer,
		 "Namespace not supported for the XML entity %s", buffer);
       XmlParseError (errorParsing, msgBuffer, 0);
       return;
     }

   TtaFreeMemory (buffer);
}
/*--------------------  Entities  (end)  ---------------------*/


/*--------------------  Comments  (start)  ---------------------*/
/*----------------------------------------------------------------------
   CreateXmlComment
   Create a comment element into the Thot tree.
  ----------------------------------------------------------------------*/
static void       CreateXmlComment (char *commentValue)
{
   ElementType    elType, elTypeLeaf;
   Element  	  commentEl, commentLineEl, commentLeaf, lastChild;
   char          *mappedName;
   char           cont;
   char           fallback[5];
   unsigned char *buffer;
   unsigned char *srcbuf;
   wchar_t        wcharRead;
   int            length, i, j,error;
   int            nbBytesRead = 0;
   Language       lang;
   ThotBool       level = TRUE;

   length = strlen (commentValue);
   buffer = TtaGetMemory (length + 1);
   i = 0; j = 0;
   buffer[j] = EOS;

   /* Create a Thot element for the comment */
   elType.ElSSchema = NULL;
   elType.ElTypeNum = 0;

   GetXmlElType (NULL, "XMLcomment", &elType,
		 &mappedName, &cont, &level);
   if (elType.ElTypeNum > 0)
     {
       commentEl = TtaNewElement (XMLcontext.doc, elType);
       XmlSetElemLineNumber (commentEl);
       InsertXmlElement (&commentEl);
       /* Create a XMLcomment_line element as the first child of */
       /* Element XMLcomment */
       elType.ElSSchema = NULL;
       elType.ElTypeNum = 0;
       GetXmlElType (NULL, "XMLcomment_line", &elType,
		     &mappedName, &cont, &level);
       commentLineEl = TtaNewElement (XMLcontext.doc, elType);
       XmlSetElemLineNumber (commentLineEl);
       TtaInsertFirstChild (&commentLineEl, commentEl, XMLcontext.doc);
   
       while (i < length)
	 {
	   srcbuf = (unsigned char *) &commentValue[i];
	   nbBytesRead = TtaGetNextWideCharFromMultibyteString (&wcharRead,
								&srcbuf, UTF_8);
	   i += nbBytesRead;
	   
	   if (wcharRead < 0x100)
	     {
	       /* Look for line break in the comment and create as many */
	       /* XMLcomment_line elements as needed */
	       if ((int)wcharRead == EOL || (int)wcharRead == __CR__)
		 /* New line */
		 {
		   /* Put the current content into a text comment line */
		   buffer[j] = EOS;
		   elTypeLeaf.ElSSchema = elType.ElSSchema;
		   elTypeLeaf.ElTypeNum = 1;
		   commentLeaf = TtaNewElement (XMLcontext.doc, elTypeLeaf);
		   XmlSetElemLineNumber (commentLeaf);
		   if ((lastChild = TtaGetLastChild (commentLineEl)) == NULL)
		     TtaInsertFirstChild (&commentLeaf, commentLineEl,
					  XMLcontext.doc);
		   else
		     TtaInsertSibling (commentLeaf, lastChild,
				       FALSE, XMLcontext.doc);	     
		   TtaSetTextContent (commentLeaf, &buffer[0],
				      XMLcontext.language, XMLcontext.doc);
		   j = 0;
		   buffer[j] = EOS;
		   /* Create a new XMLcomment_line element */
		   commentLineEl = TtaNewElement (XMLcontext.doc, elType);
		   XmlSetElemLineNumber (commentLineEl);
		   /* Inserts the new XMLcomment_line after the previous one */
		   TtaInsertSibling (commentLineEl, TtaGetParent (commentLeaf),
				     FALSE, XMLcontext.doc);
		 }
	       else
		   buffer[j++] = (char) wcharRead;
	     }
	   else
	     {
	       /* It's not an 8bits character */
	       if (buffer[0] != EOS)
		 {
		   /* Put the current content into a text comment line */
		   buffer[j] = EOS;
		   /* Create a text element as child of element XMLcomment_line */
		   elTypeLeaf.ElSSchema = elType.ElSSchema;
		   elTypeLeaf.ElTypeNum = 1;
		   commentLeaf = TtaNewElement (XMLcontext.doc, elTypeLeaf);
		   XmlSetElemLineNumber (commentLeaf);
		   if ((lastChild = TtaGetLastChild (commentLineEl)) == NULL)
		     TtaInsertFirstChild (&commentLeaf, commentLineEl,
					  XMLcontext.doc);
		   else
		     TtaInsertSibling (commentLeaf, lastChild,
				       FALSE, XMLcontext.doc);	     
		   TtaSetTextContent (commentLeaf, &buffer[0],
				      XMLcontext.language, XMLcontext.doc);
		   j = 0;
		   buffer[j] = EOS;
		 }
	       /* Try to find a fallback character */
	       GetFallbackCharacter ((int) wcharRead, fallback, &lang);
	       if (fallback[0] == '?')
		 {
		   /* The character is not found in the fallback table */
		   /* Create a symbol leaf */
		   elTypeLeaf.ElSSchema = elType.ElSSchema;
		   elTypeLeaf.ElTypeNum = 3;
		   commentLeaf = TtaNewElement (XMLcontext.doc, elTypeLeaf);
		   XmlSetElemLineNumber (commentLeaf);
		   if ((lastChild = TtaGetLastChild (commentLineEl)) == NULL)
		     TtaInsertFirstChild (&commentLeaf, commentLineEl,
					  XMLcontext.doc);
		   else
		     TtaInsertSibling (commentLeaf, lastChild,
				       FALSE, XMLcontext.doc);	     
		   /* Put the symbol '?' into the new symbol leaf */
		   TtaSetGraphicsShape (commentLeaf, fallback[0], XMLcontext.doc);
		   /* Changes the wide char code associated with that symbol */
		   TtaSetSymbolCode (commentLeaf, wcharRead, XMLcontext.doc);
		   /* Make that leaf read-only */
		   TtaSetAccessRight (commentLeaf, ReadOnly, XMLcontext.doc);
		 }
	       else
		 {
		   /* The character is found in the fallback table */
		   /* Create a new text leaf */
		   elTypeLeaf.ElSSchema = elType.ElSSchema;
		   elTypeLeaf.ElTypeNum = 1;
		   commentLeaf = TtaNewElement (XMLcontext.doc, elTypeLeaf);
		   XmlSetElemLineNumber (commentLeaf);
		   if ((lastChild = TtaGetLastChild (commentLineEl)) == NULL)
		     TtaInsertFirstChild (&commentLeaf, commentLineEl,
					  XMLcontext.doc);
		   else
		     TtaInsertSibling (commentLeaf, lastChild,
				       FALSE, XMLcontext.doc);	     
		   /* Put the fallback character into the new text leaf */
		   TtaSetTextContent (commentLeaf, fallback, lang, XMLcontext.doc);
		 }
	     }
	 }

       /* Process last line */
       if (buffer[0] != EOS)
	 {
	   buffer[j] = EOS;
	   elTypeLeaf.ElSSchema = elType.ElSSchema;
	   elTypeLeaf.ElTypeNum = 1;
	   commentLeaf = TtaNewElement (XMLcontext.doc, elTypeLeaf);
	   XmlSetElemLineNumber (commentLeaf);
	   if ((lastChild = TtaGetLastChild (commentLineEl)) == NULL)
	     TtaInsertFirstChild (&commentLeaf, commentLineEl, XMLcontext.doc);
	   else
	     TtaInsertSibling (commentLeaf, lastChild, FALSE, XMLcontext.doc);	     
	   TtaSetTextContent (commentLeaf, &buffer[0],
			      XMLcontext.language, XMLcontext.doc);
	 }
       
       (*(currentParserCtxt->ElementComplete)) (commentEl, XMLcontext.doc,
						&error);
       XMLcontext.lastElementClosed = TRUE;
       TtaFreeMemory (buffer);
     }
}

/*--------------------  Comments  (end)  ------------------------------*/

/*--------------------  PI  (start)  ----------------------------------*/
/*----------------------------------------------------------------------
   XmlStyleSheetPi
  ---------------------------------------------------------------------*/
static void      XmlStyleSheetPi (char *PiData)
{
   int           length, i, j;
   char         *ptr, *end;
   char         *buffer, *tmpbuffer;
   char         *css_href = NULL;
   char          delimitor;
   char          msgBuffer[MaxMsgLength];
   CSSmedia      css_media;
   CSSInfoPtr    css_info;
   ThotBool      ok;

   length = strlen (PiData);
   buffer = TtaGetMemory (length + 1);
   i = 0; j = 0;
   
   /* get the "type" attribute */
   ok = FALSE;
   end = NULL;
   strcpy (buffer, PiData);
   ptr = strstr (buffer, "type");
   if (ptr)
     {
       ptr = strstr (ptr, "=");
       ptr++;
       while (ptr[0] != EOS && ptr[0] == ' ')
	 ptr++;
       if (ptr[0] != EOS)
	 {
	   delimitor = ptr[0];
	   tmpbuffer = TtaGetMemory (length + 1);
	   end = strchr (&ptr[1], delimitor);
	   if (end && end[0] != EOS && tmpbuffer != NULL)
	     {
	       end[0] = EOS;
	       strcpy (tmpbuffer, &ptr[1]);
	       if (!strcmp (tmpbuffer, "text/css"))
		 ok = TRUE;
	     }
	   if (tmpbuffer != NULL)
	     TtaFreeMemory (tmpbuffer);
	 }
     }

   if (!ok)
     {
       sprintf (msgBuffer,
		 "xml-stylesheet : attribute \"type\" not defined or not supported");
       XmlParseError (errorParsing, msgBuffer, 0);
     }

   if (ok)
     {
       /* get the "media" attribute */
       end = NULL;
       css_media = CSS_ALL;
       strcpy (buffer, PiData);
       ptr = strstr (buffer, "media");
       if (ptr)
	 {
	   ptr = strstr (ptr, "=");
	   ptr++;
	   while (ptr[0] != EOS && ptr[0] == ' ')
	     ptr++;
	   if (ptr[0] != EOS)
	     {
	       delimitor = ptr[0];
	       end = strchr (&ptr[1], delimitor);
	       tmpbuffer = TtaGetMemory (length + 1);
	       if (end && end[0] != EOS && tmpbuffer != NULL)
		 {
		   end[0] = EOS;
		   strcpy (tmpbuffer, &ptr[1]);
		   if (!strcasecmp (tmpbuffer, "screen"))
		     css_media = CSS_SCREEN;
		   else if (!strcasecmp (tmpbuffer, "print"))
		     css_media = CSS_PRINT;
		   else if (!strcasecmp (tmpbuffer, "all"))
		     css_media = CSS_ALL;
		   else
		     css_media = CSS_OTHER;
		 }
	       if (tmpbuffer != NULL)
		 TtaFreeMemory (tmpbuffer);
	     }
	 }
     }
   
   if (ok)
     {
       /* get the "href" attribute */
       end = NULL;
       strcpy (buffer, PiData);
       ptr = strstr (buffer, "href");
       if (ptr)
	 {
	   ptr = strstr (ptr, "=");
	   ptr++;
	   while (ptr[0] != EOS && ptr[0] == ' ')
	     ptr++;
	   if (ptr[0] != EOS)
	     {
	       delimitor = ptr[0];
	       css_href = TtaGetMemory (length + 1);
	       end = strchr (&ptr[1], delimitor);
	       if (end && end[0] != EOS && css_href != NULL)
		 {
		   end[0] = EOS;
		   strcpy (css_href, &ptr[1]);
		   css_info = NULL;
		   LoadStyleSheet (css_href, XMLcontext.doc, NULL,
				   css_info, css_media);
		   if (css_href != NULL)
		     TtaFreeMemory (css_href);
		 }
	     }
	 }
     }

   TtaFreeMemory (buffer);
}

/*----------------------------------------------------------------------
   CreateXmlPi
   Create a Processing Instruction element into the Thot tree.
  ---------------------------------------------------------------------*/
static void       CreateXmlPi (char *PiTarget, char *PiData)
{
   ElementType    elType, elTypeLeaf;
   Element  	  PiEl, PiLineEl, PiLeaf, lastChild;
   Language       lang;
   char          *mappedName;
   char          *PiValue = NULL;
   char           cont;
   char           fallback[5];
   char           msgBuffer[MaxMsgLength];
   unsigned char *buffer;
   unsigned char *srcbuf;
   wchar_t        wcharRead;
   int            length, i, j,error;
   int            nbBytesRead = 0;
   ThotBool       level = TRUE;

   length = strlen (PiTarget) + strlen (PiData);
   length++;
   buffer = TtaGetMemory (length + 1);
   i = 0; j = 0;
   buffer[j] = EOS;
   PiValue = TtaGetMemory (length + 1);
   strcpy (PiValue, PiTarget);
   strcat (PiValue, " ");
   strcat (PiValue, PiData);
 
   /* Create a Thot element for the PI */
   elType.ElSSchema = NULL;
   elType.ElTypeNum = 0;

   GetXmlElType (NULL, "XMLPI", &elType, &mappedName, &cont, &level);
   if (elType.ElTypeNum > 0)
     {
       PiEl = TtaNewElement (XMLcontext.doc, elType);
       XmlSetElemLineNumber (PiEl);
       InsertXmlElement (&PiEl);
       /* Create a XMLPI_line element as the first child of element XMLPI */
       elType.ElSSchema = NULL;
       elType.ElTypeNum = 0;
       GetXmlElType (NULL, "XMLPI_line", &elType, &mappedName, &cont, &level);
       PiLineEl = TtaNewElement (XMLcontext.doc, elType);
       XmlSetElemLineNumber (PiLineEl);
       TtaInsertFirstChild (&PiLineEl, PiEl, XMLcontext.doc);

       while (i < length && PiValue[i] != EOS)
	 {
	   srcbuf = (unsigned char *) &PiValue[i];
	   nbBytesRead = TtaGetNextWideCharFromMultibyteString (&wcharRead,
								&srcbuf, UTF_8);
	   i += nbBytesRead;
	   
	   if (wcharRead < 0x100)
	     {
	       /* Look for line break in the PI and create as many */
	       /* XMLPI_line elements as needed */
	       if ((int)wcharRead == EOL || (int)wcharRead == __CR__)
		 /* New line */
		 {
		   /* Put the current content into a text PI line */
		   buffer[j] = EOS;
		   elTypeLeaf.ElSSchema = elType.ElSSchema;
		   elTypeLeaf.ElTypeNum = 1;
		   PiLeaf = TtaNewElement (XMLcontext.doc, elTypeLeaf);
		   XmlSetElemLineNumber (PiLeaf);
		   if ((lastChild = TtaGetLastChild (PiLineEl)) == NULL)
		     TtaInsertFirstChild (&PiLeaf, PiLineEl, XMLcontext.doc);
		   else
		     TtaInsertSibling (PiLeaf, lastChild, FALSE, XMLcontext.doc);
		   TtaSetTextContent (PiLeaf, &buffer[0],
				      XMLcontext.language, XMLcontext.doc);
		   j = 0;
		   buffer[j] = EOS;
		   /* Create a new XMLPI_line element */
		   PiLineEl = TtaNewElement (XMLcontext.doc, elType);
		   XmlSetElemLineNumber (PiLineEl);
		   /* Inserts the new XMLPI_line after the previous one */
		   TtaInsertSibling (PiLineEl, TtaGetParent (PiLeaf),
				     FALSE, XMLcontext.doc);
		 }
	       else
		   buffer[j++] = (char) wcharRead;
	     }
	   else
	     {
	       /* It's not an 8bits character */
	       if (buffer[0] != EOS)
		 {
		   /* Put the current content into a text PI line */
		   buffer[j] = EOS;
		   /* Create a text element as child of element XMLPI_line */
		   elTypeLeaf.ElSSchema = elType.ElSSchema;
		   elTypeLeaf.ElTypeNum = 1;
		   PiLeaf = TtaNewElement (XMLcontext.doc, elTypeLeaf);
		   XmlSetElemLineNumber (PiLeaf);
		   if ((lastChild = TtaGetLastChild (PiLineEl)) == NULL)
		     TtaInsertFirstChild (&PiLeaf, PiLineEl, XMLcontext.doc);
		   else
		     TtaInsertSibling (PiLeaf, lastChild, FALSE, XMLcontext.doc);
		   TtaSetTextContent (PiLeaf, &buffer[0],
				      XMLcontext.language, XMLcontext.doc);
		   j = 0;
		   buffer[j] = EOS;
		 }
	       /* Try to find a fallback character */
	       GetFallbackCharacter ((int) wcharRead, fallback, &lang);
	       if (fallback[0] == '?')
		 {
		   /* The character is not found in the fallback table */
		   /* Create a symbol leaf */
		   elTypeLeaf.ElSSchema = elType.ElSSchema;
		   elTypeLeaf.ElTypeNum = 3;
		   PiLeaf = TtaNewElement (XMLcontext.doc, elTypeLeaf);
		   XmlSetElemLineNumber (PiLeaf);
		   if ((lastChild = TtaGetLastChild (PiLineEl)) == NULL)
		     TtaInsertFirstChild (&PiLeaf, PiLineEl, XMLcontext.doc);
		   else
		     TtaInsertSibling (PiLeaf, lastChild, FALSE, XMLcontext.doc);
		   /* Put the symbol '?' into the new symbol leaf */
		   TtaSetGraphicsShape (PiLeaf, fallback[0], XMLcontext.doc);
		   /* Changes the wide char code associated with that symbol */
		   TtaSetSymbolCode (PiLeaf, wcharRead, XMLcontext.doc);
		   /* Make that leaf read-only */
		   TtaSetAccessRight (PiLeaf, ReadOnly, XMLcontext.doc);
		 }
	       else
		 {
		   /* The character is found in the fallback table */
		   /* Create a new text leaf */
		   elTypeLeaf.ElSSchema = elType.ElSSchema;
		   elTypeLeaf.ElTypeNum = 1;
		   PiLeaf = TtaNewElement (XMLcontext.doc, elTypeLeaf);
		   XmlSetElemLineNumber (PiLeaf);
		   if ((lastChild = TtaGetLastChild (PiLineEl)) == NULL)
		     TtaInsertFirstChild (&PiLeaf, PiLineEl, XMLcontext.doc);
		   else
		     TtaInsertSibling (PiLeaf, lastChild, FALSE, XMLcontext.doc);
		   /* Put the fallback character into the new text leaf */
		   TtaSetTextContent (PiLeaf, fallback, lang, XMLcontext.doc);
		 }
	     }
	 }

       /* Process last line */
       if (buffer[0] != EOS)
	 {
	   buffer[j] = EOS;
	   elTypeLeaf.ElSSchema = elType.ElSSchema;
	   elTypeLeaf.ElTypeNum = 1;
	   PiLeaf = TtaNewElement (XMLcontext.doc, elTypeLeaf);
	   XmlSetElemLineNumber (PiLeaf);
	   if ((lastChild = TtaGetLastChild (PiLineEl)) == NULL)
	     TtaInsertFirstChild (&PiLeaf, PiLineEl, XMLcontext.doc);
	   else
	     TtaInsertSibling (PiLeaf, lastChild, FALSE, XMLcontext.doc);
	   TtaSetTextContent (PiLeaf, &buffer[0], XMLcontext.language,
			      XMLcontext.doc);
	 }
       
       (*(currentParserCtxt->ElementComplete)) (PiEl, XMLcontext.doc, &error);
       XMLcontext.lastElementClosed = TRUE;
       TtaFreeMemory (PiValue);
       TtaFreeMemory (buffer);
     }
   
   /* Call the treatment that correspond to that PI */
   /* Actually, Amaya supports only the "xml-stylesheet" PI */
   if (!strcmp (PiTarget, "xml-stylesheet"))
     XmlStyleSheetPi (PiData);
   else
     {
       sprintf (msgBuffer,
		 "Processing Instruction not supported : %s", PiTarget);
       XmlParseError (errorParsing, msgBuffer, 0);
     }
}
/*--------------------  PI  (end)  ---------------------------------*/


/*-----------  EXPAT handlers associated with Amaya  ---------------*/

/*--------------------------------------------------------------------
   Hndl_CdataStart
   Handlers that get called at the beginning of a CDATA section
  ------------------------------------------------------------------*/
static void     Hndl_CdataStart (void *userData)

{
#ifdef EXPAT_PARSER_DEBUG
  printf ("\n Hndl_CdataStart");
#endif /* EXPAT_PARSER_DEBUG */

  ParsingCDATA = TRUE;
}

/*----------------------------------------------------------------------
   Hndl_CdataEnd
   Handlers that get called at the end of a CDATA section
  ----------------------------------------------------------------------*/
static void     Hndl_CdataEnd (void *userData)

{
#ifdef EXPAT_PARSER_DEBUG
  printf ("\n Hndl_CdataEnd");
#endif /* EXPAT_PARSER_DEBUG */

  ParsingCDATA = FALSE;
}

/*----------------------------------------------------------------------
   Hndl_CharacterData
   Handler for the text
   The string the handler receives is NOT zero terminated.
   We have to use the length argument to deal with the end of the string.
  ----------------------------------------------------------------------*/
static void       Hndl_CharacterData (void *userData,
				      const XML_Char *data,
				      int   length)

{
   unsigned char *buffer;
   unsigned char *srcbuf;
   wchar_t        wcharRead;
   char           charRead;
   int            nbBytesRead = 0;
   int            i, j;

#ifdef EXPAT_PARSER_DEBUG
   printf ("\n Hndl_CharacterData - length = %d - ", length);
#endif /* EXPAT_PARSER_DEBUG */

   buffer = TtaGetMemory (length + 1);
   j = 0;
   i = 0;
   buffer[j] = EOS;

   while (i < length)
     {
       srcbuf = (unsigned char *) &data[i];
       nbBytesRead = TtaGetNextWideCharFromMultibyteString (&wcharRead,
							    &srcbuf, UTF_8);
       i += nbBytesRead;

       if (wcharRead < 0x100)
	 {
	   /* It's an 8bits character */
	   charRead = (char) wcharRead;
	   buffer[j++] = charRead;
	 }
       else
	 {
	   /* It's not an 8bits character */
	   if (buffer[0] != EOS)
	     {
	       /* Put the current content of the buffer into the document */
	       buffer[j] = EOS;
	       PutInXmlElement (buffer);
	       j = 0;
	       buffer[j] = EOS;
	     }
	   /* Try to find a fallback character */
	   XmlGetFallbackCharacter (wcharRead);
	   ImmediatelyAfterTag = FALSE;
	 }
     }

   if (buffer[0] != EOS)
     {
       buffer[j] = EOS;
       PutInXmlElement (buffer);
     }

   TtaFreeMemory (buffer);
}

/*----------------------------------------------------------------------
   Hndl_Comment
   Handler for comments
   The data is all text inside the comment delimiters
  ----------------------------------------------------------------------*/
static void     Hndl_Comment (void *userData, const XML_Char *data)

{
#ifdef EXPAT_PARSER_DEBUG
   printf ("\n Hndl_Comment %s", data);
#endif /* EXPAT_PARSER_DEBUG */

  if (!IgnoreCommentAndPi) 
    CreateXmlComment ((char*) data);
}

#ifdef IV
/*----------------------------------------------------------------------
   Hndl_Default
   Handler for any characters in the document which wouldn't
   otherwise be handled.
   This includes both data for which no handlers can be set
   (like some kinds of DTD declarations) and data which could be
   reported but which currently has no handler set.
  ----------------------------------------------------------------------*/
static void     Hndl_Default (void *userData,
			      const XML_Char *data,
			      int   length)

{
#ifdef EXPAT_PARSER_DEBUG
  int  i;
  printf ("\n Hndl_Default - length = %d - ", length);
  for (i=0; i<length; i++)
      printf ("%c", data[i]);
#endif /* EXPAT_PARSER_DEBUG */

  /* Specific treatment for the entities */
  if (length > 1 && data[0] == '&')
    CreateXmlEntity ((char*) data, length);
}
#endif /* IV */

/*----------------------------------------------------------------------
   Hndl_DefaultExpand
   Default handler with expansion of internal entity references
  ----------------------------------------------------------------------*/
static void     Hndl_DefaultExpand (void *userData,
				    const XML_Char *data,
				    int   length)

{
#ifdef EXPAT_PARSER_DEBUG
   int i;
   printf ("\n Hndl_DefaultExpand - length = %d - ", length);
   for (i=0; i<length; i++)
       printf ("%c", data[i]);
#endif /* EXPAT_PARSER_DEBUG */
   
   /* Specific treatment for the entities */
   if (length > 1 && data[0] == '&')
     CreateXmlEntity ((char*) data, length);
}

/*----------------------------------------------------------------------
   Hndl_DoctypeStart
   Handler for the start of the DOCTYPE declaration.
   It is called when the name of the DOCTYPE is encountered.
  ----------------------------------------------------------------------*/
static void     Hndl_DoctypeStart (void *userData,
				   const XML_Char *doctypeName)

{
#ifdef EXPAT_PARSER_DEBUG
   printf ("\n Hndl_DoctypeStart %s", doctypeName);
#endif /* EXPAT_PARSER_DEBUG */
}

/*----------------------------------------------------------------------
   Hndl_DoctypeEnd
   Handler for the start of the DOCTYPE declaration.
   It is called when the closing > is encountered,
   but after processing any external subset.
  ----------------------------------------------------------------------*/
static void     Hndl_DoctypeEnd (void *userData)

{
#ifdef EXPAT_PARSER_DEBUG
   printf ("\n Hndl_DoctypeEnd");
#endif /* EXPAT_PARSER_DEBUG */
}

/*----------------------------------------------------------------------
   Hndl_ElementStart
   Handler for start tags
   Attributes are passed as a pointer to a vector of char pointers
  ----------------------------------------------------------------------*/
static void       Hndl_ElementStart (void *userData,
				     const XML_Char *name,
				     const XML_Char **attlist)

{
   char          *buffer = NULL;
   unsigned char *attrName = NULL;
   unsigned char *attrValue = NULL;
   char          *elementName = NULL;
   char          *uriName = NULL;
   char          *ptr = NULL;
   PtrParserCtxt  elementParserCtxt = NULL;
   char           msgBuffer[MaxMsgLength];
   
#ifdef EXPAT_PARSER_DEBUG
   printf ("\n Hndl_ElementStart '%s'\n", name);
#endif /* EXPAT_PARSER_DEBUG */
   
   /* Treatment for the GI */
   if (XMLcontext.parsingTextArea)
     {
       /* We are parsing the contents of a TEXTAREA element */
       /* If a start tag appears, consider it as plain text */
     }
   else
     {
       /* Look for the context associated with that element */
       buffer = TtaGetMemory ((strlen (name) + 1));
       strcpy (buffer, (char*) name);
       if ((ptr = strrchr (buffer, NS_SEP)) != NULL)
	 {
	   *ptr = EOS;
	   ptr++;
	   elementName = TtaGetMemory ((strlen (ptr) + 1));
	   strcpy (elementName, ptr);
	   uriName = TtaGetMemory ((strlen (buffer) + 1));
	   strcpy (uriName, buffer);
	   
	   if ((currentParserCtxt != NULL &&
		strcmp (uriName, currentParserCtxt->UriName)) ||
	       (currentParserCtxt == NULL))
	     ChangeXmlParserContextUri (uriName);
	 }
       else
	 {
	   elementName = TtaGetMemory (strlen (buffer) + 1);
	   strcpy (elementName, buffer);
	 }
       
#ifdef XML_GENERIC
       /* We assign the generic XML context by default */ 
       if (currentParserCtxt == NULL)
	 currentParserCtxt = XmlGenericParserCtxt;
#else /* XML_GENERIC */
       /* We stop parsing if the context is null, ie,
	  if Thot doesn't know the corresponding namespace */ 
       if (currentParserCtxt == NULL)
	 {
	   sprintf (msgBuffer, 
		    "Namespace not supported for the element :\"%s\"", name);
	   XmlParseError (errorNotWellFormed, msgBuffer, 0);
	   DisableExpatParser ();
	   return;
	 }
#endif /* XML_GENERIC */
       
       /* We save the element current context */
       elementParserCtxt = currentParserCtxt;
       
       /* Ignore the virtual root of a XML sub-tree when */
       /* we are parsing the result of a transformation */
       if (strcmp (elementName, SUBTREE_ROOT) != 0)
	 {
	   /* Treatment called at the beginning of start tag */
	   StartOfXmlStartElement (uriName, elementName);
	   
	   /*-------  Treatment of the attributes -------*/
	   while (*attlist != NULL)
	     {
	       /* Create the corresponding Thot attribute */
	       attrName = TtaGetMemory ((strlen (*attlist)) + 1);
	       strcpy (attrName, *attlist);
#ifdef EXPAT_PARSER_DEBUG
	       printf ("  attr %s :", attrName);
#endif /* EXPAT_PARSER_DEBUG */
	       EndOfAttributeName (attrName);
	       
	       /* Restore the element context */
	       /* It occurs if the attribute name is unknown */
	       if (currentParserCtxt == NULL)
		 currentParserCtxt = elementParserCtxt;
	       
	       /* Filling of the attribute value */
	       attlist++;
	       if (*attlist != NULL)
		 {
		   attrValue = TtaGetMemory ((strlen (*attlist)) + 1);
		   strcpy (attrValue, *attlist);
#ifdef EXPAT_PARSER_DEBUG
		   printf (" value=%s \n", attrValue);
#endif /* EXPAT_PARSER_DEBUG */
		   EndOfAttributeValue (attrValue, attrName);
		 }
	       attlist++;
	       if (attrName != NULL)
		 TtaFreeMemory (attrName);
	       if (attrValue != NULL)
		 TtaFreeMemory (attrValue);
	       
	       /* Restore the context (it may have been changed */
	       /* by the treatment of the attribute) */
	       currentParserCtxt = elementParserCtxt;
	     }

	   /*----- Treatment called at the end of start tag -----*/
	   EndOfXmlStartElement (elementName);
	   
	   /*----- We are immediately after a start tag -----*/
	   ImmediatelyAfterTag = TRUE;
	   
	   /* Initialize the root element */
	   if (XMLRootName[0] == EOS)
	     /* This is the first parsed element */
	     strcpy (XMLRootName, (char*) name);
	 }
       
       if (uriName != NULL)
	 TtaFreeMemory (uriName);
       TtaFreeMemory (elementName);
       TtaFreeMemory (buffer);
     }
}

/*----------------------------------------------------------------------
   Hndl_ElementEnd
   Handler for end tags
  ----------------------------------------------------------------------*/
static void     Hndl_ElementEnd (void *userData,
				 const XML_Char *name)

{
   char       *uriName = NULL;
   char       *elementName;
   char       *buffer;
   char       *ptr;
   char        msgBuffer[MaxMsgLength];

#ifdef EXPAT_PARSER_DEBUG
   printf ("\n Hndl_ElementEnd '%s'", name);
#endif /* EXPAT_PARSER_DEBUG */

   ImmediatelyAfterTag = FALSE;

   /* Look for the context associated with that element */
   buffer = TtaGetMemory ((strlen (name) + 1));
   strcpy (buffer, (char*) name);
   if ((ptr = strrchr (buffer, NS_SEP)) != NULL)
     {
       *ptr = EOS;
       ptr++;
       elementName = TtaGetMemory ((strlen (ptr) + 1));
       strcpy (elementName, ptr);
       uriName = TtaGetMemory ((strlen (buffer) + 1));
       strcpy (uriName, buffer);

       if ((currentParserCtxt != NULL &&
	    strcmp (buffer, currentParserCtxt->UriName)) ||
	   (currentParserCtxt == NULL))
	 ChangeXmlParserContextUri (buffer);
     }
   else
     {
       elementName = TtaGetMemory (strlen (buffer) + 1);
       strcpy (elementName, buffer);
     }

#ifdef XML_GENERIC
   /* We assign the generic XML context by default */ 
   if (currentParserCtxt == NULL)
     currentParserCtxt = XmlGenericParserCtxt;
#else /* XML_GENERIC */
   /* We stop parsing if the context is null, ie,
      if Thot doesn't know the corresponding namespace */ 
   if (currentParserCtxt == NULL)
     {
       sprintf (msgBuffer, 
		"Namespace not supported for the element :\"%s\"", name);
       XmlParseError (errorNotWellFormed, msgBuffer, 0);
       DisableExpatParser ();
       return;
     }
#endif /* XML_GENERIC */
   
   /* Ignore the virtual root of a XML sub-tree */
   if (strcmp (elementName, SUBTREE_ROOT) != 0)
     EndOfXmlElement (uriName, elementName);

   /* Is it the end tag of the root element ? */
   if (!strcmp (XMLRootName, (char*) name) && stackLevel == 1)
     XMLrootClosed = TRUE;

   if (uriName != NULL)
     TtaFreeMemory (uriName);   
   TtaFreeMemory (elementName);   
   TtaFreeMemory (buffer);   
}

/*----------------------------------------------------------------------
   Hndl_ExternalEntityRef
   Handler for external entity references.
   his handler is also called for processing an external DTD subset.
  ----------------------------------------------------------------------*/
static int     Hndl_ExternalEntityRef (void *userData,
				       const XML_Char *context,
				       const XML_Char *base,
				       const XML_Char *systemId,
				       const XML_Char *publicId)

{
#ifdef EXPAT_PARSER_DEBUG
  printf ("\n Hndl_ExternalEntityRef");
  printf ("\n   context  : %s", context);
  printf ("\n   base     : %s", base);
  printf ("\n   systemId : %s", systemId);
  printf ("\n   publicId : %s", publicId);
#endif /* EXPAT_PARSER_DEBUG */
  return 1;
}

/*----------------------------------------------------------------------
   Hndl_NameSpaceStart
   Handler for the start of namespace declarations
  ----------------------------------------------------------------------*/
static void     Hndl_NameSpaceStart (void *userData,
				     const XML_Char *prefix,
				     const XML_Char *uri)

{
#ifdef EXPAT_PARSER_DEBUG
  printf ("\n Hndl_NameSpaceStart");
  printf ("\n   prefix : %s; uri : %s", prefix, uri);
#endif /* EXPAT_PARSER_DEBUG */
}

/*----------------------------------------------------------------------
   Hndl_NameSpaceEnd
   Handler for the end of namespace declarations
  ----------------------------------------------------------------------*/
static void     Hndl_NameSpaceEnd (void *userData,
				   const XML_Char *prefix)

{
#ifdef EXPAT_PARSER_DEBUG
  printf ("\n Hndl_NameSpaceEnd");
  printf ("\n   prefix : %s", prefix);
#endif /* EXPAT_PARSER_DEBUG */
}

/*----------------------------------------------------------------------
   Hndl_Notation
   Handler that receives notation declarations.
  ----------------------------------------------------------------------*/
static void     Hndl_Notation (void *userData,
			       const XML_Char *notationName,
			       const XML_Char *base,
			       const XML_Char *systemId,
			       const XML_Char *publicId)

{
#ifdef EXPAT_PARSER_DEBUG
  printf ("\n Hndl_Notation");
  printf ("\n   notationName : %s", notationName);
  printf ("\n   base         : %s", base);
  printf ("\n   systemId     : %s", systemId);
  printf ("\n   publicId     : %s", publicId);
#endif /* EXPAT_PARSER_DEBUG */
}

/*----------------------------------------------------------------------
   Hndl_NotStandalone
   Handler that is called if the document is not "standalone".
   This happens when there is an external subset or a reference
   to a parameter entity, but does not have standalone set to "yes" 
   in an XML declaration.
   If this handler returns 0, then the parser will throw an
   XML_ERROR_NOT_STANDALONE error.
  ----------------------------------------------------------------------*/
static int     Hndl_NotStandalone (void *userData)

{
#ifdef EXPAT_PARSER_DEBUG
  printf ("\n Hndl_NotStandalone");
#endif /* EXPAT_PARSER_DEBUG */
  return 1;
}

/*----------------------------------------------------------------------
   Hndl_PI
   Handler for processing instructions.
   The target is the first word in the processing instruction.
   The pidata is the rest of the characters in it after skipping
   all whitespace after the initial word.
  ----------------------------------------------------------------------*/
static void     Hndl_PI (void *userData,
			 const XML_Char *target,
			 const XML_Char *pidata)

{
#ifdef EXPAT_PARSER_DEBUG
  printf ("\n Hndl_PI");
  printf ("\n   target : %s", target);
  printf ("\n   pidata : %s", pidata);
#endif /* EXPAT_PARSER_DEBUG */
  if (!IgnoreCommentAndPi) 
    CreateXmlPi ((char*) target, (char*) pidata);
}

/*----------------------------------------------------------------------
   Hndl_UnknownEncoding
   Handler to deal with encodings other than the built in
  ----------------------------------------------------------------------*/
static int     Hndl_UnknownEncoding (void           *encodingData,
				     const XML_Char *name,
				     XML_Encoding   *info)

{
#ifdef EXPAT_PARSER_DEBUG
  printf ("\n Hndl_UnknownEncoding");
  printf ("\n   name : %s", name);
#endif /* EXPAT_PARSER_DEBUG */
  XMLUnknownEncoding = TRUE;
  XmlParseError (errorEncoding,
		 TtaGetMessage (AMAYA, AM_UNKNOWN_ENCODING), -1);
  return 1;
}

/*----------------------------------------------------------------------
   Hndl_UnparsedEntity
   Handler that receives declarations of unparsed entities.
   These are entity declarations that have a notation (NDATA) field:
  ----------------------------------------------------------------------*/
static void     Hndl_UnparsedEntity (void *userData,
				     const XML_Char *entityName,
				     const XML_Char *base,
				     const XML_Char *systemId,
				     const XML_Char *publicId,
				     const XML_Char *notationName)

{
#ifdef EXPAT_PARSER_DEBUG
  printf ("\n Hndl_UnparsedEntity");
  printf ("\n   entityName   : %s", entityName);
  printf ("\n   base         : %s", base);
  printf ("\n   systemId     : %s", systemId);
  printf ("\n   publicId     : %s", publicId);
  printf ("\n   notationName : %s", notationName);
#endif /* EXPAT_PARSER_DEBUG */
}

/*---------------- End of Handler definition ----------------*/

/*----------------------------------------------------------------------
   FreeXmlParser
   Frees all ressources associated with the XML parser.
  ----------------------------------------------------------------------*/
void             FreeXmlParserContexts (void)

{
  PtrParserCtxt  ctxt, nextCtxt;

   /* free parser contexts */
   ctxt = firstParserCtxt;
   while (ctxt != NULL)
      {
	nextCtxt = ctxt->NextParserCtxt;
	TtaFreeMemory (ctxt->SSchemaName);
	TtaFreeMemory (ctxt->UriName);
	TtaFreeMemory (ctxt);
	ctxt = nextCtxt;
      }
   firstParserCtxt = NULL;
   currentParserCtxt = NULL;
}

/*----------------------------------------------------------------------
   DisableExpatParser
   Disable all handlers
  ----------------------------------------------------------------------*/
static void    DisableExpatParser ()

{    
  int    paramEntityParsing;

  paramEntityParsing = XML_PARAM_ENTITY_PARSING_NEVER;
  XML_SetCdataSectionHandler (Parser, NULL, NULL);
  XML_SetCharacterDataHandler (Parser, NULL);
  XML_SetCommentHandler (Parser, NULL);
  XML_SetDefaultHandlerExpand (Parser, NULL);
  XML_SetDoctypeDeclHandler (Parser, NULL, NULL);
  XML_SetElementHandler (Parser, NULL, NULL);
  XML_SetExternalEntityRefHandler (Parser, NULL);
  XML_SetNamespaceDeclHandler (Parser, NULL, NULL);
  XML_SetNotationDeclHandler (Parser, NULL);
  XML_SetNotStandaloneHandler (Parser, NULL);
  XML_SetParamEntityParsing (Parser, paramEntityParsing);
  XML_SetProcessingInstructionHandler (Parser, NULL);
  XML_SetUnknownEncodingHandler (Parser, NULL, 0);
  XML_SetUnparsedEntityDeclHandler (Parser, NULL);
}
/*----------------------------------------------------------------------
   FreeExpatParser
  ----------------------------------------------------------------------*/
static void     FreeExpatParser ()

{  
  DisableExpatParser ();
  XML_ParserFree (Parser);
  Parser = NULL;
}


/*----------------------------------------------------------------------
   InitializeExpatParser
   Specific initialization for expat
  ----------------------------------------------------------------------*/
static void  InitializeExpatParser (CHARSET charset)

{  
  int        paramEntityParsing;
  char       msgBuffer[MaxMsgLength];

  /* Enable parsing of parameter entities */
  paramEntityParsing = XML_PARAM_ENTITY_PARSING_UNLESS_STANDALONE;

  /* Disable "Read as Iso-Latin1" entry */
  TtaSetItemOff (XMLcontext.doc, 1, File, BLatinReading);

  /* Construct a new parser with namespace processing */
  /* accordingly to the document encoding */
  /* If that encoding is unknown, we don''t parse the document */
  if (charset == UNDEFINED_CHARSET)
    {
      /* Defalut encoding for XML documents */
      Parser = XML_ParserCreateNS ("UTF-8", NS_SEP);
      /* Display a warning message */
      sprintf (msgBuffer,
		"No encoding specified, assuming UTF-8");
      XmlParseError (undefinedEncoding, msgBuffer, 0);
      /* Enable "Read as Iso-Latin1 entry" */
      TtaSetItemOn (XMLcontext.doc, 1, File, BLatinReading);
    }
  else if (charset == UTF_8 || charset == UTF_16)
    /* These encoding are automatically recognized by Expat */
    Parser = XML_ParserCreateNS (NULL, NS_SEP);
  else if (charset == US_ASCII)
    /* US-ASCII may has been set for us-ascii or ascii */
    Parser = XML_ParserCreateNS ("US-ASCII", NS_SEP);
  else if (charset == ISO_8859_1)
    /* ISO_8859_1 may has been set for a HTML document with no encoding */
    /* In this case, the default encoding is ISO_8859_1, not UTF-8 */
    Parser = XML_ParserCreateNS ("ISO-8859-1", NS_SEP);
  else if (charset == ISO_8859_2   || charset == ISO_8859_3   ||
	   charset == ISO_8859_4   || charset == ISO_8859_5   ||
	   charset == ISO_8859_6   || charset == ISO_8859_6_E ||
	   charset == ISO_8859_6_I || charset == ISO_8859_7   ||
	   charset == ISO_8859_8   || charset == ISO_8859_8_E ||
	   charset == ISO_8859_8_I || charset == ISO_8859_9   ||
	   charset == ISO_8859_10  || charset == ISO_8859_15  ||
	   charset == ISO_8859_supp)
    Parser = XML_ParserCreateNS ("ISO-8859-1", NS_SEP);
  /* Consider WINDOWS_1252 (Windows Latin 1) as ISO_8859_1 */
  else if (charset == WINDOWS_1252)
    Parser = XML_ParserCreateNS ("ISO-8859-1", NS_SEP);
  else
    {
      XMLUnknownEncoding = TRUE;
      XmlParseError (errorEncoding,
		     TtaGetMessage (AMAYA, AM_UNKNOWN_ENCODING), -1);
      Parser = XML_ParserCreateNS ("UTF-8", NS_SEP);
      return;
    }

  /* Define the user data pointer that gets passed to handlers */
  /* (not use  Amaya actually) */
  /* XML_SetUserData (Parser, (void*) doc); */
  
  /* Set handlers that get called at the beginning 
     and end of a CDATA section */
  XML_SetCdataSectionHandler (Parser,
			      Hndl_CdataStart,
			      Hndl_CdataEnd);
    
  /* Set a text handler */
  XML_SetCharacterDataHandler (Parser,
			       Hndl_CharacterData);
  
  /* Set a handler for comments */
  XML_SetCommentHandler (Parser,
			 Hndl_Comment);
  
  /* Set default handler with no expansion of internal entity references */
  /*
  XML_SetDefaultHandler (Parser,
			 Hndl_Default);
  */
  
  /* Set a default handler with expansion of internal entity references */
  XML_SetDefaultHandlerExpand (Parser,
			       Hndl_DefaultExpand);

  /* Set a handler for DOCTYPE declaration */
  XML_SetDoctypeDeclHandler (Parser,
			     Hndl_DoctypeStart,
			     Hndl_DoctypeEnd); 

  /* Set handlers for start and end tags */
  XML_SetElementHandler (Parser,
			 Hndl_ElementStart,
			 Hndl_ElementEnd);
 
  /* Set an external entity reference handler */
  XML_SetExternalEntityRefHandler (Parser,
				   Hndl_ExternalEntityRef);
  
  /* Set handlers for namespace declarations */
  XML_SetNamespaceDeclHandler (Parser,
			       Hndl_NameSpaceStart,
			       Hndl_NameSpaceEnd);
  
  /* Set a handler for notation declarations */
  XML_SetNotationDeclHandler (Parser,
			      Hndl_Notation);
  
  /* Set a handler for no 'standalone' document */
  XML_SetNotStandaloneHandler (Parser,
			       Hndl_NotStandalone);

  /* Controls parsing of parameter entities */
  XML_SetParamEntityParsing (Parser,
			     paramEntityParsing);
  
  /* Set a handler for processing instructions */
  XML_SetProcessingInstructionHandler (Parser,
				       Hndl_PI);
  
  /* Set a handler to deal with encodings other than the built in */
  XML_SetUnknownEncodingHandler (Parser,
				 Hndl_UnknownEncoding, 0);
  
  /* Set a handler that receives declarations of unparsed entities */
  XML_SetUnparsedEntityDeclHandler (Parser,
				    Hndl_UnparsedEntity);
}

/*----------------------------------------------------------------------
   InitializeXmlParsingContext
   initializes variables and stack for parsing file.
  ----------------------------------------------------------------------*/
static void  InitializeXmlParsingContext (Document doc,
					  Element  lastElem,
					  ThotBool isClosed,
					  ThotBool isSubTree)

{
  XMLcontext.doc = doc;
  XMLcontext.lastElement = lastElem;
  XMLcontext.lastElementClosed = isClosed;
  ParsingSubTree = isSubTree;
  
  /* initialize global variables */
  XMLcontext.readingAnAttrValue = FALSE;
  XMLcontext.parsingTextArea = FALSE;
  XMLcontext.parsingCSS = FALSE;
  XMLcontext.mergeText = FALSE;
  XMLcontext.withinTable = 0;
  currentAttribute = NULL;
  lastAttrElement = NULL;
  lastMappedAttr = NULL;
  ImmediatelyAfterTag = FALSE;
  UnknownTag = FALSE;
  XMLRootName[0] = EOS;
  XMLrootClosed = FALSE;
  IgnoreCommentAndPi = FALSE;
  ParsingCDATA = FALSE;

  htmlLineRead = 0;
  htmlCharRead = 0;
  
  /* initialize the stack of opened elements */
  stackLevel = 1;
  elementStack[0] = RootElement;
}

/*----------------------------------------------------------------------
  ChangeSVGImageType
  ----------------------------------------------------------------------*/
Element         ChangeSvgImageType (Element el, Document doc)
{
  ElementType   elType;
  Element       svgImageElement, svgImageContent;
  Attribute     attr, nextattr;
 
  svgImageElement = NULL;
  svgImageContent = NULL;
  elType = TtaGetElementType (el);

  if ((strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0) &&
      elType.ElTypeNum == HTML_EL_PICTURE_UNIT)
    {
      /* create a SVG_Image element within a HTML element*/
      elType.ElTypeNum = HTML_EL_SVG_Image;
      svgImageElement = TtaNewElement (doc, elType);
      if (svgImageElement == NULL)
	return NULL;
      else
	TtaInsertSibling (svgImageElement, el, FALSE, doc);
      
      /* Attach the attributes to that new element */
      nextattr = NULL;
      TtaNextAttribute (el, &nextattr);
      while (nextattr != NULL)
	{
	  attr = nextattr;
	  TtaNextAttribute (el, &nextattr);
	  TtaAttachAttribute (svgImageElement, attr, doc);
	}
      
      /* create a SVG_ImageContent element */
      elType.ElTypeNum = HTML_EL_SVG_ImageContent;
      svgImageContent = TtaNewElement (doc, elType);
      if (svgImageContent != NULL)
	TtaInsertFirstChild (&svgImageContent, svgImageElement, doc);
      
      /* Remove the PICTURE_UNIT element form the tree */
      TtaRemoveTree (el, doc);
    }
  else if ((strcmp (TtaGetSSchemaName (elType.ElSSchema), "GraphML") == 0) &&
	   elType.ElTypeNum == GraphML_EL_PICTURE_UNIT)
    {
      /* create a SVG_Image element within a SVG element*/
      elType.ElTypeNum = GraphML_EL_SVG_Image;
      svgImageContent = TtaNewElement (doc, elType);
      if (svgImageContent == NULL)
	return NULL;
      else
	TtaInsertSibling (svgImageContent, el, FALSE, doc);
      
      /* Attach the attributes to that new element */
      nextattr = NULL;
      TtaNextAttribute (el, &nextattr);
      while (nextattr != NULL)
	{
	  attr = nextattr;
	  TtaNextAttribute (el, &nextattr);
	  TtaAttachAttribute (svgImageContent, attr, doc);
	}
            
      /* Remove the PICTURE_UNIT element form the tree */
      TtaRemoveTree (el, doc);
    }

  return svgImageContent;
}

/*----------------------------------------------------------------------
   ParseXmlSubTree
   Return TRUE if the parsing of the sub-tree has no error.
   Parse a XML sub-tree and complete the corresponding Thot abstract tree.
   Xml sub-tree is given in fileName or xmlBuffer, one parameter should be null
   Return TRUE if the parsing of the XML sub-tree doesn't detect errors.
  ----------------------------------------------------------------------*/
ThotBool       ParseXmlSubTree (char     *xmlBuffer,
				char     *fileName,
				Element   el,
				ThotBool  isclosed,
				Document  doc,
				Language  lang,
			        char     *DTDname)

{
  int          tmpLen = 0;
  char        *transBuffer = NULL;
  char        *schemaName;
  ElementType  elType;
  Element      parent;
  CHARSET      charset;
  Element      svgEl = NULL;
#define	 COPY_BUFFER_SIZE	1024
  gzFile       infile;
  char         bufferRead[COPY_BUFFER_SIZE];
  int          res;
  ThotBool     endOfFile = FALSE;
  
  if (fileName == NULL && xmlBuffer == NULL)
    return FALSE;

  if (fileName != NULL && xmlBuffer != NULL)
    return FALSE;

  TtaSetDisplayMode (doc, DeferredDisplay);

  /* Initialize all parser contexts */
  if (firstParserCtxt == NULL)
    InitXmlParserContexts ();
 
  /* general initialization */
  RootElement = NULL;
  if (fileName != NULL && DTDname!= NULL &&
      strcmp (DTDname, "SVG") == 0)
    /* We are parsing an external SVG image */
    {
      svgEl = ChangeSvgImageType (el, doc);
      if (svgEl == NULL)
	return FALSE;
      ChangeXmlParserContextDTD ("GraphML");
      InitializeXmlParsingContext (doc, svgEl, isclosed, TRUE);
      /* When we parse an external xml file, we don't consider comments and PI */
      IgnoreCommentAndPi = TRUE;
    }
  else
    {
      if (isclosed)
	{
	  parent = TtaGetParent (el);
	  elType = TtaGetElementType (parent);
	}
      else
	elType = TtaGetElementType (el);
      schemaName = TtaGetSSchemaName(elType.ElSSchema);
      ChangeXmlParserContextDTD (schemaName);
      InitializeXmlParsingContext (doc, el, isclosed, TRUE);
    }

  /* specific Initialization */
  XMLcontext.language = lang;
  DocumentSSchema = TtaGetDocumentSSchema (doc);

  /* Expat initialization */
  charset = TtaGetDocumentCharset (doc);
  /* For HTML documents, the default charset is ISO_8859_1 */
  if (charset == UNDEFINED_CHARSET && !DocumentMeta[doc]->xmlformat)
    charset = ISO_8859_1;
  InitializeExpatParser (charset);
 
  TtaSetStructureChecking (0, doc);

  if (xmlBuffer != NULL)
    {
      /* Parse virtual DOCTYPE */
      if (!XML_Parse (Parser, DECL_DOCTYPE, DECL_DOCTYPE_LEN, 0))
	XmlParseError (errorNotWellFormed,
		       (char *) XML_ErrorString (XML_GetErrorCode (Parser)), 0);
      
      /* Parse the input XML buffer and complete the Thot document */
      if (!XMLNotWellFormed)
	{
	  /* We create a virtual root for the sub-tree to be parsed */
	  tmpLen = (strlen (xmlBuffer)) + 1;
	  tmpLen = tmpLen + (2 * SUBTREE_ROOT_LEN) + 1;
	  transBuffer = TtaGetMemory (tmpLen);
	  strcpy (transBuffer, "<SUBTREE_ROOT>");
	  strcat (transBuffer, xmlBuffer);
	  strcat (transBuffer, "</SUBTREE_ROOT>");
	  tmpLen = strlen (transBuffer);
	  
	  if (!XML_Parse (Parser, transBuffer, tmpLen, 1))
	    {
	      if (!XMLrootClosed)
		XmlParseError (errorNotWellFormed,
			       (char *) XML_ErrorString (XML_GetErrorCode (Parser)), 0);
	    }
	  if (transBuffer != NULL)   
	    TtaFreeMemory (transBuffer);   
	}
    }
  else
    {
      /* Set document URL */
      tmpLen = strlen (fileName);
      docURL = TtaGetMemory (tmpLen + 1);
      strcpy (docURL, fileName);

      /* Reading of the file */
      infile = gzopen (fileName, "r");
      if (infile != 0)
	{
	  while (!endOfFile && !XMLNotWellFormed)
	    {
	      /* read the XML file */
	      res = gzread (infile, bufferRead, COPY_BUFFER_SIZE);      
	      if (res < COPY_BUFFER_SIZE)
		endOfFile = TRUE;
	      
	      if (!XML_Parse (Parser, bufferRead, res, endOfFile))
		XmlParseError (errorNotWellFormed,
			       (char *) XML_ErrorString (XML_GetErrorCode (Parser)), 0);
	    }
	}
    }
  
  TtaSetStructureChecking (1, doc);

  if (svgEl != NULL && !XMLNotWellFormed)
    /* TtaSetAccessRight (TtaGetParent (svgEl), ReadOnly, doc); */
    TtaSetAccessRight (svgEl, ReadOnly, doc);

  if (docURL != NULL)
    {
      TtaFreeMemory (docURL);
      docURL = NULL;
    }
  TtaSetDisplayMode (doc, DisplayImmediately);

  /* Free expat parser */ 
  FreeXmlParserContexts ();
  FreeExpatParser ();

  return (!XMLNotWellFormed);
}

/*----------------------------------------------------------------------
   ParseIncludedXml
   Parse a XML sub-tree included in a HTML document and complete the 
   corresponding Thot abstract tree.
   Xml sub-tree is given in infile or htmlBuffer, one parameter should 
   be null
   Return TRUE if the parsing of the sub-tree has no error.
  ----------------------------------------------------------------------*/
ThotBool ParseIncludedXml (FILE     *infile,
			   char     *infileBuffer,
			   int       infileBufferLength,
			   ThotBool *infileEnd,
			   ThotBool *infileNotToRead,
			   char     *infilePreviousBuffer,
			   int      *infileLastChar,
			   char     *htmlBuffer,
			   int      *index,
			   int      *nbLineRead,
			   int      *nbCharRead,
			   char     *DTDname,
			   Document  doc,
			   Element  *el,
			   ThotBool *isclosed,
			   Language  lang)
{
  ThotBool     endOfParsing = FALSE;
  ThotBool     found;
  int          res = 0;
  int          tmpLen = 0;
  int          offset = 0;
  int          i;
  ElementType  elType;
  char        *schemaName;
  char        *tmpBuffer = NULL;
  CHARSET      charset;

  if (infile == NULL && htmlBuffer == NULL)
    return TRUE;

  /* general initialization */
  /* If htmlBuffer isn't null, we are parging a html sub-tree */
  /* including XML elements */ 
  RootElement = NULL;
  if (htmlBuffer == NULL)
    InitializeXmlParsingContext (doc, *el, *isclosed, FALSE);
  else
    InitializeXmlParsingContext (doc, *el, *isclosed, TRUE);

  /* specific Initialization */
  XMLcontext.language = lang;
  DocumentSSchema = TtaGetDocumentSSchema (doc);

  /* Initialize  counters */
  extraLineRead = 0;
  extraOffset = 0;
  htmlLineRead = *nbLineRead;
  htmlCharRead = *nbCharRead;

  /* Expat initialization */
  charset = TtaGetDocumentCharset (doc);
  /* For HTML documents, the default charset is ISO_8859_1 */
  if (charset == UNDEFINED_CHARSET)
    charset = ISO_8859_1;
  InitializeExpatParser (charset);

  /* initialize all parser contexts */
  if (firstParserCtxt == NULL)
    InitXmlParserContexts ();
  if (DTDname != NULL)
    ChangeXmlParserContextDTD (DTDname);
  else
    {
      elType = TtaGetElementType (XMLcontext.lastElement);
      schemaName = TtaGetSSchemaName(elType.ElSSchema);
      ChangeXmlParserContextDTD (schemaName);
    }

  /* Parse virtual DOCTYPE */
  if (!XML_Parse (Parser, DECL_DOCTYPE, DECL_DOCTYPE_LEN, 0))
    XmlParseError (errorNotWellFormed,
		   (char *) XML_ErrorString (XML_GetErrorCode (Parser)), 0);
  else
    {
      extraLineRead = XML_GetCurrentLineNumber (Parser);
      extraOffset = XML_GetCurrentByteIndex (Parser);
    }

  /* Parse the input file or HTML buffer and complete the Thot document */

  /* If htmlBuffer isn't null, we are parsing a XML sub-tree */
  /* included in a transformation */
  if (htmlBuffer != NULL)
    {
      /* parse the HTML buffer */
      tmpLen = strlen (htmlBuffer) - *index;
      tmpBuffer = TtaGetMemory (tmpLen);
      for (i = 0; i < tmpLen; i++)
	tmpBuffer[i] = htmlBuffer[*index + i];	  
      if (!XML_Parse (Parser, tmpBuffer, tmpLen, 1))
	{
	  if (!XMLrootClosed)
	    XmlParseError (errorNotWellFormed,
			   (char *) XML_ErrorString (XML_GetErrorCode (Parser)), 0);
	}
    }
  else
    {
      /* read and parse the HTML file sequentialy */
      while (!endOfParsing && !XMLNotWellFormed)
	{
	  if (*index == 0)
	    {
	      if (*infileNotToRead)
		*infileNotToRead = FALSE;
	      else
		{
		  res = gzread (infile, infileBuffer, infileBufferLength);
		  if (res < infileBufferLength)
		      endOfParsing = TRUE;
		  if (res <= 0)
		    {
		      *infileEnd = TRUE;
		      *infileLastChar = 0;
		    }
		  else
		      *infileLastChar = res - 1;
		}
	    }

	  if (tmpBuffer != NULL)   
	    TtaFreeMemory (tmpBuffer);   
	  if (*infileNotToRead)
	    {
	      tmpLen = strlen (infilePreviousBuffer) - *index;
	      tmpBuffer = TtaGetMemory (tmpLen);
	      for (i = 0; i < tmpLen; i++)
		tmpBuffer[i] = infilePreviousBuffer[*index + i];	  
	    }
	  else
	    {
	      if (endOfParsing)
		tmpLen = res  - *index;
	      else
		tmpLen = strlen (infileBuffer) - *index;
	      tmpBuffer = TtaGetMemory (tmpLen);
	      for (i = 0; i < tmpLen; i++)
		tmpBuffer[i] = infileBuffer[*index + i];	  
	    }
	  if (!XML_Parse (Parser, tmpBuffer, tmpLen, endOfParsing))
	    {
	      if (XMLrootClosed)
		endOfParsing = TRUE;
	      else
		XmlParseError (errorNotWellFormed,
			       (char *) XML_ErrorString (XML_GetErrorCode (Parser)), 0);
	    }
	  else
	    {
	      *index = 0;
	      extraOffset =  extraOffset + tmpLen;
	    }
	}
    }

  /* return char/lines read */
  if (htmlBuffer == NULL)
    {
      if (XML_GetCurrentLineNumber (Parser) - extraLineRead <= 0)
	/* We stay on the same line */
	*nbCharRead += XML_GetCurrentColumnNumber (Parser);
      else
	{
	  /* We read at least one new line */
	  *nbLineRead = *nbLineRead + XML_GetCurrentLineNumber (Parser) - extraLineRead;
	  *nbCharRead = XML_GetCurrentColumnNumber (Parser);
	}
    }

  /* We look for the '>' character of the XML end tag */
  offset = XML_GetCurrentByteIndex (Parser) - extraOffset - 1;
  found = FALSE;
  i = offset;
  while (i >= 0 && !found)
    {
      if (tmpBuffer[i] == '>')
	found = TRUE;
      else
	i--;
    }
  if (found)
    {
      i++;
      *index += i;
    }
  else
    *index += offset;

  *el = XMLcontext.lastElement;
  *isclosed = XMLcontext.lastElementClosed;

  if (tmpBuffer != NULL)   
    TtaFreeMemory (tmpBuffer);   

  /* Free expat parser */ 
  FreeXmlParserContexts ();
  FreeExpatParser ();

  return (!XMLNotWellFormed);
}

/*---------------------------------------------------------------------------
   XmlParse
   Parses the XML file infile and builds the equivalent Thot abstract tree.
  ---------------------------------------------------------------------------*/
static void   XmlParse (FILE     *infile,
			ThotBool *xmlDec,
			ThotBool *xmlDoctype)

{
#define	 COPY_BUFFER_SIZE	1024
   char         bufferRead[COPY_BUFFER_SIZE];
   int          i;
   int          res;
   ThotBool     endOfFile = FALSE;
   int          tmpLineRead = 0;
  
   if (infile != NULL)
       endOfFile = FALSE;
   else
       return;

   /* Initialize local counters */
   extraLineRead = 0;
   extraOffset = 0;
   htmlLineRead = 0;
   htmlCharRead = 0;
     
   while (!endOfFile && !XMLNotWellFormed)
     {
       /* read the XML file */
       res = gzread (infile, bufferRead, COPY_BUFFER_SIZE);      
       if (res < COPY_BUFFER_SIZE)
	   endOfFile = TRUE;
       i = 0;

       if (!*xmlDoctype)
	 /* There is no DOCTYPE Declaration 
	    We include a virtual DOCTYPE declaration so that EXPAT parser
	    doesn't stop processing when it finds an external entity */	  
	 {
	   if (*xmlDec)
	     /* There is a XML declaration */
	     /* We look for the first '>' character */
	     {
	       while ((bufferRead[i] != '>') && i < res)
		 i++;
	       if (i < res)
		 {
		   i++;
		   if (!XML_Parse (Parser, bufferRead, i, FALSE))
		     XmlParseError (errorNotWellFormed,
				    (char *) XML_ErrorString (XML_GetErrorCode (Parser)), 0);
		   res = res - i;
		 }
	     }

	   /* Virtual DOCTYPE Declaration */
	   if (!XMLNotWellFormed)
	     {
	       tmpLineRead = XML_GetCurrentLineNumber (Parser);
	       if (!XML_Parse (Parser, DECL_DOCTYPE, DECL_DOCTYPE_LEN, 0))
		 XmlParseError (errorNotWellFormed,
				(char *) XML_ErrorString (XML_GetErrorCode (Parser)), 0);
	       *xmlDoctype = TRUE;
	       extraLineRead = XML_GetCurrentLineNumber (Parser) - tmpLineRead;
	     }
	 }

       /* Standard EXPAT processing */
       if (!XMLNotWellFormed)
	 {
	   if (!XML_Parse (Parser, &bufferRead[i], res, endOfFile))
	     XmlParseError (errorNotWellFormed,
			    (char *) XML_ErrorString (XML_GetErrorCode (Parser)), 0);
	 }
     }
}

/*----------------------------------------------------------------------
   StartXmlParser loads the file Directory/xmlFileName for
   displaying the document documentName.
   The parameter pathURL gives the original (local or
   distant) path or URL of the xml document.
  ----------------------------------------------------------------------*/
void StartXmlParser (Document doc,
		     char *fileName,
		     char *documentName,
		     char *documentDirectory,
		     char *pathURL,
		     ThotBool xmlDec,
		     ThotBool xmlDoctype)
{
  Element         el, oldel;
  char           *s;
  char            tempname[MAX_LENGTH];
  char            temppath[MAX_LENGTH];
  int             length, error;
  ThotBool        isXHTML;
  CHARSET         charset;

  /* General initialization */
  RootElement = TtaGetMainRoot (doc);
  InitializeXmlParsingContext (doc, RootElement, FALSE, FALSE);

  /* Specific Initialization */
  XMLcontext.language = TtaGetDefaultLanguage ();
  DocumentSSchema = TtaGetDocumentSSchema (doc);

  /* Reading of the file */
  stream = gzopen (fileName, "r");

  if (stream != 0)
    {
      if (documentName[0] == EOS &&
	  !TtaCheckDirectory (documentDirectory))
	{
	  strcpy (documentName, documentDirectory);
	  documentDirectory[0] = EOS;
	  s = TtaGetEnvString ("PWD");
	  /* Set path on current directory */
	  if (s != NULL)
	    strcpy (documentDirectory, s);
	  else
	    documentDirectory[0] = EOS;
	}
      TtaAppendDocumentPath (documentDirectory);

      /* Set document URL */
      length = strlen (pathURL);
      if (strcmp (pathURL, fileName) == 0)
	{
	  docURL = TtaGetMemory (length + 1);
	  strcpy (docURL, pathURL);
	}
      else
	{
	  length += strlen (fileName) + 20;
	  docURL = TtaGetMemory (length+1);
	  sprintf (docURL, "%s temp file: %s", pathURL, fileName);
	}

      /* Do not check the Thot abstract tree against the structure */
      /* schema while building the Thot document. */
      /* Some valid XHTML documents could be considered as invalid Thot documents */
      /* For example, a <tbody> as a child of a <table> would be considered */
      /* invalid because the Thot SSchema requires a Table_body element in between */
      TtaSetStructureChecking (0, doc);

      /* Set the notification mode for the new document */
      TtaSetNotificationMode (doc, 1);

      /* Is the current document a XHTML document */
      isXHTML = (strcmp (TtaGetSSchemaName (DocumentSSchema),
			  "HTML") == 0);
      LoadUserStyleSheet (doc);

      TtaSetDisplayMode (doc, NoComputedDisplay);

      /* Delete all element except the root element */
      el = TtaGetFirstChild (RootElement);
      while (el != NULL)
	{
	  oldel = el;
	  TtaNextSibling (&el);
	  TtaDeleteTree (oldel, doc);
	}
	
      /* Save the path or URL of the document */
      TtaExtractName (pathURL, temppath, tempname);
      TtaSetDocumentDirectory (doc, temppath);

      /* Disable auto save */
      TtaSetDocumentBackUpInterval (doc, 0);

      /* Initialize all parser contexts if not done yet */
      if (firstParserCtxt == NULL)
	InitXmlParserContexts ();

      /* Select root context */
      if (strcmp (TtaGetSSchemaName (DocumentSSchema), "HTML") == 0)
	ChangeXmlParserContextDTD ("HTML");
      else if (strcmp (TtaGetSSchemaName (DocumentSSchema), "GraphML") == 0)
	ChangeXmlParserContextDTD ("GraphML");
      else if (strcmp (TtaGetSSchemaName (DocumentSSchema), "MathML") == 0)
	ChangeXmlParserContextDTD ("MathML");
      else
#ifdef XML_GENERIC
	ChangeXmlParserContextDTD ("XML");
#else /* XML_GENERIC */
      	ChangeXmlParserContextDTD ("HTML");
#endif /* XML_GENERIC */

      /* Gets the document charset */
      charset = TtaGetDocumentCharset (doc);
      /* Specific initialization for expat */
      InitializeExpatParser (charset);
      /* Parse the input file and build the Thot tree */
      XmlParse (stream, &xmlDec, &xmlDoctype);
      /* Completes all unclosed elements */
      if (currentParserCtxt != NULL)
	{
	  el = XMLcontext.lastElement;
	  while (el != NULL)
	    {
		(*(currentParserCtxt->ElementComplete))
		  (el, XMLcontext.doc, &error);
		el = TtaGetParent (el);
	    }
	}
      /* Check the Thot abstract tree for XHTML documents*/
      if (isXHTML)
	CheckAbstractTree (pathURL, XMLcontext.doc);
      FreeExpatParser ();
      FreeXmlParserContexts ();
      gzclose (stream);
      if (docURL != NULL)
	{
	  TtaFreeMemory (docURL);
	  docURL = NULL;
	}
      TtaSetDisplayMode (doc, DisplayImmediately);

      /* Check the Thot abstract tree against the structure schema. */
      TtaSetStructureChecking (1, doc);
      DocumentSSchema = NULL;
    }
  TtaSetDocumentUnmodified (doc);
  
}

/* end of module */
