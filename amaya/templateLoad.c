/*
 *
 *  COPYRIGHT INRIA and W3C, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#include "templates.h"

#include "thot_sys.h"
#include "tree.h"
#include "document.h"
#include "containers.h"
#include "Elemlist.h"
#include "insertelem_f.h"


#define THOT_EXPORT extern
#include "templateDeclarations.h"

#include "templates_f.h"
#include "templateDeclarations_f.h"
#include "templateUtils_f.h"
#include "templateInstantiate_f.h"
#include "Templatebuilder_f.h"
#include "templateLoad_f.h"

#include "AHTURLTools_f.h"
#include "HTMLactions_f.h"
#include "init_f.h"
#include "Xml2thot_f.h"

#ifdef TEMPLATES
#include "Template.h"
#endif /* TEMPLATES */

/*----------------------------------------------------------------------
  Creates an Element type and stores all needed information. 
  ----------------------------------------------------------------------*/
void AddElementDeclaration (XTigerTemplate t, Element el)
{
#ifdef TEMPLATES
  char *name;

  if (!t)
    return;

  name = GetAttributeStringValueFromNum (el, Template_ATTR_name, NULL);
  if (name)
  {
    Template_DeclareNewElement (t, name);
    TtaFreeMemory (name);
  }
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Template_AddLibraryToImport
  Declare libraries to import.
  Dont redeclare them if already loaded.
  ----------------------------------------------------------------------*/
void Template_AddLibraryToImport (XTigerTemplate t, Element el)
{
#ifdef TEMPLATES
  XTigerTemplate lib = NULL;
  char* src = NULL;
  char tempfile[MAX_LENGTH], tempname[MAX_LENGTH];

  if (t)
  {
    src = GetAttributeStringValueFromNum(el, Template_ATTR_src, NULL);

#ifdef AMAYA_DEBUG  
    printf("%s requires %s\n", t->uri, src);
#endif /* AMAYA_DEBUG */

    NormalizeURL(src, TtaGetDocument(el), tempfile, tempname, NULL);
    lib = LookForXTigerLibrary(tempfile);
    HashMap_Set(t->libraries, TtaStrdup(tempfile), lib);
    TtaFreeMemory(src);
  }
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
 * Template_CheckTypesAttribute
 * Check if declarations exist in a template and predeclare them if any.
  ----------------------------------------------------------------------*/
void Template_CheckTypesAttribute (XTigerTemplate t, Element el)
{
#ifdef TEMPLATES
  char           *types;
  StringSet       set;
  ForwardIterator iter;
  StringSetNode   node;
  
  if (!t)
    return;

  types = GetAttributeStringValueFromNum (el, Template_ATTR_types, NULL);
  if (types)
  {
    set = StringSet_CreateFromString(types, " ");
    iter = StringSet_GetForwardIterator(set);
    ITERATOR_FOREACH(iter, StringSetNode, node)
      {
        if ( Template_GetDeclaration (t, (const char*)node->elem) == NULL)
          //TODO_XTIGER We must add the current namespace
          Template_DeclareNewUnknown (t, (const char*)node->elem);
      }
    TtaFreeMemory(iter);
    TtaFreeMemory (types);
    StringSet_Destroy (set);
  }
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Template_AddHeadParameters
  Add template parameter (version and templateVersion) to the template descriptor.
  ----------------------------------------------------------------------*/
void Template_AddHeadParameters(XTigerTemplate t, Element el)
{
#ifdef TEMPLATES
  if (!t)
    return;

  t->version = GetAttributeStringValueFromNum(el, Template_ATTR_version, NULL);
  t->templateVersion = GetAttributeStringValueFromNum(el, Template_ATTR_templateVersion, NULL);
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Template_ParseDeclarations
  Parse a template document to fill template declarations.
  @param t Template to parse.
  @param el Current element, NULL to begin document parsing.
  ----------------------------------------------------------------------*/
void Template_ParseDeclarations (XTigerTemplate t, Element el)
{
#ifdef TEMPLATES
  ElementType  elType;
  Element      child;
  char        *name = NULL, *include = NULL, *exclude = NULL;
  Declaration old = NULL;

  if (!t)
    return;

  if (el == NULL)
    el = TtaGetMainRoot (t->doc);

  elType = TtaGetElementType (el);	
  if (!strcmp (TtaGetSSchemaName (elType.ElSSchema),"Template"))
    {
      switch (elType.ElTypeNum)
        {
        case Template_EL_component:
          name = GetAttributeStringValueFromNum (el, Template_ATTR_name, NULL);
          if (name && name[0])
            {
              old = Template_GetDeclaration(t, name);
              if (old)
                {
                  if (old->nature == UnknownNat || old->nature == ComponentNat)
                    {
                      Template_RemoveUnknownDeclaration(t, old);
                      Template_DeclareNewComponent (t, name, el, 0);                      
                    }
                  else
                    Template_AddError(t, TtaGetMessage(AMAYA, AM_TEMPLATE_ERR_MULTICOMP), name);
                }
              else
                Template_DeclareNewComponent (t, name, el, 0);
            }
          TtaFreeMemory (name);
          break;
        case Template_EL_union:
          name    = GetAttributeStringValueFromNum (el, Template_ATTR_name, NULL);
          include   = GetAttributeStringValueFromNum (el, Template_ATTR_includeAt, NULL);
          exclude   = GetAttributeStringValueFromNum (el, Template_ATTR_exclude, NULL);
          if (name)
            Template_DeclareNewUnion (t, name, include, exclude);
          TtaFreeMemory (name);
          TtaFreeMemory (include);
          TtaFreeMemory (exclude);
          break;
        case Template_EL_bag:
          Template_CheckTypesAttribute (t, el);
          break;
        case Template_EL_useEl:
        case Template_EL_useSimple:
          Template_CheckTypesAttribute (t, el);
         /*  elType = TtaGetElementType (el); */
/*           if (elType.ElTypeNum == Template_EL_useEl) */
/*             { */
/*               if (!NeedAMenu (el, t->doc)) */
/*                 TtaChangeTypeOfElement (el, t->doc, Template_EL_useSimple); */
/*             } */
          break;
        default:
          break;
        }
    }

  child = TtaGetFirstChild (el);
  while (child)
    {
      Template_ParseDeclarations (t, child);
      TtaNextSibling (&child);
    }
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Template_PreParseDeclarations
  Parse a template document to declare import dependancies.
  @param t Template to parse.
  @param el Current element, NULL to begin document parsing.
  ----------------------------------------------------------------------*/
void Template_PreParseDeclarations (XTigerTemplate t, Element el)
{
#ifdef TEMPLATES
  ElementType elType;
  
  if (!t)
    return;

  if (el == NULL)
    el = TtaGetMainRoot(t->doc);

  elType = TtaGetElementType (el);  
  if (!strcmp (TtaGetSSchemaName (elType.ElSSchema),"Template"))
    {
      switch (elType.ElTypeNum)
        {
        case Template_EL_head:
          Template_AddHeadParameters(t,el);
          break;
        case Template_EL_import:
          Template_AddLibraryToImport (t, el);
          break;
        default:
          break;
        }
    }
  
  Element child = TtaGetFirstChild (el);
  while (child)
    {
      Template_PreParseDeclarations (t, child);
      TtaNextSibling (&child);
    }
#endif /* TEMPLATES */
}


/*----------------------------------------------------------------------
  Template_PrepareTemplate: Prepare template after loading it
  and before using it.
  ----------------------------------------------------------------------*/
void Template_PrepareTemplate (XTigerTemplate t, Document doc)
{
#ifdef TEMPLATES
  SSchema          templateSSchema;
  ForwardIterator  iter;
  HashMapNode      node;
  XTigerTemplate   templ = NULL;
  ElementType	     searchedType1, searchedType2, searchedType3;
  ElementType      elType, parentType;
  Element	         root, el, asc, prev, next, parent;
  Declaration      dec;
  char            *name;

  Template_Clear(t);
  Template_PreParseDeclarations(t, 0);
  iter = HashMap_GetForwardIterator(t->libraries);
  // Load dependancies
  ITERATOR_FOREACH(iter, HashMapNode, node)
    {
      if (!Template_LoadXTigerTemplateLibrary ((XTigerTemplate)node->elem))
        Template_AddError(t, TtaGetMessage(AMAYA, AM_TEMPLATE_ERR_BADLIB),
              ((XTigerTemplate)node->elem)->uri);
    }

  // Add standard libraries.
  Template_AddStandardDependancies(t);
  
  // Propagate dependancy elements
  ITERATOR_FOREACH(iter, HashMapNode, node)
    Template_AddLibraryDeclarations (t, (XTigerTemplate)node->elem);

  TtaFreeMemory(iter);

  if (t->base_uri)
    {
      templ = GetXTigerTemplate(t->base_uri);
      if (templ)
        Template_AddLibraryDeclarations(t, templ);
    }
  
  Template_ParseDeclarations (t, NULL);
  Template_MoveUnknownDeclarationToXmlElement (t);
  Template_FillDeclarations (t);
  Template_PreInstantiateComponents (t);
  Template_CalcBlockLevel (t);

  // check the block level of each use element
  if (DocumentTypes[doc] != docHTML)
    return;
  root = TtaGetRootElement (doc);
  elType = TtaGetElementType (root);
  elType.ElTypeNum = HTML_EL_Pseudo_paragraph;
  templateSSchema = TtaGetSSchema ("Template", doc);
  searchedType1.ElSSchema = templateSSchema;
  searchedType1.ElTypeNum = Template_EL_useEl;
  searchedType2.ElSSchema = templateSSchema;
  searchedType2.ElTypeNum = Template_EL_useSimple;
  searchedType3.ElSSchema = templateSSchema;
  searchedType3.ElTypeNum = Template_EL_bag;
  el = root;
  while (el)
    {
      el = TtaSearchElementAmong5Types (searchedType1, searchedType2,
                                        searchedType3, searchedType1,
                                        searchedType1, SearchForward, el);
      if (el)
        {
          //look at the refered type
          name = GetUsedTypeName (el);
          dec = Template_GetDeclaration (t, name);
          if (dec && dec->blockLevel == 2)
            {
             asc = TtaGetTypedAncestor (el, elType);
             if (asc)
               {
                 // chech if the parent is a repeat
#ifdef TEMPLATE_DEBUG
printf ("Move %s out of the pseudo paragraph\n",name);
#endif /* TEMPLATE_DEBUG */
                 parent = TtaGetParent (el);
                 parentType = TtaGetElementType (parent);
                 if (parentType.ElSSchema == templateSSchema &&
                     parentType.ElTypeNum == Template_EL_repeat)
                   {
                     el = parent;
                     parent = TtaGetParent (el);
                   }
                 prev = el;
                 TtaPreviousSibling (&prev);
                 while (el)
                   {
                     // move the element and next siblings after the pseudo paragraph
                     next = el;
                     TtaNextSibling (&next);
                     TtaRemoveTree (el, doc);
                     TtaInsertSibling (el, asc, FALSE, doc);
                     el = next;
                   }
                 // next origin of the search
                 if (prev)
                   el = prev;
                 else
                   el = parent;
               }
            }
          TtaFreeMemory (name);
        }
    }
#endif /* TEMPLATES */
}

#ifdef TEMPLATES
static ThotBool TemplateIsLoaded = FALSE;
/*----------------------------------------------------------------------
  LoadTemplate_callback: Called after loading a template.
  ----------------------------------------------------------------------*/
static void LoadTemplate_callback (int newdoc, int status,  char *urlName,
                            char *outputfile, char* proxyName,
                            AHTHeaders *http_headers, void * context)
{
  TemplateIsLoaded = TRUE;
}
#endif /* TEMPLATES */

/*----------------------------------------------------------------------
  LoadTemplate loads the template document and returns its type.
  If template is already loaded, returns its type.
  Return docFree itf the template cannot be loaded.
  ----------------------------------------------------------------------*/
DocumentType LoadTemplate (Document doc, char* templatename)
{
#ifdef TEMPLATES
  Document         newdoc = 0;
  DocumentType     docType = docFree;
  char            *s, *directory;
  unsigned int     size = strlen (templatename) + 1;
  XTigerTemplate   t = NULL;

  if (!IsW3Path (templatename))
    {
      //Stores the template path and shows it in next instanciation forms
      directory	= (char*) TtaGetMemory (size);
      s	= (char*) TtaGetMemory (size);
      TtaExtractName (templatename, directory, s);	
      TtaSetEnvString ("TEMPLATES_DIRECTORY", directory, TRUE);
      TtaFreeMemory (directory);
      TtaFreeMemory (s);
    }

  //If types are not loaded we load the template and we parse it
  t = GetXTigerTemplate (templatename);
  if (t == NULL || t->doc == 0 ||
      DocumentURLs[t->doc] == NULL ||
      strcmp (templatename, DocumentURLs[t->doc]))
    {
      // The template is not loaded, load it !

      // the current loading document changes and should be restored
      TemplateIsLoaded = FALSE;
      //Load the document
      newdoc = GetAmayaDoc (templatename, NULL, 0, 0, CE_TEMPLATE, FALSE, 
                            (void (*)(int, int, char*, char*, char*,
                                      const AHTHeaders*, void*)) LoadTemplate_callback,
                            (void *) /*ctx*/ NULL);
      
      while (!TemplateIsLoaded)
        TtaHandlePendingEvents ();

      t = GetXTigerDocTemplate(newdoc);
      if (t)
        {
          if (t->uri == NULL)
            t->uri = TtaStrdup (templatename);
#ifdef TEMPLATE_DEBUG  
    printf("XTiger template %s loaded.\n", t->uri);
#endif /* TEMPLATE_DEBUG */
          if (Template_HasErrors(t))
            Template_ShowErrors(t);
          else
            docType = DocumentTypes[newdoc];
          Template_AddReference (t);
        }
     }
  else
    // The template is already loaded, use it.
    docType = DocumentTypes[t->doc];

  if (t)
    {
      // register the reference to the template document
      TtaAddDocumentReference(t->doc);
      return docType;
    }
#endif /* TEMPLATES */
  return docFree;
}


#ifdef TEMPLATES
static ThotBool LibraryIsLoaded = FALSE;
/*----------------------------------------------------------------------
  LoadTemplate_callback: Called after loading a template.
  ----------------------------------------------------------------------*/
static void LoadLibrary_callback (int newdoc, int status,  char *urlName,
                            char *outputfile, char* proxyName,
                            AHTHeaders *http_headers, void * context)
{
  LibraryIsLoaded = TRUE;
}
#endif /* TEMPLATES */

/*----------------------------------------------------------------------
  Template_LoadXTigerTemplateLibrary
  Load a library with all its dependancies.
  @param t Template of preimported library.
  ----------------------------------------------------------------------*/
ThotBool Template_LoadXTigerTemplateLibrary (XTigerTemplate t)
{
  ThotBool         result = FALSE;
#ifdef TEMPLATES
  int              docLoading;
  Document         newdoc = 0;
  
  if (t && !Template_IsLoaded(t))
    {
      docLoading = W3Loading;
      W3Loading = 0;
      LibraryIsLoaded = FALSE;
      // Load the document (look at LoadTemplate)
      newdoc = GetAmayaDoc (t->uri, NULL, 0, 0, CE_TEMPLATE, FALSE, 
                            (void (*)(int, int, char*, char*, char*,
                                      const AHTHeaders*, void*)) LoadLibrary_callback,
                            (void *) NULL);
      
      while (!LibraryIsLoaded)
        TtaHandlePendingEvents ();
      W3Loading = docLoading;      
  }

  if (t)
    {
      Template_AddReference (t);
      result = !Template_HasErrors (t);

#ifdef TEMPLATE_DEBUG 
      if (result)
        printf("XTiger library %s loaded successfully.\n", t->uri);
      else
        printf("XTiger library %s has error(s)\n", t->uri);
#endif /* TEMPLATE_DEBUG */
    }
#endif /* TEMPLATES */
  return result;
}
