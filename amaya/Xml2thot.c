/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * html2thot parses a HTML file and builds the corresponding abstract tree
 * for a Thot document of type HTML.
 *
 * Author: V. Quint
 *         L. Carcone 
 *         R. Guetari (W3C/INRIA): Unicode version 
 */

#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"
#include "parser.h"
#include "zlib.h"
#include "css_f.h"
#include "HTMLactions_f.h"
#include "HTMLedit_f.h"
#include "HTMLimage_f.h"
#include "HTMLtable_f.h"
#include "HTMLimage_f.h"
#include "UIcss_f.h"

#include "fetchHTMLname.h"
#include "XHTMLbuilder_f.h"
#include "MathMLbuilder_f.h"
#ifdef GRAPHML
#include "GraphMLbuilder_f.h"
#endif
#include "XmlNameSpaces_f.h"
#include "html2thot_f.h"
#include "styleparser_f.h"

#ifdef EXPAT
#include "xmlparse.h"
#define NS_SEP ':'
#endif /* EXPAT */

/* maximum length of a Thot structure schema name */
#define MAX_SS_NAME_LENGTH 32

typedef struct _ElemToBeChecked *PtrElemToBeChecked;
typedef struct _ElemToBeChecked
  {
     Element               Elem;	/* the element to be checked */
     PtrElemToBeChecked    nextElemToBeChecked;
  }
ElemToBeChecked;


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
#ifdef GRAPHML
   HTML_EL_XMLGraphics,
#endif
   0};

/* empty elements */
static int          EmptyElement[] =
{
   HTML_EL_AREA,
   HTML_EL_BASE,
   HTML_EL_BaseFont,
   HTML_EL_BR,
   HTML_EL_COL,
   HTML_EL_FRAME,
   HTML_EL_Horizontal_Rule,
   HTML_EL_Input,
   HTML_EL_ISINDEX,
   HTML_EL_LINK,
   HTML_EL_META,
   HTML_EL_Parameter,
   HTML_EL_PICTURE_UNIT,
   0};

/* block level elements */
static int          BlockLevelElement[] =
{
   HTML_EL_Paragraph, HTML_EL_Pseudo_paragraph,
   HTML_EL_H1, HTML_EL_H2, HTML_EL_H3, HTML_EL_H4, HTML_EL_H5, HTML_EL_H6,
   HTML_EL_TITLE, HTML_EL_Term, HTML_EL_CAPTION, HTML_EL_LEGEND,
   0};

/* start tags that imply the end of a current element */
/* any tag of each line implies the end of the current element if the type of
   that element is in the same line */
typedef char        oneLine[100];
static oneLine      EquivEndingElem[] =
{
   "dt dd li option",
   "h1 h2 h3 h4 h5 h6",
   "address pre listing xmp",
   ""
};
/* acording the HTML DTD, HR should be added to the 2nd line above, as it */
/* is not allowed within a H1, H2, H3, etc. But we should tolerate that case */
/* because many documents contain rules in headings... */

/* start tags that imply the end of current element */
static oneLine      StartTagEndingElem[] =
{
   "form closes form p p* hr h1 h2 h3 h4 h5 h6 dl ul ol menu dir address pre listing xmp head",
   "head closes p p*",
   "title closes p p*",
   "body closes head style script title p p*",
   "li closes p p* h1 h2 h3 h4 h5 h6 dl address pre listing xmp head",
   "hr closes p p* head",
   "h1 closes p p* head",
   "h2 closes p p* head",
   "h3 closes p p* head",
   "h4 closes p p* head",
   "h5 closes p p* head",
   "h6 closes p p* head",
   "dir closes p p* head",
   "address closes p p* head ul",
   "pre closes p p* head ul",
   "listing closes p p* head",
   "xmp closes p p* head",
   "blockquote closes p p* head",
   "dl closes p p* dt menu dir address pre listing xmp head",
   "dt closes p p* menu dir address pre listing xmp head",
   "dd closes p p* menu dir address pre listing xmp head",
   "ul closes p p* head ol menu dir address pre listing xmp",
   "ol closes p p* head ul",
   "menu closes p p* head ul",
   "p closes p p* head h1 h2 h3 h4 h5 h6",
   "p* closes p p* head",
   "div closes p p* head",
   "noscript closes p p* head",
   "center closes font b i p p* head",
   "a closes a",
   "caption closes p p*",
   "colgroup closes caption colgroup col p p*",
   "col closes caption col p p*",
   "table closes p p* head h1 h2 h3 h4 h5 h6 pre listing xmp a",
   "th closes th td",
   "td closes th td",
   "tr closes th td tr caption col colgroup",
   "thead closes caption col colgroup",
   "tfoot closes th td tr caption col colgroup thead tbody",
   "tbody closes th td tr caption col colgroup thead tfoot tbody",
   "optgroup closes option",
   "fieldset closes legend p p* head h1 h2 h3 h4 h5 h6 pre listing xmp a",
   ""
};


/* ---------------------- static variables ---------------------- */

#ifdef EXPAT
/* Expat perser identifier */
static XML_Parser  parser;
#endif /* EXPAT */

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
    Proc	   ElementComplete;	/* action to be called when an element
					   has been generated completely */
    Proc	   AttributeComplete;	/* action to be called when an
					   attribute has been generated */
    Proc	   GetDTDName;		/* returns the name of the DTD to be
					   used for parsing the contents of an
					   element that uses a different DTD */
  }
XMLparserContext;

/* All parser contexts describing known XML DTDs constitute a chain */
                /* first context in the chain*/
static PtrParserCtxt	firstParserCtxt = NULL;
                /* current context */
static PtrParserCtxt	currentParserCtxt = NULL;

#define MAX_URI_NAME_LENGTH  60
#define XHTML_URI            TEXT("http://www.w3.org/1999/xhtml")
#define MathML_URI           TEXT("http://www.w3.org/1998/Math/MathML")
#define GraphML_URI          TEXT("Unknown URI")

/* SSchema stack */
#define MAX_NS   10
typedef CHAR_T   NS_SchemaName[MAX_SS_NAME_LENGTH];
                  /* Thot Schema name */
static NS_SchemaName  NS_SchemaStack [MAX_NS]; 
static int            NS_Level = 0;


/* parser stack */
	        /* maximum stack height */
#define MAX_STACK_HEIGHT   200
                  /* XML element name */
static USTRING       nameElementStack[MAX_STACK_HEIGHT]; 
                  /* element in the Thot abstract tree */
static Element       elementStack[MAX_STACK_HEIGHT];
                  /* element language */
static Language	     languageStack[MAX_STACK_HEIGHT];
static PtrParserCtxt parserCtxtStack[MAX_STACK_HEIGHT];
                  /* first free element on the stack */
static int           stackLevel = 0;


static gzFile      stream = 0;
                /* number of lines read in the file */
static int         NumberOfLinesRead = 0;
                /* number of characters read in the current line */
static int         NumberOfCharRead = 0;
                /* <PRE> has just been read */
static ThotBool    AfterTagPRE = FALSE;
                /* reading the content of a STYLE element */
static ThotBool    ParsingCSS = FALSE;
                /* reading the content of a text area element */
static ThotBool    ParsingTextArea = FALSE;
                /* <TABLE> has been read */
static int         WithinTable = 0;
                /* path or URL of the document */
static CHAR_T*     docURL = NULL;



/* line number in the source file of the beginning
   of the text contained in the buffer */
static int	   BufferLineNumber = 0;


/* information about the Thot document under construction */
                /* the HTML structure schema */
static SSchema     DocumentSSchema = NULL;
                /* root element of the document */
static Element     rootElement;
                /* last element created */
static Element     lastElement = NULL;
                /* last element is complete */
static ThotBool    lastElementClosed = FALSE;
                /* index in the GIMappingTable of the
		   element being created */
static int         lastElemEntry = 0;
                /* the last start tag encountered is invalid */
static ThotBool    UnknownTag = FALSE;

                /* last attribute created */
static Attribute   lastAttribute = NULL;
                /* element with which the last
		   attribute has been associated */
static Attribute   lastAttrElement = NULL;
                /* entry in the AttributeMappingTable
		   of the attribute being created */
static AttributeMapping* lastMappedAttr = NULL;
                /* the last attribute encountered is invalid */
static ThotBool    UnknownAttr = FALSE;
static ThotBool    ReadingAnAttrValue = FALSE;

                /* TEXT element of the current Comment element */
static Element     CommentText = NULL;
                /* character data should be catenated
		   with the last Text element */
static ThotBool    MergeText = FALSE;
static PtrElemToBeChecked FirstElemToBeChecked = NULL;
static PtrElemToBeChecked LastElemToBeChecked = NULL;


/* information about the Thot document under construction */
static Document     currentDocument = 0;         /* the Thot document */
static Language     currentLanguage;             /* language used in the document */
static Element	    currentElement = NULL;
static ThotBool	    currentElementClosed = FALSE;
static CHAR_T	    currentElementContent = ' ';
static Attribute    currentAttribute = NULL;
static ThotBool	    HTMLStyleAttribute = FALSE;
static ThotBool	    XMLrootClosed = FALSE;
static STRING	    XMLrootClosingTag = NULL;
static int	    XMLrootLevel = 0;
static ThotBool	    lastTagRead = FALSE;
static ThotBool     XMLabort = FALSE;

static CHAR_T	    currentMappedName[16];

/* information about an entity being read */
                /* maximum size entity */
#define MaxEntityLength 80

                /* maximum size of error messages */
#define MaxMsgLength 200


#ifdef __STDC__
static void         ProcessStartGI (CHAR_T* GIname);
static ThotBool     InsertElement (Element * el);
#else
static void         ProcessStartGI ();
static ThotBool     InsertElement ();
#endif

static FILE*    ErrFile = (FILE*) 0;
static CHAR_T   ErrFileName [80];

extern CHARSET  CharEncoding;
extern ThotBool charset_undefined;


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

   /* initialize XHTML parser */
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
   ctxt->MapAttribute = (Proc) XhtmlMapAttribute;;
   ctxt->MapAttributeValue = (Proc) XhtmlMapAttributeValue;
   ctxt->MapEntity = (Proc) XhtmlMapEntity;
   ctxt->EntityCreated = (Proc) XhtmlEntityCreated;
   ctxt->ElementComplete = NULL;
   ctxt->AttributeComplete = NULL;
   ctxt->GetDTDName = NULL;
   prevCtxt = ctxt;

   /* initialize MathML parser */
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
   ctxt->EntityCreated = (Proc) MathMLEntityCreated;
   ctxt->ElementComplete = (Proc) MathMLElementComplete;
   ctxt->AttributeComplete = (Proc) MathMLAttributeComplete;
   ctxt->GetDTDName = (Proc) MathMLGetDTDName;
   prevCtxt = ctxt;

#ifdef GRAPHML
   /* initialize GraphML parser */
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
   ctxt->ElementComplete = (Proc) GraphMLElementComplete;
   ctxt->AttributeComplete = (Proc) GraphMLAttributeComplete;
   ctxt->GetDTDName = (Proc) GraphMLGetDTDName;
   prevCtxt = ctxt;
#endif /* GRAPHML */
}

/*----------------------------------------------------------------------
   ChangeXmlParserContext
   Get the parser context correponding to a given DTD
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ChangeXmlParserContext (STRING DTDname)
#else
static void         ChangeXmlParserContext (DTDname)
STRING              DTDname;
 
#endif
{
  currentParserCtxt = firstParserCtxt;
  while (currentParserCtxt != NULL &&
	 ustrcmp (DTDname, currentParserCtxt->SSchemaName))
    currentParserCtxt = currentParserCtxt->NextParserCtxt;

  /* initialize the corresponding entry */
  if (currentParserCtxt != NULL)
        currentParserCtxt->XMLSSchema = 
          GetXmlDTD (currentParserCtxt->XMLtype, currentDocument);
}


/*----------------------------------------------------------------------
   SearchXmlParserContextUri
   Get the Thot schema name correponding to a given uri
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SearchXmlParserContextUri (STRING uriName,
					       STRING SSchemaName)
#else
static void         SearchXmlParserContextUri (uriName,
					       SSchemaName)
STRING      uriName;
STRING      SSchemaName;

#endif
{
  SSchemaName[0] = EOS;

  currentParserCtxt = firstParserCtxt;
  while (currentParserCtxt != NULL &&
	 ustrcmp (uriName, currentParserCtxt->UriName))
    currentParserCtxt = currentParserCtxt->NextParserCtxt;

  /* Return the corresponding Thot schema */
  if (currentParserCtxt != NULL)
      ustrcpy (SSchemaName, currentParserCtxt->SSchemaName);
}


