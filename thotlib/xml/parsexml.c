/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
   module:  parsexml.c
   Authors: Monte Regis
            Bonhomme Stephane

   Comments: this module tries to parse an XML document file using XML 1.0 
             Recommandation and Namespace draft.
	     It uses a state automaton.
	     this module uses constant defined in constxml.h.

   Extern function: Document  XmlParseDoc (char* name)

   Compilation directives: -DNAMESPACE for Debuging

   
*/

#define EOS     '\0'
#define SPACE    ' '
#define MAX_LENGTH     512
#include "constmedia.h"
#include "typemedia.h"
#include "tree.h"
#include "content.h"
#include "view.h"
#include "presentation.h"
#include "language.h"
#include "attribute.h"
#include "document.h"
#include "application.h"
#include "app.h"

#include "translatexml_f.h"
#include "constxml.h"
#include "typexml.h"
#include "xmlmodule_f.h"
#include "thotmodule_f.h"
#include "callback_f.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "constmedia.h"
#include "typemedia.h"
#include "appdialogue.h"
#include "appdialogue_tv.h"
#include "edit_tv.h"
/* an entity name */
typedef unsigned char entName[10];

/* an entity representing an ISO-Latin-1 character */
typedef struct _EntityDictEntry
  {		
     entName	charName;	/* entity name */
     int	charCode;	/* decimal code of the ISO-Latin1 char */
  }
EntityDictEntry;

/* Xml predefined entities */
static EntityDictEntry XmlpredifinedEntities[] =
{
   /* This table MUST be in alphabetical order */
   {AMP_ENTITY, 38},
   {APOS_ENTITY, 39},
   {GT_ENTITY, 62},
   {LT_ENTITY, 60},
   {QUOT_ENTITY, 34},

   {"zzzz", 0}			/* this last entry is required */
};

typedef int         state;	/* a state of the parser automaton */

/* maximum length of a Thot structure schema name */
#define MAX_SS_NAME_LENGTH 32
/* Context parser suppressed */
/* information about the input file */
static boolean      immAfterTag = FALSE;  /* A tag has just been read */	
/* input buffer */
#define MAX_BUFFER_LENGTH 1000
#define ALMOST_FULL_BUFFER 700
static unsigned char inputBuffer[MAX_BUFFER_LENGTH];
static int          bufferLength = 0;	  /* actual length of text in input
					     buffer */

/* information about the Thot document under construction */
static Document     currentDocument;              /* the current Thot document */
static char         currentDocumentName[20];      /* the current doc name */
static SSchema      currentDocSSchema = NULL;     /* the current doc SSchema */
static Language     currentLanguage;	          /* language in use */
static SSchema	    currentSSchema = NULL;        /* current SSchema */
static SSchema	    currentAttrSSchema = NULL;
static boolean	    ReadingXmlElement = FALSE;    /* Xml schema flag */
static boolean	    ReadingThotElement = FALSE;   /* Thot schema flag */
static Element	    createdElement = NULL;        /* new created element */
static boolean	    currentElementClosed = FALSE; /* structure flag */
static Attribute    currentAttribute = NULL;      /* current attribute */
static unsigned char currentAttributeName[30];    /* current attribute name */
static boolean	    ReadingXmlNSAttribute = FALSE;/* namespace attr flag */
static boolean	    ReadingXmlAttribute = FALSE;  /* xml attr flag */
static boolean	    ReadingThotAttribute = FALSE; /* thot attr flag */
static boolean	    ReadingAssocRoot = FALSE;     /* thot assoc root flag */
static boolean      DoCreateElement = TRUE;       /* flag for NotifyElement */
static boolean      DoCreateAttribute = TRUE;     /* flag for NotifyAttribute*/
static int          IgnoreElemLevel;              /* stack level from where */
                                                  /* elements are ignored (events) */
static unsigned char currentGI[64];               /* the GI of the last elem read */
static int          nbAssocRoot = 0;              /* number of assoc trees read */
/* parser stack */
#define MAX_STACK_HEIGHT 200		  /* maximum stack height */
static unsigned char* GIStack[MAX_STACK_HEIGHT]; /* Xml element name */
static Element      elementStack[MAX_STACK_HEIGHT];  /* element in the Thot abstract tree */
static Language     languageStack[MAX_STACK_HEIGHT]; /* element language */
static int          stackLevel = 0;       /* first free element on the stack */

/* parser automaton */
static boolean	    XmlautomatonInitalized = FALSE;
static state        currentState;	  /* current state of the automaton */
static state        returnState;	  /* return state from subautomaton */
static boolean      normalTransition;

/* information about an entity being read */
#define MAX_ENTITY_LENGTH 80
static char         entityName[MAX_ENTITY_LENGTH]; /* name of entity being
					     read */
static int          entityNameLength = 0; /* length of entity name read so
					     far */


/*----------------------------------------------------------------------
   XmlError: Manage errors
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void         XmlError (Document doc, unsigned char *text)
#else
void         XmlError (doc,text)
document doc,;
unsigned char *text;
#endif
{
   printf ("document:%s\n",TtaGetDocumentName(doc));
   printf ("error:%s\n",text);
}

/* ---------------------------------------------------------------------
   XmlSetCurrentDocument: Creates a new document and initialize it.
                          launched after the first xmlns attribute
  ----------------------------------------------------------------------*/
#ifdef __STDC__
Element XmlSetCurrentDocument (char *schemaName)
#else
Element XmlSetCurrentDocument (schemaName)
char *schemaName;
#endif
{
  Element root,el;
  Attribute attr;
  AttributeType attrType;
  int attrKind;
  
  /* Creating Document and Initializing document */
  currentDocument = TtaNewDocument(schemaName,currentDocumentName);
  TtaSetMandatoryInsertion (FALSE, currentDocument);
  currentDocSSchema = TtaGetDocumentSSchema (currentDocument);
  /* deleting skeleton and root attributes (execpt Lang) */
  root = TtaGetMainRoot (currentDocument);
  el = TtaGetFirstChild (root);
  while (el!=NULL)
    {
      TtaDeleteTree (el,currentDocument);
      el = TtaGetFirstChild (root);
    }
  attr = NULL;
  TtaNextAttribute (root, &attr);
  while (attr != NULL)
    {
      TtaGiveAttributeType (attr, &attrType, &attrKind);
      if (attrType.AttrTypeNum != 1)
	TtaRemoveAttribute (root, attr, currentDocument);
      TtaNextAttribute (root, &attr);
   }
  strcpy (currentGI, inputBuffer);
  currentElementClosed = FALSE;
  return root;
}

