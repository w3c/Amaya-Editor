/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2001
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
	Entry:XMLparse
*/

#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"
#include "parser.h"
#include "fetchHTMLname_f.h"
 
/* an entity name */
typedef char entName[10];

/* an entity representing an ISO-Latin-1 character */
typedef struct _EntityDictEntry
  {		
     entName	charName;	/* entity name */
     int	charCode;	/* decimal code of the ISO-Latin1 char */
  }
EntityDictEntry;

/* XML predefined entities */
static EntityDictEntry XMLpredifinedEntities[] =
{
   /* This table MUST be in alphabetical order */
   {"amp", 38},
   {"apos", 39},
   {"gt", 62},
   {"lt", 60},
   {"quot", 34},

   {"zzzz", 0}		/* this last entry is required */
};

typedef int         state;	/* a state of the parser automaton */

/* maximum length of a Thot structure schema name */
#define MAX_SS_NAME_LENGTH 32

/* a parser context. It describes the specific actions to be executed
when parsing an XML document fragment according to a given DTD */

typedef struct _XMLparserContext *PtrParserCtxt;
typedef struct _XMLparserContext
  {
    PtrParserCtxt NextParserCtxt;	/* next parser context */
    char	  *SSchemaName;		/* name of Thot structure schema */
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

/* --------------------- static variables ------------------*/

/* information about XML languages */
/* All parser contexts describing known XML DTDs constitute a chain */
static PtrParserCtxt	firstParserCtxt = NULL;	/* first context in the chain*/
static PtrParserCtxt	currentParserCtxt = NULL;    /* current context */
static PtrParserCtxt    XLinkParserCtxt = NULL;      /* XLink parser context */

/* information about the input file */
static ThotBool     immAfterTag = FALSE;  /* A tag has just been read */

/* input buffer */
#define MAX_BUFFER_LENGTH 1000
#define ALLMOST_FULL_BUFFER 700
static unsigned char inputBuffer[MAX_BUFFER_LENGTH];
static int     bufferLength = 0;	  /* actual length of text in input
					     buffer */

/* information about the Thot document under construction */
static Document     currentDocument = 0;  /* the Thot document */
static Language     currentLanguage;	  /* language used in the document */
static Element	    currentElement = NULL;
static ThotBool	    currentElementClosed = FALSE;
static char	    currentElementContent = ' ';
static Attribute    currentAttribute = NULL;
static ThotBool	    HTMLStyleAttribute = FALSE;
static ThotBool	    XMLrootClosed = FALSE;
static int	    XMLrootLevel = 0;
static ThotBool     XMLabort = FALSE;
static ThotBool     XlinkAttribute = FALSE;

/* parser stack */
#define MAX_STACK_HEIGHT 200		  /* maximum stack height */
static unsigned char *XMLelementType[MAX_STACK_HEIGHT]; /* XML element name */
static Element        elementStack[MAX_STACK_HEIGHT];  /* element in the Thot
                                             abstract tree */
static Language       languageStack[MAX_STACK_HEIGHT]; /* element language */
static PtrParserCtxt  parserCtxtStack[MAX_STACK_HEIGHT];
static int            stackLevel = 0;       /* first free element on the stack */

/* parser automaton */
static ThotBool	    XMLautomatonInitalized = FALSE;
static state        currentState;	  /* current state of the automaton */
static state        returnState;	  /* return state from subautomaton */
static ThotBool     normalTransition;

/* information about an entity being read */
#define MAX_ENTITY_LENGTH 80
static char       entityName[MAX_ENTITY_LENGTH]; /* name of entity being
					     read */
static int          entityNameLength = 0; /* length of entity name read so
					     far */

static Element	    commentText = NULL;	  /* Text element representing the
					     contents of the current comment */

#include "MathMLbuilder_f.h"
#ifdef _SVG
#include "SVGbuilder_f.h"
#endif
#include "XLinkbuilder_f.h"
#include "fetchXMLname_f.h"
#include "html2thot_f.h"
#include "styleparser_f.h"


/*----------------------------------------------------------------------
   InitParserContexts
   Create the chain of parser contexts decribing all recognized XML DTDs
  ----------------------------------------------------------------------*/
static void            InitParserContexts (void)
{
   PtrParserCtxt	ctxt, prevCtxt;

   firstParserCtxt = NULL;
   prevCtxt = NULL;
   ctxt = NULL;

   /* create and initialize a context for the MathML parser */
   ctxt = TtaGetMemory (sizeof (XMLparserContext));
   firstParserCtxt = ctxt;
   ctxt->NextParserCtxt = NULL;	/* last context */
   ctxt->SSchemaName = TtaGetMemory (MAX_SS_NAME_LENGTH);
   strcpy (ctxt->SSchemaName, "MathML");
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
#ifdef _SVG
   /* create and initialize a context for the SVG parser */
   ctxt = TtaGetMemory (sizeof (XMLparserContext));
   prevCtxt->NextParserCtxt = ctxt;
   ctxt->NextParserCtxt = NULL;	/* last context */
   ctxt->SSchemaName = TtaGetMemory (MAX_SS_NAME_LENGTH);
   strcpy (ctxt->SSchemaName, "SVG");
   ctxt->XMLSSchema = NULL;
   ctxt->XMLtype = GRAPH_TYPE;
   ctxt->MapAttribute = (Proc) MapSVGAttribute;
   ctxt->MapAttributeValue = (Proc) MapSVGAttributeValue;
   ctxt->MapEntity = (Proc) MapSVGEntity;
   ctxt->EntityCreated = (Proc) SVGEntityCreated;
   ctxt->ElementComplete = (Proc) SVGElementComplete;
   ctxt->AttributeComplete = (Proc) SVGAttributeComplete;
   ctxt->GetDTDName = (Proc) SVGGetDTDName;
   prevCtxt = ctxt;
#endif /* _SVG */
   /* create and initialize a context for the XLink parser */
   ctxt = TtaGetMemory (sizeof (XMLparserContext));
   prevCtxt->NextParserCtxt = ctxt;
   ctxt->NextParserCtxt = NULL;	/* last context */
   ctxt->SSchemaName = TtaGetMemory (MAX_SS_NAME_LENGTH);
   strcpy (ctxt->SSchemaName, "XLink");
   ctxt->XMLSSchema = NULL;
   ctxt->XMLtype = XLINK_TYPE;
   ctxt->MapAttribute = (Proc) MapXLinkAttribute;
   ctxt->MapAttributeValue = (Proc) MapXLinkAttributeValue;
   ctxt->MapEntity = NULL;
   ctxt->EntityCreated = NULL;
   ctxt->ElementComplete = NULL;
   ctxt->AttributeComplete = (Proc) XLinkAttributeComplete;
   ctxt->GetDTDName = NULL;
   XLinkParserCtxt = ctxt;
   prevCtxt = ctxt;
}

/*----------------------------------------------------------------------
   XMLInsertElement
  ----------------------------------------------------------------------*/
void XMLInsertElement (Element el)
{
  if (currentElementClosed)
     TtaInsertSibling (el, currentElement, FALSE, currentDocument);
  else
    {
     TtaInsertFirstChild (&el, currentElement, currentDocument);
     currentElementClosed = TRUE;
    }
  currentElement = el;
}

/*----------------------------------------------------------------------
   XMLElementComplete
  ----------------------------------------------------------------------*/
void XMLElementComplete (Element el, Document doc)
{
   PtrParserCtxt	ctxt;
   ElementType		elType;
   int                  error;

   elType = TtaGetElementType (el);
   if (currentParserCtxt)
      {
      if (currentParserCtxt->ElementComplete)
         (*(currentParserCtxt->ElementComplete)) (el, doc, &error);
      }
   else
      {
      /* initialize all parser contexts if not done yet */
      if (firstParserCtxt == NULL)
         InitParserContexts ();

      ctxt = firstParserCtxt;
      while (ctxt != NULL &&
	    strcmp (ctxt->SSchemaName, TtaGetSSchemaName (elType.ElSSchema)))
	 ctxt = ctxt->NextParserCtxt;
      if (ctxt != NULL)
	 if (ctxt->ElementComplete)
	    (*(ctxt->ElementComplete)) (el, doc, &error);
      }
}

/*----------------------------------------------------------------------
   CreateTextElement
  ----------------------------------------------------------------------*/
static void CreateTextElement (char *text, Language lang)
{
  Element	newElement;
  ElementType	elType;

  elType = TtaGetElementType (currentElement);
  elType.ElTypeNum = 1;	/* Text element */
  newElement = TtaNewElement (currentDocument, elType);
  SetElemLineNumber (newElement);
  XMLInsertElement (newElement);
  TtaSetTextContent (newElement, text, lang, currentDocument);
  currentElementClosed = TRUE;
  XMLElementComplete (currentElement, currentDocument);
}


/*----------------------------------------------------------------------
   XMLTextToDocument
   Put the contents of input buffer in the Thot document.
  ----------------------------------------------------------------------*/
void         XMLTextToDocument ()

{
  int		i, firstChar, lastChar;

  /* close the input buffer */
  inputBuffer[bufferLength] = EOS;

  /* suppress leading spaces */
  for (firstChar = 0;
       inputBuffer[firstChar] <= SPACE &&
				inputBuffer[firstChar] != EOS;
       firstChar++);
  if (inputBuffer[firstChar] != EOS)
     {
     /* suppress trailing spaces */
     lastChar = strlen (inputBuffer) - 1;
     for (i = lastChar; inputBuffer[i] <= SPACE && i > 0; i--);     
     inputBuffer[i+1] = EOS;
     CreateTextElement (&(inputBuffer[firstChar]), currentLanguage);
     }
 
   /* the input buffer is now empty */
   bufferLength = 0;
}

/*----------------------------------------------------------------------
   StartOfTag
   Beginning of a XML tag (start or end tag).
   Put the preceding text in the Thot document.
  ----------------------------------------------------------------------*/
static void         StartOfTag (char c)
{
   if (bufferLength > 0)
      XMLTextToDocument (); 
}

/*----------------------------------------------------------------------
   PutInBuffer
   Put character c in the input buffer.
  ----------------------------------------------------------------------*/
static void         PutInBuffer (unsigned char c)
{
  int                 len;

  /* put the character into the buffer if it is not an ignored char. */
  if ((int) c == 9)		/* HT */
    len = 8;			/* HT = 8 spaces */
  else
    len = 1;
  if (c != EOS)
    {
      if (bufferLength + len >= ALLMOST_FULL_BUFFER && currentState == 0)
	XMLTextToDocument ();
      if (bufferLength + len >= MAX_BUFFER_LENGTH)
	{
	  if (currentState == 0)
	    XMLTextToDocument ();
	  bufferLength = 0;
	}
      if (len == 1)
	inputBuffer[bufferLength++] = c;
      else
	/* HT */
	do
	  {
	    inputBuffer[bufferLength++] = SPACE;
	    len--;
	  }
	while (len > 0);
    }
}

/*----------------------------------------------------------------------
   ChangeParserContext
   Get the parser context correponding to a given DTD
  ----------------------------------------------------------------------*/
static void ChangeParserContext (char *DTDname)
{
  currentParserCtxt = firstParserCtxt;
  while (currentParserCtxt != NULL &&
	 strcmp (DTDname, currentParserCtxt->SSchemaName))
     currentParserCtxt = currentParserCtxt->NextParserCtxt;

  /* initialize the corresponding entry */
  if (currentParserCtxt != NULL && currentParserCtxt->XMLSSchema == NULL)
     currentParserCtxt->XMLSSchema = GetXMLSSchema (currentParserCtxt->XMLtype,
						    currentDocument);
}


/*----------------------------------------------------------------------
   GetXMLElementType
   Search in the mapping tables the entry for the element type of
   name XMLname and returns the corresponding Thot element type.
   Returns -1 and schema = NULL if not found.
  ----------------------------------------------------------------------*/
void GetXMLElementType (char *XMLname, ElementType *elType, char **mappedName,
			char *content, Document doc)
{
  PtrParserCtxt	ctxt;
  ThotBool      level;

  /* initialize all parser contexts if not done yet */
  if (firstParserCtxt == NULL)
    InitParserContexts ();

  /* Look at the current context if there is one */
  if (currentParserCtxt != NULL)
    {
      /* by default we're looking at in the current schema */
      elType->ElSSchema = currentParserCtxt->XMLSSchema;
      MapXMLElementType (currentParserCtxt->XMLtype, XMLname, elType,
			 mappedName, content, &level, doc);
    }
  else if (elType->ElSSchema != NULL)
    {
      /* The schema is known -> search the corresponding context */
      ctxt = firstParserCtxt;
      while (ctxt != NULL &&
	     strcmp (TtaGetSSchemaName(elType->ElSSchema), ctxt->SSchemaName))
	  ctxt = ctxt->NextParserCtxt;
      /* get the Thot element number */
      if (ctxt != NULL)
	MapXMLElementType (ctxt->XMLtype, XMLname, elType, mappedName,
			   content, &level, doc);
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
	      MapXMLElementType (ctxt->XMLtype, XMLname, elType, mappedName,
				 content, &level, doc);
	      if (elType->ElSSchema != NULL)
		ctxt->XMLSSchema = elType->ElSSchema;
	    }
	  ctxt = ctxt->NextParserCtxt;
	}
    }
}

