/*

 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
/*
   module: xmlmodule.c
   Authors: Monte Regis
            Bonhomme Stephane

   Comments: this module manage XML/XLL special attributes
             while parsing XML documents
	     this module uses constant defined in constxml.h

   Extern function: 
     - ThotBool ParseXMLAttribute (PrefixType **pPrefixs,
                                   Document doc,
				   Element el, 
				   STRING attrName, 
				   STRING value)
            Parse the Thot attribute (pPrefixs is the parser prefix list
	      for opening document's nature in XmlAddNS. updated.)
     - void  XmlSetRef(Document doc)
            Update the references when parsing has finished
	    (All elements shall be created to update internals references 
 */
#define MAX_ATTR 100;

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "ustring.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "tree.h"
#include "content.h"
#include "view.h"
#include "language.h"
#include "attribute.h"
#include "document.h"
#include "application.h"
#include "appstruct.h"
#include "reference.h"
#include "constxml.h"
#include "typexml.h"
#include "edit_tv.h"

/* temporary pointer to parser prefixlist */
static PrefixType *ParserPrefixs;

/* reference storing structure */
typedef struct _XmlReferenceType
{
  Element       El;
  Attribute     Attr;
  Document      TargetDoc;
  STRING        TargetLabel;
  ThotBool       IsInclusion;
  struct _XmlReferenceType *Next;
}XmlReferenceType;

/* references list */
static XmlReferenceType *ReferenceList = NULL;


#include "parsexml_f.h"
#include "xmlmodule_f.h"

/*----------------------------------------------------------------------
    XmlAddNSPresentation: add a presentation for a given namespace
       this actually loads the SSchema and Pschema.
       if prefix is null this is the main SSchema of the doc.
   ----------------------------------------------------------------------*/
#ifdef __STDC__
void XmlAddNSPresentation (Document doc, STRING prefix, STRING presentationName)
#else /* __STDC__ */
void XmlAddNSPresentation (doc, prefix, presentationName)
Document doc;
STRING prefix;
STRING presentationName;
#endif /* __STDC__ */
{
  PrefixType         *courPrefix;
 
  if (presentationName != NULL && presentationName[0]!= EOS && doc!=0)
    {
      if (prefix == NULL || prefix[0] == EOS)
	TtaSetPSchema (doc, presentationName);
      else
	{
	  courPrefix = ParserPrefixs;
	  while (courPrefix != NULL && ustrcmp (courPrefix->Name, prefix))
	    courPrefix = courPrefix->Next;
	  if (courPrefix != NULL)
	    {
	      if (courPrefix->IsExtSchema)
		courPrefix->Schema = TtaNewSchemaExtension (doc, courPrefix->SchemaName, presentationName);
	      else
		courPrefix->Schema = TtaNewNature (TtaGetDocumentSSchema (doc), courPrefix->SchemaName, presentationName);
	    }
	}
    }
}
                   
 /*----------------------------------------------------------------------
    XmlAddNS: add a nature or extension in the document
      Schemas aren't added here because we have to know the
      presentation schema to add them. Waiting API
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void XmlAddNSSchema (Document doc, STRING prefixName, STRING schemaName)
#else /* __STDC__ */
void XmlAddNSSchema (doc, prefixName, schemaName)
Document doc;
STRING prefixName;
STRING schemaName;
#endif /* __STDC__ */
{
  PrefixType         *newPrefix;
  CHAR_T              buf[32];
  
  buf[0]='\0';
  if (schemaName != NULL && prefixName!= NULL && schemaName[0]!= EOS && doc!=0)
    if (XmlGetNSSchema (prefixName) == NULL)
      {
	newPrefix = (PrefixType *) TtaGetMemory (sizeof (PrefixType));
	newPrefix->Next = ParserPrefixs;
	ParserPrefixs = newPrefix;
	newPrefix->Name = TtaStrdup (prefixName);
	newPrefix->Schema = TtaGetSSchema (schemaName, doc);
	newPrefix->IsExtSchema = FALSE;
	if (ustrlen (schemaName) > 4 && !ustrcmp (&schemaName[ustrlen (schemaName) - 4], TEXT("_EXT")))
	  /* We've read an extension */
	  {
	    if (newPrefix->Schema == NULL)
	      schemaName [ustrlen (schemaName) - 4] = EOS;
	    newPrefix->Schema = TtaNewSchemaExtension (doc, schemaName, 
						       buf);
	    newPrefix->IsExtSchema = TRUE;
	  }
	else if (newPrefix->Schema == NULL)
	  newPrefix->Schema = TtaNewNature (TtaGetDocumentSSchema (doc),
					    schemaName, 
					    buf); 
	
	newPrefix->SchemaName = TtaStrdup (schemaName);

      }
    else
      XmlError(doc, TEXT("No default schema given"));
}
/*----------------------------------------------------------------------
  XmlGetPrefixSchema: seach the prefix associated schema
  ----------------------------------------------------------------------*/
