/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2000
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * Xml2thot initializes and launches the Expat parser and processes all
 * events sent by Expat. It builds the Thot abstract tree corresponding
 * to an XML file.
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
#include "css_f.h"
#include "HTMLactions_f.h"
#include "HTMLedit_f.h"
#include "HTMLimage_f.h"
#include "HTMLtable_f.h"
#include "HTMLimage_f.h"
#include "UIcss_f.h"
#include "fetchHTMLname.h"

#include "fetchHTMLname_f.h"
#include "fetchXMLname_f.h"
#include "html2thot_f.h"
#include "Xml2thot_f.h"
#include "init_f.h"
#ifdef GRAPHML
#include "GraphMLbuilder_f.h"
#endif /* GRAPHML */
#include "MathMLbuilder_f.h"
#include "styleparser_f.h"
#include "XHTMLbuilder_f.h"
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
static XML_Parser  parser = NULL;

/* global data used by the HTML parser */
static ParserData  XMLcontext = {0, 0, NULL, 0, FALSE, FALSE, FALSE, FALSE, FALSE};

/* a parser context. It describes the specific actions to be executed
when parsing an XML document fragment according to a given DTD */
typedef struct _XMLparserContext *PtrParserCtxt;
typedef struct _XMLparserContext
  {
    STRING	   UriName;		/* URI of namespaces for that DTD */
    PtrParserCtxt  NextParserCtxt;	/* next parser context */
    STRING	   SSchemaName;		/* name of Thot structure schema */
    SSchema	   XMLSSchema;		/* the Thot structure schema */
    int            XMLtype;             /* indentifier used by fetchname */
    Proc	   MapAttribute;	/* returns the Thot attribute corresp.
					   to an XML attribute name */
    Proc	   MapAttributeValue;	/* returns the Thot value corresp. to
					   the name of an XML attribute value*/
    Proc	   MapEntity;		/* returns the value of a XML entity */
    
    Proc	   EntityCreated;	/* action to be called when an entity
					   has been parsed */
    Proc	   InsertElem;	        /* action to be called to insert an
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
/* xhtml context */
static PtrParserCtxt	xhtmlParserCtxt = NULL;
/* XLink parser context */
static PtrParserCtxt    XLinkParserCtxt = NULL;

#define MAX_URI_NAME_LENGTH  60
#define XHTML_URI            TEXT("http://www.w3.org/1999/xhtml")
#define MathML_URI           TEXT("http://www.w3.org/1998/Math/MathML")
#define GraphML_URI          TEXT("http://www.w3.org/2000/svg")
#define XLink_URI            TEXT("http://www.w3.org/1999/xlink")
#define NAMESPACE_URI        TEXT("http://www.w3.org/XML/1998/namespace")


/* parser stack */
	                /* maximum stack height */
#define MAX_STACK_HEIGHT   200
                        /* XML element name */
static USTRING       nameElementStack[MAX_STACK_HEIGHT]; 
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
static gzFile      stream = 0;
                        /* path or URL of the document */
static CHAR_T*     docURL = NULL;

/* information about the Thot document under construction */
                        /* Document structure schema */
static SSchema      DocumentSSchema = NULL;
                        /* root element of the document */
static Element      rootElement;
                        /* type of the root element */
static ElementType  rootElType;
                        /* name of the root element */
static CHAR_T       XMLrootName[100];
                        /* root element is closed */
static ThotBool	    XMLrootClosed = FALSE;
                        /* the last start tag encountered is invalid */
static ThotBool     UnknownTag = FALSE;
                        /* last attribute created */
static Attribute    currentAttribute = NULL;
                        /* element with which the last */
                        /* attribute has been associated */
static Element      lastAttrElement = NULL;
                        /* entry in the AttributeMappingTable */
		        /* of the attribute being created */
static AttributeMapping* lastMappedAttr = NULL;

static ThotBool	    ParsingSubTree = FALSE;
static ThotBool	    ImmediatelyAfterTag = FALSE;
static ThotBool	    HTMLStyleAttribute = FALSE;
static ThotBool	    XMLSpaceAttribute = FALSE;
static CHAR_T	    currentElementContent = ' ';
static CHAR_T	    currentElementName[40];

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

#ifdef __STDC__
static void   StartOfXmlStartElement (CHAR_T *GIname);
static void   XmlInsertElement (Element *el);
static void   DisableExpatParser ();
#else
static void   StartOfXmlStartElement (GIname);
static void   XmlInsertElement (el);
static void   DisableExpatParser ();
#endif


/*----------------------------------------------------------------------
   ChangeXmlParserContextDTD
   Get the parser context correponding to a given DTD
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     ChangeXmlParserContextDTD (STRING DTDname)
#else
static void     ChangeXmlParserContextDTD (DTDname)
STRING              DTDname;
 
#endif
{
  currentParserCtxt = firstParserCtxt;
  while (currentParserCtxt != NULL &&
	 ustrcmp (DTDname, currentParserCtxt->SSchemaName))
    currentParserCtxt = currentParserCtxt->NextParserCtxt;

  /* initialize the corresponding entry */
  if (currentParserCtxt != NULL &&
      currentParserCtxt->XMLSSchema == NULL) 
    currentParserCtxt->XMLSSchema = 
      GetXMLSSchema (currentParserCtxt->XMLtype, XMLcontext.doc);
}

/*----------------------------------------------------------------------
   ChangeXmlParserContextUri
   Get the parser context correponding to a given uri
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     ChangeXmlParserContextUri (STRING uriName)
#else
static void     ChangeXmlParserContextUri (uriName)
STRING      uriName;

#endif
{
  currentParserCtxt = firstParserCtxt;
  while (currentParserCtxt != NULL &&
	 ustrcmp (uriName, currentParserCtxt->UriName))
    currentParserCtxt = currentParserCtxt->NextParserCtxt;

  /* Return the corresponding Thot schema */
  if (currentParserCtxt != NULL &&
      currentParserCtxt->XMLSSchema == NULL) 
    currentParserCtxt->XMLSSchema = 
      GetXMLSSchema (currentParserCtxt->XMLtype, XMLcontext.doc);
}

/*----------------------------------------------------------------------
   ChangeXmlParserContextRootName
   Get the parser context correponding to a given tag
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     ChangeXmlParserContextTagName (STRING tagName)
#else
static void     ChangeXmlParserContextTagName (tagName)
STRING      tagName;

#endif
{
  if (!ustrcmp (tagName, TEXT("math")))
    ChangeXmlParserContextDTD (TEXT("MathML"));
  else
    if (!ustrcmp (tagName, TEXT("svg")) ||
	!ustrcmp (tagName, TEXT("xmlgraphics")))
      ChangeXmlParserContextDTD (TEXT("GraphML"));
    else
      if (!ustrcmp (tagName, TEXT("html")))
	ChangeXmlParserContextDTD (TEXT("HTML"));
      else
	currentParserCtxt = NULL;
}

/*----------------------------------------------------------------------
   InitXmlParserContexts
   Create the chain of parser contexts decribing all recognized XML DTDs
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void            InitXmlParserContexts (void)
#else
static void            InitXmlParserContexts ()
#endif
{
   PtrParserCtxt	ctxt, prevCtxt;

   firstParserCtxt = NULL;
   prevCtxt = NULL;
   ctxt = NULL;

   /* create and initialize a context for the XHTML parser */
   ctxt = TtaGetMemory (sizeof (XMLparserContext));
   if (prevCtxt == NULL)
      firstParserCtxt = ctxt;
   else
      prevCtxt->NextParserCtxt = ctxt;
   ctxt->NextParserCtxt = NULL;
   ctxt->SSchemaName = TtaAllocString (MAX_SS_NAME_LENGTH);
   ustrcpy (ctxt->SSchemaName, TEXT("HTML"));
   ctxt->UriName = TtaAllocString (MAX_URI_NAME_LENGTH);
   ustrcpy (ctxt->UriName, XHTML_URI);
   ctxt->XMLSSchema = NULL;
   ctxt->XMLtype = XHTML_TYPE;
   ctxt->MapAttribute = (Proc) MapHTMLAttribute;
   ctxt->MapAttributeValue = (Proc) MapHTMLAttributeValue;
   ctxt->MapEntity = (Proc) XhtmlMapEntity;
   /* ctxt->EntityCreated = (Proc) XhtmlEntityCreated; */
   ctxt->EntityCreated = NULL;
   ctxt->InsertElem = (Proc) XhtmlInsertElement;
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
   xhtmlParserCtxt = ctxt;

   /* create and initialize a context for the MathML parser */
   ctxt = TtaGetMemory (sizeof (XMLparserContext));
   if (prevCtxt == NULL)
      firstParserCtxt = ctxt;
   else
      prevCtxt->NextParserCtxt = ctxt;
   ctxt->NextParserCtxt = NULL;
   ctxt->SSchemaName = TtaAllocString (MAX_SS_NAME_LENGTH);
   ustrcpy (ctxt->SSchemaName, TEXT("MathML"));
   ctxt->UriName = TtaAllocString (MAX_URI_NAME_LENGTH);
   ustrcpy (ctxt->UriName, MathML_URI);
   ctxt->XMLSSchema = NULL;
   ctxt->XMLtype = MATH_TYPE;
   ctxt->MapAttribute = (Proc) MapMathMLAttribute;
   ctxt->MapAttributeValue = (Proc) MapMathMLAttributeValue;
   ctxt->MapEntity = (Proc) MapMathMLEntity;
   /*   ctxt->EntityCreated = (Proc) MathMLEntityCreated; */
   ctxt->EntityCreated = (Proc) PutMathMLEntity;
   ctxt->InsertElem = (Proc) XmlInsertElement;
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

#ifdef GRAPHML
   /* create and initialize a context for the GraphML parser */
   ctxt = TtaGetMemory (sizeof (XMLparserContext));
   if (prevCtxt == NULL)
      firstParserCtxt = ctxt;
   else
      prevCtxt->NextParserCtxt = ctxt;
   ctxt->NextParserCtxt = NULL;
   ctxt->SSchemaName = TtaAllocString (MAX_SS_NAME_LENGTH);
   ustrcpy (ctxt->SSchemaName, TEXT("GraphML"));
   ctxt->UriName = TtaAllocString (MAX_URI_NAME_LENGTH);
   ustrcpy (ctxt->UriName, GraphML_URI);
   ctxt->XMLSSchema = NULL;
   ctxt->XMLtype = GRAPH_TYPE;
   ctxt->MapAttribute = (Proc) MapGraphMLAttribute;
   ctxt->MapAttributeValue = (Proc) MapGraphMLAttributeValue;
   ctxt->MapEntity = (Proc) MapGraphMLEntity;
   ctxt->EntityCreated = (Proc) GraphMLEntityCreated;
   ctxt->InsertElem = (Proc) XmlInsertElement;
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
#endif /* GRAPHML */

   /* create and initialize a context for the XLink parser */
   ctxt = TtaGetMemory (sizeof (XMLparserContext));
   if (prevCtxt == NULL)
      firstParserCtxt = ctxt;
   else
      prevCtxt->NextParserCtxt = ctxt;
   ctxt->NextParserCtxt = NULL;	/* last context */
   ctxt->SSchemaName = TtaAllocString (MAX_SS_NAME_LENGTH);
   ustrcpy (ctxt->SSchemaName, TEXT("XLink"));
   ctxt->UriName = TtaAllocString (MAX_URI_NAME_LENGTH);
   ustrcpy (ctxt->UriName, XLink_URI);
   ctxt->XMLSSchema = NULL;
   ctxt->XMLtype = XLINK_TYPE;
   ctxt->MapAttribute = (Proc) MapXLinkAttribute;
   ctxt->MapAttributeValue = (Proc) MapXLinkAttributeValue;
   ctxt->MapEntity = NULL;
   ctxt->EntityCreated = NULL;
   ctxt->InsertElem = NULL;
   ctxt->ElementComplete = NULL;
   ctxt->AttributeComplete = (Proc) XLinkAttributeComplete;
   ctxt->GetDTDName = NULL;
   XLinkParserCtxt = ctxt;
   ctxt->DefaultLineBreak = TRUE;
   ctxt->DefaultLeadingSpace = TRUE;   
   ctxt->DefaultTrailingSpace = TRUE;  
   ctxt->DefaultContiguousSpace = TRUE;
   ctxt->PreserveLineBreak = FALSE;    
   ctxt->PreserveLeadingSpace = FALSE;   
   ctxt->PreserveTrailingSpace = FALSE;  
   ctxt->PreserveContiguousSpace = FALSE;
   prevCtxt = ctxt;

   currentParserCtxt = NULL;

}

/*----------------------------------------------------------------------
   XmlSetElemLineNumber
   Assigns the current line number (number given by EXPAT parser.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void         XmlSetElemLineNumber (Element el)
#else
void         XmlSetElemLineNumber (el)
Element		el;
#endif
{
  int     lineNumber;

  if (ParsingSubTree)
    lineNumber = 0;
  else
    lineNumber = XML_GetCurrentLineNumber (parser) + htmlLineRead - extraLineRead;
  TtaSetElementLineNumber (el, lineNumber);
}

/*----------------------------------------------------------------------
   XmlParseError
   print the error message msg on stderr.
   When the line is 0 ask to expat the current line number
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void        XmlParseError (Document doc, CHAR_T* msg, int line)
#else
void        XmlParseError (doc, msg, line)
Document    doc;
CHAR_T*     msg;
int         line;
#endif
{
#ifdef _I18N_
   unsigned char   mbcsMsg [MAX_TXT_LEN * 2];
   unsigned char*  ptrMbcsMas = &mbcsMsg[0];

   TtaWCS2MBS (&msg, &ptrMbcsMas, UTF_8 /* ISO_8859_1 */);
#else  /* !_I18N_ */
   unsigned char*  mbcsMsg = msg;