/*----------------------------------------------------------------------
   EndOfXMLEndTag
   The end ('>') of an end tag has been read.
   Close the corresponding Thot element.
  ----------------------------------------------------------------------*/
static void         EndOfXMLEndTag (char c)
{
  stackLevel--;
  if (stackLevel >= 0)
    currentLanguage = languageStack[stackLevel];
  if (elementStack[stackLevel] != NULL)
    {
      /* restore the parser context */
      currentParserCtxt = parserCtxtStack[stackLevel];
      currentElement = elementStack[stackLevel];
      currentElementClosed = TRUE;
      XMLElementComplete (currentElement, currentDocument);
    }
  if (stackLevel == XMLrootLevel)
    /* end of the XML object. Return to the calling parser */
    XMLrootClosed = TRUE;
  immAfterTag = TRUE;
}

/*----------------------------------------------------------------------
   EndOfStartTag
   A ">" or a "/" has been read. It indicates the end of a start tag.
  ----------------------------------------------------------------------*/
static void EndOfStartTag (char c)
{
  char	          DTDname[100];

  if (currentElementContent == 'X' && c != '/')
     /* the current element will contain elements from another DTD */
     {
     /* ask the concerned builder about the DTD to be used */
     DTDname[0] = EOS;
     if (XMLelementType[stackLevel-1] != NULL)
        if (currentParserCtxt->GetDTDName)
           (*(currentParserCtxt->GetDTDName)) (DTDname,
					       XMLelementType[stackLevel-1]);
     if (strcmp (DTDname, "HTML") == 0)
	{
	ParseIncludedHTML (currentElement, XMLelementType[stackLevel-1]);
	/* when returning from the HTML parser, the end tag has already
           been read */
	EndOfXMLEndTag ('>');
	}
     else
	{
        if (DTDname[0] != EOS)
           /* Parse the content of this element according to the new DTD */
           ChangeParserContext (DTDname);
	}
     }
  if (c == '/')
     /* this is an empty element. Do not expect an end tag */
     {
     stackLevel--;
     if (stackLevel > 0)
        currentLanguage = languageStack[stackLevel - 1];
     /* restore the parser context */
     currentParserCtxt = parserCtxtStack[stackLevel];
     if (elementStack[stackLevel] != NULL)
        {
        currentElement = elementStack[stackLevel];
        currentElementClosed = TRUE;
        XMLElementComplete (currentElement, currentDocument);
        }
     }
  else if (c == '>')
     immAfterTag = TRUE;
  currentAttribute = NULL;
  HTMLStyleAttribute = FALSE;
}

