/*

 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
/*
   module: thotmodule.c
   Authors: Monte Regis
            Bonhomme Stephane

   Comments: this module manage Thot special attributes
             while parsing XML documents
	     this module uses constant defined in constxml.h

   Extern function: 
     - boolean ParseThotAttribute (PrefixType *Prefixs,
                                   Document doc,
				   Element el, 
				   char *attrName, 
				   char *value)
            Parse the Thot attribute (Prefixs is the parser prefix list
	      for opening document's nature in XmlAddPS)
     - void  XmlSetPresentation(Document doc)
            Update the presentation rules when views are opened
 */

#define MAX_ATTR 100;
#undef THOT_EXPORT
#define THOT_EXPORT extern
#define EOS     '\0'
#define SPACE    ' '
#define MAX_LENGTH     512
#include "tree.h"
#include "content.h"
#include "view.h"
#include "language.h"
#include "attribute.h"
#include "document.h"
#include "application.h"
#include "app.h"

#include "constxml.h"
#include "typexml.h"
#include "parsexml_f.h"

/* specific presentation storing structure */
typedef struct _XmlPresentationType{
  Element  El;           /* associated element */
  char    *ViewName;     /* PRule view */
  int      PRuleNum;     
  int      PRuleValue;
  struct  _XmlPresentationType *Next;
}XmlPresentationType;

static int      NbEntries = 8;        /* number of thot attribute parsed */
static char     PPrefixName[9] = "";  /* temporary structure to store prefix */
static char     PSchemaName[30] = ""; /* temporary structure to store PSchema */
/* temporary pointer to parser prefixlist */
static PrefixType     *ParserPrefixs;
/* specific presentation list */
static XmlPresentationType *XmlPresentation=NULL;

/*----------------------------------------------------------------------
   XmlAddPS: add a nature presentation in the document
             Except main document, no nature are previously added so
	     this function add them to document
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void XmlAddPS (Document doc)
#else
static void XmlAddPS (doc)
Document doc;
#endif
{
  PrefixType         *tempPrefix;
  char               *schemaName;

  schemaName = TtaGetSSchemaName(TtaGetDocumentSSchema(doc));

  if ( PSchemaName[0] != EOS &&  PPrefixName[0] != EOS)
    if(!strcmp( PPrefixName,DEFAULT_VALUE))
      /* We've read the default and main schema presentation*/
      /* Creating Document and Initializing document */
      TtaSetPSchema(doc, PSchemaName);
    else 
      {
	tempPrefix = ParserPrefixs;
	while (tempPrefix != NULL && strcmp(tempPrefix->Name,PPrefixName))
	  tempPrefix = tempPrefix->Next;
	if (tempPrefix != NULL)
	  if (tempPrefix->Schema==NULL)
	    /* if nature had not been added ... */
	    {
	      if ((strlen(tempPrefix->SchemaName))>4 && 
		  !strcmp(&(tempPrefix->SchemaName[strlen(tempPrefix->SchemaName)-4]),"_EXT"))
		/* We've read an extension */
		{
		  tempPrefix->SchemaName[strlen(tempPrefix->SchemaName)-4]=EOS;
		  tempPrefix->Schema = TtaNewSchemaExtension(doc,tempPrefix->SchemaName,PSchemaName);
		}
	      else 
		/* We've read an nature  */
		tempPrefix->Schema = TtaNewNature(TtaGetDocumentSSchema(doc),tempPrefix->SchemaName,PSchemaName);
	    
	    }
	  else
	    {
	      /* Warning: No API, waiting for a generic API that change */ 
	      /*   a Nature or Ext presentation
	           it would much more coherent and stable to add nature in 
	           xmlmodule.c AddNS */ 
	      if (TtaGetSchemaExtension(doc,tempPrefix->SchemaName)!=NULL) 
		/* We've read an extension */ 
	      {
		/*TtaChangeExtPSchema(tempPrefix->Schema,PSchemaName);*/
	      } 
	      else if (TtaGetSSchema(tempPrefix->SchemaName,doc) != NULL) 
		/* Schema is known so we've read a nature */ 
		{
		  /*TtaChangeExtPSchema(tempPrefix->Schema,PSchemaName)*/
		}
	      else 
	      XmlError(doc,"Bad xml entry");
	    }
	else
	  XmlError(doc,"Bad xml entry");
      }
  PSchemaName[0] = EOS;
  PPrefixName[0] = EOS;
}