/* ---------------------------------------------------------------------
   XmlChangeCurrentLangage: function useful because called by an
                            external module xmlmodule.c
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void XmlChangeCurrentLangage (Language lang)
#else
void XmlChangeCurrentLangage (lang)
Language lang;
#endif
{
  currentLanguage = lang;
}

/*----------------------------------------------------------------------
   EndOfPrefix
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void EndOfPrefix (unsigned char c)
#else
static void EndOfPrefix (c)
unsigned char c;
#endif
{
  inputBuffer[bufferLength]=EOS;
  if (!strcmp(inputBuffer,"xml"))
    /* Reading an xml element or attribute */
    ReadingXmlElement = TRUE;
  else if (!strcmp(inputBuffer,"thot"))
    /* Reading a thot element or attribute */
    ReadingThotElement = TRUE;
  /* else if (!strcmp(inputBuffer,"xmlns")) */
    /* Reading the xmlns attribute (prefix/attr inversed */
/*     { */
/*       ReadingXmlElement = TRUE;       */
/*       ReadingXmlNSAttribute = TRUE; */
/*     } */
  else
    currentSSchema = XmlGetNSSchema (inputBuffer);
  bufferLength = 0;
}

/*----------------------------------------------------------------------
   XmlInsertElement: Insert an element and make all necessary links
  ----------------------------------------------------------------------*/
#ifdef __STDC__
Element XmlInsertElement (Element newElement)
#else
Element XmlInsertElement (newElement)
Element newElement;

#endif
{
  Element	lastElement;
  Element	tempElement;
  Element	descendElement;
  ElementType	elType;
  Attribute	currentAttribute, tempAttribute;
  boolean	isChild = FALSE;
  boolean	success = FALSE;
  int		val;
  int		parentStackLevel;
  char		buf[255];
  Element	elemParent ;
  boolean	insertAsSibling ;
  Element	elemSibling ;
	
#ifdef DEBUG
  ElementType elt2;

  elType = TtaGetElementType (newElement);
  printf ("inserting %s  ", TtaGetElementTypeOriginalName (elType));
  if (stackLevel != 0)
    {
      elt2 = TtaGetElementType (elementStack[stackLevel-1]);
      printf ("inside %s  ", TtaGetElementTypeOriginalName (elt2));
    }
  if (currentElementClosed)
    {
      elt2 = TtaGetElementType (elementStack[stackLevel]);
      printf ("after %s", TtaGetElementTypeOriginalName(elt2));
    }
  printf ("(level %d)\n",stackLevel);
#endif

  elType = TtaGetElementType (newElement);
  if (stackLevel == 0)
    {
      /* the root is already inserted : setting the attributes */
      tempElement = TtaGetMainRoot (currentDocument);
      currentAttribute = NULL;
      tempAttribute = NULL;
      TtaNextAttribute (newElement, &currentAttribute);
      TtaNextAttribute (tempElement, &tempAttribute);
      if (currentAttribute != NULL)
	{/* it is the language attribute (mandatory on root)*/
	  val = 255;
	  TtaGiveTextAttributeValue (currentAttribute, buf, &val);
	  TtaSetAttributeText (tempAttribute, buf, tempElement, currentDocument);
	}
      TtaNextAttribute (tempElement, &currentAttribute);	   
      while (currentAttribute != NULL)
	{ /* setting other attributes */
	  TtaRemoveAttribute (newElement,  currentAttribute, currentDocument);
	  TtaAttachAttribute (tempElement, currentAttribute, currentDocument);
	  TtaNextAttribute (tempElement, &currentAttribute);	   
	}  
      newElement = tempElement;
      success = TRUE;
    }
  else if (ReadingAssocRoot)
    {
      (LoadedDocument[currentDocument-1])->DocAssocRoot[nbAssocRoot++] = (PtrElement) newElement;
      ReadingAssocRoot = FALSE;
      success = TRUE;
    } 
  else
    {
      /* searches the parent element */
      parentStackLevel = stackLevel-1;
      elemParent = elementStack[parentStackLevel];
      while (elemParent == NULL && parentStackLevel>0)
	elemParent = elementStack[--parentStackLevel];
      /* searching the previous sibling */
      if (currentElementClosed && elementStack[stackLevel] != NULL)
	{
	  insertAsSibling = TRUE;
	  elemSibling = elementStack[stackLevel];
	}
      else
	insertAsSibling = FALSE;
      
      if (elType.ElTypeNum == PAGE_BREAK) 
	/* page break are special element*/
	{
	  XmlSetPageBreakProperties (newElement);
	  if (insertAsSibling) 
	    TtaInsertSibling (newElement, elemSibling, FALSE, currentDocument); 
	  else
	    TtaInsertFirstChild (&newElement, elemParent, currentDocument); 
	  success = (TtaGetErrorCode()==0);
	}
      else
	/* normal element */
	{
	  if (insertAsSibling)
	    /* inserting a sibling */
	    {
	      TtaInsertSibling (newElement, elemSibling, FALSE, currentDocument);
	      if (TtaGetErrorCode()==0)
		success = TRUE;
	      else
		/* normal sibling insertion failed */
		{		  
		  /* trying to create the descent from the father to that element */
		  descendElement = TtaCreateDescent (currentDocument, elemParent, elType);
		  if (descendElement != NULL)
		    {
		      if (TtaGetConstruct (elemParent) == ConstructList)
			/* the parent is a list element */
			{
			  /* TtaCreateDescent creates a sibling before so we have
			     to remove tree and insert it after lastchlid */
			  tempElement = TtaGetFirstChild (elemParent);
			  lastElement = TtaGetLastChild (elemParent);
			  if (tempElement != lastElement)
			    {
			      TtaRemoveTree (tempElement, currentDocument);
			      TtaInsertSibling (tempElement, lastElement, FALSE, currentDocument);
			    }
			}
		    }
		  /* insert the new element in place of the last descendant */
		  if (TtaGetErrorCode()==0)
		    {
		      lastElement = descendElement;
		      TtaPreviousSibling (&lastElement);
		      if (lastElement == NULL)
			{
			  lastElement = TtaGetParent (descendElement);
			  isChild = TRUE;
			}
		      TtaRemoveTree (descendElement, currentDocument);
		      if (isChild)
			TtaInsertFirstChild (&newElement, lastElement, currentDocument);
		      else
			TtaInsertSibling (newElement, lastElement, FALSE, currentDocument);
		      if (TtaGetErrorCode()==0)
			success = TRUE;
#ifdef DEBUG
		      printf ("---> Tool tool Kit error fixed \n");
#endif 
		    }
		}
	    }
	  else if (elemParent != NULL)
	    /* inserting a first child */
	    {
	      TtaInsertFirstChild (&newElement, elemParent, currentDocument);
	      if (TtaGetErrorCode()==0)
		{
		  success = TRUE;
		}
	      else
		/* normal insertion failed */
		{
		  TtaDeleteTree (newElement, currentDocument);
		  /* creating descent */
		  descendElement = TtaCreateDescent (currentDocument, elemParent, elType);
		  if (descendElement == NULL)
		    /* well... no way to insert that element */
		    XmlError(currentDocument, "Invalid Document Structure");
		  if (TtaGetErrorCode()==0)
		    {
		      lastElement = descendElement;
		      TtaPreviousSibling (&lastElement);
		      if (lastElement == NULL)
			{
			  lastElement = TtaGetParent (descendElement);
			  isChild = TRUE;
			}
		      TtaRemoveTree (descendElement, currentDocument);
		      if (isChild)
			TtaInsertFirstChild (&newElement, lastElement, currentDocument);
		      else
			TtaInsertSibling (newElement, lastElement, FALSE, currentDocument);
		      success = (TtaGetErrorCode()==0);
#ifdef DEBUG
		      printf ("---> Tool tool Kit error fixed \n");
#endif 
		    }	   
		}
	    }
	}
    }
  if (success)
    return newElement;
  else
    return NULL;
}

