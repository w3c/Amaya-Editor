#include "templates.h"

#include "thot_sys.h"
#include "tree.h"
#include "document.h"
#include "containers.h"
#include "insertelem_f.h"


#define THOT_EXPORT extern
#include "templateDeclarations.h"


#include "mydictionary_f.h"
#include "templateDeclarations_f.h"
#include "templateUtils_f.h"
#include "templateInstantiate_f.h"
#include "templateLoad_f.h"

#include "AHTURLTools_f.h"
#include "HTMLactions_f.h"
#include "init_f.h"

#ifdef TEMPLATES
#include "Template.h"
/* Information needed for the callback after loading a template.
   Just the path of the template, which identifies it. */
typedef struct _TemplateCtxt
{
	char			      *templatePath;
  ThotBool        isloaded;
  Document        newdoc;
  XTigerTemplate  t;
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
    NewElement (t, name);
    TtaFreeMemory (name);
  }
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Creates a Union type and stores all needed information. 
  ----------------------------------------------------------------------*/
void AddUnionDeclaration (XTigerTemplate t, Element el)
{
#ifdef TEMPLATES
	char *name, *include, *exclude;
  
  if(!t)
    return;
	
	name	  = GetAttributeStringValueFromNum (el, Template_ATTR_name, NULL);
	include   = GetAttributeStringValueFromNum (el, Template_ATTR_includeAt, NULL);
	exclude   = GetAttributeStringValueFromNum (el, Template_ATTR_exclude, NULL);
  if(name && include)
  {
  	NewUnion (t, name, 
             Dictionary_CreateFromList (include), 
             Dictionary_CreateFromList (exclude));
  }
  TtaFreeMemory (name);
	TtaFreeMemory (include);
	TtaFreeMemory (exclude);
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Creates a Component type and stores all needed information. 
  ----------------------------------------------------------------------*/
void AddComponentDeclaration (XTigerTemplate t, Element el)
{
#ifdef TEMPLATES
	char *name;
  
  if(!t)
    return;
	
	name = GetAttributeStringValueFromNum (el, Template_ATTR_name, NULL);
  if(name)
  	NewComponent (t, name, el);	
	TtaFreeMemory (name);
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
    Dictionary_Add(t->libraries, tempfile, lib);

    TtaFreeMemory(src);
  }
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void CheckTypesAttribute (XTigerTemplate t, Element el)
{
#ifdef TEMPLATES
	char *types;
	DicDictionary dic;
  
  if(!t)
    return;

	types = GetAttributeStringValueFromNum (el, Template_ATTR_types, NULL);
  if(types)
  {
  	dic = Dictionary_CreateFromList (types);
  	
  	for (Dictionary_First (dic); !Dictionary_IsDone (dic); Dictionary_Next (dic))
  		if ( Template_GetDeclaration (t, Dictionary_CurrentKey (dic)) == NULL)
  			//TODO_XTIGER We must add the current namespace
  			NewElement (t, Dictionary_CurrentKey (dic));
  	
  	TtaFreeMemory (types);
  	Dictionary_Clean (dic);
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
	ElementType type;
  
  if(!t)
    return;

  if(el==NULL)
    el = TtaGetMainRoot(t->doc);

  type = TtaGetElementType (el);	
	if (!strcmp (TtaGetSSchemaName (type.ElSSchema),"Template"))
    {
      switch (type.ElTypeNum)
        {
        case Template_EL_component:
          AddComponentDeclaration (t,el);
          break;
        case Template_EL_union:
          AddUnionDeclaration (t,el);
          break;
        case Template_EL_bag:
          CheckTypesAttribute (t, el);
          break;
        case Template_EL_useEl:
        case Template_EL_useSimple:
          CheckTypesAttribute (t, el);
          break;
        default:
          break;
        }
    }
	
	Element child = TtaGetFirstChild (el);
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

  if(el==NULL)
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
        ctx->t = NewXTigerTemplate (ctx->templatePath, TRUE);
      SetTemplateDocument(ctx->t, newdoc);
  
      ctx->isloaded = TRUE;
      ctx->newdoc   = newdoc;
    }
  ctx->isloaded = TRUE;
}
#endif /* TEMPLATES */


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void LoadTemplate (Document doc, char* templatename)
{
#ifdef TEMPLATES
  Document      newdoc = 0;
	char			   *s, *directory;
	unsigned int	size = strlen (templatename) + 1;
  XTigerTemplate t = NULL;
  Record rec;

  if (!IsW3Path (templatename))
    {
      //Stores the template path for show it in next instanciation forms
      directory	= (char*) TtaGetMemory (size);
      s	= (char*) TtaGetMemory (size);
      TtaExtractName (templatename, directory, s);	
      TtaSetEnvString ("TEMPLATES_DIRECTORY", directory, TRUE);
      TtaFreeMemory (directory);
      TtaFreeMemory (s);
    }

	//If types are not loaded we load the template and we parse it
	if (!Dictionary_Get (Templates_Dic, templatename))
    {	
      //Load the document
      TemplateCtxt *ctx	= (TemplateCtxt *)TtaGetMemory (sizeof (TemplateCtxt));
      ctx->templatePath	= TtaStrdup (templatename);
      ctx->isloaded = FALSE;
      ctx->t = NULL;
      newdoc = GetAmayaDoc (templatename, NULL, 0, 0, CE_TEMPLATE, FALSE, 
                            (void (*)(int, int, char*, char*, char*, const AHTHeaders*, void*)) LoadTemplate_callback,
                            (void *) ctx);
      while (!ctx->isloaded)
        TtaHandlePendingEvents ();
      t = ctx->t;

      Template_PreParseDeclarations(t, 0);

      // Load dependancies
      for(rec=t->libraries->first; rec; rec=rec->next)
      {
        Template_LoadXTigerTemplateLibrary((XTigerTemplate)rec->element);
        AddLibraryDeclarations(t, (XTigerTemplate)rec->element);
      }

      Template_ParseDeclarations  (t, 0);
      PreInstantiateComponents (t);

      ctx->t->isLoaded = TRUE;

      DoInstanceTemplate (TtaStrdup(ctx->templatePath));
      DocumentTypes[ctx->newdoc] = docTemplate;
      
      TtaFreeMemory(ctx->templatePath);
      TtaFreeMemory(ctx);
    }

#ifdef AMAYA_DEBUG  
  DumpAllDeclarations();
#endif /* AMAYA_DEBUG */
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Template_LoadPreImportedLibrary
  Load a library with all its dependancies.
  @param t Template of preimported library.
  ----------------------------------------------------------------------*/
void Template_LoadXTigerTemplateLibrary(XTigerTemplate t)
{
#ifdef TEMPLATES
  Record rec;
  Document newdoc = 0;
  
  if(t && !t->isLoaded)
  {
    // Load the document (look at LoadTemplate)
    TemplateCtxt *ctx = (TemplateCtxt *)TtaGetMemory (sizeof (TemplateCtxt));
    ctx->templatePath = TtaStrdup (t->name);
    ctx->isloaded = FALSE;
    ctx->t = t;
    newdoc = GetAmayaDoc (t->name, NULL, 0, 0, CE_TEMPLATE, FALSE, 
                          (void (*)(int, int, char*, char*, char*, const AHTHeaders*, void*)) LoadTemplate_callback,
                          (void *) ctx);
    while (!ctx->isloaded)
      TtaHandlePendingEvents ();

    Template_PreParseDeclarations(t, 0);

    // Load dependancies
    for(rec=t->libraries->first; rec; rec=rec->next)
    {
      Template_LoadXTigerTemplateLibrary((XTigerTemplate)rec->element);
      AddLibraryDeclarations(t, (XTigerTemplate)rec->element);
    }

    Template_ParseDeclarations  (t, 0);
    PreInstantiateComponents (t);
    
    t->isLoaded = TRUE;
#ifdef AMAYA_DEBUG  
    printf("XTiger library %s loaded.\n", t->name);
#endif /* AMAYA_DEBUG */

    TtaFreeMemory(ctx->templatePath);
    TtaFreeMemory(ctx);

  }  
#endif /* TEMPLATES */
}