/*----------------------------------------------------------------------
   GetXmlElType
   Search in the mapping tables the entry for the element type of
   name Xmlname and returns the corresponding Thot element type.
   Returns -1 and schema = NULL if not found.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void       GetXmlElType (STRING XMLname,
				ElementType *elType,
				STRING *mappedName,
				CHAR_T *content,
				Document doc)
#else
static void       GetXmlElType (XMLname,
				elType,
				mappedName,
				content,
				doc)
STRING           XMLname;
ElementType     *elType;
STRING          *mappedName;
CHAR_T          *content;
Document         doc;
#endif
{

  PtrParserCtxt	ctxt;

  /* initialize all parser contexts if not done yet */
  if (firstParserCtxt == NULL)
    InitXmlParserContexts ();

  /* Look at the current context if there is one */
  if (currentParserCtxt != NULL)
    {
      /* by default we're looking at in the current schema */
      elType->ElSSchema = currentParserCtxt->XMLSSchema;
      MapXmlElType (currentParserCtxt->XMLtype, XMLname,
		    elType, mappedName, content, doc);
    }
  else
    if (elType->ElSSchema != NULL)
      {
	/* The schema is known -> search the corresponding context */
	ctxt = firstParserCtxt;
	while (ctxt != NULL &&
	       ustrcmp (TtaGetSSchemaName (elType->ElSSchema), ctxt->SSchemaName))
	  ctxt = ctxt->NextParserCtxt;
	/* get the Thot element number */
	if (ctxt != NULL)
	  MapXmlElType (ctxt->XMLtype, XMLname,
			elType, mappedName, content, doc);
      }
  
  /* if not found, look at other contexts */
  if (elType->ElTypeNum == 0)
    {
      ctxt = firstParserCtxt;
      while (ctxt != NULL && elType->ElSSchema == NULL)
	{
	  elType->ElSSchema = NULL;
	  if (ctxt != currentParserCtxt)
	    {
	      MapXmlElType (ctxt->XMLtype, XMLname,
			    elType, mappedName, content, doc);
	      if (elType->ElSSchema != NULL)
		  ctxt->XMLSSchema = elType->ElSSchema;
	    }
	  ctxt = ctxt->NextParserCtxt;
	}
    }
}

/*----------------------------------------------------------------------
  ParseCharset: Parses the element HTTP-EQUIV and looks for the charset 
  value.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void ParseCharset (Element el) 
#else  /* !__STDC__ */
static void ParseCharset (el) 
Element el;
#endif /* !__STDC__ */
{
 
   int length;
   CHAR_T *text, *text2, *ptrText, *str;
   CHAR_T  charsetname[MAX_LENGTH];
   int     pos, index = 0;
   AttributeType attrType;
   Attribute attr;
   Element root;

   if (!charset_undefined)
 	  return;

  attrType.AttrSSchema = DocumentSSchema;
  attrType.AttrTypeNum = HTML_ATTR_http_equiv;
  attr = TtaGetAttribute (el, attrType);
  if (attr != NULL) {
     /* There is a HTTP-EQUIV attribute */
     length = TtaGetTextAttributeLength (attr);
     if (length > 0) {
        text = TtaAllocString (length + 1);
        TtaGiveTextAttributeValue (attr, text, &length);
        if (!ustrcasecmp (text, TEXT("content-type"))) {
           attrType.AttrTypeNum = HTML_ATTR_meta_content;
           attr = TtaGetAttribute (el, attrType);
           if (attr != NULL) {
              length = TtaGetTextAttributeLength (attr);
              if (length > 0) {
                 text2 = TtaAllocString (length + 1);
                 TtaGiveTextAttributeValue (attr, text2, &length);
                 ptrText = text2;
                 while (*ptrText) {
                       *ptrText = utolower (*ptrText);
                       ptrText++;
				 }

                 str = ustrstr (text2, TEXT("charset="));
    
                 if (str) {
 			       pos = str - text2 + 8;

                   while (text2[pos] != WC_SPACE &&
			  text2[pos] != WC_TAB && text2[pos] != WC_EOS)
                         charsetname[index++] = text2[pos++];
                   charsetname[index] = WC_EOS;
                   CharEncoding = TtaGetCharset (charsetname);

                   if (CharEncoding == UNDEFINED_CHARSET)
                      CharEncoding = UTF_8;
                   else {
                        /* copy the charset to the document's metadata info */
                        root = TtaGetMainRoot (currentDocument);
                        attrType.AttrTypeNum = HTML_ATTR_Charset;
                        attr = TtaGetAttribute (root, attrType);
                        if (!attr)
                           /* the root element does not have a Charset attribute.
                           Create one */
						{
                          attr = TtaNewAttribute (attrType);
                          TtaAttachAttribute (root, attr, currentDocument);
						}
                        TtaSetAttributeText (attr, charsetname,
					     root, currentDocument);
				   }
                   charset_undefined = FALSE;
				 }
                 TtaFreeMemory (text2);
			  }       
		   } 
		}
        TtaFreeMemory (text);
	 }
  }
}


/*----------------------------------------------------------------------
   copyCEstring
   Create a copy of the string of elements pointed by first and 
   return a pointer on the first element of the copy.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PtrClosedElement copyCEstring (PtrClosedElement first)
#else
static PtrClosedElement copyCEstring (first)
PtrClosedElement    first;

#endif
{
   PtrClosedElement    ret, cur, next, prev;

   ret = NULL;
   cur = first;
   prev = NULL;
   while (cur != NULL)
     {
	next = (PtrClosedElement) TtaGetMemory (sizeof (ClosedElement));
	next->nextClosedElem = NULL;
	next->tagNum = cur->tagNum;
	if (ret == NULL)
	   ret = next;
	else
	   prev->nextClosedElem = next;
	prev = next;
	cur = cur->nextClosedElem;
     }
   return ret;
}


/*----------------------------------------------------------------------
   Within  
   Checks if an element of type ThotType is in the stack.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     Within (int ThotType,
			    SSchema ThotSSchema)
#else
static ThotBool     Within (ThotType,
			    ThotSSchema)
int                 ThotType;
SSchema		    ThotSSchema;

#endif
{
   ThotBool            ret;
   int                 i;
   ElementType         elType;

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
   InsertSibling   return TRUE if the new element must be inserted
   in the Thot document as a sibling of lastElement;
   return FALSE it it must be inserted as a child.
  ----------------------------------------------------------------------*/
static ThotBool     InsertSibling ()
{
   if (stackLevel == 0)
       return FALSE;
   else
     if (lastElementClosed ||
	 TtaIsLeaf (TtaGetElementType (lastElement)) ||
	 (nameElementStack[stackLevel - 1] != NULL &&
	  currentElementContent == 'E'))
       return TRUE;
     else
       return FALSE;

}

/*----------------------------------------------------------------------
   IsEmptyElement 
   Return TRUE if element el is defined as an empty element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool        IsEmptyElement (Element el)
#else
static ThotBool        IsEmptyElement (el)
Element             el;

#endif
{
   ElementType         elType;
   int                 i;
   ThotBool            ret;

   ret = FALSE;
   elType = TtaGetElementType (el);
   i = 0;
   while (EmptyElement[i] > 0 && EmptyElement[i] != elType.ElTypeNum)
      i++;
   if (EmptyElement[i] == elType.ElTypeNum)
      ret = TRUE;
   return ret;
}

/*----------------------------------------------------------------------
   IsBlockElement  
   Return TRUE if element el is a block element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     IsBlockElement (Element el)
#else
static ThotBool     IsBlockElement (el)
Element             el;

#endif
{
   ElementType         elType;
   int                 i;
   ThotBool            ret;

   ret = FALSE;
   elType = TtaGetElementType (el);
   i = 0;
   while (BlockLevelElement[i] > 0 &&
	  BlockLevelElement[i] != elType.ElTypeNum)
      i++;
   if (BlockLevelElement[i] == elType.ElTypeNum)
      ret = TRUE;
   return ret;
}

/*----------------------------------------------------------------------
   CannotContainText 
   Return TRUE if element el is a block element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     CannotContainText (ElementType elType)
#else
static ThotBool     CannotContainText (elType)
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
   BlockInCharLevelElem
   Element el is a block-level element. If its parent is a character-level
   element, add a record in the list of block-level elements to be
   checked when the document is complete.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void BlockInCharLevelElem (Element el)
#else
static void BlockInCharLevelElem (el)
Element             el;

#endif
{
   PtrElemToBeChecked  elTBC;
   Element             parent;

   if (LastElemToBeChecked != NULL)
      if (LastElemToBeChecked->Elem == el)
	 /* this element is already in the queue */
	 return;

   parent = TtaGetParent (el);
   if (parent != NULL)
     if (IsCharacterLevelElement (parent))
	{
	elTBC = (PtrElemToBeChecked) TtaGetMemory(sizeof(ElemToBeChecked));
	elTBC->Elem = el;
	elTBC->nextElemToBeChecked = NULL;
	if (LastElemToBeChecked == NULL)
	   FirstElemToBeChecked = elTBC;
	else
	   LastElemToBeChecked->nextElemToBeChecked = elTBC;
	LastElemToBeChecked = elTBC;
	}
}

/*----------------------------------------------------------------------
   CheckSurrounding
   Inserts an element Pseudo_paragraph in the abstract tree of the Thot
   document if el is a leaf and is not allowed to be a child of element parent.
   Return TRUE if element *el has been inserted in the tree.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     CheckSurrounding (Element *el,
				      Element  parent)
#else
static ThotBool     CheckSurrounding (el,
				      parent)
Element          *el;
Element           parent;

#endif
{
   ElementType         parentType, newElType, elType;
   Element             newEl, ancestor, prev, prevprev;
   ThotBool	       ret;

   if (parent == NULL)
      return(FALSE);
   ret = FALSE;
   elType = TtaGetElementType (*el);
   if (elType.ElTypeNum == HTML_EL_TEXT_UNIT || elType.ElTypeNum == HTML_EL_BR
       || elType.ElTypeNum == HTML_EL_PICTURE_UNIT
       || elType.ElTypeNum == HTML_EL_Input
       || elType.ElTypeNum == HTML_EL_Text_Area)
     {
	/* the element to be inserted is a character string */
	/* Search the ancestor that is not a character level element */
	ancestor = parent;
	while (ancestor != NULL && IsCharacterLevelElement (ancestor))
	   ancestor = TtaGetParent (ancestor);
	if (ancestor != NULL)
	  {
	   elType = TtaGetElementType (ancestor);
	   if (CannotContainText (elType) &&
	       !Within (HTML_EL_Option_Menu, DocumentSSchema))
	      /* Element ancestor cannot contain text directly. Create a */
	      /* Pseudo_paragraph element as the parent of the text element */
	      {
	      newElType.ElSSchema = DocumentSSchema;
	      newElType.ElTypeNum = HTML_EL_Pseudo_paragraph;
	      newEl = TtaNewElement (currentDocument, newElType);
	      TtaSetElementLineNumber (newEl, NumberOfLinesRead);
	      /* insert the new Pseudo_paragraph element */
	      InsertElement (&newEl);
	      if (newEl != NULL)
	        {
	          /* insert the Text element in the tree */
	          TtaInsertFirstChild (el, newEl, currentDocument);
	          BlockInCharLevelElem (newEl);
		  ret = TRUE;

		  /* if previous siblings of the new Pseudo_paragraph element
		     are character level elements, move them within the new
		     Pseudo_paragraph element */
		  prev = newEl;
		  TtaPreviousSibling (&prev);
		  while (prev != NULL)
		     {
		     if (!IsCharacterLevelElement (prev))
			prev = NULL;
		     else
			{
			prevprev = prev;  TtaPreviousSibling (&prevprev);
			TtaRemoveTree (prev, currentDocument);
			TtaInsertFirstChild (&prev, newEl, currentDocument);
			prev = prevprev;
			}
		     }
	        }
     	      }
	  }
     }

   if (elType.ElTypeNum == HTML_EL_TEXT_UNIT ||
       (elType.ElTypeNum != HTML_EL_Inserted_Text &&
	IsCharacterLevelElement (*el)))
      /* it is a character level element */
     {
	parentType = TtaGetElementType (parent);
	if (parentType.ElTypeNum == HTML_EL_Text_Area)
	   /* A basic element cannot be a child of a Text_Area */
	   /* create a Inserted_Text element as a child of Text_Area */
	  {
	     newElType.ElSSchema = DocumentSSchema;
	     newElType.ElTypeNum = HTML_EL_Inserted_Text;
	     newEl = TtaNewElement (currentDocument, newElType);
	     TtaSetElementLineNumber (newEl, NumberOfLinesRead);
	     InsertElement (&newEl);
	     if (newEl != NULL)
	       {
	         TtaInsertFirstChild (el, newEl, currentDocument);
		 ret = TRUE;
	       }
	  }
     }
  return ret;
}