/*----------------------------------------------------------------------
   XmlTextToDocument: Put the contents of input buffer in the Thot 
                      document. It creates the Text element if not created
  ----------------------------------------------------------------------*/
void         XmlTextToDocument ()
{
  int		i, firstChar, lastChar;
  ElementType	elType;
  Element       elNew;

  /* close the input buffer */
  inputBuffer[bufferLength] = EOS;
#ifdef DEBUG
  printf ("  XmlTextToDocument \n");
#endif 

  if (DoCreateElement)
    {
      /* suppress leading spaces */
      for (firstChar = 0; 
	   inputBuffer[firstChar] <= SPACE && inputBuffer[firstChar] != EOS;
	   firstChar++);
      if (inputBuffer[firstChar] != EOS)
	/* We don't write empty text */
	{
	  /* suppress trailing spaces */
	  lastChar = strlen (inputBuffer) - 1;
	  for (i = lastChar; inputBuffer[i] <= SPACE && i > 0; i--);     
	  inputBuffer[i+1] = EOS;
	  
	  if (currentElementClosed || TtaGetElementType (elementStack[stackLevel-1]).ElTypeNum != 1)
	    /* There hasn't been the Text tag so the element is not created */
	    {
	      elType.ElTypeNum = 1;	/* Text element */
	      elType.ElSSchema = currentSSchema;
	      elNew = TtaNewElement (currentDocument, elType);
	      elNew = XmlInsertElement (elNew);
	      currentElementClosed = TRUE;
	      if (elNew == NULL)
		TtaDeleteTree (elNew, currentDocument);
	      elementStack[stackLevel] = elNew;
	    }
	  else
	    elNew = elementStack[stackLevel-1];
	  TtaSetTextContent (elNew, &inputBuffer[firstChar], currentLanguage, currentDocument);
	}  
    }
  /* the input buffer is now empty */
  bufferLength = 0;
}

/*----------------------------------------------------------------------
   StartOfTag
   Beginning of a Xml tag (start or end tag).
   Put the preceding text in the Thot document.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         StartOfTag (unsigned char c)
#else
static void         StartOfTag (c)
unsigned char                c;

#endif
{
#ifdef DEBUG
  printf ("  StartOfTag \n");
#endif 
  if (bufferLength > 0)
    /* Text have been readed */
    XmlTextToDocument (); 
  currentSSchema = currentDocSSchema;
  ReadingXmlElement = FALSE;
  ReadingThotElement = FALSE;
}

/*----------------------------------------------------------------------
   PutInBuffer
   Put character c in the input buffer.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         PutInBuffer (unsigned char c)
#else
static void         PutInBuffer (c)
unsigned char       c;

#endif
{
  int                 len;

  /* put the character into the buffer if it is not an ignored char. */
  if ((int) c == 9)		/* HT */
    len = 8;			/* HT = 8 spaces */
  else
    len = 1;
  if (c != EOS)
    {
      if (bufferLength + len >= ALMOST_FULL_BUFFER && currentState == 0)
	XmlTextToDocument ();
      if (bufferLength + len >= MAX_BUFFER_LENGTH)
	{
	  if (currentState == 0)
	    XmlTextToDocument ();
	  else
	    XmlError (currentDocument, "Panic: buffer overflow !!!");
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
   EndOfXmlEndTag
   The end ('>') of an end tag has been read.
   Close the corresponding Thot element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndOfXmlEndTag (unsigned char c)
#else
static void         EndOfXmlEndTag (c)
unsigned char                c;

#endif
{
  ElementType         elType;
  NotifyElement       notifyEl;
#ifdef DEBUG
  printf ("  EndOfXmlEndTag  \n");
#endif 

  if (!ReadingThotElement && !ReadingXmlElement)
    {
      /* Thot Event ElemRead.Post */
      if (DoCreateElement && elementStack [stackLevel-1] != NULL)
	{
	  elType = TtaGetElementType (elementStack [stackLevel-1]);
	  notifyEl.event = TteElemRead;
	  notifyEl.document = currentDocument;
	  notifyEl.element = elementStack [stackLevel-1];
	  notifyEl.elementType.ElTypeNum = elType.ElTypeNum;
	  notifyEl.elementType.ElSSchema = elType.ElSSchema;
	  notifyEl.position = 0;
	  CallEventType ((NotifyEvent *) & notifyEl, FALSE);
	  currentElementClosed = TRUE;
	}
      else if (stackLevel-1 == IgnoreElemLevel)
	{
	  DoCreateElement = TRUE;
	}
      if (stackLevel > 0)
	{
	  currentLanguage = languageStack[stackLevel-1];
	  stackLevel --;
	}
    }
  immAfterTag = TRUE;
}

/*----------------------------------------------------------------------
   EndOfStartTag
   A ">" or a "/" has been read. It indicates the end of a start tag.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndOfStartTag (unsigned char c)
#else
static void         EndOfStartTag (c)
unsigned char                c;

#endif
{
  Element currentElement = NULL;
#ifdef DEBUG
  printf ("   EndOfStartTag \n");
#endif 
  if (!ReadingThotElement && !ReadingXmlElement)
    {
      if (DoCreateElement)
	currentElement = XmlInsertElement (createdElement);
      elementStack[stackLevel] = currentElement;
      GIStack[stackLevel] = TtaStrdup (currentGI);
      languageStack[stackLevel] = currentLanguage;
      if (c != '/')
	{
	  if (DoCreateElement)
	    currentElementClosed = FALSE;
	  stackLevel ++;
	}
      else
	/* this is an empty element. Do not expect an end tag */
	{
	  if (DoCreateElement)
	    currentElementClosed = TRUE;
	}
    }
  currentGI[0]='\0';
  currentAttribute = NULL; 
  currentSSchema = currentDocSSchema;
  ReadingThotElement = FALSE; 
  ReadingXmlElement = FALSE; 
  ReadingAssocRoot = FALSE;
  if (c == '>')
    immAfterTag = TRUE;
}

