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
     - boolean ParseXMLAttribute (PrefixType **pPrefixs,
                                   Document doc,
				   Element el, 
				   char *attrName, 
				   char *value)
            Parse the Thot attribute (pPrefixs is the parser prefix list
	      for opening document's nature in XmlAddNS. updated.)
     - void  XmlSetRef(Document doc)
            Update the references when parsing has finished
	    (All elements shall be created to update internals references 
 */
#define MAX_ATTR 100;

#undef THOT_EXPORT
#define THOT_EXPORT extern
#define EOS     '\0'
#define SPACE    ' '
#define MAX_LENGTH     512
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
#include "app.h"
#include "reference.h"

#include "constxml.h"
#include "typexml.h"
#include "parsexml_f.h"

static int NbEntries = 5;          /* number of thot attribute parsed */
/* temporary pointer to parser prefixlist */
static PrefixType *ParserPrefixs;
/* reference storing structure */
typedef struct _XmlReferenceType
{
  Element       El;
  Attribute     Attr;
  Document      TargetDoc;
  char         *TargetLabel;
  boolean       IsInclusion;
  struct _XmlReferenceType *Next;
}XmlReferenceType;
/* references list */
static XmlReferenceType *ReferenceList = NULL;

/*----------------------------------------------------------------------
   XmlAddNS: add a nature in the document
      The calling parameters aren't the same because it isn't and can't
      be in the action table.
      Actually, nature aren't added here because we have to know the
      presentation schema to add them. Waiting API
  ----------------------------------------------------------------------*/
static void XmlAddNS (Document doc, unsigned char *prefixName,unsigned char *schemaName)
{
  PrefixType         *newPrefix;

  if (prefixName[0]==EOS)
    /* We've read the default and main schema */
    {
    if (schemaName!=NULL)
      {
	/* Creating Document and Initializing document */
	XmlSetCurrentDocument(schemaName);
      }
    }
  else if(doc!=0)
    {
      newPrefix = (PrefixType *) TtaGetMemory (sizeof (PrefixType));
      newPrefix->Next = ParserPrefixs;
      ParserPrefixs = newPrefix;
      newPrefix->SchemaName = TtaStrdup (schemaName);
      newPrefix->Name = TtaStrdup (prefixName);
      if ((schemaName[0]!=EOS && prefixName[0]!=EOS && 
	   doc != (Document)NULL && strlen(schemaName))>4 && 
	  !strcmp(&schemaName[strlen(schemaName)-4],"_EXT"))
	/* We've read an extension */
	{
	  /* Warning: Nature ain't created yet: */
	  /*   Waiting for presentation schema name */
	  newPrefix->Schema = NULL;
	  schemaName[strlen(schemaName)-4]=EOS;
	}
      else if (doc != (Document)NULL && schemaName!=NULL && prefixName != NULL)
	/* We've read an nature  */
	newPrefix->Schema = NULL;
      else
	XmlError(doc,"Bad xml attribute entry");
    }
    else
      XmlError(0,"No default schema given");
}

/*----------------------------------------------------------------------
   XmlAddRef: add a reference in list for later updating
              attr=NULL if reference element
  ----------------------------------------------------------------------*/
void XmlAddRef (Document doc, Element el, Attribute attr)
{
  XmlReferenceType  *newRef;

  newRef = (XmlReferenceType *) TtaGetMemory (sizeof (XmlReferenceType));
  newRef->El = el;
  newRef->Attr = attr;
  newRef->TargetDoc = 0;
  newRef->TargetLabel = NULL;
  newRef->IsInclusion = FALSE;
  newRef->Next = ReferenceList;
  ReferenceList = newRef;
}

/*----------------------------------------------------------------------
   XmlSetTarget: update the reference's target label
                 after xml:href attribute (for element) or ref attr.
  ----------------------------------------------------------------------*/
void  XmlSetTarget (Document doc,Element el, unsigned char *value)
{
  char docName[100];
  char label[20];
  int   i = 0;
  XmlReferenceType *tempRef;

  tempRef = ReferenceList;
  while (tempRef != NULL)
    {
      if (tempRef->El==el)
	/* Reference is found */
	{
	  while (value[i]!='#')
	    {
	      docName[i] = value[i];
	      i++;
	    }
	  if (i!=0)
	    /* Warning: Reference from another document */
	    {/* To Complete */}
	  i++;
	  strcpy(label,&value[i]);
	  tempRef->TargetLabel = TtaStrdup(label);
	  break;
	}
      tempRef = tempRef->Next;
    }

}

/*----------------------------------------------------------------------
   XmlSetRef: Update references after the end of parsing
  ----------------------------------------------------------------------*/
void XmlSetRef (Document doc)
{
  XmlReferenceType *tempRef;
  XmlReferenceType *inter;
  tempRef = ReferenceList;
  while (tempRef != NULL)
    {
      if (tempRef->TargetLabel != NULL && tempRef->El != NULL)
	{
	  /* in same document */
	  if (tempRef->TargetDoc==0)
	    tempRef->TargetDoc=doc;
	  /* To complete */
	  else
	    {
	    }
	  /* reference attribute */
	  if (tempRef->Attr!=NULL)
	    {
	      TtaSetAttributeReference(tempRef->Attr,tempRef->El,doc,
				       TtaSearchElementByLabel(tempRef->TargetLabel,
				         TtaGetMainRoot(tempRef->TargetDoc)),
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
static void  XmlSetLinkType (Document doc,Element el, unsigned char *value)
{  
}

/*----------------------------------------------------------------------
   XmlSetLabel: set an element label
  ----------------------------------------------------------------------*/
static void  XmlSetLabel (Document doc,Element el, unsigned char *value)
{
  /* Warning: Needed API for setting label*/
  strcpy(((PtrElement)el)->ElLabel, value);
}

/*----------------------------------------------------------------------
   XmlSetLanguage
  ----------------------------------------------------------------------*/
static void XmlSetLanguage (Document doc,Element el, unsigned char *value)
{
  Attribute	attr;
  AttributeType	attrType;

  attrType.AttrTypeNum = 1; /* langage attr */
  attrType.AttrSSchema = TtaGetElementType(el).ElSSchema;

  attr = TtaGetAttribute (el, attrType);
  /* creating attribute */
  if (attr == NULL)
    {
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (el, attr, doc);
    }
  TtaSetAttributeText(attr,value,el,doc);
  /* change parser context */
  XmlChangeCurrentLangage(TtaGetLanguageIdFromName(value));
}

/*----------------------------------------------------------------------
   XmlSetInclusion: storing the inline attribute. Not used yet.
  ----------------------------------------------------------------------*/
static void XmlSetInclusion (Document doc,Element el, unsigned char *value)
{
  XmlReferenceType *tempRef;
  tempRef = ReferenceList;
  while (tempRef != NULL)
    {
      if (tempRef->El==el)
	tempRef->IsInclusion = TRUE;
    }
}

static XmlAttrEntry XmlAttr[] = 
{
  {XML_LANG_ATTR, (Proc) XmlSetLanguage},
  {XML_ID_ATTR, (Proc) XmlSetLabel},
  {XML_HREF_ATTR, (Proc) XmlSetTarget},
  {XML_LINK_ATTR, (Proc) XmlSetLinkType},
  {XML_INLINE_ATTR, (Proc) XmlSetInclusion}
};

/*----------------------------------------------------------------------
   ParseXmlAttribute: Handle XML/XLL attributes
  ----------------------------------------------------------------------*/
boolean ParseXmlAttribute (PrefixType **pPrefixs, Document doc,Element el, unsigned char *attrName, unsigned char *value)
{
  int i=0;
  
  /* Testing if the attribute is the xmlns:prefix="xxx" attribute */
  if ((strlen(attrName)>=6)&&!strncmp(&attrName[strlen(attrName)-6],":xmlns",6))
    {
      ParserPrefixs = *pPrefixs;
      attrName[strlen(attrName)-6]=EOS;
      XmlAddNS(doc, attrName,value);
      *pPrefixs = ParserPrefixs; /* changing parser prefix list */
      return (TRUE);
    }
  else
    {
      while (i<NbEntries && strcmp(attrName,XmlAttr[i].AttrName)) i++;
      if (i<NbEntries)
	{
	  (*(XmlAttr[i].AttrAction))(doc,el,value);
	  return (TRUE);
	}
      else return (FALSE);
    }
}