/*----------------------------------------------------------------------
   InsertElement   
   Inserts element el in the abstract tree of the Thot document,
   at the current position.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     InsertElement (Element * el)
#else
static ThotBool     InsertElement (el)
Element            *el;

#endif
{
   ThotBool            ret;
   Element             parent;

   if (InsertSibling ())
     {
	if (lastElement == NULL)
	   parent = NULL;
	else
	   parent = TtaGetParent (lastElement);
	if (!CheckSurrounding (el, parent))
	  if (parent != NULL)
	    TtaInsertSibling (*el, lastElement, FALSE, currentDocument);
	  else
	    {
	       TtaDeleteTree (*el, currentDocument);
	       *el = NULL;
	    }
	ret = TRUE;
     }
   else
     {
	if (!CheckSurrounding (el, lastElement))
	  TtaInsertFirstChild (el, lastElement, currentDocument);
	ret = FALSE;
     }
   if (*el != NULL)
     {
	lastElement = *el;
	lastElementClosed = FALSE;
     }
   return ret;
}

/*----------------------------------------------------------------------
   ProcessOptionElement
   If multiple is FALSE, remove the SELECTED attribute from the
   option element, except if it's element el.
   If parsing is TRUE, associate a DefaultSelected attribute with
   element option if it has a SELECTED attribute.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void        ProcessOptionElement (Element option,
					 Element el,
					 Document doc,
					 ThotBool multiple,
					 ThotBool parsing)
#else  /* __STDC__ */
static void        ProcessOptionElement (option,
					 el,
					 doc,
					 multiple,
					 parsing)
Element             option;
Element		    el;
Document            doc;
ThotBool	    multiple;
ThotBool            parsing;

#endif /* __STDC__ */
{
   ElementType		elType;
   AttributeType	attrType;
   Attribute		attr;

   elType = TtaGetElementType (option);
   attrType.AttrSSchema = elType.ElSSchema;
   attrType.AttrTypeNum = HTML_ATTR_Selected;
   if (!multiple && option != el)
      {
      /* Search the SELECTED attribute */
      attr = TtaGetAttribute (option, attrType);
      /* remove it if it exists */
      if (attr != NULL)
	 TtaRemoveAttribute (option, attr, doc);
      }
   if (parsing)
      {
      attr = TtaGetAttribute (option, attrType);
      if (attr != NULL)
	 {
	 attrType.AttrTypeNum = HTML_ATTR_DefaultSelected;
	 attr = TtaGetAttribute (option, attrType);
	 if (!attr)
	    {
	    /* create the DefaultSelected attribute */
	    attr = TtaNewAttribute (attrType);
	    TtaAttachAttribute (option, attr, doc);
	    TtaSetAttributeValue (attr, HTML_ATTR_DefaultSelected_VAL_Yes_,
				  option, doc);
	    }
	 }
      }
}


/*----------------------------------------------------------------------
   LastLeafInElement
   return the last leaf element in element el.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static Element      LastLeafInElement (Element el)
#else
static Element      LastLeafInElement (el)
Element             el;

#endif
{
   Element             child, lastLeaf;

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
   ElementComplete
   Element el is complete. Check its attributes and its contents.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ElementComplete (Element el)
#else
static void         ElementComplete (el)
Element             el;
#endif
{
   ElementType         elType, newElType, childType;
   Element             constElem, child, desc, leaf, prev, next, last,
		       elFrames, lastFrame, lastChild;
   Attribute           attr;
   AttributeType       attrType;
   Language            lang;
   STRING              text;
   CHAR_T              lastChar[2];
   STRING              name1;
   int                 length;

   elType = TtaGetElementType (el);
   /* is this a block-level element in a character-level element? */
   if (!IsCharacterLevelElement (el) && elType.ElTypeNum != HTML_EL_Comment_)
      BlockInCharLevelElem (el);

   newElType.ElSSchema = elType.ElSSchema;
   switch (elType.ElTypeNum)
    {
    case HTML_EL_Object:	/*  it's an object */
       /* create Object_Content */
       child = TtaGetFirstChild (el);
       if (child != NULL)
	 elType = TtaGetElementType (child);
	 
       /* is it the PICTURE element ? */
       if (child == NULL || elType.ElTypeNum != HTML_EL_PICTURE_UNIT)
	 {
	   desc = child;
	   /* create the PICTURE element */
	   elType.ElTypeNum = HTML_EL_PICTURE_UNIT;
	   child = TtaNewTree (currentDocument, elType, "");
	   if (desc == NULL)
	     TtaInsertFirstChild (&child, el, currentDocument);
	   else
	     TtaInsertSibling (child, desc, TRUE, currentDocument);
	 }
       /* copy attribute data into SRC attribute of Object_Image */
       attrType.AttrSSchema = DocumentSSchema;
       attrType.AttrTypeNum = HTML_ATTR_data;
       attr = TtaGetAttribute (el, attrType);
       if (attr != NULL)
	 {
	    length = TtaGetTextAttributeLength (attr);
	    if (length > 0)
	      {
		name1 = TtaAllocString (length + 1);
		TtaGiveTextAttributeValue (attr, name1, &length);
		attrType.AttrTypeNum = HTML_ATTR_SRC;
		attr = TtaGetAttribute (child, attrType);
		if (attr == NULL)
		  {
		    attr = TtaNewAttribute (attrType);
		    TtaAttachAttribute (child, attr, currentDocument);
		  }
		TtaSetAttributeText (attr, name1, child, currentDocument);
		TtaFreeMemory (name1);
	      }
	 }
       /* is the Object_Content element already created ? */
       desc = child;
       TtaNextSibling(&desc);
       if (desc != NULL)
	 elType = TtaGetElementType (desc);
	 
       /* is it the Object_Content element ? */
       if (desc == NULL || elType.ElTypeNum != HTML_EL_Object_Content)
	 {
	   /* create Object_Content */
	   elType.ElTypeNum = HTML_EL_Object_Content;
	   desc = TtaNewTree (currentDocument, elType, "");
	   TtaInsertSibling (desc, child, FALSE, currentDocument);
	   /* move previous existing children into Object_Content */
	   child = TtaGetLastChild(el);
	   while (child != desc)
	     {
	       TtaRemoveTree (child, currentDocument);
	       TtaInsertFirstChild (&child, desc, currentDocument);
	       child = TtaGetLastChild(el);
	     }
	 }
	break;

    case HTML_EL_Unnumbered_List:
    case HTML_EL_Numbered_List:
    case HTML_EL_Menu:
    case HTML_EL_Directory:
	/* It's a List element. It should only have List_Item children.
	   If it has List element chidren, move these List elements
	   within their previous List_Item sibling.  This is to fix
           a bug in document generated by Mozilla. */
	prev = NULL;
	next = NULL;
	child = TtaGetFirstChild (el);
	while (child != NULL)
	   {
	   next = child;
	   TtaNextSibling (&next);
	   elType = TtaGetElementType (child);
	   if (elType.ElTypeNum == HTML_EL_Unnumbered_List ||
	       elType.ElTypeNum == HTML_EL_Numbered_List ||
	       elType.ElTypeNum == HTML_EL_Menu ||
	       elType.ElTypeNum == HTML_EL_Directory)
	     /* this list element is a child of another list element */
	     if (prev)
		{
		elType = TtaGetElementType (prev);
		if (elType.ElTypeNum == HTML_EL_List_Item)
		   {
		   /* get the last child of the previous List_Item */
		   desc = TtaGetFirstChild (prev);
		   last = NULL;
		   while (desc)
		      {
		      last = desc;
		      TtaNextSibling (&desc);
		      }
		   /* move the list element after the last child of the
		      previous List_Item */
		   TtaRemoveTree (child, currentDocument);
		   if (last)
		      TtaInsertSibling (child, last, FALSE, currentDocument);
		   else
		      TtaInsertFirstChild (&child, prev, currentDocument);
		   child = prev;
		   }
	        }
	   prev = child;
	   child = next;
	   }
	break;

    case HTML_EL_FRAMESET:
	/* The FRAMESET element is now complete.  Gather all its FRAMESET
	   and FRAME children and wrap them up in a Frames element */
	elFrames = NULL; lastFrame = NULL;
	lastChild = NULL;
	child = TtaGetFirstChild (el);
	while (child != NULL)
	   {
	   next = child;
	   TtaNextSibling (&next);
	   elType = TtaGetElementType (child);
	   if (elType.ElTypeNum == HTML_EL_FRAMESET ||
	       elType.ElTypeNum == HTML_EL_FRAME ||
	       elType.ElTypeNum == HTML_EL_Comment_)
	       {
	       /* create the Frames element if it does not exist */
	       if (elFrames == NULL)
		 {
		    newElType.ElSSchema = DocumentSSchema;
		    newElType.ElTypeNum = HTML_EL_Frames;
		    elFrames = TtaNewElement (currentDocument, newElType);
		    TtaSetElementLineNumber (elFrames, NumberOfLinesRead);
		    TtaInsertSibling (elFrames, child, TRUE, currentDocument);
		 }
	       /* move the element as the last child of the Frames element */
	       TtaRemoveTree (child, currentDocument);
	       if (lastFrame == NULL)
		  TtaInsertFirstChild (&child, elFrames, currentDocument);
	       else
		  TtaInsertSibling (child, lastFrame, FALSE, currentDocument);
	       lastFrame = child;
	       }
	   child = next;
           }
	break;

    case HTML_EL_Input:	/* it's an INPUT without any TYPE attribute */
	/* Create a child of type Text_Input */
	elType.ElTypeNum = HTML_EL_Text_Input;
	child = TtaNewTree (currentDocument, elType, "");
	TtaSetElementLineNumber (child, NumberOfLinesRead);
	TtaInsertFirstChild (&child, el, currentDocument);
	/* now, process it like a Text_Input element */
    case HTML_EL_Text_Input:
    case HTML_EL_Password_Input:
    case HTML_EL_File_Input:
      /* get element Inserted_Text */
      child = TtaGetFirstChild (el);
      if (child != NULL)
	{
	  attrType.AttrSSchema = DocumentSSchema;
	  attrType.AttrTypeNum = HTML_ATTR_Value_;
	  attr = TtaGetAttribute (el, attrType);
	  if (attr != NULL)
	    {
	      /* copy the value of attribute "value" into the first text
		 leaf of element */
	      length = TtaGetTextAttributeLength (attr);
	      if (length > 0)
		{
		  /* get the text leaf */
		  leaf = TtaGetFirstChild (child);
		  if (leaf != NULL)
		    {
		      childType = TtaGetElementType (leaf);
		      if (childType.ElTypeNum == HTML_EL_TEXT_UNIT)
			{
			  /* copy attribute value into the text leaf */
			  text = TtaAllocString (length + 1);
			  TtaGiveTextAttributeValue (attr, text, &length);
			  TtaSetTextContent (leaf, text, currentLanguage,
					     currentDocument);
			  TtaFreeMemory (text);
			}
		    }
		}
	    }
	}
      break;

	case HTML_EL_META:
         ParseCharset (el);
         break;

    case HTML_EL_STYLE_:	/* it's a STYLE element */
    case HTML_EL_Preformatted:	/* it's a PRE */
    case HTML_EL_SCRIPT:	/* it's a SCRIPT element */
       /* if the last line of the Preformatted is empty, remove it */
       leaf = LastLeafInElement (el);
       if (leaf != NULL)
	  {
	    elType = TtaGetElementType (leaf);
	    if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
	      /* the last leaf is a TEXT element */
	      {
		length = TtaGetTextLength (leaf);
		if (length > 0)
		  {
		     TtaGiveSubString (leaf, lastChar, length, 1);
		     if (lastChar[0] == EOL)
			/* last character is new line, delete it */
			{
		        if (length == 1)
		          /* empty TEXT element */
		          TtaDeleteTree (leaf, currentDocument);
		        else
		          /* remove the last character */
		          TtaDeleteTextContent (leaf, length, 1,
					        currentDocument);
			}
		  }
	      }
	  }
       if (ParsingCSS)
	 {
	   text = GetStyleContents (el);
	   if (text)
	     {
	       ReadCSSRules (currentDocument, NULL, text, FALSE);
	       TtaFreeMemory (text);
	     }
	   ParsingCSS = FALSE;
	 }
	/* and continue as if it were a Preformatted or a Script */
       break;

    case HTML_EL_Text_Area:	/* it's a Text_Area */
      ParsingTextArea = FALSE;
       child = TtaGetFirstChild (el);
       if (child == NULL)
	  /* it's an empty Text_Area */
	  /* insert a Inserted_Text element in the element */
	 {
	   newElType.ElTypeNum = HTML_EL_Inserted_Text;
	   child = TtaNewTree (currentDocument, newElType, "");
	   TtaInsertFirstChild (&child, el, currentDocument);
	 }
       else
	 {
	   /* save the text into Default_Value attribute */
	   attrType.AttrSSchema = DocumentSSchema;
	   attrType.AttrTypeNum = HTML_ATTR_Default_Value;
	   if (TtaGetAttribute (el, attrType) == NULL)
	     /* attribute Default_Value is missing */
	     {
	       attr = TtaNewAttribute (attrType);
	       TtaAttachAttribute (el, attr, currentDocument);
	       desc = TtaGetFirstChild (child);
	       length = TtaGetTextLength (desc) + 1;
	       text = TtaAllocString (length);
	       TtaGiveTextContent (desc, text, &length, &lang);
	       TtaSetAttributeText (attr, text, el, currentDocument);
	       TtaFreeMemory (text);
	     }
	 }
       /* insert a Frame element */
       newElType.ElTypeNum = HTML_EL_Frame;
       constElem = TtaNewTree (currentDocument, newElType, "");
       TtaInsertSibling (constElem, child, FALSE, currentDocument);
       break;

    case HTML_EL_Radio_Input:
    case HTML_EL_Checkbox_Input:
       /* put an attribute Checked if it is missing */
       attrType.AttrSSchema = DocumentSSchema;
       attrType.AttrTypeNum = HTML_ATTR_Checked;
       if (TtaGetAttribute (el, attrType) == NULL)
	  /* attribute Checked is missing */
	 {
	    attr = TtaNewAttribute (attrType);
	    TtaAttachAttribute (el, attr, currentDocument);
	    TtaSetAttributeValue (attr, HTML_ATTR_Checked_VAL_No_, el,
				  currentDocument);
	 }
       break;

    case HTML_EL_Option_Menu:
       /* Check that at least one option has a SELECTED attribute */
       OnlyOneOptionSelected (el, currentDocument, TRUE);
       break;
    case HTML_EL_PICTURE_UNIT:
       break;

    case HTML_EL_LINK:
       CheckCSSLink (el, currentDocument, DocumentSSchema);
       break;

    case HTML_EL_Data_cell:
    case HTML_EL_Heading_cell:
      /* insert a pseudo paragraph into empty cells */
       child = TtaGetFirstChild (el);
       if (child == NULL)
	 {
	   elType.ElTypeNum = HTML_EL_Pseudo_paragraph;
	   child = TtaNewTree (currentDocument, elType, "");
	   if (child != NULL)
	     TtaInsertFirstChild (&child, el, currentDocument);
	 }

       /* detect whether we're parsing a whole table or just a cell */
       if (WithinTable == 0)
	 NewCell (el, currentDocument, FALSE);
       break;

    case HTML_EL_Table:
       CheckTable (el, currentDocument);
       WithinTable--;
       break;

    case HTML_EL_TITLE:
       /* show the TITLE in the main window */
       UpdateTitle (el, currentDocument);
       break;

    default:
       break;
    }
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
   int                 length, nbspaces;
   ElementType         elType;
   Element             lastLeaf;
   CHAR_T                lastChar[2];
   ThotBool            endingSpacesDeleted;

   endingSpacesDeleted = FALSE;
   if (IsBlockElement (el))
      /* it's a block element. */
      {
	   /* Search the last leaf in the element's tree */
	   lastLeaf = LastLeafInElement (el);
	   if (lastLeaf != NULL)
	     {
	       elType = TtaGetElementType (lastLeaf);
	       if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
		 /* the las leaf is a TEXT element */
		 {
		   length = TtaGetTextLength (lastLeaf);
		   if (length > 0)
		     {
		       /* count ending spaces */
		       nbspaces = 0;
		       do
			 {
			   TtaGiveSubString (lastLeaf, lastChar, length,
					     1);
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
			   TtaDeleteTree (lastLeaf, currentDocument);
			 else
			   /* remove the ending spaces */
			   TtaDeleteTextContent (lastLeaf, length + 1,
						 nbspaces, currentDocument);
		     }
		 }
	     }
	   endingSpacesDeleted = TRUE;
      }
   return endingSpacesDeleted;
}

