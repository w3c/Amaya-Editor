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
#include "templateLoad_f.h"

#include "AHTURLTools_f.h"
#include "HTMLactions_f.h"
#include "init_f.h"
#include "Xml2thot_f.h"

#ifdef TEMPLATES
#include "Template.h"
/* Information needed for the callback after loading a template.
   Just the path of the template, which identifies it. */
typedef struct _TemplateCtxt
{
  char           *templatePath;
  Document        newdoc;     // the template document entry
  XTigerTemplate  t;          // the template descriptor
  int             docLoading; // get in memory the loading document
  ThotBool        isloaded;   // true when the template is loaded
} TemplateCtxt;
#endif

/*----------------------------------------------------------------------
  Creates an Element type and stores all needed information. 
  ----------------------------------------------------------------------*/
void AddElementDeclaration (XTigerTemplate t, Element el)
{
#ifdef TEMPLATES
  char *name;

  if(!t)
    return;

  name = GetAttributeStringValueFromNum (el, Template_ATTR_name, NULL);
  if(name)
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

  if(t)
  {
    src = GetAttributeStringValueFromNum(el, Template_ATTR_src, NULL);

#ifdef AMAYA_DEBUG  
    printf("%s requires %s\n", t->name, src);
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
  char *types;
  HashMap map;
  ForwardIterator iter;
  HashMapNode node;
  
  if(!t)
    return;

  types = GetAttributeStringValueFromNum (el, Template_ATTR_types, NULL);
  if(types)
  {
    map = KeywordHashMap_CreateFromList (NULL, -1, types);
    iter = HashMap_GetForwardIterator(map);
    ITERATOR_FOREACH(iter, HashMapNode, node)
      {
        if ( Template_GetDeclaration (t, (const char*)node->key) == NULL)
          //TODO_XTIGER We must add the current namespace
          Template_DeclareNewElement (t, (const char*)node->key);
      }
    TtaFreeMemory(iter);

    TtaFreeMemory (types);
    HashMap_Destroy (map);
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
  if(!t)
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
  ElementType  type;
  Element      child;
  char        *name = NULL, *include = NULL, *exclude = NULL;
  Declaration old = NULL;

  if (!t)
    return;

  if (el == NULL)
    el = TtaGetMainRoot (t->doc);

  type = TtaGetElementType (el);	
  if (!strcmp (TtaGetSSchemaName (type.ElSSchema),"Template"))
    {
      switch (type.ElTypeNum)
        {
        case Template_EL_component:
          name = GetAttributeStringValueFromNum (el, Template_ATTR_name, NULL);
          if(name && name[0])
            {
              old = Template_GetDeclaration(t, name);
              if(old)
                {
                  if(old->nature==UnknownNat)
                    {
                      Template_RemoveUnknownDeclaration(t, old);
                      Template_DeclareNewComponent (t, name, el);                      
                    }
                  else
                    {
                      Template_AddError(t, TtaGetMessage(AMAYA, AM_TEMPLATE_ERR_MULTICOMP), name);
                    }
                }
              else
                Template_DeclareNewComponent (t, name, el);
            }
          TtaFreeMemory (name);
          break;
        case Template_EL_union:
          name    = GetAttributeStringValueFromNum (el, Template_ATTR_name, NULL);
          include   = GetAttributeStringValueFromNum (el, Template_ATTR_includeAt, NULL);
          exclude   = GetAttributeStringValueFromNum (el, Template_ATTR_exclude, NULL);
          
          if(name)
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
  ElementType type;
  
  if(!t)
    return;

  if(el == NULL)
    el = TtaGetMainRoot(t->doc);

  type = TtaGetElementType (el);  
  if (!strcmp (TtaGetSSchemaName (type.ElSSchema),"Template"))
    {
      switch (type.ElTypeNum)
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
void Template_PrepareTemplate(void* templ)
{
#ifdef TEMPLATES
  XTigerTemplate   t = (XTigerTemplate)templ;
  ForwardIterator  iter;
  HashMapNode      node;
  
  Template_PreParseDeclarations(t, 0);

  iter = HashMap_GetForwardIterator(t->libraries);
  // Load dependancies
  ITERATOR_FOREACH(iter, HashMapNode, node)
    {
      if(!Template_LoadXTigerTemplateLibrary ((XTigerTemplate)node->elem))
        Template_AddError(t, TtaGetMessage(AMAYA, AM_TEMPLATE_ERR_BADLIB),
              ((XTigerTemplate)node->elem)->name);
    }

  // Add standard libraries.
  Template_AddStandardDependancies(t);
  
  // Propagate dependancy elements
  ITERATOR_FOREACH(iter, HashMapNode, node)
    Template_AddLibraryDeclarations (t, (XTigerTemplate)node->elem);

  TtaFreeMemory(iter);

  Template_ParseDeclarations  (t, NULL);
  Template_MoveUnknownDeclarationToXmlElement(t);
  Template_FillDeclarations (t);
  
  Template_PreInstantiateComponents (t);
  Template_CalcBlockLevel (t);
#endif /* TEMPLATES */
}


/*----------------------------------------------------------------------
  LoadTemplate_callback: Called after loading a template.
  ----------------------------------------------------------------------*/
#ifdef TEMPLATES
static void LoadTemplate_callback (int newdoc, int status,  char *urlName,
                            char *outputfile, char* proxyName,
                            AHTHeaders *http_headers, void * context)
{
  TemplateCtxt *ctx = (TemplateCtxt*)context;
  
  if (newdoc)
    {
      // the template is now loaded
      if(!ctx->t)
        ctx->t = NewXTigerTemplate (ctx->templatePath);
      SetTemplateDocument (ctx->t, newdoc);
  
      ctx->isloaded = TRUE;
      ctx->newdoc   = newdoc;
    }
  ctx->isloaded = TRUE;
      // restore the loading document
  W3Loading = ctx->docLoading;
}
#endif /* TEMPLATES */


/*----------------------------------------------------------------------
  LoadTemplate loads the template document and returns its type.
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
  ForwardIterator  iter;
  HashMapNode      node;

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
  if (!GetXTigerTemplate(templatename))
    {
      //Load the document
      TemplateCtxt *ctx	= (TemplateCtxt *)TtaGetMemory (sizeof (TemplateCtxt));
      ctx->templatePath	= TtaStrdup (templatename);
      ctx->isloaded = FALSE;
      ctx->t = NULL;
      // the current loading document changes and should be restored
      ctx->docLoading = W3Loading;
      W3Loading = 0;
      newdoc = GetAmayaDoc (templatename, NULL, 0, 0, CE_TEMPLATE, FALSE, 
                            (void (*)(int, int, char*, char*, char*, const AHTHeaders*, void*)) LoadTemplate_callback,
                            (void *) ctx);
      while (!ctx->isloaded)
        TtaHandlePendingEvents ();
      t = ctx->t;

      if (t)
        {
          Template_PrepareTemplate(t);
          ctx->t->isLoaded = TRUE;
#ifdef AMAYA_DEBUG  
    printf("XTiger template %s loaded.\n", t->name);
#endif /* AMAYA_DEBUG */
          if(Template_HasErrors(t))
            Template_ShowErrors(t);
          else
            {
              DoInstanceTemplate (ctx->templatePath);
              // get the real docType of the template
              docType = DocumentTypes[ctx->newdoc];
              DocumentTypes[ctx->newdoc] = docTemplate;
              TtaSetDocumentUnmodified (ctx->newdoc);
              UpdateTemplateMenus(doc);
            }
        }
      TtaFreeMemory(ctx->templatePath);
      TtaFreeMemory(ctx);
    }

#ifdef AMAYA_DEBUG  
  DumpAllDeclarations();
#endif /* AMAYA_DEBUG */
  return docType;
#else /* TEMPLATES */
  return docFree;
#endif /* TEMPLATES */
}


/*----------------------------------------------------------------------
  Template_LoadXTigerTemplateLibrary
  Load a library with all its dependancies.
  @param t Template of preimported library.
  ----------------------------------------------------------------------*/
ThotBool Template_LoadXTigerTemplateLibrary (XTigerTemplate t)
{
#ifdef TEMPLATES
  ForwardIterator iter;
  HashMapNode     node;
  Document        newdoc = 0;
  
  if (t && !t->isLoaded)
  {
    // Load the document (look at LoadTemplate)
    TemplateCtxt *ctx = (TemplateCtxt *)TtaGetMemory (sizeof (TemplateCtxt));
    ctx->templatePath = TtaStrdup (t->name);
    ctx->isloaded = FALSE;
    ctx->t = t;
    newdoc = GetAmayaDoc (t->name, NULL, 0, 0, CE_TEMPLATE, FALSE, 
                          (void (*)(int, int, char*, char*, char*,
                                    const AHTHeaders*, void*)) LoadTemplate_callback,
                          (void *) ctx);
    while (!ctx->isloaded)
      TtaHandlePendingEvents ();

    Template_PrepareTemplate(t);
    
    DocumentTypes[ctx->newdoc] = docTemplate;
    t->isLoaded = TRUE;

    
#ifdef AMAYA_DEBUG  
    if(Template_HasErrors(t))
      printf("XTiger library %s has error(s)\n", t->name);
    else
      printf("XTiger library %s loaded successfully.\n", t->name);
#endif /* AMAYA_DEBUG */

    TtaFreeMemory(ctx->templatePath);
    TtaFreeMemory(ctx);

    return !Template_HasErrors(t);
  }
#endif /* TEMPLATES */
  return FALSE;
}