/*----------------------------------------------------------------------
    EndOfEmptyTag
    A ">" after a "/" has been read. It indicates the end of a empty tag.
  ----------------------------------------------------------------------*/
static void         EndOfEmptyTag (char c)
{
  immAfterTag = TRUE;
}

/*----------------------------------------------------------------------
   EndOfStartGI
   The name of an element type has been read in a start tag.
   Create the corresponding Thot element.
  ----------------------------------------------------------------------*/
static void EndOfStartGI (char c)
{
  Element		newElement;
  ElementType		elType;
  char                 *mappedName;
  unsigned char         msgBuffer[MAX_BUFFER_LENGTH];
  int			i;

  /* close the input buffer */
  inputBuffer[bufferLength] = EOS;
  if (currentElement != NULL)
    {
      if (stackLevel == MAX_STACK_HEIGHT)
	{
	  normalTransition = FALSE;
	  XMLabort = TRUE;
	}
      else
	{
	  /* look for a colon in the element name (namespaces) and ignore the
	     prefix if there is one */
	  for (i = 0; i < bufferLength && inputBuffer[i] != ':'; i++);
	  if (inputBuffer[i] == ':')
	    i++;
	  else
	    i = 0;
	  elType.ElSSchema = NULL;
	  elType.ElTypeNum = 0;
	  GetXMLElementType (&inputBuffer[i], &elType, &mappedName,
			     &currentElementContent, currentDocument);
	  if (elType.ElTypeNum <= 0)
	    {
	      sprintf (msgBuffer, "Unknown XML element %s", inputBuffer);
	      HTMLParseError (currentDocument, msgBuffer);
	      XMLelementType[stackLevel] = NULL;
	      elementStack[stackLevel] = NULL;
	    }
	  else
	    {
	      newElement = TtaNewElement (currentDocument, elType);
	      SetElemLineNumber (newElement);
	      XMLInsertElement (newElement);
	      currentElementClosed = FALSE;
	      XMLelementType[stackLevel] = mappedName;
	      elementStack[stackLevel] = newElement;
	    }
	  currentAttribute = NULL;
	  HTMLStyleAttribute = FALSE;
	  languageStack[stackLevel] = currentLanguage;
	  /* save the current parser context */
	  parserCtxtStack[stackLevel] = currentParserCtxt;
	  stackLevel++;
	}
    }
  
  /* the input buffer is now empty */
  bufferLength = 0;
}

/*----------------------------------------------------------------------
   EndOfStartGIandTag
   A ">" has been read. It indicates the end of an element name and the
   end of a start tag.
  ----------------------------------------------------------------------*/
static void         EndOfStartGIandTag (char c)
{
   EndOfStartGI (c);
   EndOfStartTag (c);
}

/*----------------------------------------------------------------------
   EndOfClosingTagName
   An element name has been read in a closing tag.
   Check that it closes the right element.
  ----------------------------------------------------------------------*/
static void EndOfClosingTagName (char c)
{
  int		    i;
  unsigned char             msgBuffer[MAX_BUFFER_LENGTH];

  /* close the input buffer */
  inputBuffer[bufferLength] = EOS;

  /* look for a colon in the element name (namespaces) and ignore the
     prefix if there is one */
  for (i = 0; i < bufferLength && inputBuffer[i] != ':'; i++);
  if (inputBuffer[i] == ':')
     i++;
  else
     i = 0;

  if (XMLelementType[stackLevel - 1] != NULL)
    /* the corresponding opening tag was a known tag */
    if (strcmp(&inputBuffer[i], XMLelementType[stackLevel - 1]) != 0)
      /* the end tag does not close the current element */
      {
	/* print an error message */
	sprintf (msgBuffer,
		  "Unexpected XML end tag </%s> instead of </%s>",
		  inputBuffer, XMLelementType[stackLevel - 1]);
	HTMLParseError (currentDocument, msgBuffer);
	normalTransition = FALSE;
	XMLabort = TRUE;
      }
  
  /* the input buffer is now empty */
  bufferLength = 0;
}

/*----------------------------------------------------------------------
   EndOfNameAndClosingTag
   An element name followed by a '>' has been read in a closing tag.
  ----------------------------------------------------------------------*/
static void EndOfNameAndClosingTag (char c)
{
   EndOfClosingTagName (c);
   EndOfXMLEndTag (c);
}

/*----------------------------------------------------------------------
   EndOfAttrName
   An XML attribute name has been read.
   Create the corresponding Thot attribute.
  ----------------------------------------------------------------------*/