/*----------------------------------------------------------------------
   CloseElement
   End of XML mappedName element.
   Terminate all corresponding Thot elements.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     CloseElement (USTRING mappedName,
				  ThotBool onStartTag)
#else
static ThotBool     CloseElement (mappedName,
				  onStartTag)
USTRING           mappedName;
ThotBool         onStartTag;
#endif
{
   int                 i;
   Element             el, parent;
   ThotBool            ret, stop, spacesDeleted;

   ret = FALSE;
   /* the closed HTML element corresponds to a Thot element. */
   stop = FALSE;
   /* type of the element to be closed */

   if (stackLevel > 0)
     {
       el = lastElement;
       if (lastElementClosed)
	  el = TtaGetParent (el);
       i = stackLevel - 1;

       /* If we meet the end tag of a form, font or center
	  looks for that element in the stack, but not at
	  a higher level as a table element */
       if (!onStartTag &&
	   (!ustrcmp (mappedName, TEXT("form")) ||
            !ustrcmp (mappedName, TEXT("font")) ||
            !ustrcmp (mappedName, TEXT("center"))))
	 {
	   while (i > 0 &&
		  mappedName != nameElementStack[i] &&
		  !stop)
	     if (!ustrcmp (nameElementStack[i], TEXT("tbody")) ||
		 !ustrcmp (nameElementStack[i], TEXT("tr"))    ||
		 !ustrcmp (nameElementStack[i], TEXT("th"))    ||
		 !ustrcmp (nameElementStack[i], TEXT("td")))
	       {
		 /* ignore this end tag */
		 ret = FALSE;
		 stop = TRUE;
		 i = -1;
	       }
	     else
	       i--;
	 }
       else
	 /* looks in the stack for the element to be closed */
	 while (i >= 0 && mappedName != nameElementStack[i])
	   i--;
       
       if (i >= 0 && mappedName == nameElementStack[i])
	 /* element found in the stack */
	 {
	   /* This element and its whole subtree are closed */
	   stackLevel = i;
	   lastElement = elementStack[i];
	   lastElementClosed = TRUE;
	   ret = TRUE;
	 }

       if (ret)
	 /* successful close */
	 {
	   /* remove closed elements from the stack */
	   while (i > 0)
	     if (elementStack[i] == lastElement)
	       {
		 stackLevel = i;
		 i = 0;
	       }
	     else
	       {
		 if (TtaIsAncestor (elementStack[i], lastElement))
	             stackLevel = i;
	         i--;
	       }

	   if (stackLevel > 0)
	     currentLanguage = languageStack[stackLevel - 1];

	   /* complete all closed elements */
	   if (el != lastElement)
	       if (!TtaIsAncestor(el, lastElement))
	           el = NULL;
	   
	   spacesDeleted = FALSE;
	   while (el != NULL)
	     {
	       ElementComplete (el);
	       if (!spacesDeleted)
	          /* If the element closed is a block-element, remove */
	          /* spaces contained at the end of that element */
	          spacesDeleted = RemoveEndingSpaces (el);
	       if (el == lastElement)
		 el = NULL;
	       else
		 el = TtaGetParent (el);
	     }
	 }
     }
   
   return ret;
}

/*--------------------  StartElement  (start)  ---------------------*/

/*----------------------------------------------------------------------
   ProcessEndGI
   Function called at the end of a start tag.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ProcessEndGI (CHAR_T *name)
#else
static void         ProcessEndGI (name)
CHAR_T   *name;

#endif
{

  ElementType         elType;
  AttributeType       attrType;
  Attribute           attr;
  int                 length;
  STRING              text;

  UnknownTag = FALSE;
  if ((lastElement != NULL) &&
      (currentMappedName != NULL))
    {
      if (!ustrcmp (nameElementStack[stackLevel - 1], TEXT("pre"))   ||
	  !ustrcmp (nameElementStack[stackLevel - 1], TEXT("style")) ||
	  !ustrcmp (nameElementStack[stackLevel - 1], TEXT("script")))
	{
	  /* a <PRE>, <STYLE> or <SCRIPT> tag has been read */
	  AfterTagPRE = TRUE;
	}
      else
	{
	  if (!ustrcmp (nameElementStack[stackLevel - 1], TEXT("table")))
	    /* <TABLE> has been read */
	    WithinTable++;
	  else
	    if (currentElementContent == 'E')
	      /* this is an empty element. Do not expect an end tag */
	      {
		CloseElement (currentMappedName, TRUE);
		ElementComplete (lastElement);
	      }
	}
      
      /* if it's a LI element, creates its IntItemStyle attribute
	 according to surrounding elements */
      SetAttrIntItemStyle (lastElement, currentDocument);
      /* if it's an AREA element, computes its position and size */
      ParseAreaCoords (lastElement, currentDocument);
 
      /* if it's a STYLE element in CSS notation, activate the CSS */
      /* parser for parsing the element content */
      elType = TtaGetElementType (lastElement);
      if (elType.ElTypeNum == HTML_EL_STYLE_)
	{
	  /* Search the Notation attribute */
	  attrType.AttrSSchema = elType.ElSSchema;
	  attrType.AttrTypeNum = HTML_ATTR_Notation;
	  attr = TtaGetAttribute (lastElement, attrType);
	  if (attr == NULL)
	    /* No Notation attribute. Assume CSS by default */
	    ParsingCSS = TRUE;
	  else
	    /* the STYLE element has a Notation attribute */
	    /* get its value */
	    {
	      length = TtaGetTextAttributeLength (attr);
	      text = TtaAllocString (length + 1);
	      TtaGiveTextAttributeValue (attr, text, &length);
	      if (!ustrcasecmp (text, TEXT("text/css")))
		ParsingCSS = TRUE;
	      TtaFreeMemory (text);
	    }
	}
      else
	if (elType.ElTypeNum == HTML_EL_Text_Area)
	  {
	    /* we have to read the content as a simple text unit */
	    ParsingTextArea = TRUE;
	  }     
    }
}

/*----------------------------------------------------------------------
   ContextOK 
   Returns TRUE if the element at position entry in the mapping table
   is allowed to occur in the current structural context.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     ContextOK (STRING elName)
#else
static ThotBool     ContextOK (elName)
STRING    elName;

#endif
{
   ThotBool      ok;

   if (stackLevel == 0 || nameElementStack[stackLevel - 1] == NULL)
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
		     ProcessStartGI (TEXT("tr"));
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
		     ProcessStartGI (TEXT("tr"));
		   }
		 else
		   ok = FALSE;
	   }

       if (ok)
	 /* refuse BODY within BODY */
	 if (ustrcmp (elName, TEXT("body")) == 0)
	     if (Within (HTML_EL_BODY, DocumentSSchema))
	         ok = FALSE;

       if (ok)
	 /* refuse HEAD within HEAD */
	 if (ustrcmp (elName, TEXT("head")) == 0)
	   if (Within (HTML_EL_HEAD, DocumentSSchema))
	     ok = FALSE;

       if (ok)
	 /* refuse STYLE within STYLE */
	 if (ustrcmp (elName, TEXT("style")) == 0)
	   if (Within (HTML_EL_STYLE_, DocumentSSchema))
	     ok = FALSE;

       return ok;
     }
}

