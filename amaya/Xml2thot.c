/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2002
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
#include "document.h"

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
#include "SVGbuilder_f.h"
#ifdef XML_GENERIC
#include "Xmlbuilder_f.h"
#endif /* XML_GENERIC */
#include "XLinkbuilder_f.h"
#ifdef ANNOTATIONS
#include "annotlib.h"
#include "ANNOTtools_f.h"
#endif /* ANNOTATIONS */
#include "xmlparse.h"
#define NS_SEP '|'
/* maximum length of a Thot structure schema name */
#define MAX_SS_NAME_LENGTH 32

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
    Proc	   UnknownNameSpace;  	/* action to be called if an element 
					   belongs to a not-suported namespace */
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
/* XHTML context */
static PtrParserCtxt	XhtmlParserCtxt = NULL;
/* Generic XML context */
static PtrParserCtxt	GenericXmlParserCtxt = NULL;

#define MAX_URI_NAME_LENGTH  60
#define XHTML_URI       "http://www.w3.org/1999/xhtml"
#define MathML_URI      "http://www.w3.org/1998/Math/MathML"
#define SVG_URI         "http://www.w3.org/2000/svg"
#define XLink_URI       "http://www.w3.org/1999/xlink"
#define NAMESPACE_URI   "http://www.w3.org/XML/1998/namespace"

/* Namespaces table */
#define MAX_NS_TABLE   50
                     /* NameSpace prefix (if defined) */
static char         *Ns_Prefix[MAX_NS_TABLE]; 
                     /* NameSpace URI */
static char         *Ns_Uri[MAX_NS_TABLE]; 
                     /* first free element on the table */
static int           Ns_Level = 0;

/* Current namespaces table */
static char         *CurNs_Prefix[MAX_NS_TABLE]; 
static char         *CurNs_Uri[MAX_NS_TABLE]; 
static int           CurNs_Level = 0;

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
static int           stackLevel = 1;


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
                     /* the last start tag is unknown in the current NS */
static ThotBool      UnknownElement = FALSE;
                     /* the last start tag belongs to a non-supported NS */
static ThotBool      UnknownNS = FALSE;
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
                     /* Comments and PI generated by Amaya are skipped */
static ThotBool      ExtraPI = FALSE;
                     /* the document DOCTYPE is currently parsed */
static ThotBool      WithinDoctype = FALSE;
                     /* parsing errors are not reported for external resources */
static ThotBool      ShowParsingErrors = FALSE;;
static ThotBool	     ParsingSubTree = FALSE;
static ThotBool	     ImmediatelyAfterTag = FALSE;
static ThotBool	     HTMLStyleAttribute = FALSE;
static ThotBool	     XMLSpaceAttribute = FALSE;
static ThotBool      ParsingCDATA = FALSE;
static ThotBool      IgnoreCommentAndPi = FALSE;;
static char	     currentElementContent = ' ';
static char	     currentElementName[40];

/* Global variable to handle white-space in XML documents */
static ThotBool      RemoveLineBreak = FALSE;
static ThotBool      RemoveLeadingSpace = FALSE;   
static ThotBool      RemoveTrailingSpace = FALSE;
static ThotBool      RemoveContiguousSpace = FALSE;

/* "Extra" counters for the characters and the lines read */
static int           extraLineRead = 0;
static int           extraOffset = 0;
static int           htmlLineRead = 0;
static int           htmlCharRead = 0;

/* Virtual DOCTYPE Declaration */
#define DECL_DOCTYPE "<!DOCTYPE html PUBLIC \"\" \"\">\n"
#define DECL_DOCTYPE_LEN 29
#define DECL_XML "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n"
#define DECL_XML_LEN 44
static ThotBool      VirtualDoctype = FALSE;

/* Virtual root for XML sub-tree */
#define SUBTREE_ROOT "amaya-root-subtree"
static  int         SUBTREE_ROOT_LEN = 20;

/* maximum size of error messages */
#define MaxMsgLength 200

static void   StartOfXmlStartElement (char *name);
static void   DisableExpatParser ();
static void   XhtmlCheckInsert (Element *el, Element parent, Document doc, ThotBool *inserted);
static void   XmlCheckInsert (Element *el, Element parent, Document doc, ThotBool *inserted);
static void   XhtmlCheckContext (char *elName, ElementType elType, ThotBool *isAllowed);
static void   XmlCheckContext (char *elName, ElementType elType, ThotBool *isAllowed);

/*----------------------------------------------------------------------
   ChangeXmlParserContextByDTD
   Get the parser context correponding to a given DTD
  ----------------------------------------------------------------------*/
static void   ChangeXmlParserContextByDTD (char *DTDname)

{
  currentParserCtxt = firstParserCtxt;
  while (currentParserCtxt != NULL &&
	 strcmp (DTDname, currentParserCtxt->SSchemaName))
    currentParserCtxt = currentParserCtxt->NextParserCtxt;

  /* initialize the corresponding entry */
  if (currentParserCtxt != NULL &&
      currentParserCtxt != GenericXmlParserCtxt &&
      currentParserCtxt->XMLSSchema == NULL)
    {
      currentParserCtxt->XMLSSchema = 
	GetXMLSSchema (currentParserCtxt->XMLtype, XMLcontext.doc);
      TtaSetUriSSchema (currentParserCtxt->XMLSSchema, currentParserCtxt->UriName);
    }
}

/*----------------------------------------------------------------------
   ChangeXmlParserContextByUri
   Get the parser context correponding to a given namespace uri
  ----------------------------------------------------------------------*/
static void   ChangeXmlParserContextByUri (char *uriName)

{
  currentParserCtxt = firstParserCtxt;
  while (currentParserCtxt != NULL &&
	 strcmp (uriName, currentParserCtxt->UriName))
    currentParserCtxt = currentParserCtxt->NextParserCtxt;

  /* Initialize the corresponding Thot schema */
  if (currentParserCtxt != NULL &&
      currentParserCtxt != GenericXmlParserCtxt &&
      currentParserCtxt->XMLSSchema == NULL)
    {
      currentParserCtxt->XMLSSchema = 
	GetXMLSSchema (currentParserCtxt->XMLtype, XMLcontext.doc);
      TtaSetUriSSchema (currentParserCtxt->XMLSSchema, currentParserCtxt->UriName);
    }
}

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
   ctxt->CheckContext = (Proc) XhtmlCheckContext;
   ctxt->CheckInsert = (Proc) XhtmlCheckInsert;
   ctxt->ElementComplete = (Proc) XhtmlElementComplete;
   ctxt->AttributeComplete = NULL;
   ctxt->GetDTDName = NULL;
   ctxt->UnknownNameSpace = UnknownXhtmlNameSpace;
   ctxt->DefaultLineBreak = TRUE;
   ctxt->DefaultLeadingSpace = TRUE;   
   ctxt->DefaultTrailingSpace = TRUE;  
   ctxt->DefaultContiguousSpace = TRUE;
   ctxt->PreserveLineBreak = FALSE;    
   ctxt->PreserveLeadingSpace = FALSE;   
   ctxt->PreserveTrailingSpace = FALSE;  
   ctxt->PreserveContiguousSpace = FALSE;
   XhtmlParserCtxt = ctxt;
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
   ctxt->CheckContext = (Proc) XmlCheckContext;
   ctxt->CheckInsert = (Proc) XmlCheckInsert;
   ctxt->ElementComplete = (Proc) MathMLElementComplete;
   ctxt->AttributeComplete = (Proc) MathMLAttributeComplete;
   ctxt->GetDTDName = (Proc) MathMLGetDTDName;
   ctxt->UnknownNameSpace = UnknownMathMLNameSpace;
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
   strcpy (ctxt->SSchemaName, "SVG");
   ctxt->UriName = TtaGetMemory (MAX_URI_NAME_LENGTH);
   strcpy (ctxt->UriName, SVG_URI);
   ctxt->XMLSSchema = NULL;
   ctxt->XMLtype = SVG_TYPE;
   ctxt->MapAttribute = (Proc) MapSVGAttribute;
   ctxt->MapAttributeValue = (Proc) MapSVGAttributeValue;
   ctxt->CheckContext = (Proc) XmlCheckContext;
   ctxt->CheckInsert = (Proc) XmlCheckInsert;
   ctxt->ElementComplete = (Proc) SVGElementComplete;
   ctxt->AttributeComplete = (Proc) SVGAttributeComplete;
   ctxt->GetDTDName = (Proc) SVGGetDTDName;
   ctxt->UnknownNameSpace = UnknownSVGNameSpace;
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
   ctxt->CheckContext = (Proc) XmlCheckContext;
   ctxt->CheckInsert = (Proc) XmlCheckInsert;
   ctxt->ElementComplete = NULL;
   ctxt->AttributeComplete = (Proc) XLinkAttributeComplete;
   ctxt->GetDTDName = NULL;
   ctxt->UnknownNameSpace = NULL;
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
   ctxt->CheckContext =  (Proc) XmlCheckContext;
   ctxt->CheckInsert = (Proc) XmlCheckInsert;
   ctxt->ElementComplete = (Proc) XmlElementComplete;
   ctxt->AttributeComplete = (Proc) XmlAttributeComplete;
   ctxt->GetDTDName = NULL;
   ctxt->UnknownNameSpace = NULL;
   ctxt->DefaultLineBreak = TRUE;
   ctxt->DefaultLeadingSpace = TRUE;   
   ctxt->DefaultTrailingSpace = TRUE;  
   ctxt->DefaultContiguousSpace = TRUE;
   ctxt->PreserveLineBreak = FALSE;    
   ctxt->PreserveLeadingSpace = FALSE;   
   ctxt->PreserveTrailingSpace = FALSE;  
   ctxt->PreserveContiguousSpace = FALSE;
   prevCtxt = ctxt;
   GenericXmlParserCtxt = ctxt;
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
   Print the error message msg on stderr.
   When the line is 0 ask to expat the current line number
   When the variable ShowParsingErrors is set to FALSE,
   the message is ignored (we are parsing an external resource).
  ----------------------------------------------------------------------*/
