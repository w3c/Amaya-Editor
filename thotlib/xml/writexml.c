/*

 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
   module: writexml.c
   Authors: Monte Regis
            Bonhomme Stephane

   Comments: this module produce XML document file using XML 1.0 
             Recommandation and Namespace draft.
	     this module uses constant defined in constxml.h

   Extern function: ThotBool SauveXmlDoc (BinFile xmlFile, 
                                         Document doc, 
					 ThotBool withEvent)
   
   Compilation directives: -DNAMESPACE for XML namespaces in output file
                           -DINDENT for indentation in output file
		    


 */

#include "ustring.h"
#include "thot_gui.h"
#include "thot_sys.h"
#include "reference.h"
#include "attribute.h"
#include "content.h"
#include "message.h"
#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"
#include "constpiv.h"
#include "fileaccess.h"
#include "appaction.h"
#include "appstruct.h"
#include "appdialogue.h"
#include "constxml.h"
#include "typexml.h"

#include "attributes_f.h"
#include "callback_f.h"
#include "changepresent_f.h"
#include "memory_f.h"
#include "translatexml_f.h"

static PrefixType *Prefixs = NULL;  /* List of namespace prefixs */
                                    /* See typexml.h for structure */
static int        NbPrefix;         /* counter used for unique prefix name */
static int        XmlDepth = 0;     /* Tree depth */
static Document   OpenedRefDoc[10] = {0,0,0,0,0,0,0,0,0,0};
static int        NbOpenedRefDoc = 0;/* structures for extern references */
static StrAtomPair *ListAtomPair;    /* list of first paired elements */
static ThotBool writeBeginTextTag = FALSE; /* should a text element be fused */
                                          /* with its next sibling ? */

/*----------------------------------------------------------------------
  XmlGetElementLabel : gets a label for the element el
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void XmlGetElementLabel (STRING buffer, Element el)
#else /* __STDC__ */
static void XmlGetElementLabel (buffer, el)
STRING buffer;
Element el;
#endif /* __STDC__ */
{
  buffer[0] = EOS;
  ustrcpy (buffer, TtaGetElementLabel (el));
  if (buffer[0] != '#')
    {
      buffer[0] = '#';
      buffer[1] = EOS;
      ustrncat (buffer, NameThotToXml (TtaGetElementType(el).ElSSchema, TtaGetElementType(el).ElTypeNum, 0, 0),3);
      
      ustrcat(buffer, TEXT("_"));
      ustrcat(buffer, TtaGetElementLabel (el) + 1);
    } 
}

/*----------------------------------------------------------------------
  XmlWriteString:
	Writes a string in the xmlFile WITHOUT the trailing \0
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool  XmlWriteString (BinFile xmlFile, STRING str)
#else /* __STDC__ */
BinFile    xmlFile;
STRING     str;
#endif /* __STDC__ */

{
  ThotBool        ok = TRUE;
  int             i = 0;

  if (str != NULL)
    while (str[i] != EOS)
      {
        ok = ok && TtaWriteByte (xmlFile, str[i]);
        i++;
      }
  return ok;
}

/*----------------------------------------------------------------------
  XmlWriteCharData:
	Writes Char data in the xmlFile WITHOUT the trailing \0
	i.e. consider XML escaped caracters
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool  XmlWriteCharData (BinFile xmlFile,STRING str)
#else /* __STDC__ */
static ThotBool XmlWriteCharData (xmlFile, str)
BinFile  xmlFile;
STRING   str;
#endif /* __STDC__ */
{
  ThotBool        ok = TRUE;
  int             i = 0;

  if (str != NULL)
    while (str[i] != '\0')
      {
	switch (str[i])
	  {
	    /* Entities are defined in constxml.h */
	  case '<':
	    ok = ok && TtaWriteByte (xmlFile,'&');
	    ok = ok && XmlWriteString (xmlFile, LT_ENTITY);
	    ok = ok && TtaWriteByte (xmlFile,';');
	    break;
	  case '>':
	    ok = ok && TtaWriteByte (xmlFile,'&');
	    ok = ok && XmlWriteString (xmlFile, GT_ENTITY);
	    ok = ok && TtaWriteByte (xmlFile,';');
	    break;
	  case '&':
	    ok = ok && TtaWriteByte (xmlFile,'&');
	    ok = ok && XmlWriteString (xmlFile, AMP_ENTITY);
	    ok = ok && TtaWriteByte (xmlFile,';');
	    break;
	  case '\'':
	    ok = ok && TtaWriteByte (xmlFile,'&');
	    ok = ok && XmlWriteString (xmlFile, APOS_ENTITY);
	    ok = ok && TtaWriteByte (xmlFile,';');
	    break;
	  case '"':
	    ok = ok && TtaWriteByte (xmlFile,'&');
	    ok = ok && XmlWriteString (xmlFile, QUOT_ENTITY);
	    ok = ok && TtaWriteByte (xmlFile,';');
	    break;
	  case '\212':
	    ok = ok && XmlWriteString (xmlFile, OPEN_TAG);
	    ok = ok && XmlWriteString (xmlFile, THOT_SCHEMA);
	    ok = ok && XmlWriteString (xmlFile, BR_TAG);
	    ok = ok && XmlWriteString (xmlFile, CLOSE_EMPTY_TAG);
	    break;
	  default:
	    ok = ok && TtaWriteByte (xmlFile, str[i]);
	    break;
	  }
	i++;
      }
  return ok;
}


/*----------------------------------------------------------------------
         XmlWriteInteger
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool        XmlWriteInteger (BinFile xmlFile, int n)
#else  /* __STDC__ */
static ThotBool        XmlWriteInteger (xmlFile, n)
BinFile             xmlFile;
int                 n;
#endif /* __STDC__ */
{
  CHAR_T tempChar[20];

  usprintf(tempChar, TEXT("%d"), n);
  return (XmlWriteString(xmlFile, tempChar));
}

/*----------------------------------------------------------------------
   	 XmlWriteElementName
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool XmlWriteElementName (BinFile xmlFile, ElementType elType)
#else /* __STDC__ */
static ThotBool XmlWriteElementName (xmlFile, elType)
BinFile  xmlFile;
ElementType elType;
#endif /* __STDC__ */
{
  return (XmlWriteString (xmlFile, NameThotToXml (elType.ElSSchema,
						  elType.ElTypeNum,
						  0, 0)));
}

/*----------------------------------------------------------------------
   	XmlWriteAttrName
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool  XmlWriteAttrName (BinFile xmlFile, STRING attrName)
#else /* __STDC__ */
static ThotBool XmlWriteAttrName (xmlFile, attrName)
BinFile  xmlFile;
STRING   attrName;
#endif /* __STDC__ */
{
  ThotBool        ok = TRUE;

  ok = ok && XmlWriteString (xmlFile,attrName);
  ok = ok && TtaWriteByte (xmlFile, '=');
  return ok;
}

/*----------------------------------------------------------------------
   	 XmlWriteAttrIntValue
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool XmlWriteAttrIntValue (BinFile xmlFile, int value)
#else /* __STDC__ */
static ThotBool XmlWriteAttrIntValue (xmlFile, value)
BinFile  xmlFile;
int             value;

#endif /* __STDC__ */
{
  ThotBool        ok;

  ok = TtaWriteByte (xmlFile, '"');
  ok = ok && XmlWriteInteger (xmlFile, value);
  ok = ok && TtaWriteByte (xmlFile, '"');
  return ok;
}