#endif /* _I18N_ */

   HTMLErrorsFound = TRUE;
   if (!ErrFile)
     {
      usprintf (ErrFileName, TEXT("%s%c%d%cHTML.ERR"),
		TempFileDirectory, DIR_SEP, doc, DIR_SEP);
      if ((ErrFile = ufopen (ErrFileName, TEXT("w"))) == NULL)
         return;
     }

   if (line != 0)
     {
       fprintf (ErrFile, "   line %d, char %d: %s\n", line, 0, mbcsMsg);
       fclose (ErrFile);
       ErrFile = NULL;
     }
   else if (doc == XMLcontext.doc)
     {
      /* the error message is related to the document being parsed */
      if (docURL != NULL)
	{
	  fprintf (ErrFile, "*** Errors in %s\n", docURL);
	  TtaFreeMemory (docURL);
	  docURL = NULL;
	}
      /* print the line number and character number before the message */
      fprintf (ErrFile, "   line %d, char %d: %s\n",
	       XML_GetCurrentLineNumber (parser) + htmlLineRead -  extraLineRead,
	       XML_GetCurrentColumnNumber (parser),
	       mbcsMsg);
     }
   else
     /* print only the error message */
     fprintf (ErrFile, "%s\n", mbcsMsg);
}

/*----------------------------------------------------------------------
  IsParsingCSS 
  Returns the value of ParsingCSS boolean.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool  IsParsingCSS ()
#else
ThotBool  IsParsingCSS ()

#endif
{
   return XMLcontext.parsingCSS;
}

/*----------------------------------------------------------------------
  SetParsingCSS 
  Sets the value of ParsingCSS boolean.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void  SetParsingCSS (ThotBool value)
#else
void  SetParsingCSS (value)
ThotBool   value;

#endif
{
   XMLcontext.parsingCSS = value;
}

/*----------------------------------------------------------------------
  SetParsingTextArea
  Sets the value of ParsingTextArea boolean.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void  SetParsingTextArea (ThotBool value)
#else
void  SetParsingTextArea (value)
ThotBool   value;

#endif
{
   XMLcontext.parsingTextArea = value;
}

/*----------------------------------------------------------------------
  IsWithinTable 
  Returns the value of WithinTable integer.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int  IsWithinTable ()
#else
int  IsWithinTable ()

#endif
{
   return XMLcontext.withinTable;
}

/*----------------------------------------------------------------------
  SubWithinTable
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void  SubWithinTable ()
#else
void  SubWithinTable ()
#endif
{
   XMLcontext.withinTable--;
}

/*----------------------------------------------------------------------
   XmlWhiteSpaceHandling
   Is there an openend element with a xml:space attribute ?
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void    XmlWhiteSpaceHandling ()
#else
static void    XmlWhiteSpaceHandling ()

#endif
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
#ifdef __STDC__
static void    XmlWhiteSpaceInStack (CHAR_T  *attrValue)
#else
static void    XmlWhiteSpaceInStack (attrValue)
CHAR_T     *attrValue,

#endif
{
  if (attrValue == NULL)
      spacePreservedStack[stackLevel-1] = 'P';
  else
    {
      if ((ustrcmp (attrValue, TEXT("default")) == 0))
	spacePreservedStack[stackLevel-1] = 'D';
      else
	spacePreservedStack[stackLevel-1] = 'P';
    }
}

/*----------------------------------------------------------------------
  XmlWithinStack  
  Checks if an element of type ThotType is in the stack.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool  XmlWithinStack (int ThotType,
				 SSchema ThotSSchema)
#else
static ThotBool  XmlWithinStack (ThotType, ThotSSchema)
int       ThotType;
SSchema	  ThotSSchema;

#endif
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
   XhtmlCannotContainText 
   Return TRUE if element el is a block element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     XhtmlCannotContainText (ElementType elType)
#else
static ThotBool     XhtmlCannotContainText (elType)
ElementType         elType;

#endif
{
   int                 i;
   ThotBool            ret;

   if (ustrcmp (TtaGetSSchemaName (elType.ElSSchema), TEXT("HTML")))
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
   CheckSurrounding
   Inserts an element Pseudo_paragraph in the abstract tree of
   the Thot document if el is a leaf and is not allowed to be
   a child of element parent.
   If element *el is not a character level element and parent is
   a Pseudo_paragraph, insert *el as a sibling of element parent.

   Return TRUE if element *el has been inserted in the tree.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     CheckSurrounding (Element *el,
				      Element  parent, 
				      Document doc)
#else
static ThotBool     CheckSurrounding (el, parent, doc)
Element     *el;
Element      parent;
Document     doc;

#endif
{
   ElementType         parentType, newElType, elType;
   Element             newEl, ancestor, prev, prevprev;
   ThotBool	       ret;

   if (parent == NULL)
       return(FALSE);

   ret = FALSE;
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
	while (ancestor != NULL &&
	       IsXMLElementInline (ancestor))
	       ancestor = TtaGetParent (ancestor);

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
		    ret = TRUE;
		    
		    /* if previous siblings of the new Pseudo_paragraph element
		       are character level elements, move them within the new
		       Pseudo_paragraph element */
		    prev = newEl;
		    TtaPreviousSibling (&prev);
		    while (prev != NULL)
		      {
			if (!IsXMLElementInline (prev))
			  prev = NULL;
			else
			  {
			    prevprev = prev;  TtaPreviousSibling (&prevprev);
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
     if (!IsXMLElementInline (*el))
       /* it is not a character level element */
       /* don't insert it as a child of a Pseudo_paragraph, but as a sibling */
       {
	 parentType = TtaGetElementType (parent);
	 if (parentType.ElTypeNum == HTML_EL_Pseudo_paragraph)
	   {
	     TtaInsertSibling (*el, parent, FALSE, doc);
	     ret = TRUE;
	   }
       }

   if (!ret)
     if (elType.ElTypeNum == HTML_EL_TEXT_UNIT ||
         (elType.ElTypeNum != HTML_EL_Inserted_Text &&
	  IsXMLElementInline (*el)))
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
		 ret = TRUE;
	       }
	   }
       }

   return ret;
}

/*----------------------------------------------------------------------
   XhtmlInsertElement 
   Insert an XHTML element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void     XhtmlInsertElement (Element *el)
#else
void     XhtmlInsertElement (el)
Element *el;

#endif
{
   Element   parent;

   if (InsertSibling ())
     {
       if (XMLcontext.lastElement == NULL)
	   parent = NULL;
       else
	   parent = TtaGetParent (XMLcontext.lastElement);

       if (!CheckSurrounding (el, parent, XMLcontext.doc))
	 {
	   if (parent != NULL)
	       TtaInsertSibling (*el, XMLcontext.lastElement, FALSE, XMLcontext.doc);
	   else
	     {
	       TtaDeleteTree (*el, XMLcontext.doc);
	       *el = NULL;
	     }
	 }
     }
   else
     {
       if (!CheckSurrounding (el, XMLcontext.lastElement, XMLcontext.doc))
	   TtaInsertFirstChild (el, XMLcontext.lastElement, XMLcontext.doc);
     }
}

/*----------------------------------------------------------------------
   XmlInsertElement 
   Insert a XML element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     XmlInsertElement (Element *el)
#else
static void     XmlInsertElement (el)
Element    *el;

#endif
{
  if (InsertSibling ())
       TtaInsertSibling (*el, XMLcontext.lastElement, FALSE, XMLcontext.doc);
   else
       TtaInsertFirstChild (el, XMLcontext.lastElement, XMLcontext.doc);
}

/*---------------------------------------------------------------------------
   InsertXmlElement   
   Inserts an element el in the Thot abstract tree , at the current position.
  ---------------------------------------------------------------------------*/
#ifdef __STDC__
void     InsertXmlElement (Element *el)
#else
void     InsertXmlElement (el)
Element  *el;

#endif
{
  if (currentParserCtxt != NULL)
    {
      (*(currentParserCtxt->InsertElem)) (el);
      if (*el != NULL)
	{
	  XMLcontext.lastElement = *el;
	  XMLcontext.lastElementClosed = FALSE;
	}
    }
}

/*----------------------------------------------------------------------
   GetXmlElType
   Search in the mapping tables the entry for the element type of
   name Xmlname and returns the corresponding Thot element type.
   Schema = NULL if not found.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void       GetXmlElType (STRING       XMLname,
				ElementType *elType,
				STRING      *mappedName,
				CHAR_T      *content,
				Document     doc)
#else
static void       GetXmlElType (XMLname, elType, mappedName, content, doc)

STRING         XMLname;
ElementType   *elType;
STRING        *mappedName;
CHAR_T        *content;
Document       doc;
#endif
{
 /* initialize all parser contexts if not done yet */
  if (firstParserCtxt == NULL)
    InitXmlParserContexts ();

  /* Look at the current context if there is one */
  if (currentParserCtxt != NULL)
    {
      elType->ElSSchema = currentParserCtxt->XMLSSchema;
      MapXMLElementType (currentParserCtxt->XMLtype, XMLname,
			 elType, mappedName, content, doc);
    }
  else
    {
      /* not found */
      elType->ElTypeNum = 0;
      elType->ElSSchema = NULL;
    }
}

/*----------------------------------------------------------------------
   XmlLastLeafInElement
   return the last leaf element in element el.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
Element      XmlLastLeafInElement (Element el)
#else
Element      XmlLastLeafInElement (el)
Element             el;

#endif
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

/*----------------------------------------------------------------------
   RemoveEndingSpaces
   If element el is a block-level element, remove all spaces contained
   at the end of that element.
   Return TRUE if spaces have been removed.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     RemoveEndingSpaces (Element el)
#else
static ThotBool     RemoveEndingSpaces (el)
Element el;

#endif
{
   int           length, nbspaces;
   ElementType   elType;
   Element       lastLeaf;
   CHAR_T        lastChar[2];
   ThotBool      endingSpacesDeleted;

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
	   endingSpacesDeleted = TRUE;
      }
   return endingSpacesDeleted;
}

/*----------------------------------------------------------------------
   RemoveTrailingSpaces
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     RemoveTrailingSpaces (Element el)
#else
static void     RemoveTrailingSpaces (el)
Element el;

#endif
{
   int           length, nbspaces;
   ElementType   elType;
   Element       lastLeaf;
   CHAR_T        lastChar[2];

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
	       if (RemoveTrailingSpace)
		 {
		   do
		     {
		       TtaGiveSubString (lastLeaf, lastChar, length, 1);
		       if (lastChar[0] == WC_SPACE)
			 {
			   length--;
			   nbspaces++;
			 }
		     }
		   while (lastChar[0] == WC_SPACE && length > 0);
		 }
	       else
		 {
		   TtaGiveSubString (lastLeaf, lastChar, length, 1);
		   if (lastChar[0] == WC_CR || lastChar[0] == WC_EOL)
		     {
		       length--;
		       nbspaces++;
		     }
		 }

	       if (nbspaces > 0)
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

/*----------------------------------------------------------------------
   XmlCloseElement
   Terminate the corresponding Thot element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool   XmlCloseElement (USTRING mappedName)
#else
static ThotBool   XmlCloseElement (mappedName)
USTRING          mappedName;
#endif
{
   int                 i, error;
   Element             el, parent;
   ElementType         parentType;
   ThotBool            ret, spacesDeleted;

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
	       (*(currentParserCtxt->ElementComplete)) (el, XMLcontext.doc,
							&error);
	       /* If the element closed is a block-element, remove */
	       /* spaces contained at the end of that element */
	       /*
	       if (!spacesDeleted)
	          spacesDeleted = RemoveEndingSpaces (el);
	       */

	       /* Remove the trailing spaces of that element */
	       RemoveTrailingSpaces (el);

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
   XhtmlContextOK 
   Returns TRUE if the element at position entry in the mapping table
   is allowed to occur in the current structural context.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     XhtmlContextOK (STRING elName)
#else
static ThotBool     XhtmlContextOK (elName)
STRING    elName;

#endif
{
   ThotBool      ok;

   if (stackLevel <= 1 || nameElementStack[stackLevel - 1] == NULL)
     return TRUE;
   else
     {
       ok = TRUE;
       /* only TH and TD elements are allowed as children of a TR element */
       if (!ustrcmp (nameElementStack[stackLevel - 1], TEXT("tr")))
	 if (ustrcmp (elName, TEXT("th")) &&
	     ustrcmp (elName, TEXT("td")))
	   ok = FALSE;

       if (ok)
	 /* only CAPTION, THEAD, TFOOT, TBODY, COLGROUP, COL and TR are */
	 /* allowed as children of a TABLE element */
	 if (!ustrcmp (nameElementStack[stackLevel - 1], TEXT("table")))
	   {
	     if (ustrcmp (elName, TEXT("caption"))  &&
		 ustrcmp (elName, TEXT("thead"))    &&
		 ustrcmp (elName, TEXT("tfoot"))    &&
		 ustrcmp (elName, TEXT("tbody"))    &&
		 ustrcmp (elName, TEXT("colgroup")) &&
		 ustrcmp (elName, TEXT("col"))      &&
		 ustrcmp (elName, TEXT("tr")))
	         if (!ustrcmp (elName, TEXT("td")) ||
		     !ustrcmp (elName, TEXT("th")))
		   /* Table cell within a table, without a tr. Assume tr */
		   {
		     /* simulate a <TR> tag */
		     StartOfXmlStartElement (TEXT("tr"));
		   }
		 else
		   ok = FALSE;
	   }

       if (ok)
	 /* CAPTION, THEAD, TFOOT, TBODY, COLGROUP are allowed only as
	    children of a TABLE element */
	 if (ustrcmp (elName, TEXT("caption"))  == 0 ||
	     ustrcmp (elName, TEXT("thead"))    == 0 ||
	     ustrcmp (elName, TEXT("tfoot"))    == 0 ||
	     ustrcmp (elName, TEXT("tbody"))    == 0 ||
	     ustrcmp (elName, TEXT("colgroup")) == 0)
	     if (ustrcmp (nameElementStack[stackLevel - 1], TEXT("table")) != 0)
	         ok = FALSE;

       if (ok)
	 /* only TR is allowed as a child of a THEAD, TFOOT or TBODY element */
	 if (!ustrcmp (nameElementStack[stackLevel - 1], TEXT("thead")) ||
	     !ustrcmp (nameElementStack[stackLevel - 1], TEXT("tfoot")) ||
	     !ustrcmp (nameElementStack[stackLevel - 1], TEXT("tbody")))
	   {
	     if (ustrcmp (elName, TEXT("tr")))
	         if (!ustrcmp (elName, TEXT("td")) ||
		     !ustrcmp (elName, TEXT("th")))
		   /* Table cell within a thead, tfoot or tbody without a tr. */
		   /* Assume tr */
		   {
		     /* simulate a <tr> tag */
		     StartOfXmlStartElement (TEXT("tr"));
		   }
		 else
		   ok = FALSE;
	   }

       if (ok)
	 /* refuse BODY within BODY */
	 if (ustrcmp (elName, TEXT("body")) == 0)
	     if (XmlWithinStack (HTML_EL_BODY, DocumentSSchema))
	         ok = FALSE;

       if (ok)
	 /* refuse HEAD within HEAD */
	 if (ustrcmp (elName, TEXT("head")) == 0)
	   if (XmlWithinStack (HTML_EL_HEAD, DocumentSSchema))
	     ok = FALSE;

       if (ok)
	 /* refuse STYLE within STYLE */
	 if (ustrcmp (elName, TEXT("style")) == 0)
	   if (XmlWithinStack (HTML_EL_STYLE_, DocumentSSchema))
	     ok = FALSE;

       return ok;
     }
}