/*----------------------------------------------------------------------
   ProcessStartGI  
   The name of an element type has been read in a start tag.
   Create the corresponding Thot thing (element, attribute,
   or character), according to the mapping table.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ProcessStartGI (CHAR_T* GIname)
#else
static void         ProcessStartGI (GIname)
CHAR_T*             GIname;

#endif
{
  ElementType         elType;
  Element             newElement;
  int                 i;
  CHAR_T              msgBuffer[MaxMsgLength];
  ThotBool            sameLevel;
  STRING              mappedName= NULL;
  ThotBool            elInStack = FALSE;

  /* ignore tag <P> within PRE */
  if (Within (HTML_EL_Preformatted, DocumentSSchema))
    if (ustrcasecmp (GIname, TEXT("p")) == 0)
      return;

  if (currentElement != NULL)
      ParseHTMLError (currentDocument,
		      TEXT("XML parser error 1"));
  else
    if (stackLevel == MAX_STACK_HEIGHT)
      {
	ParseHTMLError (currentDocument,
			TEXT("**FATAL** Too many XML levels"));
	XMLabort = TRUE;
      }
    else
      {
	/* search the XML element name in the corresponding mapping table */
       elType.ElSSchema = NULL;
       elType.ElTypeNum = 0;
       currentMappedName[0] = WC_EOS;
       GetXmlElType (GIname, &elType, &mappedName,
		     &currentElementContent, currentDocument);
       if (mappedName != NULL)
	   ustrcpy (currentMappedName, mappedName);

       if (elType.ElTypeNum <= 0)
	 {
	   /* not found in the corresponding DTD */
	   if (ustrlen (GIname) > MaxMsgLength - 20)
	       GIname[MaxMsgLength - 20] = WC_EOS;
	   usprintf (msgBuffer, TEXT("Unknown XML element %s"), GIname);
	   ParseHTMLError (currentDocument, msgBuffer);
	   UnknownTag = TRUE;
	   nameElementStack[stackLevel] = NULL;
	   elementStack[stackLevel] = NULL;
	   elInStack = TRUE;
	 }
       else
	 {
	   /* element found in the corresponding DTD */
	   if (!ContextOK (mappedName))
	     /* element not allowed in the current structural context */
	     {
	       usprintf (msgBuffer,
			 TEXT("Tag <%s> is not allowed here"), GIname);
	       ParseHTMLError (currentDocument, msgBuffer);
	       UnknownTag = TRUE;
	       nameElementStack[stackLevel] = NULL;
	       elementStack[stackLevel] = NULL;
	       elInStack = TRUE;
	     }
	   else
	     {
	       newElement = NULL;
	       sameLevel = TRUE;
	       
	       if (elType.ElTypeNum == HTML_EL_HTML)
		 /* the corresponding Thot element is the root of the
		    abstract tree, which has been created at initialization */
		 newElement = rootElement;
	       else
		 /* create a Thot element */
		 {
		   if (currentElementContent == 'E')
		     /* empty XML element. Create all children specified */
		     /* in the Thot structure schema */
		     newElement = TtaNewTree (currentDocument, elType, "");
		   else
		     /* the HTML element may have children. Create only */
		     /* the corresponding Thot element, without any child */
		     newElement = TtaNewElement (currentDocument, elType);
		   
		   TtaSetElementLineNumber (newElement, NumberOfLinesRead);
		   sameLevel = InsertElement (&newElement);
		   if (newElement != NULL)
		     {
		       if (currentElementContent == 'E')
			   lastElementClosed = TRUE;
			   currentElementClosed = TRUE;

		       if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
			 /* an empty Text element has been created. The */
			 /* following character data must go to that elem. */
			 MergeText = TRUE;
		     }
		 }
	       
	       if (currentElementContent != 'E')
		 {
		   elementStack[stackLevel] = newElement;
		   nameElementStack[stackLevel] = mappedName;
		   elInStack = TRUE;
		 }
	     }
	 }

       if (elInStack)
	 {
	   languageStack[stackLevel] = currentLanguage;
	   parserCtxtStack[stackLevel] = currentParserCtxt;
	   stackLevel++;
	 }

       currentAttribute = NULL;
       HTMLStyleAttribute = FALSE;

      }
  
}
/*----------------------  StartElement  (end)  -----------------------*/


/*----------------------  EndElement  (start)  -----------------------*/

/*----------------------------------------------------------------------
   EndOfElement
   Terminate all corresponding Thot elements.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void    EndOfElement (CHAR_T *GIname)
#else
static void    EndOfElement (GIname)
CHAR_T     *GIname;

#endif
{
   CHAR_T       msgBuffer[MaxMsgLength];
   STRING       mappedName;
   ElementType  elType;
   int          i;
   ThotBool     ok;


   if (ParsingTextArea)
     {
       if (ustrcasecmp (GIname, TEXT("textarea")) != 0)
         /* We are parsing the contents of a textarea element. The end
	    tag is not the one closing the current textarea, consider it
	    as plain text */
	 {
	   return;
         }
     }

   /* is it the end of the current HTML fragment ? */
   ok = FALSE;
   if (XMLrootClosingTag != EOS)
     {
       if (ustrcasecmp (GIname, XMLrootClosingTag) == 0)
	 {
	   XMLrootClosed = TRUE;
	   ok = TRUE;
	 }
     }
   
   if (!ok)
     {
       /* search the XML tag in the mapping table */
       elType.ElSSchema = NULL;
       elType.ElTypeNum = 0;
       GetXmlElType (GIname, &elType, &mappedName,
		     &currentElementContent, currentDocument);

       if (elType.ElTypeNum <= 0)
	   /* not found in the corresponding DTD */
	 {
	   if (ustrlen (GIname) > MaxMsgLength - 20)
	       GIname[MaxMsgLength - 20] = WC_EOS;
	   usprintf (msgBuffer,
		     TEXT("Unknown XML element </%s>"), GIname);
	   ParseHTMLError (currentDocument, msgBuffer);
	 }
       else
	 {
	   if (!CloseElement (mappedName, FALSE))
	     /* the end tag does not close any current element */
	     {
	       usprintf (msgBuffer,
			 TEXT("Unexpected end tag </%s>"), GIname);
	       ParseHTMLError (currentDocument, msgBuffer);
	     }
	 }
     }

   AfterTagPRE = FALSE;
}
/*---------------------  EndElement  (end)  --------------------------*/


/*----------------------  Data  (start)  -----------------------------*/

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
   ElementType     elType;
   Element         elText, parent, ancestor, prev;
   int             i;
   int             length;
   ThotBool        ignoreLeadingSpaces;
   static ThotBool EmptyLine;

   length = ustrlen (data);
   i = 0;

   /* remove leading spaces for merged text and */
   /* replace single CR character by space character */
   /* except for <PRE>, <STYLE> and <SCRIPT> elements */
   if (!AfterTagPRE)
     {
       if (length == 1 &&
	   (data[0] == WC_EOL  || data[0] == WC_CR))
	 {
	   data[0] = WC_SPACE;
	   EmptyLine = 1;
	 }
       else
	 if (EmptyLine)
	   {
	     while ((data[i] == WC_SPACE || data[i] == WC_TAB) &&
		    data[i] != WC_EOS)
	            i++;
	     EmptyLine = 0;
	   }
     }
   
   /* replace single CR character by space character */
   /* except for <PRE>, <STYLE> and <SCRIPT> elements */
   /*
   if (length == 1 && !AfterTagPRE &&
       (data[0] == WC_EOL  || data[0] == WC_CR))
     {
       data[0] = WC_SPACE;
     }
   */

   if (lastElement != NULL)
     {
	if (InsertSibling ())
	   /* There is a previous sibling (lastElement) 
	      for the new Text element */
	  {
	     parent = TtaGetParent (lastElement);
	     if (parent == NULL)
		 parent = lastElement;
	     elType = TtaGetElementType (parent);
	     if (IsCharacterLevelElement (lastElement) &&
		 elType.ElTypeNum != HTML_EL_Option_Menu &&
		 elType.ElTypeNum != HTML_EL_OptGroup)
	       {
		 ignoreLeadingSpaces = FALSE;
		 elType = TtaGetElementType (lastElement);
		 if (elType.ElTypeNum == HTML_EL_BR)
		     ignoreLeadingSpaces = TRUE;
	       }
	     else
	         ignoreLeadingSpaces = TRUE;
	  }
	else
	   /* the new Text element should be the first child of the latest
	      element encountered */
	  {
	    parent = lastElement;
	    ignoreLeadingSpaces = TRUE;
	    elType = TtaGetElementType (lastElement);
	    if (elType.ElTypeNum != HTML_EL_Option_Menu &&
		elType.ElTypeNum != HTML_EL_OptGroup)
	      {
	        ancestor = parent;
	        while (ignoreLeadingSpaces &&
		       IsCharacterLevelElement (ancestor))
		  {
		    prev = ancestor;
		    TtaPreviousSibling (&prev);
		    if (prev == NULL)
		        ancestor = TtaGetParent (ancestor);
		    else
		        ignoreLeadingSpaces = FALSE;
		  }
	      }
	  }

	if (ignoreLeadingSpaces)
	    if (!Within (HTML_EL_Preformatted, DocumentSSchema) &&
		!Within (HTML_EL_STYLE_, DocumentSSchema) &&
		!Within (HTML_EL_SCRIPT, DocumentSSchema))
	        /* suppress leading spaces */
	        while (data[i] <= WC_SPACE && data[i] != WC_EOS)
		  i++;

	if (data[i] != WC_EOS)
	  {
	    elType = TtaGetElementType (lastElement);
	    if (elType.ElTypeNum == HTML_EL_TEXT_UNIT && MergeText)
	      {
		TtaAppendTextContent (lastElement,
				      &(data[i]),
				      currentDocument);
	      }
	    else
	      {
		/* create a TEXT element */
		elType.ElSSchema = DocumentSSchema;
		elType.ElTypeNum = HTML_EL_TEXT_UNIT;
		elText = TtaNewElement (currentDocument, elType);
		TtaSetElementLineNumber (elText, BufferLineNumber);
		InsertElement (&elText);
		lastElementClosed = TRUE;
		MergeText = TRUE;
		/* put the content of the input buffer into the TEXT element */
		if (elText != NULL)
		  {
		    TtaSetTextContent (elText,
				       &(data[i]),
				       currentLanguage,
				       currentDocument);
		  }
	      }
	  }
     }
}
/*----------------------  Data  (end)  ---------------------------*/


/*--------------------  Attributes  (start)  ---------------------*/

/*----------------------------------------------------------------------
   CreateAttr 
   Create an attribute of type attrType for the element el.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CreateAttr (Element el,
				AttributeType attrType,
				CHAR_T* text,
				ThotBool invalid)
#else
static void         CreateAttr (el,
				attrType,
				text,
				invalid)
Element             el;
AttributeType       attrType;
CHAR_T*             text;
ThotBool            invalid;

#endif
{
   int                 attrKind;
   int                 length;
   CHAR_T*             buffer;
   Attribute           attr, oldAttr;

   if (attrType.AttrTypeNum != 0)
     {
	oldAttr = TtaGetAttribute (el, attrType);
	if (oldAttr != NULL)
	   /* this attribute already exists */
	   attr = oldAttr;
	else
	   /* create a new attribute and attach it to the element */
	  {
	     attr = TtaNewAttribute (attrType);
	     TtaAttachAttribute (el, attr, currentDocument);
	  }
	lastAttribute = attr;
	lastAttrElement = el;
	TtaGiveAttributeType (attr, &attrType, &attrKind);
	if (attrKind == 0)	/* enumerate */
	   TtaSetAttributeValue (attr, 1, el, currentDocument);
	/* attribute BORDER without any value (ThotBool attribute) is */
	/* considered as BORDER=1 */
	if (attrType.AttrTypeNum == HTML_ATTR_Border)
	   TtaSetAttributeValue (attr, 1, el, currentDocument);
	if (invalid)
	   /* Copy the name of the invalid attribute as the content */
	   /* of the Invalid_attribute attribute. */
	  {
	     length = ustrlen (text) + 2;
	     length += TtaGetTextAttributeLength (attr);
	     buffer = TtaAllocString (length + 1);
	     TtaGiveTextAttributeValue (attr, buffer, &length);
	     ustrcat (buffer, TEXT(" "));
	     ustrcat (buffer, text);
	     TtaSetAttributeText (attr, buffer, el, currentDocument);
	     TtaFreeMemory (buffer);
	  }
     }
}