/*----------------------------------------------------------------------
    EndOfEmptyTag
    A ">" after a "/" or "?" has been read. 
    It indicates the end of a empty tag.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndOfEmptyTag (unsigned char c)
#else
static void         EndOfEmptyTag (c)
unsigned char                c;

#endif
{
  immAfterTag = TRUE;
}


/*----------------------------------------------------------------------
   EndOfStartGI
   The name of an element type has been read in a start tag.
   Create the corresponding Thot element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndOfStartGI (unsigned char c)
#else
static void         EndOfStartGI (c)
unsigned char                c;
#endif
{
  ElementType		elType;
  unsigned char        msgBuffer[MAX_BUFFER_LENGTH];
  NotifyElement       notifyEl;

  /* close the input buffer */
  inputBuffer[bufferLength] = EOS;

#ifdef DEBUG
  printf ("   EndOfStartGI                  ---         %s\n",inputBuffer);
#endif 

  if (stackLevel == 0)
    /* Root element : create the document */
    {
      createdElement = XmlSetCurrentDocument (inputBuffer);
    }
  else
    {           
      if (ReadingThotElement)
	/* the Thot special elements: do not create them */
	{
	  if (!strcmp (inputBuffer, BR_TAG))
	    {
	      /* Warning: not the good string for linebreak */
	      strncpy(inputBuffer, "\212", 5);
	      bufferLength = 5;
	      XmlTextToDocument ();
	      createdElement = NULL;
	    }
	  else if (!strcmp (inputBuffer, PRES_SCHEMA_TAG))
	      ;
	  createdElement = NULL;
	}
      else if (DoCreateElement)
	/* normal element */
	{
	  elType.ElSSchema = currentSSchema;
	  elType.ElTypeNum =  NameXmlToThot (elType.ElSSchema, 
					     inputBuffer, 0, 0);
	  if (elType.ElTypeNum == 0)
	    {
	      sprintf (msgBuffer, "Unknown Xml or Thot  element %s", inputBuffer);
	      XmlError (currentDocument, msgBuffer);
	      createdElement = NULL;
	    }
	  else
	    {
	      /* Thot Event elemread.pre */
	      notifyEl.event = TteElemRead;
	      notifyEl.document = currentDocument;
	      notifyEl.element = elementStack[stackLevel - 1];
	      notifyEl.elementType.ElTypeNum = elType.ElTypeNum;
	      notifyEl.elementType.ElSSchema = elType.ElSSchema;
	      notifyEl.position = 0;
	      DoCreateElement = !CallEventType ((NotifyEvent *) & notifyEl, TRUE);
	      if (DoCreateElement)
		{
		  /* create a new element */
		  createdElement = TtaNewElement (currentDocument, elType); 
		  if (TtaIsElementTypeReference (elType) && createdElement!=NULL)
		    /* storing the reference element in xmlmodule for later updating */
		    XmlAddRef (currentDocument, createdElement, NULL);
		}
	      else
		IgnoreElemLevel = stackLevel;
	    }
	}
    }
  strcpy (currentGI, inputBuffer);
  currentAttrSSchema = currentDocSSchema;
  bufferLength = 0;
}

/*----------------------------------------------------------------------
   EndOfStartGIandTag
   A ">" has been read. It indicates the end of an element name and the
   end of a start tag.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndOfStartGIandTag (unsigned char c)
#else
static void         EndOfStartGIandTag (c)
unsigned char                c;

#endif
{
   EndOfStartGI (c);
   EndOfStartTag (c);
}

/*----------------------------------------------------------------------
   EndOfClosingTagName
   An element name has been read in a closing tag.
   Check that it closes the right element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndOfClosingTagName (unsigned char c)
#else
static void         EndOfClosingTagName (c)
unsigned char                c;

#endif
{
  unsigned char       msgBuffer[MAX_BUFFER_LENGTH];

  /* close the input buffer */
  inputBuffer[bufferLength] = EOS;
#ifdef DEBUG
   printf ("  EndOfClosingTagName  \n");
#endif 
  if (GIStack[stackLevel - 1] != NULL)
    /* the corresponding opening tag was a known tag */
    if (strcmp(inputBuffer, GIStack[stackLevel - 1]) != 0)
      /* the end tag does not close the current element */
      {
	/* print an error message */
	sprintf (msgBuffer, "Unexpected Xml end tag </%s> instead of </%s>",
		 inputBuffer, GIStack[stackLevel - 1]);
	XmlError (currentDocument, msgBuffer);
      }
  /* the input buffer is now empty */
  bufferLength = 0;
}

