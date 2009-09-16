/*
 *
 *  COPYRIGHT INRIA and W3C, 2006-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#include "templates.h"
#include "Templatename.h"
#include "templates_f.h"

#include "AHTURLTools_f.h"
#include "HTMLsave_f.h"
#include "templateUtils_f.h"
#include <stdarg.h>

/*----------------------------------------------------------------------
GetSchemaFromDocType: Returns the name of the schema corresponding to
a doc type.
----------------------------------------------------------------------*/
const char *GetSchemaFromDocType (DocumentType docType)
{
#ifdef TEMPLATES
	switch (docType)
    {
    case docAnnot :
		return "Annot";
    case docBookmark :
		return "Topics";
    case docSVG :
		return "SVG";
    case docMath :
		return "MathML";
    case docXml :
		return "XML";
    default :
		return "HTML";
    }
#endif // TEMPLATES
	return "HTML";
}

/*----------------------------------------------------------------------
  IsUseInstantiated
  Return TRUE if the use must be instantiated
  ----------------------------------------------------------------------*/
ThotBool IsUseInstantiated (Element el, Document doc)
{
#ifdef TEMPLATES
  Element       parent;
  ElementType	  elType, parentType;
  int           option;
  ThotBool      opt;

  elType = TtaGetElementType (el);
  if (elType.ElTypeNum == Template_EL_useEl &&
      elType.ElSSchema &&
      !strcmp (TtaGetSSchemaName (elType.ElSSchema), "Template"))
    {
      parent = TtaGetParent (el);
      parentType = TtaGetElementType (parent);
      if (parentType.ElTypeNum == Template_EL_repeat &&
          parentType.ElSSchema == elType.ElSSchema &&
          el == TtaGetFirstChild (parent))
        {
          // check if the minOccurs of the repeat is 0
          option = GetMinOccurence (parent, doc);
          opt = option != 0;
        }
      else
        {
          option = GetAttributeIntValueFromNum (el, Template_ATTR_option);
          opt = (option == 0 || option == Template_ATTR_option_VAL_option_set);
        }
  return opt;
    }
#endif /* TEMPLATES */
  return FALSE;
}