/*----------------------------------------------------------------------
   	 XmlWriteAttrStrValue
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool  XmlWriteAttrStrValue (BinFile xmlFile, STRING value)
#else /* __STDC__ */
static ThotBool  XmlWriteAttrStrValue (xmlFile, value)
BinFile  xmlFile;
STRING   value;
#endif /* __STDC__ */
{
  ThotBool ok;

  ok = TtaWriteByte (xmlFile, '"');
  ok = ok && XmlWriteCharData (xmlFile, value);
  ok = ok && TtaWriteByte (xmlFile, '"');
  return ok;
}

/*----------------------------------------------------------------------
   	XmlWriteComments: almost not used as there is no more comments
	                  in Thot documents
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool XmlWriteComments (BinFile xmlFile, STRING comments)
#else /* __STDC__ */
static ThotBool XmlWriteComments (xmlFile, comments)
BinFile xmlFile;
STRING  comments;
#endif /* __STDC__ */
{
  ThotBool ok;

  ok = XmlWriteString (xmlFile, OPEN_COMMENT);
  ok = ok && XmlWriteString (xmlFile, comments);
  ok = ok && XmlWriteString (xmlFile, CLOSE_COMMENT);
  return ok;
}

/*----------------------------------------------------------------------
   	XmlMakePrefix make the prefix from the first two letters of 
	the nature name and an arbitrary number
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING  XmlMakePrefix (SSchema sSchema)
#else /*__STDC__*/
static STRING  XmlMakePrefix (sSchema)
SSchema sSchema;
#endif /*__STDC__*/
{
  STRING       schemaName;
  CHAR_T       tempName[5];
  PrefixType*  newPrefix;

  /* Making prefix's name */
  schemaName = TtaGetSSchemaName (sSchema);
  usprintf (&tempName[2], TEXT("%d"), NbPrefix++);
  tempName[0] = schemaName[0];
  tempName[1] = schemaName[1];

  /* Adding prefix to internal structure */
  newPrefix = (PrefixType *) TtaGetMemory (sizeof (PrefixType));
  newPrefix->Next = Prefixs;
  Prefixs = newPrefix;
  newPrefix->Schema = sSchema;
  newPrefix->Name = TtaStrdup (tempName);

  return (newPrefix->Name);
}

/*----------------------------------------------------------------------
   	XmlWritePrefix gets the prefix from the SchemaName
	Returns NULL if S name isn't in list
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool          XmlWritePrefix (BinFile xmlFile, SSchema sSchema)
#else /*__STDC__*/
static ThotBool          XmlWritePrefix (xmlFile,sSchema)
BinFile xmlFile;
SSchema sSchema;
#endif /*__STDC__*/

{
  STRING         result = NULL;
  PrefixType     *tempPrefix;
  ThotBool        ok = TRUE;

  tempPrefix = Prefixs;
  while (result == NULL && tempPrefix != NULL)
    /* Searching prefix in internal structure */
    {
      if (TtaSameSSchemas(sSchema,tempPrefix->Schema))
	  result = tempPrefix->Name;
      tempPrefix = tempPrefix->Next;
    }
  if (result!=NULL)
    /* Writing prefix */
    {
      ok = ok && XmlWriteString (xmlFile, result);
      ok = ok && XmlWriteString (xmlFile, TEXT(":"));
    }
  return ok;
}

/*----------------------------------------------------------------------
   	XmlFreePrefixs: free internals prefixs structures
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void              XmlFreePrefixs ()
#else /*__STDC__*/
static void              XmlFreePrefixs ()
#endif /*__STDC__*/

{
  PrefixType         *tempPrefix;

  tempPrefix = Prefixs;
  while (tempPrefix != NULL)
    {
      Prefixs = tempPrefix;
      tempPrefix = tempPrefix->Next;
      TtaFreeMemory(Prefixs);
    }
  Prefixs = NULL;
  NbPrefix = 0;
}

/*----------------------------------------------------------------------
  XmlWriteVersionNumber: Write the current xml version
----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool XmlWriteVersionNumber (BinFile xmlFile)
#else /* __STDC__ */
static ThotBool XmlWriteVersionNumber (xmlFile)
BinFile  xmlFile;

#endif /* __STDC__ */
{
  ThotBool ok;

  ok = XmlWriteString (xmlFile, OPEN_XML);
  ok = ok && XmlWriteString (xmlFile, XML_SPACE);
  ok = ok && XmlWriteAttrName (xmlFile, TEXT("version"));
  /* Current XML version: 1 */
  ok = ok && XmlWriteAttrIntValue (xmlFile, 1);
  ok = ok && XmlWriteString(xmlFile, XML_SPACE);
  ok = ok && XmlWriteAttrName (xmlFile, TEXT("encoding"));
  ok = ok && XmlWriteAttrStrValue (xmlFile, TEXT("ISO-8859-1"));
  ok = ok && XmlWriteString(xmlFile, XML_SPACE);
  /* Document doesn't work with a DTD */
  ok = ok && XmlWriteAttrName (xmlFile, TEXT("standalone"));
  ok = ok && XmlWriteAttrStrValue (xmlFile, TEXT("yes"));
  ok = ok && XmlWriteString (xmlFile, CLOSE_XML);
  return ok;
}

/*----------------------------------------------------------------------
   WriteXmlHeader: writes the heading of the document 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool WriteXmlHeader (BinFile xmlFile, Document doc)
#else /* __STDC__ */
static ThotBool WriteXmlHeader (xmlFile, doc)
BinFile  xmlFile;
Document        doc;

#endif /* __STDC__ */
{
  /* Write the XML version heading */
  return XmlWriteVersionNumber (xmlFile);
}


/*----------------------------------------------------------------------
   XmlWriteDocSchemaNames: write additional structure schemas 
    used by the document doc as namespace attributes.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool XmlWriteDocSchemaNames (BinFile xmlFile, Document doc)
#else /* __STDC__ */
static ThotBool XmlWriteDocSchemaNames (xmlFile, doc)
BinFile  xmlFile;
Document        doc;

#endif /* __STDC__ */
{
  SSchema         tempSchema;
  CHAR_T          tempName[40];
  ThotBool        ok = TRUE;

  /* read document's extensions */
  tempSchema = NULL;
  TtaNextSchemaExtension (doc, &tempSchema);
  while (tempSchema != NULL)
    {
      ok = ok && XmlWriteString(xmlFile, _NEWLINE_);
      ok = ok && XmlWriteString(xmlFile, XML_NAMESPACE_ATTR);
      ok = ok && XmlWriteString(xmlFile, TEXT(":"));
      /* make and write the extension's prefix */
      /* prefix= first 2 letters of ext name and 1 arbitrary number */
      ok = ok && XmlWriteAttrName(xmlFile, XmlMakePrefix (tempSchema));
      /* write the extension's schema path  */
      /* Warning: signifying extension by strcating "_EXT" is not a 
	          permanent solution. Waiting more of XML NameSpaces... */
      ustrcpy (tempName, TtaGetSSchemaName (tempSchema));
      ustrcat (tempName, TEXT("_EXT"));
      ok = ok && XmlWriteAttrStrValue (xmlFile, tempName);
      TtaNextSchemaExtension (doc, &tempSchema);
    }

  /* read document's natures */
  tempSchema = NULL;
  TtaNextNature (doc, &tempSchema);
  while (tempSchema != NULL)
    {
      ok = ok && XmlWriteString(xmlFile, _NEWLINE_);
      ok = ok && XmlWriteString(xmlFile, XML_NAMESPACE_ATTR);
      ok = ok && XmlWriteString(xmlFile, TEXT(":"));
      /* make and write the extension's prefix */
      /* prefix=first 2 letters of ext and  1arbitrary number */
      ok = ok && XmlWriteAttrName(xmlFile, XmlMakePrefix (tempSchema));
      /* write the nature's schema path */
      ok = ok && XmlWriteAttrStrValue (xmlFile, TtaGetSSchemaName (tempSchema));
      TtaNextNature (doc, &tempSchema);
    }
  return ok;
}


