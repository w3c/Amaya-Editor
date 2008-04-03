/*
 *
 *  COPYRIGHT INRIA and W3C, 2006-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#include "templates.h"

#define THOT_EXPORT extern
#include "templateDeclarations.h"

#include "Elemlist.h"

#include "wxdialogapi_f.h"
#include "EDITimage_f.h"
#include "HTMLsave_f.h"
#include "HTMLtable_f.h"
#include "html2thot_f.h"
#include "init_f.h"
#include "templates_f.h"
#include "templateDeclarations_f.h"
#include "templateInstantiate_f.h"
#include "Templatebuilder_f.h"
#include "templateLoad_f.h"
#include "templateUtils_f.h"
#include "fetchHTMLname_f.h"
#include "Template.h"

#ifdef TEMPLATES
#define TEMPLATE_SCHEMA_NAME "Template"

typedef struct _InstantiateCtxt
{
  char         *templatePath;
  char         *instancePath;
  char         *schemaName;
  Document      doc;
  DocumentType  docType;
  ThotBool      dontReplace;
} InstantiateCtxt;
#endif /* TEMPLATES */



/*----------------------------------------------------------------------
  Template_InsertRepeatChildAfter
  Insert a child to a xt:repeat
  The decl parameter must be valid and will not be verified. It must be a
    direct child element or the "use in the use" for union elements.
  @param el element (xt:repeat) in which insert a new element
  @param decl Template declaration of the element to insert
  @param elPrev Element (xt:use) after which insert the new elem, NULL if first.
  @return The inserted element 
  ----------------------------------------------------------------------*/