/*----------------------------------------------------------------------
   StartOfXmlStartElement  
   The name of an element type has been read from a start tag.
   Create the corresponding Thot element according to the mapping table.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void   StartOfXmlStartElement (CHAR_T* GIname)
#else
static void   StartOfXmlStartElement (GIname)
CHAR_T*             GIname;

#endif
{
  ElementType         elType;
  Element             newElement;
  CHAR_T              msgBuffer[MaxMsgLength];
  STRING              mappedName = NULL;
  ThotBool            elInStack = FALSE;

  UnknownTag = FALSE;

  /* ignore tag <P> within PRE for Xhtml elements */
  if (currentParserCtxt != NULL &&
      (ustrcmp (currentParserCtxt->SSchemaName, TEXT("HTML")) == 0) &&
      (XmlWithinStack (HTML_EL_Preformatted, currentParserCtxt->XMLSSchema)) &&
      (ustrcasecmp (GIname, TEXT("p")) == 0))
    {
       UnknownTag = TRUE;
       return;
    }

  if (stackLevel == MAX_STACK_HEIGHT)
    {
      XmlParseError (XMLcontext.doc, TEXT("**FATAL** Too many XML levels"),0);
      XMLabort = TRUE;
      UnknownTag = TRUE;
      return;
    }

  /* search the XML element name in the corresponding mapping table */
  elType.ElSSchema = NULL;
  elType.ElTypeNum = 0;
  currentElementName[0] = WC_EOS;
  GetXmlElType (GIname, &elType, &mappedName,
		&currentElementContent, XMLcontext.doc);

  if (mappedName == NULL)
    /* element not found in the corresponding DTD */
    {
      if (ParsingLevel[XMLcontext.doc] != L_Transitional)
	{
	  /***  What to do in this case ? */
	}
      /* doesn't process that element */
      usprintf (msgBuffer, TEXT("Invalid XML element %s"), GIname);
      XmlParseError (XMLcontext.doc, msgBuffer, 0);
      UnknownTag = TRUE;
      return;
    }
  else
    ustrcpy (currentElementName, mappedName);
  
  /* element found in the corresponding DTD */
  if (currentParserCtxt != NULL &&
      (ustrcmp (currentParserCtxt->SSchemaName, TEXT("HTML")) == 0) &&
      (!XhtmlContextOK (mappedName)))
    /* Xhtml element not allowed in the current structural context */
    {
      usprintf (msgBuffer,
		TEXT("The XML element %s is not allowed here"), GIname);
      XmlParseError (XMLcontext.doc, msgBuffer, 0);
      UnknownTag = TRUE;
      elInStack = FALSE;
    }
  else
    {
      newElement = NULL;
      if (rootElement != NULL &&
	  elType.ElTypeNum == rootElType.ElTypeNum &&
	  elType.ElSSchema == rootElType.ElSSchema)
	/* the corresponding Thot element is the root of the */
	/* abstract tree, which has been created at initialization */
	newElement = rootElement;
      else
	{
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
	  if (newElement != NULL)
	    {
	      /* an empty Text element has been created. */
	      /* The following character data must go to that elem. */
	      if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
		XMLcontext.mergeText = TRUE;
	    }
	}   
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
#ifdef __STDC__
static void     EndOfXmlStartElement (CHAR_T *name)
#else
static void     EndOfXmlStartElement (name)
CHAR_T   *name;

#endif
{

  ElementType     elType;
  AttributeType   attrType;
  Attribute       attr;
  int             length;
  STRING          text;
  STRING          elSchemaName;

  if (UnknownTag)
    return;

  if (XMLcontext.lastElement != NULL && currentElementName[0] != WC_EOS)
    {
      elType = TtaGetElementType (XMLcontext.lastElement);
      elSchemaName = TtaGetSSchemaName (elType.ElSSchema);
      
      if (ustrcmp (TEXT("HTML"), elSchemaName) == 0)
	{
	  if (!ustrcmp (nameElementStack[stackLevel - 1], TEXT("pre"))   ||
	      !ustrcmp (nameElementStack[stackLevel - 1], TEXT("style")) ||
	      !ustrcmp (nameElementStack[stackLevel - 1], TEXT("script")))
	    /* a <PRE>, <STYLE> or <SCRIPT> tag has been read */
	    XmlWhiteSpaceInStack (NULL);
	  else
	    if (!ustrcmp (nameElementStack[stackLevel - 1], TEXT("table")))
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
		  text = TtaAllocString (length + 1);
		  TtaGiveTextAttributeValue (attr, text, &length);
		  if (!ustrcasecmp (text, TEXT("text/css")))
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
   XmlEndElement
   Terminate all corresponding Thot elements.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void    XmlEndElement (CHAR_T *GIname)
#else
static void    XmlEndElement (GIname)
CHAR_T     *GIname;

#endif
{
   CHAR_T         msgBuffer[MaxMsgLength];
   ElementType    elType;
   STRING         mappedName = NULL;

   if (XMLcontext.parsingTextArea)
     if (ustrcasecmp (GIname, TEXT("textarea")) != 0)
       /* We are parsing the contents of a textarea element. */
       /* The end tag is not the one closing the current textarea, */
       /* consider it as plain text */
       return;
   
   /* Ignore the virtual root of a XML sub-tree */
   if (ustrcmp (GIname, SUBTREE_ROOT) == 0)
     return;

   /* search the XML element name in the corresponding mapping table */
   elType.ElSSchema = NULL;
   elType.ElTypeNum = 0;
   currentElementName[0] = WC_EOS;
   GetXmlElType (GIname, &elType, &mappedName,
		 &currentElementContent, XMLcontext.doc);
   
   if (mappedName == NULL)
     /* element not found in the corresponding DTD */
     {
       if (ParsingLevel[XMLcontext.doc] != L_Transitional)
	 {
	   /***  What to do in this case ? */
	 }
       /* doesn't process that element */
       usprintf (msgBuffer, TEXT("Invalid XML element %s"), GIname);
       XmlParseError (XMLcontext.doc, msgBuffer, 0);
     }
   else
     {
       /* element found in the corresponding DTD */
       if (!XmlCloseElement (mappedName))
	 /* the end tag does not close any current element */
	 {
	   usprintf (msgBuffer, TEXT("Unexpected end tag %s"), GIname);
	   XmlParseError (XMLcontext.doc, msgBuffer, 0);
	 }
     }
}
/*---------------------  EndElement  (end)  --------------------------*/


/*----------------------  Data  (start)  -----------------------------*/

/*----------------------------------------------------------------------
   IsLeadingSpaceUseless
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool  IsLeadingSpaceUseless ()
#else  /* __STDC__ */
static ThotBool  IsLeadingSpaceUseless ()
#endif  /* __STDC__ */

{
   ElementType     elType;
   STRING          elSchemaName;
   Element         parent, ancestor, prev;
   ThotBool        removeLeadingSpaces;

   if (InsertSibling ())
     /* There is a previous sibling (XMLcontext.lastElement) 
	for the new Text element */
     {
       parent = TtaGetParent (XMLcontext.lastElement);
       if (parent == NULL)
	 parent = XMLcontext.lastElement;
       elType = TtaGetElementType (parent);
       elSchemaName = TtaGetSSchemaName (elType.ElSSchema);
       if (IsXMLElementInline (XMLcontext.lastElement) &&
	   (ustrcmp (TEXT("HTML"), elSchemaName) == 0) &&
	   (elType.ElTypeNum != HTML_EL_Option_Menu) &&
	   (elType.ElTypeNum != HTML_EL_OptGroup))
	 {
	   removeLeadingSpaces = FALSE;
	   elType = TtaGetElementType (XMLcontext.lastElement);
	   if ((elType.ElTypeNum == HTML_EL_BR) && 
	       (ustrcmp (TEXT("HTML"), elSchemaName) == 0))
	     removeLeadingSpaces = TRUE;
	 }
       else
	 removeLeadingSpaces = TRUE;
     }
   else
     /* the new Text element should be the first child 
	of the latest element encountered */
     {
       parent = XMLcontext.lastElement;
       removeLeadingSpaces = TRUE;
       elType = TtaGetElementType (XMLcontext.lastElement);
       elSchemaName = TtaGetSSchemaName (elType.ElSSchema);
       if ((ustrcmp (TEXT("HTML"), elSchemaName) != 0) ||
	   ((ustrcmp (TEXT("HTML"), elSchemaName) == 0) &&
	   elType.ElTypeNum != HTML_EL_Option_Menu &&
	   elType.ElTypeNum != HTML_EL_OptGroup))
	 {
	   ancestor = parent;
	   while (removeLeadingSpaces &&
		  IsXMLElementInline (ancestor))
	     {
	       prev = ancestor;
	       TtaPreviousSibling (&prev);
	       if (prev == NULL)
		 ancestor = TtaGetParent (ancestor);
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
#ifdef __STDC__
void     PutInXmlElement (STRING data)
#else  /* __STDC__ */
void     PutInXmlElement (data)
STRING      data;
#endif  /* __STDC__ */

{
   ElementType  elType;
   Element      elText;
   int          i=0;
   int          length;
   Language     lang;
   ThotBool     uselessSpace = FALSE;
   CHAR_T      *buffer, *bufferws, *buffertext;
   int          i1, i2=0, i3=0;

#ifdef LC
   printf ("\n  PutInXmlElement - length : %d, data \"%s\"",
	   ustrlen (data), data);
#endif /* LC */

   i = 0;
   /* Immediately after a start tag, treatment of the leading spaces */
   /* If RemoveLeadingSpace = TRUE, we suppress all leading white-space */
   /* characters, otherwise, we only suppress the first line break*/
   if (ImmediatelyAfterTag)
     {
       if (RemoveLeadingSpace)
	 {
	   while (data[i] != WC_EOS &&
		  (data[i] == WC_EOL || data[i] == WC_CR ||
		   data[i] == WC_TAB || data[i] == WC_SPACE))
	     i++;
	   if (data[i] != WC_EOS)
	     ImmediatelyAfterTag = FALSE;
	 }
       else
	 {
	   if (data[0] == WC_EOL || data[0] == WC_CR)
	     i = 1;
	   ImmediatelyAfterTag = FALSE;
	 }
     }
 
   if (data[i] == WC_EOS)
     return;

   length = ustrlen (&(data[i]));
   bufferws = TtaAllocString (length+1);
   strcpy (bufferws, &(data[i]));
   
   /* Convert line-break or tabs into space character */
   i = 0;
   if (RemoveLineBreak)
     {
       while (bufferws[i] != WC_EOS)
	 {
	   if (bufferws[i] == WC_EOL || bufferws[i] == WC_CR ||
	       bufferws[i] == WC_TAB)
	     bufferws[i]= WC_SPACE;
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
	   while (bufferws[i] != WC_EOS && bufferws[i] == WC_SPACE)
	     i++;
       
       /* Collapse contiguous spaces */ 
       if (bufferws[i] != WC_EOS)
	 {
	   length = ustrlen (bufferws);
	   buffer = TtaAllocString (length+1);
	   if (RemoveContiguousSpace)
	     {
	       for (i1 = i; i1 <= length; i1++)
		 {
		   if (bufferws[i1] <= WC_SPACE && bufferws[i1] != WC_EOS)
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
	       if ((buffer[i1] == WC_SPACE) && RemoveContiguousSpace)
		 {
		   /* Is the last character of text element a space */
		   length = TtaGetTextLength (XMLcontext.lastElement) + 1;
		   buffertext = TtaAllocString (length);
		   TtaGiveTextContent (XMLcontext.lastElement,
				       buffertext, &length, &lang);
		   /* Remove leading space if last content was finished by a space */
		   if ((buffertext[length-1] == WC_SPACE))
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
	     }
	   TtaFreeMemory (buffer);
	 }
       TtaFreeMemory (bufferws);
     }
}
/*----------------------  Data  (end)  ---------------------------*/


/*--------------------  Attributes  (start)  ---------------------*/

/*----------------------------------------------------------------------
   XhtmlCreateAttr 
   Create an attribute of type attrType for the element el.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         XhtmlCreateAttr (Element       el,
				     AttributeType attrType,
				     CHAR_T*       text,
				     ThotBool      invalid,
				     Document      doc)
#else
static void         XhtmlCreateAttr (el, attrType, text, invalid, doc)
Element        el;
AttributeType  attrType;
CHAR_T*        text;
ThotBool       invalid;
Document       doc;

#endif
{
  int          attrKind;
  int          length;
  CHAR_T*      buffer;
  Attribute    attr, oldAttr;

  if (attrType.AttrTypeNum != 0)
    {
      oldAttr = TtaGetAttribute (el, attrType);
      if (oldAttr != NULL)
	/* this attribute already exists */
	attr = oldAttr;
      else
	{
	  /* create a new attribute and attach it to the element */
	  attr = TtaNewAttribute (attrType);
	  TtaAttachAttribute (el, attr, doc);
	}

      currentAttribute = attr;
      lastAttrElement = el;
      TtaGiveAttributeType (attr, &attrType, &attrKind);

      if (attrKind == 0)	/* enumerate */
	TtaSetAttributeValue (attr, 1, el, doc);

      if (attrType.AttrTypeNum == HTML_ATTR_Border)
	/* attribute BORDER without any value (ThotBool attribute) is */
	/* considered as BORDER=1 */
	TtaSetAttributeValue (attr, 1, el, doc);

      if (invalid)
	{
	  /* Copy the name of the invalid attribute as the content */
	  /* of the Invalid_attribute attribute. */
	  length = ustrlen (text) + 2;
	  length += TtaGetTextAttributeLength (attr);
	  buffer = TtaAllocString (length + 1);
	  TtaGiveTextAttributeValue (attr, buffer, &length);
	  ustrcat (buffer, TEXT(" "));
	  ustrcat (buffer, text);
	  TtaSetAttributeText (attr, buffer, el, doc);
	  TtaFreeMemory (buffer);
	}
    }
}

/*----------------------------------------------------------------------
   XhtmlEndOfAttrName   
   End of a XML attribute that belongs to the HTML DTD
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         XhtmlEndOfAttrName (CHAR_T  *attrName,
					Element  el,
					Document doc)
#else
static void         XhtmlEndOfAttrName (attrName, el, doc)
CHAR_T   *attrName;
Element   el;
Document  doc;

#endif
{
   AttributeMapping*   mapAttr;
   AttributeType       attrType;
   ElementType         elType;
   Element             child;
   Attribute           attr;
   CHAR_T              translation;
   ThotBool            invalidAttr;
   CHAR_T              msgBuffer[MaxMsgLength];


   invalidAttr = FALSE;
   attrType.AttrTypeNum = 0;

   mapAttr = MapHTMLAttribute (attrName, &attrType, currentElementName, doc);

   if (attrType.AttrTypeNum <= 0)
     {
       /* this attribute is not in the mapping table */
       if (ustrcasecmp (attrName, TEXT("xml:lang")) == 0)
	 /* attribute xml:lang is not considered as invalid, but it is
	    ignored */
	 lastMappedAttr = NULL;
       else
	 {
	   usprintf (msgBuffer, TEXT("Unknown attribute %s"), attrName);
	   XmlParseError (doc, msgBuffer, 0);
	   /* attach an Invalid_attribute to the current element */
	   mapAttr = MapHTMLAttribute (TEXT("unknown_attr"), &attrType,
				       currentElementName, doc);
	   invalidAttr = TRUE;
	 }
     }

   if (attrType.AttrTypeNum > 0 && el != NULL &&
       (!XMLcontext.lastElementClosed ||
	(XMLcontext.lastElement != rootElement)))
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
	   XhtmlCreateAttr (el, attrType, attrName, invalidAttr, doc);
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
		   TtaSetAttributeText (attr, TEXT("link"), el, doc);
		 }
	     }
	   else
	     if (attrType.AttrTypeNum == HTML_ATTR_Checked)
	       {
		 /* create Default-Checked attribute */
		 child = TtaGetFirstChild (el);
		 if (child != NULL)
		   {
		     attrType.AttrTypeNum = HTML_ATTR_DefaultChecked;
		     attr = TtaNewAttribute (attrType);
		     TtaAttachAttribute (child, attr, doc);
		     TtaSetAttributeValue (attr, HTML_ATTR_DefaultChecked_VAL_Yes_,
					   child, doc);
		   }
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
#ifdef __STDC__
static void     XmlEndOfAttrName (CHAR_T  *attrName,
				  Element  el,
				  Document doc)
#else
static void     XmlEndOfAttrName (attrName, el, doc)
CHAR_T     *attrName;
Element     el;
Document    doc;

#endif
{
   AttributeType       attrType;
   Attribute           attr;
   CHAR_T              msgBuffer[MaxMsgLength];

   attrType.AttrTypeNum = 0;

   if (currentParserCtxt->MapAttribute)
       (*(currentParserCtxt->MapAttribute)) (attrName, &attrType,
					     currentElementName, doc);
       
   if (attrType.AttrTypeNum <= 0)
     /* Not found. Is it a HTML attribute (style, class, id for instance) */
     MapHTMLAttribute (attrName, &attrType, currentElementName, doc);

   if (attrType.AttrTypeNum <= 0)
      /* this attribute is not in a mapping table */
     {
       usprintf (msgBuffer, TEXT("Unknown XML attribute %s"), attrName);
       XmlParseError (doc, msgBuffer, 0);
     }
   else
     {
       if (ustrcasecmp (attrName, TEXT("style")) == 0)
	   HTMLStyleAttribute = TRUE;
       attr = TtaGetAttribute (el, attrType);
       if (!attr)
	 {
	   attr = TtaNewAttribute (attrType);
	   TtaAttachAttribute (el, attr, doc);
	 }
       currentAttribute = attr;
     }
}

/*----------------------------------------------------------------------
   EndOfAttributeName   
   A XML attribute has been read. 
   Create the corresponding Thot attribute.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void      EndOfAttributeName (CHAR_T *attrName)
#else
static void      EndOfAttributeName (attrName)
CHAR_T         *attrName;

#endif
{
   CHAR_T         *buffer;
   CHAR_T         *bufName;
   CHAR_T         *ptr;
   CHAR_T          msgBuffer[MaxMsgLength];

   currentAttribute = NULL;
   lastMappedAttr = NULL;
   HTMLStyleAttribute = FALSE;
   XMLSpaceAttribute = FALSE;
 
   if (UnknownTag)
     return;

   /* look for a NS_SEP in the tag name (namespaces) */ 
   /* and ignore the prefix if there is one */
   buffer = TtaGetMemory (strlen (attrName) + 1);
   ustrcpy (buffer, (CHAR_T*) attrName);
   if ((ptr = ustrrchr (buffer, NS_SEP)) != NULL)
     {
       *ptr = WC_EOS;
       ptr++;

       /* Specific treatment to get round a bug in EXPAT parser */
       /* This one replaces first "xml:" prefix by the namespaces URI */
       if (ustrcmp (buffer, NAMESPACE_URI) == 0)
	 {
	   bufName = TtaGetMemory (strlen (ptr) + 5);
	   ustrcpy (bufName, TEXT("xml:"));
	   ustrcat (bufName, ptr);
	 }
       else
	 {
	   bufName = TtaGetMemory (strlen (ptr) + 1);
	   ustrcpy (bufName, ptr);
	   if (currentParserCtxt != NULL &&
	       ustrcmp (buffer, currentParserCtxt->UriName))
	     ChangeXmlParserContextUri (buffer);
	 }
     }
   else
     {
       bufName = TtaGetMemory (strlen (buffer) + 1);
       ustrcpy (bufName, buffer);
     }
   
   if (currentParserCtxt == NULL)
     {
       usprintf (msgBuffer,
		 TEXT("Unknown Namepaces for attribute :\"%s\""), attrName);
       XmlParseError (XMLcontext.doc, msgBuffer, 0);
     }
   else
     {
       /* Is it a xml:space attribute */
       if (ustrncmp (bufName, TEXT("xml:space"), 9) == 0)
	 XMLSpaceAttribute = TRUE;
       else
	 {
	   /* the attribute xml:lang is replaced by the attribute "lang" */
	   if (ustrncmp (bufName, TEXT("xml:lang"), 8) == 0)
	     ustrcpy (bufName, TEXT("lang"));
	   if (ustrcmp (currentParserCtxt->SSchemaName, TEXT("HTML")) == 0)
	     XhtmlEndOfAttrName (bufName, XMLcontext.lastElement, XMLcontext.doc);
	   else
	     XmlEndOfAttrName (bufName, XMLcontext.lastElement, XMLcontext.doc);
	 }
     }
   
   TtaFreeMemory (buffer);
   TtaFreeMemory (bufName);
   return;
}

/*----------------------------------------------------------------------
   XhtmlPutInContent    
   Put the string ChrString in the leaf of current element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static Element      XhtmlPutInContent (STRING ChrString)
#else
static Element      XhtmlPutInContent (ChrString)
STRING              ChrString;

#endif
{
   Element             el, child;
   ElementType         elType;
   int                 length;

   el = NULL;
   if (XMLcontext.lastElement != NULL)
     {
	/* search first leaf of current element */
	el = XMLcontext.lastElement;
	do
	  {
	     child = TtaGetFirstChild (el);
	     if (child != NULL)
		el = child;
	  }
	while (child != NULL);
	elType = TtaGetElementType (el);
	length = 0;
	if (elType.ElTypeNum == 1)
	   length = TtaGetTextLength (el);
	if (length == 0)
	   TtaSetTextContent (el, ChrString, XMLcontext.language, XMLcontext.doc);
	else
	   TtaAppendTextContent (el, ChrString, XMLcontext.doc);
     }
   return el;
}

/*----------------------------------------------------------------------
   XhtmlTypeAttrValue 
   Value val has been read for the HTML attribute TYPE.
   Create a child for the current Thot element INPUT accordingly.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         XhtmlTypeAttrValue (CHAR_T* val)
#else
static void         XhtmlTypeAttrValue (val)
CHAR_T*             val;

#endif
{
  ElementType         elType;
  Element             newChild;
  AttributeType       attrType;
  Attribute           attr;
  CHAR_T              msgBuffer[MaxMsgLength];
  int                 value;

  attrType.AttrSSchema = currentParserCtxt->XMLSSchema;
  attrType.AttrTypeNum = DummyAttribute;
  MapHTMLAttributeValue (val, attrType, &value);
  if (value < 0)
    {
      usprintf (msgBuffer, TEXT("Unknown attribute value \"type=%s\""), val);
      XmlParseError (XMLcontext.doc, msgBuffer, 0);
      usprintf (msgBuffer, TEXT("type=%s"), val);
      MapHTMLAttribute (TEXT("unknown_attr"), &attrType,
			 NULL, XMLcontext.doc);
      XhtmlCreateAttr (XMLcontext.lastElement, attrType,
		       msgBuffer, TRUE, XMLcontext.doc);
    }
  else
    {
      elType = TtaGetElementType (XMLcontext.lastElement);
      if (elType.ElTypeNum != HTML_EL_Input)
	{
        if (ustrlen (val) > MaxMsgLength - 40)
	   val[MaxMsgLength - 40] = WC_EOS;
	usprintf (msgBuffer, TEXT("Duplicate attribute \"type = %s\""), val);
	}
      else
	{
	  elType.ElSSchema = currentParserCtxt->XMLSSchema;
	  elType.ElTypeNum = value;
	  newChild = TtaNewTree (XMLcontext.doc, elType, "");
	  XmlSetElemLineNumber (newChild);
	  TtaInsertFirstChild (&newChild, XMLcontext.lastElement, XMLcontext.doc);
	  if (value == HTML_EL_PICTURE_UNIT)
	    {
	      /* add the attribute IsInput to input pictures */
	      attrType.AttrSSchema = elType.ElSSchema;
	      attrType.AttrTypeNum = HTML_ATTR_IsInput;
	      attr = TtaNewAttribute (attrType);
	      TtaAttachAttribute (newChild, attr, XMLcontext.doc);
	    }
	}
    }
}

