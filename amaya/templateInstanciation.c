#include "templateInstanciation.h"

#include "HTMLactions_f.h"
#include "init_f.h"
#include "HTMLsave_f.h"

#include "templateDeclarations.h"
#include "templateUtils.h"

#include "Template.h"

#ifdef TEMPLATES

#define TEMPLATE_SCHEMA_NAME "Template"
 
typedef struct _InstanciationCtxt
{
	char *			templatePath;
	char *			instancePath;
	char *			schemaName;
	DocumentType	docType;
	ThotBool		dontReplace;
} InstanciationCtxt;

#endif /* TEMPLATES */

typedef struct _AttSearch
{
  int   att;
  int   type;
} AttSearch;

static AttSearch    URL_attr_tab[] = {
  {HTML_ATTR_HREF_, XHTML_TYPE},
  {HTML_ATTR_codebase, XHTML_TYPE},
  {HTML_ATTR_Script_URL, XHTML_TYPE},
  {HTML_ATTR_SRC, XHTML_TYPE},
  {HTML_ATTR_data, XHTML_TYPE},
  {HTML_ATTR_background_, XHTML_TYPE},
  {HTML_ATTR_Style_, XHTML_TYPE},
  {HTML_ATTR_cite, XHTML_TYPE},
  //{XLink_ATTR_href_, XLINK_TYPE},
  {MathML_ATTR_style_, MATH_TYPE},
#ifdef _SVG
  {SVG_ATTR_style_, SVG_TYPE},
  {SVG_ATTR_xlink_href, SVG_TYPE}
#endif
};

void RegisterURLs(Document doc, Element el)
{
#ifdef TEMPLATES

  SSchema             XHTMLSSchema, MathSSchema, SVGSSchema, XLinkSSchema;
  AttributeType       attrType;
  Attribute           attr;
  int                 max;

  XHTMLSSchema = TtaGetSSchema ("HTML", doc);
  MathSSchema = TtaGetSSchema ("MathML", doc);
  SVGSSchema = TtaGetSSchema ("SVG", doc);
  XLinkSSchema = TtaGetSSchema ("XLink", doc);

  max = sizeof (URL_attr_tab) / sizeof (AttSearch);

  for(int i=0; i<max; i++)
    {
      attrType.AttrTypeNum = URL_attr_tab[i].att;
      switch (URL_attr_tab[i].type)
        {
        case XHTML_TYPE:
          attrType.AttrSSchema = XHTMLSSchema;
          break;
        case MATH_TYPE:
          attrType.AttrSSchema = MathSSchema;
          break;
        case SVG_TYPE:
          attrType.AttrSSchema = SVGSSchema;
          break;
        case XLINK_TYPE:
          attrType.AttrSSchema = XLinkSSchema;
          break;
        default:
          attrType.AttrSSchema = NULL;
        }

      attr = TtaGetAttribute(el, attrType);
      if(attr!=NULL)      
        TtaRegisterAttributeReplace(attr, el, doc);

    }  

  for(Element child = TtaGetFirstChild(el);child;TtaNextSibling(&child))
    RegisterURLs(doc, child);

#endif /* TEMPLATES*/
}