static void         EndOfAttrName (char c)
{
  Attribute	attr;
  AttributeType	attrType;
  int		i;
  ThotBool      level;

  /* close the input buffer */
  inputBuffer[bufferLength] = EOS;

  currentAttribute = NULL;
  HTMLStyleAttribute = FALSE;
  if (XMLelementType[stackLevel-1] != NULL)
     {
     attrType.AttrTypeNum = 0;
     i = 0;
     if (strncmp (inputBuffer, "xml:", 4) == 0)
        /* special xml attributes */
        {
        if (currentParserCtxt->MapAttribute)
           (*(currentParserCtxt->MapAttribute)) (&inputBuffer[4], &attrType,
						 XMLelementType[stackLevel-1],
						 &level, currentDocument);
	}
     else
	{
        /* look for a colon in the attribute name and ignore the namespace
	   prefix that precedes if there is one */
        for (i = 0; i < bufferLength && inputBuffer[i] != ':'; i++);
        if (inputBuffer[i] == ':')
	   /* there is a namespace prefix */
	   {
	   inputBuffer[i] = EOS;
	   i++;
	   /****** this is wrong: the prefix for the XLink namespace may be
	      anything else. This is a trick, waiting for a full
	      implementation of namespaces in this XML parser */
	   if (strcmp(&inputBuffer[0], "xlink") == 0)
	      /* this attribute is in the XLink namespace */
	      {
	      XlinkAttribute = TRUE;
              (*(XLinkParserCtxt->MapAttribute)) (&inputBuffer[i], &attrType,
						  XMLelementType[stackLevel-1],
						  &level, currentDocument);
	      }
	   else
	      /* this attribute is supposed to be in the current namespace */
	      if (currentParserCtxt->MapAttribute)
                 (*(currentParserCtxt->MapAttribute)) (&inputBuffer[i],
						  &attrType,
						  XMLelementType[stackLevel-1],
						  &level, currentDocument);
	   }
        else
	   {
	   i = 0;
	   if (currentParserCtxt->MapAttribute)
              (*(currentParserCtxt->MapAttribute)) (&inputBuffer[i], &attrType,
					          XMLelementType[stackLevel-1],
					          &level, currentDocument);
	   }
	}
     if (attrType.AttrTypeNum <= 0)
        /* not found. Is it a HTML attribute (style, class, id for instance) */
	MapHTMLAttribute (&inputBuffer[i], &attrType,
			  XMLelementType[stackLevel-1],
			  &level, currentDocument);
     if (attrType.AttrTypeNum > 0)
        {
	if (strcasecmp (&inputBuffer[i], "style") == 0)
	   HTMLStyleAttribute = TRUE;
        attr = TtaGetAttribute (currentElement, attrType);
        if (!attr)
	   {
	   attr = TtaNewAttribute (attrType);
	   TtaAttachAttribute (currentElement, attr, currentDocument);
	   }
	currentAttribute = attr;
        }
     }
 
  /* the input buffer is now empty */
  bufferLength = 0;
}

/*----------------------------------------------------------------------
   EndOfAttrNameAndTag
   A ">" or a "/" (XML) has been read. It indicates the end of an attribute
   name and the end of a start tag.
  ----------------------------------------------------------------------*/
static void         EndOfAttrNameAndTag (char c)
{
   EndOfAttrName (c);
   EndOfStartTag (c);
}

/*----------------------------------------------------------------------
   EndOfAttrValue
   An attribute value has been read.
   Put that value in the current Thot attribute.
  ----------------------------------------------------------------------*/
static void         EndOfAttrValue (char c)
{
   AttributeType	attrType;
   int			attrKind, val;

   /* close the input buffer */
   inputBuffer[bufferLength] = EOS;

   /* inputBuffer contains the attribute value */
   if (currentAttribute != NULL)
     {
     TtaGiveAttributeType (currentAttribute, &attrType, &attrKind);
     switch (attrKind)
	{
	case 0:       /* enumerate */
	   val = 0;
	   if (XlinkAttribute)
	         (*(XLinkParserCtxt->MapAttributeValue)) (inputBuffer,
							  attrType, &val);
	   else
	      if (currentParserCtxt->MapAttributeValue)
	         (*(currentParserCtxt->MapAttributeValue)) (inputBuffer,
							    attrType, &val);
	   if (val > 0)
	      TtaSetAttributeValue (currentAttribute, val, currentElement,
				    currentDocument);
	   break;

	case 1:       /* integer */
	   sscanf (inputBuffer, "%d", &val);
	   TtaSetAttributeValue (currentAttribute, val, currentElement,
				 currentDocument);
	   break;

	case 2:       /* text */
	   TtaSetAttributeText (currentAttribute, inputBuffer, currentElement,
				currentDocument);
	   if (HTMLStyleAttribute)
	      ParseHTMLSpecificStyle (currentElement, inputBuffer,
				      currentDocument, 1, FALSE);
	   break;

	case 3:       /* reference */
	   break;
	}
     if (currentParserCtxt != NULL && !HTMLStyleAttribute)
        if (currentParserCtxt->AttributeComplete)
           (*(currentParserCtxt->AttributeComplete)) (currentAttribute,
					      currentElement, currentDocument);
     }

   HTMLStyleAttribute = FALSE;
   currentAttribute = NULL;
   XlinkAttribute = FALSE;

   /* the input buffer is now empty */
   bufferLength = 0;
}

/*----------------------------------------------------------------------
   StartOfEntity
   A character '&' has been encountered.
  ----------------------------------------------------------------------*/
static void         StartOfEntity (char c)
{
   entityNameLength = 0;
}

/*----------------------------------------------------------------------
   EndOfEntity
   End of an XML entity name. Search that entity in the entity tables
   and put the corresponding content in the input buffer.
  ----------------------------------------------------------------------*/
static void         EndOfEntity (char c)
{
   int		i;
   unsigned char	entityValue[MAX_ENTITY_LENGTH];	
   char	alphabet;
   Language	lang;

   entityName[entityNameLength] = EOS;

   /* First, look in the predifined entities table */
   for (i = 0; XMLpredifinedEntities[i].charCode > 0 &&
	       strcmp (XMLpredifinedEntities[i].charName, entityName);
	       i++);
   if (!strcmp (XMLpredifinedEntities[i].charName, entityName))
      /* entity found in the predifined table */
      PutInBuffer ((char) (XMLpredifinedEntities[i].charCode));
   else
      /* entity not in the predifined table */
      {
      /* look in the entity table for the current DTD */
      alphabet = EOS;
      if (currentParserCtxt->MapEntity)
         (*(currentParserCtxt->MapEntity)) (entityName, entityValue,
					    &alphabet);
      lang = 0;
      if (alphabet == EOS)
	 /* Unknown entity */
	 {
         entityValue[0] = EOS;
	 lang = -1;
	 }
      else
	 if (entityValue[0] != EOS)
	   lang = TtaGetLanguageIdFromAlphabet(alphabet);
      if (currentAttribute)
	/* entity in an attribute value */
	{
	for (i = 0; entityValue[i] != EOS; i++)
	  PutInBuffer (entityValue[i]);
	}
      else
	/* entity in an element */
	if (currentParserCtxt->EntityCreated)
	   (*(currentParserCtxt->EntityCreated)) (entityValue, lang,
						  entityName, currentDocument);
      }
   entityNameLength = 0;
}

/*----------------------------------------------------------------------
   EntityChar
   A character belonging to an XML entity has been read.
  ----------------------------------------------------------------------*/
static void EntityChar (unsigned char c)
{
   int                 i;

   if (entityNameLength < MAX_ENTITY_LENGTH - 1)
      entityName[entityNameLength++] = c;
   else
      {
      /* entity too long */
      /* consider the entity name read so far as ordinary text */
      PutInBuffer ('&');
      for (i = 0; i < entityNameLength; i++)
	  PutInBuffer (entityName[i]);
      /* next state is the return state from the entity subautomaton, not the
	state computed by the automaton.
	In addition the character read has not been processed yet */
      normalTransition = FALSE;
      currentState = returnState;
      /* end of entity */
      entityNameLength = 0;      
      }
}