/*----------------------------------------------------------------------
   XmlEndOfAttrValue
   An attribute value has been read for a element that
   doesn't belongs to XHTML DTD
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         XmlEndOfAttrValue (CHAR_T *attrValue)
#else
static void         XmlEndOfAttrValue (attrValue)
CHAR_T     *attrValue;

#endif
{
   AttributeType     attrType;
   int		     attrKind, val;
   UCHAR_T           msgBuffer[MaxMsgLength];

   TtaGiveAttributeType (currentAttribute, &attrType, &attrKind);
   switch (attrKind)
     {
     case 0:       /* enumerate */
       (*(currentParserCtxt->MapAttributeValue)) (attrValue, attrType, &val);
       if (val <= 0)
	 {
	   usprintf (msgBuffer, TEXT("Unknown attribute value %s"), attrValue);
	   XmlParseError (XMLcontext.doc, msgBuffer, 0);	
	 }
       else
	 TtaSetAttributeValue (currentAttribute, val,
			       XMLcontext.lastElement, XMLcontext.doc);
       break;
     case 1:       /* integer */
       usscanf (attrValue, TEXT("%d"), &val);
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
   XhtmlEndOfAttrValue
   An attribute value has been read for a element that belongs to XHTML DTD
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         XhtmlEndOfAttrValue (CHAR_T *attrValue)
#else
static void         XhtmlEndOfAttrValue (attrValue)
CHAR_T     *attrValue;