/*----------------------------------------------------------------------
   XmlWriteSchemaPres: Write the doc's schemas presentation
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool XmlWriteSchemaPres (BinFile xmlFile, Document doc)
#else /* __STDC__ */
static ThotBool XmlWriteSchemaPres (xmlFile, doc)
BinFile  xmlFile;
Document doc;

#endif /* __STDC__ */
{
  PrefixType         *tempPrefix;
  ThotBool        ok = TRUE;

#ifdef INDENT
  int i;
  for (i=0;i<XmlDepth;i++)
    ok = ok && XmlWriteString (xmlFile, DEPTH_SPACE);
#endif
  /* thot element presschema for noticing presentations schemas */
  ok = XmlWriteString (xmlFile, OPEN_TAG);
  ok = ok && XmlWriteString (xmlFile, THOT_SCHEMA);
  ok = ok && XmlWriteString (xmlFile, PRES_SCHEMA_TAG);
  /* write the principal schema prefix */
  ok = ok && XmlWriteString(xmlFile, XML_SPACE);
  ok = ok && XmlWriteString (xmlFile, THOT_SCHEMA);
  ok = ok && XmlWriteAttrName (xmlFile, P_PREFIX_ATTR);
  ok = ok && XmlWriteAttrStrValue (xmlFile, DEFAULT_VALUE);
  /* write the principal schema presentation */
  ok = ok && XmlWriteString(xmlFile, XML_SPACE);
  ok = ok && XmlWriteString (xmlFile, THOT_SCHEMA);
  ok = ok && XmlWriteAttrName (xmlFile, P_SCHEMA_ATTR);
  ok = ok && XmlWriteAttrStrValue (xmlFile, 
	TtaGetPSchemaName(TtaGetDocumentSSchema (doc)));
  ok = ok && XmlWriteString (xmlFile, CLOSE_EMPTY_TAG);

  /* We use stocked prefixs to list natures and extensions */
  tempPrefix = Prefixs;
  while (tempPrefix != NULL)
    {
#ifdef INDENT
      for (i=0;i<XmlDepth;i++)
	ok = ok && XmlWriteString (xmlFile, DEPTH_SPACE);
#endif
      ok = ok && XmlWriteString (xmlFile, OPEN_TAG);
      ok = ok && XmlWriteString (xmlFile, THOT_SCHEMA);
      ok = ok && XmlWriteString (xmlFile, PRES_SCHEMA_TAG);      
      /* write the schema prefix */
      ok = ok && XmlWriteString(xmlFile, XML_SPACE);
      ok = ok && XmlWriteString (xmlFile, THOT_SCHEMA);
      ok = ok && XmlWriteAttrName (xmlFile, P_PREFIX_ATTR);
      ok = ok && XmlWriteAttrStrValue (xmlFile, tempPrefix->Name);
      /* write the schema presentation */
      ok = ok && XmlWriteString(xmlFile, XML_SPACE);
      ok = ok && XmlWriteString (xmlFile, THOT_SCHEMA);
      ok = ok && XmlWriteAttrName (xmlFile, P_SCHEMA_ATTR);
      ok = ok && XmlWriteAttrStrValue (xmlFile, 
	    TtaGetPSchemaName(tempPrefix->Schema));
      ok = ok && XmlWriteString (xmlFile, CLOSE_EMPTY_TAG);
      tempPrefix = tempPrefix->Next;
    }
  return ok;
}

/*----------------------------------------------------------------------
   XmlWriteReference: Writes a reference
                      See inside for names specifications
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool XmlWriteReference (BinFile xmlFile, Document doc, Element el, Attribute attr)
#else /* __STDC__ */
static ThotBool XmlWriteReference (xmlFile,doc,el,attr)
BinFile xmlFile;
Document doc;
Element el;
Attribute attr;
#endif /* __STDC__ */
{
  Element    refEl;
  Document   refDoc;
  CHAR_T     refDocName[50];
  CHAR_T     tempName[100];
  ThotBool   ok=TRUE;
  int        l;

  bzero(refDocName,50);

  if (el!=NULL)
    /* Talking about a reference element */
    TtaGiveReferredElement(el,&refEl,tempName,&refDoc);
  else if (attr!=NULL)
    /* Talking about a reference attribute */
    TtaGiveReferenceAttributeValue (attr, &refEl, tempName, &refDoc);
      
  if (tempName[0]!='\0' && refDoc==0)
    /* It's an external reference */
    {
      if ((refDoc = TtaGetDocumentFromName (tempName)) == 0)
	{
	  refDoc = TtaOpenDocument(tempName,0);
	  OpenedRefDoc[NbOpenedRefDoc] = refDoc;
	  TtaGiveReferredElement(el,&refEl,tempName,&refDoc);
	  NbOpenedRefDoc ++;
	}
    }

  if (refEl!=NULL)
    /* reference element exists */
    {
      if (el!=NULL)
	/* adding XLL attributes to reference elements */
	{
	  if (TtaIsInAnInclusion(el))
	    /* Inline element */
	    {
	      ok = ok && XmlWriteString(xmlFile, XML_SPACE);
	      ok = ok && XmlWriteString(xmlFile, XML_SCHEMA);
	      ok = ok && XmlWriteAttrName(xmlFile, XML_INLINE_ATTR);
	      ok = ok && XmlWriteAttrStrValue(xmlFile, TRUE_VALUE);
	    }
	  /* The necessary XLL link type attribute */
	  ok = ok && XmlWriteString(xmlFile, XML_SPACE);
	  ok = ok && XmlWriteString(xmlFile, XML_SCHEMA);
	  ok = ok && XmlWriteAttrName(xmlFile, XML_LINK_ATTR);
	  ok = ok && XmlWriteAttrStrValue (xmlFile,XML_SIMPLE_LINK);
	  /* Href attribute */
	  ok = XmlWriteString(xmlFile, XML_SPACE);	  
	  ok = ok && XmlWriteString(xmlFile, XML_SCHEMA);
	  ok = ok && XmlWriteAttrName(xmlFile, XML_HREF_ATTR);
	}

      l = 0;
      if (tempName[0] != EOS)
	{
	/* external reference */	  
	  TtaGetDocumentDirectory(refDoc,tempName,100);
	  ustrcat(tempName, TEXT("/"));
	  ustrcat(tempName,TtaGetDocumentName(refDoc));
	  l = ustrlen (tempName);
	}
      XmlGetElementLabel (&tempName[l], refEl);
      ok = ok && XmlWriteAttrStrValue (xmlFile,tempName);
    }
  return ok;
}

/*----------------------------------------------------------------------
   	XmlCloseRefDoc: Close the referred document opened
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void              XmlCloseRefDoc ()
#else /*__STDC__*/
static void              XmlCloseRefDoc ()
#endif /*__STDC__*/

{
  int i=0;

  while (i<NbOpenedRefDoc)
    {
      TtaCloseDocument(OpenedRefDoc[i]);
      OpenedRefDoc[i]=0;
      i++;
    }
  NbOpenedRefDoc=0;
}

/*----------------------------------------------------------------------
   XmlPutAttribut: write an attribut
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool XmlPutAttribut (BinFile xmlFile, Attribute attr, Document doc)
#else /* __STDC__ */
static ThotBool XmlPutAttribut (xmlFile, attr, doc)
BinFile xmlFile;
Attribute      attr;
Document       doc;