/*----------------------------------------------------------------------
   XmlSetPPrefix
  ----------------------------------------------------------------------*/
static void XmlSetPPrefix (Document doc,Element el, char *value)
{
  if (PPrefixName[0]!=EOS)
    XmlError (doc, "Already a prefix read. \n");
  else
    {
      strcpy (PPrefixName,value);
      if (PSchemaName[0]!=EOS)
	/* we have both PPrefix and PSchema */
	XmlAddPS (doc);
    }
}

/*----------------------------------------------------------------------
   XmlSetPSchema
  ----------------------------------------------------------------------*/
static void XmlSetPSchema (Document doc,Element el, char *value)
{
  if (PSchemaName[0]!=EOS)
    XmlError (doc, "Already a schema read. \n");
  else
    {
      strcpy (PSchemaName,value);
      if (PPrefixName[0]!=EOS)
	/* we have both PPrefix and PSchema */
	XmlAddPS (doc);
    }
}

/*----------------------------------------------------------------------
  XmlSetHolophraste 
  ----------------------------------------------------------------------*/
static void  XmlSetHolophraste (Document doc,Element el, char *value)
{
  if (!strcmp(value,TRUE_VALUE))
      TtaHolophrastElement(el,TRUE,doc);
}

/*----------------------------------------------------------------------
   XmlHoldPresentation: store presentation rules from a thot:style attribute
      Value is typed like: "ViewName:PRuleNum:PRuleValue;..."
  ----------------------------------------------------------------------*/
static void  XmlHoldPresentation(Document doc,Element el, char *value)
{

  XmlPresentationType  *newPres;
  int     begin=0;
  int     end=0;
  int     length=0;

  length = strlen (value)-1;
  while(begin<length)
    {
      /* Creating new structure */
      newPres = (XmlPresentationType *)TtaGetMemory(sizeof(XmlPresentationType));
      newPres->Next = XmlPresentation;
      XmlPresentation = newPres;

      XmlPresentation->El = el;
      /* reading view name */
      while(end<length&&value[end]!=':') end++;
      value[end]=EOS;
      newPres->ViewName = TtaStrdup(&value[begin]);
      begin = end + 1;
      /* reading PRule number */
      while(end<length&&value[end]!=':') end++;
      value[end]=EOS;
      newPres->PRuleNum = atoi(&value[begin]);
      begin = end + 1;
      /* reading PRule value */
      while(end<length&&value[end]!=';') end++;
      value[end]=EOS;
      newPres->PRuleValue = atoi(&value[begin]);
      begin = end + 1;
    }
}
/*----------------------------------------------------------------------
   XmlSetPresentation: Update doc presentation after views are opened
         Warning: box values don't work yet
                  TtaGetPruleValue gives percent and absolute position
		  from upper box, as TtaChangeBoxe are offset.
  ----------------------------------------------------------------------*/
