/*
 *
 *  COPYRIGHT INRIA and W3C, 2006-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#include "templates.h"
#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"
#include "templateDeclarations.h"

#include "Elemlist.h"
#include "AHTURLTools_f.h"
#include "wxdialogapi_f.h"
#include "EDITimage_f.h"
#include "HTMLedit_f.h"
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
#include "fetchXMLname_f.h"
#include "styleparser_f.h"

#ifdef TEMPLATES
#define TEMPLATE_SCHEMA_NAME "Template"
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
  Element     child; /* First xt:use of the repeat.*/
  Element     use, parent; /* xt:use to insert.*/
  ElementType useType;  /* type of xt:use.*/
  char       *types = NULL;

  if (!TtaGetDocumentAccessMode (doc))
    return NULL;
  
  /* Copy xt:use with xt:types param */
  child = TtaGetFirstChild (el);
  useType = TtaGetElementType (child);
  use = TtaCopyElement (child, doc, doc, el);
  types = GetAttributeStringValueFromNum (child, Template_ATTR_types, NULL);
  if (useType.ElTypeNum != Template_EL_useSimple)
    TtaChangeElementType (use, Template_EL_useSimple);
  if (types)
    {
      SetAttributeStringValueWithUndo (use, Template_ATTR_types, types);
      TtaFreeMemory (types);
    }
  else
    SetAttributeStringValueWithUndo (use, Template_ATTR_types, decl->name);
  /* insert it */
  if (elPrev)
    TtaInsertSibling (use, elPrev, FALSE, doc);
  else
    TtaInsertSibling (use, child, TRUE, doc);
  // look for the enclosing target element
  parent = GetParentLine (use, useType.ElSSchema);
  Template_InsertUseChildren(doc, use, decl, parent, TRUE);
  SetAttributeStringValueWithUndo (use, Template_ATTR_title, decl->name);
  SetAttributeStringValueWithUndo (use, Template_ATTR_currentType, decl->name);
  TtaRegisterElementCreate (use, doc);
  return use;
#else /* TEMPLATES */
  return NULL;
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Template_InsertBagChild
  Insert a child to a xt:bag at the current insertion point.
  The decl parameter must be valid and will not be verified.
  @param sel the refered element. If NULL use the selection
  @param bag element (xt:bag) in which insert a new element
  @param decl Template declaration of the element to insert
  @return The inserted element
  ----------------------------------------------------------------------*/