Element Template_InsertRepeatChildAfter (Document doc, Element el,
                                         Declaration decl, Element elPrev)
{
#ifdef TEMPLATES
  Element     useFirst; /* First xt:use of the repeat.*/
  Element     use;      /* xt:use to insert.*/
  ElementType useType;  /* type of xt:use.*/

  if (!TtaGetDocumentAccessMode (doc))
    return NULL;
  
  /* Copy xt:use with xt:types param */
  useFirst = TtaGetFirstChild (el);
  useType = TtaGetElementType (useFirst);
  use = TtaCopyElement (useFirst, doc, doc, el);

  TtaChangeElementType(use, Template_EL_useSimple);

  /* insert it */
  if (elPrev)
    TtaInsertSibling(use, elPrev, FALSE, doc);
  else
    TtaInsertSibling(use, useFirst, TRUE, doc);
  Template_InsertUseChildren(doc, use, decl);

  TtaRegisterElementCreate (use, doc);
  return use;
#else /* TEMPLATES */
  return NULL;
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Template_InsertRepeatChild
  Insert a child to a xt:repeat
  The decl parameter must be valid and will not be verified. It must be a
    direct child element or the "use in the use" for union elements.
  @param el element (repeat) in which insert a new element
  @param decl Template declaration of the element to insert
  @param pos Position of insertion (0 before all, 1 after first ... -1 after all)
  @return The inserted element
  ----------------------------------------------------------------------*/
Element Template_InsertRepeatChild (Document doc, Element el, Declaration decl, int pos)
{
#ifdef TEMPLATES
  if (!TtaGetDocumentAccessMode(doc) || !decl)
    return NULL;

  if (pos == 0)
    return Template_InsertRepeatChildAfter (doc, el, decl, NULL);
  else if (pos == -1)
    return Template_InsertRepeatChildAfter (doc, el, decl, TtaGetLastChild(el));
  else
  {
    Element elem = TtaGetFirstChild(el);
    pos--;
    while (pos > 0)
      {
        TtaNextSibling(&elem);
        pos--;
      }
    return Template_InsertRepeatChildAfter (doc, el, decl, elem);
  }
#else /* TEMPLATES */
  return NULL;
#endif /* TEMPLATES */
}


/*----------------------------------------------------------------------
  Template_InsertBagChild
  Insert a child to a xt:bag at the current insertion point.
  The decl parameter must be valid and will not be verified.
  @param el element (xt:bag) in which insert a new element
  @param decl Template declaration of the element to insert
  @return The inserted element
  ----------------------------------------------------------------------*/
Element Template_InsertBagChild (Document doc, Element el, Declaration decl, ThotBool before)
{
#ifdef TEMPLATES
  Element     sel;
  ElementType newElType, selType;
  int start, end;
  SSchema sstempl = TtaGetSSchema ("Template", doc);

  if (!TtaGetDocumentAccessMode (doc) || !decl)
    return NULL;

  TtaGiveFirstSelectedElement (doc, &sel, &start, &end);
  if (TtaIsAncestor (sel, el))
  {
    
    switch(decl->nature)
    {
      case UnionNat:
        newElType.ElTypeNum = Template_EL_useEl;
        newElType.ElSSchema = sstempl;
        break;
      case ComponentNat:
        newElType.ElTypeNum = Template_EL_useSimple;
        newElType.ElSSchema = sstempl;
        break;
      case XmlElementNat:
        GIType (decl->name, &newElType, doc);
        break;
      default:
        break;
    }

    selType = TtaGetElementType (sel);
    if (decl->blockLevel && 
        (TtaIsLeaf (selType) || !IsTemplateElement (sel)))
      {
        // force the insertion of a block level element at the right position
        while (sel && IsCharacterLevelElement (sel))
          sel = TtaGetParent (sel);
        if (sel)
          TtaSelectElement (doc, sel);
        TtaInsertAnyElement (doc, before);
        TtaExtendUndoSequence (doc);
      }
    TtaInsertElement (newElType, doc);
    TtaGiveFirstSelectedElement (doc, &sel, &start, &end);
    if (sel && newElType.ElSSchema == sstempl)
      {
        selType = TtaGetElementType (sel);
        TtaUnselect (doc);
        
        if (selType.ElSSchema == newElType.ElSSchema &&
            selType.ElTypeNum == Template_EL_useSimple)
          {
            SetAttributeStringValueWithUndo (sel, Template_ATTR_types, decl->name);
            SetAttributeStringValueWithUndo (sel, Template_ATTR_title, decl->name);
            Template_InsertUseChildren (doc, sel, decl);
          }
      }   
    return sel;
  }
#endif /* TEMPLATES */
  return NULL;
}


/*----------------------------------------------------------------------
  CreateTemplate
  Create a template from any document.
  ----------------------------------------------------------------------*/
void CreateTemplate(Document doc, char *templatePath)
{
#ifdef TEMPLATES
  Document          newdoc = 0;
  Element           root, head, elem, xt, title, child, last;
  ElementType       elType, xtType;
  char             *s;
  ThotBool          mathPI;
  SSchema           templSchema;
  XTigerTemplate    t;
  
  if(IsTemplateInstanceDocument(doc))
    {
      ShowMessage(TtaGetMessage (AMAYA, AM_TEMPLATE_ERR_INSTANCE),
          TtaGetMessage (AMAYA, AM_TEMPLATE_ERR_CREATION));
      return;
    }

  if(IsTemplateDocument(doc))
    {
      ShowMessage(TtaGetMessage (AMAYA, AM_TEMPLATE_ERR_TEMPLATE),
          TtaGetMessage (AMAYA, AM_TEMPLATE_ERR_CREATION));
      return;
    }
  
  root = TtaGetRootElement(doc);
  elType = TtaGetElementType (root);
  // get the target document type
  s = TtaGetSSchemaName (elType.ElSSchema);
  
  TtaNewNature (doc, elType.ElSSchema,  NULL, "Template", "TemplateP");
  TtaSetANamespaceDeclaration(doc, root, "xt", Template_URI);
  templSchema = TtaGetSSchema("Template", doc);
  TtaSetUriSSchema(templSchema, Template_URI);

  // Insert xt:head and others
  TtaSetStructureChecking (FALSE, doc);
  if (strcmp (s, "HTML") == 0)
    {
      // Initialize the xt:head
      elType.ElTypeNum = HTML_EL_HEAD;
      head = TtaSearchTypedElement (elType, SearchInTree, root);
      if(head)
        {
          xtType.ElSSchema = templSchema;
          xtType.ElTypeNum = Template_EL_head;
          xt = TtaNewElement(doc, xtType);
          
          elem = TtaGetLastChild(head);
          if(elem)
              TtaInsertSibling(xt, elem, FALSE, doc);
          else
              TtaInsertFirstChild(&xt, head, doc);
          
          SetAttributeStringValue(xt, Template_ATTR_version, Template_Current_Version);
          SetAttributeStringValue(xt, Template_ATTR_templateVersion, "1.0");
        }
      
      // Initialize the document title
      elType.ElTypeNum = HTML_EL_TITLE;
      title = TtaSearchTypedElement (elType, SearchInTree, root);
      if(title)
        {
          // Create xt:use for title
          xtType.ElTypeNum = Template_EL_useSimple;
          xt = TtaNewElement(doc, xtType);
          TtaInsertFirstChild(&xt, title, doc);
          SetAttributeStringValue(xt, Template_ATTR_types, "string");
          SetAttributeStringValue(xt, Template_ATTR_title, "title");
          
          // Move current title content to xt:use
          last = NULL;
          while(child = TtaGetLastChild(title), child!=NULL)
            {
              if(child==xt)
                break;
              TtaRemoveTree(child, doc);
              if(last)
                TtaInsertSibling(child, last, FALSE, doc);
              else
                TtaInsertFirstChild(&child, xt, doc);
              last = child;
            }
        }
    }
  else
    {
      xtType.ElSSchema = templSchema;
      xtType.ElTypeNum = Template_EL_head;
      xt = TtaNewElement(doc, xtType);
      TtaInsertFirstChild(&xt, root, doc);
      SetAttributeStringValue(xt, Template_ATTR_version, Template_Current_Version);
      SetAttributeStringValue(xt, Template_ATTR_templateVersion, "1.0");      
    }
  // Save changes
  TtaSetStructureChecking (TRUE, doc);
  
  // Save document
  TtaGetEnvBoolean ("GENERATE_MATHPI", &mathPI);
  TtaSetEnvBoolean("GENERATE_MATHPI", TRUE, TRUE);
  SaveDocumentToNewDoc(doc, newdoc, templatePath, NULL);
  TtaSetEnvBoolean("GENERATE_MATHPI", mathPI, TRUE);
  
  TtaClearUndoHistory (doc);
  RemoveParsingErrors (doc);

  TtaFreeMemory(DocumentURLs[doc]);
  DocumentURLs[doc] = TtaStrdup(templatePath);
  
  if(DocumentMeta[doc]==NULL)
    DocumentMeta[doc] = DocumentMetaDataAlloc();
  
  DocumentMeta[doc]->method = CE_TEMPLATE;
  if(DocumentMeta[doc]->initial_url)
    {
      TtaFreeMemory(DocumentMeta[doc]->initial_url);
      DocumentMeta[doc]->initial_url = NULL;
    }
  TtaSetDocumentModified (doc);

  // Load template-related infos :
  // like LoadTemplate(..)
  t = LookForXTigerTemplate(templatePath);
  t->doc = doc;
  Template_PrepareTemplate(t);
  //  DocumentTypes[doc] = docTemplate;
  t->state |= templloaded|templTemplate;

#ifdef AMAYA_DEBUG  
    DumpAllDeclarations();
#endif /* AMAYA_DEBUG */
    
  /* Update the URL combo box */
  AddURLInCombobox (DocumentURLs[doc], NULL, FALSE);
  TtaSetTextZone (doc, 1, URL_list);
  /* Update template menus */
  UpdateTemplateMenus(doc);

#endif /* TEMPLATES */  
}

/*----------------------------------------------------------------------
  CreateInstance
  basedoc is the displayed doc that launchs the creation of instance
  ----------------------------------------------------------------------*/
void CreateInstance(char *templatePath, char *instancePath, int basedoc)
{
#ifdef TEMPLATES
  Document          doc = 0, newdoc = 0;
  ElementType       elType;
  Element           root, title, text;
  CHARSET           charset;
  char             *localFile, *s, *charsetname;
  ThotBool          changes = FALSE;

  XTigerTemplate t = GetXTigerTemplate(templatePath);
  if (t == NULL)
    {
      // the template cannot be loaded
      InitConfirm (doc, 1, TtaGetMessage (AMAYA, AM_BAD_TEMPLATE));
      return;
    }
  // the template document
  doc = GetTemplateDocument (t);
  // localize the new created document
  if (DontReplaceOldDoc)
    newdoc = TtaGetNextDocumentIndex ();
  else
    newdoc = basedoc;

  if (!TtaHasUndoSequence (doc))
    {
      TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
      root = TtaGetRootElement(doc);
      elType = TtaGetElementType (root);
      // get the target document type
      s = TtaGetSSchemaName (elType.ElSSchema);
      // Do special stuff for HTML documents
      if (strcmp (s, "HTML") == 0)
        {
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
        }

      // update the charset if needed
      charsetname = TtaGetEnvString ("DOCUMENT_CHARSET");
      charset = TtaGetCharset (charsetname);
      if (charset != UNDEFINED_CHARSET &&
          DocumentMeta[doc]->charset &&
          strcmp (charsetname, DocumentMeta[doc]->charset))
        {
          TtaSetDocumentCharset (doc, charset, FALSE);
          DocumentMeta[doc]->charset = TtaStrdup (charsetname);
          SetNamespacesAndDTD (doc);
        }
      // Save HTML special changes
      TtaCloseUndoSequence (doc);
      changes = TRUE;

      // Save document
      SaveDocumentToNewDoc(doc, newdoc, instancePath, &localFile);

      // Revert HTML special changes
      if(changes)
        TtaUndoNoRedo (doc);
      
      TtaFreeMemory(localFile);

      TtaClearUndoHistory (doc);
      RemoveParsingErrors (doc);
      GetAmayaDoc (instancePath, NULL, basedoc, basedoc, CE_INSTANCE,
                   !DontReplaceOldDoc, NULL, NULL);
      TtaSetDocumentModified (newdoc);
      UpdateTemplateMenus(newdoc);
    }
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void InstantiateTemplate_callback (int newdoc, int status,  char *urlName,
                                   char *outputfile,
                                   char *proxyName, AHTHeaders *http_headers,
                                   void * context)
{
#ifdef TEMPLATES
  InstantiateCtxt *ctx = (InstantiateCtxt*)context;

  DoInstanceTemplate (ctx->templatePath);
  CreateInstance (ctx->templatePath, ctx->instancePath, ctx->doc);
  TtaFreeMemory (ctx->templatePath);
  TtaFreeMemory (ctx->instancePath);
  TtaFreeMemory (ctx);
#endif /* TEMPLATES */
}

///*----------------------------------------------------------------------
//  ----------------------------------------------------------------------*/
//void InstantiateTemplate (Document doc, char *templatename, char *docname,
//                          DocumentType docType, ThotBool loaded)
//{
//#ifdef TEMPLATES
//  if (!loaded)
//    {
//      // Create the callback context
//      InstantiateCtxt *ctx = (InstantiateCtxt *)TtaGetMemory (sizeof (InstantiateCtxt));
//      ctx->templatePath	= TtaStrdup (templatename);
//      ctx->instancePath	= TtaStrdup (docname);
//      ctx->schemaName = GetSchemaFromDocType(docType);
//      ctx->doc = doc;
//      ctx->docType = docType;
//
//      GetAmayaDoc (templatename, NULL, doc, doc, CE_MAKEBOOK, FALSE, 
//                   (void (*)(int, int, char*, char*, char*,
//                             const AHTHeaders*, void*)) InstantiateTemplate_callback,
//                   (void *) ctx);
//    }
//  else
//    {
//      DoInstanceTemplate (templatename);
//      CreateInstance (templatename, docname, doc);
//    }  
//#endif /* TEMPLATES */
//}

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
  int             val;

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
      val = TtaGetAttributeValue(useAttr);
      if (val == Template_ATTR_useAt_VAL_optional)
      {
        return;
      }
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
                  if (text)
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

  elType.ElSSchema = TtaGetSSchema("Template", doc);
  elType.ElTypeNum = Template_EL_TEXT_UNIT;
  newEl = TtaNewElement (doc, elType);
  TtaSetTextContent (newEl, (unsigned char*) empty, 0, doc);
#endif /* TEMPLATES */
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

  GIType (decl->name, &elType, doc);
  if (elType.ElTypeNum != 0)
  {
    newEl = TtaNewTree (doc, elType, "");
  }
#endif /* TEMPLATES */
  return newEl;
}


/*----------------------------------------------------------------------
  InsertWithNotify applies pre and post functions when inserting the new
  element el after child (if not NULL) or as first child of parent.
  ----------------------------------------------------------------------*/
Element InsertWithNotify (Element el, Element child, Element parent, Document doc)
{
  ElementType      elType;
  NotifyElement    event;
  char            *name;
  ThotBool         isRow = FALSE, isCell = FALSE;
  ThotBool         isImage = FALSE;
  ThotBool         oldStructureChecking;

  // avoid to check attributes now
  oldStructureChecking = TtaGetStructureChecking (doc);
  TtaSetStructureChecking (FALSE, doc);

  elType = TtaGetElementType (el);
  name = TtaGetSSchemaName (elType.ElSSchema);
  isCell = ((!strcmp (name,"HTML") &&
             elType.ElTypeNum == HTML_EL_Data_cell ||
             elType.ElTypeNum == HTML_EL_Heading_cell) ||
            (!strcmp (name,"MathML") && elType.ElTypeNum == MathML_EL_MTD));
  isRow = ((!strcmp (name,"HTML") && elType.ElTypeNum == HTML_EL_Table_row) ||
           (!strcmp (name,"MathML") &&
            (elType.ElTypeNum == MathML_EL_MTR ||
             elType.ElTypeNum == MathML_EL_MLABELEDTR)));
  isImage = (!strcmp (name,"HTML") && 
              (elType.ElTypeNum == HTML_EL_IMG || elType.ElTypeNum == HTML_EL_Object));
  if (child)
    TtaInsertSibling (el, child, FALSE, doc);
  else
    TtaInsertFirstChild (&el, parent, doc);
  TtaSetStructureChecking (oldStructureChecking, doc);

  if (isImage)
    InsertImageOrObject (el, doc);
  else if (isCell)
    {
      // a cell is created
      NewCell (el, doc, TRUE, TRUE, TRUE);
    }
  else if (isRow)
    {
      // a row is created
      event.element = el;
      event.document = doc;
      RowPasted (&event);
    }
  
  if (!strcmp (name,"HTML"))
    {
      elType.ElTypeNum = HTML_EL_IMG;
      child = TtaSearchTypedElement (elType, SearchInTree, el);
      while (child)
        {
          InsertImageOrObject (child, doc);
          child = TtaSearchTypedElementInTree (elType, SearchForward, el, child);
        }
      elType.ElTypeNum = HTML_EL_Object;
      child = TtaSearchTypedElement (elType, SearchInTree, el);
      while (child)
        {
          InsertImageOrObject (child, doc);
          child = TtaSearchTypedElementInTree (elType, SearchForward, el, child);
        }
    }
  return el;
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
  Element     newEl = NULL;
#ifdef TEMPLATES
  Element     current = NULL;
  Element     child = NULL;
  //char       *attrCurrentTypeValue;
  //ElementType elType;
  
  if (TtaGetDocumentAccessMode(doc))
  {
    switch (dec->nature)
    {
      case SimpleTypeNat:
        newEl = Template_GetNewSimpleTypeInstance(doc, el, dec);
        newEl = InsertWithNotify (newEl, NULL, el, doc);
        break;
      case XmlElementNat:
        newEl = Template_GetNewXmlElementInstance(doc, el, dec);
        newEl = InsertWithNotify (newEl, NULL, el, doc);
        break;
      case ComponentNat:
        newEl = TtaCopyTree(dec->componentType.content, doc, doc, el);
        ProcessAttr (dec->usedIn, newEl, doc);        
        /* Copy elements from new use to existing use. */
        while ((child = TtaGetFirstChild(newEl)))
        {
          TtaRemoveTree (child, doc);
          child = InsertWithNotify (child, current, el, doc);
          current = child; 
        }
        
        /* Copy currentType attribute. */
        //attrCurrentTypeValue = GetAttributeStringValue (el, Template_ATTR_currentType, NULL);
        //SetAttributeStringValue (el, Template_ATTR_currentType, attrCurrentTypeValue);
        TtaDeleteTree(newEl, doc);
        newEl = el;
        break;
      default :
        //Impossible
        break;   
    }
    Template_FixAccessRight (dec->usedIn, el, doc);
    TtaUpdateAccessRightInViews (doc, el);    
  }  
#endif /* TEMPLATES */
  return newEl;
}


/*----------------------------------------------------------------------
  Fix access rights.
  ----------------------------------------------------------------------*/
void Template_FixAccessRight (XTigerTemplate t, Element el, Document doc)
{
#ifdef TEMPLATES
  ElementType elType;
  Element     child;
  char        currentType[MAX_LENGTH];
  Declaration decl;
  
  if (t && el && doc)
    {
      elType = TtaGetElementType(el);
      if (elType.ElSSchema == TtaGetSSchema ("Template", doc))
        {
          switch (elType.ElTypeNum)
            {
            case Template_EL_TEXT_UNIT:
              //TtaSetAccessRight( el, ReadWrite, doc);
              return;
            case Template_EL_useEl:
            case Template_EL_useSimple:
              GiveAttributeStringValueFromNum(el, Template_ATTR_currentType,
                                              (char*)currentType, NULL);
              decl = Template_GetDeclaration(t, currentType);
              if (decl)
                {
                  switch (decl->nature)
                    {
                      case SimpleTypeNat:
                      case XmlElementNat:
                        TtaSetAccessRight (el, ReadWrite, doc);
                        return;
                      default:
                        TtaSetAccessRight (el, ReadOnly, doc);
                         break;
                    }
                }
              break;
            case Template_EL_bag:
            case Template_EL_repeat:
              TtaSetAccessRight(el, ReadWrite, doc);
              break;
            default:
              TtaSetAccessRight(el, ReadOnly, doc);
              break;
            }
        }

      child = TtaGetFirstChild (el);
      // fix access right to children
      while (child)
        {
          Template_FixAccessRight (t, child, doc);
          TtaNextSibling (&child);
        }
    }
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  AddPromptIndicator
  ----------------------------------------------------------------------*/
void AddPromptIndicator (Element el, Document doc)
{
#ifdef TEMPLATES
  ElementType         elType;
  AttributeType       attrType;
  Attribute           att;

  if (el)
    {
      elType = TtaGetElementType (el);
      attrType.AttrSSchema = elType.ElSSchema;
      attrType.AttrTypeNum = Template_ATTR_prompt;
      att = TtaGetAttribute (el, attrType);
      if (att == NULL)
        {
          att = TtaNewAttribute (attrType);
          TtaAttachAttribute (el, att, doc);
        }
    }
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  InstantiateUse
  ----------------------------------------------------------------------*/
Element InstantiateUse (XTigerTemplate t, Element el, Document doc,
                        ThotBool registerUndo)
{
#ifdef TEMPLATES
  Element          cont = NULL;
  ElementType      elType;
  Declaration      dec;
  int              size, nbitems, i;
  struct menuType  *items;
  char             *types, *text = NULL;
  ThotBool          oldStructureChecking;

  if (!t)
    return NULL;

  /* get the value of the "types" attribute */
  cont = NULL;
  elType = TtaGetElementType (el);
  AddPromptIndicator (el, doc);
  types = GetAttributeStringValueFromNum (el, Template_ATTR_types, &size);
  if (!types || types[0] == EOS)
    {
      TtaFreeMemory (types);
      return NULL;
    }
  giveItems (types, size, &items, &nbitems);
  // No structure checking
  oldStructureChecking = TtaGetStructureChecking (doc);
  TtaSetStructureChecking (FALSE, doc);
  
  if (nbitems == 1)
    /* only one type in the "types" attribute */
    {
      dec = Template_GetDeclaration (t, items[0].label);
      if (dec)
      {
        cont = Template_InsertUseChildren (doc, el, dec);
        if (cont)
        {
          TtaChangeTypeOfElement (el, doc, Template_EL_useSimple);
          if (registerUndo)
            TtaRegisterElementTypeChange (el, Template_EL_useEl, doc);
        }
      }
    }
  TtaFreeMemory (text);
  TtaFreeMemory (types);
  
  for (i = 0; i < nbitems; i++)
    TtaFreeMemory(items[i].label);
  TtaFreeMemory(items);
  TtaSetStructureChecking (oldStructureChecking, doc);
  
  Template_FixAccessRight (t, el, doc);
  TtaUpdateAccessRightInViews (doc, el);
  
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
void InstantiateRepeat (XTigerTemplate t, Element el, Document doc,
                        ThotBool registerUndo)
{
#ifdef TEMPLATES
  Element        child, newChild;
  ElementType    newElType;
  Attribute      minAtt,  maxAtt;
  AttributeType  minType, maxType;
  char          *text, *types = NULL, *title = NULL;
  int            curVal, minVal,  maxVal;
  int            childrenCount;


  if (!t)
    return;

  //Preparing types
  minType.AttrSSchema = TtaGetSSchema (TEMPLATE_SCHEMA_NAME, doc);
  minType.AttrTypeNum = Template_ATTR_minOccurs;
  maxType.AttrSSchema =  minType.AttrSSchema;
  maxType.AttrTypeNum = Template_ATTR_maxOccurs;
  newElType.ElSSchema = minType.AttrSSchema;
  //Get minOccurs and maxOccurs attributes
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
          curVal = minVal;
        }
      else
        //Error : Attribute with no value
        return;
    }
  else
    {
      minVal = 0;
      curVal = 1;
    }

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

  text = (char*)TtaGetMemory(MAX_LENGTH);
  //Create non existing min max attributes
  if (minAtt == NULL)
    {      
      minAtt = TtaNewAttribute (minType);
      sprintf (text, "%d", minVal);
      TtaAttachAttribute (el, minAtt, doc);
      TtaSetAttributeText (minAtt, text, el, doc);
      if (registerUndo)
        TtaRegisterAttributeCreate (minAtt, el, doc);
    }

  if (maxAtt == NULL)
    {  
      maxAtt = TtaNewAttribute (maxType);
      if (maxVal < INT_MAX)
        sprintf(text, "%d", maxVal);
      else
        sprintf (text, "*");
      TtaAttachAttribute (el, maxAtt, doc);      
      TtaSetAttributeText (maxAtt, text, el, doc);
      if (registerUndo)
        TtaRegisterAttributeCreate (maxAtt, el, doc);
    }
  TtaFreeMemory(text);

  //We must have minOccurs children
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
  types = GetAttributeStringValueFromNum (child, Template_ATTR_types, NULL);
  title = GetAttributeStringValueFromNum (child, Template_ATTR_title, NULL);
  newElType.ElTypeNum = Template_EL_useEl;
  while (childrenCount < curVal)
    {
      newChild = TtaNewElement (doc, newElType);
      // Insert it
      TtaInsertSibling (newChild, child, FALSE, doc);
      SetAttributeStringValueWithUndo (newChild, Template_ATTR_types, types);
      SetAttributeStringValueWithUndo (newChild, Template_ATTR_title, title);
      InstantiateUse (t, newChild, doc, TRUE);
      
      if (registerUndo)
        TtaRegisterElementCreate (newChild, doc);
      child = newChild;
      childrenCount++;
    }
    
  Template_FixAccessRight (t, el, doc);
  TtaUpdateAccessRightInViews (doc, el);
  TtaFreeMemory (types);
  TtaFreeMemory (title);
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

  if (!t || !el)
    return;
  
//  static int off = 0;
//  int i;
//  off++;
//  printf("ParseTemplate ");
//  for(i=0; i<off; i++)
//    printf(" ");
//  DumpTemplateElement(el, doc);
//  printf("\n");
  
  name = TtaGetSSchemaName (elType.ElSSchema);
  if (!strcmp (name, "Template"))
    {
      switch(elType.ElTypeNum)
        {
        case Template_EL_head :
          //Remove it and all of its children
          TtaDeleteTree(el, doc);
          //We must stop searching into this tree
//          off--;
          return;
          break;
        case Template_EL_component :
          // remove the name attribute
          attType.AttrSSchema = elType.ElSSchema;
          attType.AttrTypeNum = Template_ATTR_name;
          name = GetAttributeStringValueFromNum (el, Template_ATTR_name, NULL);		  		  
          TtaRemoveAttribute (el, TtaGetAttribute (el, attType), doc);
          // replace the component by a use
          if (NeedAMenu (el, doc))
            TtaChangeElementType (el, Template_EL_useEl);
          else
            TtaChangeElementType (el, Template_EL_useSimple);
          // generate the types attribute
          attType.AttrTypeNum = Template_ATTR_types;
          att = TtaNewAttribute (attType);
          TtaAttachAttribute (el, att, doc);
          if (name)
            TtaSetAttributeText (att, name, el, doc);
          // generate the title attribute
          attType.AttrTypeNum = Template_ATTR_title;
          att = TtaNewAttribute (attType);
          TtaAttachAttribute (el, att, doc);
          if (name)
            TtaSetAttributeText (att, name, el, doc);
          // generate the currentType attribute
          attType.AttrTypeNum = Template_ATTR_currentType;
          att = TtaNewAttribute (attType);
          TtaAttachAttribute (el, att, doc);
          if (name)
            TtaSetAttributeText (att, name, el, doc);
          TtaFreeMemory(name);
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
            // add the initial indicator
          AddPromptIndicator (el, doc);
            
          if (!TtaGetFirstChild (el))
            InstantiateUse (t, el, doc, FALSE);
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
//  off--;
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void DoInstanceTemplate (char *templatename)
{
#ifdef TEMPLATES
  XTigerTemplate  t;
  ElementType     elType;
  Element         root, piElem, doctype, line, text, elNew, elFound;
  Document        doc;
  char           *s, *charsetname = NULL, buffer[MAX_LENGTH];
  int             pi_type;

  //Instantiate all elements
  t = GetXTigerTemplate(templatename);
  if (!t)
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
  if (doctype == NULL)
    {
      elType.ElTypeNum = pi_type;      
      piElem = TtaSearchTypedElement (elType, SearchInTree, root);
      if (piElem == NULL)
        {
          /* generate the XML declaration */
          /* Check the Thot abstract tree against the structure schema. */
          TtaSetStructureChecking (FALSE, doc);
          piElem = TtaNewTree (doc, elType, "");
          TtaInsertFirstChild (&piElem, root, doc);
          line = TtaGetFirstChild (piElem);
          text = TtaGetFirstChild (line);
          strcpy (buffer, "xml version=\"1.0\" encoding=\"");
          charsetname = UpdateDocumentCharset (doc);
          strcat (buffer, charsetname);
          strcat (buffer, "\"");
          TtaSetTextContent (text, (unsigned char*)buffer,  Latin_Script, doc);
          TtaSetStructureChecking (TRUE, doc);
          TtaFreeMemory(charsetname);
        }
    }
  
  /* generate the XTiger PI */
  /* Check the Thot abstract tree against the structure schema. */
  TtaSetStructureChecking (FALSE, doc);
  elType.ElTypeNum = pi_type;
  elNew = TtaNewTree (doc, elType, "");
  if (doctype)
    TtaInsertSibling (elNew, doctype, FALSE, doc);
  else
    TtaInsertSibling (elNew, piElem, FALSE, doc);
  line = TtaGetFirstChild (elNew);
  text = TtaGetFirstChild (line);
  strcpy (buffer, "xtiger template=\"");
  strcat (buffer, templatename);
  strcat (buffer, "\" version=\"");
  if (t->version)
    strcat (buffer, t->version);
  else
    strcat (buffer, "0.8");
  strcat (buffer, "\"");
  if (t->templateVersion)
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
  Template_PreInstantiateComponents
  Instantiates all components in order to improve editing.
  ----------------------------------------------------------------------*/
void Template_PreInstantiateComponents (XTigerTemplate t)
{
#ifdef TEMPLATES 
  if (!t)
    return;

  ForwardIterator iter = SearchSet_GetForwardIterator(GetComponents(t));
  Declaration     dec;
  SearchSetNode   node;

  ITERATOR_FOREACH(iter, SearchSetNode, node)
    {
      dec = (Declaration) node->elem;
      ParseTemplate(t, GetComponentContent(dec), GetTemplateDocument(t), TRUE);
    }
  TtaFreeMemory(iter);
#endif /* TEMPLATES */
}
