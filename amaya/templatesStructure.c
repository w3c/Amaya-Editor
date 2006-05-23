/*
*
*  COPYRIGHT INRIA and W3C, 1996-2005
*  Please first read the full copyright statement in file COPYRIGHT.
*
*/

/*
* Authors: Francesc Campoy Flores
*
*/

#define THOT_EXPORT extern
#include "amaya.h"
#include "document.h"
#include "init_f.h"
#include "HTMLimage_f.h"
#include "HTMLactions_f.h"
#include "HTMLhistory_f.h"
#include "AHTURLTools_f.h"
#include "Xml2thot_f.h"

#include "templateDeclarations.h"
#include "templatesStructure_f.h"

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

typedef struct _InstanciationCtxt
{
	char *			templatePath;
	char *			instancePath;
	char *			schemaName;
	DocumentType	docType;
	ThotBool		dontReplace;
} InstanciationCtxt;

#endif /* TEMPLATES */

/*----------------------------------------------------------------------
GetSchemaFromDocType: Returns the name of the schema corresponding to 
a doc type.
----------------------------------------------------------------------*/
char *GetSchemaFromDocType (DocumentType docType)
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
Creates an Element type and stores all needed information. 
----------------------------------------------------------------------*/
void RemoveOldDeclarations (XTigerTemplate t, char *name) {
#ifdef TEMPLATES
	Remove(t->components, name);
	Remove(t->elements, name);
	Remove(t->unions, name);
	Remove(t->simpleTypes, name);
#endif // TEMPLATES
}