/*----------------------------------------------------------------------
   EndOfDecEntity
   End of a decimal entity.
   Convert the string read into a number and put the character
   having that code in the input buffer.
  ----------------------------------------------------------------------*/
static void         EndOfDecEntity (char c)
{
   int                 code;
   char              buffer[MAX_ENTITY_LENGTH+2];
   unsigned char	       entityValue[MAX_ENTITY_LENGTH];	

   entityName[entityNameLength] = EOS;
   sscanf (entityName, "%d", &code);
   if (code < 255)
      /* that's an ISO Latin character */
      PutInBuffer ((char) code);
   else
      /* code of a Unicode character */
      {
      strcpy (buffer, "#");
      strcat (buffer, entityName);
      entityValue[0] = EOS;
      if (currentAttribute)
	/* entity in an attribute value */
	PutInBuffer ('?');
      else
	/* entity in an element */
	if (currentParserCtxt->EntityCreated)
	   (*(currentParserCtxt->EntityCreated)) (entityValue, -1, buffer,
						  currentDocument);
      }
   entityNameLength = 0;
}

/*----------------------------------------------------------------------
   DecEntityChar
   A character belonging to a decimal entity has been read.
   Put that character in the entity buffer.
  ----------------------------------------------------------------------*/
static void DecEntityChar (char c)
{
   int		i;

   if (entityNameLength < MAX_ENTITY_LENGTH - 1)
     {
      /* the entity buffer is not full */
      if (c >= '0' && c <= '9')
	 /* the character is a decimal digit */
	 entityName[entityNameLength++] = c;
      else
	 /* ERROR: not a decimal digit. */
	 {
	 PutInBuffer ('&');
	 PutInBuffer ('#');
	 for (i = 0; i < entityNameLength; i++)
	     PutInBuffer (entityName[i]);
	 entityNameLength = 0;
	 /* next state is state 0, not the state computed by the automaton */
	 /* and the character read has not been processed yet */
	 normalTransition = FALSE;
	 currentState = 0;
	 /* error message */
	 HTMLParseError (currentDocument, "Invalid decimal entity");
	 normalTransition = FALSE;
	 XMLabort = TRUE;
	 }
     }
}

/*----------------------------------------------------------------------
   EndOfHexEntity
   End of a decimal entity.
   Convert the string read into a number and put the character
   having that code in the input buffer.
  ----------------------------------------------------------------------*/
static void         EndOfHexEntity (char c)
{
   int                 code;
   char              buffer[MAX_ENTITY_LENGTH+2];
   unsigned char	       entityValue[MAX_ENTITY_LENGTH];	

   entityName[entityNameLength] = EOS;
   sscanf (entityName, "%x", &code);
   if (code < 255)
      /* that's an ISO Latin character */
      PutInBuffer ((char) code);
   else
      /* code of a Unicode character */
      {
      strcpy (buffer, "#x");
      strcat (buffer, entityName);
      entityValue[0] = EOS;
      if (currentAttribute)
	/* entity in an attribute value */
	PutInBuffer ('?');
      else
	/* entity in an element */
	if (currentParserCtxt->EntityCreated)
	   (*(currentParserCtxt->EntityCreated)) (entityValue, -1, buffer,
						  currentDocument);
      }
   entityNameLength = 0;
}

/*----------------------------------------------------------------------
   HexEntityChar
   A character belonging to a decimal entity has been read.
   Put that character in the entity buffer.
  ----------------------------------------------------------------------*/
static void         HexEntityChar (char c)
{
   int		i;

   if (entityNameLength < MAX_ENTITY_LENGTH - 1)
     {
      /* the entity buffer is not full */
      if ((c >= '0' && c <= '9') ||
          (c >= 'a' && c <= 'f') ||
          (c >= 'A' && c <= 'F'))
         /* the character is a valid hexadecimal digit */
	 entityName[entityNameLength++] = c;
      else
	 /* ERROR: not an hexadecimal digit */
	 {
	 PutInBuffer ('&');
	 PutInBuffer ('#');
	 PutInBuffer ('x');
	 for (i = 0; i < entityNameLength; i++)
	     PutInBuffer (entityName[i]);
	 entityNameLength = 0;
	 /* next state is state 0, not the state computed by the automaton */
	 /* and the character read has not been processed yet */
	 normalTransition = FALSE;
	 currentState = 0;
	 /* error message */
	 HTMLParseError (currentDocument, "Invalid hexadecimal entity");
	 normalTransition = FALSE;
	 XMLabort = TRUE;
	 }
     }
}

/*----------------------------------------------------------------------
   XMLerror
   The character following '/' in a start tag is not '>'.
  ----------------------------------------------------------------------*/
static void         XMLerror (char c)
{
   HTMLParseError (currentDocument, "Invalid XML syntax");
   normalTransition = FALSE;
   XMLabort = TRUE;

}

/*----------------------------------------------------------------------
   PutAmpersandSpace
   Put '& ' in the input buffer.
  ----------------------------------------------------------------------*/
static void         PutAmpersandSpace (char c)
{
   PutInBuffer ('&');
   PutInBuffer (SPACE);
   XMLerror (c);
}

/*----------------------------------------------------------------------
   StartOfComment
   Beginning of an XML comment.
  ----------------------------------------------------------------------*/
static void         StartOfComment (char c)
{
   ElementType	 elType;
   Element	 commentEl, commentLineEl;
   char          *mappedName;
   char          cont;
   unsigned char msgBuffer[MAX_BUFFER_LENGTH];

   /* create a Thot element for the comment */
   elType.ElSSchema = NULL;
   elType.ElTypeNum = 0;
   GetXMLElementType ("XMLcomment", &elType, &mappedName, &cont,
		      currentDocument);
   if (elType.ElTypeNum <= 0)
     {
       sprintf (msgBuffer, "Unknown XML element %s", inputBuffer);
       HTMLParseError (currentDocument, msgBuffer);
     }
   else
     {
       commentEl = TtaNewElement (currentDocument, elType);
       SetElemLineNumber (commentEl);
       XMLInsertElement (commentEl);
       /* create a XMLcomment_line element as the first child of */
       /* element XMLcomment */
       elType.ElSSchema = NULL;
       elType.ElTypeNum = 0;
       GetXMLElementType ("XMLcomment_line", &elType, &mappedName,
			  &cont, currentDocument);
       commentLineEl = TtaNewElement (currentDocument, elType);
       SetElemLineNumber (commentLineEl);
       TtaInsertFirstChild (&commentLineEl, commentEl, currentDocument);
       /* create a TEXT element as the first child of element XMLcomment_line*/
       elType.ElTypeNum = 1;
       commentText = TtaNewElement (currentDocument, elType);
       SetElemLineNumber (commentText);
       TtaInsertFirstChild (&commentText, commentLineEl, currentDocument);
       TtaSetTextContent (commentText, "", currentLanguage,
			  currentDocument);
     }
   /* the input buffer is now empty */
   bufferLength = 0;
}