#endif
{
   AttributeType       attrType, attrType1;
   Attribute	       attr;
   ElementType	       elType;
   Element             child;
   Language            lang;
   CHAR_T              translation;
   char                shape;
   STRING              buffer;
   STRING              attrName;
   int                 val;
   int                 length;
   int                 attrKind;
   ThotBool            done = FALSE;
   CHAR_T              msgBuffer[MaxMsgLength];

   /* treatments of some particular HTML attributes */
   if (!ustrcmp (lastMappedAttr->XMLattribute, TEXT("style")))
     {
       TtaSetAttributeText (currentAttribute, attrValue,
			    lastAttrElement, XMLcontext.doc);
       ParseHTMLSpecificStyle (XMLcontext.lastElement, attrValue,
			       XMLcontext.doc, 1, FALSE);
       done = TRUE;
     }
   else
     {
       if (!ustrcmp (lastMappedAttr->XMLattribute, TEXT("link")))
	   HTMLSetAlinkColor (XMLcontext.doc, attrValue);
       else
	   if (!ustrcmp (lastMappedAttr->XMLattribute, TEXT("alink")))
	       HTMLSetAactiveColor (XMLcontext.doc, attrValue);
	   else
	       if (!ustrcmp (lastMappedAttr->XMLattribute, TEXT("vlink")))
		   HTMLSetAvisitedColor (XMLcontext.doc, attrValue);
     }

   if (!done)
     {
       val = 0;
       translation = lastMappedAttr->AttrOrContent;

       switch (translation)
	 {
	 case 'C':	/* Content */
	   child = XhtmlPutInContent (attrValue);
	   if (child != NULL)
	       TtaAppendTextContent (child, TEXT("\" "), XMLcontext.doc);
	   break;

	 case 'A':
	   if (currentAttribute != NULL)
	     {
	       TtaGiveAttributeType (currentAttribute, &attrType, &attrKind);
	       switch (attrKind)
		 {
		 case 0:	/* enumerate */
		   MapHTMLAttributeValue (attrValue, attrType, &val);
		   if (val < 0)
		     {
		       TtaGiveAttributeType (currentAttribute,
					     &attrType, &attrKind);
		       attrName = TtaGetAttributeName (attrType);
		       usprintf (msgBuffer,
				 TEXT("Unknown attribute value \"%s = %s\""),
				 attrName, attrValue);
		       XmlParseError (XMLcontext.doc, msgBuffer, 0);

		       /* remove the attribute and replace it by an */
		       /* Invalid_attribute */
		       TtaRemoveAttribute (lastAttrElement,
					   currentAttribute, XMLcontext.doc);
		       usprintf (msgBuffer, TEXT("%s=%s"), attrName, attrValue);
		       MapHTMLAttribute (TEXT("unknown_attr"),
					  &attrType,
					  NULL,
					  XMLcontext.doc);
		       XhtmlCreateAttr (lastAttrElement, attrType,
					msgBuffer, TRUE, XMLcontext.doc);
		     }
		   else
		       TtaSetAttributeValue (currentAttribute, val,
					     lastAttrElement, XMLcontext.doc);
		   break;
		 case 1:	/* integer */
		   if (attrType.AttrTypeNum == HTML_ATTR_Border &&
		       !ustrcasecmp (attrValue, TEXT("border")))
		     {
		       /* border="border" for a table */
		       val = 1;
		       TtaSetAttributeValue (currentAttribute, val,
					     lastAttrElement, XMLcontext.doc);
		     }
		   else
		     {
		       if (usscanf (attrValue, TEXT("%d"), &val))
			   TtaSetAttributeValue (currentAttribute, val,
						 lastAttrElement, XMLcontext.doc);
		       else
			 {
			   TtaRemoveAttribute (lastAttrElement,
					       currentAttribute, XMLcontext.doc);
			   usprintf (msgBuffer,
				     TEXT("Invalid attribute value \"%s\""),
				     attrValue);
			   XmlParseError (XMLcontext.doc, msgBuffer, 0);
			 }
		     }
		   break;
		 case 2:	/* text */
		   if (currentAttribute != NULL)
		     {
		       TtaSetAttributeText (currentAttribute, attrValue,
					    lastAttrElement, XMLcontext.doc);
		       if (attrType.AttrTypeNum == HTML_ATTR_Langue)
			 {
			   /* it's a LANG attribute value */
			   lang = TtaGetLanguageIdFromName (attrValue);
			   if (lang == 0)
			     {
			       usprintf (msgBuffer,
					 TEXT("Unknown language: %s"),
					 attrValue);
			       XmlParseError (XMLcontext.doc, msgBuffer, 0);
			     }
			   else
			     {
			       /* change current language */
			       XMLcontext.language = lang;
			       languageStack[stackLevel - 1] = XMLcontext.language;
			     }
			   if (!TtaGetParent (lastAttrElement))
			     /* it's a LANG attribute on the root element */
			     /* set the RealLang attribute */
			     {
			       attrType1.AttrSSchema = DocumentSSchema;
			       attrType1.AttrTypeNum = HTML_ATTR_RealLang;
			       attr = TtaNewAttribute (attrType1);
			       TtaAttachAttribute (lastAttrElement,
						   attr, XMLcontext.doc);
			       TtaSetAttributeValue (attr,
						     HTML_ATTR_RealLang_VAL_Yes_,
						     lastAttrElement, XMLcontext.doc);
			     }
			 }
		     }
		   else
		     /* this is the content of an invalid attribute */
		     /* append it to the current Invalid_attribute */
		     {
		       length = ustrlen (attrValue) + 2;
		       length += TtaGetTextAttributeLength (currentAttribute);
		       buffer = TtaAllocString (length + 1);
		       TtaGiveTextAttributeValue (currentAttribute,
						  buffer, &length);
		       ustrcat (buffer, TEXT("="));
		       ustrcat (buffer, attrValue);
		       TtaSetAttributeText (currentAttribute, buffer,
					    lastAttrElement, XMLcontext.doc);
		       TtaFreeMemory (buffer);
		     }
		   break;
		 case 3:	/* reference */
		   break;
		 }
	     }
	   break;
	   
	 case SPACE:
	   XhtmlTypeAttrValue (attrValue);
	   break;

	 default:
	   break;
	 }

      if (lastMappedAttr->ThotAttribute == HTML_ATTR_Width__)
	 /* HTML attribute "width" for a table or a hr */
	 /* create the corresponding attribute IntWidthPercent or */
	 /* IntWidthPxl */
	 CreateAttrWidthPercentPxl (attrValue, lastAttrElement,
				    XMLcontext.doc, -1);
      else
	 if (!ustrcmp (lastMappedAttr->XMLattribute, TEXT("size")))
	   {
	     TtaGiveAttributeType (currentAttribute, &attrType, &attrKind);
	     if (attrType.AttrTypeNum == HTML_ATTR_Font_size)
	         CreateAttrIntSize (attrValue, lastAttrElement, XMLcontext.doc);
	   }
      else
	 if (!ustrcmp (lastMappedAttr->XMLattribute, TEXT("shape")))
	     {
	       child = TtaGetFirstChild (lastAttrElement);
	       if (child != NULL)
		 {
		   switch (val)
		     {
		     case HTML_ATTR_shape_VAL_rectangle:
		       shape = 'R';
		       break;
		     case HTML_ATTR_shape_VAL_circle:
		       shape = 'a';
		       break;
		     case HTML_ATTR_shape_VAL_polygon:
		       shape = 'p';
		       break;
		     default:
		       shape = SPACE;
		       break;
		     }
		   TtaSetGraphicsShape (child, shape, XMLcontext.doc);
		 }
	     }
	   else
	     if (!ustrcmp (lastMappedAttr->XMLattribute, TEXT("value")))
	       {
		 elType = TtaGetElementType (lastAttrElement);
		 if (elType.ElTypeNum == HTML_EL_Text_Input ||
		     elType.ElTypeNum == HTML_EL_Password_Input ||
		     elType.ElTypeNum == HTML_EL_File_Input ||
		     elType.ElTypeNum == HTML_EL_Input)
		   /* create a Default_Value attribute with the same content */
		   {
		     attrType1.AttrSSchema = attrType.AttrSSchema;
		     attrType1.AttrTypeNum = HTML_ATTR_Default_Value;
		     attr = TtaNewAttribute (attrType1);
		     TtaAttachAttribute (lastAttrElement, attr, XMLcontext.doc);
		     TtaSetAttributeText (attr, attrValue,
					  lastAttrElement, XMLcontext.doc);
		   }
	       }

       /* Some HTML attributes are equivalent to a CSS property:      */
       /*      background     ->                   background         */
       /*      bgcolor        ->                   background         */
       /*      text           ->                   color              */
       /*      color          ->                   color              */
	     else
	       if (!ustrcmp (lastMappedAttr->XMLattribute, TEXT("background")))
		 {
		   if (ustrlen (attrValue) > MaxMsgLength - 30)
		       attrValue[MaxMsgLength - 30] = WC_EOS;
		   usprintf (msgBuffer,
			     TEXT("background: url(%s)"),
			     attrValue );
		   ParseHTMLSpecificStyle (XMLcontext.lastElement, msgBuffer,
					   XMLcontext.doc, 1, FALSE);
		 }
	       else
		 if (!ustrcmp (lastMappedAttr->XMLattribute, TEXT("bgcolor")))
		     HTMLSetBackgroundColor (XMLcontext.doc,
					     XMLcontext.lastElement, attrValue);
		 else
		   if (!ustrcmp (lastMappedAttr->XMLattribute, TEXT("text")) ||
		       !ustrcmp (lastMappedAttr->XMLattribute, TEXT("color")))
		     HTMLSetForegroundColor (XMLcontext.doc,
					     XMLcontext.lastElement, attrValue);
     }
}

/*----------------------------------------------------------------------
   EndOfAttributeValue
   An attribute value has been read from the HTML file.
   Put that value in the current Thot attribute.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void    EndOfAttributeValue (CHAR_T *attrValue)
#else
static void    EndOfAttributeValue (attrValue)
CHAR_T     *attrValue;

#endif
{

   if (lastMappedAttr != NULL  || currentAttribute != NULL) 
     {
       if (currentParserCtxt != NULL)
	 {
	   if (XMLSpaceAttribute)
	     XmlWhiteSpaceInStack (attrValue);
	   if (ustrcmp (currentParserCtxt->SSchemaName, TEXT("HTML")) == 0)
	     XhtmlEndOfAttrValue (attrValue);
	   else
	     XmlEndOfAttrValue (attrValue);
	 }
     }

   currentAttribute = NULL;
   HTMLStyleAttribute = FALSE;
   XMLSpaceAttribute = FALSE;
}

/*--------------------  Attributes  (end)  ---------------------*/


/*--------------------  Entities  (start)  ---------------------*/

/*----------------------------------------------------------------------
   PutMathMLEntity
   A MathML entity has been created by the XML parser.
   Create an attribute EntityName containing the entity name.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void        PutMathMLEntity (USTRING entityValue,
			     Language lang,
			     STRING entityName,
			     Document doc)
#else
void        PutMathMLEntity (entityValue,
			     lang,
			     entityName,
			     doc)
USTRING   entityValue;
Language  lang;
STRING    entityName;
Document  doc;

#endif
{
  ElementType	 elType;
  Element	 elText;
  AttributeType attrType;
  Attribute	 attr;
  int		 len, code;
#define MAX_ENTITY_LENGTH 80
  CHAR_T	 buffer[MAX_ENTITY_LENGTH];
  
  if (lang < 0)
    /* Unknown entity */
    {
      /* By default display a question mark */
      entityValue[0] = '?';
      entityValue[1] = WC_EOS;
      lang = TtaGetLanguageIdFromAlphabet('L');
      /* Let's see if we can do more */
      if (entityName[0] == '#')
	/* It's a number */
	{
	  if (entityName[1] == 'x')
	    /* It's a hexadecimal number */
	    usscanf (&entityName[2], TEXT("%x"), &code);
	  else
	    /* It's a decimal number */
	    usscanf (&entityName[1], TEXT("%d"), &code);
	  
	  GetFallbackCharacter (code, entityValue, &lang);
	}
    }
  
  elType.ElTypeNum = MathML_EL_TEXT_UNIT; 
  elType.ElSSchema = GetMathMLSSchema (doc);
  elText = TtaNewElement (doc, elType);
  XmlSetElemLineNumber (elText);
  InsertXmlElement (&elText);
  TtaSetTextContent (elText, entityValue, lang, doc);
  XMLcontext.lastElementClosed = TRUE;
  XMLcontext.mergeText = FALSE; 
  
  /* Make that text leaf read-only */
  TtaSetAccessRight (elText, ReadOnly, doc);
  
  /* Associate an attribute EntityName with the new text leaf */
  attrType.AttrSSchema = GetMathMLSSchema (doc);
  attrType.AttrTypeNum = MathML_ATTR_EntityName;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (elText, attr, doc);
  
  len = ustrlen (entityName);
  if (len > MAX_ENTITY_LENGTH -3)
    len = MAX_ENTITY_LENGTH -3;
  buffer[0] = '&';
  ustrncpy (&buffer[1], entityName, len);
  buffer[len+1] = ';';
  buffer[len+2] = WC_EOS;
  TtaSetAttributeText (attr, buffer, elText, doc);
}

/*----------------------------------------------------------------------
   CreateXmlEntity
   End of a XML entity. 
   Search that entity in the corresponding entity table and 
   put the corresponding character in the corresponding element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CreateXmlEntity (STRING entityName)
#else
static void         CreateXmlEntity (entityName)
STRING              entityName;
#endif
{
   CHAR_T         msgBuffer[MaxMsgLength];
   STRING         buffer;
   CHAR_T*        ptr;
   CHAR_T	  alphabet;
   int            entityVal;	
   UCHAR_T        entityValue[MaxEntityLength];	
   Language	  lang;

#ifdef LC
   printf ("\n CreateXmlEntity - Name : %s", entityName);
#endif /* LC */

   buffer = TtaAllocString (ustrlen (entityName));
   ustrcpy (buffer, &entityName[1]);
   if ((ptr = ustrrchr (buffer, TEXT(';'))))
       ustrcpy (ptr, TEXT("\0"));
#ifdef LC
   printf ("\n CreateXmlEntity - buffer:%s", buffer);
#endif /* LC */
   
   
   if (ustrcmp (currentParserCtxt->SSchemaName, TEXT("HTML")) == 0)
       XhtmlMapEntity (buffer, &entityVal, &alphabet);
   else
       (*(currentParserCtxt->MapEntity)) (buffer, entityValue, &alphabet);
   
   lang = 0;
   if (alphabet == WC_EOS)
     {
       /* Unknown entity */
       entityValue[0] = WC_EOS;
       lang = -1;
       usprintf (msgBuffer, TEXT("Unknown entity %s;"), entityName);
       XmlParseError (XMLcontext.doc, msgBuffer, 0);
     }
   else
     {
       if (entityValue[0] != WC_EOS)
	 lang = TtaGetLanguageIdFromAlphabet(alphabet);
     }
   
   if (ustrcmp (currentParserCtxt->SSchemaName, TEXT("HTML")) == 0)
       XhtmlEntityCreated (entityVal, lang, entityName, &XMLcontext);
   else
       (*(currentParserCtxt->EntityCreated)) (entityValue, lang,
					      buffer, XMLcontext.doc);       
   TtaFreeMemory (buffer);
}
/*--------------------  Entities  (end)  ---------------------*/