/*----------------------------------------------------------------------
   EndOfAttrName   
   A XML attribute has been read. 
   Create the corresponding Thot attribute.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndOfAttrName (CHAR_T *attrName)
#else
static void         EndOfAttrName (attrName)
CHAR_T         *attrName;

#endif
{
   AttributeMapping*   mapAttr;
   AttributeType       attrType;
   ElementType         elType;
   Element             child;
   Attribute           attr;
   CHAR_T              translation;
   ThotBool            invaldAttr;
   CHAR_T              msgBuffer[MaxMsgLength];


   currentAttribute = NULL;
   HTMLStyleAttribute = FALSE;

   if (nameElementStack[stackLevel-1] == NULL) 
       return;

   /* if a single '/' or '?' has been read instead of an attribute name, ignore
      that character.  This is to accept the XML syntax for empty elements or
      processing instructions, such as <img src="SomeUrl" /> or
      <?xml version="1.0"?>  */

   /* A FAIRE */

   invaldAttr = FALSE;
   /* get the corresponding Thot attribute */
   if (UnknownTag)
      /* ignore attributes of unknown tags */
      mapAttr = NULL;
   else
      mapAttr = XhtmlMapAttribute (attrName, &attrType,
				   nameElementStack[stackLevel-1],
				   currentDocument);

   if (mapAttr == NULL)
      /* this attribute is not in the HTML mapping table */
     {
	if (ustrcasecmp (attrName, TEXT("xmlns")) == 0 ||
	    ustrncasecmp (attrName, TEXT("xmlattrNamens:"), 6) == 0)
	  /* this is a namespace declaration */
	  {
	    lastMappedAttr = NULL;
	    /**** register this namespace ****/;
	  }
	else
	  if (ustrcasecmp (attrName, TEXT("xml:lang")) == 0)
	    /* attribute xml:lang is not considered as invalid, but it is
	       ignored */
	    lastMappedAttr = NULL;
	  else
	    {
	      if (ustrlen (attrName) > MaxMsgLength - 30)
		  attrName[MaxMsgLength - 30] = WC_EOS;
	      usprintf (msgBuffer,
			TEXT("Unknown attribute \"%s\""),
			attrName);
	      ParseHTMLError (currentDocument, msgBuffer);
	      /* attach an Invalid_attribute to the current element */
	      mapAttr = XhtmlMapAttribute (TEXT("unknown_attr"),
					   &attrType,
					   nameElementStack[stackLevel-1],
					   currentDocument);
	      invaldAttr = TRUE;
	      UnknownAttr = TRUE;
	    }
     }
   else
        UnknownAttr = FALSE;


   if (mapAttr != NULL && lastElement != NULL &&
       (!lastElementClosed || (lastElement != rootElement)))
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
	   CreateAttr (lastElement, attrType, attrName, invaldAttr);
	   if (attrType.AttrTypeNum == HTML_ATTR_HREF_)
	     {
	       elType = TtaGetElementType (lastElement);
	       if (elType.ElTypeNum == HTML_EL_Anchor)
		 /* attribute HREF for element Anchor */
		 /* create attribute PseudoClass = link */
		 {
		   attrType.AttrTypeNum = HTML_ATTR_PseudoClass;
		   attr = TtaNewAttribute (attrType);
		   TtaAttachAttribute (lastElement, attr, currentDocument);
		   TtaSetAttributeText (attr, TEXT("link"),
					lastElement, currentDocument);
		 }
	     }
	   else
	     if (attrType.AttrTypeNum == HTML_ATTR_Checked)
	       {
		 /* create Default-Checked attribute */
		 child = TtaGetFirstChild (lastElement);
		 if (child != NULL)
		   {
		     attrType.AttrTypeNum = HTML_ATTR_DefaultChecked;
		     attr = TtaNewAttribute (attrType);
		     TtaAttachAttribute (child, attr, currentDocument);
		     TtaSetAttributeValue (attr, HTML_ATTR_DefaultChecked_VAL_Yes_,
					   child, currentDocument);
		   }
	       }
	     else 
	       if (attrType.AttrTypeNum == HTML_ATTR_Selected)
		 {
		   /* create Default-Selected attribute */
		   attrType.AttrTypeNum = HTML_ATTR_DefaultSelected;
		   attr = TtaNewAttribute (attrType);
		   TtaAttachAttribute (lastElement, attr, currentDocument);
		   TtaSetAttributeValue (attr, HTML_ATTR_DefaultSelected_VAL_Yes_,
					 lastElement, currentDocument);
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
   PutInContent    
   Put the string ChrString in the leaf of current element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static Element      PutInContent (STRING ChrString)
#else
static Element      PutInContent (ChrString)
STRING              ChrString;

#endif
{
   Element             el, child;
   ElementType         elType;
   int                 length;

   el = NULL;
   if (lastElement != NULL)
     {
	/* search first leaf of current element */
	el = lastElement;
	do
	  {
	     child = TtaGetFirstChild (el);
	     if (child != NULL)
		el = child;
	  }
	while (child != NULL);
	elType = TtaGetElementType (el);
	length = 0;
	if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
	   length = TtaGetTextLength (el);
	if (length == 0)
	   TtaSetTextContent (el, ChrString, currentLanguage, currentDocument);
	else
	   TtaAppendTextContent (el, ChrString, currentDocument);
     }
   return el;
}

/*----------------------------------------------------------------------
   TypeAttrValue 
   Value val has been read for the HTML attribute TYPE.
   Create a child for the current Thot element INPUT accordingly.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         TypeAttrValue (CHAR_T* val)
#else
static void         TypeAttrValue (val)
CHAR_T*             val;

#endif
{
  ElementType         elType;
  Element             newChild;
  AttributeType       attrType;
  Attribute           attr;
  CHAR_T              msgBuffer[MaxMsgLength];
  int                 value;

  attrType.AttrSSchema = DocumentSSchema;
  attrType.AttrTypeNum = DummyAttribute;
  XhtmlMapAttributeValue (val, attrType, &value);

  if (value < 0)
    {
      if (ustrlen (val) > MaxMsgLength - 40)
          val[MaxMsgLength - 40] = WC_EOS;
      usprintf (msgBuffer, TEXT("Unknown attribute value \"type = %s\""), val);
      ParseHTMLError (currentDocument, msgBuffer);
      usprintf (msgBuffer, TEXT("type=%s"), val);
      XhtmlMapAttribute (TEXT("unknown_attr"), &attrType,
			 NULL, currentDocument);
      CreateAttr (lastElement, attrType, msgBuffer, TRUE);
    }
  else
    {
      elType = TtaGetElementType (lastElement);
      if (elType.ElTypeNum != HTML_EL_Input)
	{
        if (ustrlen (val) > MaxMsgLength - 40)
	   val[MaxMsgLength - 40] = WC_EOS;
	usprintf (msgBuffer, TEXT("Duplicate attribute \"type = %s\""), val);
	}
      else
	{
	  elType.ElSSchema = DocumentSSchema;
	  elType.ElTypeNum = value;
	  newChild = TtaNewTree (currentDocument, elType, "");
	  TtaSetElementLineNumber (newChild, NumberOfLinesRead);
	  TtaInsertFirstChild (&newChild, lastElement, currentDocument);
	  if (value == HTML_EL_PICTURE_UNIT)
	    {
	      /* add the attribute IsInput to input pictures */
	      attrType.AttrSSchema = elType.ElSSchema;
	      attrType.AttrTypeNum = HTML_ATTR_IsInput;
	      attr = TtaNewAttribute (attrType);
	      TtaAttachAttribute (newChild, attr, currentDocument);
	    }
	}
    }
}

/*----------------------------------------------------------------------
   EndOfAttrValue
   An attribute value has been read from the HTML file.
   Put that value in the current Thot attribute.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndOfAttrValue (CHAR_T *attrValue)
#else
static void         EndOfAttrValue (attrValue)
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
   ThotBool            done;
   CHAR_T              msgBuffer[MaxMsgLength];

   ReadingAnAttrValue = FALSE;

   if (UnknownAttr)
       /* this is the end of value of an invalid attribute. Keep the */
       /* quote character that ends the value for copying it into the */
       /* Invalid_attribute. */
     {
       /* What to do in this case ? */
     }

   if (lastMappedAttr == NULL)
     {
       return;
     }

   done = FALSE;
   /* treatments of some particular HTML attributes */
   if (!ustrcmp (lastMappedAttr->XMLattribute, TEXT("style")))
     {
       TtaSetAttributeText (lastAttribute, attrValue,
			    lastAttrElement, currentDocument);
       ParseHTMLSpecificStyle (lastElement, attrValue,
			       currentDocument, FALSE);
       done = TRUE;
     }
   else
     {
       if (!ustrcmp (lastMappedAttr->XMLattribute, TEXT("link")))
	   HTMLSetAlinkColor (currentDocument, attrValue);
       else
	   if (!ustrcmp (lastMappedAttr->XMLattribute, TEXT("alink")))
	       HTMLSetAactiveColor (currentDocument, attrValue);
	   else
	       if (!ustrcmp (lastMappedAttr->XMLattribute, TEXT("vlink")))
		   HTMLSetAvisitedColor (currentDocument, attrValue);
     }

   if (!done)
     {
       val = 0;
       translation = lastMappedAttr->AttrOrContent;

       switch (translation)
	 {
	 case 'C':	/* Content */
	   child = PutInContent (attrValue);
	   if (child != NULL)
	       TtaAppendTextContent (child, TEXT("\" "), currentDocument);
	   break;

	 case 'A':
	   if (lastAttribute != NULL)
	     {
	       TtaGiveAttributeType (lastAttribute, &attrType, &attrKind);
	       switch (attrKind)
		 {
		 case 0:	/* enumerate */
		   XhtmlMapAttributeValue (attrValue, attrType, &val);
		   if (val < 0)
		     {
		       TtaGiveAttributeType (lastAttribute, &attrType, &attrKind);
		       attrName = TtaGetAttributeName (attrType);
		       if (ustrlen (attrValue) > MaxMsgLength - 30)
			   attrValue[MaxMsgLength - 30] = WC_EOS;
		       usprintf (msgBuffer,
				 TEXT("Unknown attribute value \"%s = %s\""),
				 attrName, attrValue);
		       ParseHTMLError (currentDocument, msgBuffer);

		       /* remove the attribute and replace it by an */
		       /* Invalid_attribute */
		       TtaRemoveAttribute (lastAttrElement,
					   lastAttribute, currentDocument);
		       usprintf (msgBuffer, TEXT("%s=%s"), attrName, attrValue);
		       XhtmlMapAttribute (TEXT("unknown_attr"),
					  &attrType,
					  NULL,
					  currentDocument);
		       CreateAttr (lastAttrElement, attrType, msgBuffer, TRUE);
		     }
		   else
		       TtaSetAttributeValue (lastAttribute, val,
					     lastAttrElement, currentDocument);
		   break;
		 case 1:	/* integer */
		   if (attrType.AttrTypeNum == HTML_ATTR_Border &&
		       !ustrcasecmp (attrValue, TEXT("border")))
		     {
		       /* border="border" for a table */
		       val = 1;
		       TtaSetAttributeValue (lastAttribute, val,
					     lastAttrElement, currentDocument);
		     }
		   else
		     {
		       if (usscanf (attrValue, TEXT("%d"), &val))
			   TtaSetAttributeValue (lastAttribute, val,
						 lastAttrElement, currentDocument);
		       else
			 {
			   TtaRemoveAttribute (lastAttrElement,
					       lastAttribute, currentDocument);
			   usprintf (msgBuffer,
				     TEXT("Invalid attribute value \"%s\""),
				     attrValue);
			   ParseHTMLError (currentDocument, msgBuffer);
			 }
		     }
		   break;
		 case 2:	/* text */
		   if (!UnknownAttr)
		     {
		       TtaSetAttributeText (lastAttribute, attrValue,
					    lastAttrElement, currentDocument);
		       if (attrType.AttrTypeNum == HTML_ATTR_Langue)
			 {
			   /* it's a LANG attribute value */
			   lang = TtaGetLanguageIdFromName (attrValue);
			   if (lang == 0)
			     {
			       usprintf (msgBuffer,
					 TEXT("Unknown language: %s"),
					 attrValue);
			       ParseHTMLError (currentDocument, msgBuffer);
			     }
			   else
			     {
			       /* change current language */
			       currentLanguage = lang;
			       languageStack[stackLevel - 1] = currentLanguage;
			     }
			   if (!TtaGetParent (lastAttrElement))
			     /* it's a LANG attribute on the root element */
			     /* set the RealLang attribute */
			     {
			       attrType1.AttrSSchema = DocumentSSchema;
			       attrType1.AttrTypeNum = HTML_ATTR_RealLang;
			       attr = TtaNewAttribute (attrType1);
			       TtaAttachAttribute (lastAttrElement,
						   attr, currentDocument);
			       TtaSetAttributeValue (attr,
						     HTML_ATTR_RealLang_VAL_Yes_,
						     lastAttrElement, currentDocument);
			     }
			 }
		     }
		   else
		     /* this is the content of an invalid attribute */
		     /* append it to the current Invalid_attribute */
		     {
		       length = ustrlen (attrValue) + 2;
		       length += TtaGetTextAttributeLength (lastAttribute);
		       buffer = TtaAllocString (length + 1);
		       TtaGiveTextAttributeValue (lastAttribute,
						  buffer, &length);
		       ustrcat (buffer, TEXT("="));
		       ustrcat (buffer, attrValue);
		       TtaSetAttributeText (lastAttribute, buffer,
					    lastAttrElement, currentDocument);
		       TtaFreeMemory (buffer);
		     }
		   break;
		 case 3:	/* reference */
		   break;
		 }
	     }
	   break;
	   
	 case SPACE:
	   TypeAttrValue (attrValue);
	   break;

	 default:
	   break;
	 }

       if (lastMappedAttr->ThotAttribute == HTML_ATTR_Width__)
	 /* HTML attribute "width" for a table or a hr */
	 /* create the corresponding attribute IntWidthPercent or */
	 /* IntWidthPxl */
	 CreateAttrWidthPercentPxl (attrValue, lastAttrElement,
				    currentDocument, -1);
       else
	 if (!ustrcmp (lastMappedAttr->XMLattribute, TEXT("size")))
	   {
	     TtaGiveAttributeType (lastAttribute, &attrType, &attrKind);
	     if (attrType.AttrTypeNum == HTML_ATTR_Font_size)
	         CreateAttrIntSize (attrValue,
				    lastAttrElement,
				    currentDocument);
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
		   TtaSetGraphicsShape (child, shape, currentDocument);
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
		     TtaAttachAttribute (lastAttrElement, attr, currentDocument);
		     TtaSetAttributeText (attr, attrValue,
					  lastAttrElement, currentDocument);
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
		   ParseHTMLSpecificStyle (lastElement, msgBuffer,
					   currentDocument, FALSE);
		 }
	       else
		 if (!ustrcmp (lastMappedAttr->XMLattribute, TEXT("bgcolor")))
		     HTMLSetBackgroundColor (currentDocument,
					     lastElement, attrValue);
		 else
		   if (!ustrcmp (lastMappedAttr->XMLattribute, TEXT("text")) ||
		       !ustrcmp (lastMappedAttr->XMLattribute, TEXT("color")))
		     HTMLSetForegroundColor (currentDocument,
					     lastElement, attrValue);
     }
}
/*--------------------  Attributes  (end)  ---------------------*/