#endif /* __STDC__ */
{
  ThotBool        attrOK;
  AttributeType   attrType;
  int             attrKind;
  Element         elRef;
  SSchema         sSchema;
  STRING          tempChar;
  CHAR_T          tempName[100];
  int             tempLength;
  Document        docRef;
  ThotBool        ok = TRUE;

  TtaGiveAttributeType (attr,&attrType,&attrKind);
  sSchema = attrType.AttrSSchema;
  attrOK = TRUE;

  /* Warning: Xml export doesn't suppress fixed attribute */ 
  /* We do not write reference attribute that are NULL or point on Nothing*/
  if (attrKind==REF_ATTR_TYPE)
    {
      TtaGiveReferenceAttributeValue (attr, &elRef, tempName,&docRef);
      if (tempName==NULL)
	attrOK = FALSE;
    }
  if (attrOK)
    {
      ok = ok && XmlWriteString (xmlFile, XML_SPACE);
      /* attrType = 1 is the XML lang attribute */
      if (attrType.AttrTypeNum == 1)
	{
	  ok = ok && XmlWriteString(xmlFile, XML_SCHEMA);
	  ok = ok && XmlWriteAttrName (xmlFile, XML_LANG_ATTR);
	}
      else
	{
	  /* Write the attribute Name and its prefix     */
	  if (!TtaSameSSchemas (sSchema, TtaGetDocumentSSchema (doc)))
	    /* If schema isn't default schema, prefix is added */
	    ok = ok && XmlWritePrefix (xmlFile,sSchema);
	  ok = ok && XmlWriteAttrName (xmlFile,
				       NameThotToXml (sSchema, 0, attrType.AttrTypeNum, 0));
	}
      switch (attrKind)
	{
	case ENUM_ATTR_TYPE:
	  /* Warning: Integer value putted for enum value */
	  /* To improve */
	  ok = ok && XmlWriteAttrIntValue (xmlFile, TtaGetAttributeValue(attr));
	  break;
	case NUM_ATTR_TYPE:
	  ok = ok && XmlWriteAttrIntValue (xmlFile,TtaGetAttributeValue(attr));
	  break;
	case REF_ATTR_TYPE:
	  ok = XmlWriteReference(xmlFile, doc, NULL, attr);
	  break;
	case TEXT_ATTR_TYPE:
	  tempLength = TtaGetTextAttributeLength (attr) + 1;
	  tempChar = TtaAllocString (tempLength);
	  TtaGiveTextAttributeValue (attr, tempChar, &tempLength);
	  ok = ok && XmlWriteAttrStrValue (xmlFile, tempChar);
	  TtaFreeMemory(tempChar);
	  break;
	default:
	  break;
	}
    }
  return ok;
}
/*----------------------------------------------------------------------
  XmlSameText returns true if the element following el has the same
  attributes, specific presentation and both are nor referenced
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool         XmlSameText (Element el)
#else /* __STDC__ */
static ThotBool         XmlSameText (el)
Element         el;
#endif /* __STDC__ */
{
  PtrElement pEl1, pEl2;
  	
  writeBeginTextTag = TRUE;
  
  pEl1 = (PtrElement)el;
  if (pEl1 != NULL)
    {
      pEl2 = pEl1->ElNext;
      if (pEl2 != NULL && pEl1->ElLeafType == LtText && pEl1->ElTerminal)
	if (pEl2->ElTerminal && pEl2->ElLeafType == LtText)
	  if (pEl2->ElLanguage == pEl1->ElLanguage)
	    if (!pEl2->ElHolophrast && !pEl1->ElHolophrast)
	      if (SameAttributes (pEl1, pEl2))
		if (pEl1->ElSource == NULL && pEl2->ElSource == NULL)
		  if (BothHaveNoSpecRules (pEl1, pEl2))
		    if (pEl1->ElStructSchema->SsRule[pEl1->ElTypeNumber - 1].SrConstruct != CsConstant)
		      if (pEl2->ElStructSchema->SsRule[pEl2->ElTypeNumber - 1].SrConstruct != CsConstant)
			if (pEl1->ElSource == NULL && pEl2->ElSource == NULL)
			  writeBeginTextTag = FALSE;
    }
  return (!writeBeginTextTag);
}