/*--------------------  Comments  (start)  ---------------------*/
/*----------------------------------------------------------------------
   CreateXmlComment
   Create a comment element into the Thot tree.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void    CreateXmlComment (CHAR_T *commentValue)
#else
static void    CreateXmlComment (commentValue)
CHAR_T     *commentValue;

#endif
{
   ElementType   elType, elTypeTxt;
   Element  	 commentEl, commentLineEl, commentText;
   STRING        mappedName;
   CHAR_T        cont;
   UCHAR_T       msgBuffer[MaxMsgLength];
   int           start, i, error;

   /* Create a Thot element for the comment */
   elType.ElSSchema = NULL;
   elType.ElTypeNum = 0;
   GetXmlElType (TEXT("XMLcomment"), &elType,
		 &mappedName, &cont, XMLcontext.doc);
   if (elType.ElTypeNum <= 0)
     {
       usprintf (msgBuffer,
		 TEXT("Unknown element %s"),
		 commentValue);
       XmlParseError (XMLcontext.doc, msgBuffer, 0);
     }
   else
     {
       commentEl = TtaNewElement (XMLcontext.doc, elType);
       XmlSetElemLineNumber (commentEl);
       InsertXmlElement (&commentEl);

       /* Create a XMLcomment_line element as the first child of */
       /* Element XMLcomment */
       elType.ElSSchema = NULL;
       elType.ElTypeNum = 0;
       GetXmlElType (TEXT("XMLcomment_line"), &elType,
		     &mappedName, &cont, XMLcontext.doc);
       commentLineEl = TtaNewElement (XMLcontext.doc, elType);
       XmlSetElemLineNumber (commentLineEl);
       TtaInsertFirstChild (&commentLineEl, commentEl, XMLcontext.doc);

       /* Create a TEXT element as the first child of element XMLcomment_line*/
       elTypeTxt.ElSSchema = elType.ElSSchema;
       elTypeTxt.ElTypeNum = 1;
       commentText = TtaNewElement (XMLcontext.doc, elTypeTxt);
       XmlSetElemLineNumber (commentText);
       TtaInsertFirstChild (&commentText, commentLineEl, XMLcontext.doc);
       TtaSetTextContent (commentText, TEXT(""), XMLcontext.language,
			  XMLcontext.doc);
       /* Look for line break in the comment and create as many */
       /* XMLcomment_line elements as needed */
       i = 0; start = 0;
       do
	 {
	   if ((int)commentValue[i] == EOL || (int)commentValue[i] == __CR__)
	     /* New line */
	     {
	       commentValue[i] = EOS;
	       TtaAppendTextContent (commentText, &commentValue[start],
				     XMLcontext.doc);
	       /* Create a new XMLcomment_line element */
	       commentLineEl = TtaNewElement (XMLcontext.doc, elType);
	       XmlSetElemLineNumber (commentLineEl);
	       /* Inserts the new XMLcomment_line after the previous one */
	       TtaInsertSibling (commentLineEl, TtaGetParent (commentText),
				 FALSE, XMLcontext.doc);
	       /* Create a TEXT element as the first child of new XMLcomment_line element */
	       commentText = TtaNewElement (XMLcontext.doc, elTypeTxt);
	       XmlSetElemLineNumber (commentText);
	       TtaInsertFirstChild (&commentText, commentLineEl,
				    XMLcontext.doc);
	       TtaSetTextContent (commentText, TEXT(""), XMLcontext.language,
				  XMLcontext.doc);
	       i++;
	       start = i;   /* Start of next comment line */
	     }
	   else if (commentValue[i] != EOS)
	     i++;
	 }
       while (commentValue[i] != EOS);
       /* Process last line */
       if (i > start + 1)
	 TtaAppendTextContent (commentText, &commentValue[start],
			       XMLcontext.doc);
       (*(currentParserCtxt->ElementComplete)) (commentEl, XMLcontext.doc,
						&error);
       XMLcontext.lastElementClosed = TRUE;
     }
}
/*--------------------  Comments  (end)  ---------------------*/


/*-----------  EXPAT handlers associated with Amaya  ---------------*/

/*----------------------------------------------------------------------
   Hndl_CdataStart
   Handlers that get called at the beginning of a CDATA section
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     Hndl_CdataStart (void *userData)
#else  /* __STDC__ */
static void     Hndl_CdataStart (userData)
void            *userData; 
#endif  /* __STDC__ */

{
#ifdef LC
  printf ("\n Hndl_CdataStart");
#endif /* LC */
}

/*----------------------------------------------------------------------
   Hndl_CdataEnd
   Handlers that get called at the end of a CDATA section
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     Hndl_CdataEnd (void *userData)
#else  /* __STDC__ */
static void     Hndl_CdataEnd (userData)
void            *userData; 
#endif  /* __STDC__ */

{
#ifdef LC
  printf ("\n Hndl_CdataEnd");
#endif /* LC */
}

/*----------------------------------------------------------------------
   Hndl_CharacterData
   Handler for the text
   The string the handler receives is NOT zero terminated.
   We have to use the length argument to deal with the end of the string.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     Hndl_CharacterData (void *userData,
				    const XML_Char *data,
				    int   length)
#else  /* __STDC__ */
static void     Hndl_CharacterData (userData, data, length)
void            *userData; 
const XML_Char  *data;
int              length;
#endif  /* __STDC__ */

{
   unsigned char *buffer;
   CHAR_T        *bufferwc;
   int            i;

#ifdef LC
   /* printf ("\n Hndl_CharacterData - length = %d - ", length); */
#endif /* LC */

   buffer = TtaAllocString (length + 1);
   bufferwc = TtaAllocString (length + 1);

   for (i=0; i<length; i++)
     {
       buffer[i] = data[i];
#ifdef LC
       /* printf ("%c", data[i]); */
#endif /* LC */
     }
   buffer[length] = WC_EOS;

   /* Transform UTF_8 coded buffer into WC coded buffer */
   TtaMBS2WCS (&buffer, &bufferwc, UTF_8);
   PutInXmlElement (bufferwc);

   TtaFreeMemory (buffer);
   TtaFreeMemory (bufferwc);
}

/*----------------------------------------------------------------------
   Hndl_Comment
   Handler for comments
   The data is all text inside the comment delimiters
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     Hndl_Comment (void *userData,
			      const XML_Char *data)
#else  /* __STDC__ */
static void     Hndl_Comment (userData,
			      data)
void            *userData; 
const XML_Char  *data;
#endif  /* __STDC__ */

{
#ifdef LC
   printf ("\n Hndl_Comment %s", data);
#endif /* LC */

   CreateXmlComment ((CHAR_T*) data);
}

/*----------------------------------------------------------------------
   Hndl_Default
   Handler for any characters in the document which wouldn't
   otherwise be handled.
   This includes both data for which no handlers can be set
   (like some kinds of DTD declarations) and data which could be
   reported but which currently has no handler set.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     Hndl_Default (void *userData,
			      const XML_Char *data,
			      int   length)
#else  /* __STDC__ */
static void     Hndl_Default (userData, data, length)
void            *userData; 
const XML_Char  *data;
int              length;
#endif  /* __STDC__ */

{
#ifdef LC
  int  i;

  printf ("\n Hndl_Default - length = %d - ", length);

  for (i=0; i<length; i++)
    {
      printf ("%c", data[i]);
    }
#endif /* LC */
}

/*----------------------------------------------------------------------
   Hndl_DefaultExpand
   Default handler with expansion of internal entity references
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     Hndl_DefaultExpand (void *userData,
				    const XML_Char *data,
				    int   length)
#else  /* __STDC__ */
static void     Hndl_DefaultExpand (userData, data, length)
void            *userData; 
const XML_Char  *data;
int              length;
#endif  /* __STDC__ */

{
   int        i;
   STRING     buffer;

#ifdef LC
   printf ("\n Hndl_DefaultExpand - length = %d - ", length);

   for (i=0; i<length; i++)
       printf ("%c", data[i]);
#endif /* LC */
   
   /* Treatment for the entities */
   if (length > 1 && data[0] == '&')
     {
       buffer = TtaAllocString (length + 1);
       for (i = 0; i < length; i++)
	 buffer[i] = data[i];
       buffer[length] = WC_EOS;
       CreateXmlEntity (buffer);
       TtaFreeMemory (buffer);
     }
}

/*----------------------------------------------------------------------
   Hndl_DoctypeStart
   Handler for the start of the DOCTYPE declaration.
   It is called when the name of the DOCTYPE is encountered.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     Hndl_DoctypeStart (void *userData,
				   const XML_Char *doctypeName)
#else  /* __STDC__ */
static void     Hndl_DoctypeStart (userData, doctypeName)
void            *userData; 
const XML_Char  *doctypeName;
#endif  /* __STDC__ */

{
#ifdef LC
   printf ("\n Hndl_DoctypeStart %s", doctypeName);
#endif /* LC */
}

/*----------------------------------------------------------------------
   Hndl_DoctypeEnd
   Handler for the start of the DOCTYPE declaration.
   It is called when the closing > is encountered,
   but after processing any external subset.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     Hndl_DoctypeEnd (void *userData)
#else  /* __STDC__ */
static void     Hndl_DoctypeEnd (userData)
void            *userData; 
#endif  /* __STDC__ */

{
#ifdef LC
   printf ("\n Hndl_DoctypeEnd");
#endif /* LC */
}

/*----------------------------------------------------------------------
   Hndl_ElementStart
   Handler for start tags
   Attributes are passed as a pointer to a vector of char pointers
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     Hndl_ElementStart (void *userData,
				   const XML_Char *name,
				   const XML_Char **attlist)
#else  /* __STDC__ */
static void     Hndl_ElementStart (userData, name, attlist)
void            *userData; 
const XML_Char  *name;
const XML_Char **attlist;
#endif  /* __STDC__ */

{
   int             nbatts;
   CHAR_T         *buffer = NULL;
   CHAR_T         *bufAttr;
   CHAR_T         *bufName = NULL;
   CHAR_T         *ptr;
   PtrParserCtxt   elementParserCtxt = NULL;
   CHAR_T          msgBuffer[MaxMsgLength];

#ifdef LC
   printf ("\n Hndl_ElementStart '%s'\n", name);
#endif /* LC */
  
   /* Initialize root element name and parser context if not done yet */
   if (XMLrootName[0] == WC_EOS)
     strcpy (XMLrootName, (CHAR_T*) name);

   /* Treatment for the GI */
   if (XMLcontext.parsingTextArea)
     {
       /* We are parsing the contents of a TEXTAREA element. If a start
	  tag appears, consider it as plain text */
     }
   else
     {
       /* Look for the context associated with that element */
       buffer = TtaGetMemory ((strlen (name) + 1));
       ustrcpy (buffer, (CHAR_T*) name);
       if ((ptr = ustrrchr (buffer, NS_SEP)))
	 {
	   *ptr = WC_EOS;
	    ptr++;
	    bufName = TtaGetMemory ((strlen (ptr) + 1));
	    ustrcpy (bufName, ptr);
	    
            if ((currentParserCtxt != NULL &&
		 ustrcmp (buffer, currentParserCtxt->UriName)) ||
		(currentParserCtxt == NULL))
	      ChangeXmlParserContextUri (buffer);
	 }
       else
	 {
	   bufName = TtaGetMemory (strlen (buffer) + 1);
	   ustrcpy (bufName, buffer);
	 }

       /* We stop parsing if context is null, ie,
	  if Thot doesn't know the corresponding namespaces */ 
      if (currentParserCtxt == NULL)
	{
	  usprintf (msgBuffer, 
		    TEXT("Unknown Namepaces for element :\"%s\""), name);
	  XmlParseError (XMLcontext.doc, msgBuffer, 0);
	  XMLabort = TRUE;
	  DisableExpatParser ();
	  return;
	}
      else
	  elementParserCtxt = currentParserCtxt;

      /* Ignore the virtual root of a XML sub-tree */
      if (ustrcmp (bufName, SUBTREE_ROOT) != 0)
	{
	  /* Treatment called at the beginning of start tag */
	  StartOfXmlStartElement (bufName);
	  
	  /*-------  Treatment of the attributes -------*/
	  nbatts = XML_GetSpecifiedAttributeCount (parser);
	  while (*attlist != NULL)
	    {
	      /* Create the corresponding Thot attribute */
	      bufAttr = TtaGetMemory ((strlen (*attlist)) + 1);
	      strcpy (bufAttr, *attlist);
#ifdef LC
	      printf ("\n  attr %s :", bufAttr);
#endif /* LC */
	      EndOfAttributeName (bufAttr);
	      TtaFreeMemory (bufAttr);
	      
	      /* Element context if attribute name is unknown */
	      if (currentParserCtxt == NULL)
		currentParserCtxt = elementParserCtxt;
	      
	      /* Filling of the attribute value */
	      attlist++;
	      if (*attlist != NULL)
		{
		  bufAttr = TtaGetMemory ((strlen (*attlist)) + 1);
		  strcpy (bufAttr, *attlist);
#ifdef LC
		  printf (" value=%s ", bufAttr);
#endif /* LC */
		  EndOfAttributeValue (bufAttr);
		  TtaFreeMemory (bufAttr);
		}
	      attlist++;
	    }
	  /* Restore the context (it may have been changed */
	  /* by the treatment of the attributes) */
	  currentParserCtxt = elementParserCtxt;
	  
	  /*----- Treatment called at the end of start tag -----*/
	  EndOfXmlStartElement (bufName);
	  
	  /*----- We are immediately after a start tag -----*/
	  ImmediatelyAfterTag = TRUE;
	}

       TtaFreeMemory (bufName);
       TtaFreeMemory (buffer);
     }
}

/*----------------------------------------------------------------------
   Hndl_ElementEnd
   Handler for end tags
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     Hndl_ElementEnd (void *userData,
				 const XML_Char *name)
#else  /* __STDC__ */
static void     Hndl_ElementEnd (userData, name)
void            *userData; 
const XML_Char  *name
#endif  /* __STDC__ */