/*----------------------------------------------------------------------
   EndOfNameAndClosingTag
   An element name followed by a '>' has been read in a closing tag.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndOfNameAndClosingTag (unsigned char c)
#else
static void         EndOfNameAndClosingTag (c)
unsigned char                c;

#endif
{
   EndOfClosingTagName (c);
   EndOfXmlEndTag (c);
}

/*----------------------------------------------------------------------
   EndOfAttrPrefix
   An attribute prefix has been read.
   ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndOfAttrPrefix (unsigned char c)
#else
static void         EndOfAttrPrefix (c)
unsigned char                c;

#endif
{ 
  inputBuffer[bufferLength]=EOS;

#ifdef DEBUG
   printf ("  EndOfAttrPrefix                  ---         %s\n",inputBuffer);
#endif 
  if (!strcmp(inputBuffer,"xml"))
    /* Reading an xml element or attribute */
    ReadingXmlAttribute = TRUE; 
  else if (!strcmp(inputBuffer,"thot"))
    /* Reading a thot element or attribute */
    ReadingThotAttribute = TRUE;
  else if (!strcmp(inputBuffer,"xmlns"))
    /* Reading the xmlns attribute (prefix/attr inversed */
    ReadingXmlNSAttribute = TRUE;
  else
    currentAttrSSchema = XmlGetNSSchema (inputBuffer);
  bufferLength = 0;
}

/*----------------------------------------------------------------------
   EndOfAttrName
   An Xml attribute name has been read.
   Create the corresponding Thot attribute.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndOfAttrName (unsigned char c)
#else
static void         EndOfAttrName (c)
unsigned char                c;

#endif
{
  Attribute	attr, oldAttr;
  AttributeType	attrType;
  int           attrKind;
  unsigned char msgBuffer[MAX_BUFFER_LENGTH];
  NotifyAttribute     notifyAttr;

  /* close the input buffer */
  inputBuffer[bufferLength] = EOS;
#ifdef DEBUG
   printf ("  EndOfAttrName                  ---         %s\n",inputBuffer);
#endif   
  currentAttribute = NULL;
  strcpy (currentAttributeName, inputBuffer);
  if (ReadingXmlAttribute)
    ;
  else if (ReadingThotAttribute)
    {
      if (!strcmp (inputBuffer, ASSOC_TREE_ATTR))
	{
	  ReadingAssocRoot = TRUE; 
	}
    }
  else if (ReadingXmlNSAttribute)
    ;
  else if (createdElement != NULL)
    {
      attrType.AttrSSchema = currentAttrSSchema;
      attrType.AttrTypeNum = NameXmlToThot(currentAttrSSchema,
					   inputBuffer,
					   1,
					   0);
     
      if (attrType.AttrTypeNum == 0 )
	{
	  sprintf (msgBuffer, "Unknown Xml attribute %s", inputBuffer);
	  XmlError (currentDocument, msgBuffer);
	}
      else
	{ 
	  /* Thot Event AttrRead.Pre */
	  notifyAttr.event = TteAttrRead;
	  notifyAttr.document = currentDocument;
	  notifyAttr.element = createdElement;
	  notifyAttr.attribute = NULL;
	  notifyAttr.attributeType.AttrTypeNum = attrType.AttrTypeNum;
	  notifyAttr.attributeType.AttrSSchema = attrType.AttrSSchema;
	  DoCreateAttribute = !CallEventAttribute (&notifyAttr, TRUE);
	  
	  if (DoCreateAttribute)
	    {
	      oldAttr = TtaGetAttribute (createdElement, attrType);
	      if (oldAttr != NULL)
		{
		  /* this attribute already exists for the current element */
		  /* it will be updated at EndOfAttrValue */
		  currentAttribute = oldAttr;
#ifdef DEBUG
		  sprintf (msgBuffer, "Duplicate Xml attribute %s", inputBuffer);
		  XmlError (currentDocument, msgBuffer);	
#endif
		}
	      else
		{
		  attr = TtaNewAttribute (attrType);
		  TtaGiveAttributeType (attr,&attrType,&attrKind);
		  if (attrKind == 3)
		    /* storing reference attribute in xmlmodule for later updating */
		    XmlAddRef(currentDocument,createdElement,attr);
		  TtaSetStructureChecking (FALSE, currentDocument);
		  TtaAttachAttribute (createdElement, attr, currentDocument);
		  TtaSetStructureChecking (TRUE, currentDocument);
		  TtaSetMandatoryInsertion (FALSE, currentDocument);
		  currentAttribute = attr;
		}
	    }
	}
    }
  /* the input buffer is now empty */
  bufferLength = 0;
}

/*----------------------------------------------------------------------
   EndOfAttrNameAndTag
   A ">" or a "/" (Xml) has been read. It indicates the end of an attribute
   name and the end of a start tag.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndOfAttrNameAndTag (unsigned char c)
#else
static void         EndOfAttrNameAndTag (c)
unsigned char                c;

#endif
{
   EndOfAttrName (c);
   EndOfStartTag (c);
}

/*----------------------------------------------------------------------
   EndOfAttrValue
   An attribute value has been read.
   Put that value in the current Thot attribute.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndOfAttrValue (unsigned char c)
#else
static void         EndOfAttrValue (c)
unsigned char                c;

#endif
{
   AttributeType	attrType;
   int			attrKind, val;
   unsigned char        msgBuffer[MAX_BUFFER_LENGTH];
   NotifyAttribute     notifyAttr;

   /* close the input buffer */
   inputBuffer[bufferLength] = EOS;
#ifdef DEBUG
   printf ("  EndOfAttrValue                  ---         %s\n",inputBuffer);