void  CreateInstance(char *templatePath, char *instancePath) {
#ifdef TEMPLATES
  ThotBool alreadyViewing = FALSE;
  int alreadyOnDoc = 0;

  XTigerTemplate t = (XTigerTemplate)Get(templates, templatePath);

  if(t==NULL)
    //The template must be loaded before calling this function
    return;

  Document doc = GetTemplateDocument(t);
  DocumentType docType = DocumentTypes[doc];

  while(alreadyOnDoc<DocumentTableLength-1 && !alreadyViewing)
    {
      alreadyOnDoc++;
      if(DocumentURLs[alreadyOnDoc]!=NULL)
        alreadyViewing = !strcmp(DocumentURLs[alreadyOnDoc],instancePath);
    }

  if(!TtaPrepareUndo(doc))
    {
      TtaOpenUndoSequence(doc, NULL, NULL, 0, 0);

      RegisterURLs(doc, TtaGetRootElement(doc));
      
      SetRelativeURLs (doc, instancePath);
      
      switch (docType)
        {
        case docSVG :
          TtaExportDocumentWithNewLineNumbers (doc, instancePath, "SVGT");
          break;
        case docMath :
          TtaExportDocumentWithNewLineNumbers (doc, instancePath, "MathMLT");
          break;
        case docHTML :
          if(TtaGetDocumentProfile(doc)==L_Xhtml11 || TtaGetDocumentProfile(doc)==L_Basic)
            TtaExportDocumentWithNewLineNumbers (doc, instancePath, "HTMLT11");
          else
            TtaExportDocumentWithNewLineNumbers (doc, instancePath, "HTMLTX");
          break;
        default :
          TtaExportDocumentWithNewLineNumbers (doc, instancePath, NULL);
          break;
        }
      
      TtaCloseUndoSequence(doc);
      TtaUndoNoRedo(doc);
      TtaClearUndoHistory(doc);
    }

  if(!alreadyViewing) //Open the instance
    {      
      TtaExtractName (instancePath, DirectoryName, DocumentName);
      CallbackDialogue (BaseDialog + OpenForm, INTEGER_DATA, (char *) 1);
    }
  else //Reload on the existing view
    Reload(alreadyOnDoc, 0);  
  
#endif /* TEMPLATES */
}

void InstantiateTemplate_callback (int newdoc, int status,  char *urlName,
								   char *outputfile, AHTHeaders *http_headers,
								   void * context)
{
#ifdef TEMPLATES
	InstanciationCtxt *ctx = (InstanciationCtxt*)context;

	InstanciateTemplate(ctx->templatePath);

  CreateInstance(ctx->templatePath, ctx->instancePath);

  TtaFreeMemory(ctx->templatePath);
  TtaFreeMemory(ctx->instancePath);
  TtaFreeMemory(ctx);
#endif /* TEMPLATES */
}


/*----------------------------------------------------------------------
----------------------------------------------------------------------*/
void InstanciateTemplate (Document doc, char *templatename, char *docname,
                          DocumentType docType, ThotBool loaded)
{
#ifdef TEMPLATES
	if(!loaded)
	{
		//Creation of the callback context
		InstanciationCtxt *ctx	= (InstanciationCtxt *)TtaGetMemory (sizeof (InstanciationCtxt));
		ctx->templatePath		= TtaStrdup (templatename);
		ctx->instancePath		= TtaStrdup (docname);
		ctx->schemaName			= GetSchemaFromDocType(docType);
		ctx->docType			= docType;
		
		GetAmayaDoc (templatename, NULL, doc, doc, CE_MAKEBOOK, FALSE, 
			(void (*)(int, int, char*, char*, const AHTHeaders*, void*)) InstantiateTemplate_callback,
			(void *) ctx);
	}
	else
    {
      InstanciateTemplate(templatename);
      CreateInstance(templatename, docname);
    }
  
#endif /* TEMPLATES */
}