{
   CHAR_T       *bufName;
   CHAR_T       *buffer;
   CHAR_T       *ptr;

#ifdef LC
   printf ("\n Hndl_ElementEnd '%s'", name);
#endif /* LC */

   ImmediatelyAfterTag = FALSE;

   /* Look for the context associated with that element */
   buffer = TtaGetMemory ((strlen (name) + 1));
   ustrcpy (buffer, (CHAR_T*) name);
   if ((ptr = ustrrchr (buffer, NS_SEP)))
     {
       *ptr = WC_EOS;
       ptr++;
       bufName = TtaGetMemory ((strlen (ptr) + 1));
       ustrcpy (bufName, ptr);
       
       if ((currentParserCtxt != NULL &&
	    ustrcmp (buffer, currentParserCtxt->UriName)) ||
	   (currentParserCtxt == NULL))
	 ChangeXmlParserContextUri (buffer);
     }
   else
     {
       bufName = TtaGetMemory (strlen (buffer) + 1);
       ustrcpy (bufName, buffer);
       if (currentParserCtxt == NULL)
	 ChangeXmlParserContextTagName (bufName);
     }
       
   /* Ignore the virtual root of a XML sub-tree */
   if (ustrcmp (bufName, SUBTREE_ROOT) != 0)
     XmlEndElement ((CHAR_T*) (bufName));

   /* Is it the end tag of the root element ? */
   if (!ustrcmp (XMLrootName, (CHAR_T*) name) && stackLevel == 1)
     XMLrootClosed = TRUE;

   TtaFreeMemory (bufName);   
   TtaFreeMemory (buffer);   
}

/*----------------------------------------------------------------------
   Hndl_ExternalEntityRef
   Handler for external entity references.
   his handler is also called for processing an external DTD subset.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int     Hndl_ExternalEntityRef (void *userData,
				       const XML_Char *context,
				       const XML_Char *base,
				       const XML_Char *systemId,
				       const XML_Char *publicId)
#else  /* __STDC__ */
static int     Hndl_ExternalEntityRef (userData,
				       context,
				       base,
				       systemId,
				       publicId)
void            *userData; 
const XML_Char  *context;
const XML_Char  *base;
const XML_Char  *systemId;
const XML_Char  *publicId;
#endif  /* __STDC__ */

{
#ifdef LC
  printf ("\n Hndl_ExternalEntityRef");
  printf ("\n   context  : %s", context);
  printf ("\n   base     : %s", base);
  printf ("\n   systemId : %s", systemId);
  printf ("\n   publicId : %s", publicId);
#endif /* LC */
  return 1;
}

/*----------------------------------------------------------------------
   Hndl_NameSpaceStart
   Handler for the start of namespace declarations
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     Hndl_NameSpaceStart (void *userData,
				     const XML_Char *prefix,
				     const XML_Char *uri)
#else  /* __STDC__ */
static void     Hndl_NameSpaceStart (userData, prefix, uri)
void            *userData; 
const XML_Char  *prefix;
const XML_Char  *uri;
#endif  /* __STDC__ */

{   

#ifdef LC
  printf ("\n Hndl_NameSpaceStart");
  printf ("\n   prefix : %s; uri : %s", prefix, uri);
#endif /* LC */
}

/*----------------------------------------------------------------------
   Hndl_NameSpaceEnd
   Handler for the end of namespace declarations
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     Hndl_NameSpaceEnd (void *userData,
				   const XML_Char *prefix)
#else  /* __STDC__ */
static void     Hndl_NameSpaceEnd (userData, prefix)
void            *userData; 
const XML_Char  *prefix;
#endif  /* __STDC__ */

{
#ifdef LC
  printf ("\n Hndl_NameSpaceEnd");
  printf ("\n   prefix : %s", prefix);
#endif /* LC */
}

/*----------------------------------------------------------------------
   Hndl_Notation
   Handler that receives notation declarations.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     Hndl_Notation (void *userData,
			       const XML_Char *notationName,
			       const XML_Char *base,
			       const XML_Char *systemId,
			       const XML_Char *publicId)
#else  /* __STDC__ */
static void     Hndl_Notation (userData,
			       notationName,
			       base,
			       systemId,
			       publicId)
void            *userData; 
const XML_Char  *notationName;
const XML_Char  *base;
const XML_Char  *systemId;
const XML_Char  *publicId;
#endif  /* __STDC__ */

{
#ifdef LC
  printf ("\n Hndl_Notation");
  printf ("\n   notationName : %s", notationName);
  printf ("\n   base         : %s", base);
  printf ("\n   systemId     : %s", systemId);
  printf ("\n   publicId     : %s", publicId);
#endif /* LC */
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
#ifdef __STDC__
static int     Hndl_NotStandalone (void *userData)
#else  /* __STDC__ */
static int     Hndl_NotStandalone (userData)
void            *userData; 
#endif  /* __STDC__ */

{
#ifdef LC
  printf ("\n Hndl_NotStandalone");
#endif /* LC */
  return 1;
}

/*----------------------------------------------------------------------
   Hndl_PI
   Handler for processing instructions.
   The target is the first word in the processing instruction.
   The pidata is the rest of the characters in it after skipping
   all whitespace after the initial word.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     Hndl_PI (void *userData,
			 const XML_Char *target,
			 const XML_Char *pidata)
#else  /* __STDC__ */
static void     Hndl_PI (userData, target, pidata)
void            *userData; 
const XML_Char  *target;
const XML_Char  *pidata;
#endif  /* __STDC__ */

{
#ifdef LC
  printf ("\n Hndl_PI");
  printf ("\n   target : %s", target);
  printf ("\n   pidata : %s", pidata);
  /* No treatment in Amaya for PI */
#endif /* LC */
}

/*----------------------------------------------------------------------
   Hndl_UnknownEncoding
   Handler to deal with encodings other than the built in
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int     Hndl_UnknownEncoding (void           *encodingData,
				     const XML_Char *name,
				     XML_Encoding   *info)
#else  /* __STDC__ */
static int     Hndl_UnknownEncoding (userData, data, length)
void            *encodingData; 
const XML_Char  *name;
XML_Encoding    *info
#endif  /* __STDC__ */

{
#ifdef LC
  printf ("\n Hndl_UnknownEncoding");
  printf ("\n   name : %s", name);
#endif /* LC */
  return 1;
}

/*----------------------------------------------------------------------
   Hndl_UnparsedEntity
   Handler that receives declarations of unparsed entities.
   These are entity declarations that have a notation (NDATA) field:
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     Hndl_UnparsedEntity (void *userData,
				     const XML_Char *entityName,
				     const XML_Char *base,
				     const XML_Char *systemId,
				     const XML_Char *publicId,
				     const XML_Char *notationName)
#else  /* __STDC__ */
static void     Hndl_UnparsedEntity (userData,
				     entityName,
				     base,
				     systemId,
				     publicId,
				     notationName)
void            *userData; 
const XML_Char  *entityName;
const XML_Char  *base;
const XML_Char  *systemId;
const XML_Char  *publicId;
const XML_Char  *notationName;
#endif  /* __STDC__ */

{
#ifdef LC
  printf ("\n Hndl_UnparsedEntity");
  printf ("\n   entityName   : %s", entityName);
  printf ("\n   base         : %s", base);
  printf ("\n   systemId     : %s", systemId);
  printf ("\n   publicId     : %s", publicId);
  printf ("\n   notationName : %s", notationName);
#endif /* LC */
}

/*---------------- End of Handler definition ----------------*/

/*----------------------------------------------------------------------
   FreeXmlParser
   Frees all ressources associated with the XML parser.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FreeXmlParserContexts (void)
#else
void                FreeXmlParserContexts ()
#endif
{
   PtrParserCtxt       ctxt, nextCtxt;

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
#ifdef __STDC__
static void      DisableExpatParser ()
#else  /* __STDC__ */
static void      DisableExpatParser ()
#endif  /* __STDC__ */

{    
  int    paramEntityParsing;

  paramEntityParsing = XML_PARAM_ENTITY_PARSING_NEVER;
  XML_SetCdataSectionHandler (parser, NULL, NULL);
  XML_SetCharacterDataHandler (parser, NULL);
  XML_SetCommentHandler (parser, NULL);
  XML_SetDefaultHandlerExpand (parser, NULL);
  XML_SetDoctypeDeclHandler (parser, NULL, NULL);
  XML_SetElementHandler (parser, NULL, NULL);
  XML_SetExternalEntityRefHandler (parser, NULL);
  XML_SetNamespaceDeclHandler (parser, NULL, NULL);
  XML_SetNotationDeclHandler (parser, NULL);
  XML_SetNotStandaloneHandler (parser, NULL);
  XML_SetParamEntityParsing (parser, paramEntityParsing);
  XML_SetProcessingInstructionHandler (parser, NULL);
  XML_SetUnknownEncodingHandler (parser, NULL, 0);
  XML_SetUnparsedEntityDeclHandler (parser, NULL);
}
/*----------------------------------------------------------------------
   FreeExpatParser
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         FreeExpatParser ()
#else  /* __STDC__ */
static void         FreeExpatParser ()
#endif  /* __STDC__ */

{  
  DisableExpatParser ();
  XML_ParserFree (parser);
  parser = NULL;
}


/*----------------------------------------------------------------------
   InitializeExpatParser
   Specific initialization for expat
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         InitializeExpatParser ()
#else  /* __STDC__ */
static void         InitializeExpatParser ()
#endif  /* __STDC__ */

{  
  int    paramEntityParsing;

  /*  Enable parsing of parameter entities */
  paramEntityParsing = XML_PARAM_ENTITY_PARSING_UNLESS_STANDALONE;

  /* Construct a new parser with namespace processing */
  parser = XML_ParserCreateNS ("ISO-8859-1", NS_SEP);
 
  /* Define the user data pointer that gets passed to handlers */
  /* (not use  Amaya actually) */
  /* XML_SetUserData (parser, (void*) doc); */
  
  /* Set handlers that get called at the beginning 
     and end of a CDATA section */
  XML_SetCdataSectionHandler (parser,
			      Hndl_CdataStart,
			      Hndl_CdataEnd);
    
  /* Set a text handler */
  XML_SetCharacterDataHandler (parser,
			       Hndl_CharacterData);
  
  /* Set a handler for comments */
  XML_SetCommentHandler (parser,
			 Hndl_Comment);
  
  /* Set default handler with  no expansion of internal entity references */
  /* 
     XML_SetDefaultHandler (parser,
                            Hndl_Default);
  */
  
  /* Set a default handler with expansion of internal entity references */
  XML_SetDefaultHandlerExpand (parser,
			       Hndl_DefaultExpand);

  /* Set a handler for DOCTYPE declaration */
  XML_SetDoctypeDeclHandler (parser,
			     Hndl_DoctypeStart,
			     Hndl_DoctypeEnd); 

  /* Set handlers for start and end tags */
  XML_SetElementHandler (parser,
			 Hndl_ElementStart,
			 Hndl_ElementEnd);
 
  /* Set an external entity reference handler */
  XML_SetExternalEntityRefHandler (parser,
				   Hndl_ExternalEntityRef);
  
  /* Set handlers for namespace declarations */
  XML_SetNamespaceDeclHandler (parser,
			       Hndl_NameSpaceStart,
			       Hndl_NameSpaceEnd);
  
  /* Set a handler for notation declarations */
  XML_SetNotationDeclHandler (parser,
			      Hndl_Notation);
  
  /* Set a handler for no 'standalone' document */
  XML_SetNotStandaloneHandler (parser,
			       Hndl_NotStandalone);

  /* Controls parsing of parameter entities */
  XML_SetParamEntityParsing (parser,
			     paramEntityParsing);
  
  /* Set a handler for processing instructions */
  XML_SetProcessingInstructionHandler (parser,
				       Hndl_PI);
  
  /* Set a handler to deal with encodings other than the built in */
  XML_SetUnknownEncodingHandler (parser,
				 Hndl_UnknownEncoding, 0);
  
  /* Set a handler that receives declarations of unparsed entities */
  XML_SetUnparsedEntityDeclHandler (parser,
				    Hndl_UnparsedEntity);
}

/*----------------------------------------------------------------------
   InitializeXmlParsingContext
   initializes variables and stack for parsing file.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void    InitializeXmlParsingContext (Document doc,
					    Element  lastElem,
					    ThotBool isClosed,
					    ThotBool isSubTree)
#else  /* __STDC__ */
static void    InitializeXmlParsingContext (doc, lastElem, isClosed, isSubtree)

Document   doc;
Element    lastElem;
ThotBool   isClosed;
ThotBool   isSubTree;
#endif  /* __STDC__ */
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
  XMLrootName[0] = WC_EOS;
  XMLrootClosed = FALSE;

  XMLabort = FALSE;
  htmlLineRead = 0;
  htmlCharRead = 0;
  
  /* initialize the stack of opened elements */
  stackLevel = 1;
  elementStack[0] = rootElement;
}

/*----------------------------------------------------------------------
   ParseXmlSubTree
   Parse a XML sub-tree given in buffer and complete the corresponding
   Thot abstract tree.
   Return TRUE if the parsing of the XML sub-tree don't detect errors.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool    ParseXmlSubTree (char     *xmlBuffer,
			     Element  *el,
			     ThotBool *isclosed,
			     Document  doc,
			     Language  lang)
#else
ThotBool    ParseXmlSubTree (xmlBuffer, el, isclosed, doc, lang)

char      *xmlBuffer;
Element   *el;
ThotBool  *isclosed;
Document   doc;
Language   lang;
#endif
{
  int          tmpLen = 0;
  CHAR_T      *transBuffer = NULL;
  CHAR_T      *schemaName;
  ElementType  elType;

  if (xmlBuffer == NULL)
    return FALSE;

  /* general initialization */
  rootElement = NULL;
  InitializeXmlParsingContext (doc, *el, *isclosed, TRUE);

  /* specific Initialization */
  XMLcontext.language = lang;
  DocumentSSchema = TtaGetDocumentSSchema (doc);

  /* Expat initialization */
  InitializeExpatParser ();

  /* initialize all parser contexts */
  if (firstParserCtxt == NULL)
    InitXmlParserContexts ();
  elType = TtaGetElementType (XMLcontext.lastElement);
  schemaName = TtaGetSSchemaName(elType.ElSSchema);
  ChangeXmlParserContextDTD (schemaName);

  /* Parse virtual DOCTYPE */
  if (!XML_Parse (parser, DECL_DOCTYPE, DECL_DOCTYPE_LEN, 0))
    {
      XmlParseError (XMLcontext.doc,
		     (CHAR_T *) XML_ErrorString (XML_GetErrorCode (parser)), 0);
      XMLabort = TRUE;
    }

  /* Parse the input XML buffer and complete the Thot document */
  if (!XMLabort)
    {
      /* We create a virtual root for the sub-tree to be parsed */
      tmpLen = (strlen (xmlBuffer)) + 1;
      tmpLen = tmpLen + (2 * SUBTREE_ROOT_LEN) + 1;
      transBuffer = TtaGetMemory (tmpLen);
      strcpy (transBuffer, TEXT("<"));
      strcat (transBuffer, SUBTREE_ROOT);
      strcat (transBuffer, TEXT(">"));
      strcat (transBuffer, xmlBuffer);
      strcat (transBuffer, TEXT("</"));
      strcat (transBuffer, SUBTREE_ROOT);
      strcat (transBuffer, TEXT(">"));
      tmpLen = strlen (transBuffer);

      if (!XML_Parse (parser, transBuffer, tmpLen, 1))
	{
	  if (!XMLrootClosed)
	    {
	      XmlParseError (XMLcontext.doc, (CHAR_T *) XML_ErrorString (XML_GetErrorCode (parser)), 0);
	      XMLabort = TRUE;
	    }
	}
      if (transBuffer != NULL)   
	TtaFreeMemory (transBuffer);   
    }

  /* Free expat parser */ 
  FreeXmlParserContexts ();
  FreeExpatParser ();

  return (!XMLabort);
}

