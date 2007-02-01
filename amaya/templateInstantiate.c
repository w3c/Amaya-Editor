/*
 *
 *  COPYRIGHT INRIA and W3C, 2006-2007
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

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


/*----------------------------------------------------------------------
  CreateInstance
  ----------------------------------------------------------------------*/
void  CreateInstance(char *templatePath, char *instancePath)
{
#ifdef TEMPLATES
  Document     doc = 0;
  DocumentType docType;
  ElementType  elType;
  Element      root, title, text;
  char        *s;
  int          alreadyOnDoc = 0;
  ThotBool     alreadyViewing = FALSE;

  XTigerTemplate t = (XTigerTemplate)Dictionary_Get (Templates_Dic, templatePath);
  if (t == NULL)
    //The template must be loaded before calling this function
    return;

  doc = GetTemplateDocument (t);
  while (alreadyOnDoc < DocumentTableLength-1 && !alreadyViewing)
    {
      alreadyOnDoc++;
      if (DocumentURLs[alreadyOnDoc])
        alreadyViewing = !strcmp (DocumentURLs[alreadyOnDoc],instancePath);
    }

  if (!TtaPrepareUndo (doc))
    {
      TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
      root = TtaGetRootElement(doc);
      elType = TtaGetElementType (root);
      // get the target document type
      s = TtaGetSSchemaName (elType.ElSSchema);
      if (strcmp (s, "HTML") == 0)
        docType = docHTML;
      else if (strcmp (s, "SVG") == 0)
        docType = docSVG;
      else if (strcmp (s, "MathML") == 0)
        docType = docMath;
      else
        docType = docXml;

      SetRelativeURLs (doc, instancePath);
      
      switch (docType)
        {
        case docSVG:
          TtaExportDocumentWithNewLineNumbers (doc, instancePath, "SVGT");
          break;
        case docMath:
          TtaExportDocumentWithNewLineNumbers (doc, instancePath, "MathMLT");
          break;
        case docHTML:
          // Initialize the document title
          elType.ElTypeNum = HTML_EL_TITLE;
          title = TtaSearchTypedElement (elType, SearchInTree, root);
          text = TtaGetFirstChild (title);
          while (text)
            {
              elType = TtaGetElementType (text);
              if (elType.ElTypeNum == HTML_EL_TEXT_UNIT && Answer_text[0] != EOS)
                {
                  TtaSetTextContent (text, (unsigned char*)Answer_text,
                                     TtaGetDefaultLanguage (), doc);
                  text = NULL;
                }
              else if ((elType.ElTypeNum == Template_EL_useEl ||
                        elType.ElTypeNum == Template_EL_useSimple) &&
                       !strcmp (TtaGetSSchemaName (elType.ElSSchema), "Template"))
                // Ignore the template use element
                text = TtaGetFirstChild (text);
              else
                // Look for the first text child
                TtaNextSibling (&text);
            }
          if (TtaGetDocumentProfile(doc)==L_Xhtml11 || TtaGetDocumentProfile(doc)==L_Basic)
            TtaExportDocumentWithNewLineNumbers (doc, instancePath, "HTMLT11");
          else
            TtaExportDocumentWithNewLineNumbers (doc, instancePath, "HTMLTX");
          break;
        default:
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
      if(!text)
        return;
      if(strcmp (text, "optional") == 0)
      {
        TtaFreeMemory(text);
        return;
      }
      TtaFreeMemory(text);
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
                  if(text)
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
  Template_GetNewSimpleTypeInstance
  Create an new instance of xt:use/SimpleType
  The decl attribute must embed SimpleType declaration (no validation).
  @param decl Declaration of new element
  @param parent Future parent element
  @param doc Document
  @return The new element
  ----------------------------------------------------------------------*/
Element Template_GetNewSimpleTypeInstance(Document doc, Element parent, Declaration decl)
{
  Element           newEl = NULL;
#ifdef TEMPLATES
  ElementType       elType;
  char             *empty = " ";
  elType.ElSSchema = TtaGetSSchema(TEMPLATE_SSHEMA_NAME, doc);
  elType.ElTypeNum = Template_EL_TEXT_UNIT;
  newEl = TtaNewElement (doc, elType);
  
  TtaSetTextContent (newEl, (unsigned char*) empty, 0, doc);

#endif 
  return newEl;
}

/*----------------------------------------------------------------------
  Template_GetNewXmlElementInstance
  Create an new instance of xt:use/XmlElement
  The decl attribute must embed XmlElement declaration (no validation).
  @param decl Declaration of new element
  @param parent Future parent element
  @param doc Document
  @return The new element
  ----------------------------------------------------------------------*/
Element Template_GetNewXmlElementInstance(Document doc, Element parent, Declaration decl)
{
  Element           newEl = NULL;
#ifdef TEMPLATES
  ElementType       elType;

  GIType(decl->name, &elType, doc);
  if(elType.ElTypeNum!=0)
  {
    newEl = TtaNewTree (doc, elType, "");
  }
#endif /* TEMPLATES */
  return newEl;
}

/*----------------------------------------------------------------------
  Template_InsertUseChildren
  Insert children to a xt:use
  The dec parameter must be valid and will not be verified. It must be a
    direct child element (for union elements).
  @param el element (xt:use) in which insert a new element
  @param dec Template declaration of the element to insert
  @return The inserted element (the xt:use element if insertion is multiple as component)
  ----------------------------------------------------------------------*/
Element Template_InsertUseChildren(Document doc, Element el, Declaration dec)
{
  Element     newEl   = NULL;
#ifdef TEMPLATES
  Element     current = NULL;
  Element     child   = NULL;
  //char       *attrCurrentTypeValue;
  //ElementType elType;
  
  if(TtaGetDocumentAccessMode(doc))
  {
    switch (dec->nature)
    {
      case SimpleTypeNat:
        newEl = Template_GetNewSimpleTypeInstance(doc, el, dec);
        TtaInsertFirstChild (&newEl, el, doc);
        break;
      case XmlElementNat:
        newEl = Template_GetNewXmlElementInstance(doc, el, dec);
        TtaInsertFirstChild (&newEl, el, doc);
        break;
      case ComponentNat:
        newEl = TtaCopyTree(dec->componentType.content, doc, doc, el);
        ProcessAttr (dec->declaredIn, newEl, doc);
        
        /* Copy elements from new use to existing use. */
        while((child = TtaGetFirstChild(newEl)))
        {
          TtaRemoveTree (child, doc);
          if(current)
            TtaInsertSibling (child, current, FALSE, doc);
          else
            TtaInsertFirstChild (&child, el, doc);      
          current = child; 
        }
        
        /* Copy currentType attribute. */
        //attrCurrentTypeValue = GetAttributeStringValue (el, Template_ATTR_currentType, NULL);
        //SetAttributeStringValue (el, Template_ATTR_currentType, attrCurrentTypeValue);
        TtaDeleteTree(newEl, doc);
        newEl = el;
        break;
      case UnionNat :
        /* Nothing to do.*/
  //                elType.ElTypeNum = Template_EL_useEl;
  //                cont = TtaNewElement (doc, elType);
  //                if (cont)
  //                  {
  //                    TtaSetAccessRight (cont, ReadWrite, doc);
  //                    at = TtaNewAttribute (att);
  //                    if (at)
  //                      {
  //                        TtaAttachAttribute (cont, at, doc);
  //                        TtaSetAttributeText(at, types, cont, doc);
  //                      }
  //                  }
        /* @@@@@ */
        break;
      default :
        //Impossible
        break;   
    }
  }  
#endif /* TEMPLATES */
  return newEl;
}

/*----------------------------------------------------------------------
  InstantiateUse
  ----------------------------------------------------------------------*/
Element InstantiateUse (XTigerTemplate t, Element el, Document doc,
                        ThotBool insert)
{
#ifdef TEMPLATES
	Element          cont = NULL;
  ElementType      elType;
  Declaration      dec;
  int              size, nbitems;
  struct menuType  *items;
  char             *types;
  ThotBool          oldStructureChecking;

  if(!t)
    return NULL;

  /* get the value of the "types" attribute */
  cont = NULL;
  elType = TtaGetElementType (el);
  types = GetAttributeStringValueFromNum(el, Template_ATTR_types, &size);
  if(!types)
    return NULL;
  giveItems (types, size, &items, &nbitems);
  // No structure checking
  oldStructureChecking = TtaGetStructureChecking (doc);
  TtaSetStructureChecking (FALSE, doc);
  
  if (nbitems == 1)
    /* only one type in the "types" attribute */
    {
      dec = Template_GetDeclaration (t, items[0].label);
      if (dec)
        cont = Template_InsertUseChildren(doc, el, dec);
    }
  TtaFreeMemory(types);
  TtaFreeMemory(items);
  TtaSetStructureChecking (oldStructureChecking, doc);
  return cont;
#else /* TEMPLATES */
  return NULL;
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  InstantiateRepeat
  Check for min and max param and validate xt:repeat element content.
  @param registerUndo True to register undo creation sequences.
  ----------------------------------------------------------------------*/
void InstantiateRepeat (XTigerTemplate t, Element el, Document doc, ThotBool registerUndo)
{
#ifdef TEMPLATES
  int            curVal,  minVal,  maxVal;
  Attribute      curAtt,  minAtt,  maxAtt;
  AttributeType  curType, minType, maxType;
  char           *text;

  if(!t)
    return;

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
      if(registerUndo)
        TtaRegisterAttributeCreate(minAtt, el, doc);
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
      if(registerUndo)
        TtaRegisterAttributeCreate(maxAtt, el, doc);
    }

  if (!curAtt)
    {
      curAtt = TtaNewAttribute(curType);
      sprintf(text,"%d",curVal);
      TtaAttachAttribute(el, curAtt, doc);
      TtaSetAttributeText(curAtt, text, el, doc);
      if(registerUndo)
        TtaRegisterAttributeCreate(curAtt, el, doc);
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
      if(registerUndo)
        TtaRegisterElementCreate(newChild, doc);
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
	
  if(!t)
    return;
  
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
          
          name = GetAttributeStringValueFromNum (el, Template_ATTR_name, NULL);		  		  
          TtaRemoveAttribute (el, TtaGetAttribute (el, attType), doc);
          if (NeedAMenu (el, doc))
            TtaChangeElementType (el, Template_EL_useEl);
          else
            TtaChangeElementType (el, Template_EL_useSimple);
          
          attType.AttrTypeNum = Template_ATTR_types;
          att = TtaNewAttribute (attType);
          TtaAttachAttribute (el, att, doc);
          if(name)
            TtaSetAttributeText (att, name, el, doc);
          
          attType.AttrTypeNum = Template_ATTR_currentType;
          att = TtaNewAttribute (attType);
          TtaAttachAttribute (el, att, doc);
          if(name)
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
          InstantiateRepeat (t, el, doc, FALSE);
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
  if(!t)
    return;
  
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
  strcat (buffer, "\" version=\"");
  if (t->version)
    strcat (buffer, t->version);
  else
    strcat (buffer, "0.8");
  strcat (buffer, "\"");
  if(t->templateVersion)
    {
      strcat (buffer, " templateVersion=\"");
      strcat (buffer, t->templateVersion);
      strcat (buffer, "\"");
    }
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
  if(!t)
    return;

  DicDictionary components = GetComponents(t);
  Declaration comp;

  for(Dictionary_First(components);!Dictionary_IsDone(components);Dictionary_Next(components))
    {
      comp = (Declaration) Dictionary_CurrentElement(components);
      ParseTemplate(t, GetComponentContent(comp), GetTemplateDocument(t), TRUE);
    }
#endif /* TEMPLATES */
}
