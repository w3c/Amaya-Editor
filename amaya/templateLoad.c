#include "templates.h"

#define THOT_EXPORT extern
#include "templateDeclarations.h"

#include "mydictionary_f.h"
#include "templateDeclarations_f.h"
#include "templateUtils_f.h"
#include "templateInstantiate_f.h"

#include "HTMLactions_f.h"
#include "init_f.h"

#ifdef TEMPLATES
#include "Template.h"
/* Information needed for the callback after loading a template.
   Just the path of the template, which identifies it. */
typedef struct _TemplateCtxt
{
	char			*templatePath;
} TemplateCtxt;
#endif

/*----------------------------------------------------------------------
  Creates an Element type and stores all needed information. 
  ----------------------------------------------------------------------*/
void AddElementDeclaration (XTigerTemplate t, Element el)
{
#ifdef TEMPLATES
	char *name;

	name = GetAttributeStringValue (el, Template_ATTR_name);
	NewElement (t, name);
	if (name)
    TtaFreeMemory (name);
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Creates a Union type and stores all needed information. 
  ----------------------------------------------------------------------*/
void AddUnionDeclaration (XTigerTemplate t, Element el)
{
#ifdef TEMPLATES
	char *name, *include, *exclude;
	
	name	  = GetAttributeStringValue (el, Template_ATTR_name);
	include   = GetAttributeStringValue (el, Template_ATTR_includeAt);
	exclude   = GetAttributeStringValue (el, Template_ATTR_exclude);
	NewUnion (t, name, 
           CreateDictionaryFromList (include), 
           CreateDictionaryFromList (exclude));
	
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
	
	name = GetAttributeStringValue (el, Template_ATTR_name);
	NewComponent (t, name, el);	
	TtaFreeMemory (name);
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Load (if needed) a library and adds all its declarations to a template
  ----------------------------------------------------------------------*/
void AddImportedLibrary (XTigerTemplate t, Element el)
{
#ifdef TEMPLATES
#ifdef TODO_XTIGER
	XTigerTemplate lib = NULL;
	//Load the library
	AddLibraryDeclarations (t,lib);
#endif
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void CheckTypesAttribute (XTigerTemplate t, Element el)
{
#ifdef TEMPLATES
	char *types;
	DicDictionary dic;

	types = GetAttributeStringValue (el, Template_ATTR_types);
	dic = CreateDictionaryFromList (types);
	
	for (First (dic); !IsDone (dic); Next (dic))
		if ( GetDeclaration (t, CurrentKey (dic)) == NULL)
			//TODO_XTIGER We must add the current namespace
			NewElement (t, CurrentKey (dic));
	
	TtaFreeMemory (types);
	CleanDictionary (dic);
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void ParseDeclarations (XTigerTemplate t, Element el)
{
#ifdef TEMPLATES	
	ElementType type = TtaGetElementType (el);
	
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
        case Template_EL_import:
          AddImportedLibrary (t, el);
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
      ParseDeclarations (t, child);
      TtaNextSibling (&child);
    }
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  LoadTemplate_callback: Called after loading a template.
  ----------------------------------------------------------------------*/
void LoadTemplate_callback (int newdoc, int status,  char *urlName,
                            char *outputfile, AHTHeaders *http_headers,
                            void * context)
{	
#ifdef TEMPLATES 
#ifdef AMAYA_DEBUG 
	char localname[MAX_LENGTH];
	FILE *file;
#endif /* AMAYA_DEBUG */
  Element el;
	TemplateCtxt *ctx = (TemplateCtxt*)context;
	
	XTigerTemplate t = NewXTigerTemplate (ctx->templatePath, TRUE);
  SetTemplateDocument (t, newdoc);
	el = TtaGetMainRoot (newdoc);
	ParseDeclarations  (t, el);
	RedefineSpecialUnions (t);
	PreInstantiateComponents (t);
  
#ifdef AMAYA_DEBUG	
	DumpDeclarations (t);
  strcpy (localname, TempFileDirectory);
  strcat (localname, DIR_STR);
  strcat (localname, "template.debug");
  file = TtaWriteOpen (localname);
  
	TtaListAbstractTree (TtaGetMainRoot (newdoc), file);
	TtaWriteClose (file);
#endif

  DoInstanceTemplate (ctx->templatePath);
  TtaFreeMemory (ctx->templatePath);
  TtaFreeMemory (ctx);
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void LoadTemplate (Document doc, char* templatename)
{
#ifdef TEMPLATES
  Document newdoc;
	char			*directory, *document;
	unsigned int	size = strlen (templatename) + 1;
	
	//Stores the template path for show it in next instanciation forms
	directory	= (char*) TtaGetMemory (size);
	document	= (char*) TtaGetMemory (size);

	TtaExtractName (templatename, directory, document);	
	TtaSetEnvString ("TEMPLATES_DIRECTORY", directory, TRUE);

	TtaFreeMemory (directory);
	TtaFreeMemory (document);

	//If types are not loaded we load the template and we parse it
	if (!Get (Templates_Dic, templatename))
    {	
      //Creation of the callback context
      TemplateCtxt *ctx	= (TemplateCtxt *)TtaGetMemory (sizeof (TemplateCtxt));
      ctx->templatePath	= TtaStrdup (templatename);

      DontReplaceOldDoc = TRUE;
      newdoc = GetAmayaDoc (templatename, NULL, doc, doc, CE_TEMPLATE, FALSE, 
                            (void (*)(int, int, char*, char*, char*, const AHTHeaders*, void*)) LoadTemplate_callback,
                            (void *) ctx);
      if (newdoc)
        DocumentTypes[newdoc] = docTemplate;
    }
#endif /* TEMPLATES */
}