void InstanciateRepeat(XTigerTemplate t, Element el, Document doc)
{
#ifdef TEMPLATES
  int            curVal,  minVal,  maxVal;
  Attribute      curAtt,  minAtt,  maxAtt;
  AttributeType  curType, minType, maxType;
  char           *text;

  //Preparing types
  minType.AttrSSchema = maxType.AttrSSchema = curType.AttrSSchema 
    = TtaGetSSchema(TEMPLATE_SCHEMA_NAME, doc);

  curType.AttrTypeNum = Template_ATTR_currentOccurs; 
  minType.AttrTypeNum = Template_ATTR_minOccurs;
  maxType.AttrTypeNum = Template_ATTR_maxOccurs;

  //Get currentOccurs, minOccurs and maxOccurs attributes
  curAtt = TtaGetAttribute(el, curType);
  minAtt = TtaGetAttribute(el, minType);
  maxAtt = TtaGetAttribute(el, maxType);

  //Get the values
  if(minAtt)
    {
      text = GetAttributeStringValue(el, minAtt);
      if(text)
        {
          minVal = atoi(text);
          TtaFreeMemory(text);
        }
      else
        //Error : Attribute with no value
        return;
    }
  else
    minVal = 0;

  if(maxAtt)
    {
      text = GetAttributeStringValue(el, maxAtt);
      if(text)
        {
          if(strcmp(text,"*")==0)
            maxVal = INT_MAX;
          else
            maxVal = atoi(text);
          TtaFreeMemory(text);
        }
      else
        //Error : Attribute with no value
        return;
    }
  else
    maxVal = INT_MAX;

  if(curAtt)
    {
      text = GetAttributeStringValue(el, curAtt);
      if(text)
        {
          maxVal = atoi(text);
          TtaFreeMemory(text);
        }
      else
        //Error : Attribute with no value
        return;
    }
  else
    curVal = minVal;

  text = (char*)TtaGetMemory(MAX_LENGTH);

  //Create non existing attributes
  if(!minAtt)
    {      
      minAtt = TtaNewAttribute(minType);
      sprintf(text,"%d",minVal);
      TtaAttachAttribute(el, minAtt, doc);
      TtaSetAttributeText(minAtt, text, el, doc);
    }

  if(!maxAtt)
    {  
      maxAtt = TtaNewAttribute(maxType);
      if(maxVal<INT_MAX)
        sprintf(text,"%d",maxVal);
      else
        sprintf(text,"*");
      TtaAttachAttribute(el, maxAtt, doc);      
      TtaSetAttributeText(maxAtt, text, el, doc);
    }

  if(!curAtt)
    {      
      curAtt = TtaNewAttribute(curType);
      sprintf(text,"%d",curVal);
      TtaAttachAttribute(el, curAtt, doc);
      TtaSetAttributeText(curAtt, text, el, doc);
    }

  if(text)
    TtaFreeMemory(text);

  //We must have currentOccurs children
  Element  child, newChild;
  int      childrenCount;

  child = TtaGetFirstChild(el);
  if(!child)
    //Error : a repeat must have at least one child which will be the model
    return;
  
  for(childrenCount = 0; child; TtaNextSibling(&child))
    {
      //TODO : Check that every children is valid
      childrenCount ++;
    }

  if(childrenCount > maxVal)
    //Error : too many children!
    return;  

  child = TtaGetLastChild(el);

  while(childrenCount < curVal)
    {
      //Create a new child
      newChild = TtaCopyTree(child, doc, doc, el);
      TtaInsertSibling(newChild, child, FALSE, doc);
      child = newChild;
      childrenCount++;
    }
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
----------------------------------------------------------------------*/
void ParseTemplate (XTigerTemplate t, Element el, Document doc)
{
#ifdef TEMPLATES
	AttributeType attType;
	Attribute att;
	char *name;
	ElementType type = TtaGetElementType(el);
	
	if(strcmp(TtaGetSSchemaName(type.ElSSchema),"Template")==0)
    {
      switch(type.ElTypeNum)
        {
        case Template_EL_head :
          //Remove it and all of its children
          TtaDeleteTree(el, doc);
          //We must stop searching into this tree
          return;
          break;
        case Template_EL_component :
          //Replace by a use				
          attType.AttrSSchema = TtaGetElementType(el).ElSSchema;
          attType.AttrTypeNum = Template_ATTR_name;
          
          name = GetAttributeStringValue(el, Template_ATTR_name);		  		  
          TtaRemoveAttribute(el, TtaGetAttribute(el, attType),doc);
			
          TtaChangeElementType(el, Template_EL_useEl);
          
          attType.AttrTypeNum = Template_ATTR_types;
          att = TtaNewAttribute(attType);
          TtaAttachAttribute(el, att, doc);
          TtaSetAttributeText(att, name, el, doc);
          
          attType.AttrTypeNum = Template_ATTR_currentType;
          att = TtaNewAttribute(attType);
          TtaAttachAttribute(el, att, doc);		  
          TtaSetAttributeText(att, name, el, doc);
          
          break;
        case Template_EL_bag :
          //Link to types
          //Allow editing the content
          break;
        case Template_EL_useEl :
          //Link to types
          //Create the default content if needed
          //If simple type allow editing the content
          break;
        case Template_EL_attribute :
          //Initialize the attribute
          //Allow the edition of the attribute
          break;
        case Template_EL_repeat :
          InstanciateRepeat(t, el, doc);
          break;
        default :
          break;
        }
    }
	
	Element child = TtaGetFirstChild(el);
	Element aux; //Needed when deleting trees
	while(child!=NULL) {
		aux = child;
		TtaNextSibling(&aux);
		ParseTemplate(t, child, doc);
		child = aux;
	}
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
----------------------------------------------------------------------*/
void InstanciateTemplate(char *templatename)
{
#ifdef TEMPLATES
	XTigerTemplate	t;
	ElementType		  elType;
	Element			    root, piElem, doctype, elFound, text;
	char		        *s, *charsetname = NULL, buffer[MAX_LENGTH];
  int             pi_type;
  Document        doc;

	//Instanciate all elements
	t		=	(XTigerTemplate) Get(templates,templatename);
  doc = GetTemplateDocument(t);
	root	=	TtaGetMainRoot (doc);
	ParseTemplate(t, root, doc);

  //Look for PIs
  /* check if the document has a DOCTYPE declaration */
#ifdef ANNOTATIONS
  if (DocumentTypes[doc]  == docAnnot)
    elType = TtaGetElementType (root);
  else
#endif /* ANNOTATIONS */
    elType = TtaGetElementType (root);
  s = TtaGetSSchemaName (elType.ElSSchema);
  if (strcmp (s, "HTML") == 0)
    {
      elType.ElTypeNum = HTML_EL_DOCTYPE;
      pi_type = HTML_EL_XMLPI;
    }
#ifdef _SVG
  else if (strcmp (s, "SVG") == 0)
    {
      elType.ElTypeNum = SVG_EL_DOCTYPE;
      pi_type = SVG_EL_XMLPI;
    }
#endif /* _SVG */
  else if (strcmp (s, "MathML") == 0)
    {
      elType.ElTypeNum = MathML_EL_DOCTYPE;
      pi_type = MathML_EL_XMLPI;
    }
  else
    {
      elType.ElTypeNum = XML_EL_doctype;
      pi_type = XML_EL_xmlpi;
    }
  doctype = TtaSearchTypedElement (elType, SearchInTree, root);

  if(!doctype)
    {
      /* generate the XML declaration */
      /* Check the Thot abstract tree against the structure schema. */
      TtaSetStructureChecking (FALSE, doc);
      elType.ElTypeNum = pi_type;
      doctype = TtaNewTree (doc, elType, "");
      TtaInsertFirstChild (&doctype, root, doc);
      elFound = TtaGetFirstChild (doctype);
      text = TtaGetFirstChild (elFound);
      strcpy (buffer, "xml version=\"1.0\" encoding=\"");
      charsetname = UpdateDocumentCharset (doc);
      strcat (buffer, charsetname);
      strcat (buffer, "\"");
      TtaSetTextContent (text, (unsigned char*)buffer,  Latin_Script, doc);
      TtaSetStructureChecking (TRUE, doc);
    }
  
  /* generate the XTiger PI */
  /* Check the Thot abstract tree against the structure schema. */
  TtaSetStructureChecking (FALSE, doc);
  elType.ElTypeNum = pi_type;
  piElem = TtaNewTree (doc, elType, "");
  TtaInsertSibling(piElem, doctype, FALSE, doc);
  elFound = TtaGetFirstChild (piElem);
  text = TtaGetFirstChild (elFound);
  strcpy (buffer, "xtiger template=\"");
  strcat (buffer, templatename);
  strcat (buffer, "\"");
  TtaSetTextContent (text, (unsigned char*)buffer,  Latin_Script, doc);
  TtaSetStructureChecking (TRUE, doc);

          
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
PreInstanciateComponents: Instanciates all components in order to improve
edition.
----------------------------------------------------------------------*/
void PreInstanciateComponents(XTigerTemplate t)
{
#ifdef TEMPLATES 
  DicDictionary components = GetComponents(t);
  Declaration comp;

  for(First(components);!IsDone(components);Next(components))
    {
      comp = (Declaration) CurrentElement(components);
      ParseTemplate(t, GetComponentContent(comp), GetTemplateDocument(t));
    }
#endif /* TEMPLATES */
}