/*----------------------------------------------------------------------
   PutInComment
   Put character c in the current XML comment.
  ----------------------------------------------------------------------*/
static void PutInComment (unsigned char c)
{
   ElementType	elType;
   Element	commentLineEl;
   char        *mappedName;
   char	        cont;

   if (c != EOS)
     {
     if ((int) c == EOL || (int) c == __CR__)
       /* new line in a comment */
       {
	 /* put the content of inputBuffer into the current XMLcomment_line */
	 inputBuffer[bufferLength] = EOS;
	 TtaAppendTextContent (commentText, inputBuffer, currentDocument);
	 bufferLength = 0;
	 /* create a new XMLcomment_line element */
	 elType.ElSSchema = NULL;
	 elType.ElTypeNum = 0;
	 GetXMLElementType ("XMLcomment_line", &elType, &mappedName,
			    &cont, currentDocument);
	 commentLineEl = TtaNewElement (currentDocument, elType);
         SetElemLineNumber (commentLineEl);
	 /* inserts the new XMLcomment_line after the previous one */
	 TtaInsertSibling (commentLineEl, TtaGetParent (commentText), FALSE,
			   currentDocument);
	 /* create a TEXT element as the first child of element
	    XMLcomment_line */
	 elType.ElTypeNum = 1;
	 commentText = TtaNewElement (currentDocument, elType);
         SetElemLineNumber (commentText);
	 TtaInsertFirstChild (&commentText, commentLineEl, currentDocument);
	 TtaSetTextContent (commentText, "", currentLanguage,
			    currentDocument);
       }
     else
       {
         if (bufferLength >= ALLMOST_FULL_BUFFER)
	   {
	     /* close the input buffer */
	     inputBuffer[bufferLength] = EOS;
	     TtaAppendTextContent (commentText, inputBuffer, currentDocument);
	     bufferLength = 0;
	   }
         inputBuffer[bufferLength++] = c;
       }
     }
}
 
/*----------------------------------------------------------------------
   EndOfComment
   End of an XML comment.
  ----------------------------------------------------------------------*/
static void         EndOfComment (char c)
{
   if (bufferLength > 0)
      {
      inputBuffer[bufferLength] = EOS;
      TtaAppendTextContent (commentText, inputBuffer, currentDocument);
      }
   /* the input buffer is now empty */
   bufferLength = 0;
   commentText = NULL;
   XMLElementComplete (currentElement, currentDocument);
   currentElementClosed = TRUE;
}

/*----------------------------------------------------------------------
   PutDash
   Put a dash character in the current comment.
  ----------------------------------------------------------------------*/
static void         PutDash (char c)
{
   PutInComment ('-');
   PutInComment (c);
}

/*----------------------------------------------------------------------
   PutQuestionMark put a question mark in the current PI.
  ----------------------------------------------------------------------*/
static void         PutQuestionMark (char c)
{
   PutInBuffer ('?');
   PutInBuffer (c);
}

/*----------------------------------------------------------------------
   EndOfDeclaration
  ----------------------------------------------------------------------*/
static void         EndOfDeclaration (char c)
{
   /*** process the XML declaration available in inputBuffer ***/

   /* the input buffer is now empty */
   bufferLength = 0;
}

/*----------------------------------------------------------------------
   EndOfPI      A Processing Instruction has been read
  ----------------------------------------------------------------------*/
static void         EndOfPI (char c)
{
   inputBuffer[bufferLength] = EOS;
   if (bufferLength < 1 || inputBuffer[bufferLength-1] != '?')
     {
      HTMLParseError (currentDocument, "Missing question mark");
      normalTransition = FALSE;
      XMLabort = TRUE;
     }
   else
      /* process the Processing Instruction available in inputBuffer */
      {
      inputBuffer[bufferLength-1] = EOS;
      /* printf ("PI: %s\n", inputBuffer); */
      }
   /* the input buffer is now empty */
   bufferLength = 0;
}


/*----------------------------------------------------------------------
   Do_nothing
   Do nothing.
  ----------------------------------------------------------------------*/
static void         Do_nothing (char c)
{
}

/* some type definitions for the automaton */
typedef struct _Transition *PtrTransition;
/* a transition of the automaton in "executable" form */
typedef struct _Transition
{
  unsigned char	    trigger;	      /* the imput character that triggers the
				         transition */
  Proc           action;	      /* the procedure to be called when the
				         transition occurs */
  state          newState;	      /* the new state of the automaton after
				         the transition */
  PtrTransition  nextTransition;   /* next transition from the same state */
}
Transition;

/* a state of the automaton */
typedef struct _StateDescr
{
  state          automatonState;   /* the state */
  PtrTransition  firstTransition;  /* first transition from that state */
}
StateDescr;

/* the automaton that drives the XML parser */
#define MAX_STATE 40
static StateDescr   XMLautomaton[MAX_STATE];
/* a transition of the automaton in "source" form */
typedef struct _sourceTransition
  {
     state          initState;	      /* initial state of transition */
     char         trigger;	      /* the imput character that triggers the
					 transition */
     Proc           transitionAction; /* the procedure to be called when the
					 transition occurs */
     state          newState;	      /* final state of the transition */
  }
sourceTransition;