#endif   
   if (ReadingXmlNSAttribute) 
     {
       /* We're talking about THE xmlns attribute */
       /*   Adding :xmlns in end  of prefix so that ParseXml knows */
       /*   Because in that case there is three data:
	    the xmlns attribute, the prefix, the schema name, 
	    instead of two (the attribute and the value) */
       XmlAddNSSchema (currentDocument, currentAttributeName, inputBuffer);
       ReadingXmlNSAttribute = FALSE;
     }
   else if (ReadingXmlAttribute)
     {
       /* xml module will treat the attribute */
       ParseXmlAttribute (currentDocument, createdElement,
			  currentAttributeName, inputBuffer);
       ReadingXmlAttribute = FALSE;
     }
   else if (ReadingThotAttribute)
     {
       /* thotmodule will treat the attribute */
       ParseThotAttribute (currentDocument, createdElement,
			   currentAttributeName, inputBuffer);
        ReadingThotAttribute = FALSE;
     }
   else if (currentAttribute != NULL)
     /* normal attribute */
     if (DoCreateAttribute)
       {
	 TtaGiveAttributeType (currentAttribute, &attrType, &attrKind);
	 switch (attrKind)
	   {
	   case 0:       /* enumerate */
	     /* Warning: it reads only the integer value */
	     /* To improve */
	     sscanf (inputBuffer, "%d", &val);
	     TtaSetAttributeValue (currentAttribute, val, createdElement, currentDocument);
	     break;
	   case 1:       /* integer */
	     sscanf (inputBuffer, "%d", &val);
	     TtaSetAttributeValue (currentAttribute, val, createdElement, currentDocument);
	     break;
	   case 2:       /* text */
	     TtaSetAttributeText (currentAttribute, inputBuffer, createdElement, currentDocument);
	     break;
	   case 3:       /* reference */
	     XmlSetTarget (currentDocument, createdElement, inputBuffer);
	     break;
	   }
	 /* Thot Event AttrRead.Post */
	 notifyAttr.event = TteAttrRead;
	 notifyAttr.document = currentDocument;
	 notifyAttr.element = createdElement;
	 notifyAttr.attribute = currentAttribute;
	 notifyAttr.attributeType.AttrTypeNum = attrType.AttrTypeNum;
	 notifyAttr.attributeType.AttrSSchema = attrType.AttrSSchema;
	 CallEventAttribute (&notifyAttr, FALSE);
       }
   else
     {
       sprintf (msgBuffer, "Value of Unknown Xml attribute ");
       XmlError (currentDocument, msgBuffer);
     }
  
   /* the input buffer is now empty */
   currentAttribute = NULL;
   currentAttributeName[0] = EOS;
   currentAttrSSchema = currentDocSSchema;
   bufferLength = 0;
}

/*----------------------------------------------------------------------
   StartOfEntity
   A character '&' has been encountered.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         StartOfEntity (unsigned char c)
#else
static void         StartOfEntity (c)
unsigned char                c;

#endif
{
   entityNameLength = 0;
}

/*----------------------------------------------------------------------
   EndOfEntity
   End of an Xml entity name. Search that entity in the entity tables
   and put the corresponding content in the input buffer.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndOfEntity (unsigned char c)
#else
static void         EndOfEntity (c)
unsigned char                c;

#endif
{
   int                 i;
   unsigned char       msgBuffer[MAX_BUFFER_LENGTH];

   entityName[entityNameLength] = EOS;

   /* First, look in the predifined entities table */
   for (i = 0; XmlpredifinedEntities[i].charCode > 0 &&
	       strcmp (XmlpredifinedEntities[i].charName, entityName);
	       i++);
   if (!strcmp (XmlpredifinedEntities[i].charName, entityName))
      /* entity found in the predifined table */
      PutInBuffer ((char) (XmlpredifinedEntities[i].charCode));
   else
      /* entity not in the predifined table */
      {
	/* consider the entity as ordinary text */
	PutInBuffer ('&');
	for (i = 0; i < entityNameLength; i++)
	  PutInBuffer (entityName[i]);
	PutInBuffer (';');
	/* print an error message */
	sprintf (msgBuffer, "Unknown Xml entity \"&%s;\"", entityName);
	XmlError (currentDocument, msgBuffer);
      }
   entityNameLength = 0;
}

/*----------------------------------------------------------------------
   EntityChar
   A character belonging to an Xml entity has been read.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EntityChar (unsigned char c)
#else
static void         EntityChar (c)
unsigned char       c;

#endif
{
   int                 i;

   if (entityNameLength < MAX_ENTITY_LENGTH - 1)
      entityName[entityNameLength++] = c;
   else
      /* entity too long */
      {
      /* error message */
      XmlError (currentDocument, "Xml entity too long");
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
   EndOfNumEntity
   End of a numerical entity.
   Convert the string read into a number and put the character
   having that code in the input buffer.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndOfNumEntity (unsigned char c)
#else
static void         EndOfNumEntity (c)
unsigned char                c;

#endif
{
   int                 code;

   entityName[entityNameLength] = EOS;
   sscanf (entityName, "%d", &code);
   PutInBuffer ((char) code);
   entityNameLength = 0;
}

/*----------------------------------------------------------------------
   NumEntityChar
   A character belonging to a Xml numerical entity has been read.
   Put that character in the entity buffer.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         NumEntityChar (unsigned char c)
#else
static void         NumEntityChar (c)
unsigned char                c;

#endif
{
   if (entityNameLength < MAX_ENTITY_LENGTH - 1)
      /* the entity buffer is not full */
      if (c >= '0' && c <= '9')
	 /* the character is a decimal digit */
	 entityName[entityNameLength++] = c;
      else
	 /* not a decimal digit. assume end of entity */
	 {
	 EndOfNumEntity (c);
	 /* next state is state 0, not the state computed by the automaton */
	 /* and the character read has not been processed yet */
	 normalTransition = FALSE;
	 currentState = 0;
	 if (c != SPACE)
	    /* error message */
	    XmlError (currentDocument, "Missing semicolon in Xml entity");
	 }
}

/*----------------------------------------------------------------------
   Xmlerror
   The character following '/' in a start tag is not '>'.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         Xmlerror (unsigned char c)
#else
static void         Xmlerror (c)
unsigned char                c;

#endif
{
   XmlError (currentDocument, "Invalid Xml syntax");
}

/*----------------------------------------------------------------------
   PutAmpersandSpace
   Put '& ' in the input buffer.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         PutAmpersandSpace (unsigned char c)
#else
static void         PutAmpersandSpace (c)
unsigned char                c;

#endif
{
   PutInBuffer ('&');
   PutInBuffer (SPACE);
   Xmlerror (c);
}

/*----------------------------------------------------------------------
   EndOfXmlDeclaration
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndOfXmlDeclaration (unsigned char c)
#else
static void         EndOfXmlDeclaration (c)
unsigned char                c;

#endif
{
   /*** process the Xml declaration available in inputBuffer ***/

   /* the input buffer is now empty */
   bufferLength = 0;
  immAfterTag = TRUE;
}

/*----------------------------------------------------------------------
   EndOfDeclaration
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndOfDeclaration (unsigned char c)
#else
static void         EndOfDeclaration (c)
unsigned char                c;

#endif
{
   /*** process the Xml declaration available in inputBuffer ***/

   /* the input buffer is now empty */
   bufferLength = 0;
   immAfterTag = TRUE;

}