/*----------------------------------------------------------------------
  XmlWriteLeaf: Write a leaf element in xmlFile
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool        XmlWriteLeaf (BinFile xmlFile,  Document doc, Element el,ThotBool taggedText)
#else /* __STDC__ */
static ThotBool        XmlWriteLeaf (xmlFile, doc, el, taggedText)
BinFile         xmlFile;
Element         el;
Document        doc;
ThotBool        taggedText;
#endif /* __STDC__ */
{
  ElementType     elType;
  Element         tempElem;
  int             x, y;
  SSchema         sSchema;
  STRING          tempChar;
  CHAR_T          s[2];
  Language        tempLanguage;
  int             tempLength;
  int             i;
  ThotBool        ok = TRUE;
  StrAtomPair     *atomPair, *prevAtomPair;
  CHAR_T          buf[32];

  elType = TtaGetElementType (el);
  sSchema = elType.ElSSchema;

  if (TtaGetConstruct(el) == ConstructConstant)
  /* We do not write constants text content, it is automaticly created */
    ;
  else if (TtaGetConstruct(el) == ConstructReference)
    /* Inline element */
      ok = ok && XmlWriteReference (xmlFile,doc, el, NULL);
  else
    {      
      if (TtaGetConstruct (el)== ConstructPair)
	{
	  if (TtaIsFirstPairedElement (el))
	    {
	      /* first paired element */
	      /* store the elment in list of first paired element */
	      atomPair = (StrAtomPair *) TtaGetMemory (sizeof (StrAtomPair));
	      atomPair->elem = el;
	      atomPair->next = ListAtomPair;
	      ListAtomPair = atomPair;
	      if (!TtaIsElementReferred(el))
		/* write element id if its done yet */
		{
		  ok = ok && XmlWriteString(xmlFile, XML_SPACE);
		  ok = ok && XmlWriteString(xmlFile, XML_SCHEMA);
		  ok = ok && XmlWriteAttrName (xmlFile, XML_ID_ATTR);
		  XmlGetElementLabel (buf, el);
		  ok = ok && XmlWriteAttrStrValue (xmlFile, buf);
		}
	    }
	  else
	    {
	      /* second paired element */
	      tempElem = TtaSearchOtherPairedElement(el);
	      if (tempElem != NULL)
		{
		  atomPair = ListAtomPair;
		  prevAtomPair = NULL;
		  while (atomPair != NULL && atomPair->elem != tempElem)
		    {
		      prevAtomPair = atomPair;
		      atomPair = atomPair->next;
		    }
		  if (atomPair != NULL)
		    {
		      ok = ok && XmlWriteString(xmlFile, XML_SPACE);
		      ok = ok && XmlWriteString(xmlFile, THOT_SCHEMA);     
		      ok = ok && XmlWriteAttrName(xmlFile, FPAIR_ATTR);
		      XmlGetElementLabel (buf, tempElem);
		      ok = ok && XmlWriteAttrStrValue (xmlFile, buf);
		      if (prevAtomPair == NULL)
			ListAtomPair = atomPair->next;
		      else
			prevAtomPair->next = atomPair->next;
		      TtaFreeMemory (atomPair);
		    }
		}
	    }
	} 
      switch (elType.ElTypeNum)
	{
	case PICTURE_UNIT:
	  /* Warning: What about the picture Type ? */
	  /* Put the image source attribute */
	  ok = XmlWriteString(xmlFile, XML_SPACE);
	  ok = ok && XmlWriteString(xmlFile, THOT_SCHEMA);
	  ok = ok && XmlWriteAttrName (xmlFile, SRC_ATTR);
	  tempLength = TtaGetTextLength(el)+1;
	  tempChar = TtaAllocString (tempLength);
	  TtaGiveTextContent (el, tempChar, &tempLength, &tempLanguage);
	  ok = ok && XmlWriteAttrStrValue (xmlFile, tempChar);
	  TtaFreeMemory(tempChar);
	  break;
	
	case TEXT_UNIT:
	  
	  /* Put text content  */
	  tempLength = TtaGetTextLength(el)+1;
	  tempChar = TtaAllocString (tempLength);
	  TtaGiveTextContent (el, tempChar, &tempLength, &tempLanguage);
	  /* language is specified before in TEXT tag */
	  if (taggedText || writeBeginTextTag)
	    /* A TEXT tag has been opened */
	    ok = ok && XmlWriteString (xmlFile, CLOSE_TAG);
	  /* Put Text content */
#ifdef INDENT
	  for (i = 0;i<XmlDepth+1;i++)
	    ok = ok && XmlWriteString (xmlFile, DEPTH_SPACE);
#endif
	  ok = ok && XmlWriteCharData (xmlFile, tempChar);
	  ok = ok && XmlWriteString (xmlFile, _NEWLINE_);
	  TtaFreeMemory(tempChar);
	  writeBeginTextTag = FALSE;
	  /* Warning: No API for associating Text sheets */
	  if (taggedText && !XmlSameText (el))
	  /* if TEXT tag opened, ends tag */
	    {
#ifdef INDENT
	      for (i=0 ; i<XmlDepth;i++)
		ok = ok && XmlWriteString (xmlFile, DEPTH_SPACE);
#endif
	      ok = ok && XmlWriteString (xmlFile, OPEN_END_TAG);
#ifdef NAMESPACE
	      if (!TtaSameSSchemas (sSchema, TtaGetDocumentSSchema (doc)))
		/* If schema is default schema, no prefix is added */
		ok = ok && XmlWritePrefix (xmlFile,sSchema);
#endif
	      ok = ok && XmlWriteElementName (xmlFile,elType);
	      ok = ok && XmlWriteString (xmlFile, CLOSE_TAG);
	    }
	  break;
	
	case REFERENCE_UNIT:
	  ok = ok && XmlWriteReference (xmlFile,doc, el, NULL);
	  break;
	
	case SYMBOL_UNIT:
	case GRAPHICS_UNIT:
	  /* write graphic code and polyline points */
	  s[0] = TtaGetGraphicsShape (el);
	  s[1] = '\0';
	  ok = ok && XmlWriteString(xmlFile, XML_SPACE);
	  ok = ok && XmlWriteString(xmlFile, THOT_SCHEMA);
	  ok = ok && XmlWriteAttrName (xmlFile, GRAPH_CODE_ATTR);
	  ok = ok && XmlWriteAttrStrValue (xmlFile, s);
	  if (elType.ElTypeNum ==  GRAPHICS_UNIT &&
	      (s[0]=='p'||s[0]=='S'||s[0]=='N'||s[0]=='U'||s[0]=='M'||
	       s[0]=='s'||s[0]=='B'||s[0]=='F'||s[0]=='A'||s[0]=='D'))
	    /* polylines types: write points  */
	    { 
	      ok = ok && XmlWriteString(xmlFile, XML_SPACE);
	      ok = ok && XmlWriteString(xmlFile, THOT_SCHEMA);
	      ok = ok && XmlWriteAttrName (xmlFile,LINE_POINTS_ATTR );
	      ok = ok && TtaWriteByte (xmlFile,'"');
	      for (i = 1; i <= TtaGetPolylineLength (el); i++)
		{
		  TtaGivePolylinePoint (el, i, UnPoint, &x, &y);
		  ok = ok && XmlWriteInteger (xmlFile, x);
		  ok = ok && TtaWriteByte(xmlFile,',');
		  ok = ok && XmlWriteInteger (xmlFile, y);
		  ok = ok && TtaWriteByte(xmlFile,';');
		}
		      ok = ok && TtaWriteByte(xmlFile,'"');
	    }
	  break;

	case PAGE_BREAK:
	  ok = ok && XmlWriteString(xmlFile, XML_SPACE);
	  ok = ok && XmlWriteString(xmlFile, THOT_SCHEMA);
	  ok = ok && XmlWriteAttrName (xmlFile, PG_NUM_ATTR);
	  ok = ok && XmlWriteAttrIntValue (xmlFile, ((PtrElement)el)->ElPageNumber);
	  ok = ok && XmlWriteString(xmlFile, XML_SPACE);
	  ok = ok && XmlWriteString(xmlFile, THOT_SCHEMA);
	  ok = ok && XmlWriteAttrName (xmlFile, PG_VIEW_ATTR);
	  ok = ok && XmlWriteAttrIntValue (xmlFile, ((PtrElement)el)->ElViewPSchema);
	  ok = ok && XmlWriteString(xmlFile, XML_SPACE);
	  ok = ok && XmlWriteString(xmlFile, THOT_SCHEMA);
	  ok = ok && XmlWriteAttrName (xmlFile, PG_TYPE_ATTR);
	  ok = ok && XmlWriteAttrIntValue (xmlFile, (int)((PtrElement)el)->ElPageType);
	  break;
	
	default:
	  break;
	}
    }
  if (elType.ElTypeNum != TEXT_UNIT)
    ok = ok && XmlWriteString (xmlFile, CLOSE_EMPTY_TAG);
  return ok;
}

/*----------------------------------------------------------------------
  XmlWriteAttributes : writes the attributes of el element
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool       XmlWriteAttributes (BinFile xmlFile, Element el, Document doc, ThotBool withEvent)
#else /* __STDC__ */
static ThotBool       XmlWriteAttributes (xmlFile, el, doc, withEvent)
BinFile  xmlFile;
Element        *pEl;
Document        doc;
ThotBool        withEvent;