/*----------------------------------------------------------------------
Returns the value of a string attribute 
----------------------------------------------------------------------*/
char *GetAttributeStringValue (Element el, int att)
{
#ifdef TEMPLATES
	AttributeType attType;
	attType.AttrSSchema = TtaGetElementType(el).ElSSchema;
	attType.AttrTypeNum = att;
	
	Attribute attribute = TtaGetAttribute(el, attType);
	
	int size = TtaGetTextAttributeLength(attribute);
	char *aux = (char*) TtaGetMemory(size+1);
	TtaGiveTextAttributeValue (attribute, aux, &size);
	return aux;
#endif /* TEMPLATES */
}

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
Imports all declarations in a library lib to a template t
----------------------------------------------------------------------*/
void AddLibraryDeclarations (XTigerTemplate t, XTigerTemplate lib)
{	
#ifdef TEMPLATES
	DicDictionary from = lib->elements;	
	DicDictionary to = t->elements;
	
	for (First(from); !IsDone(from); Next(from))
		Add (to, CurrentKey(from), CurrentElement(from));
	
	from = lib->components;	
	to = t->components;
	
	for (First(from); !IsDone(from); Next(from))
		Add (to, CurrentKey(from), CurrentElement(from));
	
	from = lib->unions;	
	to = t->unions;
	
	for (First(from); !IsDone(from); Next(from))
		Add (to, CurrentKey(from), CurrentElement(from));
	
	from = lib->simpleTypes;	
	to = t->simpleTypes;
	
	for (First(from); !IsDone(from); Next(from))
		Add (to, CurrentKey(from), CurrentElement(from));
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
Load (if needed) a library and adds all its declarations to a template
----------------------------------------------------------------------*/
void AddImportedLibrary (XTigerTemplate t, Element el)
{
#ifdef TEMPLATES
	XTigerTemplate lib = NULL;
	//Load the library
	AddLibraryDeclarations (t,lib);
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

	if(ctx->createInstance) {
		DontReplaceOldDoc = FALSE;
		InstanciateTemplate(newdoc, ctx->templatePath, ctx->instancePath, ctx->docType);
	}

	TtaFreeMemory(ctx);
#endif /* TEMPLATES */
}


/*----------------------------------------------------------------------
----------------------------------------------------------------------*/
void LoadTemplate (Document doc, char* templatename, char* docname, DocumentType doctype, ThotBool createInstance)
{
#ifdef TEMPLATES
	//Stores the template path for next instanciations
	char			*directory, *document;
	unsigned int	size = strlen(templatename);
	
	directory	= (char*) TtaGetMemory(size);
	document	= (char*) TtaGetMemory(size);

	TtaExtractName (templatename, directory, document);	
	TtaSetEnvString ("TEMPLATES_DIRECTORY", directory, TRUE);

	TtaFreeMemory(directory);
	TtaFreeMemory(document);

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
	else if(createInstance)
		InstanciateTemplate(doc, templatename, docname, doctype);
#endif /* TEMPLATES */
}

void InstantiateTemplate_callback (int newdoc, int status,  char *urlName,
								   char *outputfile, AHTHeaders *http_headers,
								   void * context)
{
#ifdef TEMPLATES
	InstanciationCtxt *ctx = (InstanciationCtxt*)context;

	//We copy the templatePath so the context can be freed later.
	InstanciateTemplate(strdup(ctx->templatePath), newdoc);

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

	//Open the instance
	DontReplaceOldDoc = FALSE;
	TtaExtractName (ctx->instancePath, DirectoryName, DocumentName);
	CallbackDialogue (BaseDialog + OpenForm, INTEGER_DATA, (char *) 1);

#endif /* TEMPLATES */
}


/*----------------------------------------------------------------------
----------------------------------------------------------------------*/
void InstanciateTemplate (Document doc, char *templatename, char *docname, DocumentType docType)
{
#ifdef TEMPLATES
		//Creation of the callback context
		InstanciationCtxt *ctx	= (InstanciationCtxt *)TtaGetMemory (sizeof (InstanciationCtxt));
		ctx->templatePath		= templatename;
		ctx->instancePath		= docname;
		ctx->schemaName			= GetSchemaFromDocType(docType);
		ctx->docType			= docType;
		
		Document newdoc = GetAmayaDoc (templatename, NULL, doc, doc, CE_MAKEBOOK, FALSE, 
			(void (*)(int, int, char*, char*, const AHTHeaders*, void*)) InstantiateTemplate_callback,
			(void *) ctx);
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
----------------------------------------------------------------------*/
void ParseTemplate (XTigerTemplate t, Element el, Document doc)
{
#ifdef TEMPLATES
	AttributeType attType;
	Attribute att;
	char *name;
	ElementType type = TtaGetElementType(el);
	
	if(strcmp(TtaGetSSchemaName(type.ElSSchema),"Template")==0)
    {
		switch(type.ElTypeNum)
        {
        case Template_EL_head :
			//Remove it and all of its children
			TtaDeleteTree(el, doc);
			//We must stop searching into this tree
			return;
			break;
        case Template_EL_component :
			//Replace by a use				
			attType.AttrSSchema = TtaGetElementType(el).ElSSchema;
			attType.AttrTypeNum = Template_ATTR_name;
			
			name = GetAttributeStringValue(el, Template_ATTR_name);		  		  
			TtaRemoveAttribute(el, TtaGetAttribute(el, attType),doc);
			
			TtaChangeElementType(el, Template_EL_useEl);
			
			attType.AttrTypeNum = Template_ATTR_types;
			att = TtaNewAttribute(attType);
			TtaAttachAttribute(el, att, doc);
			TtaSetAttributeText(att, name, el, doc);
			
			attType.AttrTypeNum = Template_ATTR_currentType;
			att = TtaNewAttribute(attType);
			TtaAttachAttribute(el, att, doc);		  
			TtaSetAttributeText(att, name, el, doc);
			
			break;
		case Template_EL_bag :
			//Link to types
			//Allow editing the content
			break;
		case Template_EL_useEl :
			//Link to types
			//Create the default content if needed
			//If simple type allow editing the content
			break;
		case Template_EL_attribute :
			//Initialize the attribute
			//Allow the edition of the attribute
		case Template_EL_repeat :
			//Create the min number of repetitñions
        default :
			break;
        }
    }
	
	Element child = TtaGetFirstChild(el);
	Element aux; //Needed when deleting trees
	while(child!=NULL) {
		aux = child;
		TtaNextSibling(&aux);
		ParseTemplate(t, child, doc);
		child = aux;
	}
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
----------------------------------------------------------------------*/
void InstanciateTemplate(char *templatename, Document doc)
{
#ifdef TEMPLATES
	XTigerTemplate	t;
	ElementType		elType;
	Element			root, piElem, piLine, piLeaf;

	char		*piValue;

	//Instanciate all elements
	t		=	(XTigerTemplate) Get(templates,templatename);
	root	=	TtaGetMainRoot (doc);
	ParseTemplate(t, root, doc);

	//Add link to template	
	elType = TtaGetElementType(root);	
	elType.ElTypeNum = HTML_EL_XMLPI;
	piElem = TtaNewElement (doc, elType);

	TtaInsertFirstChild(&piElem, root, doc);
	/* Create a XMLPI_line element as the first child of element XMLPI */

	elType.ElTypeNum = HTML_EL_PI_line;	
	piLine = TtaNewElement (doc, elType);
	TtaInsertFirstChild (&piLine, piElem, doc);
	
	piValue = (char*)TtaGetMemory(
		strlen("xtiger template=\"")+
		strlen(templatename)+
		2);
	strcpy(piValue,"xtiger template=\"");
	strcat(piValue,templatename);
	strcat(piValue, "\"");
	
	elType.ElTypeNum = HTML_EL_TEXT_UNIT;
	piLeaf = TtaNewElement (doc, elType);	
	TtaInsertFirstChild (&piLeaf, piLine, doc);

	TtaSetTextContent (piLeaf, (unsigned char *)piValue, Latin_Script, doc);
	TtaFreeMemory(piValue);

#endif /* TEMPLATES */
}
