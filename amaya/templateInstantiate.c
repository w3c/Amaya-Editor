#include "templates.h"

#define THOT_EXPORT extern
#include "templateDeclarations.h"

#include "Elemlist.h"

#include "EDITimage_f.h"
#include "HTMLactions_f.h"
#include "HTMLsave_f.h"
#include "init_f.h"
#include "mydictionary_f.h"
#include "templates_f.h"
#include "templateDeclarations_f.h"
#include "templateInstantiate_f.h"
#include "Templatebuilder_f.h"
#include "templateUtils_f.h"
#include "fetchHTMLname_f.h"
#include "Template.h"

#ifdef TEMPLATES
#define TEMPLATE_SCHEMA_NAME "Template"

typedef struct _InstantiateCtxt
{
	char *			templatePath;
	char *			instancePath;
	char *			schemaName;
	DocumentType	docType;
	ThotBool		dontReplace;
} InstantiateCtxt;
#endif /* TEMPLATES */

typedef struct _AttSearch
{
  int   att;
  int   type;
} AttSearch;

static AttSearch    URL_attr_tab[] =
  {
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

/*----------------------------------------------------------------------
  RegisterURLs
  ----------------------------------------------------------------------*/
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
      if (attr!=NULL)      
        TtaRegisterAttributeReplace(attr, el, doc);
    }  

  for (Element child = TtaGetFirstChild (el); child; TtaNextSibling (&child))
    RegisterURLs (doc, child);
#endif /* TEMPLATES*/
}

/*----------------------------------------------------------------------
  CreateInstance
  ----------------------------------------------------------------------*/
