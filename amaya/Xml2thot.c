/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2009
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
#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"
#include "parser.h"
#include "zlib.h"
#include "MathML.h"
#include "fetchHTMLname.h"
#include "document.h"

#include "AHTURLTools_f.h"
#include "EDITstyle_f.h"
#include "HTMLactions_f.h"
#include "HTMLedit_f.h"
#include "HTMLimage_f.h"
#include "HTMLtable_f.h"
#include "HTMLimage_f.h"
#include "HTMLbook_f.h"
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

#include "Elemlist.h"

#ifdef TEMPLATES
#include "MENUconf.h"
#include "templates.h"
#include "Elemlist.h"
#include "Templatebuilder_f.h"
#include "templates_f.h"
#endif /* TEMPLATES */
#include "XLinkbuilder_f.h"
#ifdef ANNOTATIONS
#include "annotlib.h"
#include "ANNOTtools_f.h"
#endif /* ANNOTATIONS */

#include "expat.h"
#define NS_SEP '|'
#define NS_COLON ':'

/* ---------------------- static variables ---------------------- */

/* Expat parser identifier */
static XML_Parser  Parser = NULL;

/* global data used by the HTML parser */
static ParserData  XMLcontext = {0, UTF_8, 0, NULL, 0,
                                 FALSE, FALSE, FALSE, FALSE, FALSE, FALSE};

/* a parser context. It describes the specific actions to be executed
   when parsing an XML document fragment according to a given DTD */
typedef struct _XMLparserContext *PtrParserCtxt;
typedef struct _XMLparserContext
{
  char	  *UriName;		/* URI of namespaces for that DTD */
  PtrParserCtxt  NextParserCtxt;	/* next parser context */
  char	  *SSchemaName;		/* name of Thot structure schema */
  SSchema  XMLSSchema;		/* the Thot structure schema */
  int      XMLtype;             /* indentifier used by fetchname */
  Proc	   MapAttribute;	/* returns the Thot attribute corresp.
                                   to an XML attribute name */
  Proc	   MapAttributeValue;	/* returns the Thot value corresp. to
                                 the name of an XML attribute value */    
  Proc	   CheckContext;        /* action to be called to verify if an
                                   element is allowed in the current
                                   structural context */
  Proc	   CheckInsert;         /* action to be called to insert an
                                   element in the abstract tree */
  Proc	   ElementCreated;	/* action to be called when an element
                                   has been created and inserted */
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
static PtrParserCtxt	FirstParserCtxt = NULL;
/* current context */
static PtrParserCtxt	CurrentParserCtxt = NULL;
/* XHTML context */
static PtrParserCtxt	XhtmlParserCtxt = NULL;
/* Generic XML context */
static PtrParserCtxt	GenericXmlParserCtxt = NULL;

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

/* XML Style Sheets table */
static char         *XML_CSS_Href[MAX_NS_TABLE]; 
static Document      XML_CSS_Doc[MAX_NS_TABLE]; 
static Element       XML_CSS_El[MAX_NS_TABLE]; 
static CSSmedia      XML_CSS_Media[MAX_NS_TABLE];
static int           XML_CSS_Level = 0;

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
/* save the docURL for some cases of parsing errors */
static char         *docURL2 = NULL;

/* information about the Thot document under construction */
/* Document structure schema */
static SSchema       DocumentSSchema = NULL;
/* root element of the document */
static Element       RootElement;
/* name of the root element */
static char         *XMLRootName;
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
/* the content of a buffer is being parsed */
static ThotBool      PARSING_BUFFER = FALSE;
/* parsing errors are not reported for external resources */
static ThotBool      ShowParsingErrors = FALSE;;
static ThotBool	     ParsingSubTree = FALSE;
static ThotBool	     ImmediatelyAfterTag = FALSE;
static ThotBool	     HTMLStyleAttribute = FALSE;
static ThotBool	     XMLSpaceAttribute = FALSE;
static ThotBool      ParsingCDATA = FALSE;
static char	     currentElementContent = ' ';
static char	     currentElementName[100];

/* Global variable to handle white-space in XML documents */
static ThotBool      RemoveLineBreak = FALSE;
static ThotBool      RemoveLeadingSpace = FALSE;   
static ThotBool      RemoveTrailingSpace = FALSE;
static ThotBool      RemoveContiguousSpace = FALSE;

/* "Extra" counters for the characters and the lines read */
static int           ExtraLineRead = 0;
static int           ExtraOffset = 0;
static int           HtmlLineRead = 0;
static int           HtmlCharRead = 0;

/* Virtual DOCTYPE Declaration */
#define DECL_DOCTYPE "<!DOCTYPE html PUBLIC \"\" \"\">\n"
#define DECL_DOCTYPE_LEN 29
#define DECL_XML "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n"
#define DECL_XML_LEN 44
static ThotBool      VirtualDoctype = FALSE;

/* maximum size of error messages */
#define MaxMsgLength 200

static void StartOfXmlStartElement (const char *name);
static void DisableExpatParser ();
static void XhtmlCheckInsert (Element *el, Element parent, Document doc,
                              ThotBool *inserted);
static void XmlCheckInsert (Element *el, Element parent, Document doc,
                            ThotBool *inserted);
static void XhtmlCheckContext (char *elName, const ElementType * elType,
                               ThotBool *isAllowed);
static void XmlCheckContext (char *elName, const ElementType *elType,
                             ThotBool *isAllowed);
static void XmlParse (FILE *infile, CHARSET charset, ThotBool *xmlDec,
                      ThotBool *xmlDoctype);

/*----------------------------------------------------------------------
  ChangeXmlParserContextByDTD
  Get the parser context correponding to a given DTD
  ----------------------------------------------------------------------*/
static void ChangeXmlParserContextByDTD (const char *DTDname)
{
  CurrentParserCtxt = FirstParserCtxt;
  while (CurrentParserCtxt != NULL &&
         strcmp ((char *)DTDname, CurrentParserCtxt->SSchemaName))
    CurrentParserCtxt = CurrentParserCtxt->NextParserCtxt;

  /* initialize the corresponding entry */
  if (CurrentParserCtxt != NULL &&
      CurrentParserCtxt != GenericXmlParserCtxt &&
      CurrentParserCtxt->XMLSSchema == NULL)
    {
      CurrentParserCtxt->XMLSSchema = 
        GetXMLSSchema (CurrentParserCtxt->XMLtype, XMLcontext.doc);
      TtaSetUriSSchema (CurrentParserCtxt->XMLSSchema, CurrentParserCtxt->UriName);
    }
}

/*----------------------------------------------------------------------
  ChangeXmlParserContextByUri
  Get the parser context correponding to a given namespace uri
  ----------------------------------------------------------------------*/
static ThotBool ChangeXmlParserContextByUri (char *uriName)

{
  ThotBool  found = FALSE;

  CurrentParserCtxt = FirstParserCtxt;

  while (!found && CurrentParserCtxt)
    {
      if (CurrentParserCtxt->UriName == NULL)
        found = uriName == NULL;
      else if (!strcmp (CurrentParserCtxt->UriName, Template_URI))
        {
          /* Templates */
          if (!strcmp ((char *)uriName, Template_URI) ||
              !strcmp ((char *)uriName, Template_URI_o) ||
              !strcmp ((char *)uriName, Template_URI_f))
            found = TRUE;
        }
      else if (!strcmp ((char *)uriName, CurrentParserCtxt->UriName))
        found = TRUE;
      if (!found)
        CurrentParserCtxt = CurrentParserCtxt->NextParserCtxt;
    }

  /* Initialize the corresponding Thot schema */
  if (CurrentParserCtxt != NULL &&
      CurrentParserCtxt != GenericXmlParserCtxt &&
      CurrentParserCtxt->XMLSSchema == NULL)
    {
      CurrentParserCtxt->XMLSSchema = 
        GetXMLSSchema (CurrentParserCtxt->XMLtype, XMLcontext.doc);
      TtaSetUriSSchema (CurrentParserCtxt->XMLSSchema, CurrentParserCtxt->UriName);
      return TRUE;
    }
  return FALSE;
}

/*----------------------------------------------------------------------
  InitXmlParserContexts
  Create the chain of parser contexts decribing all recognized XML DTDs
  ----------------------------------------------------------------------*/
static void    InitXmlParserContexts (void)

{
  PtrParserCtxt   ctxt, prevCtxt;

  FirstParserCtxt = NULL;
  prevCtxt = NULL;
  ctxt = NULL;

  /* create and initialize a context for XHTML */
  ctxt = (XMLparserContext*)TtaGetMemory (sizeof (XMLparserContext));
  if (prevCtxt == NULL)
    FirstParserCtxt = ctxt;
  else
    prevCtxt->NextParserCtxt = ctxt;
  ctxt->NextParserCtxt = NULL;
  ctxt->SSchemaName = (char *)TtaGetMemory (NAME_LENGTH);
  strcpy ((char *)ctxt->SSchemaName, "HTML");
  ctxt->UriName = (char *)TtaGetMemory (MAX_URI_NAME_LENGTH);
  strcpy ((char *)ctxt->UriName, (char *)XHTML_URI);
  ctxt->XMLSSchema = NULL;
  ctxt->XMLtype = XHTML_TYPE;
  ctxt->MapAttribute = (Proc) MapHTMLAttribute;
  ctxt->MapAttributeValue = (Proc) MapHTMLAttributeValue;
  ctxt->CheckContext = (Proc) XhtmlCheckContext;
  ctxt->CheckInsert = (Proc) XhtmlCheckInsert;
  ctxt->ElementCreated = NULL;
  ctxt->ElementComplete = (Proc) XhtmlElementComplete;
  ctxt->AttributeComplete = NULL;
  ctxt->GetDTDName = NULL;
  ctxt->UnknownNameSpace = (Proc)UnknownXhtmlNameSpace;
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
  ctxt = (XMLparserContext*)TtaGetMemory (sizeof (XMLparserContext));
  if (prevCtxt == NULL)
    FirstParserCtxt = ctxt;
  else
    prevCtxt->NextParserCtxt = ctxt;
  ctxt->NextParserCtxt = NULL;
  ctxt->SSchemaName = (char *)TtaGetMemory (NAME_LENGTH);
  strcpy ((char *)ctxt->SSchemaName, "MathML");
  ctxt->UriName = (char *)TtaGetMemory (MAX_URI_NAME_LENGTH);
  strcpy ((char *)ctxt->UriName, (char *)MathML_URI);
  ctxt->XMLSSchema = NULL;
  ctxt->XMLtype = MATH_TYPE;
  ctxt->MapAttribute = (Proc) MapMathMLAttribute;
  ctxt->MapAttributeValue = (Proc) MapMathMLAttributeValue;
  ctxt->CheckContext = (Proc) XmlCheckContext;
  ctxt->CheckInsert = (Proc) XmlCheckInsert;
  ctxt->ElementCreated = (Proc) MathMLElementCreated;
  ctxt->ElementComplete = (Proc) MathMLElementComplete;
  ctxt->AttributeComplete = (Proc) MathMLAttributeComplete;
  ctxt->GetDTDName = (Proc) MathMLGetDTDName;
  ctxt->UnknownNameSpace = (Proc)UnknownMathMLNameSpace;
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
  ctxt = (XMLparserContext*)TtaGetMemory (sizeof (XMLparserContext));
  if (prevCtxt == NULL)
    FirstParserCtxt = ctxt;
  else
    prevCtxt->NextParserCtxt = ctxt;
  ctxt->NextParserCtxt = NULL;
  ctxt->SSchemaName = (char *)TtaGetMemory (NAME_LENGTH);
  strcpy ((char *)ctxt->SSchemaName, "SVG");
  ctxt->UriName = (char *)TtaGetMemory (MAX_URI_NAME_LENGTH);
  strcpy ((char *)ctxt->UriName, (char *)SVG_URI);
  ctxt->XMLSSchema = NULL;
  ctxt->XMLtype = SVG_TYPE;
  ctxt->MapAttribute = (Proc) MapSVGAttribute;
  ctxt->MapAttributeValue = (Proc) MapSVGAttributeValue;
  ctxt->CheckContext = (Proc) XmlCheckContext;
  ctxt->CheckInsert = (Proc) SVGCheckInsert;
  ctxt->ElementCreated =  (Proc) SVGElementCreated;
  ctxt->ElementComplete = (Proc) SVGElementComplete;
  ctxt->AttributeComplete = (Proc) SVGAttributeComplete;
  ctxt->GetDTDName = (Proc) SVGGetDTDName;
  ctxt->UnknownNameSpace = (Proc)UnknownSVGNameSpace;
  ctxt->DefaultLineBreak = TRUE;
  ctxt->DefaultLeadingSpace = TRUE;   
  ctxt->DefaultTrailingSpace = TRUE;  
  ctxt->DefaultContiguousSpace = TRUE;
  ctxt->PreserveLineBreak = TRUE;    
  ctxt->PreserveLeadingSpace = FALSE;   
  ctxt->PreserveTrailingSpace = FALSE;  
  ctxt->PreserveContiguousSpace = FALSE;
  prevCtxt = ctxt;

#ifdef TEMPLATES
  /* create and initialize a context for Templates */
  ctxt = (XMLparserContext*)TtaGetMemory (sizeof (XMLparserContext));
  if (prevCtxt == NULL)
    FirstParserCtxt = ctxt;
  else
    prevCtxt->NextParserCtxt = ctxt;
  ctxt->NextParserCtxt = NULL;
  ctxt->SSchemaName = (char *)TtaGetMemory (NAME_LENGTH);
  strcpy ((char *)ctxt->SSchemaName, "Template");
  ctxt->UriName = (char *)TtaGetMemory (MAX_URI_NAME_LENGTH);
  strcpy ((char *)ctxt->UriName, (char *)Template_URI);
  ctxt->XMLSSchema = NULL;
  ctxt->XMLtype = Template_TYPE;
  ctxt->MapAttribute = (Proc) MapTemplateAttribute;
  ctxt->MapAttributeValue = (Proc) MapTemplateAttributeValue;
  ctxt->CheckContext = (Proc) XmlCheckContext;
  ctxt->CheckInsert = (Proc) TemplateCheckInsert;
  ctxt->ElementCreated = NULL;
  ctxt->ElementComplete = (Proc) TemplateElementComplete;
  ctxt->AttributeComplete = (Proc) TemplateAttributeComplete;
  ctxt->GetDTDName = (Proc) TemplateGetDTDName;
  ctxt->UnknownNameSpace = (Proc)UnknownTemplateNameSpace;
  ctxt->DefaultLineBreak = TRUE;
  ctxt->DefaultLeadingSpace = TRUE;   
  ctxt->DefaultTrailingSpace = TRUE;  
  ctxt->DefaultContiguousSpace = TRUE;
  ctxt->PreserveLineBreak = TRUE;    
  ctxt->PreserveLeadingSpace = FALSE;   
  ctxt->PreserveTrailingSpace = FALSE;  
  ctxt->PreserveContiguousSpace = FALSE;
  prevCtxt = ctxt;
#endif /* TEMPLATES */

  /* create and initialize a context for XLink */
  ctxt = (XMLparserContext*)TtaGetMemory (sizeof (XMLparserContext));
  if (prevCtxt == NULL)
    FirstParserCtxt = ctxt;
  else
    prevCtxt->NextParserCtxt = ctxt;
  ctxt->NextParserCtxt = NULL;	/* last context */
  ctxt->SSchemaName = (char *)TtaGetMemory (NAME_LENGTH);
  strcpy ((char *)ctxt->SSchemaName, "XLink");
  ctxt->UriName = (char *)TtaGetMemory (MAX_URI_NAME_LENGTH);
  strcpy ((char *)ctxt->UriName, (char *)XLink_URI);
  ctxt->XMLSSchema = NULL;
  ctxt->XMLtype = XLINK_TYPE;
  ctxt->MapAttribute = (Proc) MapXLinkAttribute;
  ctxt->MapAttributeValue = (Proc) MapXLinkAttributeValue;
  ctxt->CheckContext = (Proc) XmlCheckContext;
  ctxt->CheckInsert = (Proc) XmlCheckInsert;
  ctxt->ElementCreated = NULL;
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
  ctxt = (XMLparserContext*)TtaGetMemory (sizeof (XMLparserContext));
  if (prevCtxt == NULL)
    FirstParserCtxt = ctxt;
  else
    prevCtxt->NextParserCtxt = ctxt;
  ctxt->NextParserCtxt = NULL;	/* last context */
  ctxt->SSchemaName = (char *)TtaGetMemory (NAME_LENGTH);
  strcpy ((char *)ctxt->SSchemaName, "XML");
  ctxt->UriName = (char *)TtaGetMemory (MAX_URI_NAME_LENGTH);
  ctxt->UriName[0] = EOS;
  ctxt->XMLSSchema = NULL;
  ctxt->XMLtype = XML_TYPE;
  ctxt->MapAttribute = NULL;
  ctxt->MapAttributeValue = (Proc) MapXmlAttributeValue;
  ctxt->CheckContext =  (Proc) XmlCheckContext;
  ctxt->CheckInsert = (Proc) XmlCheckInsert;
  ctxt->ElementCreated = NULL;
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

  CurrentParserCtxt = NULL;

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
    lineNumber = XML_GetCurrentLineNumber (Parser) + HtmlLineRead - ExtraLineRead;
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
  int           pos, n;
  const char         *c;
  unsigned char       val;

  if (IgnoreErrors)
    return;
  if (!ShowParsingErrors)
    return;
  if (line == 0 && Parser == NULL)
    return;
  if (!ErrFile)
    if (OpenParsingErrors (XMLcontext.doc) == FALSE)
      return;
  
  if (docURL)
    {
      fprintf (ErrFile, "\n*** Errors/warnings in %s\n", docURL);
      TtaFreeMemory (docURL);
      docURL = NULL;
    }
  else
    {
      if (CSSErrorsFound && docURL2)
      	{
      	  fprintf (ErrFile, "\n*** Errors/warnings in %s\n", docURL2);
      	  TtaFreeMemory (docURL2);
      	  docURL2 = NULL;
      	}
    }
  
  switch (type)
    {
    case errorEncoding: 
      fprintf (ErrFile, "@  line 1, char 0:  %s\n", msg);
      XMLCharacterNotSupported = TRUE;
      break;
    case errorNotWellFormed:
      if (line == 0)
        {
          if (Parser != NULL)
            {
              line = XML_GetCurrentLineNumber (Parser) + HtmlLineRead -  ExtraLineRead;
              /* check if expat found an invalid utf-8 character or an error
                 in an attribute value or an invalid entity */
              c = XML_GetInputContext (Parser, &pos, &n);
              if (c) 
                val = (unsigned char)(c[pos]);
              else
                val = EOS;
              if (strstr ((char *)msg, "invalid token") && val > 127)
                XMLInvalidToken = TRUE;
              else
                XMLNotWellFormed = TRUE;
              fprintf (ErrFile, "@  line %d, char %d: %s\n", line,
                       (int)XML_GetCurrentColumnNumber (Parser), msg);
            }
        }
      else
        {
          fprintf (ErrFile, "@  line %d: %s\n", line, msg);
          XMLNotWellFormed = TRUE;
        }
      break;
    case errorCharacterNotSupported:
      if (line == 0)
        {
          if (Parser != NULL)
            {
              fprintf (ErrFile, "@  line %d, char %d: %s\n",
                       (int)XML_GetCurrentLineNumber (Parser) + HtmlLineRead -  ExtraLineRead,
                       (int)XML_GetCurrentColumnNumber (Parser),
                       msg);
            }
        }
      else
        fprintf (ErrFile, "@  line %d: %s\n", line, msg); 
      XMLCharacterNotSupported = TRUE;
      break;
    case errorParsing:
      XMLErrorsFound = TRUE;
    case warningMessage:
      if (line == 0)
        {
          if (Parser != NULL)
            {
              fprintf (ErrFile, "@  line %d, char %d: %s\n",
                       (int)XML_GetCurrentLineNumber (Parser) + HtmlLineRead -  ExtraLineRead,
                       (int)XML_GetCurrentColumnNumber (Parser),
                       msg);
            }
        }
      else
        fprintf (ErrFile, "@  line %d: %s\n", line, msg);
      break;
    case errorParsingProfile:
      if (line == 0)
        {
          if (Parser != NULL)
            {
              fprintf (ErrFile, "@  line %d, char %d: %s\n",
                       (int)XML_GetCurrentLineNumber (Parser) + HtmlLineRead -  ExtraLineRead,
                       (int)XML_GetCurrentColumnNumber (Parser),
                       msg);
            }
        }
      else
        fprintf (ErrFile, "@  line %d: %s\n", line, msg); 
      XMLErrorsFoundInProfile = TRUE;
      break;
    case undefinedEncoding:
      fprintf (ErrFile, "@  %s\n", msg);
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
  SetParsingScript
  Sets the value of ParsingScript boolean.
  ----------------------------------------------------------------------*/
void  SetParsingScript (ThotBool value)

{
  XMLcontext.parsingScript = value;
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

  if (CurrentParserCtxt == NULL)
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
              RemoveLineBreak = CurrentParserCtxt->DefaultLineBreak;
              RemoveLeadingSpace = CurrentParserCtxt->DefaultLeadingSpace;   
              RemoveTrailingSpace = CurrentParserCtxt->DefaultTrailingSpace;  
              RemoveContiguousSpace = CurrentParserCtxt->DefaultContiguousSpace;
            }
          else
            {
              RemoveLineBreak = CurrentParserCtxt->PreserveLineBreak;
              RemoveLeadingSpace = CurrentParserCtxt->PreserveLeadingSpace;   
              RemoveTrailingSpace = CurrentParserCtxt->PreserveTrailingSpace;  
              RemoveContiguousSpace = CurrentParserCtxt->PreserveContiguousSpace;
            }
        }
    }
  
  if (!found)
    {
      RemoveLineBreak = CurrentParserCtxt->DefaultLineBreak;
      RemoveLeadingSpace = CurrentParserCtxt->DefaultLeadingSpace;   
      RemoveTrailingSpace = CurrentParserCtxt->DefaultTrailingSpace;  
      RemoveContiguousSpace = CurrentParserCtxt->DefaultContiguousSpace;
    }
}