#endif /* __STDC__ */
{	
  Attribute attr;
  CHAR_T          buf[100];
  NotifyAttribute notifyAttr;
  AttributeType   attrType;
  int             attrKind;
  ThotBool        ok = TRUE;
  ThotBool        doit;
  PRule           pRule;

  attr = NULL;
  TtaNextAttribute (el, &attr);
  pRule = NULL;
  TtaNextPRule (el, &pRule);

  /* If it's an included element copy then write the element reference */
  
  if (TtaIsInAnInclusion (el))
    ok = ok && XmlWriteReference (xmlFile, doc, el, NULL);
  
  if (TtaIsElementReferred(el))
    /* Write the element's label if he is referred */
    {
      ok = ok && XmlWriteString(xmlFile, XML_SPACE);
      ok = ok && XmlWriteString(xmlFile, XML_SCHEMA);
      ok = ok && XmlWriteAttrName (xmlFile, XML_ID_ATTR);
      XmlGetElementLabel (buf, el);
      ok = ok && XmlWriteAttrStrValue (xmlFile, buf);
    }
  if (TtaIsHolophrasted(el))
    /* Write holophrast attribute if holophrasted */
    {
      ok = ok && XmlWriteString(xmlFile, XML_SPACE);
      ok = ok && XmlWriteString(xmlFile, THOT_SCHEMA);
      ok = ok && XmlWriteAttrName (xmlFile, HOLOPHRASTE_ATTR);
      ok = ok && XmlWriteAttrStrValue (xmlFile, TEXT("true"));
    }
  /* Warning: What's imposed attributs ? */
  /*          Export doesn't associate special treatment */
  while (ok && attr != NULL)
    {
      if (withEvent)
	{
	  /* prepare and send the event AttrSave.Pre if asked */
	  notifyAttr.event = TteAttrSave;
	  notifyAttr.document = doc;
	  notifyAttr.element = el;
	  notifyAttr.attribute = attr;
	  TtaGiveAttributeType (attr, &attrType, &attrKind);
	  notifyAttr.attributeType.AttrTypeNum = attrType.AttrTypeNum;
	  notifyAttr.attributeType.AttrSSchema = attrType.AttrSSchema;
	  doit = !CallEventAttribute (&notifyAttr, TRUE);
	}
      else
	doit = TRUE;
      if (doit)
	/* Saving attribute accepted by the application */
	/* Write the attribut */
	ok = ok && XmlPutAttribut (xmlFile, attr, doc);
      if (ok && withEvent)
	{
	  /* prepare and send the event AttrSave.Post if asked */
	  notifyAttr.event = TteAttrSave;
	  notifyAttr.document = doc;
	  notifyAttr.element = el;
	  notifyAttr.attribute = attr;
	  TtaGiveAttributeType (attr, &attrType, &attrKind);
	  notifyAttr.attributeType.AttrTypeNum = attrType.AttrTypeNum;
	  notifyAttr.attributeType.AttrSSchema = attrType.AttrSSchema;
	  CallEventAttribute (&notifyAttr, FALSE);
	}
      TtaNextAttribute (el, &attr);
    }
  /* Warning: No API to know pEl->ElPictInfo->PicPresent */
  /*          to know if it's realsize, rescale,fillframe...*/
  /*          Insert picture type: */
  /* missing an attribute then */
  
  /* Write the element's presentation rules */
  if (pRule!=NULL)
    {
      int typerule;
      int value;

      ok = ok && XmlWriteString(xmlFile, XML_SPACE);
      ok = ok && XmlWriteString(xmlFile, THOT_SCHEMA);
      ok = XmlWriteAttrName(xmlFile, STYLE_ATTR);
      ok = ok && XmlWriteString(xmlFile, TEXT("\""));
      while (pRule!=NULL)
	{	
	  typerule = TtaGetPRuleType(pRule);
	  if (typerule != PtPictInfo && ((PtrPRule) pRule)->PrType != PtFunction)
	    {
	      ok = ok && XmlWriteString(xmlFile, TtaGetViewName(doc,TtaGetPRuleView(pRule)));
	      ok = ok && XmlWriteString(xmlFile, TEXT(":"));
	      ok = ok && XmlWriteInteger(xmlFile, typerule);
	      ok = ok && XmlWriteString(xmlFile, TEXT(":"));
	      
	      value = TtaGetPRuleValue(pRule);
	      if ( typerule == PtHeight ||
		   typerule == PtWidth ||
		   typerule == PtVertPos ||
		   typerule == PtHorizPos || 
		   typerule == PtBreak1 || 
		   typerule == PtBreak2 || 
		   typerule == PtIndent || 
		   typerule == PtSize || 
		   typerule == PtLineSpacing || 
		   typerule == PtLineWeight)
		{ 
		  /* write the unit */
		  ok = ok && XmlWriteInteger(xmlFile, TtaGetPRuleUnit(pRule));
		  ok = ok && XmlWriteString(xmlFile, TEXT(":"));
		}
	  
	      if ((typerule == PtHeight || typerule == PtWidth))
		{
		  /* write the flag indicating if it is */
		  /* absolute or relative value */
		  if(((PtrPRule) pRule)->PrDimRule.DrAbsolute)
		    ok = ok && XmlWriteString(xmlFile, TEXT("A"));
		  else
		    ok = ok && XmlWriteString(xmlFile, TEXT("R"));
		  ok = ok && XmlWriteString(xmlFile, TEXT(":"));
		}

	      /* write the sign */
	      if (typerule == PtHeight ||
		  typerule == PtWidth ||
		  typerule == PtVertPos ||
		  typerule == PtHorizPos || 
		  typerule == PtIndent)
		if (value < 0)
		  ok = ok && XmlWriteString(xmlFile, TEXT("-"));
	      
	      /* write the value */
	      ok = ok && XmlWriteInteger(xmlFile, abs (value));
	      ok = ok && XmlWriteString(xmlFile, TEXT(";"));
	    }
	  TtaNextPRule (el, &pRule);
	}
      ok = ok && XmlWriteString(xmlFile, TEXT("\""));
    }  
  return ok;
}

/*----------------------------------------------------------------------
  XmlExternalise: Export in XML forma the subtree *pEl
                  Notation *pEL is used to return last Text leaf in case of
		  text element but, as no API exist for associating text
		  leaves, it's no use using an element pointer.
		  We shall implement that.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool        XmlExternalise (BinFile xmlFile, Element *pEl, Document doc, ThotBool withEvent)
#else /* __STDC__ */
static ThotBool        XmlExternalise (xmlFile, pEl, doc, withEvent)
BinFile  xmlFile;
Element        *pEl;
Document        doc;
ThotBool        withEvent;