/*----------------------------------------------------------------------
   EndOfComment
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndOfComment (unsigned char c)
#else
static void         EndOfComment (c)
unsigned char                c;

#endif
{
   /*** process the Comments available in inputBuffer ***/

   /* the input buffer is now empty */
   bufferLength = 0;
  immAfterTag = TRUE;
}

/*----------------------------------------------------------------------
   Do_nothing
   Do nothing.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         Do_nothing (unsigned char c)
#else
static void         Do_nothing (c)
unsigned char                c;

#endif
{
}

/* some type definitions for the automaton */

typedef struct _Transition *PtrTransition;

/* a transition of the automaton in "executable" form */
typedef struct _Transition
  {
    unsigned char       trigger;	/* the imput character that 
					   triggers the transition */
     Proc                action;	/* the procedure to be called when
					   the transition occurs */
     state               newState;	/* the new state of the automaton
					   after the transition */
     PtrTransition       nextTransition;/* next transition from the same state*/
  }
Transition;

/* a state of the automaton */
typedef struct _StateDescr
  {
     state               automatonState;	/* the state */
     PtrTransition       firstTransition;	/* first transition from that state */
  }
StateDescr;

/* the automaton that drives the Xml parser */
#define MAX_STATE 40
static StateDescr	Xmlautomaton[MAX_STATE];

/* a transition of the automaton in "source" form */
typedef struct _sourceTransition
  {
     state               initState;	/* initial state of transition */
     char                trigger;	/* the imput character that triggers
					   the transition */
     Proc                transitionAction;/* the procedure to be called when
					   the transition occurs */
     state               newState;	/* final state of the transition */
  }
sourceTransition;

/* the Xml automaton in "source" form */
static sourceTransition sourceAutomaton[] =
{
/*
   state, trigger, action, new state
 */
/* state 0: reading character data */
   {0, '<', (Proc) StartOfTag, 1},
   {0, '&', (Proc) StartOfEntity, -30},		/* call subautomaton 20 */
   {0, '*', (Proc) PutInBuffer, 0},	/* '*' means any other character */
/* state 1: '<' has been read */
   {1, '/', (Proc) Do_nothing, 3},
   {1, '!', (Proc) Do_nothing, 10},
   {1, '?', (Proc) Do_nothing, 19},
   {1, 'S', (Proc) Xmlerror, 1},		/*   S = Space */
   {1, '*', (Proc) PutInBuffer, 2},
/* state 2: reading the element name in a start tag */
   {2, ':', (Proc) EndOfPrefix, 2},
   {2, '/', (Proc) EndOfStartGIandTag, 18},
   {2, '>', (Proc) EndOfStartGIandTag, 0},
   {2, 'S', (Proc) EndOfStartGI, 16},	
   {2, '*', (Proc) PutInBuffer, 2},
/* state 3: reading the element name in an end tag */
   {3, ':', (Proc) EndOfPrefix, 3},
   {3, '>', (Proc) EndOfNameAndClosingTag, 0},
   {3, 'S', (Proc) EndOfClosingTagName, 7},
   {3, '*', (Proc) PutInBuffer, 3},
/* state 4: reading an attribute name */
   {4, ':', (Proc) EndOfAttrPrefix, 4},
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
   {6, '&', (Proc) StartOfEntity, -30},		/* call subautomaton 20... */
   {6, '*', (Proc) PutInBuffer, 6},
/* state 7: reading spaces and expecting end of end tag */
   {7, '>', (Proc) EndOfXmlEndTag, 0},
   {7, 'S', (Proc) Do_nothing, 7},
/* state 8: end of attribute value */
   {8, '?', (Proc) EndOfStartGIandTag, 18},
   {8, '/', (Proc) EndOfStartTag, 18},
   {8, '>', (Proc) EndOfStartTag, 0},
   {8, 'S', (Proc) Do_nothing, 16},
/* state 9: reading an attribute value between simple quotes */
   {9, '\'', (Proc) EndOfAttrValue, 8},
   {9, '&', (Proc) StartOfEntity, -30},		/* call subautomaton 20 */
   {9, '*', (Proc) PutInBuffer, 9},
/* state 10: "<!" has been read */
   {10, '-', (Proc) Do_nothing, 11},
   {10, '*', (Proc) PutInBuffer, 15},
/* state 11: "<!-" has been read. Probably a comment */
   {11, '-', (Proc) Do_nothing, 12},
/* state 12: reading a comment */
   {12, '-', (Proc) Do_nothing, 13},
   {12, '*', (Proc) PutInBuffer, 12},
/* state 13: a dash "-" has been read within a comment */
   {13, '-', (Proc) Do_nothing, 14},
   {13, '*', (Proc) PutInBuffer, 12},
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
/* state 19: "<?" has been read */
   {19, '?', (Proc) Do_nothing, 20},
   {19, '*', (Proc) PutInBuffer, 19},
/* state 20: "?" has been read */
   {20, '>', (Proc) EndOfXmlDeclaration, 0},
   {20, '*', (Proc) PutInBuffer, 19},

/* sub automaton for reading entities in various contexts */
/* state -1 means "return to calling state" */
/* state 120: a '&' has been read */
   {30, '#', (Proc) Do_nothing, 32},
   {30, 'S', (Proc) PutAmpersandSpace, -1},	/* return to calling state */
   {30, '*', (Proc) EntityChar, 31},
/* state 121: reading an name entity */
   {31, ';', (Proc) EndOfEntity, -1},	/* return to calling state */
   {31, '*', (Proc) EntityChar, 31},
/* state 122: reading a numerical entity */
   {32, ';', (Proc) EndOfNumEntity, -1},	/* return to calling state */
   {32, '*', (Proc) NumEntityChar, 32},

/* state 1000: fake state. End of automaton table */
/* the next line must be the last one in the automaton declaration */
   {1000, '*', (Proc) Do_nothing, 1000}
};

