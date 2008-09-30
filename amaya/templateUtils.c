/*
 *
 *  COPYRIGHT INRIA and W3C, 2006-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#include "templates.h"
#include "Templatename.h"
#include "templates_f.h"

#include "AHTURLTools_f.h"
#include "HTMLsave_f.h"


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
  if(sz)
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
  char         *aux;
  int           size;

  attType.AttrSSchema = TtaGetElementType(el).ElSSchema;
  attType.AttrTypeNum = att;
  attribute = TtaGetAttribute(el, attType);

  return TtaGetAttributeValue(attribute);
#else
  return NULL;
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
      if(undo)
        TtaRegisterAttributeCreate(attribute, el, doc);
    }
  TtaSetAttributeValue(attribute, value, el, doc);
  if(undo)
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
  if(sz)
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
#else
  return NULL;
#endif /* TEMPLATES */
}


/*----------------------------------------------------------------------
GetFirstEditableElement
Returns the first descendant element which is modifiable.
----------------------------------------------------------------------*/
Element GetFirstEditableElement (Element el)
{
  Element res = NULL;
  Element current = TtaGetFirstChild(el);

  while(!res && current)
  {
    res = GetFirstEditableElement(current);
    TtaNextSibling(&current);
  }

  if(!res && !TtaIsReadOnly(el))
    res = el;

  return res;
}

/*----------------------------------------------------------------------
  TemplateCanInsertFirstChild
  Test if an element can be inserted as child of another, bypassing xt.
----------------------------------------------------------------------*/
ThotBool TemplateCanInsertFirstChild(ElementType elementType, Element parent, Document document)
{
#ifdef TEMPLATES
  SSchema         templateSSchema = TtaGetSSchema ("Template", document);
  ElementType     parType;

  while(parent)
    {
      parType = TtaGetElementType(parent);
      if(parType.ElSSchema != templateSSchema)
        break;
      parent = TtaGetParent(parent);
    }
  if(!parent)
    return FALSE;
#endif /* TEMPLATES */
  return TtaCanInsertFirstChild(elementType, parent, document);
}

/*----------------------------------------------------------------------
  ValidateTemplateAttrInMenu
  Validate the status of an attribute according to xt::atribute rules.
  ----------------------------------------------------------------------*/
ThotBool ValidateTemplateAttrInMenu (NotifyAttribute * event)
{
#ifdef TEMPLATES
  Element       elem;
  Element       parent;
  ElementType   elType;
  AttributeType attrType;
  Attribute     attr;
  char*         attrName;
  char          buffer[MAX_LENGTH];
  int           sz;
  int           useAt, type;

  /* Prevent from showing attributes for template instance but not templates. */
  if(IsTemplateInstanceDocument(event->document))
    {
      /* Prevent if attribute's element is not a descendant of xt:use */
      /* Dont prevent if descendant of xt:bag. */
      parent = event->element;
      elem = GetFirstTemplateParentElement(parent);
      if(!elem)
        return TRUE;
      elType     = TtaGetElementType(elem);
      if(elType.ElTypeNum==Template_EL_bag)
        return FALSE;
      if(elType.ElTypeNum!=Template_EL_useSimple)
        return TRUE;

      /* Search for the corresponding xt:attribute element*/
      attrName = TtaGetAttributeName(event->attributeType);
      attrType.AttrSSchema = TtaGetSSchema ("Template", event->document);
      for(elem = TtaGetFirstChild(parent); elem; TtaNextSibling(&elem))
        {
          attrType.AttrTypeNum = Template_ATTR_ref_name;
          elType = TtaGetElementType(elem);
          if(elType.ElTypeNum==Template_EL_attribute &&
                  elType.ElSSchema==TtaGetSSchema ("Template", event->document))
            {
               attr = TtaGetAttribute(elem, attrType);
               if(attr)
                 {
                   sz = MAX_LENGTH;
                   TtaGiveTextAttributeValue(attr, buffer, &sz);
                   if(!strcmp(buffer, attrName))
                     {
                       /* Process the attribute filtering */
                       /* Get 'useAt' attr value. */
                       attrType.AttrTypeNum = Template_ATTR_useAt;
                       attr = TtaGetAttribute(elem, attrType);
                       if(attr)
                         useAt = TtaGetAttributeValue(attr);
                       else
                         useAt = Template_ATTR_useAt_VAL_required;
                       /* Get 'type' attr value. */
                       attrType.AttrTypeNum = Template_ATTR_type;
                       attr = TtaGetAttribute(elem, attrType);
                       if(attr)
                         type = TtaGetAttributeValue(attr);
                       else
                         type = Template_ATTR_type_VAL_string;
                       event->restr.RestrType = (RestrictionContentType)type;
                       /* If attr is prohibited, dont show it.*/
                       if(useAt==Template_ATTR_useAt_VAL_prohibited)
                           return TRUE;
                       if(useAt==Template_ATTR_useAt_VAL_required)
                         {
                           /* Force the usage of this attribute.*/
                           event->restr.RestrFlags |= attr_mandatory;
                         }

                       /* Get 'fixed' attr value. */
                       attrType.AttrTypeNum = Template_ATTR_fixed;
                       attr = TtaGetAttribute(elem, attrType);
                       if(attr)
                         {
                           sz = MAX_LENGTH;
                           TtaGiveTextAttributeValue(attr, buffer, &sz);
                           event->restr.RestrFlags |= attr_readonly;
                           event->restr.RestrDefVal = TtaStrdup(buffer);
                           return FALSE;
                         }

                       /* Get 'default' attr value.*/
                       attrType.AttrTypeNum = Template_ATTR_defaultAt;
                       attr = TtaGetAttribute(elem, attrType);
                       if(attr)
                         {
                           sz = MAX_LENGTH;
                           TtaGiveTextAttributeValue(attr, buffer, &sz);
                           event->restr.RestrDefVal = TtaStrdup(buffer);
                         }

                       /* Get 'values' attr value.*/
                       attrType.AttrTypeNum = Template_ATTR_values;
                       attr = TtaGetAttribute(elem, attrType);
                       if(attr)
                         {
                           sz = MAX_LENGTH;
                           TtaGiveTextAttributeValue(attr, buffer, &sz);
                           event->restr.RestrEnumVal = TtaStrdup(buffer);
                           event->restr.RestrFlags |= attr_enum;
                         }
                       return FALSE;
                     }
                 }
            }
        }

      return TRUE;
    }
  else
#endif /* TEMPLATES */
    return FALSE;
}