void  XmlSetPresentation(Document doc)
{
  PRule   newPRule;
  XmlPresentationType *inter;
  
  while(XmlPresentation!=NULL)
    {
      switch(XmlPresentation->PRuleNum)
	  /* Manage the different PRule types */
	{
	case 4: /* case heigth */
	  TtaChangeBoxSize(XmlPresentation->El, doc,
	                   TtaGetViewFromName(doc,XmlPresentation->ViewName),
			   0,XmlPresentation->PRuleValue,UnPoint);
	  break;
        case 5: /* case width */
	  TtaChangeBoxSize(XmlPresentation->El, doc,
	                   TtaGetViewFromName(doc,XmlPresentation->ViewName),
			   XmlPresentation->PRuleValue,0,UnPoint); 
	  break;
        case 6: /* case y offset */
	  TtaChangeBoxPosition(XmlPresentation->El, doc,
	                   TtaGetViewFromName(doc,XmlPresentation->ViewName),
			   0,XmlPresentation->PRuleValue,UnPoint);
	  break;
        case 7: /* case x offset */
	  TtaChangeBoxPosition(XmlPresentation->El, doc,
	                   TtaGetViewFromName(doc,XmlPresentation->ViewName),
			   XmlPresentation->PRuleValue,0,UnPoint);
	  break;
	default: /* Other PRules */
	  newPRule = TtaGetPRule(XmlPresentation->El,XmlPresentation->PRuleNum);
	  if (newPRule==NULL)
	    /* PRule doesn't exist yet */
	    {
	      newPRule = TtaNewPRule (XmlPresentation->PRuleNum,TtaGetViewFromName(doc,XmlPresentation->ViewName),doc);
	      TtaAttachPRule(XmlPresentation->El,newPRule,doc);
	    }
	  TtaSetPRuleValue(XmlPresentation->El,newPRule,XmlPresentation->PRuleValue,doc);
	}
      /* frees current structure */
      inter = XmlPresentation->Next;
      TtaFreeMemory (XmlPresentation->ViewName);
      TtaFreeMemory (XmlPresentation);
      XmlPresentation = inter;
    }
}

/*----------------------------------------------------------------------
   XmlSetGraphicsShape
  ----------------------------------------------------------------------*/
static void  XmlSetGraphicsShape(Document doc,Element el, char *value)
{
  TtaSetGraphicsShape(el, value[0], doc);
}

/*----------------------------------------------------------------------
   XmlSetPairedPosition
  ----------------------------------------------------------------------*/
static void  XmlSetPairedPosition(Document doc,Element el, char *value)
{}

/*----------------------------------------------------------------------
   XmlSetLinePoints: Add points to polyline from thot:points attr
     value is formed: "x,y;x,y;"
  ----------------------------------------------------------------------*/
static void  XmlSetLinePoints(Document doc,Element el, char *value)
{
  int     begin=0;
  int     end=0;
  int     length=0;
  int     x;
  int     y;
  int     rank=1;

  length = strlen (value)-1;

  while(begin<length)
    {
      while(end<length&&value[end]!=',') end++;
      value[end]=EOS;
      x = atoi(&value[begin]);
      begin = end + 1;

      while(end<length&&value[end]!=';') end++;
      value[end]=EOS;
      y = atoi(&value[begin]);
      begin = end + 1;

      TtaAddPointInPolyline(el,rank,UnPoint,x,y,doc);
      rank++;
    }
}

/*----------------------------------------------------------------------
   XmlSetSrc: set image source
  ----------------------------------------------------------------------*/
static void  XmlSetSrc(Document doc,Element el, char *value)
{
  TtaSetTextContent(el,value,SPACE,doc);
}

static XmlAttrEntry ThotAttr[] = 
{
  {P_PREFIX_ATTR, (Proc) XmlSetPPrefix},
  {P_SCHEMA_ATTR, (Proc) XmlSetPSchema},
  {HOLOPHRASTE_ATTR, (Proc) XmlSetHolophraste},
  {STYLE_ATTR, (Proc) XmlHoldPresentation},
  {GRAPH_CODE_ATTR, (Proc) XmlSetGraphicsShape},
  {PAIRED_ATTR, (Proc) XmlSetPairedPosition},
  {LINE_POINTS_ATTR, (Proc) XmlSetLinePoints},
  {SRC_ATTR, (Proc) XmlSetSrc}
};

/*----------------------------------------------------------------------
  ParseThotAttribute:main function called 
  ----------------------------------------------------------------------*/
boolean ParseThotAttribute (PrefixType *Prefixs,Document doc,Element el, char *attrName, char *value)
{
  int i=0;
  /* searching thot action */
  while (i<NbEntries && strcmp(attrName,ThotAttr[i].AttrName)) i++;

  if (i<NbEntries)
    {
      ParserPrefixs = Prefixs;
      (*(ThotAttr[i].AttrAction))(doc,el,value);
      return (TRUE);
    }
  else 
    {
      XmlError(doc,"Thot Attribute unknown");
      return (FALSE);
    }
}