#endif /* __STDC__ */
{
  Element         elChild, el;
  ElementType     elType;
  Attribute       attr;
  SSchema         sSchema;
  PRule           pRule;

  NotifyElement   notifyEl;
  ThotBool        toWrite = TRUE;
  ThotBool        taggedText = TRUE;
  ThotBool        ok = TRUE;
  ThotBool        doit = TRUE;
#ifdef INDENT
  int             i;
#endif      

  el = *pEl;
  elType = TtaGetElementType (el);
  sSchema = elType.ElSSchema;
  pRule = NULL;
  TtaNextPRule (el, &pRule);
  attr = NULL;
  TtaNextAttribute (el, &attr);

  /* Test if name has an XML translation or is not first PageBreak in view */
  toWrite = 
    (NameThotToXml (elType.ElSSchema, elType.ElTypeNum, 0, 0) != NULL &&
     (elType.ElTypeNum != 6 || ((PtrElement)el)->ElPageType != PgBegin));
  /* test if the text element has to be tagged, i.e. has attributes */
  taggedText = 
    toWrite &&
    !(elType.ElTypeNum==1 && pRule==NULL && attr==NULL && 
      !TtaIsInAnInclusion (el) && 
      !TtaIsElementReferred(el) && 
      !TtaIsHolophrasted(el));

  if (toWrite && taggedText && 
      (elType.ElTypeNum != 1 || writeBeginTextTag))
    /* We write that Tag */
    {
      /* Write depth spaces */
      XmlDepth++;
#ifdef INDENT
      for (i=0;i<XmlDepth;i++)
	ok = ok && XmlWriteString (xmlFile, DEPTH_SPACE);
#endif      
      /* Open the Tag */
      ok = XmlWriteString (xmlFile, OPEN_TAG);
      
      /* Write the Tag name (= element Name) and its prefix     */
      /*       Name is took from the ThotXmlTable */
      if (!TtaSameSSchemas (sSchema, TtaGetDocumentSSchema (doc)))
	/* If schema is default schema, no prefix is added */
	ok = ok && XmlWritePrefix (xmlFile,sSchema);
      
      ok = ok && XmlWriteElementName (xmlFile,elType); 
     
      ok = ok && XmlWriteAttributes (xmlFile, el, doc, withEvent);
    }

  if (ok && TtaIsInAnInclusion (el))
    /* Don't write included element's body */
    {
      ok = ok && XmlWriteString (xmlFile, CLOSE_EMPTY_TAG);
    }
  else if (ok)
    /* Write the element's body */
    {
      if (TtaIsLeaf (elType) && (toWrite || 
				 (elType.ElTypeNum==1 && !taggedText)))
	/* it's a terminal element and to write */
	{
	  ok = ok && XmlWriteLeaf (xmlFile,doc,el,taggedText);
	}
      else
	/* it isn't a terminal element */
	if (ok)
	  /* We write element's child */
	  /* Warning: No API for knowing if it's a S paramaeter */
	  /*          In pivot version there was a test whether */
	  /*            the element was a S parameters or not: */
	  /*            if (!pSS->SsRule[pEl1->ElTypeNum - 1].SrParamElem) */
	  {
	    if (toWrite)
	      /* Ends heading if toWrite */
	      ok = ok && XmlWriteString (xmlFile, CLOSE_TAG);
	    elChild = TtaGetFirstChild (el);
	    /* successively write element's children */
	    while (ok && elChild != NULL)
	      {
		if (withEvent)
		  {
		    /* send evenement ElemSave.Pre to application, if asked */
		    notifyEl.event = TteElemSave;
		    notifyEl.document = doc;
		    notifyEl.element = elChild;
		    notifyEl.elementType.ElTypeNum =
		      (TtaGetElementType (elChild)).ElTypeNum;
		    notifyEl.elementType.ElSSchema =
		      (TtaGetElementType (elChild)).ElSSchema;
		    notifyEl.position = 0;
		    doit = !CallEventType ((NotifyEvent *) & notifyEl, TRUE);
		  }
		else 
		  doit = TRUE;
		if (doit)
		  /* Saving element accepted by the application */
		  /* Write an element's child */
		  ok = ok && XmlExternalise (xmlFile, &elChild, doc, withEvent);
		if (withEvent && ok)
		  /* send evenement ElemSave.Post to application, if asked */
		  {
		    notifyEl.event = TteElemSave;
		    notifyEl.document = doc;
		    notifyEl.element = elChild;
		    notifyEl.elementType.ElTypeNum =
		      (TtaGetElementType (elChild)).ElTypeNum;
		    notifyEl.elementType.ElSSchema =
		      (TtaGetElementType (elChild)).ElSSchema;
		    notifyEl.position = 0;
		  }
		/* Go to next child */
		TtaNextSibling (&elChild);
	      }
	    /* close TAG if toWrite*/
	    if (toWrite)
	      {
#ifdef INDENT
		for (i=0;i<XmlDepth;i++)
		  ok = ok && XmlWriteString (xmlFile, DEPTH_SPACE);
#endif
		ok = ok && XmlWriteString (xmlFile, OPEN_END_TAG);
		if (!TtaSameSSchemas (sSchema, TtaGetDocumentSSchema (doc)))
		  /* If schema is default schema, no prefix is added */
		  ok = ok && XmlWritePrefix (xmlFile,sSchema);
		ok = ok && XmlWriteElementName (xmlFile,elType);
		ok = ok && XmlWriteString (xmlFile, CLOSE_TAG);
	      }
	  }
	else
	  if (toWrite)
	    /* Write end of an inclusion */
	    ok = ok && XmlWriteString (xmlFile, CLOSE_EMPTY_TAG);
    }
  if (toWrite&&taggedText)
    XmlDepth--;
  return ok;
}

/*----------------------------------------------------------------------
   XmlWriteDocAssocRoot: Write the doc associed trees
                         Those trees are at same level that document
			 root tree.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool XmlWriteDocAssocRoot (BinFile xmlFile, Document doc, ThotBool withEvent)
#else /* __STDC__ */
static ThotBool XmlWriteDocAssocRoot (xmlFile, doc, withEvent)
BinFile         xmlFile;
Document        doc;
ThotBool        withEvent;
#endif /* __STDC__ */
{
  Element         nextEl, rootEl;
  ElementType     rootType, elType;
  NotifyElement   notifyEl;
  ThotBool        ok = TRUE;
  ThotBool        doit = TRUE;
  
  rootEl = NULL;
  TtaNextAssociatedRoot (doc, &rootEl);
  while (rootEl != NULL)
    {
      rootType = TtaGetElementType (rootEl);
      nextEl = TtaGetFirstChild (rootEl);
      if (nextEl != NULL)
	/* Is there anything else then PageBreak */
	{
	  if (TtaGetElementType (nextEl).ElTypeNum == PAGE_BREAK)
	    nextEl = TtaSearchNoPageBreak (nextEl, TRUE);

	  if (nextEl != NULL)
	    /*there's not only PageBreak */
	    {
	      if (withEvent)
		{
		  /* send evenement ElemSave.Pre to application, if asked */
		  notifyEl.event = TteElemSave;
		  notifyEl.document = doc;
		  notifyEl.element = rootEl;
		  notifyEl.elementType.ElTypeNum = rootType.ElTypeNum;
		  notifyEl.elementType.ElSSchema = rootType.ElSSchema;
		  notifyEl.position = 0;
		  doit = !CallEventType ((NotifyEvent *) & notifyEl, TRUE);
		}
	      else
		doit = TRUE;
	      /* Saving element accepted by the application */
	      if (doit)
		/* write assoc root */
		XmlDepth++;
#ifdef INDENT
	      for (i=0;i<XmlDepth;i++)
		ok = ok && XmlWriteString (xmlFile, DEPTH_SPACE);
#endif      
	      /* Open the root Tag */
	      ok = XmlWriteString (xmlFile, OPEN_TAG);
	      /* Write the Tag name (= element Name) and its prefix     */
	      /*       Name is took from the ThotXmlTable */
	      if (!TtaSameSSchemas (rootType.ElSSchema, 
				    TtaGetDocumentSSchema (doc)))
		/* If schema is default schema, no prefix is added */
		ok = ok && XmlWritePrefix (xmlFile,rootType.ElSSchema);
      
	      ok = ok && XmlWriteElementName (xmlFile,rootType); 
	      /* write assoc tree thot ns attribute */
	      ok = ok && XmlWriteString(xmlFile, XML_SPACE);
	      ok = ok && XmlWriteString(xmlFile, THOT_SCHEMA);
	      ok = ok && XmlWriteAttrName (xmlFile, ASSOC_TREE_ATTR);
	      ok = ok && XmlWriteAttrStrValue (xmlFile, TRUE_VALUE);

	      /* write assoc tree attributes */
	      ok = ok && XmlWriteAttributes (xmlFile, rootEl, doc, withEvent);
	      ok = ok && XmlWriteString (xmlFile, CLOSE_TAG);
	      if (ok)
		{
		  /* successively write root's children */
		  while (ok && nextEl != NULL)
		    {
		      elType = TtaGetElementType (nextEl);
		      if (withEvent)
			{
			  /* send evenement ElemSave.Pre to application, if asked */
			  notifyEl.event = TteElemSave;
			  notifyEl.document = doc;
			  notifyEl.element = nextEl;
			  notifyEl.elementType.ElTypeNum = elType.ElTypeNum;
			  notifyEl.elementType.ElSSchema = elType.ElSSchema;
			  notifyEl.position = 0;
			  doit = !CallEventType ((NotifyEvent *) & notifyEl, TRUE);
			}
		      else 
			doit = TRUE;
		      if (doit)
			/* Saving element accepted by the application */
			/* Write an element's child */
			ok = ok && XmlExternalise (xmlFile, &nextEl, doc, withEvent);
		      if (withEvent && ok)
			/* send evenement ElemSave.Post to application, if asked */
			{
			  notifyEl.event = TteElemSave;
			  notifyEl.document = doc;
			  notifyEl.element = nextEl;
			  notifyEl.elementType.ElTypeNum = elType.ElTypeNum;
			  notifyEl.elementType.ElSSchema = elType.ElSSchema;
			  notifyEl.position = 0;
			}
		      /* Go to next child */
		      TtaNextSibling (&nextEl);
		    }	  
		}
	      /*  assoc root end tag*/
#ifdef INDENT
	      for (i=0;i<XmlDepth;i++)
		ok = ok && XmlWriteString (xmlFile, DEPTH_SPACE);
#endif
	      ok = ok && XmlWriteString (xmlFile, OPEN_END_TAG);
	      if (!TtaSameSSchemas (rootType.ElSSchema,
				    TtaGetDocumentSSchema (doc)))
		/* If schema is default schema, no prefix is added */
		ok = ok && XmlWritePrefix (xmlFile,rootType.ElSSchema);
	      ok = ok && XmlWriteElementName (xmlFile,rootType);
	      ok = ok && XmlWriteString (xmlFile, CLOSE_TAG);
	      if (withEvent && ok)
		{
		  /* send evenement ElemSave.Post to application, if asked */
		  notifyEl.event = TteElemSave;
		  notifyEl.document = doc;
		  notifyEl.element = rootEl;
		  notifyEl.elementType.ElTypeNum = rootType.ElTypeNum;
		  notifyEl.elementType.ElSSchema = rootType.ElSSchema;
		  notifyEl.position = 0;
		  CallEventType ((NotifyEvent *) & notifyEl, FALSE);
		}
	    }
	}
      TtaNextAssociatedRoot (doc, &rootEl);
    }
  return ok;
}

