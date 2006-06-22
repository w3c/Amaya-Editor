#include "templateLoad.h"

#include "templateDeclarations.h"
#include "templateUtils.h"
#include "templateInstanciation.h"

#include "HTMLactions_f.h"
#include "init_f.h"

#ifdef TEMPLATES

#include "Template.h"

/* Information needed for the callback after loading a template.
Just the path of the template, which identifies it. */
typedef struct _TemplateCtxt
{
	char			*templatePath;
	char			*instancePath;
	DocumentType    docType;
	ThotBool		dontReplace;
	ThotBool		createInstance;
} TemplateCtxt;

#endif

/*----------------------------------------------------------------------
Creates an Element type and stores all needed information. 
----------------------------------------------------------------------*/
void AddElementDeclaration (XTigerTemplate t, Element el)
{
#ifdef TEMPLATES
	char *name;
	Declaration dec;
	
	name	  = GetAttributeStringValue (el, Template_ATTR_name);
	
	dec  = NewElement(t, name);
	
	RemoveOldDeclarations(t, name);
	Add(t->elements, name, dec);
	
	if (name)    TtaFreeMemory (name);
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
Creates a Union type and stores all needed information. 
----------------------------------------------------------------------*/
void AddUnionDeclaration (XTigerTemplate t, Element el)
{
#ifdef TEMPLATES
	char *name, *include, *exclude;
	Declaration dec;
	
	name	  = GetAttributeStringValue (el, Template_ATTR_name);
	include   = GetAttributeStringValue (el, Template_ATTR_includeAt);
	exclude   = GetAttributeStringValue (el, Template_ATTR_exclude);
	
	dec  = NewUnion(t, name, 
		CreateDictionaryFromList(include), 
		CreateDictionaryFromList(exclude));
	
	RemoveOldDeclarations(t, name);
	Add(t->unions, name, dec);
	
	if (name)    TtaFreeMemory (name);
	if (include) TtaFreeMemory (include);
	if (exclude) TtaFreeMemory (exclude);
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
Creates a Component type and stores all needed information. 
----------------------------------------------------------------------*/
void AddComponentDeclaration (XTigerTemplate t, Element el)
{
#ifdef TEMPLATES
	char *name;
	Declaration dec;
	
	name = GetAttributeStringValue (el, Template_ATTR_name);
	dec = NewComponent(t, name, el);
	
	RemoveOldDeclarations(t, name);
	Add(t->components, name, dec);
	
	if (name)    TtaFreeMemory (name);
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
	Declaration dec;
	
	types = GetAttributeStringValue (el, Template_ATTR_types);
	dic   = CreateDictionaryFromList(types);
	
	for (First(dic); !IsDone(dic); Next(dic))
	{
		if ( GetDeclaration(t, CurrentKey(dic)) == NULL)
		{
			//TODO_XTIGER We must add the current namespace
			dec = NewElement(t, CurrentKey(dic));
			Add(t->elements, CurrentKey(dic), dec);
		}
	}
	
	TtaFreeMemory(types);
	CleanDictionary(dic);
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
----------------------------------------------------------------------*/
void ParseDeclarations (XTigerTemplate t, Element el)
{
#ifdef TEMPLATES	
	ElementType type = TtaGetElementType(el);
	
	if(strcmp(TtaGetSSchemaName(type.ElSSchema),"Template")==0)
    {
		switch(type.ElTypeNum)
        {
        case Template_EL_component :
			AddComponentDeclaration(t,el);
			break;
        case Template_EL_union :
			AddUnionDeclaration(t,el);
			break;
        case Template_EL_import :
			AddImportedLibrary(t, el);
			break;
		case Template_EL_bag :
			CheckTypesAttribute(t, el);
			break;
		case Template_EL_useEl :
			CheckTypesAttribute(t, el);
			break;
        default :
			break;
        }
    }
	
	Element child = TtaGetFirstChild(el);
	while(child!=NULL) {
		ParseDeclarations(t, child);
		TtaNextSibling(&child);
	}
#endif /* TEMPLATES */
}

void PrintUnion (Declaration dec, int indent, XTigerTemplate t, FILE *file)
{	
#ifdef TEMPLATES
	DicDictionary  dic;
	Declaration    aux;
	char*		   indentation;
	int 		   i=0;
	
	indentation = (char*) TtaGetMemory(indent*sizeof(char)+1);
	for(i=0;i<indent;i++)
		indentation [i] = '\t';
	indentation [indent] = '\0';
	
	
	dic = dec->unionType.include;
	if(!IsEmpty(dic))
	{
		fprintf(file, "\n%sINCLUDE",indentation);
		
		for(First(dic);!IsDone(dic);Next(dic))
		{
			aux = (Declaration) CurrentElement(dic);
			switch(aux->nature)
			{
			case COMPONENT :			
			case SIMPLE_TYPE :
			case XMLELEMENT :
				fprintf(file, "\n%s+ %s ",indentation,aux->name);
				if(aux->declaredIn!=t)
					fprintf(file, "*");
				break;
			case UNION :
				fprintf(file, "\n%s+ %s ",indentation,aux->name);
				if(aux->declaredIn!=t)
					fprintf(file, "*");
				PrintUnion(aux, indent+1, t, file);
			default :
				//impossible
				break;
			}
		}
	}
	
	dic = dec->unionType.exclude;
	if(!IsEmpty(dic))
	{
		fprintf(file, "\n%sEXCLUDE",indentation);
		
		for(First(dic);!IsDone(dic);Next(dic))
		{
			aux = (Declaration) CurrentElement(dic);
			switch(aux->nature)
			{
			case COMPONENT :			
			case SIMPLE_TYPE :
			case XMLELEMENT :
				fprintf(file, "\n%s- %s ",indentation,aux->name);
				if(aux->declaredIn!=t)
					fprintf(file, "*");
				break;
			case UNION :
				fprintf(file, "\n%s- %s ",indentation,aux->name);
				if(aux->declaredIn!=t)
					fprintf(file, "*");
				PrintUnion(aux, indent+1, t, file);
			default :
				//impossible
				break;
			}
		}
	}
	
	TtaFreeMemory(indentation);
#endif /* TEMPLATES */
}

void PrintDeclarations (XTigerTemplate t, FILE *file)
{
#ifdef TEMPLATES
	DicDictionary  aux;
	Declaration    dec;
	
	fprintf(file, "SIMPLE TYPES\n");
	fprintf(file, "------------");
	aux = t->simpleTypes;
	for(First(aux);!IsDone(aux);Next(aux))
	{
		dec = (Declaration) CurrentElement(aux);
		fprintf(file, "\n%s ",dec->name);
		if(dec->declaredIn!=t)
			fprintf(file, "*");
	}
	
	aux = t->elements;
	if(!IsEmpty(aux))
	{
		fprintf(file, "\n\nXML ELEMENTS\n");
		fprintf(file, "------------");	
		for(First(aux);!IsDone(aux);Next(aux))
		{
			dec = (Declaration) CurrentElement(aux);
			fprintf(file,"\n%s ",dec->name);
			if(dec->declaredIn!=t)
				fprintf(file, "*");
		}
	}
	
	aux = t->components;
	if(!IsEmpty(aux))
	{
		fprintf(file, "\n\nCOMPONENTS\n");
		fprintf(file, "----------");	
		for(First(aux);!IsDone(aux);Next(aux))
		{
			dec = (Declaration) CurrentElement(aux);
			fprintf(file,"\n%s ",dec->name);
			if(dec->declaredIn!=t)
				fprintf(file, "*");
			fprintf(file,"\n********************\n");
			TtaListAbstractTree (dec->componentType.content, file);
			fprintf(file,"********************\n");
		}
	}
	
	aux = t->unions;
	if(!IsEmpty(aux))
	{
		fprintf(file, "\n\nUNIONS\n");
		fprintf(file, "------");
		for(First(aux);!IsDone(aux);Next(aux))
		{
			dec = (Declaration) CurrentElement(aux);
			fprintf(file,"\n%s ",dec->name);
			if(dec->declaredIn!=t)
				fprintf(file, "*");
			PrintUnion(dec, 1, t, file);
		}
	}
#endif /* TEMPLATES */
}

void DumpDeclarations(XTigerTemplate t)
{
#ifdef TEMPLATES
	char localname[MAX_LENGTH];
	FILE *file;

    strcpy (localname, TempFileDirectory);
    strcat (localname, DIR_STR);
    strcat (localname, "templateDecl.debug");
    file = TtaWriteOpen (localname);
		
	PrintDeclarations(t, file);

	TtaWriteClose (file);
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
	TemplateCtxt *ctx = (TemplateCtxt*)context;
	
	XTigerTemplate t = NewXTigerTemplate (TRUE);
	t->isLibrary = FALSE;
	
	Add (templates, ctx->templatePath, t);

	Element el = TtaGetMainRoot(newdoc);
	ParseDeclarations (t, el);	

#ifdef AMAYA_DEBUG	
	DumpDeclarations (t);

	char localname[MAX_LENGTH];
	FILE *file;

    strcpy (localname, TempFileDirectory);
    strcat (localname, DIR_STR);
    strcat (localname, "template.debug");
    file = TtaWriteOpen (localname);
		
	TtaListAbstractTree (TtaGetMainRoot(newdoc), file);

	TtaWriteClose (file);
#endif

    if(ctx->createInstance)
	{
		DontReplaceOldDoc = FALSE;
		InstanciateTemplate(newdoc, ctx->templatePath, ctx->instancePath, ctx->docType, TRUE);
	}
/* TODO: Must we free the resources??
	else
	{
		//Free the template document
		TtaCloseDocument(newdoc);
		FreeDocumentResource(newdoc);
	}
*/
	switch (ctx->docType)
	{
	case docSVG :
		TtaExportDocumentWithNewLineNumbers (newdoc, ctx->instancePath, "SVGT");
		break;
	case docMath :
		TtaExportDocumentWithNewLineNumbers (newdoc, ctx->instancePath, "MathMLT");
		break;
	case docHTML :
		if(TtaGetDocumentProfile(newdoc)==L_Xhtml11)
			TtaExportDocumentWithNewLineNumbers (newdoc, ctx->instancePath, "HTMLT11");
		else
			TtaExportDocumentWithNewLineNumbers (newdoc, ctx->instancePath, "HTMLTX");
		break;
	default :
		TtaExportDocumentWithNewLineNumbers (newdoc, ctx->instancePath, NULL);
		break;
	}

	TtaFreeMemory(ctx);
#endif /* TEMPLATES */
}


/*----------------------------------------------------------------------
----------------------------------------------------------------------*/
void LoadTemplate (Document doc, char* templatename, char* docname, DocumentType doctype, ThotBool createInstance)
{
#ifdef TEMPLATES

	//Stores the template path for show it in next instanciation forms
	char			*directory, *document;
	unsigned int	size = strlen(templatename);
	
	directory	= (char*) TtaGetMemory(size);
	document	= (char*) TtaGetMemory(size);

	TtaExtractName (templatename, directory, document);	
	TtaSetEnvString ("TEMPLATES_DIRECTORY", directory, TRUE);

	TtaFreeMemory(directory);
	TtaFreeMemory(document);

	//If types are not loaded we load the template and we parse it
	if(!Get(templates,templatename))
	{	
		//Creation of the callback context
		TemplateCtxt *ctx	= (TemplateCtxt *)TtaGetMemory (sizeof (TemplateCtxt));
		ctx->templatePath	= templatename;
		ctx->instancePath	= docname;
		ctx->docType		= doctype;
		ctx->createInstance = createInstance;
	
		Document newdoc = GetAmayaDoc (templatename, NULL, doc, doc, CE_MAKEBOOK, FALSE, 
			(void (*)(int, int, char*, char*, const AHTHeaders*, void*)) LoadTemplate_callback,
			(void *) ctx);
	}

	//Otherwise we will load the template later
	else if(createInstance)
		InstanciateTemplate(doc, templatename, docname, doctype, FALSE);

#endif /* TEMPLATES */
}