/*----------------------------------------------------------------------
Set the value of a string attribute
----------------------------------------------------------------------*/
void SetAttributeStringValue (Element el, int att, const char* value)
{
#ifdef TEMPLATES
  Document      doc = TtaGetDocument(el);
  AttributeType attType;
  Attribute     attribute;

  if (doc == 0 || !TtaGetDocumentAccessMode(doc))
    return;
  attType.AttrSSchema = TtaGetElementType(el).ElSSchema;
  attType.AttrTypeNum = att;
  attribute = TtaGetAttribute(el, attType);
  if (attribute == NULL)
    {
      attribute = TtaNewAttribute (attType);
      TtaAttachAttribute(el, attribute, doc);
    }
  TtaSetAttributeText(attribute, value, el, doc);
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
Set the value of a string attribute and registering it in undo sequence.
----------------------------------------------------------------------*/
void SetAttributeStringValueWithUndo (Element el, int att, char* value)
{
#ifdef TEMPLATES
  Document      doc = TtaGetDocument(el);
  AttributeType attType;
  Attribute     attribute;

  if (doc == 0 || !TtaGetDocumentAccessMode(doc))
    return;
  attType.AttrSSchema = TtaGetElementType(el).ElSSchema;
  attType.AttrTypeNum = att;
  attribute = TtaGetAttribute(el, attType);
  if (attribute == NULL)
    {
      attribute = TtaNewAttribute (attType);
      TtaAttachAttribute(el, attribute, doc);
      TtaRegisterAttributeCreate(attribute, el, doc);
    }
  TtaSetAttributeText(attribute, value, el, doc);
  TtaRegisterAttributeReplace(attribute, el, doc);
#endif /* TEMPLATES */
}


/*----------------------------------------------------------------------
  GetMinOccurence returns the minOccurs value
----------------------------------------------------------------------*/
int GetMinOccurence (Element el, Document doc)
{
  int            minVal = 1;
#ifdef TEMPLATES
  AttributeType  minType;
  Attribute      minAtt;
  char          *text;

  // Get minOccurs
  minType.AttrSSchema = TtaGetElementType (el).ElSSchema;
  minType.AttrTypeNum = Template_ATTR_minOccurs;
  minAtt = TtaGetAttribute (el, minType);
  if (minAtt)
    {
      text = GetAttributeStringValue(el, minAtt, NULL);
      if (text)
        {
          minVal = atoi(text);
          TtaFreeMemory(text);
        }
    }
#endif /* TEMPLATES */
  return minVal;
}

/*----------------------------------------------------------------------
Returns the value of a string attribute without copy it
----------------------------------------------------------------------*/
void GiveAttributeStringValueFromNum (Element el, int att, char* buff, int* sz)
{
#ifdef TEMPLATES
  AttributeType attType;
  Attribute     attribute;
  int           size;

  attType.AttrSSchema = TtaGetElementType(el).ElSSchema;
  attType.AttrTypeNum = att;
  attribute = TtaGetAttribute(el, attType);

  size = TtaGetTextAttributeLength(attribute);
  TtaGiveTextAttributeValue (attribute, buff, &size);
  buff[size] = EOS;
  if (sz)
    *sz = size;
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Returns the value of a string attribute or NULL
----------------------------------------------------------------------*/
char *GetAttributeStringValueFromNum (Element el, int att, int* sz)
{
#ifdef TEMPLATES
	AttributeType attType;
  Attribute     attribute;
  char         *aux;
  int           size;

	attType.AttrSSchema = TtaGetElementType(el).ElSSchema;
	attType.AttrTypeNum = att;
	attribute = TtaGetAttribute(el, attType);
	if (attribute == NULL)
    return NULL;
	size = TtaGetTextAttributeLength (attribute);
	aux = (char*) TtaGetMemory (size+1);
	TtaGiveTextAttributeValue (attribute, aux, &size);
  aux[size] = EOS;
  if (sz)
    *sz = size;
	return aux;
#else
	return NULL;
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
Returns the value of an int attribute
----------------------------------------------------------------------*/
int GetAttributeIntValueFromNum (Element el, int att)
{
#ifdef TEMPLATES
  AttributeType attType;
  Attribute     attribute;

  attType.AttrSSchema = TtaGetElementType (el).ElSSchema;
  attType.AttrTypeNum = att;
  attribute = TtaGetAttribute (el, attType);
  if (attribute)
    return TtaGetAttributeValue (attribute);
  else
    return 0;
#else
  return 0;
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
Set the value of a int attribute and registering it in undo sequence if wanted
----------------------------------------------------------------------*/
void SetAttributeIntValue (Element el, int att, int value, ThotBool undo)
{
#ifdef TEMPLATES
  Document      doc = TtaGetDocument(el);
  AttributeType attType;
  Attribute     attribute;

  if (doc == 0 || !TtaGetDocumentAccessMode(doc))
    return;
  attType.AttrSSchema = TtaGetElementType(el).ElSSchema;
  attType.AttrTypeNum = att;
  attribute = TtaGetAttribute(el, attType);
  if (attribute == NULL)
    {
      attribute = TtaNewAttribute (attType);
      TtaAttachAttribute(el, attribute, doc);
      if (undo)
        TtaRegisterAttributeCreate(attribute, el, doc);
    }
  TtaSetAttributeValue(attribute, value, el, doc);
  if (undo)
    TtaRegisterAttributeReplace(attribute, el, doc);
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
Returns the value of a string attribute
----------------------------------------------------------------------*/
char *GetAttributeStringValue (Element el, Attribute attribute, int* sz)
{
#ifdef TEMPLATES
	int size = TtaGetTextAttributeLength(attribute);
	char *aux = (char*) TtaGetMemory(size+1);

	TtaGiveTextAttributeValue (attribute, aux, &size);
  aux[size] = EOS;
  if (sz)
    *sz = size;
	return aux;
#else
	return NULL;
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  GetAncestorComponentName returns the name of the ancestor component
  or NULL;
  ----------------------------------------------------------------------*/
char *GetAncestorComponentName (Element *el)
{
#ifdef TEMPLATES
  ElementType   elType;
  Element       anc = NULL;
  char         *name;

  elType = TtaGetElementType (*el);
  elType.ElTypeNum = Template_EL_component;
  anc = TtaGetParent (*el);
  anc = TtaGetExactTypedAncestor (anc, elType);
  if (anc)
    {
      name = GetAttributeStringValueFromNum (anc, Template_ATTR_name, NULL);
      *el = anc;
      return name;
    }
#endif /* TEMPLATES */
  return NULL;
}

/*----------------------------------------------------------------------
  AllowAttributeEdit returns TRUE if the template allows the user to edit
  the attribute
  ----------------------------------------------------------------------*/
ThotBool AllowAttributeEdit (Element el, Document doc, char *name)
{
#ifdef TEMPLATES
  Element      child;
  ElementType  childType;
  char         *value;
  int           val;
  ThotBool      ok;

  if (name == NULL || *name == EOS)
    return FALSE;
  child = TtaGetFirstChild (el);
  while (child)
    {
      childType = TtaGetElementType (child);
      if (childType.ElTypeNum == Template_EL_attribute &&
          !strcmp (TtaGetSSchemaName(childType.ElSSchema) , "Template"))
        {
          value = GetAttributeStringValueFromNum (child, Template_ATTR_ref_name, NULL);
          ok = (value && !strcmp (value, name));
          TtaFreeMemory (value);
          if (ok)
            {
              val = GetAttributeIntValueFromNum (el, Template_ATTR_useAt);
              if (val == Template_ATTR_useAt_VAL_prohibited)
                return FALSE;
              value = GetAttributeStringValueFromNum (child, Template_ATTR_fixed, NULL);
              if (value)
                {
                  TtaFreeMemory (value);
                  return FALSE;
                }
              return ok;
            }
          else
            TtaNextSibling(&child);
        }
      else
        return FALSE;
    }
#endif /* TEMPLATES */
  return FALSE;
}

/*----------------------------------------------------------------------
GetFirstEditableElement
Returns the first descendant element which is modifiable.
----------------------------------------------------------------------*/
Element GetFirstEditableElement (Element el)
{
  ElementType     elType;
  Element         res = NULL, current;

  current = TtaGetFirstChild(el);
  while (!res && current)
    {
      // skip col colgroup 
      elType = TtaGetElementType (current);
      if ((elType.ElSSchema &&
           strcmp (TtaGetSSchemaName(elType.ElSSchema), "HTML")) ||
          (elType.ElTypeNum != HTML_EL_ColStruct &&
           elType.ElTypeNum != HTML_EL_Table_head &&
           elType.ElTypeNum != HTML_EL_Comment_))
        res = GetFirstEditableElement (current);
      TtaNextSibling(&current);
    }
  if (!res && !TtaIsReadOnly(el))
    res = el;
  return res;
}

/*----------------------------------------------------------------------
  TemplateCanInsertFirstChild
  Test if an element can be inserted as child of another, bypassing xt.
----------------------------------------------------------------------*/
ThotBool TemplateCanInsertFirstChild (ElementType elementType, Element parent,
                                      Document document)
{
#ifdef TEMPLATES
  SSchema         templateSSchema = TtaGetSSchema ("Template", document);
  ElementType     parType;

  while (parent)
    {
      parType = TtaGetElementType(parent);
      if (parType.ElSSchema != templateSSchema)
        break;
      parent = TtaGetParent(parent);
    }
  if (!parent)
    return FALSE;
#endif /* TEMPLATES */
  return TtaCanInsertFirstChild(elementType, parent, document);
}

/*----------------------------------------------------------------------
  CheckTemplateAttrInMenu
  Validate the status of an attribute according to xt::atribute rules.
	Return TRUE if the attribute is not valid
  ----------------------------------------------------------------------*/
ThotBool CheckTemplateAttrInMenu (NotifyAttribute *event)
{
#ifdef TEMPLATES
  Document      doc = event->document;
  Element       elem, parent = event->element;
  ElementType   elType;
  SSchema       schema;
  AttributeType attrType;
  Attribute     attr;
  char         *attrName;
  char          buffer[MAX_LENGTH];
  int           sz, useAt, type;

  /* Prevent from showing attributes for template instance but not templates. */
  if (IsTemplateInstanceDocument(doc))
    {
      schema = TtaGetSSchema ("Template", doc);
      /* Prevent if attribute's element is not a descendant of xt:use */
      /* Dont prevent if descendant of xt:bag. */
      elem = GetFirstTemplateParentElement (parent);
      if (elem)
        {
          elType = TtaGetElementType (elem);
          if (elType.ElTypeNum == Template_EL_bag)
            return FALSE;	/* let Thot perform normal operation */
          if (elType.ElTypeNum != Template_EL_useSimple)
            return TRUE;
          if (!TtaIsReadOnly (parent))
            return FALSE;	/* let Thot perform normal operation */
        }
      /* Search for the corresponding xt:attribute element*/
      attrName = TtaGetAttributeName (event->attributeType);
      attrType.AttrSSchema = schema;
      for (elem = TtaGetFirstChild (parent); elem; TtaNextSibling (&elem))
        {
          attrType.AttrTypeNum = Template_ATTR_ref_name;
          elType = TtaGetElementType(elem);
          if (elType.ElTypeNum == Template_EL_attribute && elType.ElSSchema == schema)
            {
               attr = TtaGetAttribute(elem, attrType);
               if (attr)
                 {
                   sz = MAX_LENGTH;
                   TtaGiveTextAttributeValue(attr, buffer, &sz);
                   if (!strcmp(buffer, attrName))
                     {
                       /* Process the attribute filtering */
                       /* Get 'useAt' attr value. */
                       attrType.AttrTypeNum = Template_ATTR_useAt;
                       attr = TtaGetAttribute(elem, attrType);
                       if (attr)
                         useAt = TtaGetAttributeValue(attr);
                       else
                         useAt = Template_ATTR_useAt_VAL_required;
                       /* Get 'type' attr value. */
                       attrType.AttrTypeNum = Template_ATTR_type;
                       attr = TtaGetAttribute(elem, attrType);
                       if (attr)
                         type = TtaGetAttributeValue(attr);
                       else
                         type = Template_ATTR_type_VAL_string;

                       event->restr.RestrType = (RestrictionContentType)type;
                       /* If attr is prohibited, dont show it.*/
                       if (useAt == Template_ATTR_useAt_VAL_prohibited)
                           return TRUE;
                       if (useAt == Template_ATTR_useAt_VAL_required)
                         /* Force the usage of this attribute.*/
                         event->restr.RestrFlags |= attr_mandatory;

                       /* Get 'fixed' attr value. */
                       attrType.AttrTypeNum = Template_ATTR_fixed;
                       attr = TtaGetAttribute(elem, attrType);
                       if (attr)
                         {
                           sz = MAX_LENGTH;
                           TtaGiveTextAttributeValue(attr, buffer, &sz);
                           event->restr.RestrFlags |= attr_readonly;
                           event->restr.RestrDefVal = TtaStrdup(buffer);
                           return FALSE;	/* let Thot perform normal operation */
                         }

                       /* Get 'default' attr value.*/
                       attrType.AttrTypeNum = Template_ATTR_defaultAt;
                       attr = TtaGetAttribute(elem, attrType);
                       if (attr)
                         {
                           sz = MAX_LENGTH;
                           TtaGiveTextAttributeValue(attr, buffer, &sz);
                           event->restr.RestrDefVal = TtaStrdup(buffer);
                         }

                       /* Get 'values' attr value.*/
                       attrType.AttrTypeNum = Template_ATTR_values;
                       attr = TtaGetAttribute(elem, attrType);
                       if (attr)
                         {
                           sz = MAX_LENGTH;
                           TtaGiveTextAttributeValue(attr, buffer, &sz);
                           event->restr.RestrEnumVal = TtaStrdup(buffer);
                           event->restr.RestrFlags |= attr_enum;
                         }
                       return FALSE;	/* let Thot perform normal operation */
                     }
                 }
            }
        }
      return TRUE;
    }
#endif /* TEMPLATES */
  return FALSE;
}

/*----------------------------------------------------------------------
 * Dump element path
  ----------------------------------------------------------------------*/
void DumpElementSubPath (Element el, char* buffer)
{
#ifdef TEMPLATE_DEBUG
  Element parent = TtaGetParent(el);
  if (parent == NULL)
    strcpy(buffer, TtaGetElementTypeName(TtaGetElementType(el)));
  else
    {
      DumpElementSubPath(parent, buffer);
      strcat(buffer, "/");
      strcat(buffer, TtaGetElementTypeName(TtaGetElementType(el)));
    }
#endif /* TEMPLATE_DEBUG */
}

/*----------------------------------------------------------------------
 * Dump element path
  ----------------------------------------------------------------------*/
void DumpElementPath (Element el)
{
#ifdef TEMPLATE_DEBUG
  char buffer[MAX_LENGTH];

  DumpElementSubPath(el, buffer);
  printf("%s\n", buffer);
#endif /* TEMPLATE_DEBUG */
}


/*----------------------------------------------------------------------
 * Dump template element
  ----------------------------------------------------------------------*/
void DumpTemplateElement (Element el, Document doc)
{
#ifdef TEMPLATE_DEBUG
  ElementType    elType;
  AttributeType  attType;
  Attribute      att;
  SSchema        schema = TtaGetSSchema ("Template", doc);
  char*          str;
  char           buffer[MAX_LENGTH];
  int            len;
  Language       lang;

  if (el && doc)
    {
      elType = TtaGetElementType(el);
      printf("- %p %d ", elType.ElSSchema, elType.ElTypeNum);
      printf(" %s", TtaGetSSchemaName(elType.ElSSchema));
      printf(":%s", TtaGetElementTypeName(elType));
      if (elType.ElTypeNum == 1)
        {
          len = MAX_LENGTH-1;
          TtaGiveTextContent(el, (unsigned char*)buffer, &len, &lang);
          buffer[len] = EOS;
          printf(" \"%s\"", buffer);
        }

      if (elType.ElSSchema == schema)
        {
          switch(elType.ElTypeNum)
            {
              case Template_EL_head:
                str = GetAttributeStringValueFromNum(el, Template_ATTR_version, NULL);
                printf(" version=%s", str);
                TtaFreeMemory(str);
                str = GetAttributeStringValueFromNum(el, Template_ATTR_templateVersion, NULL);
                printf(" templateVersion=%s", str);
                TtaFreeMemory(str);
                break;
              case Template_EL_component:
                str = GetAttributeStringValueFromNum(el, Template_ATTR_name, NULL);
                printf(" name=%s", str);
                TtaFreeMemory(str);
                break;
              case Template_EL_union:
                str = GetAttributeStringValueFromNum(el, Template_ATTR_name, NULL);
                printf(" name=%s", str);
                TtaFreeMemory(str);
                str = GetAttributeStringValueFromNum(el, Template_ATTR_includeAt, NULL);
                printf(" include=%s", str);
                TtaFreeMemory(str);
                str = GetAttributeStringValueFromNum(el, Template_ATTR_exclude, NULL);
                printf(" exclude=%s", str);
                TtaFreeMemory(str);
                break;
              case Template_EL_import:
                str = GetAttributeStringValueFromNum(el, Template_ATTR_src, NULL);
                printf(" src=%s", str);
                TtaFreeMemory(str);
                break;
              case Template_EL_repeat:
                str = GetAttributeStringValueFromNum(el, Template_ATTR_title, NULL);
                printf(" label=%s", str);
                TtaFreeMemory(str);
                str = GetAttributeStringValueFromNum(el, Template_ATTR_minOccurs, NULL);
                printf(" minOccurs=%s", str);
                TtaFreeMemory(str);
                str = GetAttributeStringValueFromNum(el, Template_ATTR_maxOccurs, NULL);
                printf(" maxOccurs=%s", str);
                TtaFreeMemory(str);
                break;
              case Template_EL_useSimple:
              case Template_EL_useEl:
                str = GetAttributeStringValueFromNum(el, Template_ATTR_title, NULL);
                printf (" label=%s", str);
                TtaFreeMemory(str);
                str = GetAttributeStringValueFromNum(el, Template_ATTR_types, NULL);
                printf (" types=%s", str);
                TtaFreeMemory(str);
                attType.AttrSSchema = elType.ElSSchema;
                attType.AttrTypeNum = Template_ATTR_option;
                att = TtaGetAttribute (el, attType);
                if (att)
                  printf (" option");
                break;
              case Template_EL_bag:
                str = GetAttributeStringValueFromNum(el, Template_ATTR_title, NULL);
                printf(" label=%s", str);
                TtaFreeMemory(str);
                str = GetAttributeStringValueFromNum(el, Template_ATTR_types, NULL);
                printf(" types=%s", str);
                TtaFreeMemory(str);
                break;
              case Template_EL_attribute:
                str = GetAttributeStringValueFromNum(el, Template_ATTR_ref_name, NULL);
                printf (" name=%s", str);
                TtaFreeMemory(str);
                str = GetAttributeStringValueFromNum(el, Template_ATTR_type, NULL);
                printf (" type=%s", str);
                TtaFreeMemory(str);
                str = GetAttributeStringValueFromNum(el, Template_ATTR_useAt, NULL);
                printf (" use=%s", str);
                TtaFreeMemory(str);
                str = GetAttributeStringValueFromNum(el, Template_ATTR_defaultAt, NULL);
                printf (" default=%s", str);
                TtaFreeMemory(str);
                str = GetAttributeStringValueFromNum(el, Template_ATTR_fixed, NULL);
                printf (" fixed=%s", str);
                TtaFreeMemory(str);
                break;
            }
        }
    }
#endif /* TEMPLATE_DEBUG */
}

/*----------------------------------------------------------------------
 * Dump subtree
  ----------------------------------------------------------------------*/
void DumpSubtree(Element el, Document doc, int off)
{
#ifdef TEMPLATE_DEBUG
  Element child = TtaGetFirstChild(el);
  int i;

  for (i=0; i<off; i++)
    printf("  ");
  DumpTemplateElement(el, doc);
  printf("\n");

  while (child)
    {
      DumpSubtree(child, doc, off+1);
      TtaNextSibling(&child);
    }
#endif /* TEMPLATE_DEBUG */
}

/*----------------------------------------------------------------------
  Save an opened document to a specified path in order to open.
  The parameter doc is the original doc to be saved
  The parameter newdoc is the new document
  The parameter newpath is the newdoc URI
  Return the saved localFile (to be freed) or NULL
  ----------------------------------------------------------------------*/
char *SaveDocumentToNewDoc(Document doc, Document newdoc, char* newpath)
{
  ElementType   elType;
  Element       root;
  char         *localFile = NULL, *s;
  ThotBool      res = FALSE;

  localFile = GetLocalPath (newdoc, newpath);

  // apply link changes to the template
    TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
  SetRelativeURLs (doc, newpath, NULL, FALSE, FALSE, FALSE, FALSE);
  // prepare the new document view
  TtaExtractName (newpath, DirectoryName, DocumentName);

  root = TtaGetRootElement(doc);
  elType = TtaGetElementType (root);
  // get the target document type
  s = TtaGetSSchemaName (elType.ElSSchema);
  if (strcmp (s, "HTML") == 0)
    {
      /* docType = docHTML; */
      if (TtaGetDocumentProfile(doc) == L_Xhtml11 ||
          TtaGetDocumentProfile(doc) == L_Basic)
        res = TtaExportDocumentWithNewLineNumbers (doc, localFile, "HTMLT11", FALSE);
      else
        res = TtaExportDocumentWithNewLineNumbers (doc, localFile, "HTMLTX", FALSE);
    }
  else if (strcmp (s, "SVG") == 0)
    /* docType = docSVG; */
    res = TtaExportDocumentWithNewLineNumbers (doc, localFile, "SVGT", FALSE);
  else if (strcmp (s, "MathML") == 0)
    /* docType = docMath; */
    res = TtaExportDocumentWithNewLineNumbers (doc, localFile, "MathMLT", FALSE);
  else
    /* docType = docXml; */
    res = TtaExportDocumentWithNewLineNumbers (doc, localFile, NULL, FALSE);

  // restore the previous state of the template
  TtaCloseUndoSequence (doc);
  TtaUndoNoRedo (doc);
  if (res)
    return localFile;
  else
    {
      TtaFreeMemory (localFile);
      return NULL;
    }
}

/*----------------------------------------------------------------------
  TemplateGetParentHead looks for the parent xt:head element
  ----------------------------------------------------------------------*/
Element TemplateGetParentHead (Element el, Document doc)
{
#ifdef TEMPLATES
  ElementType headType;
  SSchema     schema;

  schema = TtaGetSSchema ("Template", doc);
  if (schema == TtaGetDocumentSSchema (doc))
    return TtaGetMainRoot (doc);
  if (schema == NULL)
    // no template element in that document
    return NULL;

  headType.ElTypeNum = Template_EL_head;
  return TtaGetExactTypedAncestor (el, headType);
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  TemplateFindHead looks for the xt:head element and creates it
  if it doesn't exist.
  ----------------------------------------------------------------------*/
Element TemplateFindHead (Document doc)
{
#ifdef TEMPLATES
  ElementType headType, elType;
  Element     head, root;

  headType.ElSSchema = TtaGetSSchema ("Template", doc);
  if (headType.ElSSchema == NULL)
    return NULL;

  root = TtaGetMainRoot (doc);
  elType = TtaGetElementType (root);
  headType.ElTypeNum = Template_EL_head;
  if (elType.ElSSchema == headType.ElSSchema)
    head = root;
  else
    head = TtaSearchTypedElement (headType, SearchInTree, root);
  if (head == NULL)
    {
      // create the template head
      head = TtaNewElement (doc, headType);
      if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML"))
        {
          elType.ElTypeNum = HTML_EL_HEAD;
          root = TtaSearchTypedElement (elType, SearchInTree, root);
        }
      TtaInsertFirstChild (&head, root, doc);
      SetAttributeStringValue (head, Template_ATTR_version, Template_Current_Version);
      SetAttributeStringValue (head, Template_ATTR_templateVersion, "1.0");      
    }
  return head;
#else /* TEMPLATES */
  return NULL;
#endif /* TEMPLATES */
}