/*----------------------------------------------------------------------
   SauveXmlDoc:	save Document in 'xmlFile' file in XML format
      File must be opened before calling this functions and will still
      be after
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool        SauveXmlDoc (BinFile xmlFile, Document doc, ThotBool withEvent)
#else /* __STDC__ */
ThotBool        SauveXmlDoc (xmlFile, doc, withEvent)
BinFile         xmlFile;
Document        doc;
ThotBool        withEvent;

#endif /* __STDC__ */
{
  Element         el, elChild;
  NotifyElement   notifyEl;
  ThotBool        ok;
  ThotBool        doit = TRUE;
  SSchema         schema;
  StrAtomPair     *atomPair;

  ListAtomPair = NULL;
  /* write the xmlFile header */
  ok = WriteXmlHeader (xmlFile, doc);
  /* write doc associate comments if it exists  */
  /* Warning: No API function for accessing comments */
  /*   if (doc->DocComment != NULL) */
  /*     ok = ok && PutComment (xmlFile, pDoc->DocComment); */
  ok = ok && XmlWriteComments(xmlFile, TEXT("Generated by Thot"));
  /* Warning: Structure: Open thot special document begin tag  */
  /* This tag is used to have previous declaration of the main 
     document schema and to have associated tree at the same level
     than root tree */
  /* Write document's body */
  
  el = TtaGetMainRoot (doc);
  if (el != NULL)
    {
      if (withEvent)	
	{	
	  /* send evenement ElemSave.Pre to application, if asked */
	  notifyEl.event = TteElemSave;
	  notifyEl.document = doc;
	  notifyEl.element = el;
	  notifyEl.elementType.ElTypeNum =
	    (TtaGetElementType (el)).ElTypeNum;
	  notifyEl.elementType.ElSSchema =
	    (TtaGetElementType (el)).ElSSchema;
	  notifyEl.position = 0;
	  doit = !CallEventType ((NotifyEvent *) & notifyEl, TRUE);
	}
      else
	doit = TRUE;
    }
  if (doit)
    {
      ok = ok && XmlWriteString (xmlFile, OPEN_TAG);
      /* Write the main str schema as root tag GI */
      schema = TtaGetDocumentSSchema (doc);
      ok = ok && XmlWriteString (xmlFile, TtaGetSSchemaName (schema));  
      /* Write additional schemas as namespace attributes  */
      ok = ok && XmlWriteDocSchemaNames (xmlFile, doc);
      /*wirte root element attributes */
      ok = ok && XmlWriteAttributes (xmlFile, el, doc, withEvent);
      ok = ok && XmlWriteString (xmlFile, CLOSE_TAG);
      
      ok = ok && XmlWriteString (xmlFile, _NEWLINE_);
      /* Write the schema presentations */

      ok = ok && XmlWriteSchemaPres (xmlFile, doc);
      ok = ok && XmlWriteString (xmlFile, _NEWLINE_);
	
      if (ok)
	/* We write root element's child */
	/* Warning: No API for knowing if it's a S paramaeter */
	/*          In pivot version there was a test whether */
	/*            the element was a S parameters or not: */
	/*            if (!pSS->SsRule[pEl1->ElTypeNum - 1].SrParamElem) */
	{
	  elChild = TtaGetFirstChild (el);
	  /* successively write element's children */
	  while (ok && elChild != NULL)
	    {
	      if (withEvent)
		{
		  /* send evenement ElemSave.Pre to application, if asked */
		  notifyEl.event = TteElemSave;
		  notifyEl.document = doc;
		  notifyEl.element = elChild;
		  notifyEl.elementType.ElTypeNum =
		    (TtaGetElementType (elChild)).ElTypeNum;
		  notifyEl.elementType.ElSSchema =
		    (TtaGetElementType (elChild)).ElSSchema;
		  notifyEl.position = 0;
		  doit = !CallEventType ((NotifyEvent *) & notifyEl, TRUE);
		}
	      else 
		doit = TRUE;
	      if (doit)
		/* Saving element accepted by the application */
		/* Write an element's child */
		ok = ok && XmlExternalise (xmlFile, &elChild, doc, withEvent);
	      if (withEvent && ok)
		/* send evenement ElemSave.Post to application, if asked */
		{
		  notifyEl.event = TteElemSave;
		  notifyEl.document = doc;
		  notifyEl.element = elChild;
		  notifyEl.elementType.ElTypeNum =
		    (TtaGetElementType (elChild)).ElTypeNum;
		  notifyEl.elementType.ElSSchema =
		    (TtaGetElementType (elChild)).ElSSchema;
		  notifyEl.position = 0;
		}
	      /* Go to next child */
	      TtaNextSibling (&elChild);
	    }
	}
      /* Write non-empty associated trees */
      ok = ok && XmlWriteDocAssocRoot (xmlFile, doc, withEvent);
	  
      /* close root element tag */
#ifdef INDENT
      for (i=0;i<XmlDepth;i++)
	ok = ok && XmlWriteString (xmlFile, DEPTH_SPACE);
#endif
      ok = ok && XmlWriteString (xmlFile, OPEN_END_TAG);
      ok = ok && XmlWriteString (xmlFile,TtaGetSSchemaName (schema));
      ok = ok && XmlWriteString (xmlFile, CLOSE_TAG);
      /* send evenement ElemSave.Post to application, if asked */
      if (withEvent && ok)
	{
	  notifyEl.event = TteElemSave;
	  notifyEl.document = doc;
	  notifyEl.element = el;	 
	  notifyEl.elementType.ElTypeNum = (TtaGetElementType (el)).ElTypeNum;
	  notifyEl.elementType.ElSSchema = (TtaGetElementType (el)).ElSSchema;
	  notifyEl.position = 0;
	  CallEventType ((NotifyEvent *) & notifyEl, FALSE);
	}
    }
  
  XmlFreePrefixs();
  XmlCloseRefDoc();
  while (ListAtomPair != NULL)
    {  
      atomPair = ListAtomPair;
      ListAtomPair = ListAtomPair->next;
      free (atomPair);
    }
  return ok;
}