#ifdef __STDC__
SSchema XmlGetNSSchema (STRING prefix)
#else /* __STDC__ */
SSchema XmlGetNSSchema (prefix)
STRING prefixName;
#endif /* __STDC__ */
{
  PrefixType  *courPrefix;
  
  courPrefix = ParserPrefixs;
  while (courPrefix != NULL && ustrcmp (courPrefix->Name, prefix))
    courPrefix = courPrefix->Next;
  if (courPrefix!=NULL)
    return (courPrefix->Schema);
  else
    return ((SSchema) NULL);
}

/*----------------------------------------------------------------------
   XmlAddRef: add a reference in list for later updating
              attr=NULL if reference element
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void XmlAddRef (Document doc, Element el, Attribute attr)
#else /* __STDC__ */
void XmlAddRef (doc, el, attr)
Document doc;
Element el;
Attribute attr;
#endif /* __STDC__ */
{
  XmlReferenceType  *newRef;

  if (el != NULL)
    {
      newRef = (XmlReferenceType *) TtaGetMemory (sizeof (XmlReferenceType));
      newRef->El = el;
      newRef->Attr = attr;
      newRef->TargetDoc = 0;
      newRef->TargetLabel = NULL;
      newRef->IsInclusion = FALSE;
      newRef->Next = ReferenceList;
      ReferenceList = newRef;
    }
}

/*----------------------------------------------------------------------
   XmlSetTarget: update the reference's target label
                 after xml:href attribute (for element) or ref attr.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void  XmlSetTarget (Document doc, Element el, STRING value)
#else /* __STDC__ */
void  XmlSetTarget (doc, el, value)
Document doc;
Element el;
STRING value;
#endif /* __STDC__ */
{
  CHAR_T docName[100];
  CHAR_T label[20];
  int    i = 0;
  XmlReferenceType *tempRef;

  if (el != NULL)
    {
      tempRef = ReferenceList;
      while (tempRef != NULL)
	{
	  if (tempRef->El == el)
	/* Reference is found */
	    {
	      while (value[i] != '#')
		{
		  docName[i] = value[i];
		  i++;
		}
	      if (i != 0)
		/* Warning: Reference from another document */
		{/* To Complete */}
	      ustrcpy(label, &value[i]);
	      tempRef->TargetLabel = TtaStrdup (label);
	      break;
	    }
	  tempRef = tempRef->Next;
	}
    }
}

/*----------------------------------------------------------------------
   XmlSetRef: Update references after the end of parsing
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void XmlSetRef (Document doc)
#else /* __STDC__ */
void XmlSetRef (doc)
#endif /* __STDC__ */
{
  XmlReferenceType *tempRef;
  XmlReferenceType *inter;

  tempRef = ReferenceList;
  while (tempRef != NULL)
    {
      if (tempRef->TargetLabel != NULL && tempRef->El != NULL)
	{
	  /* in same document */
	  if (tempRef->TargetDoc == 0)
	    tempRef->TargetDoc = doc;
	  /* To complete */
	  else
	    {
	    }
	  /* reference attribute */
	  if (tempRef->Attr!=NULL)
	    {
	      TtaSetAttributeReference(tempRef->Attr,
				       tempRef->El,
				       doc,
				       TtaSearchElementByLabel(tempRef->TargetLabel,
							       TtaGetMainRoot( tempRef->TargetDoc)),
				       tempRef->TargetDoc);
	    }
	  else
	    /* reference element */
	    {
	      TtaSetReference (tempRef->El,doc,
			       TtaSearchElementByLabel(tempRef->TargetLabel,
						       TtaGetMainRoot(tempRef->TargetDoc)),
			       tempRef->TargetDoc);
	    }
	}
      inter = tempRef;
      tempRef = tempRef->Next;
      TtaFreeMemory (inter->TargetLabel);
      TtaFreeMemory (inter);
    }
  ReferenceList = NULL;
}