/*----------------------------------------------------------------------
  XmlWhiteInStack
  The last element in stack has a xml:space attribute
  (or it is a pre, style, textarea or script element in XHTML)
  ----------------------------------------------------------------------*/
static void    XmlWhiteSpaceInStack (char  *attrValue)

{
  if (attrValue == NULL)
    spacePreservedStack[stackLevel-1] = 'P';
  else
    {
      if ((strcmp ((char *)attrValue, "default") == 0))
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
  InsertingSibling
  Return TRUE if the new element must be inserted in the Thot document
  as a sibling of lastElement;
  Return FALSE if it must be inserted as a child.
  ----------------------------------------------------------------------*/
static ThotBool InsertingSibling ()
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
      TtaSetTextContent (elLeaf, (unsigned char *)fallback, lang, XMLcontext.doc);
    }
   
  /* Associate an attribute EntityName with the new leaf */
#ifdef TEST
  /* Old treatment to generate an 'entityName' attribute, useless now */
  attrType.AttrSSchema = elType.ElSSchema;
  ptr = (unsigned char*)TtaGetSSchemaName (elType.ElSSchema);
  if (strcmp ((char *)ptr, "MathML") == 0)
    attrType.AttrTypeNum = MathML_ATTR_EntityName;
  else if (strcmp ((char *)ptr, "HTML") == 0)
    attrType.AttrTypeNum = HTML_ATTR_EntityName;
  else
    attrType.AttrTypeNum = HTML_ATTR_EntityName;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (elLeaf, attr, XMLcontext.doc);

  if (entityName)
    /* store the given entity name */
    TtaSetAttributeText (attr, (char *)entityName, elLeaf, XMLcontext.doc);
  else
    {
      /* it's a numerical entity */
      len = sprintf ((char *)buffer, "%d", (int) wcharRead);
      i = 0;
      bufName[i++] = START_ENTITY;
      bufName[i++] = '#';
      for (j = 0; j < len; j++)
        bufName[i++] = buffer[j];
      bufName[i++] = ';';
      bufName[i] = EOS;
      TtaSetAttributeText (attr, (char *)bufName, elLeaf, XMLcontext.doc);
    }
#endif
  /* genete the 'entityName' attribute for MathML only */
  attrType.AttrSSchema = elType.ElSSchema;
  ptr = (unsigned char*)TtaGetSSchemaName (elType.ElSSchema);
  if (strcmp ((char *)ptr, "MathML") == 0)
    {
      attrType.AttrTypeNum = MathML_ATTR_EntityName;
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (elLeaf, attr, XMLcontext.doc);
      if (entityName)
	/* store the given entity name */
	TtaSetAttributeText (attr, (char *)entityName, elLeaf, XMLcontext.doc);
      else
	{
	  /* it's a numerical entity */
	  len = sprintf ((char *)buffer, "%d", (int) wcharRead);
	  i = 0;
	  bufName[i++] = START_ENTITY;
	  bufName[i++] = '#';
	  for (j = 0; j < len; j++)
	    bufName[i++] = buffer[j];
	  bufName[i++] = ';';
	  bufName[i] = EOS;
	  TtaSetAttributeText (attr, (char *)bufName, elLeaf, XMLcontext.doc);
	}
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
  int           profile;
  char          msgBuffer[MaxMsgLength];
  char         *typeName;

  if (parent == NULL)
    return;
   
  elType = TtaGetElementType (*el);
  typeName = TtaGetElementTypeName (elType);

  if (elType.ElTypeNum == HTML_EL_TEXT_UNIT || 
      elType.ElTypeNum == HTML_EL_BR ||
      elType.ElTypeNum == HTML_EL_PICTURE_UNIT ||
      elType.ElTypeNum == HTML_EL_Input ||
      elType.ElTypeNum == HTML_EL_Text_Area ||
      IsXMLElementInline (elType, doc))
    {
      /* the element to be inserted is a character string */
      /* Search the ancestor that is not a character level element */
      ancestor = parent;
      ancestorType = TtaGetElementType (ancestor);
      while (ancestor &&
             !strcmp (TtaGetSSchemaName (ancestorType.ElSSchema), "Template"))
        {
          // skip template ancestors
          ancestor =  TtaGetParent (ancestor);
          ancestorType = TtaGetElementType (ancestor);
        }
      if (strcmp (TtaGetSSchemaName (ancestorType.ElSSchema), "HTML"))
        /* parent is not a HTML element */
        return;

      while (ancestor &&
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
              profile = TtaGetDocumentProfile (XMLcontext.doc);
              if ((profile == L_Basic || profile == L_Strict) &&
                  !strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML") &&
                  elType.ElTypeNum == HTML_EL_BODY)
                {
                  snprintf ((char *)msgBuffer, MaxMsgLength,
                           "Element <%s> not allowed outside a block Element - <p> forced", typeName);
                  XmlParseError (errorParsing, (unsigned char *)msgBuffer, 0);
                  newElType.ElTypeNum = HTML_EL_Paragraph;
                }
              else
                {
                  newElType.ElTypeNum = HTML_EL_Pseudo_paragraph;
                }
              /* Element ancestor cannot contain text directly. Create a */
              /* Pseudo_paragraph element as the parent of the text element */
              newElType.ElSSchema = XhtmlParserCtxt->XMLSSchema;
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
            newElType.ElSSchema = CurrentParserCtxt->XMLSSchema;
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
  Inserts an element el in the Thot abstract tree at the current position.
  ---------------------------------------------------------------------------*/
void InsertXmlElement (Element *el)
{
  Element       parent;
  ThotBool      inserted = FALSE;

  if (CurrentParserCtxt != NULL)
    {
      if (InsertingSibling ())
        {
          if (XMLcontext.lastElement == NULL)
            parent = NULL;
          else
            parent = TtaGetParent (XMLcontext.lastElement);
          (*((Proc4)CurrentParserCtxt->CheckInsert)) ((void *)el,
                                                      (void *)parent,
                                                      (void *)XMLcontext.doc,
                                                      (void *)&inserted);
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
          (*((Proc4)CurrentParserCtxt->CheckInsert)) (
                                                      (void *)el,
                                                      (void *)XMLcontext.lastElement,
                                                      (void *)XMLcontext.doc,
                                                      (void *)&inserted);
          if (!inserted)
            TtaInsertFirstChild (el, XMLcontext.lastElement, XMLcontext.doc);
        }

      if (*el)
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
Element XmlLastLeafInElement (Element el)

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
  ----------------------------------------------------------------------*/
static void RemoveTrailingSpaces (Element el)
{
  int           length;
  ElementType   elType;
  Element       lastLeaf, lastChild;

  /* Search the last leaf in the element's tree */
  lastLeaf = XmlLastLeafInElement (el);   
  if (lastLeaf)
    {
      elType = TtaGetElementType (lastLeaf);
      if (elType.ElTypeNum == 1)
        /* the last leaf is a TEXT element */
        {
          length = TtaGetElementVolume (lastLeaf);
          if (length > 0)
            TtaRemoveFinalSpaces (lastLeaf, XMLcontext.doc,
                                  RemoveTrailingSpace);
        }
    }

  /* create an empty text element after the math element */
  if (CurrentParserCtxt &&
      strcmp ((char *)CurrentParserCtxt->SSchemaName, "HTML") == 0)
    {
      lastChild = TtaGetLastChild (el);
      if (lastChild)
        {
          elType = TtaGetElementType (lastChild);
          if (elType.ElTypeNum == MathML_EL_MathML &&
              !strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML"))
            /* the last child of this HTML element is a <math> element */
            {
              /* create an empty text element after the math element */
              elType.ElSSchema = CurrentParserCtxt->XMLSSchema;
              elType.ElTypeNum = HTML_EL_TEXT_UNIT;
              lastLeaf = TtaNewElement (XMLcontext.doc, elType);
              TtaInsertSibling (lastLeaf, lastChild, FALSE, XMLcontext.doc);
            }
        }
    }
}

/*----------------------------------------------------------------------
  XmlCloseElement
  Terminate the corresponding Thot element.
  ----------------------------------------------------------------------*/
static ThotBool XmlCloseElement (char *mappedName)
{
  int              i, error;
  Element          el, parent, pseudo;
  ElementType      parentType, elType;
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
              if (InsertingSibling ())
                parent = TtaGetParent (XMLcontext.lastElement);
              else
                parent = XMLcontext.lastElement;
              if (parent != NULL)
                {
                  parentType = TtaGetElementType (parent);
                  if ((!strcmp (TtaGetSSchemaName (parentType.ElSSchema), "HTML")) && 
                      parentType.ElTypeNum == HTML_EL_Pseudo_paragraph)
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
	       
              /* Remove the trailing spaces for included pseudo-paragraph */
              elType = TtaGetElementType (el);
              if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML"))
                {
                  elType.ElTypeNum = HTML_EL_Pseudo_paragraph;
                  pseudo = el;
                  do
                    {
                      pseudo = TtaSearchTypedElementInTree (elType,
                                                            SearchForward, el, pseudo);
                      if (pseudo)
                        RemoveTrailingSpaces (pseudo);
                    }
                  while (pseudo);
                }
	       
              (*(Proc3)(CurrentParserCtxt->ElementComplete))(
                                                             (void *)&XMLcontext,
                                                             (void *)el,
                                                             (void *)&error);

              if (el == XMLcontext.lastElement)
                el = NULL;
              else
                el = TtaGetParent (el);
            }

          if (stackLevel > 1)
            {
              XMLcontext.language = languageStack[stackLevel - 1];
              CurrentParserCtxt = parserCtxtStack[stackLevel - 1];
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
  Called by the namespace declaratation start handler.
  Update the two namespace tables.
  ----------------------------------------------------------------------*/
static void NsDeclarationStart (char *ns_prefix, char *ns_uri)

{  
  if (Ns_Level >= MAX_NS_TABLE)
    {
      XmlParseError (errorNotWellFormed,
                     (unsigned char *)"**FATAL** Too many namespace declarations ", 0);
      DisableExpatParser ();
      return;
    }
  if (ns_prefix && Ns_Level == 0)
    {
      // declare a default namespace
      if (CurrentParserCtxt)
        Ns_Uri[Ns_Level] = TtaStrdup (CurrentParserCtxt->UriName);
      else
        Ns_Uri[Ns_Level] = NULL;
      Ns_Prefix[Ns_Level] = NULL;
      Ns_Level ++;
      CurNs_Level ++;      
    }
  /* Filling up the table of namespaces declared for the current element */
  CurNs_Prefix[CurNs_Level] = TtaStrdup (ns_prefix);
  CurNs_Uri[CurNs_Level] = TtaStrdup (ns_uri);
  CurNs_Level ++;

  /* Filling up the table of namespaces declared for the whole document */
  if (ns_uri)
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
  Called by the namespace declaratatin end handler.
  Remove the last declaration.
  ----------------------------------------------------------------------*/
static void NsDeclarationEnd (char *ns_prefix)
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
  
  CurNs_Level = 0;
}

/*----------------------------------------------------------------------
  NsStartProcessing
  Look for namespace declarations for the current element. 
  If there are such declarations, update the Document informations.
  Remove all current namespace declaration(s).
  ----------------------------------------------------------------------*/
static void NsStartProcessing (Element newElement, ThotBool declare)

{
  int i;

  if (CurNs_Level == 0)
    return;

  /* Update the Namespace Document informations and remove the useless declarations */
  for (i = 0; i < CurNs_Level; i++)
    {
      if (newElement && declare)
        TtaSetANamespaceDeclaration (XMLcontext.doc, newElement,
                                     CurNs_Prefix[i], CurNs_Uri[i]);
      if (CurNs_Prefix[i])
        {
          TtaFreeMemory (CurNs_Prefix[i]);
          CurNs_Prefix[i] = NULL;
        }
      if (CurNs_Uri[i])
        {
          TtaFreeMemory (CurNs_Uri[i]);
          CurNs_Uri[i] = NULL;
        }
    }
  CurNs_Level = 0; 
}

/*----------------------------------------------------------------------
  GetDefaultNsUri
  Return the default NS uri
  ----------------------------------------------------------------------*/
static char *GetDefaultNsUri (ThotBool *def_uri)

{  
  int    i;
  char  *uri;

  *def_uri = FALSE;
  uri = NULL;
  for (i = Ns_Level; i > 0; i--)
    {
      if (Ns_Prefix[i-1] == NULL)
        {
          uri = Ns_Uri[i-1];
          *def_uri = TRUE;
          i = 0;
        }
    } 
  return (uri);
}

/*----------------------------------------------------------------------
  NsGetPrefix
  Get the prefix associated with an uri
  ----------------------------------------------------------------------*/
static char *NsGetPrefix (const char *ns_uri)

{
  int     i;
  char   *ns_prefix;

  ns_prefix = NULL;
  if (ns_uri == NULL)
    return (ns_prefix);
  
  for (i = Ns_Level - 1; i >= 0; i--)
    {
      if ((ns_uri != NULL) && (Ns_Uri[i] != NULL) &&
          (strcmp ((char *)ns_uri, (char *)Ns_Uri[i]) == 0))
        {
          ns_prefix = Ns_Prefix[i];
          i = 0;
        }
    }
  return (ns_prefix);
}

/*----------------------------------------------------------------------
  UnknownXmlNsElement
  Create an Unknown_Namespace element according to the current context
  ----------------------------------------------------------------------*/
static void UnknownXmlNsElement (char *ns_uri, char *elemName, ThotBool startElem)
{
  Element     newElement;
  int         i, lg, tmplg;
  char       *ns_prefix;
#define       MAX_BUFFER_SIZE    1024
  char        elemBuffer[MAX_BUFFER_SIZE];

  if (elemName == NULL)
    return;

  strcpy ((char *)elemBuffer, "<");
  lg = 1;

  if (!startElem)
    {
      strcat (elemBuffer, "/");
      lg ++;
    }

  /* Is that namespace associated with a prefix ? */
  ns_prefix = NsGetPrefix (ns_uri);

  if (ns_prefix != NULL)
    {
      strcat (elemBuffer, ns_prefix);	  
      lg += strlen ((char *)ns_prefix);
      strcat (elemBuffer, ":");
      lg ++;
    }
  strcat (elemBuffer, elemName);	  
  lg += strlen ((char *)elemName);

  /* Search all namespace declarations for this element */
  if (startElem && CurNs_Level > 0)
    {
      for (i = 0; i < CurNs_Level; i++)
        {
          /* Check if we don't overflow the buffer */
          tmplg = 8;
          if (CurNs_Prefix[i] != NULL)
            {
              tmplg += strlen ((char *)CurNs_Prefix[i]);
              tmplg ++;
            }
          if (CurNs_Uri[i] != NULL)
            tmplg += strlen ((char *)CurNs_Uri[i]);
          tmplg +=2;

          if ((lg + tmplg) < MAX_BUFFER_SIZE)
            {
              /* We can add this declaration */	  
              strcat (elemBuffer, " xmlns");
              lg += 6;
              if (CurNs_Prefix[i] != NULL)
                {
                  strcat (elemBuffer, ":");
                  lg ++;
                  strcat (elemBuffer, CurNs_Prefix[i]);
                  lg += strlen ((char *)CurNs_Prefix[i]);
                }
              strcat (elemBuffer, "=\"");
              lg += 2;
              if (CurNs_Uri[i] != NULL)
                {
                  strcat (elemBuffer, CurNs_Uri[i]);
                  lg += strlen ((char *)CurNs_Uri[i]);
                }
              strcat (elemBuffer, "\"");
              lg ++;
            }
          else
            i = CurNs_Level;
        }
    }

  /* Create the Unknown element */
  newElement = NULL;
  (*(Proc3)(CurrentParserCtxt->UnknownNameSpace)) (
                                                   (void *)&XMLcontext,
                                                   (void *)&newElement,
                                                   (void *)elemBuffer);
  /* Store the current namespace declarations for this element */
  if (CurNs_Level > 0)
    NsStartProcessing (newElement, FALSE);

}

/*--------------------  Namespaces procedures  (end)  --------------*/


/*--------------------  StartElement  (start)  ---------------------*/

/*----------------------------------------------------------------------
  XmlCheckContext
  Verifies if the element elName is allowed to occur in the current
  structural context.
  ----------------------------------------------------------------------*/
static void   XmlCheckContext (char *elName, const ElementType * elType,
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
static void   XhtmlCheckContext (char *elName, const ElementType * elType,
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
      if (!strcmp ((char *)nameElementStack[stackLevel - 1], "tr"))
        if (strcmp ((char *)elName, "th") &&
            strcmp ((char *)elName, "td"))
          *isAllowed = FALSE;

      if (*isAllowed &&
          !strcmp (nameElementStack[stackLevel - 1], "table"))
        /* only CAPTION, THEAD, TFOOT, TBODY, COLGROUP, COL and TR are */
        /* allowed as children of a TABLE element */
        {
          if (strcmp ((char *)elName, "caption")  &&
              strcmp ((char *)elName, "thead")    &&
              strcmp ((char *)elName, "tfoot")    &&
              strcmp ((char *)elName, "tbody")    &&
              strcmp ((char *)elName, "colgroup") &&
              strcmp ((char *)elName, "col")      &&
              strcmp ((char *)elName, "tr"))
            {
              if (!strcmp ((char *)elName, "td") ||
                  !strcmp ((char *)elName, "th"))
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
        if (strcmp ((char *)elName, "caption")  == 0 ||
            strcmp ((char *)elName, "thead")    == 0 ||
            strcmp ((char *)elName, "tfoot")    == 0 ||
            strcmp ((char *)elName, "tbody")    == 0 ||
            strcmp ((char *)elName, "colgroup") == 0)
          if (strcmp ((char *)nameElementStack[stackLevel - 1], "table") != 0)
            *isAllowed = FALSE;

      if (*isAllowed)
        {
          /* only TR is allowed as a child of a THEAD, TFOOT or TBODY element */
          if (!strcmp ((char *)nameElementStack[stackLevel - 1], "thead") ||
              !strcmp ((char *)nameElementStack[stackLevel - 1], "tfoot") ||
              !strcmp ((char *)nameElementStack[stackLevel - 1], "tbody"))
            {
              if (strcmp ((char *)elName, "tr"))
                {
                  if (!strcmp ((char *)elName, "td") ||
                      !strcmp ((char *)elName, "th"))
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
          /* only LI is allowed as a child of a OL or UL element */
          if (!strcmp ((char *)nameElementStack[stackLevel - 1], "ol") ||
              !strcmp ((char *)nameElementStack[stackLevel - 1], "ul"))
            {
              if (strcmp ((char *)elName, "li"))
		*isAllowed = FALSE;
            }
        }

      if (*isAllowed)
	{
          /* only DT/DD is allowed as a child of a DL element */
    	  if (!strcmp ((char *)nameElementStack[stackLevel - 1], "dl"))
            {
              if (strcmp ((char *)elName, "dt") && strcmp ((char *)elName, "dd"))
		*isAllowed = FALSE;
            }
        }

      if (*isAllowed)
        {
          /* Block elements are not allowed within an anchor */
          if (!strcmp ((char *)nameElementStack[stackLevel - 1], "a") &&
              (!IsXMLElementInline (*elType,XMLcontext.doc )))
            *isAllowed = FALSE;
        }
       
      if (*isAllowed)
        {
          /* No one element is allowed within the title */
          if (!strcmp ((char *)nameElementStack[stackLevel - 1], "title") )
            *isAllowed = FALSE;
        }

      if (*isAllowed &&
          strcmp ((char *)elName, "body") == 0 &&
          XmlWithinStack (HTML_EL_BODY, XhtmlParserCtxt->XMLSSchema))
        /* refuse BODY within BODY */
        *isAllowed = FALSE;
       
      if (*isAllowed)
        /* refuse HEAD within HEAD */
        if (strcmp ((char *)elName, "head") == 0)
          if (XmlWithinStack (HTML_EL_HEAD, XhtmlParserCtxt->XMLSSchema))
            *isAllowed = FALSE;

      if (*isAllowed)
        /* refuse STYLE within STYLE */
        if (strcmp ((char *)elName, "style") == 0)
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
static void   GetXmlElType (const char *ns_uri, const char *elementName,
                            ElementType *elType, char **mappedName,
                            char *content, ThotBool *level)
{
#ifdef XML_GENERIC
  ThotBool      isnew;
  char         *s;
  char         *ns_name;
//  ElementType   parentType;
//  Element       parent;
#endif /* XML_GENERIC */

  /* initialize all parser contexts if not done yet */
  if (FirstParserCtxt == NULL)
    InitXmlParserContexts ();

  /* Look at the current context if there is one */
  if (CurrentParserCtxt != NULL)
    {
#ifdef XML_GENERIC
      if (CurrentParserCtxt == GenericXmlParserCtxt)
        {
          /* Search the element inside a not supported DTD */
          if (XMLRootName == NULL)
            {
              /* This is the document root */
              s = TtaGetSSchemaName (DocumentSSchema);
              elType->ElSSchema = GetGenericXMLSSchema (s, XMLcontext.doc);
              /* Initialize the current context schema */
              if (CurrentParserCtxt->XMLSSchema == NULL)
                CurrentParserCtxt->XMLSSchema = elType->ElSSchema;
              /* We instanciate the XML schema with the element name */
              /* (except for the elements 'comment', doctype and 'pi') */
              if (strcmp ((char *)elementName, "xmlcomment") &&
                  strcmp ((char *)elementName, "xmlcomment_line") &&
                  strcmp ((char *)elementName, "doctype") &&
                  strcmp ((char *)elementName, "doctype_line") &&
                  strcmp ((char *)elementName, "xmlpi") &&
                  strcmp ((char *)elementName, "xmlpi_line"))
                {
                  if (strcmp ((char *)s, "XML") == 0)
                    {
                      if (ns_uri != NULL)
                        {
                          ns_name = NsGetPrefix (ns_uri);
                          if (ns_name != NULL)
                            TtaChangeGenericSchemaNames (ns_uri, ns_name, XMLcontext.doc);
                          else
                            TtaChangeGenericSchemaNames (ns_uri, elementName, XMLcontext.doc);
                        }
                      else
                        TtaChangeGenericSchemaNames ("Default_Uri", elementName, XMLcontext.doc);
                    }
                }
            }
          else
            {
              // if (ns_uri != NULL)
                {
                  isnew = FALSE;
                  elType->ElSSchema = GetGenericXMLSSchemaByUri (ns_uri, XMLcontext.doc, &isnew);
                  if (isnew)
                    {
                      ns_name = NsGetPrefix (ns_uri);
                      if (ns_name != NULL)
                        TtaChangeGenericSchemaNames (ns_uri, ns_name, XMLcontext.doc);
                      else
                        TtaChangeGenericSchemaNames (ns_uri, elementName, XMLcontext.doc);
                      // complete the context if needed
                      if (CurrentParserCtxt && CurrentParserCtxt->XMLSSchema == NULL)
                        CurrentParserCtxt->XMLSSchema = elType->ElSSchema;
                    }
                }
              *level = TRUE;
              *content = SPACE;
            }
          MapGenericXmlElement (elementName, elType, mappedName, XMLcontext.doc);
        }
      else
        {
          /* Search the element inside a supported DTD */	  
          elType->ElSSchema = CurrentParserCtxt->XMLSSchema;
          MapXMLElementType (CurrentParserCtxt->XMLtype, elementName, elType,
                             mappedName, content, level, XMLcontext.doc);
        }
#else /* XML_GENERIC */
      elType->ElSSchema = CurrentParserCtxt->XMLSSchema;
      MapXMLElementType (CurrentParserCtxt->XMLtype, elementName, elType,
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
static void StartOfXmlStartElement (const char *name)
{
  ElementType     elType;
  Element         newElement;
  PtrParserCtxt   savParserCtxt = NULL;
  char            msgBuffer[MaxMsgLength];
  char            schemaName[NAME_LENGTH];
  char           *mappedName = NULL;
  char           *buffer, *ptr, *elementName, *nsURI, *nsDefUri;
  int             profile;
  ThotBool        elInStack = FALSE;
  ThotBool        highEnoughLevel = TRUE;
  ThotBool        isAllowed = TRUE;
  ThotBool        def_uri = FALSE;

  if (stackLevel == MAX_STACK_HEIGHT)
    {
      XmlParseError (errorNotWellFormed,
                     (unsigned char *)"**FATAL** Too many XML levels", 0);
      UnknownElement = TRUE;
      return;
    }

  UnknownNS = FALSE;
  UnknownElement = FALSE;
  elementName = NULL;
  nsURI = NULL;
  buffer = NULL;
  buffer = TtaStrdup (name);
  if (buffer == NULL)
    return;

  savParserCtxt = CurrentParserCtxt;
  /* Is this element in the scope of a namespace declaration */
  if ((ptr = strrchr (buffer, NS_SEP)) != NULL)
    {
      *ptr = EOS;
      nsURI = (char *)TtaStrdup ((char *)buffer);
      *ptr = NS_SEP;
      ptr++;
      /* check the document profile */
      profile = TtaGetDocumentProfile (XMLcontext.doc);
      if ((profile == L_Basic || profile == L_Strict) &&
          strcmp (nsURI, XHTML_URI))
        {
          snprintf ((char *)msgBuffer, MaxMsgLength,
                   "The element <%s> is not allowed by the current profile", ptr);
          XmlParseError (errorParsingProfile, (unsigned char *)msgBuffer, 0);
          TtaFreeMemory (nsURI);
          TtaFreeMemory (buffer);
          UnknownElement = TRUE; /* don't generate that element */
          CurrentParserCtxt = savParserCtxt;
          return;
        }
      /* Look for the context associated with that namespace */
      ChangeXmlParserContextByUri (nsURI);
      // it's a compound document
      if (CurrentParserCtxt && savParserCtxt != CurrentParserCtxt &&
          (!strcmp (CurrentParserCtxt->SSchemaName, "SVG") ||
           !strcmp (CurrentParserCtxt->SSchemaName, "MathML")) &&
          DocumentMeta[XMLcontext.doc])
        DocumentMeta[XMLcontext.doc]->compound = TRUE;
        
      elementName = (char *)TtaGetMemory ((strlen ((char *)ptr) + 1));
      strcpy ((char *)elementName, (char *)ptr);
    }
  else
    {
      /* No namespace declaration */
      if ((ptr = strrchr (buffer, NS_COLON)) != NULL)
        {
          /* there is an undefined prefix */
          snprintf ((char *)msgBuffer, MaxMsgLength,
                   "Undefined prefix for the element <%s>", (char *)name);
          XmlParseError (errorParsing, (unsigned char *)msgBuffer, 0);
          CurrentParserCtxt = NULL;
        }
      else if (CurrentParserCtxt->UriName != NULL &&
               CurrentParserCtxt->UriName[0] != EOS)
        {
          nsDefUri = GetDefaultNsUri (&def_uri);
          if (def_uri && nsDefUri == NULL)
            {
              /* Default namespace without uri */
              CurrentParserCtxt = NULL;
            }
        }
      elementName = (char *)TtaStrdup ((char *)buffer);
    }
  
  if (CurrentParserCtxt == NULL)
    {
#ifdef XML_GENERIC
      /* create a new XML generic context */
      CurrentParserCtxt = GenericXmlParserCtxt;
      CurrentParserCtxt->UriName = TtaStrdup (nsURI);
      TtaSetUriSSchema (CurrentParserCtxt->XMLSSchema, CurrentParserCtxt->UriName);
#else /*XML_GENERIC*/
      CurrentParserCtxt = savParserCtxt;
      UnknownNS = TRUE;
#endif /* XML_GENERIC */
    }  

  /* ignore tag <P> within PRE for Xhtml elements */
  if (CurrentParserCtxt != NULL &&
      (strcmp ((char *)CurrentParserCtxt->SSchemaName, "HTML") == 0) &&
      (XmlWithinStack (HTML_EL_Preformatted, CurrentParserCtxt->XMLSSchema)) &&
      (strcasecmp (elementName, "p") == 0))
    UnknownElement = TRUE;

  if (CurrentParserCtxt && !UnknownElement)
    {
      if (UnknownNS)
        {
          /* The element doesn't belong to a supported namespace */
          snprintf ((char *)msgBuffer, MaxMsgLength,
                   "The element <%s> doesn't belong to a supported namespace", name);
          XmlParseError (errorParsing, (unsigned char *)msgBuffer, 0);
          /* create an Unknown_namespace element */
          UnknownXmlNsElement (nsURI, elementName, TRUE);
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
              if (strcmp ((char *)CurrentParserCtxt->SSchemaName, "HTML") == 0)
                strcpy ((char *)schemaName, "XHTML");
              else
                strcpy ((char *)schemaName, (char *)CurrentParserCtxt->SSchemaName);
              if (strncmp (elementName, "SUBTREE_ROOT", 12))
                {
                  /* it's not the pseudo root element generated by transform */
                  if (highEnoughLevel)
                    {
                      /* element not found in the corresponding DTD */
                      /* don't process that element */
                      snprintf ((char *)msgBuffer, MaxMsgLength,
                                "Invalid or unsupported %s element <%s>",
                               schemaName , elementName);
                      XmlParseError (errorParsing, (unsigned char *)msgBuffer, 0);
                      UnknownElement = TRUE;
                    }
                  else
                    {
                      /* invalid element for the document profile */
                      /* don't process that element */
                      snprintf ((char *)msgBuffer, MaxMsgLength,
                               "Invalid %s element <%s> for the document profile",
                               schemaName, elementName);
                      XmlParseError (errorParsingProfile, (unsigned char *)msgBuffer, 0);
                      UnknownElement = TRUE;
                    }
                }
            }
          else
            {
              /* Element found in the corresponding DTD */
              strcpy ((char *)currentElementName, (char *)mappedName);
	      
              if (CurrentParserCtxt != NULL)
                (*(Proc3)(CurrentParserCtxt->CheckContext))(
                                                            (void *)mappedName,
                                                            (void *)&elType,
                                                            (void *)&isAllowed);
              if (!isAllowed)
                /* Element not allowed in the current structural context */
                {
                  snprintf ((char *)msgBuffer, MaxMsgLength,
                           "The XML element <%s> is not allowed here", elementName);
                  XmlParseError (errorParsing, (unsigned char *)msgBuffer, 0);
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
                    NsStartProcessing (newElement, TRUE);
		  
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
                  parserCtxtStack[stackLevel] = CurrentParserCtxt;
                  spacePreservedStack[stackLevel] = ' ';
                  stackLevel++;
                }
            }
        }
    }
  
  if (buffer != NULL)
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
static void EndOfXmlStartElement (char *name)
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
      if (strcmp ((char *)TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
        {
          if (!strcmp ((char *)nameElementStack[stackLevel - 1], "pre")   ||
              !strcmp ((char *)nameElementStack[stackLevel - 1], "style") ||
              !strcmp ((char *)nameElementStack[stackLevel - 1], "textarea") ||
              !strcmp ((char *)nameElementStack[stackLevel - 1], "script"))
            /* a <pre>, <style> <textarea> or <scriptT> tag has been read */
            XmlWhiteSpaceInStack ((char *)NULL);
          else
            if (!strcmp ((char *)nameElementStack[stackLevel - 1], "table"))
              /* <TABLE> has been read */
              XMLcontext.withinTable++;
	  
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
                  text = (char *)TtaGetMemory (length + 1);
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
      /* Specific treatment (concerns only MathML for the moment) */
      if (CurrentParserCtxt != NULL &&
          CurrentParserCtxt->ElementCreated != NULL)
        (*(Proc2)(CurrentParserCtxt->ElementCreated)) (
                                                       (void *)XMLcontext.lastElement,
                                                       (void *)XMLcontext.doc);
    }
  XmlWhiteSpaceHandling ();
}

/*----------------------  StartElement  (end)  -----------------------*/


/*----------------------  EndElement  (start)  -----------------------*/

/*----------------------------------------------------------------------
  EndOfXmlElement
  Terminate all corresponding Thot elements.
  ----------------------------------------------------------------------*/
static void EndOfXmlElement (char *name)
{
  ElementType    elType;
  PtrParserCtxt  savParserCtxt = NULL;
  int             profile;
  char          *nsURI, *elementName;
  char          *buffer;
  char          *ptr;
  char           msgBuffer[MaxMsgLength];
  char          *mappedName = NULL;
  ThotBool       highEnoughLevel = TRUE;
   
  UnknownNS = FALSE;
  UnknownElement = FALSE;
  elementName = NULL;
  nsURI = NULL;
  if (name == NULL)
    return;

  buffer = (char *)TtaStrdup ((char *)name);
  savParserCtxt = CurrentParserCtxt;
  /* Is this element in the scope of a namespace declaration */
  if ((ptr = strrchr (buffer, NS_SEP)) != NULL)
    {
      *ptr = EOS;
      nsURI = (char *)TtaStrdup ((char *)buffer);
      *ptr = NS_SEP;
      ptr++;
      /* Look for the context associated with that namespace */
      if (nsURI && ChangeXmlParserContextByUri (nsURI))
        {
          /* check the document profile */
          profile = TtaGetDocumentProfile (XMLcontext.doc);
          if (profile == L_Basic || profile == L_Strict)
            {
              TtaFreeMemory (nsURI);
              TtaFreeMemory (buffer);
              CurrentParserCtxt = savParserCtxt;
              return;
            }
        }
      elementName = (char *)TtaGetMemory ((strlen ((char *)ptr) + 1));
      strcpy ((char *)elementName, (char *)ptr);
    }
  else
    elementName = (char *)TtaStrdup ((char *)buffer);
  
  if (XMLcontext.parsingTextArea)
    if (strcasecmp (elementName, "textarea") != 0)
      /* We are parsing the contents of a textarea element. */
      /* The end tag is not the one closing the current textarea, */
      /* consider it as plain text */
      return;
 
  if (CurrentParserCtxt == NULL)
    {
#ifdef XML_GENERIC
      /* assign the generic context */
      CurrentParserCtxt = GenericXmlParserCtxt;
#else /*XML_GENERIC*/
      CurrentParserCtxt = savParserCtxt;
      UnknownNS = TRUE;
#endif /* XML_GENERIC */
    }
 
  /* search the element name in the corresponding mapping table */
  elType.ElSSchema = NULL;
  elType.ElTypeNum = 0;
  currentElementName[0] = EOS;
  if (UnknownNS)
    /* create an Unknown_namespace element */
    UnknownXmlNsElement (nsURI, elementName, FALSE);
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
              snprintf ((char *)msgBuffer, MaxMsgLength,
                        "Unexpected end tag </%s>", elementName);
              XmlParseError (errorParsing, (unsigned char *)msgBuffer, 0);
            }
        }
    }
  
  TtaFreeMemory (buffer);
  TtaFreeMemory (elementName);
  TtaFreeMemory (nsURI);
}
/*---------------------  EndElement  (end)  --------------------------*/


/*----------------------  Data  (start)  -----------------------------*/

/*----------------------------------------------------------------------
  IsLeadingSpaceUseless
  lastEl is the previous sibling of the new Text element
  doc is the current document
  isXML is TRUE if parsing a generic XML
  ----------------------------------------------------------------------*/
ThotBool  IsLeadingSpaceUseless (Element lastEl, Document doc, 
                                 ThotBool sibling, ThotBool isXML)
{
  ElementType   elType, lastElType, prevType;
  Element       parent, last;
  char         *name, *s;
  ThotBool      removeLeadingSpaces;


  if (sibling)
    /* There is a previous sibling (lastEl) for the new Text element */
    {
      parent = TtaGetParent (lastEl);
      if (parent == NULL)
        parent = lastEl;
      elType = TtaGetElementType (parent);
      lastElType = TtaGetElementType (lastEl);
      removeLeadingSpaces = TRUE;
      if (isXML)
        {
          /* Does the parent element contains a 'Line' presentation rule ? */
          /* if (TtaHasXmlInLineRule (elType, doc))*/
          /* Return FALSE for XML documents */
          removeLeadingSpaces = FALSE;
        }
      else
        {
          name = TtaGetSSchemaName (elType.ElSSchema);
          s = TtaGetSSchemaName (lastElType.ElSSchema);
          if (!strcmp (s, "MathML") &&
              lastElType.ElTypeNum == MathML_EL_MathML)
            // keep space after a Math element
            removeLeadingSpaces = FALSE;
          else if (!strcmp (name, "HTML") &&
                   // parent
                   elType.ElTypeNum != HTML_EL_HTML &&
                   elType.ElTypeNum != HTML_EL_HEAD &&
                   elType.ElTypeNum != HTML_EL_BODY &&
                   elType.ElTypeNum != HTML_EL_Division &&
                   elType.ElTypeNum != HTML_EL_Unnumbered_List &&
                   elType.ElTypeNum != HTML_EL_Numbered_List &&
                   elType.ElTypeNum != HTML_EL_Term_List &&
                   elType.ElTypeNum != HTML_EL_Definition_List &&
                   elType.ElTypeNum != HTML_EL_Table_ &&
                   elType.ElTypeNum != HTML_EL_Table_row &&
                   // element
                   !strcmp (s, "HTML") &&
                   (lastElType.ElTypeNum == HTML_EL_Comment_ ||
                    lastElType.ElTypeNum == HTML_EL_ASP_element ||
                    lastElType.ElTypeNum == HTML_EL_XMLPI))
            {
              /* Search the last significant sibling prior to a comment or a Pi */
              /* except for a comment or a Pi within the HEAD section */
              last = lastEl;
              TtaPreviousSibling (&last);
              while (last && removeLeadingSpaces)
                {
                  prevType = TtaGetElementType (last);
                  s = TtaGetSSchemaName (prevType.ElSSchema);
                  if (strcmp (s, "HTML") ||
                      (prevType.ElTypeNum != HTML_EL_Comment_ &&
		       prevType.ElTypeNum != HTML_EL_ASP_element &&
		       prevType.ElTypeNum != HTML_EL_XMLPI &&
		       prevType.ElTypeNum != HTML_EL_DOCTYPE))
                  // there is a previous element before
                    removeLeadingSpaces = (prevType.ElTypeNum == HTML_EL_TEXT_UNIT);
                  TtaPreviousSibling (&last);
                }
            }
          else if (!strcmp (name, "HTML") &&
                   IsCharacterLevelElement (lastEl))
            {
              if (elType.ElTypeNum != HTML_EL_Option_Menu &&
                  elType.ElTypeNum != HTML_EL_OptGroup)
                {
                  removeLeadingSpaces = FALSE;
                  if (lastElType.ElTypeNum == HTML_EL_BR)
                    removeLeadingSpaces = TRUE;
                }
            }
#ifdef _SVG
          else if (!strcmp (name, "SVG") &&
                   (elType.ElTypeNum == SVG_EL_text_ ||
                    elType.ElTypeNum == SVG_EL_tspan))
            removeLeadingSpaces = FALSE;
#endif /* _SVG */
          else if (!strcmp (name, "Template") &&
                   lastElType.ElTypeNum == HTML_EL_TEXT_UNIT)
            {
            removeLeadingSpaces = FALSE;
            }
        }
    }
  else
    /* the new Text element should be the first child 
       of the latest element encountered */
    {
      removeLeadingSpaces = TRUE;
    }
  return removeLeadingSpaces;
}

/*----------------------------------------------------------------------
  PutInXmlElement
  ----------------------------------------------------------------------*/
void PutInXmlElement (char *data, int length)

{
  ElementType  elType;
  Element      elText, parent;
  char        *buffer, *bufferws;
  int          i = 0;
  int          i1, i2 = 0, i3 = 0;
  ThotBool     uselessSpace = FALSE;
  ThotBool     insSibling, ok, removeEOL = RemoveLineBreak;

  i = 0;
  /* Immediately after a start tag, treatment of the leading spaces */
  /* If RemoveLeadingSpace = TRUE, we suppress all leading white-space */
  /* characters, otherwise, we only suppress the first line break */
  if (ImmediatelyAfterTag)
    {
      if (RemoveLeadingSpace)
        {
          while (data[i] == EOL || data[i] == __CR__ ||
                 data[i] == TAB || data[i] == SPACE)
            i++;
          if (data[i] != EOS)
            ImmediatelyAfterTag = FALSE;
        }
      else
        {
          if (data[0] == EOL || data[0] == __CR__)
            i = 1;
          ImmediatelyAfterTag = FALSE;
        }
    }
 
  if (CurrentParserCtxt->XMLSSchema &&
      TtaIsXmlSSchema (CurrentParserCtxt->XMLSSchema) &&
      length == 1 && data[0] == EOL)
    {
      if (XMLcontext.lastElement)
        // don't generate a XML_Element just after a text unit
        elType = TtaGetElementType (XMLcontext.lastElement);
      else
        elType.ElTypeNum = 0;
      if (elType.ElTypeNum != XML_EL_TEXT_UNIT)
        {
          // create an empty element
          elType.ElSSchema = CurrentParserCtxt->XMLSSchema;
          elType.ElTypeNum = XML_EL_xmlbr;
          elText = TtaNewElement (XMLcontext.doc, elType);
          XmlSetElemLineNumber (elText);
          InsertXmlElement (&elText);
          XMLcontext.lastElementClosed = TRUE;
          return;
        }
      else
        removeEOL = TRUE;
    }
  else if (length == i || data[i] == EOS)
    return;

  length -= i;
  bufferws = (char *)TtaGetMemory (length + 1);
  strncpy (bufferws, &data[i], length);
  bufferws[length] = EOS;

  /* Convert line-break or tabs into space character */
  i = 0;
  if (removeEOL)
    {
      while (bufferws[i] != EOS)
        {
          if (bufferws[i] == EOL || bufferws[i] == __CR__ ||
              bufferws[i] == TAB)
            bufferws[i]= SPACE;
          i++;
        }
    }

  i = 0;
  if (XMLcontext.lastElement)
  {
    /* Suppress the leading spaces in Inline elements */
    insSibling = InsertingSibling ();
    uselessSpace = IsLeadingSpaceUseless (XMLcontext.lastElement, XMLcontext.doc, insSibling,
                                          TtaIsXmlSSchema (CurrentParserCtxt->XMLSSchema));
    if (RemoveLeadingSpace && uselessSpace)
      /* suppress leading spaces */
      while (bufferws[i] == SPACE)
        i++;
       
    /* Collapse contiguous spaces */ 
    if (bufferws[i] != EOS)
      {
        length = strlen ((char *)bufferws);
        buffer = (char *)TtaGetMemory (length+1);
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
          strcpy ((char *)buffer, (char *)bufferws);
        i1 = 0;
	   
        /* Filling of the element value */
        elType = TtaGetElementType (XMLcontext.lastElement);
        if (elType.ElTypeNum == 1 && XMLcontext.mergeText)
          {
            if ((buffer[i1] == SPACE) && RemoveContiguousSpace)
              {
                /* Is the last character of text element a space */
                if (TtaHasFinalSpace (XMLcontext.lastElement, XMLcontext.doc))
                  /* Remove leading space if last content was finished by a space */
                  TtaAppendTextContent (XMLcontext.lastElement,
                                        (unsigned char *)&(buffer[i1 + 1]), XMLcontext.doc);
                else
                  TtaAppendTextContent (XMLcontext.lastElement,
                                        (unsigned char *)&(buffer[i1]), XMLcontext.doc);
              }
            else
              TtaAppendTextContent (XMLcontext.lastElement,
                                    (unsigned char *)&(buffer[i1]), XMLcontext.doc);
          }
        else
          {
            if (RemoveLeadingSpace &&
                CurrentParserCtxt->XMLSSchema &&
                !strcmp ((char *)CurrentParserCtxt->SSchemaName, "HTML") &&
                buffer[i1] == SPACE && strlen (&buffer[i1]) == 1)
              {
                // avoid to generate an empty pseudo paragraph
                ok = FALSE;
                if (InsertingSibling ())
                  parent = TtaGetParent (XMLcontext.lastElement);
                else
                  parent = XMLcontext.lastElement;
                if (parent)
                  {
                    elType = TtaGetElementType (parent);
                    if (IsCharacterLevelElement (parent) ||
                        !XhtmlCannotContainText (elType))
                      ok = TRUE; // generate the TEXT element
                  }
              }
            else
              ok = TRUE;
            if (ok && CurrentParserCtxt->XMLSSchema)
              {
                // by default insert a new text unit
                insSibling = TRUE;
                if (TtaIsXmlSSchema (CurrentParserCtxt->XMLSSchema) &&
                    XMLcontext.lastElement)
                  {
                    // replace the previous XML_Element by  a text unit
                    elType = TtaGetElementType (XMLcontext.lastElement);
                    insSibling = (elType.ElTypeNum != XML_EL_xmlbr);
                    if (!insSibling)
                      {
                      elText = XMLcontext.lastElement;
                      }
                  }
                if (insSibling)
                  {
                    elType.ElSSchema = CurrentParserCtxt->XMLSSchema;
                    /* create a TEXT element */
                    elType.ElTypeNum = HTML_EL_TEXT_UNIT;
                    elText = TtaNewElement (XMLcontext.doc, elType);
                    if (elText)
                      {
                        XmlSetElemLineNumber (elText);
                        InsertXmlElement (&elText);
                        /* put the content of the input buffer into the TEXT element */
                        TtaSetTextContent (elText, (unsigned char *)&(buffer[i1]),
                                           XMLcontext.language, XMLcontext.doc);
                      }
                  }
                else
                  {
                    // replace the empty element by a text unit
                    TtaChangeTypeOfElement (elText, XMLcontext.doc, XML_EL_TEXT_UNIT);
                    XmlSetElemLineNumber (elText);
                    XMLcontext.lastElement = elText;
                    XMLcontext.lastElementClosed = FALSE;
                    TtaSetTextContent (elText, (unsigned char *)"\n",
                                       XMLcontext.language, XMLcontext.doc);
                    TtaAppendTextContent (XMLcontext.lastElement,
                                          (unsigned char *)&(buffer[i1]), XMLcontext.doc);
                  }
                /* associate a specific 'Line' presentation rule to the 
                   parent element if we are parsing a generic-XML element */
                XMLcontext.lastElementClosed = TRUE;
                XMLcontext.mergeText = TRUE;
              }
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
  Returns
  - the content of the TEXT element.
  - the length of the parsed original string in *length.
  ----------------------------------------------------------------------*/
static unsigned char *HandleXMLstring (unsigned char *data, int *length,
                                       Element element, ThotBool stdText)
{
  unsigned char *buffer;
  unsigned char *ptr;
  unsigned char *entityName;
  int            i = 0, j = 0;
  int            max;
  int            k, l, m;
  int            entityValue;	
  ThotBool       found, end;
  char           msgBuffer[MaxMsgLength];

  max = *length;
  buffer = (unsigned char *)TtaGetMemory (4 * max + 1);
  while (i < max)
    {
      if (data[i] == START_ENTITY)
        {
          /* Maybe it is the beginning of an entity */
          end = FALSE;
          entityName = (unsigned char *)TtaGetMemory (max + 1);
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
                  found = MapXMLEntity (CurrentParserCtxt->XMLtype,
                                        (char *)&entityName[1], &entityValue);
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
                  else if (found && element)
                    {
                      if (stdText)
                        {
                          buffer[j] = EOS;
                          if (j > 0)
                            {
                              /* close the current text element */
                              PutInXmlElement ((char *)buffer, j);
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
                                               (char *)entityName, element);
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
                      if (!found)
                        {
                         sprintf ((char *)msgBuffer, "Unknown entity \"%s;\"",
                                  entityName);
                         XmlParseError (errorParsing,
                                        (unsigned char *)msgBuffer, 0);
                        }
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
        buffer[j++] = data[i++];
    }
  buffer[j] = EOS;
  if (stdText)
    {
      if (j > 0)
        PutInXmlElement ((char *)buffer, j);
    }  
  return buffer;
}
/*----------------------  Data  (end)  ---------------------------*/


/*--------------------  Attributes  (start)  ---------------------*/
/*----------------------------------------------------------------------
  UnknownXmlAttribute
  Creation and filling in of an "unknown_attr" attribute
  ----------------------------------------------------------------------*/
static void      UnknownXmlAttribute (char *xmlAttr, char *ns_uri)
     
{
  AttributeType attrType;
  Attribute     attr;
  char         *buffer, *bufattr, *ns_prefix = NULL;
  ThotBool      level = TRUE;
  int           length, buflen;

  if (CurrentParserCtxt != NULL)
    {
      /* If the uri associated with a prefix ? */
      if (ns_uri != NULL)
        ns_prefix = NsGetPrefix (ns_uri);

      /* Attach an Invalid_attribute to the current element */
      attrType.AttrSSchema = CurrentParserCtxt->XMLSSchema;
      if (CurrentParserCtxt == XhtmlParserCtxt)
        attrType.AttrTypeNum = HTML_ATTR_Unknown_attribute;
      else
        (*(Proc5)(CurrentParserCtxt->MapAttribute)) (
                                                     (void *)"unknown_attr",
                                                     (void *)&attrType,
                                                     (void *)currentElementName,
                                                     (void *)&level,
                                                     (void *)XMLcontext.doc);
      if (attrType.AttrTypeNum > 0)
        {
          attr = TtaGetAttribute (XMLcontext.lastElement, attrType);
          if (attr == NULL)
            {
              attr = TtaNewAttribute (attrType);
              TtaAttachAttribute (XMLcontext.lastElement, attr, XMLcontext.doc);
              length = strlen ((char *)xmlAttr);
              if (ns_prefix != NULL)
                {
                  length += strlen ((char *)ns_prefix);
                  buffer = (char *)TtaGetMemory (length + 3);
                  strcpy ((char *)buffer, " ");
                  strcat ((char *)buffer, (char *)ns_prefix);
                  strcat ((char *)buffer, ":");
                  strcat ((char *)buffer, (char *)xmlAttr);
                }
              else
                {
                  buffer = (char *)TtaGetMemory (length + 2);
                  strcpy ((char *)buffer, " ");
                  strcat ((char *)buffer, (char *)xmlAttr);
                }
              TtaSetAttributeText (attr, (char *)buffer,
                                   XMLcontext.lastElement,
                                   XMLcontext.doc);
              TtaFreeMemory (buffer);
            }
          else
            {
              /* Copy the name of the attribute as the content */
              /* of the Invalid_attribute attribute. */
              buflen = TtaGetTextAttributeLength (attr);
              bufattr = (char *)TtaGetMemory (buflen + 1);
              TtaGiveTextAttributeValue (attr, bufattr, &buflen);
              length = strlen ((char *)bufattr);
              length += strlen ((char *)xmlAttr);
              if (ns_prefix != NULL)
                {
                  length += strlen ((char *)ns_prefix);
                  buffer = (char *)TtaGetMemory (length + 3);
                  strcpy ((char *)buffer, (char *)bufattr);
                  strcat ((char *)buffer, " ");
                  strcat ((char *)buffer, (char *)ns_prefix);
                  strcat ((char *)buffer, ":");
                  strcat ((char *)buffer, (char *)xmlAttr);
                }
              else
                {
                  buffer = (char *)TtaGetMemory (length + 2);
                  strcpy ((char *)buffer, (char *)bufattr);
                  strcat ((char *)buffer, " ");
                  strcat ((char *)buffer, (char *)xmlAttr);
                }
              TtaSetAttributeText (attr, (char *)buffer,
                                   XMLcontext.lastElement,
                                   XMLcontext.doc);
              TtaFreeMemory (buffer);
              TtaFreeMemory (bufattr);
            }
          currentAttribute = attr;
        }
    }
  return;
}

/*----------------------------------------------------------------------
  EndOfXhtmlAttributeName   
  End of an XHTML attribute
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
          snprintf ((char *)msgBuffer, MaxMsgLength,
                    "Invalid XHTML attribute \"%s\"", attrName);
          XmlParseError (errorParsing, (unsigned char *)msgBuffer, 0);
        }
      else
        {
          snprintf ((char *)msgBuffer, MaxMsgLength,
                   "Invalid XHTML attribute \"%s\" for the document profile",
                   attrName);
          XmlParseError (errorParsingProfile, (unsigned char *)msgBuffer, 0);
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
                  TtaSetAttributeText (attr, (char *)"link", el, doc);
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
  End of a XML attribute (other than XHTML)
  ----------------------------------------------------------------------*/
static void EndOfXmlAttributeName (char *attrName, char *uriName,
                                   Element  el, Document doc)
{
  AttributeType    attrType;
  Attribute        attr;
  char             msgBuffer[MaxMsgLength];
  ThotBool         level = TRUE;
  char             schemaName[NAME_LENGTH];
  ElementType      elType;
  ThotBool         isnew;

  attrType.AttrTypeNum = 0;

#ifdef OLD
  if (strlen ((char *)attrName) >= NAME_LENGTH)
    {
      strcpy ((char *)schemaName, (char *)CurrentParserCtxt->SSchemaName);
      snprintf ((char *)msgBuffer, MaxMsgLength,
                "Attribute name too long for Amaya %s", attrName);
      XmlParseError (errorParsing, (unsigned char *)msgBuffer, 0);
      UnknownAttr = TRUE;
      return;
    }
#endif
   
  if (CurrentParserCtxt != NULL)
    {
      if (CurrentParserCtxt == GenericXmlParserCtxt)
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
        {
          if (CurrentParserCtxt->MapAttribute)
            (*(Proc5)(CurrentParserCtxt->MapAttribute)) (
                                                         (void *)attrName,
                                                         (void *)&attrType,
                                                         (void *)currentElementName,
                                                         (void *)&level,
                                                         (void *)doc);
        }
    }
   
  if (attrType.AttrTypeNum <= 0)
    {
      /* This attribute is not in the corresponding mapping table */
      strncpy ((char *)schemaName, (char *)CurrentParserCtxt->SSchemaName,NAME_LENGTH-1);
      schemaName[NAME_LENGTH-1] = EOS;
      // skip possible old template attributes
      if (strcmp (schemaName, "Template"))
        {
          snprintf (msgBuffer, MaxMsgLength,
                    "Invalid or unsupported %s attribute \"%s\"",
                    schemaName, attrName);
          XmlParseError (errorParsing, (unsigned char *)msgBuffer, 0);
          /* Attach an Invalid_attribute to the current element */
          /* It may be a valid attribute that is not yet defined in Amaya tables */
          UnknownXmlAttribute (attrName, NULL);
        }
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
static void EndOfAttributeName (char *xmlName)
{
  PtrParserCtxt savParserCtxt = NULL;
  int           profile;
  char         *attrName, *nsURI;
  char         *ptr = NULL;
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
  savParserCtxt = CurrentParserCtxt;
  nsURI = (char *)TtaStrdup ((char *)xmlName);
  if ((ptr = strrchr (nsURI, NS_SEP)) != NULL)
    {
      /* This attribute belongs to a specific namespace */
      *ptr = EOS;
      ptr++;
      /* Specific treatment to get round a bug in EXPAT parser */
      /* It replaces first "xml:" prefix by the namespaces URI */
      if (strcmp ((char *)nsURI, (char *)NAMESPACE_URI) == 0)
        {
          attrName = (char *)TtaGetMemory (strlen ((char *)ptr) + 5);
          strcpy ((char *)attrName, "xml:");
          strcat ((char *)attrName, (char *)ptr);
          if (nsURI)
            {
              TtaFreeMemory (nsURI);
              nsURI = NULL;
            }
        }
      else
        {
          attrName = (char *)TtaStrdup ((char *)ptr);
          if (UnknownNS)
            CurrentParserCtxt = NULL;
          if (CurrentParserCtxt && CurrentParserCtxt->UriName &&
              strcmp ((char *)nsURI, (char *)CurrentParserCtxt->UriName) &&
              ChangeXmlParserContextByUri (nsURI))
            {
              /* check the document profile */
              profile = TtaGetDocumentProfile (XMLcontext.doc);
              if (profile == L_Basic || profile == L_Strict)
                {
                  TtaFreeMemory (nsURI);
                  CurrentParserCtxt = savParserCtxt;
                  return;
                }
            }
        }
    }
  else
    {
      if ((ptr = strrchr (nsURI, NS_COLON)) != NULL)
        {
          /* This attribute is prefixed */
          *ptr = EOS;
          if (strcmp (nsURI, "xml") != 0)
            {
              if (CurrentParserCtxt == NULL ||
                  CurrentParserCtxt->SSchemaName == NULL ||
                  strcmp (CurrentParserCtxt->SSchemaName, "SVG") ||
                  strcmp (nsURI, "xlink"))
                {
                  snprintf ((char *)msgBuffer, MaxMsgLength,
                           "Undefined prefix for the attribute \"%s\"", (char *)xmlName);
                  XmlParseError (errorParsing, (unsigned char *)msgBuffer, 0);
                  *ptr = NS_COLON;
                  if (nsURI)
                    {
                      TtaFreeMemory (nsURI);
                      nsURI = NULL;
                    }
                  UnknownAttr = TRUE;
                  return;
                }     
            }
          *ptr = NS_COLON;
        }
      /* This attribute belongs to the same namespace as the element */
      TtaFreeMemory (nsURI);
      nsURI = NULL;
      attrName = (char *)TtaStrdup ((char *)xmlName);
      if (UnknownNS)
        /* The corresponding element doesn't belong to a supported namespace */ 
        {
          snprintf ((char *)msgBuffer, MaxMsgLength,
                   "Namespace not supported for the attribute \"%s\"", (char *)xmlName);
          XmlParseError (errorParsing, (unsigned char *)msgBuffer, 0);
          /* Create an unknown attribute  */
          UnknownXmlAttribute (attrName, NULL);
          UnknownAttr = TRUE;
        }
    }
   
  if (CurrentParserCtxt == NULL)
    {
#ifdef XML_GENERIC
      /* We assign the generic XML context by default */ 
      CurrentParserCtxt = GenericXmlParserCtxt;
#else /* XML_GENERIC */
      CurrentParserCtxt = savParserCtxt;
      snprintf (msgBuffer, MaxMsgLength,
               "Namespace not supported for the attribute \"%s\"",
               (char *)xmlName);
      XmlParseError (errorParsing, (unsigned char *)msgBuffer, 0);
      /* Create an unknown attribute  */
      UnknownXmlAttribute (attrName, nsURI);
      UnknownAttr = TRUE;
#endif /* XML_GENERIC */  
    }

  /* Is it a xml:space attribute */
  if (strncmp (attrName, "xml:space", 9) == 0)
    XMLSpaceAttribute = TRUE;
   
  /* the attribute xml:lang is replaced by the attribute "lang" */
  if (strncmp (attrName, "xml:lang", 8) == 0)
    strcpy ((char *)attrName, "lang");

  if (CurrentParserCtxt && !UnknownAttr)
    {
      if (strcmp ((char *)CurrentParserCtxt->SSchemaName, "HTML") == 0)
        EndOfXhtmlAttributeName (attrName,
                                 XMLcontext.lastElement, XMLcontext.doc);
      else
        EndOfXmlAttributeName (attrName, nsURI, XMLcontext.lastElement,
                               XMLcontext.doc);
    }
   
  TtaFreeMemory (nsURI);
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
  char            *buffer, *name;
  Language         lang;

  TtaGiveAttributeType (currentAttribute, &attrType, &attrKind);
  switch (attrKind)
    {
    case 0:       /* enumerate */
      (*(Proc3)(CurrentParserCtxt->MapAttributeValue)) (
                                                        (void *)attrValue,
                                                        (void *)&attrType,
                                                        (void *)&val);
      if (val <= 0)
        {
          snprintf ((char *)msgBuffer, MaxMsgLength,
                   "Unknown attribute value \"%s\"", (char *)attrValue);
          XmlParseError (errorParsing, (unsigned char *)msgBuffer, 0);	
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
          length = strlen ((char *)attrValue) + 4;
          length += TtaGetTextAttributeLength (currentAttribute);
          buffer = (char *)TtaGetMemory (length + 1);
          TtaGiveTextAttributeValue (currentAttribute,
                                     buffer, &length);
          strcat (buffer, "=\"");
          strcat (buffer, attrValue); 
          strcat (buffer, "\"");
          TtaSetAttributeText (currentAttribute, (char *)buffer,
                               XMLcontext.lastElement, XMLcontext.doc);
          TtaFreeMemory (buffer);
        }
      else
        {
          TtaSetAttributeText (currentAttribute, (char *)attrValue,
                               XMLcontext.lastElement, XMLcontext.doc);

          /* It's a style attribute */
          if (HTMLStyleAttribute)
            ParseHTMLSpecificStyle (XMLcontext.lastElement, attrValue,
                                    XMLcontext.doc, 1000, FALSE);
	   
          /* it's a LANG attribute value */
          if (attrType.AttrTypeNum == 1)
            {
              lang = TtaGetLanguageIdFromName (attrValue);
              if (lang < 0)
                {
                  snprintf ((char *)msgBuffer, MaxMsgLength,
                           "warning - unsupported language: %s", (char *)attrValue);
                  XmlParseError (warningMessage, (unsigned char *)msgBuffer, 0);
                }
              else
                {
                  /* change current language */
                  XMLcontext.language = lang;
                  SetLanguagInXmlStack (lang);
                }
            }
#ifdef XML_GENERIC
          else
            {
              /* check xml:id attributes */
              name = TtaGetSSchemaName (attrType.AttrSSchema);
              if (strcmp (name, "HTML") &&
                  strcmp (name, "MathML") &&
                  strcmp (name, "SVG") &&
                  attrType.AttrTypeNum == XML_ATTR_xmlid)
                CheckUniqueName (XMLcontext.lastElement, XMLcontext.doc,
                                 currentAttribute, attrType);
            }
#endif /* XML_GENERIC */
        }
      break;
    case 3:       /* reference */
      break;
    }
   
  if (CurrentParserCtxt != NULL && !HTMLStyleAttribute)
    (*(Proc3)(CurrentParserCtxt->AttributeComplete)) (
                                                      (void *)currentAttribute,
                                                      (void *)XMLcontext.lastElement,
                                                      (void *)XMLcontext.doc);
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

  if ((lastMappedAttr || currentAttribute) && CurrentParserCtxt)
    {
      length = strlen ((char *)attrValue);
      buffer = HandleXMLstring ((unsigned char *)attrValue, &length, NULL, FALSE);
      /* White-space attribute */
      if (XMLSpaceAttribute)
        XmlWhiteSpaceInStack ((char *)buffer);
      
      if (UnknownAttr)
        {
          /* This is the content of an invalid attribute */
          /* Append it to the current Invalid_attribute */
          length = strlen ((char *)attrValue) + 4;
          length += TtaGetTextAttributeLength (currentAttribute);
          bufferNS = (char *)TtaGetMemory (length + 1);
          TtaGiveTextAttributeValue (currentAttribute,
                                     bufferNS, &length);
          strcat ((char *)bufferNS, "=\"");
          strcat ((char *)bufferNS, (char *)attrValue); 
          strcat ((char *)bufferNS, "\"");
          TtaSetAttributeText (currentAttribute, (char *)bufferNS,
                               XMLcontext.lastElement, XMLcontext.doc);
          TtaFreeMemory (bufferNS);
        }
      else
        {
          if (strcmp ((char *)CurrentParserCtxt->SSchemaName, "HTML") == 0)
            EndOfHTMLAttributeValue ((char *)buffer, lastMappedAttr,
                                     currentAttribute, lastAttrElement,
                                     UnknownAttr, &XMLcontext, TRUE);
          else
            EndOfXmlAttributeValue ((char *)buffer);
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
  Search the entity in the right entity table and 
  put the corresponding character in the current element.
  ----------------------------------------------------------------------*/
static void     CreateXmlEntity (char *data, int length)
{
  unsigned char    *buffer;

  data[0] = START_ENTITY;
  buffer = (unsigned char *)HandleXMLstring ((unsigned char *)data, &length, XMLcontext.lastElement, TRUE);
  TtaFreeMemory (buffer);
}
/*--------------------  Entities  (end)  ---------------------*/

/*--------------------  Doctype  (start)  ---------------------*/
/*----------------------------------------------------------------------
  ParseDoctypeContent
  Parse the content of a DOCTYPE declaration
  -------------------------------------- -------------------------------*/
static void ParseDoctypeContent (const char *data, int length)
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

  buffer = (unsigned char *)TtaGetMemory (length + 1);
  i = 0;
  j = 0;
  while (i < length)
    {
      /* Look for line breaks in the content and create */
      /* as many DOCTYPE_line elements as needed */
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
              TtaSetTextContent (doctypeLeaf, (unsigned char *)buffer, Latin_Script, XMLcontext.doc);
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
      /* We use the Latin_Script language to avoid */
      /* the spell_checker to check this element */
      TtaSetTextContent (doctypeLeaf, (unsigned char *)buffer, Latin_Script, XMLcontext.doc);
    }

  TtaFreeMemory (buffer);
}

/*----------------------------------------------------------------------
  CreateDoctypeElement
  Create a Doctype element into the Thot tree.
  ----------------------------------------------------------------------*/
static void CreateDoctypeElement (char *name, char *sysid, char *pubid)
{
  ElementType     elType, lineType;
  Element  	  doctype, doctypeLine, doctypeLeaf, doctypeLineNew, lastChild;
  char           *mappedName, *buffer;
  char            cont;
  ThotBool        level = TRUE;

  /* Create a DOCTYPE element */
  elType.ElSSchema = NULL;
  elType.ElTypeNum = 0;
  GetXmlElType (NULL, "doctype", &elType, &mappedName, &cont, &level);
  if (elType.ElTypeNum > 0)
    {
      doctype = TtaNewElement (XMLcontext.doc, elType);
      XmlSetElemLineNumber (doctype);
      InsertXmlElement (&doctype);
      /* Make the DOCTYPE element read-only */
      TtaSetAccessRight (doctype, ReadOnly, XMLcontext.doc);
      /* Create a DOCTYPE_line element as first child */
      lineType.ElSSchema = NULL;
      lineType.ElTypeNum = 0;
      GetXmlElType (NULL, "doctype_line", &lineType, &mappedName, &cont, &level);
      doctypeLine = TtaNewElement (XMLcontext.doc, lineType);
      XmlSetElemLineNumber (doctypeLine);
      TtaInsertFirstChild (&doctypeLine, doctype, XMLcontext.doc);
      /* Create the DOCTYPE leaf */
      elType.ElSSchema = lineType.ElSSchema;
      elType.ElTypeNum = 1;
      doctypeLeaf = TtaNewElement (XMLcontext.doc, elType);
      if (doctypeLeaf != NULL)
        {
          XmlSetElemLineNumber (doctypeLeaf);
          TtaInsertFirstChild (&doctypeLeaf, doctypeLine, XMLcontext.doc);
          /* We use the Latin_Script language to avoid */
          /* the spell_chekcer to check this element */
          TtaSetTextContent (doctypeLeaf, (unsigned char *)"<!DOCTYPE ",
                             Latin_Script, XMLcontext.doc);
          TtaCancelSelection (XMLcontext.doc); /* TtaSetTextContent added a selection */
        }
     
      if (name != NULL)
        {
          elType.ElSSchema = lineType.ElSSchema;
          elType.ElTypeNum = 1;
          doctypeLeaf = TtaNewElement (XMLcontext.doc, elType);
          if (doctypeLeaf != NULL)
            {
              XmlSetElemLineNumber (doctypeLeaf);
              lastChild = TtaGetLastChild (doctypeLine);
              if (lastChild == NULL)
                TtaInsertFirstChild (&doctypeLeaf, doctypeLine, XMLcontext.doc);
              else
                TtaInsertSibling (doctypeLeaf, lastChild,
                                  FALSE, XMLcontext.doc);
              TtaSetTextContent (doctypeLeaf, (unsigned char *)name, Latin_Script, XMLcontext.doc);
              TtaCancelSelection (XMLcontext.doc); /* TtaSetTextContent added a selection */
            }
        }
      /* Is there any external DTD ? */
      if (pubid != NULL)
        {
          elType.ElSSchema = lineType.ElSSchema;
          elType.ElTypeNum = 1;
          doctypeLeaf = TtaNewElement (XMLcontext.doc, elType);
          if (doctypeLeaf != NULL)
            {
              buffer = (char *)TtaGetMemory (strlen ((char *)pubid) + 11);
              strcpy ((char *)buffer, " PUBLIC \"");
              strcat ((char *)buffer, (char *)pubid);
              strcat ((char *)buffer, "\"");
              XmlSetElemLineNumber (doctypeLeaf);
              lastChild = TtaGetLastChild (doctypeLine);
              if (lastChild == NULL)
                TtaInsertFirstChild (&doctypeLeaf, doctypeLine, XMLcontext.doc);
              else
                TtaInsertSibling (doctypeLeaf, lastChild,
                                  FALSE, XMLcontext.doc);
              TtaSetTextContent (doctypeLeaf, (unsigned char *)buffer, Latin_Script, XMLcontext.doc);
              TtaCancelSelection (XMLcontext.doc); /* TtaSetTextContent added a selection */
              TtaFreeMemory (buffer);
            }
        }
      if (sysid != NULL)
        {
          /* Create a new DOCTYPE_line element */
          if (pubid)
            {
              doctypeLineNew = TtaNewElement (XMLcontext.doc, lineType);
              if (doctypeLineNew != NULL)
                {
                  XmlSetElemLineNumber (doctypeLineNew);
                  TtaInsertSibling (doctypeLineNew, doctypeLine,
                                    FALSE, XMLcontext.doc);
                  doctypeLine = doctypeLineNew;
                }
            }
          elType.ElSSchema = lineType.ElSSchema;
          elType.ElTypeNum = 1;
          doctypeLeaf = TtaNewElement (XMLcontext.doc, elType);
          if (doctypeLeaf != NULL)
            {
              XmlSetElemLineNumber (doctypeLeaf);
              lastChild = TtaGetLastChild (doctypeLine);
              if (lastChild == NULL)
                TtaInsertFirstChild (&doctypeLeaf, doctypeLine, XMLcontext.doc);
              else
                TtaInsertSibling (doctypeLeaf, lastChild,
                                  FALSE, XMLcontext.doc);
              buffer = (char *)TtaGetMemory (strlen ((char *)sysid) + 11);
              if (pubid)
                strcpy ((char *)buffer, "       \"");
              else
                strcpy ((char *)buffer, " SYSTEM \"");
              strcat (buffer, sysid);
              strcat (buffer, "\"");
              TtaSetTextContent (doctypeLeaf, (unsigned char *)buffer, Latin_Script, XMLcontext.doc);
              TtaCancelSelection (XMLcontext.doc); /* TtaSetTextContent added a selection */
              TtaFreeMemory (buffer);
            }
        }
    }
}
/*--------------------  Doctype  (end)  ------------------------------*/

/*--------------------  CDATA  (start)  ---------------------*/
/*----------------------------------------------------------------------
  ParseCdataElement
  Parse the content of a CDATA element
  -------------------------------------- -------------------------------*/
static void ParseCdataElement (char *data, int length)

{
  ElementType     elType;
  Element  	  cdataLine, cdataLeaf, cdataLineNew, lastChild;
  char           *mappedName;
  char            cont;
  ThotBool        level = TRUE;
  unsigned char  *buffer;
  int             i, j;

  buffer = (unsigned char *)TtaGetMemory (length + 1);
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
                  TtaSetTextContent (cdataLeaf, (unsigned char *)buffer,
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
          TtaSetTextContent (cdataLeaf, (unsigned char *)buffer,
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
  GetXmlElType (CurrentParserCtxt->UriName, "xmlcomment",
                &elType, &mappedName, &cont, &level);
  if (elType.ElTypeNum > 0)
    {
      commentEl = TtaNewElement (XMLcontext.doc, elType);
      XmlSetElemLineNumber (commentEl);
      InsertXmlElement (&commentEl);
      /* Create a XMLcomment_line element as the first child of */
      /* Element XMLcomment */
      elType.ElSSchema = NULL;
      elType.ElTypeNum = 0;
      GetXmlElType (CurrentParserCtxt->UriName, "xmlcomment_line",
                    &elType, &mappedName, &cont, &level);
      commentLineEl = TtaNewElement (XMLcontext.doc, elType);
      XmlSetElemLineNumber (commentLineEl);
      TtaInsertFirstChild (&commentLineEl, commentEl, XMLcontext.doc);
      length = strlen ((char *)commentValue);
      i = 0;
      j = 0; /* parsed length */
      ptr = (unsigned char *)&commentValue[i];
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
              buffer = HandleXMLstring ((unsigned char *)ptr, &l, commentLineEl, FALSE);
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
              TtaSetTextContent (commentLeaf, (unsigned char *)buffer,
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
              ptr = (unsigned char *)&commentValue[i];
            }
        }
      (*(Proc3)(CurrentParserCtxt->ElementComplete)) (
                                                      (void *)&XMLcontext,
                                                      (void *)commentEl,
                                                      (void *)&error);
      XMLcontext.lastElementClosed = TRUE;
    }
}

/*--------------------  Comments  (end)  ------------------------------*/

/*--------------------  PI  (start)  ----------------------------------*/

/*----------------------------------------------------------------------
  LoadXmlStyleSheet
  ---------------------------------------------------------------------*/
static void LoadXmlStyleSheet (Document doc)
{
  int           i;
  char         *ptr;
  ThotBool      loadcss;
  
  TtaGetEnvBoolean ("LOAD_CSS", &loadcss);
  if (!loadcss)
    return;
  for (i = 0; i < XML_CSS_Level; i++)
    {
      if (XML_CSS_Href[i] && !strncasecmp (XML_CSS_Href[i], "data:", 5))
        {
          ptr = &XML_CSS_Href[i][5];
          if (!strncasecmp (ptr, "text/css", 8))
            /* confirm that it's a CSS stylesheet (see rfc2397) */
            ptr += 8;
          if (*ptr == ';')
            do
              ptr++;
            while (*ptr != ',' && *ptr != EOS);
          if (*ptr == ',')
            ReadCSSRules (XML_CSS_Doc[i], NULL, &ptr[9], NULL,
                          TtaGetElementLineNumber (XML_CSS_El[i]), FALSE,
                          XML_CSS_El[i]);
        }
      else
        LoadStyleSheet (XML_CSS_Href[i], XML_CSS_Doc[i],
                        XML_CSS_El[i], NULL, NULL,
                        XML_CSS_Media[i], FALSE);
    }
}

/*----------------------------------------------------------------------
  XmlStyleSheetPi
  ---------------------------------------------------------------------*/
void XmlStyleSheetPi (char *PiData, Element piEl)
{
  int           length, i, j;
  char         *ptr, *end;
  char         *buffer;
  char         *css_href = NULL;
  char          delimitor;
  CSSmedia      css_media;
  CSSInfoPtr    css_info;
  ThotBool      ok;
  ElementType   elType;
  Attribute     attr_css;
  AttributeType attrType;

  length = strlen ((char *)PiData);
  buffer = (char *)TtaGetMemory (length + 1);
  i = 0; j = 0;
  css_media = CSS_ALL;

  /* get the "type" attribute */
  ok = FALSE;
  end = NULL;
  strcpy ((char *)buffer, (char *)PiData);
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
          end = strchr (&ptr[1], delimitor);
          if (end && end[0] != EOS)
            {
              end[0] = EOS;
              if (!strcmp ((char *)&ptr[1], "text/css"))
                ok = TRUE;
            }
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
      strcpy ((char *)buffer, (char *)PiData);
      ptr = strstr (buffer, "media");
      if (ptr)
        {
          ptr = strstr (ptr, "=");
          ptr++;
          /* skip spaces */
          while (*ptr != EOS && *ptr == ' ')
            ptr++;
          if (*ptr != EOS)
            {
              /* locate delimitors */
              delimitor = *ptr;
              end = strchr (&ptr[1], delimitor);
              if (end && *end != EOS)
                {
                  *end = EOS;
                  css_media = CheckMediaCSS (&ptr[1]);
                }
            }
        }
    }
   
  if (ok)
    {
      /* get the "href" attribute */
      end = NULL;
      strcpy ((char *)buffer, (char *)PiData);
      ptr = strstr (buffer, "href");
      if (ptr)
        {
          ptr = strstr (ptr, "=");
          ptr++;
          while (ptr[0] != EOS && ptr[0] == ' ')
            ptr++;
          if (ptr[0] != EOS &&
              /* don't manage a document used by make book */
              (DocumentMeta[XMLcontext.doc] == NULL ||
               DocumentMeta[XMLcontext.doc]->method != CE_MAKEBOOK))
            {
              delimitor = ptr[0];
              css_href = (char *)TtaGetMemory (length + 1);
              end = strchr (&ptr[1], delimitor);
              if (end && end[0] != EOS && css_href)
                {
                  end[0] = EOS;
                  strcpy ((char *)css_href, (char *)&ptr[1]);
                  css_info = NULL;
                  /* get the CSS URI in UTF-8 */
                  /*css_href = ReallocUTF8String (css_href, XMLcontext.doc);*/
                  /* load the stylesheet file found here ! */
                  ptr = (char *)TtaConvertMbsToByte ((unsigned char *)css_href,
                                                     TtaGetDefaultCharset ());
                  if (ptr)
                    {
                      TtaFreeMemory (css_href);
                      css_href = ptr;
                    }
                  /* Don't apply immediately the style sheet for XML schemas */
                  elType = TtaGetElementType (piEl);
                  if (((strcmp (TtaGetSSchemaName (elType.ElSSchema), "XML") == 0) ||
                       TtaIsXmlSSchema (elType.ElSSchema)) &&
                      XML_CSS_Level < MAX_NS_TABLE)
                    {
                      XML_CSS_Href [XML_CSS_Level] = TtaStrdup (css_href); 
                      XML_CSS_Doc [XML_CSS_Level] = XMLcontext.doc; 
                      XML_CSS_El [XML_CSS_Level] = piEl; 
                      XML_CSS_Media [XML_CSS_Level] = css_media;
                      XML_CSS_Level++;	       
                    }
                  else
                    {
                      LoadStyleSheet (css_href, XMLcontext.doc, piEl,
                                      css_info, NULL, css_media, FALSE);
                      /* Create a specific attribute for XHTML documents */
                      if (strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
                        {
                          attrType.AttrSSchema = elType.ElSSchema;
                          attrType.AttrTypeNum = HTML_ATTR_is_css;
                          attr_css = TtaGetAttribute (piEl, attrType);
                          if (!attr_css)
                            {
                              attr_css = TtaNewAttribute (attrType);
                              TtaAttachAttribute (piEl, attr_css, XMLcontext.doc);
                              TtaSetAttributeText (attr_css, (char *)"text/css",
                                                   piEl, XMLcontext.doc);
                            }
                        }
                    }
                }
              TtaFreeMemory (css_href);
            }
        }
    }

  TtaFreeMemory (buffer);
}

/*----------------------------------------------------------------------
  CreateXmlPi
  Create a Processing Instruction element into the Thot tree.
  ---------------------------------------------------------------------*/
static void CreateXmlPi (char *piTarget, char *piData)
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
      length = strlen ((char *)piTarget) + strlen ((char *)piData);
      length++;
      piValue = (char *)TtaGetMemory (length + 2);
      strcpy ((char *)piValue, (char *)piTarget);
      strcat ((char *)piValue, " ");
      strcat ((char *)piValue, (char *)piData);
      i = 0;
      j = 0; /* parsed length */
      ptr = (unsigned char *)&piValue[i];
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
              buffer = HandleXMLstring ((unsigned char *)ptr, &l, piLineEl, FALSE);
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
                  TtaSetTextContent (piLeaf, (unsigned char *)buffer, Latin_Script, XMLcontext.doc);
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
              ptr = (unsigned char *)&piValue[i];
            }
        }
      (*(Proc3)(CurrentParserCtxt->ElementComplete)) (
                                                      (void *)&XMLcontext,
                                                      (void *)piEl,
                                                      (void *)&error);
      XMLcontext.lastElementClosed = TRUE;
      TtaFreeMemory (piValue);
    }
   
  /* Call the treatment associated to that PI */
  /* For the moment, Amaya only supports the "xml-stylesheet" PI */
  if (!strcmp ((char *)piTarget, "xml-stylesheet"))
    XmlStyleSheetPi (piData, piEl);
#ifdef TEMPLATES
  else if (!strcmp ((char *)piTarget, "xtiger"))
    {
      Element      root;
      // Lock the root of a template instance */
      root =	TtaGetMainRoot (XMLcontext.doc);
      TtaSetAccessRight (root, ReadOnly, XMLcontext.doc);
    }
#endif /* TEMPLATES */
  /* Warnings about PI are no longer reported */
}
/*--------------------  PI  (end)  ---------------------------------*/


/*-----------  EXPAT handlers associated with Amaya  ---------------*/

/*--------------------------------------------------------------------
  Hndl_CdataStart
  Handlers that get called at the beginning of a CDATA section
  ------------------------------------------------------------------*/
static void Hndl_CdataStart (void *userData)

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
      buffer = HandleXMLstring ((unsigned char *)ptr, &length, XMLcontext.lastElement, TRUE);
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
      ParseDoctypeContent ("<!--", 4);
      ParseDoctypeContent ((char *)buffer, strlen ((char *)buffer));
      ParseDoctypeContent ("-->", 3);
      return;
    }
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
#ifdef EXPAT_PARSER_DEBUG
  int i;
  printf ("Hndl_DefaultExpand - length = %d - '", length);
  for (i=0; i<length; i++)
    printf ("%c", data[i]);
  printf ("'\n");
#endif /* EXPAT_PARSER_DEBUG */
}

/*----------------------------------------------------------------------
  Hndl_DoctypeStart
  Handler for the start of the DOCTYPE declaration.
  It is called when the name of the DOCTYPE is encountered.
  ----------------------------------------------------------------------*/
static void Hndl_DoctypeStart (void *userData,
                               const XML_Char *doctypeName,
                               const XML_Char *sysid,
                               const XML_Char *pubid,
                               int   has_internal_subset)
{
#ifdef EXPAT_PARSER_DEBUG
  printf ("Hndl_DoctypeStart\n");
  printf ("  name       : %s\n", doctypeName);
  printf ("  sysid      : %s\n", sysid);
  printf ("  pubid      : %s\n", pubid);
  printf ("  int.subset : %d\n", has_internal_subset);
#endif /* EXPAT_PARSER_DEBUG */
  /* The content of this doctype has not to be parsed */
  if (!VirtualDoctype)
    {
      CreateDoctypeElement ((char*) doctypeName, (char*) sysid, (char*) pubid);
      WithinDoctype = TRUE;
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
      ParseDoctypeContent (">", 1);
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
      if (strncmp ((char*) name, "SUBTREE_ROOT", 12) != 0)
        {
          /* Treatment called at the beginning of a start tag */
          StartOfXmlStartElement ((char*) name);
          /* We save the current element context */
          elementParserCtxt = CurrentParserCtxt;
	   
          /*-------  Treatment of the attributes -------*/
          while (*attlist != NULL)
            {
              /* Create the corresponding Thot attribute */
              attrName = (unsigned char *)TtaGetMemory ((strlen ((char *)*attlist)) + 1);
              strcpy ((char *)attrName, (char *)*attlist);
#ifdef EXPAT_PARSER_DEBUG
              printf ("  attr %s :", attrName);
#endif /* EXPAT_PARSER_DEBUG */
              EndOfAttributeName ((char *)attrName);
	       
              /* Restore the element context */
              /* It occurs if the attribute name is unknown */
              if (CurrentParserCtxt == NULL)
                CurrentParserCtxt = elementParserCtxt;
	       
              /* Filling of the attribute value */
              attlist++;
              if (*attlist != NULL)
                {
                  attrValue = (unsigned char *)TtaGetMemory ((strlen ((char *)*attlist)) + 1);
                  strcpy ((char *)attrValue, (char *)*attlist);
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
              CurrentParserCtxt = elementParserCtxt;
            }
	   
          /*----- Treatment called at the end of a start tag -----*/
          EndOfXmlStartElement ((char*) name);
	   
          /*----- We are immediately after a start tag -----*/
          ImmediatelyAfterTag = TRUE;
	   
          /* Initialize the root element */
          if (XMLRootName == NULL)
            {
              /* This is the first parsed element */
              XMLRootName = TtaStrdup ((char*) name);
            }
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
  if ((strncmp ((char*) name, "SUBTREE_ROOT", 12) != 0) && 
      (XMLRootName != NULL) && !XMLrootClosed)
    {
      EndOfXmlElement ((char*) name); 
      /* Is it the end tag of the root element ? */
      if (!strcmp ((char *)XMLRootName, (char*) name) &&
          stackLevel == 1 && !PARSING_BUFFER)
        XMLrootClosed = TRUE;
    }
  if (!strncmp ((char*) name, "SUBTREE_ROOT", 12) && PARSING_BUFFER)
    XMLrootClosed = TRUE;
}

/*----------------------------------------------------------------------
  Hndl_ExternalEntityRef
  Handler for external entity references.
  his handler is also called for processing an external DTD subset.
  ----------------------------------------------------------------------*/
static int     Hndl_ExternalEntityRef(XML_Parser p,
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
static int Hndl_NotStandalone (void *userData)

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
static void Hndl_PI (void *userData, const XML_Char *target,
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
      ParseDoctypeContent ("<?", 2);
      buffer = (unsigned char *) target;
      ParseDoctypeContent ((char *)buffer, strlen ((char *)buffer));
      ParseDoctypeContent (" ", 1);
      buffer = (unsigned char *) pidata;
      ParseDoctypeContent ((char *)buffer, strlen ((char *)buffer));
      ParseDoctypeContent ("?>", 2);
      return;
    }
  CreateXmlPi ((char*) target, (char*) pidata);
}

/*----------------------------------------------------------------------
  Hndl_SkippedEntityHandler
  This is called in two situations:
  1) An entity reference is encountered for which no declaration
  has been read *and* this is not an error.
  2) An internal entity reference is read, but not expanded, because
  XML_SetDefaultHandler has been called.
  Note: skipped parameter entities in declarations and skipped general
  entities in attribute values cannot be reported, because
  the event would be out of sync with the reporting of the
  declarations or attribute values
  ----------------------------------------------------------------------*/
static void Hndl_SkippedEntityHandler(void *userData,
                                      const XML_Char *entityName,
                                      int   is_parameter_entity)
{
  int     length;
  char   *buffer;
#ifdef EXPAT_PARSER_DEBUG
  printf ("Hndl_SkippedEntityHandler\n");
  printf ("  entity name : %s\n", entityName);
  printf ("  parameter   : %d\n", is_parameter_entity);
#endif /* EXPAT_PARSER_DEBUG */
  length = strlen ((char *)entityName);
  if (WithinDoctype)
    ParseDoctypeContent ((char *)entityName, length);
  else
    {
      buffer = (char *)TtaGetMemory (length + 3);
      if (buffer != NULL)
        {
          strcpy ((char *)buffer, "&");
          strcat ((char *)buffer, (char*) entityName);
          strcat ((char *)buffer, ";");
          CreateXmlEntity (buffer, length+2);
          TtaFreeMemory (buffer);
        }
    }
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
                 (unsigned char *)TtaGetMessage (AMAYA, AM_UNKNOWN_ENCODING), -1);
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

/*----------------------------------------------------------------------
  Hndl_XML_XmlDeclHandler
  The XML declaration handler is called for *both* XML declarations
  and text declarations. The way to distinguish is that the version
  parameter will be NULL for text declarations. The encoding
  parameter may be NULL for XML declarations. The standalone
  parameter will be -1, 0, or 1 indicating respectively that there
  was no standalone parameter in the declaration, that it was given
  as no, or that it was given as yes.
  ----------------------------------------------------------------------*/
static void Hndl_XmlDeclHandler (void  *userData,
                                 const XML_Char *version,
                                 const XML_Char *encoding,
                                 int   standalone)
{
#ifdef EXPAT_PARSER_DEBUG
  printf ("Hndl_XmlDeclHandler\n");
  printf ("  version    : %s\n", version);
  printf ("  encoding   : %s\n", encoding);
  printf ("  standalone : %d\n", standalone);
#endif /* EXPAT_PARSER_DEBUG */
}

/*---------------- End of Handler definition ----------------*/

/*----------------------------------------------------------------------
  FreeXmlParser
  Frees all ressources associated with the XML parser.
  ----------------------------------------------------------------------*/
void FreeXmlParserContexts (void)

{
  PtrParserCtxt  ctxt, nextCtxt;
  int            i;

  /* Free parser contexts */
  ctxt = FirstParserCtxt;
  while (ctxt != NULL)
    {
      nextCtxt = ctxt->NextParserCtxt;
      TtaFreeMemory (ctxt->SSchemaName);
      TtaFreeMemory (ctxt->UriName);
      TtaFreeMemory (ctxt);
      ctxt = nextCtxt;
    }
  FirstParserCtxt = NULL;
  CurrentParserCtxt = NULL;

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
   
  /* Free XML Style Sheets table */
  for (i = 0; i < XML_CSS_Level; i++)
    {
      if (XML_CSS_Href[i])
        {
          TtaFreeMemory (XML_CSS_Href[i]);
          XML_CSS_Href[i] = NULL;
        }
    }
   
  if (XMLRootName != NULL)
    {
      TtaFreeMemory (XMLRootName);
      XMLRootName = NULL;
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
  XML_SetParamEntityParsing (Parser, (enum XML_ParamEntityParsing)paramEntityParsing);
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
static void InitializeExpatParser (CHARSET charset)
{  
  int        paramEntityParsing;

  /* Enable parsing of parameter entities */
  paramEntityParsing = XML_PARAM_ENTITY_PARSING_UNLESS_STANDALONE;

  /* Construct a new parser with namespace processing */
  /* accordingly to the document encoding */
  /* If that encoding is unknown, we don''t parse the document */
  if (charset == UNDEFINED_CHARSET)
    {
      /* Default encoding for XML documents */
      Parser = XML_ParserCreateNS ("UTF-8", NS_SEP);
      TtaSetDocumentCharset (XMLcontext.doc, UTF_8, TRUE);
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
           charset == SHIFT_JIS    || charset == GB_2312)
    /* buffers will be converted to UTF-8 by Amaya */
    Parser = XML_ParserCreateNS ("UTF-8", NS_SEP);
  else
    {
      XMLUnknownEncoding = TRUE;
      Parser = XML_ParserCreateNS ("ISO-8859-1", NS_SEP);
      XmlParseError (errorEncoding,
                     (unsigned char *)TtaGetMessage (AMAYA, AM_UNKNOWN_ENCODING), -1);
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
  
  /* Set an handler for notation declarations */
  XML_SetNotationDeclHandler (Parser,
                              Hndl_Notation);
  
  /* Set an handler for no 'standalone' document */
  XML_SetNotStandaloneHandler (Parser,
                               Hndl_NotStandalone);

  /* Controls parsing of parameter entities */
  XML_SetParamEntityParsing (Parser,
                             (enum XML_ParamEntityParsing)paramEntityParsing);
  
  /* Set an handler for processing instructions */
  XML_SetProcessingInstructionHandler (Parser,
                                       Hndl_PI);
  
  /* Set an handler to deal with encodings other than the built in */
  XML_SetUnknownEncodingHandler (Parser,
                                 Hndl_UnknownEncoding, 0);
  
  /* Set an handler that receives declarations of unparsed entities */
  XML_SetUnparsedEntityDeclHandler (Parser,
                                    Hndl_UnparsedEntity);
  
  /* Set an handler that is called for XML declarations */
  XML_SetXmlDeclHandler (Parser,
                         Hndl_XmlDeclHandler);
  
  /* Set a skipped entity handler */
  XML_SetSkippedEntityHandler (Parser,
                               Hndl_SkippedEntityHandler);
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
  XMLRootName = NULL;
  XMLrootClosed = FALSE;
  ParsingCDATA = FALSE;
  VirtualDoctype = FALSE;
  ShowParsingErrors =  TRUE;
  PARSING_BUFFER = FALSE;

  HtmlLineRead = 0;
  HtmlCharRead = 0;
  
  /* initialize the stack of opened elements */
  stackLevel = 1;
  Ns_Level = 0;
  CurNs_Level = 0;
  XML_CSS_Level = 0;
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
          buffer = (char *)TtaGetMemory (length + 1);
          TtaGiveTextAttributeValue (attrold, buffer, &length);
          TtaSetAttributeText (attrnew, (char *)buffer, elnew, doc);
          TtaFreeMemory (buffer);
          break;
        case 3:	/* reference */
          break;
        }     
    }
}

/*-----------------------------------------------------------------------------
  SetExternalElementType
  Initial treatment relative to the element which is the parent of the
  external sub-tree (document)
  use_ref is TRUE when we are parse a reference to an external use svg element
  ------------------------------------------------------------------------------*/
static Element  SetExternalElementType (Element el, Document doc,
                                        ThotBool *use_ref)
{
  ElementType   elType, parentType;
  Element       parent, elemElement, elemContent;
  AttributeType attrType;
  Attribute     attr;
  ThotBool      oldStructureChecking;
 
  elemElement = NULL;
  elemContent = NULL;
  elType = TtaGetElementType (el);
  *use_ref = FALSE;

  /* Disable structure checking */
  oldStructureChecking = TtaGetStructureChecking (doc);
  TtaSetStructureChecking (FALSE, doc);

  if ((strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0) &&
      (elType.ElTypeNum == HTML_EL_PICTURE_UNIT))
    {
      /* We are parsing an external picture within a HTML document */
      parent = TtaGetParent (el);
      parentType = TtaGetElementType (parent);
      if (parentType.ElTypeNum == HTML_EL_Object)
        {
          /* Create an External_Object_Content element */
          elType.ElTypeNum = HTML_EL_External_Object_Content;
          elemContent = TtaNewElement (doc, elType);
          if (elemContent != NULL)
            {
              attrType.AttrSSchema = elType.ElSSchema;
              TtaInsertSibling (elemContent, el, FALSE, doc);
              /* Remove the IntHeightPercent Thot attributes: this attribute
                 does not make sense for Thot. */
              attrType.AttrTypeNum = HTML_ATTR_IntHeightPercent;
              attr = TtaGetAttribute (el, attrType);
              if (attr != NULL)
                TtaRemoveAttribute (el, attr, doc);
              /* Attach the attributes to that new element */
              MoveExternalAttribute (el, elemContent, doc);
              /* Remove the PICTURE_UNIT element form the tree */
              TtaDeleteTree (el, doc);
            }
        }
      else
        {
          /* create an External_Object element instead of the PICTURE element */
          elType.ElTypeNum = HTML_EL_External_Object;
          elemElement = TtaNewElement (doc, elType);
          if (elemElement != NULL)
            {
              TtaInsertSibling (elemElement, el, FALSE, doc);
              /* Attach the attributes to that new element */
              MoveExternalAttribute (el, elemElement, doc);
              /* create an External_ObjectContent element */
              elType.ElTypeNum = HTML_EL_External_Object_Content;
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
  else if ((strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0) &&
           elType.ElTypeNum == HTML_EL_IFRAME)
    {
      /* We are parsing an iframe element within a HTML document*/
      /* Is there an Iframe_Src_Content element? */
      elType.ElTypeNum = HTML_EL_Iframe_Src_Content;
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
      /* We are parsing a SVG document */
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
           (elType.ElTypeNum == SVG_EL_use_ ||
            elType.ElTypeNum == SVG_EL_tref))
    {
      /* We are parsing an external SVG use element */
      elemContent = el;
      *use_ref = TRUE;
    }

  /* Restore the structure checking */
  TtaSetStructureChecking (oldStructureChecking, doc);

  return elemContent;
}

/*---------------------------------------------------------------------------
  ParseExternalDocument
  Parse a document called from an other document.
  The new file is parsed in an external document and then pasted 
  into the main document
  Return TRUE if the parsing of the external document doesn't detect errors.
  ---------------------------------------------------------------------------*/
void ParseExternalDocument (char *fileName, char *originalName, Element el,
                            ThotBool isclosed, Document doc, Language lang,
                            const char *typeName)
{
  ElementType   elType;
  Element       parent, oldel;
  Element       copy = NULL;
  Element       idEl = NULL, extEl = NULL;
  AttributeType extAttrType;
  DocumentType  thotType;
  Document      externalDoc = 0;
  CHARSET       charset;
  NotifyElement event;
  DisplayMode   dispMode;
  gzFile        infile;
  int           parsingLevel, extraProfile, saveDocNet;
  char          charsetname[MAX_LENGTH];
  char          type[NAME_LENGTH];
  char         *extUseUri = NULL, *extUseId = NULL, *s = NULL, *htmlURL = NULL;
  char         *schemaName = NULL, *tempName = NULL, *ptr = NULL;
  ThotBool      xmlDec, docType, isXML, useMath, isKnown;
  ThotBool      savParsingError;
  ThotBool      use_ref = FALSE;
  ThotBool      oldStructureChecking;

  if (fileName == NULL)
    return;

  /* Avoid too many redisplay */
  dispMode = TtaGetDisplayMode (doc);
  TtaSetDisplayMode (doc, NoComputedDisplay);

  /* General initialization */
  RootElement = NULL;
  if (typeName != NULL &&
      ((strcmp ((char *)typeName, "SVG") == 0) ||
       (strcmp ((char *)typeName, "MathML") == 0) ||
       (strcmp ((char *)typeName, "HTML") == 0)))
    {
      /* We are parsing an external html, svg or mathml document */
      extEl = SetExternalElementType (el, doc, &use_ref);
      if (extEl == NULL)
        return;

      /* Create a new document with no presentation schema */
      /* and load the external document */
      strcpy ((char *)type, (char *)typeName);
      externalDoc = TtaNewDocument (type, "tmp");
      if (externalDoc == 0)
        return;
      else
        {
          DocumentMeta[externalDoc] = DocumentMetaDataAlloc ();
          strcat (type, "P");
          TtaSetPSchema (externalDoc, type);
          RootElement = TtaGetMainRoot (externalDoc);
          InitializeXmlParsingContext (externalDoc, RootElement, FALSE, FALSE);
          /* Set the document reference  (used for local CSS)*/
          /* Disable structure checking for the external document*/
          TtaSetStructureChecking (FALSE, externalDoc);
          /* Delete all element except the root element */
          parent = TtaGetFirstChild (RootElement);
          // look for the root element (HTML, SVG, etc.)
          RootElement = TtaGetRootElement (externalDoc);
          while (parent && parent != RootElement)
            {
              oldel = parent;
              TtaNextSibling (&parent);
              TtaDeleteTree (oldel, externalDoc);
            }
        }
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
    }
  
  /* specific Initialization */
  XMLcontext.language = lang;
  if (externalDoc != 0)
    DocumentSSchema = TtaGetDocumentSSchema (externalDoc);
  else
    DocumentSSchema = TtaGetDocumentSSchema (doc);
  
  /* Set document URL */
  s = TtaStrdup (originalName);
  if (DocumentURLs[externalDoc])
    {
      TtaFreeMemory (DocumentURLs[externalDoc]);
      DocumentURLs[externalDoc] = NULL;
    }
  DocumentURLs[externalDoc] = s;
  
  tempName = (char *)TtaGetMemory (strlen ((char *)fileName) + 1);
  if (use_ref)
    {
      /* We are parsing an external reference for a 'use' svg element */
      extUseUri = (char *)TtaGetMemory (strlen ((char *)originalName) + 1);
      strcpy ((char *)extUseUri, (char *)originalName);
      /* Extract the ID target */
      if ((ptr = strrchr (extUseUri, '#')) != NULL)
        {
          *ptr = EOS;
          ptr++;
          extUseId = (char *)TtaGetMemory ((strlen ((char *)ptr) + 1));
          strcpy ((char *)extUseId, (char *)ptr);
        }
      if (TtaFileExist (fileName))
        strcpy ((char *)tempName, (char *)fileName);
      else if (TtaFileExist (extUseUri))
        strcpy ((char *)tempName, (char *)extUseUri);
      else
        {
          TtaFreeMemory (tempName);
          tempName = NULL;
        }
    }
  else
    strcpy ((char *)tempName, (char *)fileName);

  // Ignore errors of imported documents
  savParsingError = ShowParsingErrors;
  ShowParsingErrors = FALSE;
  IgnoreErrors = TRUE;
  charset = TtaGetDocumentCharset (doc);
  /* For XML documents, the default charset is ISO_8859_1 */
  if (charset == UNDEFINED_CHARSET && !DocumentMeta[doc]->xmlformat)
    charset = ISO_8859_1;
 
  if (tempName)
    {
      /* Parse the file and build the external document */
      infile = TtaGZOpen (tempName);
      if (infile != NULL)
        {
          /* Check if there is an xml declaration with a charset declaration */
          if (tempName != EOS)
            CheckDocHeader (tempName, &xmlDec, &docType, &isXML, &useMath, &isKnown,
                            &parsingLevel, &charset, charsetname, &thotType, &extraProfile);
	  
          /* Parse the external file */
          DocumentMeta[externalDoc]->compound = FALSE;
          if (!strcmp ((char *)typeName, "HTML"))
            {
              // Use the html parser as the document could be invalid 
              DocumentMeta[externalDoc]->xmlformat = FALSE;
              htmlURL = (char *)TtaGetMemory (strlen ((char *)s) + 1);
              if (htmlURL != NULL)
                {
                  strcpy ((char *)htmlURL, (char *)s);
                  ParseExternalHTMLDoc (externalDoc, (FILE *)infile, charset, htmlURL);
                  TtaFreeMemory (htmlURL);
                }
            } 
          else
            {
              DocumentMeta[externalDoc]->xmlformat = TRUE;
              /* Initialize parser context */
              if (FirstParserCtxt == NULL)
                InitXmlParserContexts ();
              if (schemaName != NULL)
                ChangeXmlParserContextByDTD (schemaName);
              else
                ChangeXmlParserContextByDTD (typeName);
              /* Expat initialization */
              InitializeExpatParser (charset);
              /* Expat parsing */
              XmlParse ((FILE *)infile, charset, &xmlDec, &docType);
              /* Free expat parser */ 
              FreeXmlParserContexts ();
              FreeExpatParser ();
            }
          TtaGZClose (infile);
        } 
    }

  if (externalDoc != 0 && externalDoc != doc)
    {
      /* Disable structure checking for the main document */
      oldStructureChecking = TtaGetStructureChecking (doc);
      TtaSetStructureChecking (FALSE, doc);
      if (use_ref)
        {
          /* Move the target element of the external document
             as a sub-tree of the element extEl in the source document */
          /* Search the target element */
#ifdef _SVG
          extAttrType.AttrSSchema = TtaGetSSchema ("SVG", externalDoc);
          if (extAttrType.AttrSSchema)
            /* This document uses the SVG schema */
            {
              extAttrType.AttrTypeNum = SVG_ATTR_id;
              idEl = GetElemWithAttr (externalDoc, extAttrType, extUseId, NULL, NULL);
            }
          /* Do the actual copy */
          if (idEl)
            {
              elType = TtaGetElementType (extEl);
              if (elType.ElTypeNum == SVG_EL_tref &&
                  elType.ElSSchema == extAttrType.AttrSSchema)
                /* it's a tref element: do a "flat" copy */
                CopyTRefContent (idEl, extEl, doc);
              else
                {
                  /* Copy the external sub-tree into the main document*/
                  copy = TtaCopyTree (idEl, externalDoc, doc, extEl);
                  TtaInsertFirstChild (&copy, extEl, doc);
		  if (elType.ElTypeNum == SVG_EL_use_ &&
		      elType.ElSSchema == extAttrType.AttrSSchema)
		    TtaCopyGradientUse (copy);
                }
            }
#endif /* _SVG */
        }
      else
        {
          /* Add the corresponding nature to the main document */
          elType.ElSSchema = TtaGetSSchema (typeName, doc);
          if (elType.ElSSchema == NULL)
            TtaNewNature (doc, TtaGetDocumentSSchema (doc), NULL, typeName,
                          type);

          /* Paste the external document */
          if (strcmp ((char *)typeName, "HTML") == 0)
            {
              /* XHTML documents */
              /* Handle character-level elements which contain block-level elements */
              CheckBlocksInCharElem (externalDoc);
              /* For (X)HTML documents, we paste the BODY element */
              elType.ElSSchema = TtaGetSSchema ("HTML", externalDoc);
              elType.ElTypeNum = HTML_EL_HEAD;
              idEl = TtaSearchTypedElement (elType, SearchForward, RootElement);
              TtaDeleteTree (idEl, externalDoc);
              idEl = TtaGetRootElement (externalDoc);
            }
          else
            idEl = TtaGetRootElement (externalDoc);
          /* Copy the external sub-tree into the main document*/
          copy = TtaCopyTree (idEl, externalDoc, doc, extEl);
          TtaInsertFirstChild (&copy, extEl, doc);
          /* Update the Images and the URLs in the pasted sub-tree */
          event.event = TteElemPaste;
          event.document = doc;
          event.element = copy;
          event.elementType.ElSSchema = NULL;
          event.elementType.ElTypeNum = 0; /* tell UpdateURLsInSubtree not to
                                              change IDs through MaqueUniqueName */
          event.position = externalDoc;
          event.info = 0;
          UpdateURLsInSubtree(&event, copy);
        }
      /* Move presentation-schema extensions of the external document */
      /* to the sub-tree of which 'extEl' is the root */
      /* This allow to enable the style sheets attached to the external doc */
      if (copy)
        {
          TtaMoveDocumentExtensionsToElement (externalDoc, copy);
          EmbedStyleSheets (externalDoc, doc);
        }

      /* Remove the ParsingErrors file */
      RemoveParsingErrors (externalDoc);
      /* Restore the structure checking for the main document*/
      TtaSetStructureChecking (oldStructureChecking, doc);
    }

  /* Restore ParsingError indicator */
  IgnoreErrors = FALSE;
  ShowParsingErrors = savParsingError;

  /* Delete the external document */
  if (externalDoc != 0 && externalDoc != doc)
    {
      FreeDocumentResource (externalDoc);
      TtaCloseDocument (externalDoc);
      TtaFreeMemory (DocumentURLs[externalDoc]);
      DocumentURLs[externalDoc] = NULL;
    }

  /* Restore the display mode */
  if (DocumentURLs[doc])
    TtaSetDisplayMode (doc, dispMode);

  if (docURL)
    {
      TtaFreeMemory (docURL);
      docURL = NULL;
    }
  TtaFreeMemory (tempName);
  TtaFreeMemory (extUseUri);
  TtaFreeMemory (extUseId);
  if (extEl)
    {
      /* Fetch and display the recursive images */
      /* modify the net status */
      saveDocNet = DocNetworkStatus[doc];
      DocNetworkStatus[doc] = AMAYA_NET_ACTIVE;
      FetchAndDisplayImages (doc, AMAYA_LOAD_IMAGE, extEl);
      DocNetworkStatus[doc] = saveDocNet;
      /* Make the external element not editable */
      TtaSetAccessRight (extEl, ReadOnly, doc);
    }
  return;
}

/*----------------------------------------------------------------------
  ParseXmlBuffer
  Parse a XML sub-tree given in a buffer and complete the
  corresponding Thot abstract tree.
  Return TRUE if the parsing of the buffer has no error.
  ----------------------------------------------------------------------*/
ThotBool ParseXmlBuffer (char *xmlBuffer, Element el, ThotBool isclosed,
                         Document doc, Language lang, char *typeName)
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
  if (FirstParserCtxt == NULL)
    InitXmlParserContexts ();
 
  /* general initialization */
  RootElement = NULL;
  if (isclosed)
    parent = TtaGetParent (el);
  else
    parent = el;

  // skip Template elements
  do
    {
      elType = TtaGetElementType (parent);
      parent = TtaGetParent (parent);
    }
  while (!strcmp (TtaGetSSchemaName(elType.ElSSchema), "Template"));
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
                   (unsigned char *) XML_ErrorString (XML_GetErrorCode (Parser)), 0);
  
  /* Parse the input XML buffer and complete the Thot document */
  if (!XMLNotWellFormed)
    {
      /* We create a virtual root for the sub-tree to be parsed */
      tmpLen = (strlen ((char *)xmlBuffer)) + 1;
      tmpLen = tmpLen + 14 + 15 + 1;
      transBuffer = (char *)TtaGetMemory (tmpLen);
      strcpy ((char *)transBuffer, "<SUBTREE_ROOT>");
      strcat ((char *)transBuffer, (char *)xmlBuffer);
      strcat ((char *)transBuffer, "</SUBTREE_ROOT>");
      tmpLen = strlen ((char *)transBuffer);

      PARSING_BUFFER = TRUE;    
      if (!XML_Parse (Parser, transBuffer, tmpLen, 1))
        {
          if (!XMLrootClosed)
            XmlParseError (errorNotWellFormed,
                           (unsigned char *) XML_ErrorString (XML_GetErrorCode (Parser)), 0);
        }
      PARSING_BUFFER = FALSE;    
      if (transBuffer != NULL)   
        TtaFreeMemory (transBuffer);   
    }
  
  /* Free expat parser */ 
  FreeXmlParserContexts ();
  FreeExpatParser ();

  /* Handle character-level elements which contain block-level elements */
  if ((schemaName != NULL) &&
      (strcmp ((char *)schemaName, "HTML") == 0))
    {
      TtaSetStructureChecking (FALSE, doc);
      CheckBlocksInCharElem (doc);
      TtaSetStructureChecking (TRUE, doc);
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
ThotBool ParseIncludedXml (FILE *infile, char **infileBuffer, int infileBufferLength,
                           ThotBool *infileEnd, ThotBool *infileNotToRead,
                           char *infilePreviousBuffer, int *infileLastChar,
                           char *htmlBuffer, int *index,
                           int *nbLineRead, int *nbCharRead,
                           char *typeName, Document doc,
                           Element  *el,
                           ThotBool *isclosed,
                           Language  lang)
{
  int          tmpLen = 0;
  int          offset = 0;
  int          i;
  ElementType  elType;
  char        *schemaName;
  char        *tmpBuffer = NULL;
  CHARSET      charset;
  ThotBool     endOfParsing = FALSE;
  ThotBool     found;

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
  // it's a compound document
  if (DocumentMeta[doc])
    DocumentMeta[doc]->compound = TRUE;

  /* Initialize  counters */
  ExtraLineRead = 0;
  ExtraOffset = 0;
  HtmlLineRead = *nbLineRead;
  HtmlCharRead = *nbCharRead;

  /* Expat initialization */
  charset = TtaGetDocumentCharset (doc);
  /* For HTML documents, the default charset is ISO_8859_1 */
  if (charset == UNDEFINED_CHARSET)
    charset = ISO_8859_1;
  InitializeExpatParser (charset);

  /* initialize all parser contexts */
  if (FirstParserCtxt == NULL)
    InitXmlParserContexts ();
  if (typeName != NULL)
    ChangeXmlParserContextByDTD (typeName);
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
                   (unsigned char *) XML_ErrorString (XML_GetErrorCode (Parser)), 0);
  else
    {
      ExtraLineRead = XML_GetCurrentLineNumber (Parser);
      ExtraOffset = XML_GetCurrentByteIndex (Parser);
    }

  /* Parse the input file or HTML buffer and complete the Thot document */

  /* If htmlBuffer isn't null, we are parsing a XML sub-tree */
  /* included in a transformation */
  if (htmlBuffer)
    {
      /* parse the HTML buffer */
      tmpBuffer = TtaStrdup (&htmlBuffer[*index]);
      tmpLen = strlen ((char *)tmpBuffer);
      if (!XML_Parse (Parser, tmpBuffer, tmpLen, 1))
        if (!XMLrootClosed)
          XmlParseError (errorNotWellFormed,
                         (unsigned char *) XML_ErrorString (XML_GetErrorCode (Parser)), 0);
    }
  else
    {
      /* read and parse the HTML file sequentialy */
      while (!endOfParsing && !XMLNotWellFormed)
        {
          if (tmpBuffer)
            {
              TtaFreeMemory (tmpBuffer);
              tmpBuffer = NULL;
            }
          if (*index == 0)
            {
              if (*infileNotToRead)
                // work on the previous buffer
                *infileNotToRead = FALSE;
              else
                {
                  GetNextHTMLbuffer (infile, infileEnd, infileBuffer, infileLastChar);
                  if (*infileEnd)
                    endOfParsing = TRUE;
                }
            }

          if (*infileNotToRead)
            {
              tmpLen = strlen ((char *)infilePreviousBuffer) - *index;
              tmpBuffer = (char *)TtaGetMemory (tmpLen);
              for (i = 0; i < tmpLen; i++)
                tmpBuffer[i] = infilePreviousBuffer[*index + i];	  
            }
          else
            {
              if (endOfParsing)
                tmpLen = *infileLastChar  - *index;
              else
                tmpLen = strlen ((char *)(*infileBuffer)) - *index;
              tmpBuffer = TtaStrdup (&(*infileBuffer)[*index]);
            }
          if (!XML_Parse (Parser, tmpBuffer, tmpLen, endOfParsing))
            {
              if (XMLrootClosed)
                endOfParsing = TRUE;
              else
                XmlParseError (errorNotWellFormed,
                               (unsigned char *) XML_ErrorString (XML_GetErrorCode (Parser)), 0);
            }
          else
            {
              *index = 0;
              ExtraOffset =  ExtraOffset + tmpLen;
            }
        }
    }

  /* return char/lines read */
  if (htmlBuffer == NULL)
    {
      if (XML_GetCurrentLineNumber (Parser) - ExtraLineRead <= 0)
        /* We stay on the same line */
        *nbCharRead += XML_GetCurrentColumnNumber (Parser);
      else
        {
          /* We read at least one new line */
          *nbLineRead = *nbLineRead + XML_GetCurrentLineNumber (Parser) - ExtraLineRead;
          *nbCharRead = XML_GetCurrentColumnNumber (Parser);
        }
    }

  /* We look for the '>' character of the XML end tag */
  offset = XML_GetCurrentByteIndex (Parser) - ExtraOffset - 1;
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
  The parameter skipDec is TRUE when the declaration should be sipped.
  ---------------------------------------------------------------------------*/
static void   XmlParse (FILE *infile, CHARSET charset, ThotBool *xmlDec,
                        ThotBool *xmlDoctype)

{
#define	 COPY_BUFFER_SIZE	1024
  char        bufferRead[COPY_BUFFER_SIZE + 1];
  char       *buffer;
  int         i, j;
  int         res;
  int         tmpLineRead = 0;
  ThotBool    beginning;
  ThotBool    endOfFile = FALSE, okay;
  
  if (infile != NULL)
    endOfFile = FALSE;
  else
    return;

  /* Initialize global counters */
  ExtraLineRead = 0;
  ExtraOffset = 0;
  HtmlLineRead = 0;
  HtmlCharRead = 0;
  /* add a null character at the end of the buffer by security */
  bufferRead[COPY_BUFFER_SIZE] = EOS;
  beginning = TRUE;

  while (!endOfFile && !XMLNotWellFormed && !XMLInvalidToken)
    {
      /* read the XML file */
      res = gzread (infile, bufferRead, COPY_BUFFER_SIZE);
      if (res < 0)
        return;

      if (res < COPY_BUFFER_SIZE)
        {
          endOfFile = TRUE;
          /* add a null character at the end of the buffer by security */
          bufferRead[res] = EOS;
        }

      i = 0;

      if (beginning)
        {
          if ((unsigned char) bufferRead[0] == EOL)
            {
              /* accept a newline before the XML declaration */
              i = 1;
              res = res - 1;
              HtmlLineRead = 1;
            }
          beginning = FALSE;
        }

      if (*xmlDec)
        /* There is an XML declaration */
        /* We look for the first '>' character */
        {
          j = i;
          while ((bufferRead[i] != '>') && i < res)
            i++;
          if (i < res)
            {
              i++;
              res = res - (i-j);
            }
          /* The declaration is skipped */
          *xmlDec = FALSE;
        }

      if (!*xmlDoctype)
        /* There is no DOCTYPE Declaration 
           We include a virtual DOCTYPE declaration so that EXPAT parser
           doesn't stop processing when it finds an external entity */	  
        {
          /* Virtual DOCTYPE Declaration */
          if (!XMLNotWellFormed)
            {
              VirtualDoctype = TRUE;
              tmpLineRead = XML_GetCurrentLineNumber (Parser);
              if (!XML_Parse (Parser, DECL_DOCTYPE, DECL_DOCTYPE_LEN, 0))
                XmlParseError (errorNotWellFormed,
                               (unsigned char *) XML_ErrorString (XML_GetErrorCode (Parser)), 0);
              /* The Doctype is now parsed */
              *xmlDoctype = TRUE;
              ExtraLineRead = ExtraLineRead + XML_GetCurrentLineNumber (Parser) - tmpLineRead;
            }
        }
       
      /* Standard EXPAT processing */
      if (!XMLNotWellFormed)
        {
          okay = TRUE;
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
              charset == SHIFT_JIS    || charset == GB_2312)
            {
              /* convert the original stream into UTF-8 */
              buffer = (char *)TtaConvertByteToMbs ((unsigned char *)&bufferRead[i], charset);
              if (buffer)
                {
                  okay = (XML_Parse(Parser, buffer, strlen ((char *)buffer), endOfFile) != XML_STATUS_ERROR);
                  TtaFreeMemory (buffer);
                }
            }
          else
            okay = (XML_Parse(Parser, &bufferRead[i], res, endOfFile) != XML_STATUS_ERROR);
          if (okay == XML_STATUS_ERROR)
            XmlParseError (errorNotWellFormed,
                           (unsigned char *) XML_ErrorString (XML_GetErrorCode (Parser)), 0);
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
                     char *pathURL, ThotBool withDec,
                     ThotBool withDoctype, ThotBool useMath, ThotBool externalDoc)
{
  Element         el, oldel;
  CHARSET         charset;  
  DisplayMode     dispMode;
  char            tempname[MAX_LENGTH];
  char            temppath[MAX_LENGTH];
  char           *s;
  int             error;
  ThotBool        isXHTML, xmlDec, xmlDoctype;
  ThotBool        isXml = FALSE, isXtiger;

#ifdef TEMPLATES
  // load the referred template if it's an instance
  isXtiger = IsXTiger(pathURL);
 if (!isXtiger)
  Template_CheckAndPrepareInstance(fileName, doc, pathURL);
#endif /* TEMPLATES */

  /* General initialization */
#ifdef ANNOTATIONS
  if (DocumentTypes[doc] == docAnnot)
    /* we search the start of HTML Root element in the annotation struct */
    RootElement = ANNOT_GetHTMLRoot (doc, FALSE);
  else
#endif /* ANNOTATIONS */
    RootElement = TtaGetMainRoot (doc);

  xmlDec = withDec;
  xmlDoctype = withDoctype;
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
  stream = TtaGZOpen (fileName);
  if (stream != 0)
    {
      if (documentName[0] == EOS &&
          !TtaCheckDirectory (documentDirectory))
        {
          strcpy ((char *)documentName, (char *)documentDirectory);
          documentDirectory[0] = EOS;
          s = TtaGetEnvString ("PWD");
          /* Set path on current directory */
          if (s != NULL)
            strcpy ((char *)documentDirectory, (char *)s);
          else
            documentDirectory[0] = EOS;
        }
      TtaAppendDocumentPath (documentDirectory);

      /* Set document URL */
      if (DocumentURLs[doc])
        {
          docURL = (char *)TtaGetMemory (strlen ((char *)DocumentURLs[doc]) + 1);
          strcpy ((char *)docURL, (char *)DocumentURLs[doc]);
        }
      else
        {
          docURL = (char *)TtaGetMemory (strlen ((char *)pathURL) + 1);
          strcpy ((char *)docURL, (char *)pathURL);
        }
      /* Set document URL2 */
      if (docURL)
        {
          docURL2 = (char *)TtaGetMemory (strlen ((char *)docURL) + 1);
          strcpy ((char *)docURL2, (char *)docURL);
        }


      /* Do not check the Thot abstract tree against the structure */
      /* schema while building the Thot document. */
      /* Some valid XHTML documents could be considered as invalid Thot documents */
      /* For example, a <tbody> as a child of a <table> would be considered */
      /* invalid because the Thot SSchema requires a Table_body element in between */
      TtaSetStructureChecking (FALSE, doc);
      /* Set the notification mode for the new document */
      TtaSetNotificationMode (doc, 1);
      dispMode = TtaGetDisplayMode (doc);
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

      // change the type of the root element if needed
      s = TtaGetSSchemaName (DocumentSSchema);
      if (DocumentTypes[doc] == docHTML && strcmp ((char *)s, "HTML"))
        TtaUpdateRootElementType (RootElement, "HTML", doc);
      else if (DocumentTypes[doc] == docSVG && strcmp ((char *)s, "SVG"))
        TtaUpdateRootElementType (RootElement, "SVG", doc);
      else if (DocumentTypes[doc] == docMath && strcmp ((char *)s, "MathML"))
        TtaUpdateRootElementType (RootElement, "MathML", doc);
      else if (DocumentTypes[doc] == docTemplate && strcmp ((char *)s, "Template"))
        TtaUpdateRootElementType (RootElement, "Template", doc);

      /* Initialize all parser contexts if not done yet */
      if (FirstParserCtxt == NULL)
        InitXmlParserContexts ();

      /* Select root context */
      isXHTML = FALSE;
      DocumentSSchema = TtaGetDocumentSSchema (doc);
      s = TtaGetSSchemaName (DocumentSSchema);
      if (strcmp ((char *)s, "HTML") == 0)
        {
          ChangeXmlParserContextByDTD ("HTML");
          isXHTML = TRUE;
        }
      else if (strcmp ((char *)s, "SVG") == 0)
        ChangeXmlParserContextByDTD ("SVG");
      else if (strcmp ((char *)s, "MathML") == 0)
        ChangeXmlParserContextByDTD ("MathML");
      else if (strcmp ((char *)s, "Template") == 0)
        ChangeXmlParserContextByDTD ("Template");
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
      /* load the MathML nature if it's declared plus MathML */
      if (useMath)
        TtaNewNature (doc, DocumentSSchema,  NULL, "MathML", "MathMLP");
      /* Parse the input file and build the Thot tree */
      XmlParse ((FILE*)stream, charset, &xmlDec, &xmlDoctype);

      if (!externalDoc)
        /* Load the style sheets for xml documents */
        LoadXmlStyleSheet (doc);

      /* Completes all unclosed elements */
      if (CurrentParserCtxt != NULL)
        {
          el = XMLcontext.lastElement;
          while (el != NULL)
            {
              (*(Proc3)(CurrentParserCtxt->ElementComplete)) (
                                                              (void *)&XMLcontext,
                                                              (void *)el,
                                                              (void *)&error);
              el = TtaGetParent (el);
            }
        }
      /* Check the Thot abstract tree for XHTML documents */
      if (isXHTML)
        {
          CheckAbstractTree (XMLcontext.doc, isXtiger);
          if (MapAreas[doc])
            ChangeAttrOnRoot (doc, HTML_ATTR_ShowAreas);
        }

      FreeExpatParser ();
      FreeXmlParserContexts ();
      TtaGZClose (stream);
      if (docURL)
        {
          TtaFreeMemory (docURL);
          docURL = NULL;
        }
      if (docURL2)
        {
          TtaFreeMemory (docURL2);
          docURL2 = NULL;
        }
      
      /* Display the document */
      if (!externalDoc)
        {
          /* if (DocumentTypes[doc] == docHTML) */
          /* Load specific user style only for an (X)HTML document */
          /* For a generic XML document, it would create new element types
             in the structure schema, one for each type appearing in a
             selector in the User style sheet */
          LoadUserStyleSheet (doc);
          TtaSetDisplayMode (doc, dispMode);
          UpdateStyleList (doc, 1);
        }

      /* Check the Thot abstract tree against the structure schema. */
      TtaSetStructureChecking (TRUE, doc);
      DocumentSSchema = NULL;
    }

  // if some included HTML elements affected HTML variables
  ClearHTMLParser (); 
  TtaSetDocumentUnmodified (doc);
}

/* end of module */