Element Template_InsertBagChild (Document doc, Element sel, Element bag,
                                 Declaration decl, ThotBool before)
{
#ifdef TEMPLATES
  ElementType newElType, selType;
  Element     use = NULL, el, dummy = NULL;
  SSchema     sstempl;
  int         start, end;
  ThotBool    open;

  if (!TtaGetDocumentAccessMode (doc) || !decl)
    return NULL;

  TtaGiveFirstSelectedElement (doc, &el, &start, &end);
  if (sel == NULL)
    sel = el;
  if (sel == bag || TtaIsAncestor (sel, bag))
    {
      // opent the undo sequence if needed
      open = TtaHasUndoSequence (doc);
      if (!open)
        TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
      sstempl = TtaGetSSchema ("Template", doc);
      selType = TtaGetElementType (sel);
      if (decl->blockLevel == 2 && 
          (TtaIsLeaf (selType) || !IsTemplateElement (sel)))
        {
          // force the insertion of a block level element at the right position
          while (sel && IsCharacterLevelElement (sel))
            sel = TtaGetParent (sel);
          if (sel)
            TtaSelectElement (doc, sel);
        }

      if (decl->nature == XmlElementNat)
        {
          if (el == NULL && sel != bag)
            // force a selection
            TtaSelectElement (doc, sel);
          if (sel == bag)
            {
              // insert first an empty element
              if (DocumentTypes[doc] == docHTML)
                newElType.ElTypeNum = HTML_EL_Element;
              else if (DocumentTypes[doc] == docMath)
                newElType.ElTypeNum = MathML_EL_Construct;
              else
                newElType.ElTypeNum = XML_EL_XML_Element;
              newElType.ElSSchema = TtaGetDocumentSSchema (doc);
              dummy = TtaNewElement(doc, newElType);
              TtaInsertFirstChild (&dummy, bag, doc);
              TtaRegisterElementCreate (dummy, doc);
              TtaSelectElement (doc, dummy);
            }
          GIType (decl->name, &newElType, doc);
          TtaInsertAnyElement (doc, before);
          TtaExtendUndoSequence (doc);
          TtaInsertElement (newElType, doc);
          TtaGiveFirstSelectedElement (doc, &sel, &start, &end);
          if (dummy)
            {
              TtaRegisterElementDelete (dummy, doc);
              TtaDeleteTree (dummy, doc);
            }
        }
      else if (decl->nature == ComponentNat)
        {
          // create a use element
          newElType.ElTypeNum = Template_EL_useSimple;
          newElType.ElSSchema = sstempl;
          use = TtaNewElement(doc, newElType);
          if (use)
            {
              Template_InsertUseChildren (doc, use, decl, NULL, TRUE);
              if (sel != bag)
                TtaInsertSibling (use, sel, before, doc);
              else
                 TtaInsertFirstChild (&use, bag, doc);
              SetAttributeStringValueWithUndo (use, Template_ATTR_types, decl->name);
              SetAttributeStringValueWithUndo (use, Template_ATTR_title, decl->name);
              SetAttributeStringValueWithUndo (use, Template_ATTR_currentType, decl->name);
              TtaRegisterElementCreate (use, doc);
              
              sel = use;
            }
        }
      else if (decl->nature == UnionNat)
        {
          newElType.ElTypeNum = Template_EL_useEl;
          newElType.ElSSchema = sstempl;
        }
      // close the undo sequence
      if (!open)
        TtaCloseUndoSequence (doc);
      return sel;
    }
#endif /* TEMPLATES */
  return NULL;
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

/*----------------------------------------------------------------------
  ParseTemplate
  parentLine points to the enclosing pseudo paragraph or paragraph
  Parameter loading is TRUE when the document is not already loaded.
  Return the parentline to be considered for next elements
  ----------------------------------------------------------------------*/
Element ParseTemplate (XTigerTemplate t, Element el, Document doc,
                       Element parentLine, ThotBool loading)
{
#ifdef TEMPLATES
  AttributeType attType;
  Attribute     att;
  Element       next, child = NULL, savedInline, prev, parent = NULL;
  ElementType   elType, otherType;
  Declaration   dec;
  char         *name, *types;

  if (!t || !el)
    return parentLine;

  savedInline = parentLine;
  elType = TtaGetElementType (el);
  attType.AttrSSchema = elType.ElSSchema;
  name = TtaGetSSchemaName (elType.ElSSchema);
  if (!strcmp (name, "Template"))
    {
      switch (elType.ElTypeNum)
        {
        case Template_EL_head :
          //Remove it and all of its children
          TtaDeleteTree(el, doc);
          //We must stop searching into this tree
          return parentLine;
        case Template_EL_component :
          // remove the name attribute
          attType.AttrTypeNum = Template_ATTR_name;
          name = GetAttributeStringValueFromNum (el, Template_ATTR_name, NULL);
          TtaRemoveAttribute (el, TtaGetAttribute (el, attType), doc);
          // replace the component by a use
          prev = el;
          TtaPreviousSibling (&prev);
          if (prev == NULL)
            {
              next = el;
              TtaNextSibling (&next);
              if (next == NULL)
                parent = TtaGetParent (el);
            }
          TtaRemoveTree (el, doc);
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
          /* now reinsert the element new map */
          if (prev != NULL)
            TtaInsertSibling (el, prev, FALSE, doc);
          else if (next != NULL)
            TtaInsertSibling (el, next, TRUE, doc);
          else
            TtaInsertFirstChild (&el, parent, doc);
          TtaFreeMemory(name);
          Template_FixAccessRight (t, el, doc);
          TtaUpdateAccessRightInViews (doc, el);
          break;
        case Template_EL_bag :
          Template_FixAccessRight (t, el, doc);
          TtaUpdateAccessRightInViews (doc, el);
          break;
        case Template_EL_useEl :
        case Template_EL_useSimple :
          /* if this use element is not empty, don't do anything: it is
             supposed to contain a valid instance. This should be
             checked, though */
            // add the initial indicator
          types = GetAttributeStringValueFromNum (el, Template_ATTR_types, NULL);
          if (types)
            {
              child = TtaGetFirstChild (el);
              if (!strcmp (types, "string") || !strcmp (types, "number"))
                AddPromptIndicator (el, doc);
              else
                {
                  // avoid to have a block element within a pseudo paragraph
                  dec = Template_GetDeclaration (t, types);
                  if (dec && dec->blockLevel == 2 && parentLine)
                    {
                      // move the use element after the paragraph
                      child = TtaGetParent (el);
                      otherType = TtaGetElementType (child);
                      if (otherType.ElSSchema != elType.ElSSchema ||
                          otherType.ElTypeNum == Template_EL_repeat)
                        // not need to move the parent element
                          child = el;
                      next = child;
                      prev = parentLine;
                      while (child)
                        {
                          // move the element and next siblings after the pseudo paragraph
                          TtaNextSibling (&next);
                          TtaRemoveTree (child, doc);
                          TtaInsertSibling (child, prev, FALSE, doc);
                          prev = child;
                          child = next;
                        }
                      // elements are now out of the parent line
                      savedInline = NULL;
                      parentLine = NULL;
                      child = TtaGetFirstChild (el);
                    }

                  // generate the currentType attribute
                  otherType = TtaGetElementType (child);
                  if (otherType.ElSSchema == elType.ElSSchema &&
                      otherType.ElTypeNum == Template_EL_TemplateObject)
                    {
                      // not already instantiated
                      TtaDeleteTree (child, doc);
                      child = NULL;
                    }
                  else
                    {
                      attType.AttrTypeNum = Template_ATTR_currentType;
                      att = TtaGetAttribute (el, attType);
                      if (att == NULL)
                        {
                          att = TtaNewAttribute (attType);
                          TtaAttachAttribute (el, att, doc);
                        }
                      if (otherType.ElTypeNum == 1)
                        TtaSetAttributeText (att, "string", el, doc);
                      else
                        {
                          name = (char *)GetXMLElementName (otherType, doc);
                          if (name && strcmp (name,"???"))
                            TtaSetAttributeText (att, name, el, doc);
                        }
                    }
                }
            }
          if (child == NULL)
            InstantiateUse (t, el, doc, parentLine, FALSE);
          else
            {
              Template_FixAccessRight (t, el, doc);
              TtaUpdateAccessRightInViews (doc, el);
            }
          TtaFreeMemory (types);
          break;
        case Template_EL_attribute :
          if (!loading)
            InstantiateAttribute (t, el, doc);
          break;
        case Template_EL_repeat :
          InstantiateRepeat (t, el, doc, parentLine, FALSE);
          break;
        default :
          break;
        }
    }
  else if (!strcmp (name, "HTML") &&
           (elType.ElTypeNum == HTML_EL_Pseudo_paragraph ||
            elType.ElTypeNum == HTML_EL_Paragraph))
    parentLine = el;

  child = TtaGetFirstChild (el);
  while (child)
    {
      next = child;
      TtaNextSibling (&next);
      parentLine = ParseTemplate (t, child, doc, parentLine, loading);
      child = next;
    }
  return savedInline;
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  CreateTemplate
  Create a template from any document.
  ----------------------------------------------------------------------*/
void CreateTemplate (Document doc, char *templatePath)
{
#ifdef TEMPLATES
  Element           root, head, elem, xt, title, child, last;
  ElementType       elType, xtType;
  char             *s;
  SSchema           templSchema;
  XTigerTemplate    t;
  
  if (IsTemplateInstanceDocument(doc))
    {
      ShowMessage(TtaGetMessage (AMAYA, AM_TEMPLATE_ERR_INSTANCE),
          TtaGetMessage (AMAYA, AM_TEMPLATE_ERR_CREATION));
      return;
    }

  if (IsTemplateDocument(doc))
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
  TtaSetANamespaceDeclaration (doc, root, "xt", Template_URI);
  templSchema = TtaGetSSchema ("Template", doc);
  TtaSetUriSSchema (templSchema, Template_URI);

  // Insert xt:head and others
  TtaSetStructureChecking (FALSE, doc);
  if (strcmp (s, "HTML") == 0)
    {
      // Initialize the xt:head
      elType.ElTypeNum = HTML_EL_HEAD;
      xtType.ElSSchema = templSchema;
      head = TtaSearchTypedElement (elType, SearchInTree, root);
      if(head)
        {
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
      if (title)
        {
          // Create xt:use for title
          xtType.ElTypeNum = Template_EL_useSimple;
          xt = TtaNewElement (doc, xtType);
          TtaInsertFirstChild(&xt, title, doc);
          SetAttributeStringValue (xt, Template_ATTR_types, "string");
          SetAttributeStringValue (xt, Template_ATTR_title, "title");
          
          // Move current title content to xt:use
          last = NULL;
          while(child = TtaGetLastChild(title), child!=NULL)
            {
              if (child == xt)
                break;
              TtaRemoveTree (child, doc);
              if (last)
                TtaInsertSibling (child, last, FALSE, doc);
              else
                TtaInsertFirstChild (&child, xt, doc);
              last = child;
            }
        }
    }
  else
    {
      xtType.ElSSchema = templSchema;
      xtType.ElTypeNum = Template_EL_head;
      xt = TtaNewElement (doc, xtType);
      TtaInsertFirstChild (&xt, root, doc);
      SetAttributeStringValue (xt, Template_ATTR_version, Template_Current_Version);
      SetAttributeStringValue (xt, Template_ATTR_templateVersion, "1.0");      
    }
  // Save changes
  TtaSetStructureChecking (TRUE, doc);
  if (DocumentTypes[doc] == docHTML)
    // avoid positionned boxes to overlap the xt:head section
    SetBodyAbsolutePosition (doc);

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
  Template_PrepareTemplate(t, doc);
  //  DocumentTypes[doc] = docTemplate;
  t->state |= templloaded|templTemplate;

#ifdef TEMPLATE_DEBUG  
    DumpAllDeclarations();
#endif /* TEMPLATE_DEBUG */
    
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
void CreateInstance (char *templatePath, char *instancePath,
                     char *docname,  DocumentType docType, int basedoc)
{
#ifdef TEMPLATES
  Document          doc = 0, newdoc = 0;
  Element           root, title, text;
  ElementType       elType;
  CHARSET           charset, ocharset;
  char             *s, *charsetname, *ocharsetname, *localFile;

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
  
  // close current undo sepquence in the template document
  if (TtaHasUndoSequence (doc))
    TtaCloseUndoSequence (doc);

  // update the charset if needed
  charsetname = TtaGetEnvString ("DOCUMENT_CHARSET");
  charset = TtaGetCharset (charsetname);
  ocharsetname = DocumentMeta[doc]->charset;
  ocharset =  TtaGetCharset (ocharsetname);
  if (charset != UNDEFINED_CHARSET &&
      DocumentMeta[doc]->charset &&
      strcmp (charsetname, DocumentMeta[doc]->charset))
    {
      TtaSetDocumentCharset (doc, charset, FALSE);
      DocumentMeta[doc]->charset = TtaStrdup (charsetname);
      SetNamespacesAndDTD (doc, FALSE);
    }

  // register the document type to open the right page model
  DocumentTypes[newdoc] = docType;
  // Generate the instance content as a copy of the template
  localFile = SaveDocumentToNewDoc(doc, newdoc, instancePath);
  Template_PrepareInstance (instancePath, newdoc, t->version, templatePath);
  Template_AddReference (t);

  // Revert template changes
  TtaSetDocumentCharset (doc, ocharset, FALSE);
  TtaFreeMemory (DocumentMeta[doc]->charset);
  DocumentMeta[doc]->charset = ocharsetname;
  // Now parse the instance
  // The xtiger PI will be added and components will be removed
  GetAmayaDoc (instancePath, NULL, basedoc, basedoc, CE_INSTANCE,
               !DontReplaceOldDoc, NULL, NULL);
  if (DocumentMeta[newdoc])
    DocumentMeta[newdoc]->method = CE_ABSOLUTE;
  // Generate the HTML document title
  root = TtaGetRootElement(newdoc);
  elType = TtaGetElementType (root);
  // get the target document type
  s = TtaGetSSchemaName (elType.ElSSchema);
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
                                 TtaGetDefaultLanguage (), newdoc);
              text = NULL;
              SetNewTitle (newdoc);
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

  // Insert XTiger PI
  Template_InsertXTigerPI(newdoc, t);   
  // Parse template to fill structure and remove extra data
  ParseTemplate (t, root, newdoc, NULL, FALSE);
  TtaFreeMemory (localFile);
  TtaClearUndoHistory (newdoc);
  RemoveParsingErrors (newdoc);
  TtaSetDocumentModified (newdoc);
  UpdateTemplateMenus(newdoc);
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
  Create an new instance of xt:use/SimpleType for the document doc.
  Return the new element
  ----------------------------------------------------------------------*/
Element Template_GetNewSimpleTypeInstance(Document doc)
{
  Element           newEl = NULL;
#ifdef TEMPLATES
  ElementType       elType;
  const char       *empty = " ";

  elType.ElSSchema = TtaGetSSchema("Template", doc);
  elType.ElTypeNum = Template_EL_TEXT_UNIT;
  newEl = TtaNewElement (doc, elType);
  TtaSetTextContent (newEl, (unsigned char*) empty, 0, doc);
#endif /* TEMPLATES */
  return newEl;
}

/*----------------------------------------------------------------------
  Template_GetNewXmlElementInstance
  Create an new instance of xt:use/XmlElement for the document doc.
  The parameter decl gives the type of the element of new element.
  Return the new element
  ----------------------------------------------------------------------*/
Element Template_GetNewXmlElementInstance(Document doc, Declaration decl)
{
  Element           newEl = NULL;
#ifdef TEMPLATES
  ElementType       elType;

  GIType (decl->name, &elType, doc);
  if (elType.ElTypeNum != 0)
    newEl = TtaNewTree (doc, elType, "");
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
      // special management for images and objets
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
  The parentLine parameter points to the enclosing line if any.
  ----------------------------------------------------------------------*/
Element Template_InsertUseChildren (Document doc, Element el, Declaration dec,
                                    Element parentLine, ThotBool registerUndo)
{
  Element         newEl = NULL;
#ifdef TEMPLATES
  Element         current = NULL;
  Element         child = NULL, prev, next;
  ElementType     childType, elType; 
  SSchema         sshtml;
  XTigerTemplate  t;
  
  if (TtaGetDocumentAccessMode(doc))
  {
    switch (dec->nature)
    {
      case SimpleTypeNat:
        newEl = Template_GetNewSimpleTypeInstance(doc);
        newEl = InsertWithNotify (newEl, NULL, el, doc);
        break;
      case XmlElementNat:
        newEl = Template_GetNewXmlElementInstance(doc, dec);
        newEl = InsertWithNotify (newEl, NULL, el, doc);
        break;
      case ComponentNat:
        newEl = TtaCopyTree(dec->componentType.content, doc, doc, el);
        ProcessAttr (dec->usedIn, newEl, doc);
        elType = TtaGetElementType (el);
        /* Copy elements from new use to existing use. */
#ifdef TEMPLATE_DEBUG
        DumpSubtree(newEl, doc, 0);
#endif /* TEMPLATE_DEBUG */
        sshtml = TtaGetSSchema ("HTML", doc);
        t = GetXTigerDocTemplate( doc);
        child = TtaGetFirstChild  (newEl);
        while (child)
          {
            // move the new subtree to the document
            TtaRemoveTree (child, doc);
            childType = TtaGetElementType (child);
            if (parentLine)
              {
                if (childType.ElSSchema == sshtml &&
                    childType.ElTypeNum == HTML_EL_Pseudo_paragraph)
                  {
                    prev = TtaGetFirstChild  (child);
                    while (prev)
                      {
                        next = prev;
                        TtaNextSibling (&next);
                        TtaRemoveTree (prev, doc);
                        current = InsertWithNotify (prev, current, el, doc);
                        prev = next;
                      }
                    TtaDeleteTree (child, doc);
                  }
                else
                  current = InsertWithNotify (child, current, el, doc);
              }
            else
              {
                current = InsertWithNotify (child, current, el, doc);
                // check if a new paragraph is inserted
                if (childType.ElSSchema == sshtml &&
                    childType.ElTypeNum == HTML_EL_Paragraph)
                  Template_SetInline (child, elType.ElSSchema, doc, registerUndo);
                else
                  {
                    childType.ElSSchema = sshtml;
                    childType.ElTypeNum = HTML_EL_Paragraph;
                    child = TtaSearchTypedElement (childType, SearchInTree, current);
                    while (child)
                      {
                        Template_SetInline (child, elType.ElSSchema, doc, registerUndo);
                        child = TtaSearchTypedElement (childType, SearchInTree, child);
                      }
                  }
              }
            child = TtaGetFirstChild (newEl);
          }

        TtaDeleteTree (newEl, doc);
        newEl = el;
        break;
      default :
        //Impossible
        break;   
    }
    Template_FixAccessRight (dec->usedIn, el, doc);
    if (dec->nature == ComponentNat)
      Component_FixAccessRight (el, doc);
    TtaUpdateAccessRightInViews (doc, el);
  }  
#endif /* TEMPLATES */
  return newEl;
}


/*----------------------------------------------------------------------
  Component_FixAccessRight locks children of the component
  ----------------------------------------------------------------------*/
void Component_FixAccessRight (Element el, Document doc)
{
#ifdef TEMPLATES
  Element     child;
  
  if (el && doc)
    {
      TtaSetAccessRight (el, ReadOnly, doc);
      // fix access right to children
      child = TtaGetFirstChild (el);
      while (child)
        {
          TtaSetAccessRight (child, ReadOnly, doc);
          TtaNextSibling (&child);
        }
    }
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Template_FixAccessRight fixes access rights of the el element
  ----------------------------------------------------------------------*/
void Template_FixAccessRight (XTigerTemplate t, Element el, Document doc)
{
#ifdef TEMPLATES
  ElementType elType;
  Element     child;
  Declaration decl;
  char        currentType[MAX_LENGTH], *ptr;
  
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
            case Template_EL_component:
              Component_FixAccessRight (el, doc);
              break;
            case Template_EL_useEl:
            case Template_EL_useSimple:
              GiveAttributeStringValueFromNum(el, Template_ATTR_currentType,
                                              (char*)currentType, NULL);
              if (currentType[0] == EOS)
                {
                  GiveAttributeStringValueFromNum(el, Template_ATTR_types,
                                                  (char*)currentType, NULL);
                  ptr = strstr (currentType, " ");
                  if (ptr)
                    *ptr = EOS;
                }
              decl = Template_GetDeclaration(t, currentType);
              if (decl)
                {
                  switch (decl->nature)
                    {
                      case SimpleTypeNat:
                        TtaSetAccessRight (el, ReadWrite, doc);
                        return;
                      case ComponentNat:
                        TtaSetAccessRight (el, ReadOnly, doc);
                         break;
                        //Component_FixAccessRight (el, doc);
                        //return;
                      case XmlElementNat:
                        if (TtaIsSetReadOnly (el))
                          break;
                        child = TtaGetFirstChild (el);
                        if (child)
                          TtaSetAccessRight (child, ReadWrite, doc);
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

      // fix access right to children
      child = TtaGetFirstChild (el);
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
  Template_SetInline manages inline elements
  registerUndo says if changes must be registered
  ----------------------------------------------------------------------*/
void Template_SetInline (Element el, SSchema sstempl, Document doc, ThotBool registerUndo)
{
#ifdef TEMPLATES
  Element         child = NULL;
  ElementType     elType1, elType2, elType3;

  if (el)
    {
      elType1 = TtaGetElementType (el);
      if (elType1.ElSSchema == sstempl)
        // apply to hte current template element
        SetAttributeIntValue (el, Template_ATTR_SetInLine,
                              Template_ATTR_SetInLine_VAL_Yes_, registerUndo);
      else
        elType1.ElSSchema = sstempl;
      elType1.ElTypeNum = Template_EL_useSimple;
      elType2.ElTypeNum = Template_EL_useEl;
      elType2.ElSSchema = elType1.ElSSchema;
      elType3.ElTypeNum = Template_EL_repeat;
      elType3.ElSSchema = elType1.ElSSchema;
      child = TtaSearchElementAmong5Types (elType1, elType2, elType3, elType3, elType3,
                                           SearchForward, el);
      while (child && TtaIsAncestor (child, el))
        {
          SetAttributeIntValue (child, Template_ATTR_SetInLine,
                                Template_ATTR_SetInLine_VAL_Yes_, registerUndo);
          child = TtaSearchElementAmong5Types (elType1, elType2,
                                               elType3, elType3, elType3,
                                               SearchForward, child);
        }
    }
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  InstantiateUse intantiate the use element el.
  The parentLine parameter points to the enclosing line if any.
  ----------------------------------------------------------------------*/
Element InstantiateUse (XTigerTemplate t, Element el, Document doc,
                        Element parentLine, ThotBool registerUndo)
{
#ifdef TEMPLATES
  Element          child = NULL;
  ElementType      elType;
  Declaration      dec;
  int              size, nbitems, i, option;
  struct menuType  *items;
  char             *types;
  ThotBool          oldStructureChecking, hidden;

  if (!t)
    return NULL;

  /* get the value of the "types" attribute */
  elType = TtaGetElementType (el);
  types = GetAttributeStringValueFromNum (el, Template_ATTR_types, &size);
  if (!types || types[0] == EOS)
    {
      TtaFreeMemory (types);
      return NULL;
    }
  if (!strcmp (types, "string") || !strcmp (types, "number"))
    {
      child = TtaGetFirstChild (el);
      if (child == NULL)
        {
          child = Template_GetNewSimpleTypeInstance(doc);
          child = InsertWithNotify (child, NULL, el, doc);
        }
      AddPromptIndicator (el, doc);
    }
  else
    {
      giveItems (types, size, &items, &nbitems);
      // No structure checking
      oldStructureChecking = TtaGetStructureChecking (doc);
      TtaSetStructureChecking (FALSE, doc);
      hidden = ElementIsOptional (el);
      if (hidden)
        {
          // check the current option value
          option = GetAttributeIntValueFromNum (el, Template_ATTR_option);
          hidden = (option == Template_ATTR_option_VAL_option_unset);
        }
      if (nbitems == 1 || !hidden)
        /* only one type in the "types" attribute */
        {
          dec = Template_GetDeclaration (t, items[0].label);
          if (dec)
            child = Template_InsertUseChildren (doc, el, dec, parentLine, registerUndo);
          if (nbitems == 1 && elType.ElTypeNum != Template_EL_useSimple)
            {
              TtaChangeTypeOfElement (el, doc, Template_EL_useSimple);
              if (registerUndo)
                TtaRegisterElementTypeChange (el, Template_EL_useEl, doc);
            }
        }
      else
        {
          // insert almost a pseudo element
          elType.ElTypeNum = Template_EL_TemplateObject;
          child = TtaNewElement (doc, elType);
          TtaInsertFirstChild (&child, el, doc);
        }

      for (i = 0; i < nbitems; i++)
        TtaFreeMemory(items[i].label);
      TtaFreeMemory(items);

      if (parentLine)
        // display the element in line
        Template_SetInline (el, elType.ElSSchema, doc, registerUndo);
      TtaSetStructureChecking (oldStructureChecking, doc);
    }
  TtaFreeMemory (types);
  Template_FixAccessRight (t, el, doc);
  TtaUpdateAccessRightInViews (doc, el);
  return child;
#else /* TEMPLATES */
  return NULL;
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  InstantiateRepeat
  Check for min and max param and validate xt:repeat element content.
  @param registerUndo True to register undo creation sequences.
  The parentLine parameter points to the enclosing line if any.
  ----------------------------------------------------------------------*/
void InstantiateRepeat (XTigerTemplate t, Element el, Document doc,
                        Element parentLine, ThotBool registerUndo)
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
  child = TtaGetFirstChild (el);
  if (!child)
    //Error : a repeat must have at least one child which will be the model
    return;
  
  for (childrenCount = 0; child; TtaNextSibling(&child))
    //TODO : Check that every child is valid
    childrenCount ++;

  if (childrenCount > maxVal)
    //Error : too many children!
    return;  


  if (parentLine)
    // display the element in line
    Template_SetInline (el, minType.AttrSSchema, doc, registerUndo);

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
      InstantiateUse (t, newChild, doc, parentLine, TRUE);
      
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
  Template_InsertXTigerPI
  Insert the XTiger PI element in template instance.
  Param t is the XTigerTemplate structure of the template,
  not the template instance one.
  ----------------------------------------------------------------------*/
void Template_InsertXTigerPI(Document doc, XTigerTemplate t)
{
#ifdef TEMPLATES
  ElementType     elType;
  Element         root, piElem, doctype, line, text, elNew, elFound;
  char           *s, *charsetname = NULL, buffer[MAX_LENGTH];
  int             pi_type;

  if (!t || !doc)
    return;

  root =  TtaGetMainRoot (doc);
  if (root == NULL)
    return;
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
          TtaFreeMemory (charsetname);
          TtaRegisterElementCreate (piElem, doc);
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
  if (t->uri)
    strcat (buffer, t->uri);
  else if (t->base_uri)
    strcat (buffer, t->uri);
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
  TtaRegisterElementCreate (elNew, doc);
  TtaSetStructureChecking (TRUE, doc);

  // update the document title
  if (!strcmp (s, "HTML"))
    {
      elType.ElTypeNum = HTML_EL_TITLE;
      elFound = TtaSearchTypedElement (elType, SearchInTree, root);
      text = TtaGetFirstChild (elFound);
      while (text)
        {
          elType = TtaGetElementType (text);
          if (elType.ElTypeNum == HTML_EL_TEXT_UNIT && Answer_text[0] != EOS)
            {
              TtaRegisterElementReplace (text, doc);
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
#endif /* TEMPLATES */
}


/*----------------------------------------------------------------------
  Template_PreInstantiateComponents
  Instantiates all components in order to improve editing.
  ----------------------------------------------------------------------*/
void Template_PreInstantiateComponents (XTigerTemplate t)
{
#ifdef TEMPLATES 
  ForwardIterator iter;
  Declaration     dec;
  SearchSetNode   node;

  if (!t)
    return;

  if (Template_IsInstance (t))
    {
#ifdef TEMPLATE_DEBUG
      DumpAllDeclarations();
#endif /* TEMPLATE_DEBUG */  
      iter = SearchSet_GetForwardIterator(GetComponents(t));
#ifdef TEMPLATE_DEBUG
      printf("Template_PreInstantiateComponents %s\n", t->uri);
#endif /* TEMPLATE_DEBUG */  
      ITERATOR_FOREACH(iter, SearchSetNode, node)
        {
          dec = (Declaration) node->elem;
          ParseTemplate(t, GetComponentContent(dec), GetTemplateDocument(t), NULL, TRUE);
        }
      TtaFreeMemory(iter);
    }
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Template_SetName
  Set the xt:component or xt:union element xt:name attribute.
  Make it unique.
  Return TRUE if the name is not modified.
  ----------------------------------------------------------------------*/
ThotBool Template_SetName (Document doc, Element el, const char *name, ThotBool withUndo)
{
#ifdef TEMPLATES 
  AttributeType attType;
  Attribute     attr;
  ThotBool      res, res2;

  if (doc && el && name)
    {
      attType.AttrSSchema = TtaGetElementType(el).ElSSchema;
      attType.AttrTypeNum = Template_ATTR_name;
      attr = TtaGetAttribute(el, attType);
      if (attr == NULL)
        {
          attr = TtaNewAttribute (attType);
          TtaAttachAttribute (el, attr, doc);
          if (withUndo)
            TtaRegisterAttributeCreate (attr, el, doc);
        }
      if (withUndo)
         TtaRegisterAttributeReplace(attr, el, doc);
      TtaSetAttributeText (attr, name, el, doc);
      res = TtaIsValidID (attr, TRUE);
      res2 = !MakeUniqueName(el, doc, TRUE, FALSE);
      return (res || res2);
    }
#endif /* TEMPLATES */
  return FALSE;
}

/*----------------------------------------------------------------------
  Template_SetName
  Set the xt:component or xt:union element xt:name attribute.
  Make it unique.
  Return TRUE if the name is not modified.
  ----------------------------------------------------------------------*/
ThotBool Template_SetLabel (Document doc, Element el, const char *label, ThotBool withUndo)
{
#ifdef TEMPLATES 
  AttributeType attType;
  Attribute     attr;

  if (doc && el && label)
    {
      attType.AttrSSchema = TtaGetElementType(el).ElSSchema;
      attType.AttrTypeNum = Template_ATTR_title;
      attr = TtaGetAttribute(el, attType);
      if (attr == NULL)
        {
          attr = TtaNewAttribute (attType);
          TtaAttachAttribute (el, attr, doc);
          if (withUndo)
            TtaRegisterAttributeCreate (attr, el, doc);
        }
      if (withUndo)
         TtaRegisterAttributeReplace(attr, el, doc);
      TtaSetAttributeText (attr, label, el, doc);
      return TtaIsValidID (attr, TRUE);
    }
#endif /* TEMPLATES */
  return FALSE;
}