/*----------------------------------------------------------------------
   XmlSetLinkType: XLL composed links ain't implemented yet
                   We consider it always "simple"
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void  XmlSetLinkType (Document doc, Element el, STRING value)
#else /* __STDC__ */
static void  XmlSetLinkType (doc, el, value)
Document doc;
Element el;
STRING value;
#endif /* __STDC__ */
{
  
}

/*----------------------------------------------------------------------
   XmlSetLabel: set an element label
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void  XmlSetLabel (Document doc,Element el, STRING value)
#else /* __STDC__ */
static void  XmlSetLabel (doc, el, value)
Document doc;
Element el;
STRING value;
#endif /* __STDC__ */
{
  /* Warning: Needed API for setting label*/
  if (el !=NULL)
    ustrcpy(((PtrElement)el)->ElLabel, value);
}

/*----------------------------------------------------------------------
   XmlSetLanguage
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void XmlSetLanguage (Document doc, Element el, STRING value)
#else /* __STDC__ */
static void XmlSetLanguage (doc, el, value)
Document doc; 
Element el; 
STRING value;
#endif /* __STDC__ */
{
  PtrDocument   pDoc;
  Attribute	attr;
  AttributeType	attrType;
  Language      lang;
  ThotBool      found;
  int           i;
  if (el != NULL)
    {
      attrType.AttrTypeNum = 1; /* langage attr */
      attrType.AttrSSchema = TtaGetElementType(el).ElSSchema;
      
      attr = TtaGetAttribute (el, attrType);
      /* creating attribute */
      if (attr == NULL)
	{
	  attr = TtaNewAttribute (attrType);
	  TtaAttachAttribute (el, attr, doc);
	}
      TtaSetAttributeText (attr, value, el, doc);
      lang = TtaGetLanguageIdFromName (value);
      pDoc = LoadedDocument[doc - 1];
      found = FALSE;
      for (i = 0; i < pDoc->DocNLanguages && !found; i++)
	if (pDoc->DocLanguages[i] == lang)
	  found = TRUE;
      if (!found)
	{
	  pDoc->DocLanguages[pDoc->DocNLanguages] = lang;
	  pDoc->DocNLanguages ++;
	}
      /* change parser context */
      XmlChangeCurrentLangage(lang);
    }
}

/*----------------------------------------------------------------------
   XmlSetInclusion: storing the inline attribute. Not used yet.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void XmlSetInclusion (Document doc,Element el, STRING value)
#else /* __STDC__ */
static void XmlSetInclusion ( doc, el, value)
Document doc;
Element el;
unsigned char *value;
#endif /* __STDC__ */
{
  XmlReferenceType *tempRef;
  
  if (el != NULL)
    {
      tempRef = ReferenceList;
      while (tempRef != NULL)
	{
	  if (tempRef->El==el)
	    tempRef->IsInclusion = TRUE;
	}
    }
}

/*----------------------------------------------------------------------
  Table of action for parsing of Xml Namespace Attributes
  ----------------------------------------------------------------------*/
static XmlAttrEntry XmlAttr[] = 
{
  {XML_LANG_ATTR,   (Proc) XmlSetLanguage},
  {XML_ID_ATTR,     (Proc) XmlSetLabel},
  {XML_HREF_ATTR,   (Proc) XmlSetTarget},
  {XML_LINK_ATTR,   (Proc) XmlSetLinkType},
  {XML_INLINE_ATTR, (Proc) XmlSetInclusion},
  {_EMPTYSTR_, (Proc) NULL}
};

/*----------------------------------------------------------------------
   ParseXmlAttribute: Handle XML/XLL attributes
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool ParseXmlAttribute (Document doc,Element el, STRING attrName, STRING value)
#else /* __STDC__ */
ThotBool ParseXmlAttribute (doc, el, attrName, value)
Document doc;
Element el; 
STRING attrName;
STRING value;
#endif /* __STDC__ */
{
  int i = 0;
  
  while (XmlAttr[i].AttrAction != (Proc)NULL && 
	 ustrcmp (attrName, XmlAttr[i].AttrName)) 
    i++;
  if (XmlAttr[i].AttrAction != (Proc)NULL)
    {
      (*(XmlAttr[i].AttrAction)) (doc,el,value);
    }
  else
    return (FALSE);

  return (TRUE);
}