/* the XML automaton in "source" form */
static sourceTransition sourceAutomaton[] =
{
/*
   state, trigger, action, new state
 */
/* state 0: reading character data */
   {0, '<', (Proc) StartOfTag, 1},
   {0, '&', (Proc) StartOfEntity, -30},		/* call subautomaton 30 */
   {0, '*', (Proc) PutInBuffer, 0},	/* '*' means any other character */
/* state 1: '<' has been read */
   {1, '/', (Proc) Do_nothing, 3},
   {1, '!', (Proc) Do_nothing, 10},
   {1, '?', (Proc) Do_nothing, 20},
   {1, 'S', (Proc) XMLerror, 1},		/*   S = Space */
   {1, '*', (Proc) PutInBuffer, 2},
/* state 2: reading the element name in a start tag */
   {2, '/', (Proc) EndOfStartGIandTag, 18},
   {2, '>', (Proc) EndOfStartGIandTag, 0},
   {2, 'S', (Proc) EndOfStartGI, 16},		/*   S = Space */
   {2, '*', (Proc) PutInBuffer, 2},
/* state 3: reading the element name in an end tag */
   {3, '>', (Proc) EndOfNameAndClosingTag, 0},
   {3, 'S', (Proc) EndOfClosingTagName, 7},
   {3, '*', (Proc) PutInBuffer, 3},
/* state 4: reading an attribute name */
   {4, '=', (Proc) EndOfAttrName, 5},
   {4, 'S', (Proc) EndOfAttrName, 17},
   {4, '/', (Proc) EndOfAttrNameAndTag, 18},
   {4, '>', (Proc) EndOfAttrNameAndTag, 0},
   {4, '*', (Proc) PutInBuffer, 4},
/* state 5: begin of attribute value */
   {5, '\"', (Proc) Do_nothing, 6},
   {5, '\'', (Proc) Do_nothing, 9},
   {5, 'S', (Proc) Do_nothing, 5},
/* state 6: reading an attribute value between double quotes */
   {6, '\"', (Proc) EndOfAttrValue, 8},
   {6, '&', (Proc) StartOfEntity, -30},		/* call subautomaton 30... */
   {6, '*', (Proc) PutInBuffer, 6},
/* state 7: reading spaces and expecting end of end tag */
   {7, '>', (Proc) EndOfXMLEndTag, 0},
   {7, 'S', (Proc) Do_nothing, 7},
/* state 8: end of attribute value */
   {8, '/', (Proc) EndOfStartTag, 18},
   {8, '>', (Proc) EndOfStartTag, 0},
   {8, 'S', (Proc) Do_nothing, 16},
/* state 9: reading an attribute value between simple quotes */
   {9, '\'', (Proc) EndOfAttrValue, 8},
   {9, '&', (Proc) StartOfEntity, -30},		/* call subautomaton 30 */
   {9, '*', (Proc) PutInBuffer, 9},
/* state 10: "<!" has been read */
   {10, '-', (Proc) Do_nothing, 11},
   {10, '*', (Proc) PutInBuffer, 15},
/* state 11: "<!-" has been read. Probably a comment */
   {11, '-', (Proc) StartOfComment, 12},
/* state 12: reading a comment */
   {12, '-', (Proc) Do_nothing, 13},
   {12, '*', (Proc) PutInComment, 12},
/* state 13: a dash "-" has been read within a comment */
   {13, '-', (Proc) Do_nothing, 14},
   {13, '*', (Proc) PutDash, 12},
/* state 14: a double dash "--" has been read within a comment */
   {14, '>', (Proc) EndOfComment, 0},
/* state 15: reading the prologue "<!XXX ..." */
   {15, '>', (Proc) EndOfDeclaration, 0},
   {15, '*', (Proc) PutInBuffer, 15},
/* state 16: expecting an attribute name or an end of start tag */
   {16, 'S', (Proc) Do_nothing, 16},
   {16, '/', (Proc) EndOfStartTag, 18},
   {16, '>', (Proc) EndOfStartTag, 0},
   {16, '*', (Proc) PutInBuffer, 4},
/* state 17: expecting '=' after an attribute name */
   {17, 'S', (Proc) Do_nothing, 17},
   {17, '=', (Proc) Do_nothing, 5},
/* state 18: a '/' has been read within a start tag. Expect a '>' which */
/* indicates the end of the start tag for an empty element */
   {18, '>', (Proc) EndOfEmptyTag, 0},
/* state 20: "<?" has been read; beginning of a Processing Instruction */
   {20, 'S', (Proc) Do_nothing, 20},
   {20, '?', (Proc) Do_nothing, 22},
   {20, '*', (Proc) PutInBuffer, 21},
/* state 21: reading a Processing Instruction */
   {21, '?', (Proc) Do_nothing, 22},
   {21, '*', (Proc) PutInBuffer, 21},
/* state 22: a question mark has been read in a Processing Instruction */
   {22, '>', (Proc) EndOfPI, 0},
   {22, '*', (Proc) PutQuestionMark, 21},

/* sub automaton for reading entities in various contexts */
/* state -1 means "return to calling state" */
/* state 30: an '&' has been read */
   {30, '#', (Proc) Do_nothing, 32},
   {30, 'S', (Proc) PutAmpersandSpace, -1},	/* return to calling state */
   {30, '*', (Proc) EntityChar, 31},
/* state 31: reading an name entity */
   {31, ';', (Proc) EndOfEntity, -1},		/* return to calling state */
   {31, '*', (Proc) EntityChar, 31},
/* state 32: "&#" has been read: reading a numerical entity */
   {32, 'x', (Proc) Do_nothing, 34},
   {32, 'X', (Proc) Do_nothing, 34},
   {32, '*', (Proc) DecEntityChar, 33},
/* state 33: "&#x" has been read: reading a decimal value */
   {33, ';', (Proc) EndOfDecEntity, -1},        /* return to calling state */
   {33, '*', (Proc) DecEntityChar, 33},
/* state 34: "&#x" has been read: reading an hexadecimal value */
   {34, ';', (Proc) EndOfHexEntity, -1},        /* return to calling state */
   {34, '*', (Proc) HexEntityChar, 34},
 
/* state 1000: fake state. End of automaton table */
/* the next line must be the last one in the automaton declaration */
   {1000, '*', (Proc) Do_nothing, 1000}
};

/*----------------------------------------------------------------------
   InitAutomaton
   Read the "source" form of the XML automaton and build the "executable" form.
  ----------------------------------------------------------------------*/
static void            InitXMLAutomaton (void)
{
   int                 entry;
   state               theState;
   state               curState;
   PtrTransition       trans;
   PtrTransition       prevTrans;

   for (entry = 0; entry < MAX_STATE; entry++)
       XMLautomaton[entry].firstTransition = NULL;
   entry = 0;
   curState = 1000;
   prevTrans = NULL;
   do
     {
	theState = sourceAutomaton[entry].initState;
	if (theState < 1000)
	  {
	     trans = (PtrTransition) TtaGetMemory (sizeof (Transition));
	     trans->nextTransition = NULL;
	     trans->trigger = sourceAutomaton[entry].trigger;
	     trans->action = sourceAutomaton[entry].transitionAction;
	     trans->newState = sourceAutomaton[entry].newState;
	     if (trans->trigger == 'S')		/* any spacing character */
		trans->trigger = SPACE;
	     if (trans->trigger == '*')		/* any character */
		trans->trigger = EOS;
	     if (theState != curState)
	       {
		  XMLautomaton[theState].automatonState = theState;
		  XMLautomaton[theState].firstTransition = trans;
		  curState = theState;
	       }
	     else
		prevTrans->nextTransition = trans;
	     prevTrans = trans;
	     entry++;
	  }
     }
   while (theState < 1000);
   XMLautomatonInitalized = TRUE;
}

/*----------------------------------------------------------------------
   FreeXMLParser
   Frees all ressources associated with the XML parser.
  ----------------------------------------------------------------------*/