/*----------------------------------------------------------------------
 * Dump element path
  ----------------------------------------------------------------------*/
void DumpElementSubPath(Element el, char* buffer)
{
#ifdef AMAYA_DEBUG
  Element parent = TtaGetParent(el);
  if(parent==NULL)
    strcpy(buffer, TtaGetElementTypeName(TtaGetElementType(el)));
  else
    {
      DumpElementSubPath(parent, buffer);
      strcat(buffer, "/");
      strcat(buffer, TtaGetElementTypeName(TtaGetElementType(el)));
    }
#endif /* AMAYA_DEBUG */
}

/*----------------------------------------------------------------------
 * Dump element path
  ----------------------------------------------------------------------*/
void DumpElementPath(Element el)
{
#ifdef AMAYA_DEBUG
  char buffer[MAX_LENGTH];
  DumpElementSubPath(el, buffer);
  printf("%s\n", buffer);
#endif /* AMAYA_DEBUG */
}


/*----------------------------------------------------------------------
 * Dump template element
  ----------------------------------------------------------------------*/
void DumpTemplateElement(Element el, Document doc)
{
#ifdef AMAYA_DEBUG
  ElementType    elType;
  AttributeType  attType;
  Attribute      att;
  SSchema        schema = TtaGetSSchema ("Template", doc);
  char*          str;
  char           buffer[MAX_LENGTH];
  int            len;
  Language       lang;

  if(el && doc)
    {
      elType = TtaGetElementType(el);
      printf("- %p %d ", elType.ElSSchema, elType.ElTypeNum);
      printf(" %s", TtaGetSSchemaName(elType.ElSSchema));
      printf(":%s", TtaGetElementTypeName(elType));
      if(elType.ElTypeNum==1)
        {
          len = MAX_LENGTH-1;
          TtaGiveTextContent(el, (unsigned char*)buffer, &len, &lang);
          buffer[len] = EOS;
          printf(" \"%s\"", buffer);
        }

      if(elType.ElSSchema==schema)
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
                printf(" label=%s", str);
                TtaFreeMemory(str);
                str = GetAttributeStringValueFromNum(el, Template_ATTR_types, NULL);
                printf(" types=%s", str);
                TtaFreeMemory(str);
                attType.AttrSSchema = elType.ElSSchema;
                attType.AttrTypeNum = Template_ATTR_option;
                att = TtaGetAttribute (el, attType);
                if (att)
                  printf(" option");
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
                printf(" name=%s", str);
                TtaFreeMemory(str);
                str = GetAttributeStringValueFromNum(el, Template_ATTR_type, NULL);
                printf(" type=%s", str);
                TtaFreeMemory(str);
                str = GetAttributeStringValueFromNum(el, Template_ATTR_useAt, NULL);
                printf(" use=%s", str);
                TtaFreeMemory(str);
                str = GetAttributeStringValueFromNum(el, Template_ATTR_defaultAt, NULL);
                printf(" default=%s", str);
                TtaFreeMemory(str);
                str = GetAttributeStringValueFromNum(el, Template_ATTR_fixed, NULL);
                printf(" fixed=%s", str);
                TtaFreeMemory(str);
                break;
            }
        }
    }
#endif /* AMAYA_DEBUG */
}

/*----------------------------------------------------------------------
 * Dump subtree
  ----------------------------------------------------------------------*/
void DumpSubtree(Element el, Document doc, int off)
{
#ifdef AMAYA_DEBUG
  Element child = TtaGetFirstChild(el);
  int i;

  for(i=0; i<off; i++)
    printf("  ");
  DumpTemplateElement(el, doc);
  printf("\n");

  while(child)
    {
      DumpSubtree(child, doc, off+1);
      TtaNextSibling(&child);
    }
#endif /* AMAYA_DEBUG */
}

/*----------------------------------------------------------------------
 * Save an opened document to a specified path in order to open.
 * param doc Original doc to save
 * param newdoc Document where reopen it
 * param newpath URI where save the doc
  ----------------------------------------------------------------------*/
ThotBool SaveDocumentToNewDoc(Document doc, Document newdoc, char* newpath)
{
  ElementType   elType;
  Element       root;
  char         *localFile, *s;
  ThotBool      res = FALSE;

  localFile = GetLocalPath (newdoc, newpath);
  // update all links
  SetRelativeURLs (doc, newpath, NULL, FALSE, FALSE, FALSE);
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
  return res;
}

/*----------------------------------------------------------------------
 * Retrieve the xt:head element.
  ----------------------------------------------------------------------*/
Element TemplateFindHead(Document doc)
{
#ifdef TEMPLATES
  ElementType headType;
  headType.ElSSchema = TtaGetSSchema ("Template", doc);
  headType.ElTypeNum = Template_EL_head;
  return TtaSearchTypedElement(headType, SearchInTree, TtaGetMainRoot(doc));
#else /* TEMPLATES */
  return NULL;
#endif /* TEMPLATES */
}