/*--------------------  Entities  (start)  ---------------------*/

/*----------------------------------------------------------------------
   PutNonISOlatin1Char     
   Put a Unicode character in the input buffer.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     PutNonISOlatin1Char (int code,
				     STRING prefix,
				     STRING entityName)
#else
static void     PutNonISOlatin1Char (code,
				     prefix,
				     entityName)
int              code;
STRING           prefix;
STRING           entityName;
#endif
{
   Language	 lang, l;
   ElementType	 elType;
   Element	 elText;
   AttributeType attrType;
   Attribute	 attr;
   CHAR_T	 buffer[MaxEntityLength+10];

   if (ReadingAnAttrValue)
     /* this entity belongs to an attribute value */
     {
       /* Thot can't mix different languages in the same attribute value */
       /* just discard that character */
       ;
     }
   else
     /* this entity belongs to the element contents */
     {
       MergeText = FALSE;
       /* create a new text leaf */
       elType.ElSSchema = DocumentSSchema;
       elType.ElTypeNum = HTML_EL_TEXT_UNIT;
       elText = TtaNewElement (currentDocument, elType);
       TtaSetElementLineNumber (elText, NumberOfLinesRead);
       InsertElement (&elText);
       lastElementClosed = TRUE;
       /* try to find a fallback character */
       l = currentLanguage;
       GetFallbackCharacter (code, buffer, &lang);
       /* put that fallback character in the new text leaf */
       TtaSetTextContent (elText, buffer, lang, currentDocument);
       currentLanguage = l;
       /* make that text leaf read-only */
       TtaSetAccessRight (elText, ReadOnly, currentDocument);
       /* associate an attribute EntityName with the new text leaf */
       attrType.AttrSSchema = DocumentSSchema;
       attrType.AttrTypeNum = HTML_ATTR_EntityName;
       attr = TtaNewAttribute (attrType);
       TtaAttachAttribute (elText, attr, currentDocument);
       ustrcpy (buffer, prefix);
       ustrcat (buffer, entityName);
       TtaSetAttributeText (attr, buffer, elText, currentDocument);
       MergeText = FALSE;
     }
}

/*----------------------------------------------------------------------
   EndOfEntity
   End of a HTML entity. Search that entity in the entity table 
   and put the corresponding character in the corresponding element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndOfXmlEntity (STRING entityName)
#else
static void         EndOfXmlEntity (entityName)
STRING       entityName;

#endif
{
   CHAR_T         msgBuffer[MaxMsgLength];
   STRING         buffer;
   CHAR_T*        ptr;
   CHAR_T	  alphabet;
   int            entityVal;	
   UCHAR_T        entityValue[MaxEntityLength];	
   Language	  lang;

   printf ("\n EndOfXmlEntity - Name : %s", entityName);

   buffer = TtaAllocString ((ustrlen (entityName)) - 1);
   ustrcpy (buffer, &entityName[1]);
   if (ptr = ustrrchr (buffer, TEXT(';')))
       ustrcpy (ptr, TEXT("\0"));
   printf ("\n EndOfXmlEntity - buffer:%s", buffer);

   /*
   (*(currentParserCtxt->MapEntity)) (buffer, entityValue,
				      MaxEntityLength-1, &alphabet);
   */
   XhtmlMapEntity (buffer, &entityVal,
		   MaxEntityLength-1, &alphabet);

   lang = 0;
   if (alphabet == EOS)
     {
       /* Unknown entity */
       entityValue[0] = EOS;
       lang = -1;
       PutInXmlElement (entityName);
       /* print an error message */
       usprintf (msgBuffer,
		 TEXT("Unknown XML entity \"&%s;\""),
		 entityName);
       ParseHTMLError (currentDocument, msgBuffer);
     }
   else
     {
       printf (" \n code=%d", entityVal);
       if (entityValue[0] != EOS)
	 lang = TtaGetLanguageIdFromAlphabet(alphabet);
       
       if (entityVal < 255)
	   PutInXmlElement ((STRING) entityVal);
       else
	   PutNonISOlatin1Char (entityVal,
				TEXT(""),
				entityName);
     }
   
   TtaFreeMemory (buffer);
}
/*--------------------  Entities  (end)  ---------------------*/


/*--------------------  Comments  (start)  ---------------------*/
/*----------------------------------------------------------------------
   CreateComment
   Create a comment element into the Thot tree.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void    CreateComment (CHAR_T *commentValue)
#else
static void    CreateComment (commentValue)
CHAR_T     *commentValue;

#endif
{
   ElementType   elType;
   Element  	 commentEl, commentLineEl;
   STRING        mappedName;
   CHAR_T        cont;
   UCHAR_T       msgBuffer[MaxMsgLength];

   /* create a Thot element for the comment */
   elType.ElSSchema = NULL;
   elType.ElTypeNum = 0;
   GetXmlElType (TEXT("XMLcomment"), &elType,
		 &mappedName, &cont, currentDocument);
   if (elType.ElTypeNum <= 0)
     {
       usprintf (msgBuffer,
		 TEXT("Unknown XML element %s"), commentValue);
       ParseHTMLError (currentDocument, msgBuffer);
     }
   else
     {
       commentEl = TtaNewElement (currentDocument, elType);
       TtaSetElementLineNumber (commentEl, NumberOfLinesRead);
       InsertElement (&commentEl);
       /* A changer */
       /* XMLInsertElement (commentEl); */
       lastElementClosed = TRUE;

       /* create a XMLcomment_line element as the first child of */
       /* element XMLcomment */
       elType.ElSSchema = NULL;
       elType.ElTypeNum = 0;
       GetXmlElType (TEXT("XMLcomment_line"), &elType,
		     &mappedName, &cont, currentDocument);
       commentLineEl = TtaNewElement (currentDocument, elType);
       SetElemLineNumber (commentLineEl);
       TtaInsertFirstChild (&commentLineEl, commentEl, currentDocument);

       /* create a TEXT element as the first child of element XMLcomment_line */
       elType.ElTypeNum = 1;
       CommentText = TtaNewElement (currentDocument, elType);
       SetElemLineNumber (CommentText);
       TtaInsertFirstChild (&CommentText, commentLineEl, currentDocument);
       TtaSetTextContent (CommentText, commentValue,
			  currentLanguage, currentDocument);
     }
}
/*--------------------  Comments  (end)  ---------------------*/


#ifdef EXPAT
/* Handlers associated with Amaya */

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
  printf ("\n Hndl_CdataStart");
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
  printf ("\n Hndl_CdataEnd");
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
				    int length)
#else  /* __STDC__ */
static void     Hndl_CharacterData (userData,
				    data,
				    length)
void            *userData; 
const XML_Char  *data;
int              length;
#endif  /* __STDC__ */