void  XmlParseError (ErrorType type, unsigned char *msg, int line)
{
  if (!ShowParsingErrors)
      return;

  if (line == 0 && Parser == NULL)
    return;

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
	  if (Parser != NULL)
	    {
	      fprintf (ErrFile, "  line %d, char %d: %s\n",
		       XML_GetCurrentLineNumber (Parser) + htmlLineRead -  extraLineRead,
		       XML_GetCurrentColumnNumber (Parser),
		       msg);
	    }
	}
      else
	fprintf (ErrFile, "  line %d: %s\n", line, msg); 
      XMLNotWellFormed = TRUE;
      break;
    case errorCharacterNotSupported:
      if (line == 0)
	{
	  if (Parser != NULL)
	    {
	      fprintf (ErrFile, "  line %d, char %d: %s\n",
		       XML_GetCurrentLineNumber (Parser) + htmlLineRead -  extraLineRead,
		       XML_GetCurrentColumnNumber (Parser),
		       msg);
	    }
	}
      else
	fprintf (ErrFile, "  line %d: %s\n", line, msg); 
      XMLCharacterNotSupported = TRUE;
      break;
    case errorParsing:
      XMLErrorsFound = TRUE;
    case warningMessage:
      if (line == 0)
	{
	  if (Parser != NULL)
	    {
	      fprintf (ErrFile, "  line %d, char %d: %s\n",
		       XML_GetCurrentLineNumber (Parser) + htmlLineRead -  extraLineRead,
		       XML_GetCurrentColumnNumber (Parser),
		       msg);
	    }
	}
      else
	fprintf (ErrFile, "  line %d: %s\n", line, msg);
      break;
    case errorParsingProfile:
      if (line == 0)
	{
	  if (Parser != NULL)
	    {
	      fprintf (ErrFile, "  line %d, char %d: %s\n",
		       XML_GetCurrentLineNumber (Parser) + htmlLineRead -  extraLineRead,
		       XML_GetCurrentColumnNumber (Parser),
		       msg);
	    }
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
  IsXmlParsingCSS 
  Returns the value of ParsingCSS boolean.
  ----------------------------------------------------------------------*/
ThotBool  IsXmlParsingCSS ()

{
   return XMLcontext.parsingCSS;
}

/*----------------------------------------------------------------------
  SetXmlParsingCSS 
  Sets the value of ParsingCSS boolean.
  ----------------------------------------------------------------------*/
void  SetXmlParsingCSS (ThotBool value)

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
  IsWithinXmlTable 
  Returns the value of WithinTable integer.
  ----------------------------------------------------------------------*/
int  IsWithinXmlTable ()

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
  Tries to find a fallback character and generates a symbol if necessary
  ----------------------------------------------------------------------*/
static void XmlGetFallbackCharacter (wchar_t wcharRead, char *entityName,
				     Element el)
{
   ElementType    elType;
   Element        elLeaf, lastChild;
   AttributeType  attrType;
   Attribute	  attr;
   Language       lang;
   unsigned char  fallback[5];
   unsigned char  bufName[10];
   unsigned char  buffer[10];
   unsigned char *ptr;
   int            len, i, j;

   lang = XMLcontext.language;
   GetFallbackCharacter ((int) wcharRead, fallback, &lang);
   if (fallback[0] == '?')
     {
       /* Character not found in the fallback table */
       /* Create a symbol leaf */
       elType = TtaGetElementType (el);
       elType.ElTypeNum = 3;
       elLeaf = TtaNewElement (XMLcontext.doc, elType);
       XmlSetElemLineNumber (elLeaf);
       if (el == XMLcontext.lastElement)
	 InsertXmlElement (&elLeaf);
       else
	 {
	 /* within a comment */
	   lastChild = TtaGetLastChild (el);
	   if (lastChild == NULL)
	     TtaInsertFirstChild (&elLeaf, el, XMLcontext.doc);
	   else
	     TtaInsertSibling (elLeaf, lastChild, FALSE, XMLcontext.doc);
	 }
       /* Put the symbol '?' into the new symbol leaf */
       TtaSetGraphicsShape (elLeaf, fallback[0], XMLcontext.doc);
       /* Changes the wide char code associated with that symbol */
       TtaSetSymbolCode (elLeaf, wcharRead, XMLcontext.doc);
       /* Make that leaf read-only */
       TtaSetAccessRight (elLeaf, ReadOnly, XMLcontext.doc);
     }
   else
     {
       /* Character found in the fallback table */
       /* Create a new text leaf */
       elType = TtaGetElementType (el);
       elType.ElTypeNum = 1;
       elLeaf = TtaNewElement (XMLcontext.doc, elType);
       XmlSetElemLineNumber (elLeaf);
       if (el == XMLcontext.lastElement)
	 InsertXmlElement (&elLeaf);
       else
	 {
	   /* within a comment */
	   lastChild = TtaGetLastChild (el);
	   if (lastChild == NULL)
	     TtaInsertFirstChild (&elLeaf, el, XMLcontext.doc);
	   else
	     TtaInsertSibling (elLeaf, lastChild, FALSE, XMLcontext.doc);
	 }
       /* Put the fallback character into the new text leaf */
       TtaSetTextContent (elLeaf, fallback, lang, XMLcontext.doc);
     }
   
   /* Associate an attribute EntityName with the new leaf */
   attrType.AttrSSchema = elType.ElSSchema;
   ptr = TtaGetSSchemaName (elType.ElSSchema);
   if (strcmp (ptr, "MathML") == 0)
     attrType.AttrTypeNum = MathML_ATTR_EntityName;
   else if (strcmp (ptr, "HTML") == 0)
     attrType.AttrTypeNum = HTML_ATTR_EntityName;
   else
     attrType.AttrTypeNum = HTML_ATTR_EntityName;
   attr = TtaNewAttribute (attrType);
   TtaAttachAttribute (elLeaf, attr, XMLcontext.doc);
   if (entityName)
     /* store the given entity name */
     TtaSetAttributeText (attr, entityName, elLeaf, XMLcontext.doc);
   else
     {
       /* it's a numerical entity */
       len = sprintf (buffer, "%d", (int) wcharRead);
       i = 0;
       bufName[i++] = START_ENTITY;
       bufName[i++] = '#';
       for (j = 0; j < len; j++)
	 bufName[i++] = buffer[j];
       bufName[i++] = ';';
       bufName[i] = EOS;
       TtaSetAttributeText (attr, bufName, elLeaf, XMLcontext.doc);
     }
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
	      IsXMLElementInline (elType, doc))
	 {
	   ancestor = TtaGetParent (ancestor);
	   elType = TtaGetElementType (ancestor);
	 }
       
       if (ancestor != NULL)
	 {
	   elType = TtaGetElementType (ancestor);
	   if (XhtmlCannotContainText (elType) &&
	       !XmlWithinStack (HTML_EL_Option_Menu, XhtmlParserCtxt->XMLSSchema))
	     {
	       /* Element ancestor cannot contain math directly. Create a */
	       /* Pseudo_paragraph element as the parent of the math element */
	       newElType.ElSSchema = XhtmlParserCtxt->XMLSSchema;
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
			 if (IsXMLElementInline (prevType, doc))
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
       if (strcmp (TtaGetSSchemaName (ancestorType.ElSSchema), "HTML") != 0)
	 /* parent is not a HTML element */
	 return;

       while (ancestor != NULL &&
	      IsXMLElementInline (ancestorType, doc))
	 {
	   ancestor = TtaGetParent (ancestor);
	   ancestorType = TtaGetElementType (ancestor);
	 }
       
       if (ancestor != NULL)
	 {
	   elType = TtaGetElementType (ancestor);
	   if (XhtmlCannotContainText (elType) &&
	       !XmlWithinStack (HTML_EL_Option_Menu, XhtmlParserCtxt->XMLSSchema))
	     {
	       /* Element ancestor cannot contain text directly. Create a */
	       /* Pseudo_paragraph element as the parent of the text element */
	       newElType.ElSSchema = XhtmlParserCtxt->XMLSSchema;
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
		       if (!IsXMLElementInline (prevType, doc))
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
     if (!IsXMLElementInline (elType, doc) &&
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
	  IsXMLElementInline (TtaGetElementType (*el), doc)))
       {
         /* it is a character level element */
	 parentType = TtaGetElementType (parent);
	 if (parentType.ElTypeNum == HTML_EL_Text_Area)
	   {
	     /* A basic element cannot be a child of a Text_Area */
	     /* create a Inserted_Text element as a child of Text_Area */
	     newElType.ElSSchema = currentParserCtxt->XMLSSchema;
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
Element        XmlLastLeafInElement (Element el)

{
   Element     child, lastLeaf;
   ElementType childType;

   child = el;
   lastLeaf = NULL;
   while (child != NULL)
     {
       child = TtaGetLastChild (child);
       if (child != NULL)
	 {
	   childType = TtaGetElementType (child);
	   if (TtaHasReturnCreateNLException (childType))
	     child = NULL;
	   else
	     lastLeaf = child;
	 }
     }
   return lastLeaf;
}

/*----------------------------------------------------------------------
  RemoveTrailingSpaces
  Removes all trailing spaces at the end of the element.
  If the parameter all is TRUE, removes only the last space i
  ----------------------------------------------------------------------*/
static void      RemoveTrailingSpaces (Element el)
{
   int           length;
   ElementType   elType;
   Element       lastLeaf;
   AttributeType attrType;
   Attribute     attr = NULL;

   /* Search the last leaf in the element's tree */
   lastLeaf = XmlLastLeafInElement (el);   
   if (lastLeaf != NULL)
     {
       elType = TtaGetElementType (lastLeaf);
       if (elType.ElTypeNum == 1)
	 /* the last leaf is a TEXT element */
	 {
	   length = TtaGetElementVolume (lastLeaf);
	   if (length > 0)
	     {
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
		 TtaRemoveFinalSpaces (lastLeaf, XMLcontext.doc,
				       RemoveTrailingSpace);
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
	       if (!TtaIsAncestor (el, XMLcontext.lastElement))
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

	       (*(currentParserCtxt->ElementComplete)) (el, XMLcontext.doc, &error);

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

/*--------------------  Namespaces procedures  (start)  --------------*/

/*----------------------------------------------------------------------
  NsDeclarationStart
  Treatment called by the namespace declaratatin start handler.
  Update both namespace tables.
  ----------------------------------------------------------------------*/
static void  NsDeclarationStart (char *ns_prefix, char *ns_uri)

{  
  if (Ns_Level >= MAX_NS_TABLE)
    {
      XmlParseError (errorNotWellFormed, "**FATAL** Too many namespace declarations ", 0);
      DisableExpatParser ();
      return;
    }
  
  /* Filling up the table of namespaces declared for the current element */
  CurNs_Prefix[CurNs_Level] = TtaStrdup (ns_prefix);
  CurNs_Uri[CurNs_Level] = TtaStrdup (ns_uri);
  CurNs_Level ++;

  /* Filling up the table of namespaces declared for the whole document */
  if (ns_uri != NULL)
    Ns_Uri[Ns_Level] = TtaStrdup (ns_uri);
  else
    Ns_Uri[Ns_Level] = NULL;
  
  if (ns_prefix != NULL)
    Ns_Prefix[Ns_Level] = TtaStrdup (ns_prefix);
  else
    Ns_Prefix[Ns_Level] = NULL;
  
  Ns_Level ++;
 
}

/*----------------------------------------------------------------------
  NsDeclarationEnd
  Treatment called by the namespace declaratatin end handler.
  Remove the last declaration.
  ----------------------------------------------------------------------*/
static void  NsDeclarationEnd (char *ns_prefix)
{
  int i;

  i = Ns_Level - 1;
  if (i < 0)
    return;

  if (Ns_Prefix[i] != NULL)
    {
      TtaFreeMemory (Ns_Prefix[i]);
      Ns_Prefix[i] = NULL;
    }
  if (Ns_Uri[i] != NULL)
    {
      TtaFreeMemory (Ns_Uri[i]);
      Ns_Uri[i] = NULL;
    }
  Ns_Level --;
}

/*----------------------------------------------------------------------
  NsStartProcessing
  Look for (a) namespace declaration(s) for the current element. If there
  is (are) such (a) declaration(s), update the Document informations.
  Remove all current namespace declaration(s).
  ----------------------------------------------------------------------*/
static void  NsStartProcessing (Element newElement)

{
  int i;

  if (CurNs_Level == 0)
    return;

  if (newElement == 0)
    return;

  /* Update the Namespace Document informations */
  /* and remove the useless declarations */
  for (i = 0; i < CurNs_Level; i++)
    {
      TtaSetNamespaceDeclaration (XMLcontext.doc, newElement,
				  CurNs_Prefix[i], CurNs_Uri[i]);
      if (CurNs_Prefix[i] != NULL)
	{
	  TtaFreeMemory (CurNs_Prefix[i]);
	  CurNs_Prefix[i] = NULL;
	}
      if (CurNs_Uri[i] != NULL)
	{
	  TtaFreeMemory (CurNs_Uri[i]);
	  CurNs_Uri[i] = NULL;
	}
    }
  CurNs_Level = 0; 
}

/*----------------------------------------------------------------------
  NsGiveName
  Give a name to the namespace declaration
  ----------------------------------------------------------------------*/
static void  NsGiveName (char *ns_uri, char **ns_name)

{
  int i;

  *ns_name = NULL;
  if (ns_uri == NULL)
    return;

  for (i = 0; i < Ns_Level; i++)
    {
      if ((Ns_Uri[i] != NULL) && 
	  (strcmp (ns_uri, Ns_Uri[i]) == 0))
	{
	  *ns_name = Ns_Prefix[i];
	  i = Ns_Level;
	}
    }
  return;
}

/*--------------------  Namespaces procedures  (end)  --------------*/


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
static void   XhtmlCheckContext (char *elName, ElementType elType,
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
		   StartOfXmlStartElement ("tr");
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
		     StartOfXmlStartElement ("tr");
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
	       (!IsXMLElementInline (elType,XMLcontext.doc )))
	       *isAllowed = FALSE;
	 }
       
       if (*isAllowed &&
	   strcmp (elName, "body") == 0 &&
	   XmlWithinStack (HTML_EL_BODY, XhtmlParserCtxt->XMLSSchema))
	 /* refuse BODY within BODY */
	 *isAllowed = FALSE;
       
       if (*isAllowed)
	 /* refuse HEAD within HEAD */
	 if (strcmp (elName, "head") == 0)
	   if (XmlWithinStack (HTML_EL_HEAD, XhtmlParserCtxt->XMLSSchema))
	     *isAllowed = FALSE;

       if (*isAllowed)
	 /* refuse STYLE within STYLE */
	 if (strcmp (elName, "style") == 0)
	   if (XmlWithinStack (HTML_EL_STYLE_, XhtmlParserCtxt->XMLSSchema))
	     *isAllowed = FALSE;
       
       return;
     }
}

/*----------------------------------------------------------------------
   GetXmlElType
   Search in the mapping tables the entry for the element elementName and
   returns the corresponding Thot element type.
  ----------------------------------------------------------------------*/
static void   GetXmlElType (char *ns_uri, char *elementName,
			    ElementType *elType, char **mappedName,
			    char *content, ThotBool *level)
{
#ifdef XML_GENERIC
  ThotBool    isnew;
  char       *ns_name;
#endif /* XML_GENERIC */

 /* initialize all parser contexts if not done yet */
  if (firstParserCtxt == NULL)
    InitXmlParserContexts ();

  /* Look at the current context if there is one */
  if (currentParserCtxt != NULL)
    {
#ifdef XML_GENERIC
      if (currentParserCtxt == GenericXmlParserCtxt)
	{
	  /* Search the element inside a no-supported DTD */
	  if (XMLRootName[0] == EOS)
	    {
	      /* This is the document root */
	      elType->ElSSchema = GetGenericXMLSSchema (XMLcontext.doc);
	      /* We instanciate the XML schema with the element name */
	      /* (except for the elements 'comment', doctype and 'pi') */
	      if (strcmp (elementName, "xmlcomment") &&
		  strcmp (elementName, "xmlcomment_line") &&
		  strcmp (elementName, "doctype") &&
		  strcmp (elementName, "doctype_line") &&
		  strcmp (elementName, "xmlpi") &&
		  strcmp (elementName, "xmlpi_line"))
		{
		  NsGiveName (ns_uri, &ns_name);
		  if (ns_name != NULL)
		    TtaChangeGenericSchemaNames (ns_uri, ns_name, XMLcontext.doc);
		  else
		    TtaChangeGenericSchemaNames (ns_uri, elementName, XMLcontext.doc);
		}
	    }
	  else
	    {
	      if (ns_uri != NULL)
		{
		  isnew = FALSE;
		  elType->ElSSchema = GetGenericXMLSSchemaByUri (ns_uri, XMLcontext.doc, &isnew);
		  if (isnew)
		    {
		      NsGiveName (ns_uri, &ns_name);
		      if (ns_name != NULL)
			TtaChangeGenericSchemaNames (ns_uri, ns_name, XMLcontext.doc);
		      else
			TtaChangeGenericSchemaNames (ns_uri, elementName, XMLcontext.doc);
		    }
		}
	      else
		  *elType = TtaGetElementType (RootElement);
	      *level = TRUE;
	      *content = SPACE;
	    }
	  MapGenericXmlElement (elementName, elType, mappedName, XMLcontext.doc);
	}
      else
	{
	  /* Search the element inside a supported DTD */
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
static void       StartOfXmlStartElement (char *name)
{
  ElementType     elType;
  Element         newElement;
  char            msgBuffer[MaxMsgLength];
  char            schemaName[MAX_SS_NAME_LENGTH];
  char           *mappedName = NULL;
  ThotBool        elInStack = FALSE;
  ThotBool        highEnoughLevel = TRUE;
  ThotBool        isAllowed = TRUE;
  char           *buffer, *ptr, *elementName, *nsURI;
  PtrParserCtxt   savParserCtxt = NULL;
#ifdef XML_GENERIC
  char           *s;
#endif /* XML_GENERIC */

  if (stackLevel == MAX_STACK_HEIGHT)
    {
      XmlParseError (errorNotWellFormed, "**FATAL** Too many XML levels", 0);
      UnknownElement = TRUE;
      return;
    }

  UnknownNS = FALSE;
  UnknownElement = FALSE;
  elementName = NULL;
  nsURI = NULL;

  buffer = TtaGetMemory ((strlen (name) + 1));
  if (buffer == NULL)
    return;
  strcpy (buffer, (char*) name);

  savParserCtxt = currentParserCtxt;

  /* Is this element in the scope of a namespace declaration */
  if ((ptr = strrchr (buffer, NS_SEP)) != NULL)
    {
      *ptr = EOS;
      nsURI = TtaGetMemory ((strlen (buffer) + 1));
      strcpy (nsURI, buffer);
      ptr++;
      elementName = TtaGetMemory ((strlen (ptr) + 1));
      strcpy (elementName, ptr);
      /* Look for the context associated with that namespace */
      if (nsURI != NULL)
	ChangeXmlParserContextByUri (nsURI);
    }
  else
    {
      /* No namespace declaration, use the current context */
      elementName = TtaGetMemory (strlen (buffer) + 1);
      strcpy (elementName, buffer);
    }

  if (currentParserCtxt == NULL)
    {
#ifdef XML_GENERIC
      /* Select root context */
      s = TtaGetSSchemaName (DocumentSSchema);
      if ((strcmp (s, "HTML") == 0) ||
	  (strcmp (s, "SVG") == 0) ||
	  (strcmp (s, "MathML") == 0) ||
	  (strcmp (s, "Annot") == 0))
	{
	  /* Not supported namespace within a supported DTD */
	  /* For the moment, we just ignore the namespace */
	  currentParserCtxt = savParserCtxt;
	  UnknownNS = TRUE;
	}
      else
	{
	  /* generic xml document */
	  currentParserCtxt = GenericXmlParserCtxt;
	}
#else XML_GENERIC
      currentParserCtxt = savParserCtxt;
      UnknownNS = TRUE;
#endif /* XML_GENERIC */
    }  

  /* ignore tag <P> within PRE for Xhtml elements */
  if (currentParserCtxt != NULL &&
      (strcmp (currentParserCtxt->SSchemaName, "HTML") == 0) &&
      (XmlWithinStack (HTML_EL_Preformatted, currentParserCtxt->XMLSSchema)) &&
      (strcasecmp (elementName, "p") == 0))
    UnknownElement = TRUE;

  if (currentParserCtxt != NULL && !UnknownElement)
    {
      if (UnknownNS)
	{
	  /* The element belongs to a not supported namespace */
	  sprintf (msgBuffer, 
		   "Namespace not supported for the element <%s>", name);
	  XmlParseError (errorParsing, msgBuffer, 0);
	  /* create an Unknown_namespace element */
	  newElement = NULL;
	  if (nsURI != NULL)
	    sprintf (msgBuffer, "<%s", elementName);	  
	  else
	    sprintf (msgBuffer, "<%s", elementName);	  
	  (*(currentParserCtxt->UnknownNameSpace))
	    (&XMLcontext, &newElement, msgBuffer);
	  /* Store the current namespace declarations for this element */
	  if (CurNs_Level > 0)
	    NsStartProcessing (newElement);
	}
      else
	{
	  /* search the XML element name in the corresponding mapping table */
	  elType.ElSSchema = NULL;
	  elType.ElTypeNum = 0;
	  currentElementName[0] = EOS;
	  GetXmlElType (nsURI, elementName, &elType, &mappedName,
			&currentElementContent, &highEnoughLevel);
	  
	  if (mappedName == NULL)
	    {
	      if (strcmp (currentParserCtxt->SSchemaName, "HTML") == 0)
		strcpy (schemaName, "XHTML");
	      else
		strcpy (schemaName, currentParserCtxt->SSchemaName);
	      if (highEnoughLevel)
		{
		  /* element not found in the corresponding DTD */
		  /* don't process that element */
		  sprintf (msgBuffer, "Invalid or unsupported %s element <%s>",
			   schemaName , elementName);
		  XmlParseError (errorParsing, msgBuffer, 0);
		  UnknownElement = TRUE;
		}
	      else
		{
		  /* invalid element for the document profile */
		  /* don't process that element */
		  sprintf (msgBuffer,
			   "Invalid %s element <%s> for the document profile",
			   schemaName, elementName);
		  XmlParseError (errorParsingProfile, msgBuffer, 0);
		  UnknownElement = TRUE;
		}
	    }
	  else
	    {
	      /* Element found in the corresponding DTD */
	      strcpy (currentElementName, mappedName);
	      
	      if (currentParserCtxt != NULL)
		(*(currentParserCtxt->CheckContext))(mappedName, elType, &isAllowed);
	      if (!isAllowed)
		/* Element not allowed in the current structural context */
		{
		  sprintf (msgBuffer,
			   "The XML element <%s> is not allowed here", elementName);
		  XmlParseError (errorParsing, msgBuffer, 0);
		  UnknownElement = TRUE;
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
		  
		  /* Store the current namespace declarations for this element */
		  if (CurNs_Level > 0)
		    NsStartProcessing (newElement);
		  
		  if (newElement != NULL && elType.ElTypeNum == 1)
		    /* If an empty Text element has been created, */
		    /* the following character data must go to that element */
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
	}
    }
  
  TtaFreeMemory (buffer);
  if (elementName != NULL)
    TtaFreeMemory (elementName);
  if (nsURI != NULL)
    TtaFreeMemory (nsURI);
}

/*----------------------------------------------------------------------
   EndOfXmlStartElement
   Function called at the end of a start tag.
  ----------------------------------------------------------------------*/
static void       EndOfXmlStartElement (char *name)
{
  ElementType     elType;
  AttributeType   attrType;
  Attribute       attr;
  int             length;
  char           *text;

  if (UnknownElement || UnknownNS)
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
static void       EndOfXmlElement (char *name)
{
   ElementType    elType;
   Element        newElement;
   char          *nsURI, *elementName;
   char          *buffer;
   char          *ptr;
   char           msgBuffer[MaxMsgLength];
   char          *mappedName = NULL;
   ThotBool       highEnoughLevel = TRUE;
   PtrParserCtxt  savParserCtxt = NULL;
#ifdef XML_GENERIC
   char          *s;
#endif /* XML_GENERIC */
   
  UnknownNS = FALSE;
  UnknownElement = FALSE;
  elementName = NULL;
  nsURI = NULL;

  buffer = TtaGetMemory ((strlen (name) + 1));
  if (buffer == NULL)
    return;
  strcpy (buffer, (char*) name);

  savParserCtxt = currentParserCtxt;

  /* Is this element in the scope of a namespace declaration */
  if ((ptr = strrchr (buffer, NS_SEP)) != NULL)
    {
      *ptr = EOS;
      nsURI = TtaGetMemory ((strlen (buffer) + 1));
      strcpy (nsURI, buffer);
      ptr++;
      elementName = TtaGetMemory ((strlen (ptr) + 1));
      strcpy (elementName, ptr);
      /* Look for the context associated with that namespace */
      if (nsURI != NULL)
	ChangeXmlParserContextByUri (nsURI);
    }
  else
    {
      elementName = TtaGetMemory (strlen (buffer) + 1);
      strcpy (elementName, buffer);
    }

   if (XMLcontext.parsingTextArea)
     if (strcasecmp (elementName, "textarea") != 0)
       /* We are parsing the contents of a textarea element. */
       /* The end tag is not the one closing the current textarea, */
       /* consider it as plain text */
       return;
 
  if (currentParserCtxt == NULL)
    {
#ifdef XML_GENERIC
      /* Select root context */
      s = TtaGetSSchemaName (DocumentSSchema);
      if ((strcmp (s, "HTML") == 0) ||
	  (strcmp (s, "SVG") == 0) ||
	  (strcmp (s, "MathML") == 0) ||
	  (strcmp (s, "Annot") == 0))
	{
	  /* Not supported namespace within a supported DTD */
	  /* For the moment, we just ignore the namespace */
	  currentParserCtxt = savParserCtxt;
	  UnknownNS = TRUE;
	}
      else
	{
	  /* generic xml document */
	  currentParserCtxt = GenericXmlParserCtxt;
	}
#else XML_GENERIC
      currentParserCtxt = savParserCtxt;
      UnknownNS = TRUE;
#endif /* XML_GENERIC */
    }
 
  /* search the element name in the corresponding mapping table */
  elType.ElSSchema = NULL;
  elType.ElTypeNum = 0;
  currentElementName[0] = EOS;
  if (UnknownNS)
    {
      /* create an Unknown_namespace element */
      newElement = NULL;
      if (nsURI != NULL)
	sprintf (msgBuffer, "</%s", elementName);	  
      else
	sprintf (msgBuffer, "</%s", elementName);	  
      (*(currentParserCtxt->UnknownNameSpace))
	(&XMLcontext, &newElement, msgBuffer);
    }
  else
    {
      GetXmlElType (nsURI, elementName, &elType, &mappedName,
		    &currentElementContent, &highEnoughLevel);
      if (mappedName == NULL)
	/* element not found in the corresponding DTD */
	UnknownElement = TRUE;
      else
	{
	  /* element found in the corresponding DTD */
	  if (!XmlCloseElement (mappedName))
	    {
	      /* the end tag does not close any current element */
		   sprintf (msgBuffer, "Unexpected end tag </%s>", elementName);
		   XmlParseError (errorParsing, msgBuffer, 0);
	    }
	}
    }
  
  TtaFreeMemory (buffer);
  if (elementName != NULL)
    TtaFreeMemory (elementName);
  if (nsURI != NULL)
    TtaFreeMemory (nsURI);
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

       if (strcmp (currentParserCtxt->SSchemaName, "XML") == 0)
	 {
	   /* Does the parent element contain a 'Line' presentation rule ? */
	   if (TtaHasXmlInLineRule (elType, XMLcontext.doc))
	     removeLeadingSpaces = FALSE;
	 }
       else
	 {
	   if (IsXMLElementInline (lastElType, XMLcontext.doc))
	     {
	       if (strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
		 {
		   if (elType.ElTypeNum != HTML_EL_Option_Menu &&
		       elType.ElTypeNum != HTML_EL_OptGroup)
		     {
		       removeLeadingSpaces = FALSE;
		       if (lastElType.ElTypeNum == HTML_EL_BR)
			 removeLeadingSpaces = TRUE;
		     }
		 }
	       else if (strcmp (TtaGetSSchemaName (elType.ElSSchema), "SVG") == 0)
		 {
		   if (elType.ElTypeNum == SVG_EL_text_ ||
		       elType.ElTypeNum == SVG_EL_tspan )
		     removeLeadingSpaces = FALSE;
		 }
	     }
	   else
	     {
	       if ((strcmp (TtaGetSSchemaName (lastElType.ElSSchema), "HTML") == 0) &&
		   ((lastElType.ElTypeNum == HTML_EL_Comment_) ||
		    (lastElType.ElTypeNum == HTML_EL_XMLPI)))
		 removeLeadingSpaces = XhtmlCannotContainText (elType);
	     }
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
		  IsXMLElementInline (ancestorType, XMLcontext.doc))
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
void PutInXmlElement (char *data, int length)

{
   ElementType  elType;
   Element      elText;
   char        *buffer, *bufferws;
   int          i = 0;
   int          i1, i2 = 0, i3 = 0;
   ThotBool     uselessSpace = FALSE;

   i = 0;
   /* Immediately after a start tag, treatment of the leading spaces */
   /* If RemoveLeadingSpace = TRUE, we suppress all leading white-space */
   /* characters, otherwise, we only suppress the first line break */
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
 
   length -= i;
   if (length == 0 || data[i] == EOS)
     return;

   bufferws = TtaGetMemory (length + 1);
   strncpy (bufferws, &data[i], length);
   bufferws[length] = EOS;

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
	   if (elType.ElSSchema == currentParserCtxt->XMLSSchema &&
	       elType.ElTypeNum == 1 && XMLcontext.mergeText)
	     {
	       if ((buffer[i1] == SPACE) && RemoveContiguousSpace)
		 {
		   /* Is the last character of text element a space */
		   if (TtaHasFinalSpace (XMLcontext.lastElement, XMLcontext.doc))
		   /* Remove leading space if last content was finished by a space */
		     TtaAppendTextContent (XMLcontext.lastElement,
					   &(buffer[i1 + 1]), XMLcontext.doc);
		   else
		     TtaAppendTextContent (XMLcontext.lastElement,
					   &(buffer[i1]), XMLcontext.doc);
		 }
	       else
		 TtaAppendTextContent (XMLcontext.lastElement,
				       &(buffer[i1]), XMLcontext.doc);
	     }
	   else
	     {
	       /* create a TEXT element */
	       if (currentParserCtxt->XMLSSchema != NULL)
		 elType.ElSSchema = currentParserCtxt->XMLSSchema;
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
	       /* associate a specific 'Line' presentation rule to the 
		  parent element if we are parsing a generic-XML element */
#ifdef XML_GENERIC
	       if (strcmp (currentParserCtxt->SSchemaName, "XML") == 0)
		 CreateXmlLinePRule (elText, XMLcontext.doc);
#endif /* XML_GENERIC */
	     }
	   TtaFreeMemory (buffer);
	 }
       TtaFreeMemory (bufferws);
     }
}

/*----------------------------------------------------------------------
  HandleXMLstring handles the UTF-8 character string data and generates
  entities when needed.
  When handling the content of an attribute value (element = NULL) the
  function generates a single string where entities are translated when
  it's possible.
  When handling the content of a standard element (element != NULL and
  stdText == TRUE) the function generates TEXT elements and SYMBOLS
  itself.
  When handling the content of a comment (element != NULL and
  stdText == FALSE) the function stops either when the whole string is
  handled or when it has generated a fallback character for an entity
  (or a non Iso-latin character in the non I18N version).
  Returns
  - the content of the TEXT element.
  - the length of the parsed original string in *length.
  ----------------------------------------------------------------------*/
static unsigned char *HandleXMLstring (unsigned char *data, int *length,
				       Element element, ThotBool stdText)
{
  unsigned char *buffer;
  unsigned char *ptr;
#ifndef _I18N_
  wchar_t        wcharRead;
  int            tmplen;
  unsigned char  tmpbuf[10];
  int            nbBytesRead = 0;
#endif /* _I18N_ */
  unsigned char *entityName;
  int            i = 0, j = 0;
  int            max;
  int            k, l, m;
  int            entityValue;	
  ThotBool       found, end;

  max = *length;
  buffer = TtaGetMemory (4 * max + 1);
  while (i < max)
    {
      if (data[i] == START_ENTITY)
	{
	  /* Maybe it is the beginning of an entity */
	  end = FALSE;
	  entityName = TtaGetMemory (max + 1);
	  l = 0;
	  entityName[l++] = START_ENTITY;
	  for (k = i + 1; k < max && !end; k++)
	    {
	      if (data[k] == '&')
		{
		  /* An '&' inside an other '&' ?? We suppose */
		  /* the first one doesn't belong to an entity */
		  k = max;
		  buffer[j++] = START_ENTITY;
		}
	      else if (data[k] == ';')
		{
		  /* End of the entity */
		  end = TRUE;
		  entityName[l] = EOS;
		  found = MapXMLEntity (currentParserCtxt->XMLtype,
					&entityName[1], &entityValue);
#ifdef _I18N_
		  if (found && (entityValue <  0x3FF ||
				entityValue == 0x200D ||
				entityValue == 0x200E /* lrm */ ||
				entityValue == 0x200F /* rlm */ ||
				entityValue == 0x202A /* lre */ ||
				entityValue == 0x202B /* rle */ ||
				entityValue == 0x202D /* lro */ ||
				entityValue == 0x202E /* rlo */ ||
				entityValue == 0x202C /* pdf */))
		    {
		      /* get the UTF-8 string of the unicode character */
		      ptr = &buffer[j];
		      j += TtaWCToMBstring ((wchar_t) entityValue, &ptr);
		    }
#else /* _I18N_ */
		  if (found && entityValue <= 255)
		    /* store the ISO latin1 character */
		    buffer[j++] = entityValue;
#endif /* _I18N_ */
		  else if (found && element)
		    {
		      if (stdText)
			{
			  buffer[j] = EOS;
			  if (j > 0)
			    {
			      /* close the current text element */
			      PutInXmlElement (buffer, j);
			      XMLcontext.lastElementClosed = TRUE;
			    }
			  XMLcontext.mergeText = FALSE;
			  ImmediatelyAfterTag = FALSE;
			  element = XMLcontext.lastElement;
			  j = 0;
			}
		      else
			{
			  /* stop the handling of the original string */
			  max = i;
			  *length = max;
			}
		      /* generate a fallback character */
		      entityName[l++] = ';';
		      entityName[l] = EOS;
		      XmlGetFallbackCharacter ((wchar_t)entityValue,
					       entityName, element);
		      if (stdText)
			{
			  XMLcontext.lastElementClosed = TRUE;
			  XMLcontext.mergeText = FALSE;
			}
		    }
		  else
		    {
		      /* store the entity name */
		      for (m = 0; entityName[m] != EOS; m++)
			buffer[j++] = entityName[m];
		      buffer[j++] = ';';
		    }
		}
	      else
		entityName[l++] = data[k];
	      i++;
	    }
	  TtaFreeMemory (entityName);
	  i++;
	}
      else
	{
#ifdef _I18N_
	  buffer[j++] = data[i++];
#else /* _I18N_ */
	  ptr = &data[i];
	  nbBytesRead = TtaGetNextWCFromString (&wcharRead,
						&ptr, UTF_8);
	  /* the string provided by expat is obviously correct */
	  i += nbBytesRead;
	  if (wcharRead <= 255)
	      /* ISO-Latin1 character */
	    buffer[j++] = (unsigned char) wcharRead;
	  else if (element)
	    {
	      if (stdText)
		{
		  buffer[j] = EOS;
		  if (j > 0)
		    {
		      /* close the current text element */
		      PutInXmlElement (buffer, j);
		      XMLcontext.lastElementClosed = TRUE;
		    }
		  XMLcontext.mergeText = FALSE;
		  ImmediatelyAfterTag = FALSE;
		  element = XMLcontext.lastElement;
		  j = 0;
		}
	      else
		{
		  /* stop the handling of the original string */
		  max = i;
		  *length = max;
		}
	      XmlGetFallbackCharacter (wcharRead, NULL, element);
	      if (stdText)
		{
		  XMLcontext.lastElementClosed = TRUE;
		  XMLcontext.mergeText = FALSE;
		}
	    }
	  else
	    {
	      /* it's not an ISO-Latin1 character */
	      tmplen = sprintf (tmpbuf, "%d", (int) wcharRead);
	      buffer[j++] = START_ENTITY;
	      buffer[j++] = '#';
	      for (k = 0; k < tmplen; k++)
		buffer[j++] = tmpbuf[k];
	      buffer[j++] = ';';
	    }
#endif /* _I18N_ */
	}
    }
  buffer[j] = EOS;
  if (stdText)
    {
      if (j > 0)
	PutInXmlElement (buffer, j);
    }  
  return buffer;
}
/*----------------------  Data  (end)  ---------------------------*/


/*--------------------  Attributes  (start)  ---------------------*/
/*----------------------------------------------------------------------
  UnknownXmlAttribute
  Creation of filling of an "unknown_attr" attribute
  ----------------------------------------------------------------------*/
static void      UnknownXmlAttribute (char *xmlAttr)
     
{
   AttributeType attrType;
   Attribute     attr;
   char         *buffer;
   ThotBool      level = TRUE;
   int           length;

   if (currentParserCtxt != NULL)
     {
       /* Attach an Invalid_attribute to the current element */
       attrType.AttrSSchema = currentParserCtxt->XMLSSchema;
       if (currentParserCtxt == XhtmlParserCtxt)
	 attrType.AttrTypeNum = HTML_ATTR_Unknown_attribute;
	 else
	   (*(currentParserCtxt->MapAttribute)) ("unknown_attr", &attrType,
						 currentElementName,
						 &level, XMLcontext.doc);
       if (attrType.AttrTypeNum > 0)
	 {
	   attr = TtaGetAttribute (XMLcontext.lastElement, attrType);
	   if (attr == NULL)
	     {
	       attr = TtaNewAttribute (attrType);
	       TtaAttachAttribute (XMLcontext.lastElement, attr, XMLcontext.doc);
	       length = strlen (xmlAttr);
	       buffer = TtaGetMemory (length + 2);
	       strcpy (buffer, " ");
	       strcat (buffer, xmlAttr);
	       TtaSetAttributeText (attr, buffer,
				    XMLcontext.lastElement,
				    XMLcontext.doc);
	       TtaFreeMemory (buffer);
	     }
	   else
	     {
	       /* Copy the name of the attribute as the content */
	       /* of the Invalid_attribute attribute. */
	       length = strlen (xmlAttr) + 2;
	       length += TtaGetTextAttributeLength (attr);
	       buffer = TtaGetMemory (length + 1);
	       TtaGiveTextAttributeValue (attr, buffer, &length);
	       strcat (buffer, " ");
	       strcat (buffer, xmlAttr);
	       TtaSetAttributeText (attr, buffer,
				    XMLcontext.lastElement,
				    XMLcontext.doc);
	       TtaFreeMemory (buffer);
	     }
	   currentAttribute = attr;
	 }
     }
   return;
}

/*----------------------------------------------------------------------
   EndOfXhtmlAttributeName   
   End of a XHTML attribute
  ----------------------------------------------------------------------*/
static void EndOfXhtmlAttributeName (char *attrName, Element el,
				     Document doc)
{
 AttributeMapping   *mapAttr;
 AttributeType       attrType;
 ElementType         elType;
 Attribute           attr;
 char                translation;
 ThotBool            highEnoughLevel = TRUE;
 char                msgBuffer[MaxMsgLength];

   UnknownAttr = FALSE;
   attrType.AttrTypeNum = 0;
   mapAttr = MapHTMLAttribute (attrName, &attrType, currentElementName,
			       &highEnoughLevel, doc);
   if (attrType.AttrTypeNum <= 0)
     {
       /* this attribute is not in the HTML mapping table */
       if (strcasecmp (attrName, "xml:lang") == 0)
	 /* attribute xml:lang is not considered as invalid, but it is ignored */
	 lastMappedAttr = NULL;
       else if (highEnoughLevel)
	 {
	   sprintf (msgBuffer, "Invalid XHTML attribute \"%s\"", attrName);
	   XmlParseError (errorParsing, msgBuffer, 0);
	 }
       else
	 {
	   sprintf (msgBuffer,
		    "Invalid XHTML attribute \"%s\" for the document profile",
		    attrName);
	   XmlParseError (errorParsingProfile, msgBuffer, 0);
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
static void EndOfXmlAttributeName (char *attrName, char *uriName,
				   Element  el, Document doc)
{
   AttributeType    attrType;
   Attribute        attr;
   char             msgBuffer[MaxMsgLength];
   ThotBool         level = TRUE;
   char             schemaName[MAX_SS_NAME_LENGTH];
   ElementType      elType;
   ThotBool         isnew;

   attrType.AttrTypeNum = 0;

   if (currentParserCtxt != NULL)
     {
#ifdef XML_GENERIC
       if (currentParserCtxt == GenericXmlParserCtxt)
	 {
	   if (uriName != NULL)
	     {
	       isnew = FALSE;
	       attrType.AttrSSchema = GetGenericXMLSSchemaByUri (uriName, XMLcontext.doc, &isnew);
	       if (isnew)
		 TtaChangeGenericSchemaNames (uriName, attrName, XMLcontext.doc);
	     }
	   else
	     {
	       elType = TtaGetElementType (XMLcontext.lastElement);
	       attrType.AttrSSchema = elType.ElSSchema;
	     }
	   MapGenericXmlAttribute (attrName, &attrType, doc);
	 }
       else
#endif /* XML_GENERIC */
	 {
	   if (currentParserCtxt->MapAttribute)
	     (*(currentParserCtxt->MapAttribute)) (attrName, &attrType,
						   currentElementName,
						   &level, doc);
	 }
     }
   
   if (attrType.AttrTypeNum <= 0)
     {
       /* This attribute is not in the corresponding mapping table */
       strcpy (schemaName, currentParserCtxt->SSchemaName);
       sprintf (msgBuffer, "Invalid or unsupported %s attribute \"%s\"",
		schemaName, attrName);
       XmlParseError (errorParsing, msgBuffer, 0);
       /* Attach an Invalid_attribute to the current element */
       /* It may be a valid attribute that is not yet defined in Amaya tables */
       UnknownXmlAttribute (attrName);
       UnknownAttr = TRUE;
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
static void      EndOfAttributeName (char *xmlName)
     
{
   char         *buffer;
   char         *attrName, *nsURI;
   char         *ptr = NULL;
   char         *s = NULL;
   PtrParserCtxt savParserCtxt = NULL;
   unsigned char msgBuffer[MaxMsgLength];

   currentAttribute = NULL;
   lastMappedAttr = NULL;
   UnknownAttr = FALSE;
   HTMLStyleAttribute = FALSE;
   XMLSpaceAttribute = FALSE;

   if (UnknownElement)
     /* The corresponding element doesn't belong to the current DTD */ 
     return;

   /* look for a NS_SEP in the tag name (namespaces) */ 
   /* and ignore the prefix if there is one */
   savParserCtxt = currentParserCtxt;
   buffer = TtaGetMemory (strlen (xmlName) + 1);
   strcpy (buffer, (char*) xmlName);
   nsURI = NULL;

   if ((ptr = strrchr (buffer, NS_SEP)) != NULL)
     {
       /* This attribute belongs to a specific namespace */
       *ptr = EOS;
       ptr++;
       nsURI = TtaGetMemory ((strlen (buffer) + 1));
       strcpy (nsURI, buffer);
       /* Specific treatment to get round a bug in EXPAT parser */
       /* It replaces first "xml:" prefix by the namespaces URI */
       if (strcmp (nsURI, NAMESPACE_URI) == 0)
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
	     ChangeXmlParserContextByUri (buffer);
	 }
     }
   else
     {
       /* This attribute belongs to a same namespace than the element */
       attrName = TtaGetMemory (strlen (buffer) + 1);
       strcpy (attrName, buffer);
       if (UnknownNS)
	 /* The corresponding element belongs to a no-supported namespace */ 
	 {
	   sprintf (msgBuffer, 
		    "Namespace not supported for the attribute \"%s\"", xmlName);
	   XmlParseError (errorParsing, msgBuffer, 0);
	   /* Create an unknown attribute  */
	   UnknownXmlAttribute (attrName);
	   UnknownAttr = TRUE;
	 }
     }
   
   if (currentParserCtxt == NULL)
     {
#ifdef XML_GENERIC
       /* Select root context */
       s = TtaGetSSchemaName (DocumentSSchema);
       if ((strcmp (s, "HTML") == 0) ||
	   (strcmp (s, "SVG") == 0) ||
	   (strcmp (s, "MathML") == 0) ||
	   (strcmp (s, "Annot") == 0))
	 /* It is not a generic xml document */
	 /* generate an "unknown_attr" attribute */
	 {
	   currentParserCtxt = savParserCtxt;
	   sprintf (msgBuffer, 
		    "Namespace not supported for the attribute \"%s\"",
		    xmlName);
	   XmlParseError (errorParsing, msgBuffer, 0);
	   if (nsURI != NULL)
	     sprintf (msgBuffer, "%s", attrName);
	   else
	     sprintf (msgBuffer, "%s", attrName);
	   UnknownXmlAttribute (msgBuffer);
	   UnknownAttr = TRUE;
	 }
       else
	 /* We assign the generic XML context by default */ 
	 currentParserCtxt = GenericXmlParserCtxt;
#else /* XML_GENERIC */
       currentParserCtxt = savParserCtxt;
       sprintf (msgBuffer, 
		"Namespace not supported for the attribute \"%s\"",
		xmlName);
       XmlParseError (errorParsing, msgBuffer, 0);
       /* Create an unknown attribute  */
       sprintf (msgBuffer, attrName);
       UnknownXmlAttribute (msgBuffer);
       UnknownAttr = TRUE;
#endif /* XML_GENERIC */  
     }

   /* Is it a xml:space attribute */
   if (strncmp (attrName, "xml:space", 9) == 0)
     XMLSpaceAttribute = TRUE;
   
   /* the attribute xml:lang is replaced by the attribute "lang" */
   if (strncmp (attrName, "xml:lang", 8) == 0)
     strcpy (attrName, "lang");

   if (currentParserCtxt != NULL && !UnknownAttr)
     {
       if (strcmp (currentParserCtxt->SSchemaName, "HTML") == 0)
	 EndOfXhtmlAttributeName (attrName,
				  XMLcontext.lastElement, XMLcontext.doc);
       else
	 EndOfXmlAttributeName (attrName, nsURI,
				XMLcontext.lastElement, XMLcontext.doc);
     }
   
  if (nsURI != NULL)
    TtaFreeMemory (nsURI);
   TtaFreeMemory (buffer);
   TtaFreeMemory (attrName);
   return;
}

/*----------------------------------------------------------------------
   EndOfXmlAttributeValue
   An attribute value has been read for a element that
   doesn't belongs to XHTML DTD
  ----------------------------------------------------------------------*/
static void EndOfXmlAttributeValue (char *attrValue)

{
   AttributeType    attrType;
   int		    attrKind, val;
   unsigned char    msgBuffer[MaxMsgLength];
   int              length;
   char            *buffer;

   TtaGiveAttributeType (currentAttribute, &attrType, &attrKind);
   switch (attrKind)
     {
     case 0:       /* enumerate */
       (*(currentParserCtxt->MapAttributeValue)) (attrValue, attrType, &val);
       if (val <= 0)
	 {
	   sprintf (msgBuffer, "Unknown attribute value \"%s\"", attrValue);
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
       if (UnknownAttr)
	 {
	   /* This is the content of an invalid attribute */
	   /* Append it to the current Invalid_attribute */
	   length = strlen (attrValue) + 4;
	   length += TtaGetTextAttributeLength (currentAttribute);
	   buffer = TtaGetMemory (length + 1);
	   TtaGiveTextAttributeValue (currentAttribute,
				      buffer, &length);
	   strcat (buffer, "=\"");
	   strcat (buffer, attrValue); 
	   strcat (buffer, "\"");
	   TtaSetAttributeText (currentAttribute, buffer,
				XMLcontext.lastElement, XMLcontext.doc);
	   TtaFreeMemory (buffer);
	 }
       else
	 {
	   TtaSetAttributeText (currentAttribute, attrValue,
				XMLcontext.lastElement, XMLcontext.doc);
	   if (HTMLStyleAttribute)
	     ParseHTMLSpecificStyle (XMLcontext.lastElement, attrValue,
				     XMLcontext.doc, 100, FALSE);
	 }
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
   An attribute value has been read from the parsed file.
   Put that value in the current Thot attribute.
  ----------------------------------------------------------------------*/
static void       EndOfAttributeValue (unsigned char *attrValue,
				       unsigned char *attrName)
{
   unsigned char *buffer;
   char          *bufferNS;
   int            length;

  if ((lastMappedAttr || currentAttribute) && currentParserCtxt)
    {
      length = strlen (attrValue);
      buffer = HandleXMLstring (attrValue, &length, NULL, FALSE);
      /* White-space attribute */
      if (XMLSpaceAttribute)
	XmlWhiteSpaceInStack (buffer);
      
      if (UnknownAttr)
	{
	  /* This is the content of an invalid attribute */
	  /* Append it to the current Invalid_attribute */
	  length = strlen (attrValue) + 4;
	  length += TtaGetTextAttributeLength (currentAttribute);
	  bufferNS = TtaGetMemory (length + 1);
	  TtaGiveTextAttributeValue (currentAttribute,
				     bufferNS, &length);
	  strcat (bufferNS, "=\"");
	  strcat (bufferNS, attrValue); 
	  strcat (bufferNS, "\"");
	  TtaSetAttributeText (currentAttribute, bufferNS,
			       XMLcontext.lastElement, XMLcontext.doc);
	  TtaFreeMemory (bufferNS);
	}
      else
	{
	  if (strcmp (currentParserCtxt->SSchemaName, "HTML") == 0)
	    EndOfHTMLAttributeValue (buffer, lastMappedAttr,
				     currentAttribute, lastAttrElement,
				     UnknownAttr, &XMLcontext, TRUE);
	  else
	    EndOfXmlAttributeValue (buffer);
	}
      TtaFreeMemory (buffer);
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
   unsigned char    *buffer;

   data[0] = START_ENTITY;
   buffer = HandleXMLstring (data, &length, XMLcontext.lastElement, TRUE);
   TtaFreeMemory (buffer);
}
/*--------------------  Entities  (end)  ---------------------*/

/*--------------------  Doctype  (start)  ---------------------*/
/*----------------------------------------------------------------------
   ParseDoctypeElement
   Parse the content of a DOCTYPE declaration
  -------------------------------------- -------------------------------*/
static void ParseDoctypeElement (char *data, int length)
{
  ElementType     elType;
  Element  	  doctypeLine, doctypeLeaf, doctypeLineNew, lastChild;
  char           *mappedName;
  char            cont;
  ThotBool        level = TRUE;
  unsigned char  *buffer;
  int             i, j;

  /* get the last Doctype_line element */
  doctypeLine = TtaGetLastChild (XMLcontext.lastElement);
  if (doctypeLine == NULL)
      return;

  buffer = TtaGetMemory (length + 1);
  i = 0;
  j = 0;
  while (i < length)
    {
      /* Look for line breaks in the content and create as many */
      /* DOCTYPE_line elements as needed */
      if (data[i] != EOL && data[i] != __CR__)
	{
	  buffer[j] = data[i];
	  j++;
	}
      else if (data[i] == __CR__ && i < length-1 && data[i+1] == EOL)
	{
	  /* ignoring CR in a CR/LF sequence */
	}
      else
	{
	  buffer[j] = EOS;
	  j = 0;
	  elType = TtaGetElementType (doctypeLine);
	  elType.ElTypeNum = 1;
	  doctypeLeaf = TtaNewElement (XMLcontext.doc, elType);
	  if (doctypeLeaf != NULL)
	    {
	      XmlSetElemLineNumber (doctypeLeaf);
	      /* get the position of the Doctype text */
	      lastChild = TtaGetLastChild (doctypeLine);
	      if (lastChild == NULL)
		TtaInsertFirstChild (&doctypeLeaf, doctypeLine, XMLcontext.doc);
	      else
		TtaInsertSibling (doctypeLeaf, lastChild,
				  FALSE, XMLcontext.doc);
	      /* We use the Latin_Script language to avoid the spell_chekcer */
	      /* to check this element */
	      TtaSetTextContent (doctypeLeaf, buffer, Latin_Script, XMLcontext.doc);
	    }
	  /* Create a new DOCTYPE_line element */
	  elType.ElSSchema = NULL;
	  elType.ElTypeNum = 0;
	  GetXmlElType (NULL, "doctype_line", &elType, &mappedName, &cont, &level);
	  doctypeLineNew = TtaNewElement (XMLcontext.doc, elType);
	  if (doctypeLineNew != NULL)
	    {
	      XmlSetElemLineNumber (doctypeLineNew);
	      TtaInsertSibling (doctypeLineNew, doctypeLine, FALSE, XMLcontext.doc);
	      doctypeLine = doctypeLineNew;
	    }
	}
      i++;
    }
  
  buffer [j] = EOS;
  elType = TtaGetElementType (doctypeLine);
  elType.ElTypeNum = 1;
  doctypeLeaf = TtaNewElement (XMLcontext.doc, elType);
  if (doctypeLeaf != NULL)
    {
      XmlSetElemLineNumber (doctypeLeaf);
      /* get the position of the Doctype text */
      lastChild = TtaGetLastChild (doctypeLine);
      if (lastChild == NULL)
	TtaInsertFirstChild (&doctypeLeaf, doctypeLine, XMLcontext.doc);
      else
	TtaInsertSibling (doctypeLeaf, lastChild, FALSE, XMLcontext.doc);
      /* We use the Latin_Script language to avoid the spell_chekcer */
      /* to check this element */
      TtaSetTextContent (doctypeLeaf, buffer, Latin_Script, XMLcontext.doc);
    }

  TtaFreeMemory (buffer);
}

/*----------------------------------------------------------------------
   CreateDoctypeElement
   Create a Doctype element into the Thot tree.
  ----------------------------------------------------------------------*/
static void       CreateDoctypeElement ()
{
  ElementType     elType;
  Element  	  doctypeEl, doctypeLineEl;
  char           *mappedName;
  char            cont;
  ThotBool        level = TRUE;

  /* Create a DOCTYPE element */
  elType.ElSSchema = NULL;
  elType.ElTypeNum = 0;
  GetXmlElType (NULL, "doctype", &elType, &mappedName, &cont, &level);
  if (elType.ElTypeNum > 0)
    {
      doctypeEl = TtaNewElement (XMLcontext.doc, elType);
      XmlSetElemLineNumber (doctypeEl);
      InsertXmlElement (&doctypeEl);
      /* Make the DOCTYPE element read-only */
      TtaSetAccessRight (doctypeEl, ReadOnly, XMLcontext.doc);
      /* Create a DOCTYPE_line element as first child */
      elType.ElSSchema = NULL;
      elType.ElTypeNum = 0;
      GetXmlElType (NULL, "doctype_line", &elType, &mappedName, &cont, &level);
      doctypeLineEl = TtaNewElement (XMLcontext.doc, elType);
      XmlSetElemLineNumber (doctypeLineEl);
      TtaInsertFirstChild (&doctypeLineEl, doctypeEl, XMLcontext.doc);
    }
}
/*--------------------  Doctype  (end)  ------------------------------*/

/*--------------------  CDATA  (start)  ---------------------*/
/*----------------------------------------------------------------------
   ParseCdataElement
   Parse the content of a CDATA element
  -------------------------------------- -------------------------------*/
static void       ParseCdataElement (char *data, int length)

{
  ElementType     elType;
  Element  	  cdataLine, cdataLeaf, cdataLineNew, lastChild;
  char           *mappedName;
  char            cont;
  ThotBool        level = TRUE;
  unsigned char  *buffer;
  int             i, j;

  buffer = TtaGetMemory (length + 1);
  i = 0, j = 0;

  /* get the last cdata_line element */
  cdataLine = TtaGetLastChild (XMLcontext.lastElement);
  if (cdataLine == NULL)
    return;

  /* Remove the leading spaces */
  if (RemoveLeadingSpace)
    {
      while (data[i] == TAB || data[i] == SPACE)
	i++;
    }
  
  while (i < length)
    {
      /* Look for line breaks in the content and create as many */
      /* cdata_line elements as needed */
      if (data[i] != EOL && data[i] != __CR__)
	{
	  /* Collapse contiguous spaces */
	  if (!RemoveContiguousSpace ||
	      !((data[i] == TAB || data[i] == SPACE) &&
		(buffer[j-1] == TAB || buffer[j-1] == SPACE)))
	    buffer[j] = data[i];
	  j++;
	}
      else if (data[i] == __CR__ && i < length-1 && data[i+1] == EOL)
	{
	  /* ignoring CR in a CR/LF sequence */
	}
      else
	{
	  buffer[j] = EOS;
	  j = 0;
	  if (buffer[0] != EOS)
	    {
	      elType = TtaGetElementType (cdataLine);
	      elType.ElTypeNum = 1;
	      cdataLeaf = TtaNewElement (XMLcontext.doc, elType);
	      if (cdataLeaf != NULL)
		{
		  XmlSetElemLineNumber (cdataLeaf);
		  /* get the position of the cdata text */
		  lastChild = TtaGetLastChild (cdataLine);
		  if (lastChild == NULL)
		    TtaInsertFirstChild (&cdataLeaf, cdataLine, XMLcontext.doc);
		  else
		    TtaInsertSibling (cdataLeaf, lastChild,
				      FALSE, XMLcontext.doc);
		  TtaSetTextContent (cdataLeaf, buffer,
				     XMLcontext.language, XMLcontext.doc);
		}
	    }
	  /* Create a new cdata_line element */
	  elType.ElSSchema = NULL;
	  elType.ElTypeNum = 0;
	  GetXmlElType (NULL, "cdata_line", &elType, &mappedName, &cont, &level);
	  cdataLineNew = TtaNewElement (XMLcontext.doc, elType);
	  if (cdataLineNew != NULL)
	    {
	      XmlSetElemLineNumber (cdataLineNew);
	      TtaInsertSibling (cdataLineNew, cdataLine, FALSE, XMLcontext.doc);
	      cdataLine = cdataLineNew;
	    }
	}
      i++;
    }
  
  if (j > 0)
    {
      buffer [j] = EOS;
      elType = TtaGetElementType (cdataLine);
      elType.ElTypeNum = 1;
      cdataLeaf = TtaNewElement (XMLcontext.doc, elType);
      if (cdataLeaf != NULL)
	{
	  XmlSetElemLineNumber (cdataLeaf);
	  /* get the position of the cdata text */
	  lastChild = TtaGetLastChild (cdataLine);
	  if (lastChild == NULL)
	    TtaInsertFirstChild (&cdataLeaf, cdataLine, XMLcontext.doc);
	  else
	    TtaInsertSibling (cdataLeaf, lastChild,
			      FALSE, XMLcontext.doc);
	  TtaSetTextContent (cdataLeaf, buffer,
			     XMLcontext.language, XMLcontext.doc);
	}
    }

  TtaFreeMemory (buffer);
}

/*----------------------------------------------------------------------
   CreateCdataElement
   Create a CDATA element into the Thot tree.
  ----------------------------------------------------------------------*/
static void       CreateCdataElement ()
{
  ElementType     elType;
  Element  	  cdataEl, cdataLineEl;
  char           *mappedName;
  char            cont;
  ThotBool        level = TRUE;

  /* Create a CDATA element */
  elType.ElSSchema = NULL;
  elType.ElTypeNum = 0;
  GetXmlElType (NULL, "cdata", &elType, &mappedName, &cont, &level);
  if (elType.ElTypeNum > 0)
    {
      cdataEl = TtaNewElement (XMLcontext.doc, elType);
      XmlSetElemLineNumber (cdataEl);
      InsertXmlElement (&cdataEl);
      /* Create a cdata_line element as first child */
      elType.ElSSchema = NULL;
      elType.ElTypeNum = 0;
      GetXmlElType (NULL, "cdata_line", &elType, &mappedName, &cont, &level);
      cdataLineEl = TtaNewElement (XMLcontext.doc, elType);
      XmlSetElemLineNumber (cdataLineEl);
      TtaInsertFirstChild (&cdataLineEl, cdataEl, XMLcontext.doc);
    }
}

/*--------------------  CDATA  (end)  ------------------------------*/


/*--------------------  Comments  (start)  ---------------------*/
/*----------------------------------------------------------------------
   CreateXmlComment
   Create a comment element into the Thot tree.
  ----------------------------------------------------------------------*/
static void      CreateXmlComment (char *commentValue)
{
  ElementType    elType, elTypeLeaf;
  Element  	 commentEl, commentLineEl, commentLeaf, lastChild;
  char          *mappedName;
  char           cont;
  unsigned char *buffer;
  unsigned char *ptr;
  int            length, l;
  int            i, j, error;
  ThotBool       level = TRUE;
  char          *ptrComment;

   /* Skip the comments automatically generated by Amaya */
   ExtraPI = FALSE;
   ptrComment = strstr (commentValue, "generated by Amaya");
   if (ptrComment)
     {
       ExtraPI = TRUE;
       return;
     }
 
  /* Create a Thot element for the comment */
  elType.ElSSchema = NULL;
  elType.ElTypeNum = 0;
  GetXmlElType (NULL, "xmlcomment", &elType, &mappedName, &cont, &level);
  if (elType.ElTypeNum > 0)
    {
      commentEl = TtaNewElement (XMLcontext.doc, elType);
      XmlSetElemLineNumber (commentEl);
      InsertXmlElement (&commentEl);
      /* Create a XMLcomment_line element as the first child of */
      /* Element XMLcomment */
      elType.ElSSchema = NULL;
      elType.ElTypeNum = 0;
      GetXmlElType (NULL, "xmlcomment_line", &elType, &mappedName, &cont, &level);
      commentLineEl = TtaNewElement (XMLcontext.doc, elType);
      XmlSetElemLineNumber (commentLineEl);
      TtaInsertFirstChild (&commentLineEl, commentEl, XMLcontext.doc);
      length = strlen (commentValue);
      i = 0;
      j = 0; /* parsed length */
      ptr = &commentValue[i];
      while (i <= length)
	{
	  /* Look for line break in the comment and create as many */
	  /* XMLcomment_line elements as needed */
	  if (commentValue[i] != EOL && commentValue[i] != __CR__ &&
	      commentValue[i] != EOS)
	    i++;
	  else
	    {
	      /* get the position of the comment text */
	      lastChild = TtaGetLastChild (commentLineEl);
	      l = i - j;
	      /* handles UTF-8 characters and entities in the subtree */
	      buffer = HandleXMLstring (ptr, &l, commentLineEl, FALSE);
	      /* Put the current content into a text comment line */
	      elTypeLeaf.ElSSchema = elType.ElSSchema;
	      elTypeLeaf.ElTypeNum = 1;
	      commentLeaf = TtaNewElement (XMLcontext.doc, elTypeLeaf);
	      XmlSetElemLineNumber (commentLeaf);
	      if (lastChild == NULL)
		TtaInsertFirstChild (&commentLeaf, commentLineEl,
				     XMLcontext.doc);
	      else
		TtaInsertSibling (commentLeaf, lastChild,
				  FALSE, XMLcontext.doc);	     
	      TtaSetTextContent (commentLeaf, buffer,
				 XMLcontext.language, XMLcontext.doc);
	      TtaFreeMemory (buffer);
	      j += l;
	      i = j;
	      /* check if a new line has to be generated */
	      if (commentValue[i] == EOL || commentValue[i] == __CR__)
		{
		  /* Create a new XMLcomment_line element */
		  commentLineEl = TtaNewElement (XMLcontext.doc, elType);
		  XmlSetElemLineNumber (commentLineEl);
		  /* Inserts the new XMLcomment_line after the previous one */
		  TtaInsertSibling (commentLineEl, TtaGetParent (commentLeaf),
				    FALSE, XMLcontext.doc);
		  j++;
		}
	      i++;
	      ptr = &commentValue[i];
	    }
	}
      (*(currentParserCtxt->ElementComplete)) (commentEl,  XMLcontext.doc, &error);
      XMLcontext.lastElementClosed = TRUE;
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
   CSSmedia      css_media;
   CSSInfoPtr    css_info;
   ThotBool      ok;

   length = strlen (PiData);
   buffer = TtaGetMemory (length + 1);
   i = 0; j = 0;
   css_media = CSS_ALL;

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

   /* Warnings about PI are no longer reported */
   /*
   if (!ok)
     {
       char  msgBuffer[MaxMsgLength];
       sprintf (msgBuffer,
		 "xml-stylesheet : attribute \"type\" not defined or not supported");
       XmlParseError (errorParsing, msgBuffer, 0);
     }
   */

   if (ok)
     {
       /* get the "media" attribute */
       end = NULL;
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
		   /* get the CSS URI in UTF-8 */
		   css_href = ReallocUTF8String (css_href, XMLcontext.doc);
		   LoadStyleSheet (css_href, XMLcontext.doc, NULL,
				   css_info, css_media, FALSE);
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
static void       CreateXmlPi (char *piTarget, char *piData)
{
   ElementType    elType, elTypeLeaf;
   Element  	  piEl, piLineEl, piLeaf, lastChild;
   char          *mappedName;
   char          *piValue = NULL;
   char           cont;
   unsigned char *buffer;
   unsigned char *ptr;
   int            length, l;
   int            i, j, error;
   ThotBool       level = TRUE;
   char          *ptrPi;

   /* Skip the xsl stylesheet automatically generated by Amaya */
   ptrPi = strstr (piData, MATHML_XSLT_NAME);
   if (ptrPi && ExtraPI)
     {
       ExtraPI = FALSE;
       return;
     }
  
   /* Create a Thot element for the PI */
   elType.ElSSchema = NULL;
   elType.ElTypeNum = 0;
   GetXmlElType (NULL, "xmlpi", &elType, &mappedName, &cont, &level);
   if (elType.ElTypeNum > 0)
     {
       piEl = TtaNewElement (XMLcontext.doc, elType);
       XmlSetElemLineNumber (piEl);
       InsertXmlElement (&piEl);
       /* Create a XMLPI_line element as the first child of element XMLPI */
       elType.ElSSchema = NULL;
       elType.ElTypeNum = 0;
       GetXmlElType (NULL, "xmlpi_line", &elType, &mappedName, &cont, &level);
       piLineEl = TtaNewElement (XMLcontext.doc, elType);
       XmlSetElemLineNumber (piLineEl);
       TtaInsertFirstChild (&piLineEl, piEl, XMLcontext.doc);
       length = strlen (piTarget) + strlen (piData);
       length++;
       piValue = TtaGetMemory (length + 2);
       strcpy (piValue, piTarget);
       strcat (piValue, " ");
       strcat (piValue, piData);
       i = 0;
       j = 0; /* parsed length */
       ptr = &piValue[i];
       while (i <= length)
	 {
	  /* Look for line break in the pi and create as many */
	  /* XMLpi_line elements as needed */
	  if (piValue[i] != EOL && piValue[i] != __CR__ &&
	      piValue[i] != EOS)
	    i++;
	  else
	    {
	      /* get the position of the text element */
	      lastChild = TtaGetLastChild (piLineEl);
	      l = i - j;
	      /* handles UTF-8 characters and entities in the subtree */
	      buffer = HandleXMLstring (ptr, &l, piLineEl, FALSE);
	      if (buffer[0] != EOS)
		{
		  /* Put the current content into a text element */
		  elTypeLeaf.ElSSchema = elType.ElSSchema;
		  elTypeLeaf.ElTypeNum = 1;
		  piLeaf = TtaNewElement (XMLcontext.doc, elTypeLeaf);
		  XmlSetElemLineNumber (piLeaf);
		  if (lastChild == NULL)
		    TtaInsertFirstChild (&piLeaf, piLineEl, XMLcontext.doc);
		  else
		    TtaInsertSibling (piLeaf, lastChild,
				      FALSE, XMLcontext.doc);
		  /* We use the Latin_Script language to avoid the spell_chekcer */
		  /* to check this element */
		  TtaSetTextContent (piLeaf, buffer, Latin_Script, XMLcontext.doc);
		}
	      TtaFreeMemory (buffer);
	      j += l;
	      i = j;
	      /* check if a new line has to be generated */
	      if (piValue[i] == EOL || piValue[i] == __CR__)
		{
		  /* Create a new XMLpi_line element */
		  piLineEl = TtaNewElement (XMLcontext.doc, elType);
		  XmlSetElemLineNumber (piLineEl);
		  /* Inserts the new XMLpi_line after the previous one */
		  TtaInsertSibling (piLineEl, TtaGetParent (piLeaf),
				    FALSE, XMLcontext.doc);
		  j++;
		}
	      i++;
	      ptr = &piValue[i];
	    }
	 }
       (*(currentParserCtxt->ElementComplete)) (piEl, XMLcontext.doc, &error);
       XMLcontext.lastElementClosed = TRUE;
       TtaFreeMemory (piValue);
     }
   
   /* Call the treatment that correspond to that PI */
   /* Actually, Amaya supports only the "xml-stylesheet" PI */
   if (!strcmp (piTarget, "xml-stylesheet"))
     XmlStyleSheetPi (piData);
   /* Warnings about PI are no longer reported */
   /*
   else
     {
       char msgBuffer[MaxMsgLength];
       sprintf (msgBuffer,
		"Processing Instruction not supported : %s", piTarget);
       XmlParseError (errorParsing, msgBuffer, 0);
     }
   */
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
  printf ("Hndl_CdataStart\n");
#endif /* EXPAT_PARSER_DEBUG */
  /* The content of the current element has not to be parsed */
  ParsingCDATA = TRUE;
  CreateCdataElement ();
}

/*----------------------------------------------------------------------
   Hndl_CdataEnd
   Handlers that get called at the end of a CDATA section
  ----------------------------------------------------------------------*/
static void     Hndl_CdataEnd (void *userData)

{
#ifdef EXPAT_PARSER_DEBUG
  printf ("Hndl_CdataEnd\n");
#endif /* EXPAT_PARSER_DEBUG */
  /* The content of the current element has not to be parsed */
  ParsingCDATA = FALSE;
  XMLcontext.lastElementClosed = TRUE;
}

/*----------------------------------------------------------------------
   Hndl_CharacterData
   Handler for the text
   The string the handler receives is NOT zero terminated.
   We have to use the length argument to deal with the end of the string.
  ----------------------------------------------------------------------*/
static void Hndl_CharacterData (void *userData, const XML_Char *data,
				int length)
{
  unsigned char *buffer, *ptr;

#ifdef EXPAT_PARSER_DEBUG
  printf ("Hndl_CharacterData - length = %d - \n", length);
#endif /* EXPAT_PARSER_DEBUG */
  /* The content of the current element has not to be parsed */
  if (ParsingCDATA)
    ParseCdataElement ((char*) data, length);
  else
    {
      ptr = (unsigned char *) data;
      /* handles UTF-8 characters and entities in the subtree */
      buffer = HandleXMLstring (ptr, &length, XMLcontext.lastElement, TRUE);
      /* the whole content is now handled */
      TtaFreeMemory (buffer);
    }
}

/*----------------------------------------------------------------------
   Hndl_Comment
   Handler for comments
   The data is all text inside the comment delimiters
  ----------------------------------------------------------------------*/
static void Hndl_Comment (void *userData, const XML_Char *data)
{

  unsigned char *buffer;
#ifdef EXPAT_PARSER_DEBUG
  printf ("Hndl_Comment %s\n", data);
#endif /* EXPAT_PARSER_DEBUG */
  /* This comment has not to be parsed */
  if (WithinDoctype)
    {
      buffer = (unsigned char *) data;
      ParseDoctypeElement ("<!--", 4);
      ParseDoctypeElement (buffer, strlen (buffer));
      ParseDoctypeElement ("-->", 3);
      return;
    }
  if (!IgnoreCommentAndPi) 
    CreateXmlComment ((char*) data);
}


/*----------------------------------------------------------------------
   Hndl_DefaultExpand
   Default handler with expansion of internal entity references
  ----------------------------------------------------------------------*/
static void Hndl_DefaultExpand (void *userData,
				const XML_Char *data,
				int   length)
{
  unsigned char *ptr;

#ifdef EXPAT_PARSER_DEBUG
   int i;
   printf ("Hndl_DefaultExpand - length = %d - '", length);
   for (i=0; i<length; i++)
       printf ("%c", data[i]);
   printf ("'\n");
#endif /* EXPAT_PARSER_DEBUG */
   /* The content of the current element has not to be parsed */
   ptr = (unsigned char *) data;
   /* Are we parsing the content of the DOCTYPE declaration ? */
   if (WithinDoctype)
     ParseDoctypeElement (ptr, length);
   else
     {
       /* Specific treatment for the entities */
       if (length > 1 && data[0] == '&')
	 CreateXmlEntity ((char*) data, length);
     }
}

/*----------------------------------------------------------------------
   Hndl_DoctypeStart
   Handler for the start of the DOCTYPE declaration.
   It is called when the name of the DOCTYPE is encountered.
  ----------------------------------------------------------------------*/
static void Hndl_DoctypeStart (void *userData,
			       const XML_Char *doctypeName)
{
#ifdef EXPAT_PARSER_DEBUG
   printf ("Hndl_DoctypeStart %s\n", doctypeName);
#endif /* EXPAT_PARSER_DEBUG */
   /* The content of this doctype has not to be parsed */
   if (!VirtualDoctype)
     {
       CreateDoctypeElement ();
       WithinDoctype = TRUE;
       ParseDoctypeElement ("<!DOCTYPE ", 10);
     }
}

/*----------------------------------------------------------------------
   Hndl_DoctypeEnd
   Handler for the start of the DOCTYPE declaration.
   It is called when the closing > is encountered,
   but after processing any external subset.
  ----------------------------------------------------------------------*/
static void Hndl_DoctypeEnd (void *userData)
{
#ifdef EXPAT_PARSER_DEBUG
   printf ("Hndl_DoctypeEnd\n");
#endif /* EXPAT_PARSER_DEBUG */
   /* The content of this doctype has not to be parsed */
   if (VirtualDoctype)
     VirtualDoctype = FALSE;
   else
     {
       ParseDoctypeElement (">", 1);
       XMLcontext.lastElementClosed = TRUE;
       WithinDoctype = FALSE;
     }
}

/*----------------------------------------------------------------------
   Hndl_ElementStart
   Handler for start tags
   Attributes are passed as a pointer to a vector of char pointers
  ----------------------------------------------------------------------*/
static void Hndl_ElementStart (void *userData,
			       const XML_Char *name,
			       const XML_Char **attlist)
{
   unsigned char *attrName = NULL;
   unsigned char *attrValue = NULL;
   PtrParserCtxt  elementParserCtxt = NULL;
   
#ifdef EXPAT_PARSER_DEBUG
   printf ("Hndl_ElementStart <%s>\n", name);
#endif /* EXPAT_PARSER_DEBUG */
   
   /* Treatment for the GI */
   if (XMLcontext.parsingTextArea)
     {
       /* We are parsing the contents of a TEXTAREA element */
       /* If a start tag appears, consider it as plain text */
     }
   else
     {     
       /* Ignore the virtual root of an XML sub-tree */
       /* we are parsing the result of a transformation */
       if (strcmp ((char*) name, SUBTREE_ROOT) != 0)
	 {
	   /* Treatment called at the beginning of a start tag */
	   StartOfXmlStartElement ((char*) name);
	   /* We save the current element context */
	   elementParserCtxt = currentParserCtxt;
	   
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
		   printf (" value=\"%s\"\n", attrValue);
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
	   
	   /*----- Treatment called at the end of a start tag -----*/
	   EndOfXmlStartElement ((char*) name);
	   
	   /*----- We are immediately after a start tag -----*/
	   ImmediatelyAfterTag = TRUE;
	   
	   /* Initialize the root element */
	   if (XMLRootName[0] == EOS)
	     /* This is the first parsed element */
	     strcpy (XMLRootName, (char*) name);
	 }
     }
}
       
/*----------------------------------------------------------------------
   Hndl_ElementEnd
   Handler for end tags
  ----------------------------------------------------------------------*/
static void     Hndl_ElementEnd (void *userData, const XML_Char *name)

{
#ifdef EXPAT_PARSER_DEBUG
   printf ("Hndl_ElementEnd </%s>\n", name);
#endif /* EXPAT_PARSER_DEBUG */
   
   ImmediatelyAfterTag = FALSE;
   
   /* Ignore the virtual root of a XML sub-tree */
   if ((strcmp ((char*) name, SUBTREE_ROOT) != 0) && 
       (XMLRootName[0] != EOS) && !XMLrootClosed)
     {
       EndOfXmlElement ((char*) name); 
       /* Is it the end tag of the root element ? */
       if (!strcmp (XMLRootName, (char*) name) && stackLevel == 1)
	 {
	   XMLrootClosed = TRUE;
	 }
     }
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
  printf ("\nHndl_ExternalEntityRef\n");
  printf ("  context  : %s\n", context);
  printf ("  base     : %s\n", base);
  printf ("  systemId : %s\n", systemId);
  printf ("  publicId : %s\n", publicId);
#endif /* EXPAT_PARSER_DEBUG */
  return 1;
}

/*----------------------------------------------------------------------
   Hndl_NameSpaceStart
   Handler for the start of namespace declarations
  ----------------------------------------------------------------------*/
static void     Hndl_NameSpaceStart (void *userData,
				     const XML_Char *ns_prefix,
				     const XML_Char *ns_uri)

{
#ifdef EXPAT_PARSER_DEBUG
  printf ("Hndl_NameSpaceStart - prefix=\"%s\" uri=\"%s\"\n", ns_prefix, ns_uri);
#endif /* EXPAT_PARSER_DEBUG */
  /* This namespace has not to be parsed */  
  NsDeclarationStart ((char *) ns_prefix, (char*) ns_uri);
}

/*----------------------------------------------------------------------
   Hndl_NameSpaceEnd
   Handler for the end of namespace declarations
  ----------------------------------------------------------------------*/
static void     Hndl_NameSpaceEnd (void *userData,
				   const XML_Char *ns_prefix)

{
#ifdef EXPAT_PARSER_DEBUG
  printf ("Hndl_NameSpaceEnd - prefix=\"%s\"\n", ns_prefix);
#endif /* EXPAT_PARSER_DEBUG */
  /* This namespace has not to be parsed */
  NsDeclarationEnd ((char *) ns_prefix);
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
  printf ("Hndl_Notation\n");
  printf ("  notationName : %s\n", notationName);
  printf ("  base         : %s\n", base);
  printf ("  systemId     : %s\n", systemId);
  printf ("  publicId     : %s\n", publicId);
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
  printf ("Hndl_NotStandalone\n");
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
  unsigned char *buffer;

#ifdef EXPAT_PARSER_DEBUG
  printf ("Hndl_PI\n");
  printf ("  target : %s\n", target);
  printf ("  pidata : %s\n", pidata);
#endif /* EXPAT_PARSER_DEBUG */
  /* This PI has not to be parsed */
  if (WithinDoctype)
    {
      ParseDoctypeElement ("<?", 2);
      buffer = (unsigned char *) target;
      ParseDoctypeElement (buffer, strlen (buffer));
      ParseDoctypeElement (" ", 1);
      buffer = (unsigned char *) pidata;
      ParseDoctypeElement (buffer, strlen (buffer));
      ParseDoctypeElement ("?>", 2);
      return;
    }
  if (!IgnoreCommentAndPi) 
    CreateXmlPi ((char*) target, (char*) pidata);
}

/*----------------------------------------------------------------------
   Hndl_UnknownEncoding
   Handler to deal with encodings other than the built in
  ----------------------------------------------------------------------*/
static int Hndl_UnknownEncoding (void           *encodingData,
				 const XML_Char *name,
				 XML_Encoding   *info)
{
#ifdef EXPAT_PARSER_DEBUG
  printf ("Hndl_UnknownEncoding - name : %s\n", name);
#endif /* EXPAT_PARSER_DEBUG */
  /* This PI has not to be parsed */
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
static void Hndl_UnparsedEntity (void *userData,
				 const XML_Char *entityName,
				 const XML_Char *base,
				 const XML_Char *systemId,
				 const XML_Char *publicId,
				 const XML_Char *notationName)
{
#ifdef EXPAT_PARSER_DEBUG
  printf ("Hndl_UnparsedEntity\n");
  printf ("  entityName   : %s\n", entityName);
  printf ("  base         : %s\n", base);
  printf ("  systemId     : %s\n", systemId);
  printf ("  publicId     : %s\n", publicId);
  printf ("  notationName : %s\n", notationName);
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
  int            i;

   /* Free parser contexts */
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

   /* Free NameSpace table */
   for (i = 0; i < Ns_Level; i++)
     {
       if (Ns_Prefix[i])
	 {
	   TtaFreeMemory (Ns_Prefix[i]);
	   Ns_Prefix[i] = NULL;
	 }
       if (Ns_Uri[i])
	 {
	   TtaFreeMemory (Ns_Uri[i]);
	   Ns_Uri[i] = NULL;
	 }
     }
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

  /* Construct a new parser with namespace processing */
  /* accordingly to the document encoding */
  /* If that encoding is unknown, we don''t parse the document */
  if (charset == UNDEFINED_CHARSET)
    {
      /* Default encoding for XML documents */
      Parser = XML_ParserCreateNS ("UTF-8", NS_SEP);
      /* Display a warning message */
      sprintf (msgBuffer,
	       "Warning: no encoding specified, assuming UTF-8");
      XmlParseError (undefinedEncoding, msgBuffer, 0);
      /* TtaSetDocumentCharset (XMLcontext.doc, UTF_8); */
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
	   charset == ISO_8859_6   || charset == ISO_8859_7   ||
	   charset == ISO_8859_8   || charset == ISO_8859_9   ||
	   charset == ISO_8859_15  || charset == KOI8_R       ||
	   charset == WINDOWS_1250 || charset == WINDOWS_1251 ||
	   charset == WINDOWS_1252 || charset == WINDOWS_1253 ||
	   charset == WINDOWS_1254 || charset == WINDOWS_1255 ||
	   charset == WINDOWS_1256 || charset == WINDOWS_1257 ||
	   charset == ISO_2022_JP  || charset == EUC_JP       ||
	   charset == SHIFT_JIS)
    /* buffers will be converted to UTF-8 by Amaya */
    Parser = XML_ParserCreateNS ("UTF-8", NS_SEP);
  else
    {
      XMLUnknownEncoding = TRUE;
      XmlParseError (errorEncoding,
		     TtaGetMessage (AMAYA, AM_UNKNOWN_ENCODING), -1);
      Parser = XML_ParserCreateNS ("ISO-8859-1", NS_SEP);
      /*return;*/
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
  UnknownElement = FALSE;
  XMLRootName[0] = EOS;
  XMLrootClosed = FALSE;
  IgnoreCommentAndPi = FALSE;
  ParsingCDATA = FALSE;
  VirtualDoctype = FALSE;
  ShowParsingErrors =  TRUE;

  htmlLineRead = 0;
  htmlCharRead = 0;
  
  /* initialize the stack of opened elements */
  stackLevel = 1;
  Ns_Level = 0;
  CurNs_Level = 0;
  elementStack[0] = RootElement;
}

/*----------------------------------------------------------------------
  MoveExternalAttribute
  ----------------------------------------------------------------------*/
static void  MoveExternalAttribute (Element elold, Element elnew, Document doc)
{
  AttributeType   attrType;
  Attribute       attrold, attrnew, nextattr;
  int             attrKind, val, length;
  char           *buffer;

  /* Attach the attributes to the new element */
  nextattr = NULL;
  TtaNextAttribute (elold, &nextattr);
  while (nextattr != NULL)
    {
      attrold = nextattr;
      TtaNextAttribute (elold, &nextattr);
      TtaGiveAttributeType (attrold, &attrType, &attrKind);
      
      attrnew = TtaNewAttribute (attrType);
      TtaAttachAttribute (elnew, attrnew, doc);
      switch (attrKind)
	{
	case 0: /* enumerate */
	case 1:	/* integer */
	  val = TtaGetAttributeValue (attrold);
	  TtaSetAttributeValue (attrnew, val, elnew, doc);
	  break;
	case 2:	/* text */
	  length = TtaGetTextAttributeLength (attrold);
	  buffer = TtaGetMemory (length + 1);
	  TtaGiveTextAttributeValue (attrold, buffer, &length);
	  TtaSetAttributeText (attrnew, buffer, elnew, doc);
	  TtaFreeMemory (buffer);
	  break;
	case 3:	/* reference */
	  break;
	}     
    }
}

/*----------------------------------------------------------------------
  SetExternalElementType
  Previous treatment relative to the element which is
  the parent of the new sub-tree
  ----------------------------------------------------------------------*/
static Element  SetExternalElementType (Element el, Document doc,
					ThotBool *use_ref)
{
  ElementType   elType, parentType;
  Element       parent, elemElement, elemContent;
  int           oldStructureChecking;
 
  elemElement = NULL;
  elemContent = NULL;
  elType = TtaGetElementType (el);
  *use_ref = FALSE;

  /* Disable structure checking */
  oldStructureChecking = TtaGetStructureChecking (doc);
  TtaSetStructureChecking (0, doc);

  if ((strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0) &&
      (elType.ElTypeNum == HTML_EL_PICTURE_UNIT))
    {
      /* We are parsing an external picture within a HTML document */
      /* Is there an Embed_Content element? */
      parent = TtaGetParent (el);
      parentType = TtaGetElementType (parent);
      if (parentType.ElTypeNum == HTML_EL_Object)
	{
	  /* Create a SVG_ImageContent element */
	  elType.ElTypeNum = HTML_EL_SVG_ImageContent;
	  elemContent = TtaNewElement (doc, elType);
	  if (elemContent != NULL)
	    {
	      TtaInsertSibling (elemContent, el, FALSE, doc);
	      /* Attach the attributes to that new element */
	       MoveExternalAttribute (el, elemContent, doc);
	      /* Remove the PICTURE_UNIT element form the tree */
	      TtaDeleteTree (el, doc);
	    }
	}
      else
	{
	  /* create a SVG_Image element instead of the PICTURE element */
	  elType.ElTypeNum = HTML_EL_SVG_Image;
	  elemElement = TtaNewElement (doc, elType);
	  if (elemElement != NULL)
	    {
	      TtaInsertSibling (elemElement, el, FALSE, doc);
	      /* Attach the attributes to that new element */
	       MoveExternalAttribute (el, elemElement, doc);
	      /* create a SVG_ImageContent element */
	      elType.ElTypeNum = HTML_EL_SVG_ImageContent;
	      elemContent = TtaNewElement (doc, elType);
	      if (elemContent != NULL)
		TtaInsertFirstChild (&elemContent, elemElement, doc);
	      /* Remove the PICTURE_UNIT element form the tree */
	      TtaDeleteTree (el, doc);
	    }
	}
    }
  else if ((strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0) &&
	   elType.ElTypeNum == HTML_EL_Embed_)
    {
      /* We are parsing an embed element within a HTML document*/
      /* Is there an Embed_Content element? */
      elType.ElTypeNum = HTML_EL_Embed_Content;
      elemContent = TtaSearchTypedElement (elType, SearchInTree, el);
      if (!elemContent)
	/* no, create one */
	{
	  elemContent = TtaNewElement (doc, elType);
	  if (elemContent != NULL)
	    TtaInsertFirstChild (&elemContent, el, doc);
	}
    }
  else if ((strcmp (TtaGetSSchemaName (elType.ElSSchema), "SVG") == 0) &&
	   elType.ElTypeNum == SVG_EL_PICTURE_UNIT)
    {
      /* We are parsing a SVG image */
      /* Create a SVG_Image element within a SVG element */
      elType.ElTypeNum = SVG_EL_SVG_Image;
      elemContent = TtaNewElement (doc, elType);
      if (elemContent != NULL)
	{
	  TtaInsertSibling (elemContent, el, FALSE, doc);
	  /* Attach the attributes to that new element */
	   MoveExternalAttribute (el, elemContent, doc);
	  /* Remove the PICTURE_UNIT element form the tree */
	  TtaDeleteTree (el, doc);
	}
    }
  else if ((strcmp (TtaGetSSchemaName (elType.ElSSchema), "SVG") == 0) &&
	   elType.ElTypeNum == SVG_EL_use_)
    {
      /* We are parsing an external SVG use element */
      elemContent = el;
      *use_ref = TRUE;
    }

  /* Restore the structure checking */
  TtaSetStructureChecking (oldStructureChecking, doc);

  return elemContent;
}


/*----------------------------------------------------------------------
  ParseExternalXmlResource
  Parse an xml resource called from a document and complete the
  corresponding Thot abstract tree.
  Return TRUE if the parsing of the external resource doesn't detect errors.
  ----------------------------------------------------------------------*/
ThotBool       ParseExternalXmlResource (char     *fileName,
					 Element   el,
					 ThotBool  isclosed,
					 Document  doc,
					 Language  lang,
					 char     *DTDname)

{
  int           tmpLen = 0;
  char         *schemaName = NULL;
  char         *ptr = NULL;
  ElementType   elType;
  Element       parent, oldel;
  CHARSET       charset;
  DisplayMode   dispMode;
#define	 COPY_BUFFER_SIZE	1024
  gzFile        infile;
  char          bufferRead[COPY_BUFFER_SIZE];
  int           res, i,  parsingLevel, tmpLineRead = 0;
  ThotBool      endOfFile = FALSE;
  ThotBool      xmlDec, docType, isXML, xmlns;
  DocumentType  thotType;
  ThotBool      use_ref = FALSE;
  Document      externalDoc = 0;
  Element       idEl = NULL, extEl = NULL, copyEl = NULL;
  char          charsetname[MAX_LENGTH];
  char         *extUseUri = NULL, *extUseId = NULL, *s = NULL;
  AttributeType extAttrType;

  if (fileName == NULL)
    return FALSE;

  /* Avoid too many redisplay */
  dispMode = TtaGetDisplayMode (doc);
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, DeferredDisplay);

  /* Initialize all parser contexts */
  if (firstParserCtxt == NULL)
    InitXmlParserContexts ();
 
  /* General initialization */
  RootElement = NULL;
  if (DTDname != NULL &&
      ((strcmp (DTDname, "SVG") == 0) || (strcmp (DTDname, "MathML") == 0)))
    {
      /* We are parsing an external svg or mathml file */
      extEl = SetExternalElementType (el, doc, &use_ref);
      if (extEl == NULL)
	{
	  FreeXmlParserContexts ();
	  return FALSE;
	}
      if (use_ref)
	{
	  /* We are parsing an external reference to a 'use' svg element */
	  /* Create a new document with no presentation schema */
	  /* and loads the target document */
	  externalDoc = TtaNewDocument ("SVG", "tmp");
	  if (externalDoc == 0)
	    {
	      FreeXmlParserContexts ();
	      return FALSE;
	    }
	  else
	    {
	      TtaSetPSchema (externalDoc, "SVGP");
	      RootElement = TtaGetMainRoot (externalDoc);
	      InitializeXmlParsingContext (externalDoc, RootElement, FALSE, FALSE);
	      /* Disable structure checking for the external document*/
	      TtaSetStructureChecking (0, externalDoc);
	      /* Delete all element except the root element */
	      parent = TtaGetFirstChild (RootElement);
	      while (parent != NULL)
		{
		  oldel = parent;
		  TtaNextSibling (&parent);
		  TtaDeleteTree (oldel, externalDoc);
		}
	    }
	}
      else
	InitializeXmlParsingContext (doc, extEl, isclosed, TRUE);
      ChangeXmlParserContextByDTD (DTDname);
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
      InitializeXmlParsingContext (doc, el, isclosed, TRUE);
      ChangeXmlParserContextByDTD (schemaName);
    }
  
  /* specific Initialization */
  XMLcontext.language = lang;
  DocumentSSchema = TtaGetDocumentSSchema (doc);

  /* Set document URL */
  tmpLen = strlen (fileName);
  docURL = TtaGetMemory (tmpLen + 1);

  if (use_ref)
    {
      /* We are parsing an external reference for a 'use' svg element */
      extUseUri = TtaGetMemory (strlen (fileName) + 1);
      strcpy (extUseUri, fileName);
      /* Is that element refers to an ID ? */
      if ((ptr = strrchr (extUseUri, '#')) != NULL)
	{
	  *ptr = EOS;
	  ptr++;
	  extUseId = TtaGetMemory ((strlen (ptr) + 1));
	  strcpy (extUseId, ptr);
	}
      strcpy (docURL, extUseUri);
      s = TtaStrdup (docURL);
      if (DocumentURLs[externalDoc] != NULL)
	{
	  TtaFreeMemory (DocumentURLs[externalDoc]);
	  DocumentURLs[externalDoc] = NULL;
	}
      DocumentURLs[externalDoc] = s;
    }
  else
    strcpy (docURL, fileName);
  
  /* Initialize global counters */
  extraLineRead = 0;
  extraOffset = 0;
  htmlLineRead = 0;
  htmlCharRead = 0;
  
  /* When we parse an external xml file, we ignore comments and PIs */
  /* (otherwise they are displayed in structure view) */
  /* and we don't report parsing errors */
  IgnoreCommentAndPi = TRUE;
  ShowParsingErrors = FALSE;

  /* Expat initialization */
  charset = TtaGetDocumentCharset (doc);
  /* For XML documents, the default charset is ISO_8859_1 */
  if (charset == UNDEFINED_CHARSET && !DocumentMeta[doc]->xmlformat)
    charset = ISO_8859_1;
  InitializeExpatParser (charset);
 
  /* Check if there is an xml declaration with a charset declaration */
  if (docURL[0] != EOS)
    CheckDocHeader (docURL, &xmlDec, &docType, &isXML, &xmlns,
		    &parsingLevel, &charset, charsetname, &thotType);
  
  /* Parse the input file and complete the Thot document */
  infile = gzopen (docURL, "r");
  if (infile != 0)
    {
      while (!endOfFile && !XMLNotWellFormed)
	{
	  /* read the XML file */
	  res = gzread (infile, bufferRead, COPY_BUFFER_SIZE);      
	  if (res < COPY_BUFFER_SIZE)
	    endOfFile = TRUE;
	  i = 0;
	  if (!docType)
	    /* There is no DOCTYPE Declaration 
	       We include a virtual DOCTYPE declaration so that EXPAT parser
	       doesn't stop processing when it finds an external entity */	  
	    {
	      if (xmlDec)
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
		  VirtualDoctype = TRUE;
		  tmpLineRead = XML_GetCurrentLineNumber (Parser);
		  if (!XML_Parse (Parser, DECL_DOCTYPE, DECL_DOCTYPE_LEN, 0))
		    XmlParseError (errorNotWellFormed,
				   (char *) XML_ErrorString (XML_GetErrorCode (Parser)), 0);
		  docType = TRUE;
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

  /* Free expat parser */ 
  FreeXmlParserContexts ();
  FreeExpatParser ();

  /* Handle character-level elements which contain block-level elements */
  if ((schemaName != NULL) &&
      (strcmp (schemaName, "HTML") == 0))
    {
      TtaSetStructureChecking (0, doc);
      CheckBlocksInCharElem (doc);
      TtaSetStructureChecking (1, doc);
    }

  if (use_ref && externalDoc != doc)
    {
      /* Copy the target element of the external document */
      /* as a sub-tree of the element extEl in the source document */
      /* Search the target element */
      extAttrType.AttrSSchema = TtaGetSSchema ("SVG", externalDoc);
      if (extAttrType.AttrSSchema)
	/* This document uses the SVG DTD */
	{
          extAttrType.AttrTypeNum = SVG_ATTR_id;
	  idEl = GetElemWithAttr (externalDoc, extAttrType, extUseId, NULL);
	}
      /* Copy and insert the sub-tree */
      if (idEl != NULL)
	{
	  copyEl = TtaCopyTree (idEl, externalDoc, doc, extEl);
	  if (copyEl != NULL)
	    TtaInsertFirstChild (&copyEl, extEl, doc);
	}
      /* Remove the ParsingErrors file */
      RemoveParsingErrors (externalDoc);

      /* Delete the external document */
      FreeDocumentResource (externalDoc);
      TtaCloseDocument (externalDoc);
    }
  
  if (extEl != NULL)
    {
      /* Fetch and display the recursive images */
      /* modify the net status */
      /* DocNetworkStatus[doc] = AMAYA_NET_ACTIVE; */
      FetchAndDisplayImages (doc, AMAYA_LOAD_IMAGE, extEl);
      /* Make not editable the external SVG image */
      TtaSetAccessRight (extEl, ReadOnly, doc);
    }

  if (docURL != NULL)
    {
      TtaFreeMemory (docURL);
      docURL = NULL;
    }

  if (extUseUri != NULL)
      TtaFreeMemory (extUseUri);
  if (extUseId != NULL)
      TtaFreeMemory (extUseId);
      
  /* Restore the display mode */
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, dispMode);

  return (!XMLNotWellFormed);
}

/*----------------------------------------------------------------------
   ParseXmlBuffer
   Parse a XML sub-tree given in a buffer and complete the
   corresponding Thot abstract tree.
   Return TRUE if the parsing of the buffer has no error.
  ----------------------------------------------------------------------*/
ThotBool       ParseXmlBuffer (char     *xmlBuffer,
			       Element   el,
			       ThotBool  isclosed,
			       Document  doc,
			       Language  lang,
			       char     *DTDname)
{
  int          tmpLen = 0;
  char        *transBuffer = NULL;
  char        *schemaName = NULL;
  ElementType  elType;
  Element      parent;
  CHARSET      charset;
  DisplayMode  dispMode;
  
  if (xmlBuffer == NULL)
    return FALSE;

  /* avoid too many redisplay */
  dispMode = TtaGetDisplayMode (doc);
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, DeferredDisplay);

  /* Initialize all parser contexts */
  if (firstParserCtxt == NULL)
    InitXmlParserContexts ();
 
  /* general initialization */
  RootElement = NULL;
  if (isclosed)
    {
      parent = TtaGetParent (el);
      elType = TtaGetElementType (parent);
    }
  else
    elType = TtaGetElementType (el);
  schemaName = TtaGetSSchemaName(elType.ElSSchema);
  InitializeXmlParsingContext (doc, el, isclosed, TRUE);
  ChangeXmlParserContextByDTD (schemaName);

  /* specific Initialization */
  XMLcontext.language = lang;
  DocumentSSchema = TtaGetDocumentSSchema (doc);

  /* Expat initialization */
  charset = TtaGetDocumentCharset (doc);
  /* For XML documents, the default charset is ISO_8859_1 */
  if (charset == UNDEFINED_CHARSET && !DocumentMeta[doc]->xmlformat)
    charset = ISO_8859_1;
  InitializeExpatParser (charset);
 
  /* We are parsing the result of a transformation */
  /* Parse a virtual DOCTYPE */
  VirtualDoctype = TRUE;
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
  
  /* Free expat parser */ 
  FreeXmlParserContexts ();
  FreeExpatParser ();

  /* Handle character-level elements which contain block-level elements */
  if ((schemaName != NULL) &&
      (strcmp (schemaName, "HTML") == 0))
    {
      TtaSetStructureChecking (0, doc);
      CheckBlocksInCharElem (doc);
      TtaSetStructureChecking (1, doc);
    }
      
  /* Restore the display mode */
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, dispMode);

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
    ChangeXmlParserContextByDTD (DTDname);
  else
    {
      elType = TtaGetElementType (XMLcontext.lastElement);
      schemaName = TtaGetSSchemaName(elType.ElSSchema);
      ChangeXmlParserContextByDTD (schemaName);
    }

  /* Parse a virtual DOCTYPE */
  VirtualDoctype = TRUE;
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
static void   XmlParse (FILE     *infile, CHARSET charset,
			ThotBool *xmlDec,
			ThotBool *xmlDoctype)
{
#define	 COPY_BUFFER_SIZE	1024
   char         bufferRead[COPY_BUFFER_SIZE + 1];
   char        *buffer;
   int          i;
   int          res;
   int          tmpLineRead = 0;
   ThotBool     endOfFile = FALSE, okay;
  
   if (infile != NULL)
       endOfFile = FALSE;
   else
       return;

   /* Initialize global counters */
   extraLineRead = 0;
   extraOffset = 0;
   htmlLineRead = 0;
   htmlCharRead = 0;
   /* add a null character at the end of the buffer by security */
   bufferRead[COPY_BUFFER_SIZE] = EOS;
   while (!endOfFile && !XMLNotWellFormed)
     {
       /* read the XML file */
       res = gzread (infile, bufferRead, COPY_BUFFER_SIZE);
       if (res < COPY_BUFFER_SIZE)
	 {
	   endOfFile = TRUE;
	   /* add a null character at the end of the buffer by security */
	   bufferRead[res] = EOS;
	 }
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
	       VirtualDoctype = TRUE;
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
	   if (charset == ISO_8859_2   || charset == ISO_8859_3   ||
	       charset == ISO_8859_4   || charset == ISO_8859_5   ||
	       charset == ISO_8859_6   || charset == ISO_8859_7   ||
	       charset == ISO_8859_8   || charset == ISO_8859_9   ||
	       charset == ISO_8859_15  || charset == KOI8_R       ||
	       charset == WINDOWS_1250 || charset == WINDOWS_1251 ||
	       charset == WINDOWS_1252 || charset == WINDOWS_1253 ||
	       charset == WINDOWS_1254 || charset == WINDOWS_1255 ||
	       charset == WINDOWS_1256 || charset == WINDOWS_1257 ||
	       charset == ISO_2022_JP  || charset == EUC_JP       ||
	       charset == SHIFT_JIS)
	     {
	       /* convert the original stream into UTF-8 */
	       buffer = TtaConvertByteToMbs (&bufferRead[i], charset);
	       if (buffer)
		 {
		   okay = XML_Parse (Parser, buffer, strlen (buffer), endOfFile);
		   TtaFreeMemory (buffer);
		 }
	     }
	   else
	     okay = XML_Parse (Parser, &bufferRead[i], res, endOfFile);
	   if (!okay)
	     XmlParseError (errorNotWellFormed,
			    (char *) XML_ErrorString (XML_GetErrorCode (Parser)), 0);
	 }
     }
}

/*------------------------------------------------------------------------------
   StartXmlParser 
   Loads the file Directory/xmlFileName for displaying the document documentName.
   The parameter pathURL gives the original (local or  distant) path
   or URL of the xml document.
  ------------------------------------------------------------------------------*/
void StartXmlParser (Document doc, char *fileName,
		     char *documentName, char *documentDirectory,
		     char *pathURL, ThotBool xmlDec, ThotBool xmlDoctype)
{
  Element         el, oldel;
  char            tempname[MAX_LENGTH];
  char            temppath[MAX_LENGTH];
  char           *s;
  int             error;
  ThotBool        isXHTML;
  CHARSET         charset;
#ifdef XML_GENERIC
  ThotBool        isXml = FALSE;
#endif /* XML_GENERIC */

  /* General initialization */
#ifdef ANNOTATIONS
  if (DocumentTypes[doc] == docAnnot)
    {
      /* we search the start of HTML Root element in the annotation struct */
      RootElement = ANNOT_GetHTMLRoot (doc, FALSE);
    }
  else
#endif /* ANNOTATIONS */
    RootElement = TtaGetMainRoot (doc);

  InitializeXmlParsingContext (doc, RootElement, FALSE, FALSE);

  /* Specific Initialization */
  XMLcontext.language = TtaGetDefaultLanguage ();
#ifdef ANNOTATIONS
  if (DocumentTypes[doc] == docAnnot)
    DocumentSSchema = ANNOT_GetBodySSchema (doc);
  else
#endif /* ANNOTATIONS */
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
      docURL = TtaGetMemory (strlen (pathURL) + 1);
      strcpy (docURL, pathURL);

      /* Do not check the Thot abstract tree against the structure */
      /* schema while building the Thot document. */
      /* Some valid XHTML documents could be considered as invalid Thot documents */
      /* For example, a <tbody> as a child of a <table> would be considered */
      /* invalid because the Thot SSchema requires a Table_body element in between */
      TtaSetStructureChecking (0, doc);

      /* Set the notification mode for the new document */
      TtaSetNotificationMode (doc, 1);

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
      isXHTML = FALSE;
      s = TtaGetSSchemaName (DocumentSSchema);
      if (strcmp (s, "HTML") == 0)
	{
	  ChangeXmlParserContextByDTD ("HTML");
	  isXHTML = TRUE;
	}
      else if (strcmp (s, "SVG") == 0)
	ChangeXmlParserContextByDTD ("SVG");
      else if (strcmp (s, "MathML") == 0)
	ChangeXmlParserContextByDTD ("MathML");
      else
#ifdef XML_GENERIC
	{
	  ChangeXmlParserContextByDTD ("XML");
	  isXml = TRUE;
	}
#else /* XML_GENERIC */
      	ChangeXmlParserContextByDTD ("HTML");
#endif /* XML_GENERIC */

      /* Gets the document charset */
      charset = TtaGetDocumentCharset (doc);
      /* Specific initialization for Expat */
      InitializeExpatParser (charset);
      /* Parse the input file and build the Thot tree */
      XmlParse (stream, charset, &xmlDec, &xmlDoctype);
      /* Completes all unclosed elements */
      if (currentParserCtxt != NULL)
	{
	  el = XMLcontext.lastElement;
	  while (el != NULL)
	    {
		(*(currentParserCtxt->ElementComplete)) (el, XMLcontext.doc, &error);
		el = TtaGetParent (el);
	    }
	}
      /* Check the Thot abstract tree for XHTML documents */
      if (isXHTML)
	{
	  CheckAbstractTree (pathURL, XMLcontext.doc);
	  if (SNumbering[doc])
	    ChangeAttrOnRoot (doc, HTML_ATTR_SectionNumbering);
	  if (MapAreas[doc])
	    ChangeAttrOnRoot (doc, HTML_ATTR_ShowAreas);
	}

      FreeExpatParser ();
      FreeXmlParserContexts ();
      gzclose (stream);
      if (docURL != NULL)
	{
	  TtaFreeMemory (docURL);
	  docURL = NULL;
	}

      /* Load specific user style */
      LoadUserStyleSheet (doc);

      TtaSetDisplayMode (doc, DisplayImmediately);

      /* Check the Thot abstract tree against the structure schema. */
      TtaSetStructureChecking (1, doc);
      DocumentSSchema = NULL;
    }
  TtaSetDocumentUnmodified (doc);

#ifdef XML_GENERIC
  if (isXml)
    /* While save procedure is not yet finished,
       set XML documents in read-only mode */
    ChangeToBrowserMode (doc);
#endif /* XML_GENERIC */

}

/* end of module */