void  CreateInstance(char *templatePath, char *instancePath)
{
#ifdef TEMPLATES
  Document doc = 0;
  DocumentType docType;
  ThotBool alreadyViewing = FALSE;
  int      alreadyOnDoc = 0;

  XTigerTemplate t = (XTigerTemplate)Dictionary_Get (Templates_Dic, templatePath);
  if (t == NULL)
    //The template must be loaded before calling this function
    return;

  doc = GetTemplateDocument (t);
  docType = DocumentTypes[doc];
  while (alreadyOnDoc < DocumentTableLength-1 && !alreadyViewing)
    {
      alreadyOnDoc++;
      if (DocumentURLs[alreadyOnDoc])
        alreadyViewing = !strcmp (DocumentURLs[alreadyOnDoc],instancePath);
    }

  if (!TtaPrepareUndo (doc))
    {
      TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
      RegisterURLs (doc, TtaGetRootElement(doc));
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
          if (TtaGetDocumentProfile(doc)==L_Xhtml11 || TtaGetDocumentProfile(doc)==L_Basic)
            TtaExportDocumentWithNewLineNumbers (doc, instancePath, "HTMLT11");
          else
            TtaExportDocumentWithNewLineNumbers (doc, instancePath, "HTMLTX");
          break;
        default :
          TtaExportDocumentWithNewLineNumbers (doc, instancePath, NULL);
          break;
        }
      
      TtaCloseUndoSequence (doc);
      TtaUndoNoRedo (doc);
      TtaClearUndoHistory (doc);
    }

  if (!alreadyViewing)
    {
      // Open the instance
      TtaExtractName (instancePath, DirectoryName, DocumentName);
      CallbackDialogue (BaseDialog + OpenForm, INTEGER_DATA, (char *) 1);
    }
  else
    {
      // Reload on the existing view
      Reload (alreadyOnDoc, 0);
    }
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void InstantiateTemplate_callback (int newdoc, int status,  char *urlName, char *outputfile,
				   char *proxyName, AHTHeaders *http_headers, void * context)
{
#ifdef TEMPLATES
	InstantiateCtxt *ctx = (InstantiateCtxt*)context;

	DoInstanceTemplate (ctx->templatePath);
  CreateInstance (ctx->templatePath, ctx->instancePath);
  TtaFreeMemory (ctx->templatePath);
  TtaFreeMemory (ctx->instancePath);
  TtaFreeMemory (ctx);
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void InstantiateTemplate (Document doc, char *templatename, char *docname,
                          DocumentType docType, ThotBool loaded)
{
#ifdef TEMPLATES
	if (!loaded)
    {
      // Create the callback context
      InstantiateCtxt *ctx = (InstantiateCtxt *)TtaGetMemory (sizeof (InstantiateCtxt));
      ctx->templatePath	= TtaStrdup (templatename);
      ctx->instancePath	= TtaStrdup (docname);
      ctx->schemaName = GetSchemaFromDocType(docType);
      ctx->docType = docType;
		
      GetAmayaDoc (templatename, NULL, doc, doc, CE_MAKEBOOK, FALSE, 
                   (void (*)(int, int, char*, char*, char*, const AHTHeaders*, void*)) InstantiateTemplate_callback,
                   (void *) ctx);
    }
	else
    {
      DoInstanceTemplate (templatename);
      CreateInstance (templatename, docname);
    }  
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  InstantiateAttribute
  ----------------------------------------------------------------------*/
static void InstantiateAttribute (XTigerTemplate t, Element el, Document doc)
{
#ifdef TEMPLATES
  AttributeType  useType, nameType, defaultType, attrType;
  Attribute      useAttr, nameAttr, defAttr, attr;
  ElementType    elType;
  Element        parent;
  char           *text, *elementName;
  ThotBool       level;
  NotifyAttribute event;

  parent = TtaGetParent (el);
  if (!parent)
    return;
  // if attribute "use" has value "optional", don't do anything
  useType.AttrSSchema = TtaGetSSchema (TEMPLATE_SCHEMA_NAME, doc);
  useType.AttrTypeNum = Template_ATTR_useAt;
  useAttr = TtaGetAttribute (el, useType);
  if (useAttr)
    // there is a "use" attribute. Check its value
    {
      text = GetAttributeStringValue (el, useAttr, NULL);
      if (text && strcmp (text, "optional") == 0)
        return;
    }
  // get the "name" and "default" attributes
  nameType.AttrSSchema = defaultType.AttrSSchema = TtaGetSSchema (TEMPLATE_SCHEMA_NAME, doc);
  nameType.AttrTypeNum = Template_ATTR_ref_name;
  defaultType.AttrTypeNum = Template_ATTR_defaultAt;
  nameAttr = TtaGetAttribute (el, nameType);
  defAttr = TtaGetAttribute (el, defaultType);
  if (nameAttr)
    {
      text = GetAttributeStringValue (el, nameAttr, NULL);
      if (text)
        {
          elType = TtaGetElementType (parent);
          elementName = TtaGetElementTypeName (elType);
          level = TRUE;
          MapHTMLAttribute (text, &attrType, elementName, &level, doc);
          TtaFreeMemory(text);
          attr = TtaNewAttribute (attrType);
          if (attr)
            {
              TtaAttachAttribute (parent, attr, doc);
              if (defAttr)
                {
                  text = GetAttributeStringValue (el, defAttr, NULL);
                  TtaSetAttributeText(attr, text, parent, doc);
                  TtaFreeMemory(text);
                  // if it's a src arttribute for an image, load the image
                  if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML") &&
                      elType.ElTypeNum == HTML_EL_IMG)
                    if (attrType.AttrTypeNum == HTML_ATTR_SRC &&
                        attrType.AttrSSchema == elType.ElSSchema)
                      {
                        event.document = doc;
                        event.element = parent;
                        event.attribute = attr;
                        SRCattrModified (&event);
                      }
                }
            }
        }
    }
#endif /* TEMPLATES */
}

#ifdef TEMPLATES
/*----------------------------------------------------------------------
  ProcessAttr
  Look for all "attribute" elements in the subtree and instantiate them
  ----------------------------------------------------------------------*/
static void ProcessAttr (XTigerTemplate t, Element el, Document doc)
{
  Element      child;
  ElementType  elType;

  for (child = TtaGetFirstChild (el); child; TtaNextSibling(&child))
    {
      elType = TtaGetElementType (child);
      if (elType.ElTypeNum == Template_EL_attribute &&
          !strcmp (TtaGetSSchemaName (elType.ElSSchema), TEMPLATE_SCHEMA_NAME))
        InstantiateAttribute (t, child, doc);
      else
        ProcessAttr (t, child, doc);
    }
}
#endif /* TEMPLATES */

/*----------------------------------------------------------------------
  InstantiateUse
  ----------------------------------------------------------------------*/
Element InstantiateUse (XTigerTemplate t, Element el, Document doc,
                        ThotBool insert)
{
#ifdef TEMPLATES
	Element          cont, child, prev, next;
  ElementType      elType;
	Attribute        at;
	AttributeType    att;
  Declaration      dec;
  int              size, nbitems;
  struct menuType  *items;
  char             *types;
  char             *empty = " ";
  ThotBool          oldStructureChecking;

  /* get the value of the "types" attribute */
  cont = NULL;
 elType = TtaGetElementType (el);
 /*
	att.AttrSSchema = elType.ElSSchema;
	att.AttrTypeNum = Template_ATTR_types;
	at = TtaGetAttribute (el, att);
  if (!at)
    return NULL;
	size = TtaGetTextAttributeLength (at);
	types = (char *) TtaGetMemory (size+1);	
	TtaGiveTextAttributeValue (at, types, &size);*/
  
  types = GetAttributeStringValue(el, Template_ATTR_types, &size);
  giveItems (types, size, &items, &nbitems);
  // No structure checking
  oldStructureChecking = TtaGetStructureChecking (doc);
  TtaSetStructureChecking (FALSE, doc);
  
  if (nbitems == 1)
    /* only one type in the "types" attribute */
    {
      dec = GetDeclaration (t, items[0].label);
      if (dec)
        switch(dec->nature)
          {
          case SimpleTypeNat :
            elType.ElTypeNum = Template_EL_TEXT_UNIT;
            cont = TtaNewElement (doc, elType);
            TtaInsertFirstChild (&cont, el, doc);
            TtaSetTextContent (cont, (unsigned char*) empty, 0, doc);
            cont = NULL;
            break;
          case XmlElementNat :
            GIType (dec->name, &elType, doc);
            cont = TtaNewElement (doc, elType);
            if (insert)
              TtaInsertFirstChild (&cont, el, doc);
            break;
          case ComponentNat :
            cont = TtaCopyTree (dec->componentType.content, doc, doc, el);
            ProcessAttr (t, cont, doc);
            if (insert)
              {
                prev = NULL;
                child = TtaGetFirstChild (cont);
                while (child)
                  {
                    next = child;
                    TtaNextSibling (&next);
                    TtaRemoveTree (child, doc);
                    if (prev)
                      TtaInsertSibling (child, prev, FALSE, doc);
                    else
                      TtaInsertFirstChild (&child, el, doc);
                    prev = child;
                    child = next;
                  }
                TtaDeleteTree (cont, doc);
                cont = el;
              }
            break;
          case UnionNat :
            if (!insert)
              /* the user has clicked a "repeat" button and wants to create
                 a new instance of the repeated element. Just create the
                 use element */
              {
                elType.ElTypeNum = Template_EL_useEl;
                cont = TtaNewElement (doc, elType);
                if (cont)
                  {
                    TtaSetAccessRight (cont, ReadWrite, doc);
                    at = TtaNewAttribute (att);
                    if (at)
                      {
                        TtaAttachAttribute (cont, at, doc);
                        TtaSetAttributeText(at, types, cont, doc);
                      }
                  }
              }
            break;
          default :
            //Impossible
            break;   
          }
    }
  TtaFreeMemory(types);
  TtaSetStructureChecking (oldStructureChecking, doc);
  return cont;
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void InstantiateRepeat (XTigerTemplate t, Element el, Document doc)
{
#ifdef TEMPLATES
  int            curVal,  minVal,  maxVal;
  Attribute      curAtt,  minAtt,  maxAtt;
  AttributeType  curType, minType, maxType;
  char           *text;

  //Preparing types
  curType.AttrSSchema = TtaGetSSchema (TEMPLATE_SCHEMA_NAME, doc);
  minType.AttrSSchema = maxType.AttrSSchema = curType.AttrSSchema;
  curType.AttrTypeNum = Template_ATTR_currentOccurs; 
  minType.AttrTypeNum = Template_ATTR_minOccurs;
  maxType.AttrTypeNum = Template_ATTR_maxOccurs;

  //Get currentOccurs, minOccurs and maxOccurs attributes
  curAtt = TtaGetAttribute (el, curType);
  minAtt = TtaGetAttribute (el, minType);
  maxAtt = TtaGetAttribute (el, maxType);

  //Get the values
  if (minAtt)
    {
      text = GetAttributeStringValue(el, minAtt, NULL);
      if (text)
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

  if (maxAtt)
    {
      text = GetAttributeStringValue (el, maxAtt, NULL);
      if (text)
        {
          if (!strcmp (text, "*"))
            maxVal = INT_MAX;
          else
            maxVal = atoi (text);
          TtaFreeMemory (text);
        }
      else
        //Error : Attribute with no value
        return;
    }
  else
    maxVal = INT_MAX;

  if (curAtt)
    {
      text = GetAttributeStringValue(el, curAtt, NULL);
      if (text)
        {
          curVal = atoi(text);
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
  if (!minAtt)
    {      
      minAtt = TtaNewAttribute(minType);
      sprintf(text,"%d",minVal);
      TtaAttachAttribute(el, minAtt, doc);
      TtaSetAttributeText(minAtt, text, el, doc);
    }

  if (!maxAtt)
    {  
      maxAtt = TtaNewAttribute(maxType);
      if (maxVal<INT_MAX)
        sprintf(text,"%d",maxVal);
      else
        sprintf(text,"*");
      TtaAttachAttribute(el, maxAtt, doc);      
      TtaSetAttributeText(maxAtt, text, el, doc);
    }

  if (!curAtt)
    {
      curAtt = TtaNewAttribute(curType);
      sprintf(text,"%d",curVal);
      TtaAttachAttribute(el, curAtt, doc);
      TtaSetAttributeText(curAtt, text, el, doc);
    }

  if (text)
    TtaFreeMemory(text);

  //We must have currentOccurs children
  Element  child, newChild;
  int      childrenCount;

  child = TtaGetFirstChild(el);
  if (!child)
    //Error : a repeat must have at least one child which will be the model
    return;
  
  for(childrenCount = 0; child; TtaNextSibling(&child))
    {
      //TODO : Check that every child is valid
      childrenCount ++;
    }

  if (childrenCount > maxVal)
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
  ParseTemplate
  ----------------------------------------------------------------------*/
static void ParseTemplate (XTigerTemplate t, Element el, Document doc,
                           ThotBool loading)
{
#ifdef TEMPLATES
	AttributeType attType;
	Attribute     att;
	Element       aux, child; //Needed when deleting trees
	char         *name;
	ElementType   elType = TtaGetElementType (el);
	
  name = TtaGetSSchemaName (elType.ElSSchema);
	if (!strcmp (name, "Template"))
    {
      switch(elType.ElTypeNum)
        {
        case Template_EL_head :
          //Remove it and all of its children
          TtaDeleteTree(el, doc);
          //We must stop searching into this tree
          return;
          break;
        case Template_EL_component :
          //Replace by a use				
          attType.AttrSSchema = elType.ElSSchema;
          attType.AttrTypeNum = Template_ATTR_name;
          
          name = GetAttributeStringValue (el, Template_ATTR_name, NULL);		  		  
          TtaRemoveAttribute (el, TtaGetAttribute (el, attType), doc);
          if (NeedAMenu (el, doc))
            TtaChangeElementType (el, Template_EL_useEl);
          else
            TtaChangeElementType (el, Template_EL_useSimple);
          
          attType.AttrTypeNum = Template_ATTR_types;
          att = TtaNewAttribute (attType);
          TtaAttachAttribute (el, att, doc);
          TtaSetAttributeText (att, name, el, doc);
          
          attType.AttrTypeNum = Template_ATTR_currentType;
          att = TtaNewAttribute (attType);
          TtaAttachAttribute (el, att, doc);		  
          TtaSetAttributeText (att, name, el, doc);
          
          break;
        case Template_EL_option :
          aux = NULL;
          break;
        case Template_EL_bag :
          //Link to types
          //Allow editing the content
          break;
        case Template_EL_useEl :
        case Template_EL_useSimple :
          /* if this use element is not empty, don't do anything: it is
             supposed to contain a valid instance. This should be
             checked, though */
          if (!TtaGetFirstChild (el))
            InstantiateUse (t, el, doc, TRUE);
          break;
        case Template_EL_attribute :
          if (!loading)
            InstantiateAttribute (t, el, doc);
          break;
        case Template_EL_repeat :
          InstantiateRepeat (t, el, doc);
          break;
        default :
          break;
        }
    }
	
	child = TtaGetFirstChild (el);
	while (child)
    {
      aux = child;
      TtaNextSibling (&aux);
      ParseTemplate (t, child, doc, loading);
      child = aux;
    }
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void DoInstanceTemplate (char *templatename)
{
#ifdef TEMPLATES
	XTigerTemplate	t;
	ElementType		  elType;
	Element			    root, piElem, doctype, elFound, text;
	char		        *s, *charsetname = NULL, buffer[MAX_LENGTH];
  int             pi_type;
  Document        doc;

	//Instantiate all elements
	t = (XTigerTemplate) Dictionary_Get (Templates_Dic, templatename);
  doc = GetTemplateDocument (t);
	root =	TtaGetMainRoot (doc);
	ParseTemplate (t, root, doc, FALSE);

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
  if (!doctype)
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

  // update the document title
	if (!strcmp (s, "HTML"))
    {
      elType.ElTypeNum = HTML_EL_TITLE;
      elFound = TtaSearchTypedElement (elType, SearchInTree, root);
      if (elFound)
        {
          elFound = TtaGetFirstChild (elFound);
          TtaSetTextContent (elFound, (unsigned char *)Answer_text,
                             TtaGetDefaultLanguage (), doc);
        }
    }
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  PreInstantiateComponents: Instantiates all components in order to improve
  editing.
  ----------------------------------------------------------------------*/
void PreInstantiateComponents(XTigerTemplate t)
{
#ifdef TEMPLATES 
  DicDictionary components = GetComponents(t);
  Declaration comp;

  for(Dictionary_First(components);!Dictionary_IsDone(components);Dictionary_Next(components))
    {
      comp = (Declaration) Dictionary_CurrentElement(components);
      ParseTemplate(t, GetComponentContent(comp), GetTemplateDocument(t), TRUE);
    }
#endif /* TEMPLATES */
}