{
   STRING        buffer;
   STRING        bufferwc;
   int           i;

   printf ("\n Hndl_CharacterData - length = %d - ", length);

   buffer = TtaAllocString (length + 1);
   bufferwc = TtaAllocString (length + 1);

   for (i=0; i<length; i++)
     {
       buffer[i] = data[i];
       printf ("%c", data[i]);
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
   printf ("\n Hndl_Comment %s", data);
   CreateComment ((CHAR_T*) data);
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
			      int length)
#else  /* __STDC__ */
static void     Hndl_Default (userData,
			      data,
			      length)
void            *userData; 
const XML_Char  *data;
int              length;
#endif  /* __STDC__ */

{
  int  i;
  
  printf ("\n Hndl_Default - length = %d - ", length);
  for (i=0; i<length; i++)
    {
      printf ("%c", data[i]);
    }
}

/*----------------------------------------------------------------------
   Hndl_DefaultExpand
   Default handler with expansion of internal entity references
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     Hndl_DefaultExpand (void *userData,
				    const XML_Char *data,
				    int length)
#else  /* __STDC__ */
static void     Hndl_DefaultExpand (userData,
				    data,
				    length)
void            *userData; 
const XML_Char  *data;
int              length;
#endif  /* __STDC__ */

{
   int        i;
   STRING     buffer;

   printf ("\n Hndl_DefaultExpand - length = %d - ", length);
   for (i=0; i<length; i++)
     {
       printf ("%c", data[i]);
     }
   
   /* Treatment of the entities */
   if (length > 1 && data[0] == '&')
     {
       buffer = TtaAllocString (length + 1);
       for (i = 0; i < length; i++)
	 buffer[i] = data[i];
       buffer[length] = WC_EOS;
       EndOfXmlEntity (buffer);
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
static void     Hndl_DoctypeStart (userData,
				   doctypeName)
void            *userData; 
const XML_Char  *doctypeName;
#endif  /* __STDC__ */

{
   printf ("\n Hndl_DoctypeStart %s", doctypeName);
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
   printf ("\n Hndl_DoctypeEnd");
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
static void     Hndl_ElementStart (userData,
				   name,
				   attlist)
void            *userData; 
const XML_Char  *name;
const XML_Char **attlist;
#endif  /* __STDC__ */

{
   int           nbatts;
   CHAR_T       *buffer;
   CHAR_T       *bufAttr;
   CHAR_T       *bufName;
   CHAR_T       *ptr;

   printf ("\n Hndl_ElementStart '%s'", name);
   
   /* Treatment for the GI */
   if (ParsingTextArea)
     {
       /* We are parsing the contents of a TEXTAREA element. If a start
	  tag appears, consider it as plain text */
     }
   else
     {
       /* XML syntax for empty elements <XX/> is automatically treated */

       /* look for a NS_SEP in the tag name (namespaces) and ignore the
	  prefix if there is one */
       buffer = TtaGetMemory ((strlen (name) + 1));
       ustrcpy (buffer, (CHAR_T*) name);

       if (ptr = ustrrchr (buffer, NS_SEP))
	 {
	   ptr++;
	   bufName = TtaGetMemory ((strlen (ptr) + 1));
	   ustrcpy (bufName, ptr);
	 }
       else
	 {
	   bufName = TtaGetMemory (strlen (buffer));
	   ustrcpy (bufName, buffer);
	 }

       /* Treatment for the beginning of start tag */
       ProcessStartGI (bufName);
   
       /* Treatment for the attributes */
       nbatts = XML_GetSpecifiedAttributeCount (parser);
   
       while (*attlist != NULL)
	 {
	   /* Create the corresponding Thot attribute */
	   bufAttr = TtaGetMemory ((strlen (*attlist)) + 1);
	   strcpy (bufAttr, *attlist);
	   printf ("\n  attr %s :", bufAttr);
	   EndOfAttrName (bufAttr);
	   TtaFreeMemory (bufAttr);
	   
	   /* Filling of the attribute value */
	   attlist++;
	   if (*attlist != NULL)
	     {
	       bufAttr = TtaGetMemory ((strlen (*attlist)) + 1);
	       strcpy (bufAttr, *attlist);
	       printf (" value=%s ", bufAttr);
	       EndOfAttrValue (bufAttr);
	       TtaFreeMemory (bufAttr);
	     }
	   attlist++;
	 }
   
       /* Treatment for the end of start tag */
       ProcessEndGI (bufName);

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
static void     Hndl_ElementEnd (userData,
				 name)
void            *userData; 
const XML_Char  *name
#endif  /* __STDC__ */

{
   CHAR_T       *bufName;
   CHAR_T       *buffer;
   CHAR_T       *ptr;

   printf ("\n Hndl_ElementEnd '%s'\n", name);

   /* look for a NS_SEP in the tag name (namespaces) and ignore the
      prefix if there is one */
   buffer = TtaGetMemory ((strlen (name) + 1));
   ustrcpy (buffer, (CHAR_T*) name);

   if (ptr = ustrrchr (buffer, NS_SEP))
     {
       ptr++;
       bufName = TtaGetMemory ((strlen (ptr) + 1));
       ustrcpy (bufName, ptr);
     }
   else
     {
       bufName = TtaGetMemory (strlen (buffer));
       ustrcpy (bufName, buffer);
     }

   EndOfElement ((CHAR_T*) (bufName));
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
  printf ("\n Hndl_ExternalEntityRef");
  printf ("\n   context  : %s", context);
  printf ("\n   base     : %s", base);
  printf ("\n   systemId : %s", systemId);
  printf ("\n   publicId : %s", publicId);
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
static void     Hndl_NameSpaceStart (userData,
				     prefix,
				     uri)
void            *userData; 
const XML_Char  *prefix;
const XML_Char  *uri;
#endif  /* __STDC__ */

{
  CHAR_T     NewSchemaName[MAX_SS_NAME_LENGTH];

  printf ("\n Hndl_NameSpaceStart");
  printf ("\n   prefix : %s; uri : %s", prefix, uri);

  SearchXmlParserContextUri ((CHAR_T*) uri, NewSchemaName);
  if (NewSchemaName != EOS)
    {
      ChangeXmlParserContext (NewSchemaName);
      NS_Level++;
      ustrcpy (NS_SchemaStack [NS_Level], NewSchemaName); 
    }
}

/*----------------------------------------------------------------------
   Hndl_NameSpaceEnd
   Handler for the end of namespace declarations
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     Hndl_NameSpaceEnd (void *userData,
				   const XML_Char *prefix)
#else  /* __STDC__ */
static void     Hndl_NameSpaceEnd (userData,
				   prefix)
void            *userData; 
const XML_Char  *prefix;
#endif  /* __STDC__ */

{
  printf ("\n Hndl_NameSpaceEnd");
  printf ("\n   prefix : %s", prefix);

  NS_Level--;
  if (NS_SchemaStack [NS_Level] != EOS)
      ChangeXmlParserContext (NS_SchemaStack [NS_Level]);
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
  printf ("\n Hndl_Notation");
  printf ("\n   notationName : %s", notationName);
  printf ("\n   base         : %s", base);
  printf ("\n   systemId     : %s", systemId);
  printf ("\n   publicId     : %s", publicId);
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
static void     Hndl_PI (userData,
			 target,
			 pidata)
void            *userData; 
const XML_Char  *target;
const XML_Char  *pidata;
#endif  /* __STDC__ */

{
  printf ("\n Hndl_PI %s", pidata);
  printf ("\n   target : %s", target);
  printf ("\n   pidata : %s", pidata);
  /* No treatment in Amaya for PI */
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
  printf ("\n Hndl_NotStandalone");
}

/*----------------------------------------------------------------------
   Hndl_UnknownEncoding
   Handler to deal with encodings other than the built in
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int     Hndl_UnknownEncoding (void *encodingData,
				     const XML_Char *name,
				     XML_Encoding *info)
#else  /* __STDC__ */
static int     Hndl_UnknownEncoding (userData,
				     data,
				     length)
void            *encodingData; 
const XML_Char  *name;
XML_Encoding    *info
#endif  /* __STDC__ */

{
  printf ("\n Hndl_UnknownEncoding");
  printf ("\n   name : %s", name);
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
  printf ("\n Hndl_UnparsedEntity");
  printf ("\n   entityName   : %s", entityName);
  printf ("\n   base         : %s", base);
  printf ("\n   systemId     : %s", systemId);
  printf ("\n   publicId     : %s", publicId);
  printf ("\n   notationName : %s", notationName);
}

/*------ End of Handler definition ------*/
#endif /* EXPAT */


/*----------------------------------------------------------------------
   HTMLparse
   Parse either the HTML file infile or the text  buffer HTMLbuf
   and build the equivalent Thot abstract tree.
   One parameter should be NULL.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void        XmlParse (XML_Parser *parser,
			     FILE *infile,
			     char *HTMLbuf)
#else
static void        XmlParse (parser,
			     infile,
			     HTMLbuf)
XML_Parser  *parser;
FILE        *infile;
char        *HTMLbuf;

#endif
{
   UCHAR_T      charRead; 
   ThotBool     match;
   ThotBool     endOfFile = FALSE;

#define	 COPY_BUFFER_SIZE	1024
  char          bufferRead[COPY_BUFFER_SIZE];
  int           res;


   if (infile != NULL)
     {
       endOfFile = FALSE;
     }

   XMLrootClosed = FALSE;


  /* read the XML file */

  while (!endOfFile && !XMLrootClosed)
    {
      res = gzread (infile, bufferRead, COPY_BUFFER_SIZE);

      if (res < COPY_BUFFER_SIZE)
	  endOfFile = TRUE;
      
      if (!XML_Parse (parser, bufferRead,
		      res, endOfFile))
	{
	  printf("\nError at line %d and column %d : %s",
		 XML_GetCurrentLineNumber (parser),
		 XML_GetCurrentColumnNumber (parser),
		 XML_ErrorString (XML_GetErrorCode (parser)));
	  endOfFile = TRUE;
	}
    }
  
  XMLrootClosingTag = NULL;
  XMLrootClosed = FALSE;

  if (ErrFile)
    {
      fclose (ErrFile);
      ErrFile = (FILE*) 0;
    } 
}

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
	TtaFreeMemory (ctxt);
	ctxt = nextCtxt;
      }
}


#ifdef EXPAT
/*----------------------------------------------------------------------
   FreeExpatParser
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         FreeExpatParser ()
#else  /* __STDC__ */
static void         FreeExpatParser ()
#endif  /* __STDC__ */

{  
  XML_ParserFree (parser);
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

  /* Construct a new parser */
  /*
  parser = XML_ParserCreate (NULL);
  */

  /* Construct a new parser with namespace processing */
  parser = XML_ParserCreateNS (NULL, NS_SEP);
 
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
#endif /* EXPAT */


/*----------------------------------------------------------------------
   InitializeXmlParser
   initializes variables and stack for parsing file
   the parser will insert the thot tree after or as a child
   of last elem, in the document doc.
   If last elem is NULL or doc=0, the parser doesn't initialize
   the stack
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         InitializeXmlParser (Element  lastelem,
					 ThotBool isclosed,
					 Document doc)
#else  /* __STDC__ */
static void         InitializeXmlParser (lastelem,
					 isclosed,
					 doc)
Element             lastelem;
ThotBool            isclosed;
Document            doc;
#endif  /* __STDC__ */
{
   CHAR_T           tag[20];
   Element          elem;
   int              i;
   SSchema          schema;

   stackLevel = 1;
   currentLanguage = TtaGetDefaultLanguage ();

   if (lastelem != NULL && doc != 0)
     {
	/* initialize the stack with ancestors of lastelem */
	currentDocument = doc;
	DocumentSSchema = TtaGetDocumentSSchema (currentDocument);
	rootElement = TtaGetMainRoot (currentDocument);
	if (isclosed)
	   elem = TtaGetParent (lastelem);
	else
	   elem = lastelem;

	while (elem != NULL && elem != rootElement)
	  {
	     ustrcpy (tag, GITagNameByType (TtaGetElementType (elem)));
	     if (ustrcmp (tag, TEXT("???")))
	       {
		  for (i = stackLevel; i > 0; i--)
		    {
		       nameElementStack[i + 1] = nameElementStack[i];
		       elementStack[i + 1] = elementStack[i];
		       languageStack[i + 1] = languageStack[i];
		       parserCtxtStack[i + 1] = parserCtxtStack[i];
		    }
		  schema = DocumentSSchema;
		  elementStack[1] = elem;
		  languageStack[1] = currentLanguage;
		  stackLevel++;
	       }
	     elem = TtaGetParent (elem);
	  }
	lastElement = lastelem;
	lastElementClosed = isclosed;
     }
   else
     {
	lastElement = rootElement;
	lastElementClosed = FALSE;
     }

   NumberOfCharRead = 0;
   NumberOfLinesRead = 1;

   /* input file is supposed to be XML */
   elementStack[0] = rootElement;

   /* initialize input buffer */
   lastAttribute = NULL;
   lastAttrElement = NULL;
   lastMappedAttr = NULL;
   UnknownAttr = FALSE;
   ReadingAnAttrValue = FALSE;
   MergeText = FALSE;
   AfterTagPRE = FALSE;
   ParsingCSS = FALSE;
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
			   ThotBool plainText)
#else
void        StartXmlParser (doc,
			    htmlFileName,
			    documentName,
			    documentDirectory,
			    pathURL,
			    plainText)
Document    doc;
CHAR_T*     htmlFileName;
CHAR_T*     documentName;
CHAR_T*     documentDirectory;
CHAR_T*     pathURL;
ThotBool    plainText;
#endif

{
  Element         el, oldel;
  AttributeType   attrType;
  Attribute       attr;
  CHAR_T*         s;
  CHAR_T          tempname[MAX_LENGTH];
  CHAR_T          temppath[MAX_LENGTH];
  int             length;
  ThotBool        isHTML;
  char            www_file_name[MAX_LENGTH];

  PtrParserCtxt	  oldParserCtxt;
  Document        oldDocument;
  Language        oldLanguage;
  Element         oldElement;
  ThotBool        oldElementClosed;
  Attribute	  oldAttribute;
  ThotBool        oldXMLrootClosed;
  STRING	  oldXMLrootClosingTag;
  int	          oldXMLrootLevel;
  ThotBool        oldlastTagRead;
  int	          oldStackLevel;

  currentDocument = doc;
  FirstElemToBeChecked = NULL;
  LastElemToBeChecked = NULL;
  lastElement = NULL;
  lastElementClosed = FALSE;
  lastAttribute = NULL;
  lastAttrElement = NULL;
  lastMappedAttr = NULL;
  UnknownAttr = FALSE;
  ReadingAnAttrValue = FALSE;
  CommentText = NULL;
  UnknownTag = FALSE;
  MergeText = FALSE;


  wc2iso_strcpy (www_file_name, htmlFileName);
  stream = gzopen (www_file_name, "r");

  if (stream != 0)
    {
      WithinTable = 0;
      if (documentName[0] == WC_EOS && !TtaCheckDirectory (documentDirectory))
	{
	  ustrcpy (documentName, documentDirectory);
	  documentDirectory[0] = WC_EOS;
	  s = TtaGetEnvString ("PWD");
	  /* set path on current directory */
	  if (s != NULL)
	    ustrcpy (documentDirectory, s);
	  else
	    documentDirectory[0] = WC_EOS;
	}
      TtaAppendDocumentPath (documentDirectory);

      /* the Thot document has been successfully created */
      {
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

	/* do not check the Thot abstract tree against the structure */
	/* schema while building the Thot document. */
	TtaSetStructureChecking (0, doc);
	/* set the notification mode for the new document */
	TtaSetNotificationMode (doc, 1);
	currentLanguage = TtaGetDefaultLanguage ();
	DocumentSSchema = TtaGetDocumentSSchema (doc);

	/* is the current document a HTML document */
	isHTML = (ustrcmp (TtaGetSSchemaName (DocumentSSchema),
			   TEXT("HTML")) == 0);	
	if (!isHTML)
	  {
	    /* change the document type */
	    TtaFreeView (doc, 1);
	    doc = TtaNewDocument (TEXT("HTML"), documentName);
	    if (TtaGetScreenDepth () > 1)
	        TtaSetPSchema (doc, TEXT("HTMLP"));
	    else
	        TtaSetPSchema (doc, TEXT("HTMLPBW"));
	    DocumentSSchema = TtaGetDocumentSSchema (doc);
	    isHTML = TRUE;
	  }
	
	LoadUserStyleSheet (doc);
	rootElement = TtaGetMainRoot (doc);
	/* add the default attribute PrintURL */
	attrType.AttrSSchema = DocumentSSchema;
	attrType.AttrTypeNum = HTML_ATTR_PrintURL;
	attr = TtaGetAttribute (rootElement, attrType);
	if (!attr)
	  {
	    attr = TtaNewAttribute (attrType);
	    TtaAttachAttribute (rootElement, attr, doc);
	  }
	
	TtaSetDisplayMode (doc, NoComputedDisplay);

	/* delete all element except the root element */
	el = TtaGetFirstChild (rootElement);
	while (el != NULL)
	  {
	    oldel = el;
	    TtaNextSibling (&el);
	    TtaDeleteTree (oldel, doc);
	  }
	
	/* save the path or URL of the document */
	TtaExtractName (pathURL, temppath, tempname);
	TtaSetDocumentDirectory (doc, temppath);
	/* disable auto save */
	TtaSetDocumentBackUpInterval (doc, 0);

	/* initialize all parser contexts if not done yet */
	if (firstParserCtxt == NULL)
	    InitXmlParserContexts ();
	ChangeXmlParserContext (TEXT("HTML"));
	/* initialize parsing environment */
	InitializeXmlParser (NULL, FALSE, 0);
	/* Specific initialization for expat */
	InitializeExpatParser ();
	
	/* parse the input file and build the Thot document */
	XmlParse (parser, stream, NULL);

	/* completes all unclosed elements */
	el = lastElement;
	while (el != NULL)
	  {
	    ElementComplete (el);
	    el = TtaGetParent (el);
	  }

	/* check the Thot abstract tree */
	CheckAbstractTree (pathURL);

	gzclose (stream);
	TtaFreeMemory (docURL);

	TtaSetDisplayMode (doc, DisplayImmediately);

	FreeExpatParser ();

	/* check the Thot abstract tree against the structure schema. */
	TtaSetStructureChecking (1, doc);
	DocumentSSchema = NULL;
      }
    }
   TtaSetDocumentUnmodified (doc);
   currentDocument = 0;
}

/* end of module */