void                FreeXMLParser (void)
{
   int		       state;
   PtrTransition       trans, nextTrans;
   PtrParserCtxt       ctxt, nextCtxt;
 
   /* free the internal representation of the automaton */
   if (XMLautomatonInitalized)
      for (state = 0; state < MAX_STATE; state++)
         {
         trans = XMLautomaton[state].firstTransition;
         while (trans != NULL)
            {
            nextTrans = trans->nextTransition;
            TtaFreeMemory (trans);
            trans = nextTrans;
            }
         }

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

/*----------------------------------------------------------------------
   XMLparse
   Parse the current file (or buffer) starting at the current position
   and  build the corresponding Thot abstract tree.
   DTDname: name of the DTD to be used
   doc: document to which the abstract tree belongs
   el: the previous sibling (if isclosed) or parent of the tree to be built
   lang: current language
   Return TRUE if the parsing is complete.
  ----------------------------------------------------------------------*/
ThotBool XMLparse (FILE *infile, int *index, char *DTDname, Document doc,
		   Element *el, ThotBool *isclosed, Language lang)
{
  unsigned char             charRead;
  ThotBool            match;
  PtrTransition       trans;
  ThotBool	      endOfFile;
  ThotBool	      error;
  ThotBool	      EmptyLine = TRUE;	  /* no printable character encountered
					     yet in the current line */
  PtrParserCtxt	      oldParserCtxt;
  Document	      oldDocument;
  Language	      oldLanguage;
  Element	      oldElement;
  ThotBool	      oldElementClosed;
  Attribute	      oldAttribute;
  ThotBool	      oldXMLrootClosed;
  int		      oldXMLrootLevel;
  int		      oldStackLevel;

  /* initialize all parser contexts if not done yet */
  if (firstParserCtxt == NULL)
     InitParserContexts ();
  /* intialise the XML automaton if it has not been initialized yet */
  if (!XMLautomatonInitalized)
     InitXMLAutomaton ();

  /* saves the current parsing environment and initialize the new one */
  oldDocument = currentDocument;
  currentDocument = doc;
  oldParserCtxt = currentParserCtxt;

  ChangeParserContext (DTDname);
  oldLanguage = currentLanguage;
  currentLanguage = lang;
  oldElement = currentElement;
  currentElement = *el;
  oldElementClosed = currentElementClosed;
  currentElementClosed = *isclosed;
  oldAttribute = currentAttribute;
  currentAttribute = NULL;
  oldXMLrootClosed = XMLrootClosed;
  XMLrootClosed = FALSE;
  oldXMLrootLevel = XMLrootLevel;
  XMLrootLevel = stackLevel;
  oldStackLevel = stackLevel;
  XMLabort = FALSE;
  inputBuffer[0] = EOS;
  bufferLength = 0;
  entityName[0] = EOS;
  entityNameLength = 0;

  currentState = 0;

  charRead = EOS;
  endOfFile = FALSE;
  error = FALSE;
  immAfterTag = TRUE;

  /* read the XML file sequentially */
  do
    {
      /* read one character from the source if the last character */
      /* read has been processed */
      if (charRead == EOS)
	  charRead = GetNextInputChar (infile, index, &endOfFile);
      if (charRead != EOS && !endOfFile)
	{
	  /* Check the character read */
	  /* Consider LF and FF as the end of an input line. */
	  /* Replace end of line by space, except in preformatted text. */
	  /* Replace HT by space, except in preformatted text. */
	  /* Ignore spaces at the beginning and at the end of input lines */
	  /* Ignore non printable characters except HT, LF, FF. */
	  if ((int) charRead == EOL)
	    /* LF = end of input line */
	    {
	      if (currentState != 12)
		{
		/* don't change characters in comments */
		if (currentState != 0)
		  /* not within a text element */
		  {
		    immAfterTag = FALSE;
		    if (charRead != EOS)
		      {
		      /* Replace new line by a space, except if an entity is
			 being read */
		      if (currentState == 30) 
			charRead = EOL; /* new line */
		      else
			charRead = SPACE;
		      }
		  }
		else
		  /* new line in ordinary text */
		  {
		    /* suppress all spaces preceding the end of line */
		    while (bufferLength > 0 &&
			   inputBuffer[bufferLength - 1] == SPACE)
		      bufferLength--;
		    /* ignore newlines immediately after end of tag */
		    if (immAfterTag)
		      charRead = EOS;
		    else
		      {
			/* new line is equivalent to space */
			charRead = SPACE;
			if (bufferLength > 0)
			  XMLTextToDocument ();
		      }
		  }
		}
	      else
		immAfterTag = FALSE;	
	      /* beginning of a new input line */
	      EmptyLine = TRUE;
	    }
	  else
	    /* it's not an end of line */
	    {
	      immAfterTag = FALSE;
	      if ((int) charRead == 9)
		/* HT = Horizontal tabulation */
		{
		  charRead = SPACE;
		}
	      if (charRead == SPACE)
		/* space character */
		{
		  if (currentState == 0 || currentState == 12)
		    /* in a text element or in a comment */
		    /* ignore spaces at the beginning of an input line */
		    if (EmptyLine)
		      charRead = EOS;
		}
#ifndef _I18N_
	      else if (((int) charRead < 32 ||
			((int) charRead >= 127 && (int) charRead <= 143))
		       && (int) charRead != 9)
		/* it's not a printable character, ignore it */
		charRead = EOS;
#endif /* !_I18N_ */
	      else
		/* it's a printable character. Keep it as it is and */
		/* stop ignoring spaces */
		EmptyLine = FALSE;
	    }
	  if (charRead != EOS)
	    /* a valid character has been read */
	    {
	      /* first transition of the automaton for the current state */
	      trans = XMLautomaton[currentState].firstTransition;
	      /* search a transition triggered by the character read */
	      while (trans != NULL && !XMLrootClosed)
		{
		  match = FALSE;
		  if (charRead == trans->trigger)
		    /* the char is the trigger */
		    match = TRUE;
		  else if (trans->trigger == EOS)
		    /* any char is a trigger */
		    match = TRUE;
		  else if (trans->trigger == SPACE)
		    /* any space is a trigger */
		    if ((int) charRead == TAB || (int) charRead == EOL ||
			(int) charRead == 12)
		      /* a delimiter has been read */
		      match = TRUE;
		  if (match)
		    /* transition found. Activate the transition */
		    {
		      /* call the procedure associated with the transition */
		      normalTransition = TRUE;
		      if (trans->action != NULL)
			(*(trans->action)) (charRead);
		      if (normalTransition)
			{
			  /* the input character has been processed */
			  charRead = EOS;
			  /* the procedure associated with the transition */
			  /* has not changed state explicitely */
			  /* change current automaton state */
			  if (trans->newState >= 0)
			    {
			    if (trans->newState != currentState)
			       error = FALSE;
			    currentState = trans->newState;
			    }
			  else if (trans->newState == -1)
			    /* return form subautomaton */
			    currentState = returnState;
			  else
			    /* calling a subautomaton */
			    {
			      returnState = currentState;
			      currentState = -trans->newState;
			    }
			}
		      /* done */
		      trans = NULL;
		    }
		  else
		    /* access next transition from the same state */
		    {
		      trans = trans->nextTransition;
		      if (trans == NULL)
			{
			if (!error)
			   {
			   XMLerror (charRead);
			   error = TRUE;
			   }
			charRead = EOS;
			}
		    }
		}
	    }
	}
    }
  while (!endOfFile && !XMLrootClosed && !XMLabort);

  /* return parameters */
  *el = currentElement;
  *isclosed = currentElementClosed;

  /* restore the previous parsing environment */
  if (currentParserCtxt)
      currentParserCtxt->XMLSSchema = NULL;
  currentParserCtxt = oldParserCtxt;
  currentDocument = oldDocument;
  currentLanguage = oldLanguage;
  currentElement = oldElement;
  currentElementClosed = oldElementClosed;
  currentAttribute = oldAttribute;
  XMLrootClosed = oldXMLrootClosed;
  XMLrootLevel = oldXMLrootLevel;
  stackLevel = oldStackLevel;

  return (!XMLabort);
}

/* end of module */