/*----------------------------------------------------------------------
   ParseIncludedXml
   Parse a XML sub-tree included in a HTML document and complete the 
   corresponding Thot abstract tree.
   Xml sub-tree is given in infile or htmlBuffer, one parameter should 
   be null
   Return TRUE if the parsing of the sub-tree has no error.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool    ParseIncludedXml (FILE     *infile,
			      char     *infileBuffer,
			      int       infileBufferLength,
			      ThotBool *infileEnd,
			      ThotBool *infileNotToRead,
			      char     *infilePreviousBuffer,
			      char     *htmlBuffer,
			      int      *index,
			      int      *nbLineRead,
			      int      *nbCharRead,
			      STRING    DTDname,
			      Document  doc,
			      Element  *el,
			      ThotBool *isclosed,
			      Language  lang)
#else
ThotBool    ParseIncludedXml (infile, infileBuffer, infileBufferLength,
			      infileEnd, infileNotToRead, infilePreviousBuffer,
			      htmlBuffer, index, nbLineRead, nbCharRead,
			      DTDname, doc, el, isclosed, lang)

FILE     *infile;
char     *infileBuffer;
int       infileBufferLength;
ThotBool *infileEnd;
ThotBool *infileNotToRead;
char     *infilePreviousBuffer;
char     *htmlBuffer;
int      *index;
int      *nbLineRead;
int      *nbCharRead;
STRING    DTDname;
Document  doc;
Element  *el;
ThotBool *isclosed;
Language  lang;
#endif
{
  ThotBool     endOfParsing = FALSE;
  int          res;
  int          tmpLen = 0;
  int          offset = 0;
  int          i;
  ElementType  elType;
  CHAR_T      *schemaName;
  CHAR_T      *tmpBuffer = NULL;

  if (infile == NULL && htmlBuffer == NULL)
    return TRUE;

  /* general initialization */
  /* If htmlBuffer isn't null, we are parging a html sub-tree */
  /* including XML elements */ 
  rootElement = NULL;
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
  InitializeExpatParser ();

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
  if (!XML_Parse (parser, DECL_DOCTYPE, DECL_DOCTYPE_LEN, 0))
    {
      XmlParseError (XMLcontext.doc,
		     (CHAR_T *) XML_ErrorString (XML_GetErrorCode (parser)), 0);
      XMLabort = TRUE;
    }
  else
    {
      extraLineRead = XML_GetCurrentLineNumber (parser);
      extraOffset = XML_GetCurrentByteIndex (parser);
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
      if (!XML_Parse (parser, tmpBuffer, tmpLen, 1))
	{
	  if (!XMLrootClosed)
	    {
	      XmlParseError (XMLcontext.doc, (CHAR_T *) XML_ErrorString (XML_GetErrorCode (parser)), 0);
	      XMLabort = TRUE;
	    }
	}
    }
  else
    {
      /* read and parse the HTML file sequentialy */
      while (!endOfParsing && !XMLabort)
	{
	  if (*index == 0)
	    {
	      if (*infileNotToRead)
		*infileNotToRead = FALSE;
	      else
		{
		  res = gzread (infile, infileBuffer, infileBufferLength);
		  if (res < infileBufferLength)
		    {
		      *infileEnd = TRUE;
		      endOfParsing = TRUE;
		    }
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
	      tmpLen = strlen (infileBuffer) - *index;
	      tmpBuffer = TtaGetMemory (tmpLen);
	      for (i = 0; i < tmpLen; i++)
		tmpBuffer[i] = infileBuffer[*index + i];	  
	    }
	  if (!XML_Parse (parser, tmpBuffer, tmpLen, *infileEnd))
	    {
	      if (XMLrootClosed)
		endOfParsing = TRUE;
	      else
		{
		  XmlParseError (XMLcontext.doc, (CHAR_T *) XML_ErrorString (XML_GetErrorCode (parser)), 0);
		  XMLabort = TRUE;
		}
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
      if (XML_GetCurrentLineNumber (parser) - extraLineRead <= 0)
	/* We stay on the same line */
	*nbCharRead += XML_GetCurrentColumnNumber (parser);
      else
	{
	  /* We read at least one new line */
	  *nbLineRead = *nbLineRead + XML_GetCurrentLineNumber (parser) - extraLineRead;
	  *nbCharRead = XML_GetCurrentColumnNumber (parser);
	}
    }

  offset = XML_GetCurrentByteIndex (parser) - extraOffset - 1;
  *index += offset;
  *el = XMLcontext.lastElement;
  *isclosed = XMLcontext.lastElementClosed;

  if (tmpBuffer != NULL)   
    TtaFreeMemory (tmpBuffer);   

  /* Free expat parser */ 
  FreeXmlParserContexts ();
  FreeExpatParser ();

  return (!XMLabort);
}

/*---------------------------------------------------------------------------
   XmlParse
   Parses the XML file infile and builds the equivalent Thot abstract tree.
  ---------------------------------------------------------------------------*/
#ifdef __STDC__
static void   XmlParse (FILE     *infile,
			ThotBool *xmlDec,
			ThotBool *xmlDoctype)
#else
static void   XmlParse (infile, xmlDec, xmlDoctype)
FILE      *infile;
ThotBool  *xmlDec;
ThotBool  *xmlDoctype;

#endif
{
#define	 COPY_BUFFER_SIZE	1024
   char         bufferRead[COPY_BUFFER_SIZE];
   char         tmpBuffer[COPY_BUFFER_SIZE];
   char         tmp2Buffer[COPY_BUFFER_SIZE];
   CHAR_T      *ptr;
   int          res;
   int          tmplen;
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
     
   while (!endOfFile && !XMLabort)
     {
       /* read the XML file */
       res = gzread (infile, bufferRead, COPY_BUFFER_SIZE);      
       if (res < COPY_BUFFER_SIZE)
	   endOfFile = TRUE;
       
       if (!*xmlDoctype)
	 /* There is no DOCTYPE Declaration 
	    We include a virtual DOCTYPE declaration so that EXPAT parser
	    doesn't stop processing when it finds an external entity */	  
	 {
	   if (*xmlDec)
	     /* There is a XML declaration */
	     /* We look for first '>' character */
	     {
	       strcpy (tmpBuffer, bufferRead);
	       if ((ptr = ustrchr (tmpBuffer, TEXT('>'))))
		 {
		   ptr++;
		   strcpy (tmp2Buffer, ptr);
		   *ptr = WC_EOS;
		   tmplen = strlen (tmpBuffer);
		   if (!XML_Parse (parser, tmpBuffer, tmplen, FALSE))
		     {
		       XmlParseError (XMLcontext.doc,
				      (CHAR_T *) XML_ErrorString (XML_GetErrorCode (parser)), 0);
		       XMLabort = TRUE;
		     }
		   res = res - tmplen;
		   strcpy (bufferRead, tmp2Buffer);
		 }
	     }

	   /* Virtual DOCTYPE Declaration */
	   tmpLineRead = XML_GetCurrentLineNumber (parser);
	   if (!XML_Parse (parser, DECL_DOCTYPE, DECL_DOCTYPE_LEN, 0))
	     {
	       XmlParseError (XMLcontext.doc,
			      (CHAR_T *) XML_ErrorString (XML_GetErrorCode (parser)), 0);
	       XMLabort = TRUE;
	     }
	   *xmlDoctype = TRUE;
	   extraLineRead = XML_GetCurrentLineNumber (parser) - tmpLineRead;
	 }

       /* Standard EXPAT processing */
       if (!XML_Parse (parser, bufferRead, res, endOfFile))
	 {
	   XmlParseError (XMLcontext.doc, (CHAR_T *) XML_ErrorString (XML_GetErrorCode (parser)), 0);
	   XMLabort = TRUE;
	 }
     }
   
   if (ErrFile)
     {
       fclose (ErrFile);
       ErrFile = NULL;
     } 
}

/*----------------------------------------------------------------------
   StartXmlParser loads the file Directory/xmlFileName for
   displaying the document documentName.
   The parameter pathURL gives the original (local or
   distant) path or URL of the xml document.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void       StartXmlParser (Document doc,
			   CHAR_T*  htmlFileName,
			   CHAR_T*  documentName,
			   CHAR_T*  documentDirectory,
			   CHAR_T*  pathURL,
			   ThotBool xmlDec,
			   ThotBool xmlDoctype)
#else
void       StartXmlParser (doc,
			   htmlFileName,
			   documentName,
			   documentDirectory,
			   pathURL,
			   xmlDec,
			   xmlDoctype)
Document    doc;
CHAR_T*     htmlFileName;
CHAR_T*     documentName;
CHAR_T*     documentDirectory;
CHAR_T*     pathURL;
ThotBool    xmlDec;
ThotBool    xmlDoctype;
#endif

{
  Element         el, oldel;
  AttributeType   attrType;
  Attribute       attr;
  CHAR_T*         s;
  CHAR_T          tempname[MAX_LENGTH];
  CHAR_T          temppath[MAX_LENGTH];
  STRING          profile;
  char            www_file_name[MAX_LENGTH];
  int             length, error;
  ThotBool        isXHTML;

  /* General initialization */
  rootElement = TtaGetMainRoot (doc);
  rootElType = TtaGetElementType (rootElement);     
  InitializeXmlParsingContext (doc, rootElement, FALSE, FALSE);

  /* Specific Initialization */
  XMLcontext.language = TtaGetDefaultLanguage ();
  DocumentSSchema = TtaGetDocumentSSchema (doc);
  
  /* Reading of the file */
  wc2iso_strcpy (www_file_name, htmlFileName);
  stream = gzopen (www_file_name, "r");

  if (stream != 0)
    {
      if (documentName[0] == WC_EOS &&
	  !TtaCheckDirectory (documentDirectory))
	{
	  ustrcpy (documentName, documentDirectory);
	  documentDirectory[0] = WC_EOS;
	  s = TtaGetEnvString ("PWD");
	  /* Set path on current directory */
	  if (s != NULL)
	    ustrcpy (documentDirectory, s);
	  else
	    documentDirectory[0] = WC_EOS;
	}
      TtaAppendDocumentPath (documentDirectory);

      /* Set document URL */
      length = ustrlen (pathURL);
      if (ustrcmp (pathURL, htmlFileName) == 0)
	{
	  docURL = TtaAllocString (length + 1);
	  ustrcpy (docURL, pathURL);
	}
      else
	{
	  length += ustrlen (htmlFileName) + 20;
	  docURL = TtaAllocString (length+1);
	  usprintf (docURL, TEXT("%s temp file: %s"), pathURL, htmlFileName);
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
      isXHTML = (ustrcmp (TtaGetSSchemaName (DocumentSSchema),
			  TEXT("HTML")) == 0);	
      if (isXHTML)
	{
	  strcpy (XMLrootName, (TEXT("html")));
	  /* Add the default attribute PrintURL */
	  attrType.AttrSSchema = DocumentSSchema;
	  attrType.AttrTypeNum = HTML_ATTR_PrintURL;
	  attr = TtaGetAttribute (rootElement, attrType);
	  if (!attr)
	    {
	      attr = TtaNewAttribute (attrType);
	      TtaAttachAttribute (rootElement, attr, doc);
	    }
	}

      LoadUserStyleSheet (doc);

      TtaSetDisplayMode (doc, NoComputedDisplay);

      /* Delete all element except the root element */
      el = TtaGetFirstChild (rootElement);
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
      if (ustrcmp (TtaGetSSchemaName (DocumentSSchema), TEXT("GraphML")) == 0)
	ChangeXmlParserContextDTD (TEXT("GraphML"));
      else if (ustrcmp (TtaGetSSchemaName (DocumentSSchema), TEXT("MathML")) == 0)
	ChangeXmlParserContextDTD (TEXT("MathML"));
      else
	ChangeXmlParserContextDTD (TEXT("HTML"));
      
      /* Initialize the error file */
      ErrFile = (FILE*) 0;
      ErrFileName[0] = WC_EOS;
      
      /* Specific initialization for expat */
      InitializeExpatParser ();
	
      /* Parse the input file and build the Thot document */
      XmlParse (stream, &xmlDec, &xmlDoctype);
      
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

   /* Display a warning if an error was found */
   if (XMLabort)
     {
       profile = TtaGetEnvString ("Profile");
       if (!profile)
	 profile = TEXT("");
       InitConfirm3L (XMLcontext.doc, 1, TtaGetMessage (AMAYA, AM_XML_PROFILE),
		      profile, TtaGetMessage (AMAYA, AM_XML_ERROR), FALSE);
       HTMLErrorsFound = TRUE;
     }
   else if (HTMLErrorsFound)
     {
       profile = TtaGetEnvString ("Profile");
       if (!profile)
	 profile = TEXT("");
       InitConfirm3L (XMLcontext.doc, 1, TtaGetMessage (AMAYA, AM_XML_PROFILE),
		      profile, TtaGetMessage (AMAYA, AM_XML_WARNING), FALSE);
     }
}

/* end of module */