/*----------------------------------------------------------------------
   InitAutomaton
   Read the "source" form of the Xml automaton and build the "executable" form.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void            InitXmlAutomaton (void)
#else
static void            InitXmlAutomaton ()
#endif
{
   int                 entry;
   state               theState;
   state               curState;
   PtrTransition       trans;
   PtrTransition       prevTrans;

   for (entry = 0; entry < MAX_STATE; entry++)
       Xmlautomaton[entry].firstTransition = NULL;
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
		  Xmlautomaton[theState].automatonState = theState;
		  Xmlautomaton[theState].firstTransition = trans;
		  curState = theState;
	       }
	     else
		prevTrans->nextTransition = trans;
	     prevTrans = trans;
	     entry++;
	  }
     }
   while (theState < 1000);
   XmlautomatonInitalized = TRUE;
}

/*----------------------------------------------------------------------
   FreeXmlParser
   Frees all ressources associated with the Xml parser.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FreeXmlParser (void)
#else
void                FreeXmlParser ()
#endif
{
   int		       state;
   PtrTransition       trans, nextTrans;
 
   /* free the internal representation of the automaton */
   if (XmlautomatonInitalized)
      for (state = 0; state < MAX_STATE; state++)
         {
         trans = Xmlautomaton[state].firstTransition;
         while (trans != NULL)
            {
            nextTrans = trans->nextTransition;
            TtaFreeMemory (trans);
            trans = nextTrans;
            }
         }
}

/*----------------------------------------------------------------------
   XmlParseDoc:
   Parse the current file and  build the corresponding Thot abstract tree.
   name: the file name to parsed
   Warning: No context has to be implemented because external references
            are updated after the end of parsing and Thot doesn't open
	    two document at the same time.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
Document	XmlParseDoc (char* name)
#else
Document	XmlParseDoc (name)
char	 *name;
#endif
{
  BinFile             file;
  unsigned char       charRead;
  boolean             match;
  PtrTransition       trans;
  boolean	      endOfFile;
  boolean	      error;
  boolean	      EmptyLine = TRUE;
  char                tempDocName[200];
  int                 i;
  int                 firstNameChar = 0;
  
#ifdef DEBUG
  TtaSetErrorMessages(TRUE);
#endif
  StoreTableActions();
  file = TtaReadOpen (name);
  i=0;
  strcpy(tempDocName,name);
  while(i<strlen(tempDocName)&&tempDocName[i]!='.')
    {
      if (tempDocName[i]=='/')
	firstNameChar = i+1;
      i++;
    }
  tempDocName[i]=EOS;
  tempDocName[firstNameChar-1]=EOS;
  strcpy(currentDocumentName,&tempDocName[firstNameChar]); 
  nbAssocRoot = 0;              
  /* intialise the Xml automaton if it has not been initialized yet */
  if (!XmlautomatonInitalized)
     InitXmlAutomaton ();
  stackLevel = 0;
  inputBuffer[0] = EOS;
  bufferLength = 0;
  entityName[0] = EOS;
  entityNameLength = 0;
  currentState = 0;
  charRead = EOS;
  endOfFile = FALSE;
  error = FALSE;
  immAfterTag = TRUE;   
  DoCreateElement = TRUE;       /* flag for NotifyElement */
  DoCreateAttribute = TRUE;     /* flag for NotifyAttribute*/
  IgnoreElemLevel = 0;              /* stack level from where */
                  
  /* read the Xml file sequentially */
  do
    {
      /* read one character from the source if the last character */
      /* read has been processed */
      if (charRead == EOS)
	  endOfFile = !TtaReadByte (file,&charRead);
      if (charRead != EOS && !endOfFile)
	{
	  /* Check the character read */
	  /* Consider LF and FF as the end of an input line. */
	  /* Replace end of line by space, except in preformatted text. */
	  /* Replace HT by space, except in preformatted text. */
	  /* Ignore spaces at the beginning and at the end of input lines */
	  /* Ignore non printable characters except HT, LF, FF. */
	  if ((int) charRead == 10)
	    /* LF = end of input line */
	    {
	      if (currentState != 12)
		/* don't change characters in comments */
		if (currentState != 0)
		  /* not within a text element */
		  {
		    immAfterTag = FALSE;
		    if (charRead != EOS)
		      /* Replace new line by a space, except if an entity is
			 being read */
		      if (currentState == 20) 
			charRead = '\n'; /* new line */
		      else
			charRead = SPACE;
		  }
		else
		  /* new line in ordinary text */
		  {
		    /* suppress all spaces preceding the end of line */
		    while (bufferLength > 0 && inputBuffer[bufferLength - 1] == SPACE)
		      bufferLength--;
		    /* ignore newlines immediately after end of tag */
		    if (immAfterTag)
		      charRead = EOS;
		    else
		      {
			/* new line is equivalent to space */
			charRead = SPACE;
			if (bufferLength > 0)
			  XmlTextToDocument ();
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
	      else if ((charRead < SPACE || (int) charRead >= 254 ||
			((int) charRead >= 127 && (int) charRead <= 159))
		       && (int) charRead != 9)
		/* it's not a printable character, ignore it */
		charRead = EOS;
	      else
		/* it's a printable character. Keep it as it is and */
		/* stop ignoring spaces */
		EmptyLine = FALSE;
	    }
	  if (charRead != EOS)
	    /* a valid character has been read */
	    {
	      /* first transition of the automaton for the current state */
	      trans = Xmlautomaton[currentState].firstTransition;
	      /* search a transition triggered by the character read */
	      while (trans != NULL)
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
		    if ((int) charRead == 9 || (int) charRead == 10 ||
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
			   XmlError (currentDocument, "Invalid Xml syntax");
			   error = TRUE;
			   }
			charRead = EOS;
			}
		    }
		}
	    }
	}
    }
  while (!endOfFile);
  RestoreTableActions();
  TtaSetMandatoryInsertion (TRUE, currentDocument);
  
  return (currentDocument);
  /* end of the Xml root element */
/*   if (!isclosed) */
/*      XmlElementComplete (el, currentDocument); */

  /* restore the previous parsing environment */
/*   currentParserCtxt = oldParserCtxt; */
/*   currentDocument = oldDocument; */
/*   currentLanguage = oldLanguage; */
/*   currentElementClosed = oldElementClosed; */
/*   currentAttribute = oldAttribute; */
/*   XmlrootClosed = oldXmlrootClosed; */
/*   XmlrootClosingTag = oldXmlrootClosingTag; */
/*   XmlrootLevel = oldXmlrootLevel; */
/*   lastTagRead = oldlastTagRead; */
/*   stackLevel = oldStackLevel; */
}

/* init the xml parser resource */
void XmlParserLoadResources ()
{
  if (ThotLocalActions[T_xmlparsedoc] == NULL)
    TteConnectAction (T_xmlparsedoc, (Proc) XmlParseDoc);
}
/* end of module */
