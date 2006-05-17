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
#include "templateDeclarations.h"
#include "templatesStructure_f.h"

#ifdef TEMPLATES
#include "Template.h"
/* Information needed for the callback after loading a template.
Just the path of the template, which identifies it. */
typedef struct _TemplateCtxt
{
	char *path;
} TemplateCtxt;

typedef struct _InstanciationCtxt
{
	char *			templatePath;
	char *			instancePath;
	char *			schemaName;
	DocumentType	docType;
} InstanciationCtxt;


/*----------------------------------------------------------------------
Creates an Element type and stores all needed information. 
----------------------------------------------------------------------*/
void RemoveOldDeclarations (XTigerTemplate t, char *name) {
	Remove(t->components, name);
	Remove(t->elements, name);
	Remove(t->unions, name);
	Remove(t->simpleTypes, name);
}

/*----------------------------------------------------------------------
Returns the value of a string attribute 
----------------------------------------------------------------------*/
char *GetAttributeStringValue (Element el, int att)
{
	AttributeType attType;
	attType.AttrSSchema = TtaGetElementType(el).ElSSchema;
	attType.AttrTypeNum = att;
	
	Attribute attribute = TtaGetAttribute(el, attType);
	
	int size = TtaGetTextAttributeLength(attribute);
	char *aux = (char*) TtaGetMemory(size+1);
	TtaGiveTextAttributeValue (attribute, aux, &size);
	return aux;
}

/*----------------------------------------------------------------------
Creates an Element type and stores all needed information. 
----------------------------------------------------------------------*/
void AddElementDeclaration (XTigerTemplate t, Element el)
{
	char *name;
	Declaration dec;
	
	name	  = GetAttributeStringValue (el, Template_ATTR_name);
	
	dec  = NewElement(t, name);
	
	RemoveOldDeclarations(t, name);
	Add(t->elements, name, dec);
	
	if (name)    TtaFreeMemory (name);
}

/*----------------------------------------------------------------------
Creates a Union type and stores all needed information. 
----------------------------------------------------------------------*/
void AddUnionDeclaration (XTigerTemplate t, Element el)
{
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
}

/*----------------------------------------------------------------------
Creates a Component type and stores all needed information. 
----------------------------------------------------------------------*/
void AddComponentDeclaration (XTigerTemplate t, Element el)
{
	char *name;
	Declaration dec;
	
	name = GetAttributeStringValue (el, Template_ATTR_name);
	dec = NewComponent(t, name, el);
	
	RemoveOldDeclarations(t, name);
	Add(t->components, name, dec);
	
	if (name)    TtaFreeMemory (name);
}

/*----------------------------------------------------------------------
Imports all declarations in a library lib to a template t
----------------------------------------------------------------------*/
void AddLibraryDeclarations (XTigerTemplate t, XTigerTemplate lib)
{	
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
}

/*----------------------------------------------------------------------
Load (if needed) a library and adds all its declarations to a template
----------------------------------------------------------------------*/
void AddImportedLibrary (XTigerTemplate t, Element el)
{
	XTigerTemplate lib = NULL;
	//Load the library
	AddLibraryDeclarations (t,lib);
}

/*----------------------------------------------------------------------
----------------------------------------------------------------------*/
void CheckTypesAttribute (XTigerTemplate t, Element el)
{
	char *types;
	DicDictionary dic;
	Declaration dec;
	
	types = GetAttributeStringValue (el, Template_ATTR_types);
	dic   = CreateDictionaryFromList(types);
	
	for (First(dic); !IsDone(dic); Next(dic))
		if ( GetDeclaration(t, CurrentKey(dic)) == NULL)
		{
			dec = NewElement(t, CurrentKey(dic));
			Add(t->elements, CurrentKey(dic), dec);
		}
		
		TtaFreeMemory(types);
		CleanDictionary(dic);
}

/*----------------------------------------------------------------------
----------------------------------------------------------------------*/
void ParseDeclarations (XTigerTemplate t, Element el)
{
	
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
}

/*----------------------------------------------------------------------
GetTemplate_callback: Called after loading a template.
----------------------------------------------------------------------*/
void GetTemplate_callback (int newdoc, int status,  char *urlName,
                           char *outputfile, AHTHeaders *http_headers,
                           void * context)
{	
	XTigerTemplate t = NewXTigerTemplate (TRUE);
	t->isLibrary = FALSE;
	
	Add (templates, ((TemplateCtxt*)context)->path, t);
	Element el = TtaGetMainRoot(newdoc);
	ParseDeclarations (t, el);
	
	//We free all the resources for the template
	TtaCloseDocument (newdoc);
	FreeDocumentResource (newdoc);
	CleanUpParsingErrors ();
}
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
#endif /* TEMPLATES */
	return "HTML";
}

/*----------------------------------------------------------------------
----------------------------------------------------------------------*/
void LoadTemplate (char* templatename, char* schemaname)
{
#ifdef TEMPLATES
	Document newdoc, templateDoc;
	
	if(Get(templates,templatename)) return;
	
	// We create a document (with no views) and ask Amaya to open the document
	// remplacing it (so no view will be created!)
	templateDoc = TtaNewDocument (schemaname, "tmp");
	DontReplaceOldDoc = FALSE;
	if (templateDoc != 0)
    {
		//Creation of the callback context
		TemplateCtxt *ctx = (TemplateCtxt *)TtaGetMemory (sizeof (TemplateCtxt));
		ctx->path = templatename;
		newdoc = GetAmayaDoc (templatename, NULL, templateDoc, templateDoc,
			CE_MAKEBOOK, FALSE, 
			(void (*)(int, int, char*, char*, const AHTHeaders*, void*)) GetTemplate_callback,
			(void *) ctx);
    }
#endif /* TEMPLATES */
}

void InstantiateTemplate_callback (int newdoc, int status,  char *urlName,
								   char *outputfile, AHTHeaders *http_headers,
								   void * context)
{
#ifdef TEMPLATES
	InstanciationCtxt *ctx = (InstanciationCtxt*)context;

	InstanciateTemplate(ctx->templatePath, newdoc);
	/*
	Document instanceDoc = TtaNewDocument (ctx->schemaName, ctx->instancePath);
	TtaCopyTree(TtaGetMainRoot(newdoc), newdoc, instanceDoc, TtaGetMainRoot(instanceDoc));
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

	//TtaExportDocument(newdoc, ctx->instancePath, ctx->docType);
	
	//We free all the resources for the template
	TtaCloseDocument (newdoc);
	FreeDocumentResource (newdoc);
/*
	//We free all the resources for the instance
	TtaCloseDocument (instanceDoc);
	FreeDocumentResource (instanceDoc);
*/
	CleanUpParsingErrors ();
/*	
	TtaExtractName (ctx->templatePath, DirectoryName, DocumentName);	
    TtaSetEnvString ("TEMPLATES_DIRECTORY", DirectoryName, TRUE);

	TtaExtractName (ctx->instancePath, DirectoryName, DocumentName);
	// change its directory name 
	TtaSetDocumentDirectory (instanceDoc, DirectoryName);
	TtaSetDocumentName(instanceDoc, DocumentName);

	// change its URL 
	TtaFreeMemory (DocumentURLs[instanceDoc]);
	DocumentURLs[instanceDoc] = TtaStrdup (ctx->instancePath);
	DocumentSource[instanceDoc] = 0;
	
	// add the URI in the combobox string 
	AddURLInCombobox (ctx->instancePath, NULL, FALSE);
	TtaSetTextZone (newdoc, 1, URL_list);
	

    LoadDocument (instanceDoc, ctx->instancePath, NULL, NULL, CE_ABSOLUTE,
		"", DocumentName, NULL, FALSE, &DontReplaceOldDoc, NULL);
	
	


	TtaSetDocumentModified (instanceDoc);
	W3Loading = 0;		// loading is complete now 
	DocNetworkStatus[instanceDoc] = AMAYA_NET_ACTIVE;
	int stopped_flag = FetchAndDisplayImages (instanceDoc, AMAYA_LOAD_IMAGE, NULL);
	
	if (!stopped_flag)
	{
		DocNetworkStatus[instanceDoc] = AMAYA_NET_INACTIVE;
		// almost one file is restored
		TtaSetStatus (instanceDoc, 1, TtaGetMessage (AMAYA, AM_DOCUMENT_LOADED),NULL);		 
	}
	*/
#endif /* TEMPLATES */
}


/*----------------------------------------------------------------------
----------------------------------------------------------------------*/
void InstanciateTemplate (char *templatename, char *schemaname, char *docname, DocumentType docType)
{
#ifdef TEMPLATES
	Document templateDoc = TtaNewDocument (schemaname, "tmp");
	DontReplaceOldDoc = FALSE;
	if (templateDoc != 0)
    {
		//Creation of the callback context
		InstanciationCtxt *ctx = (InstanciationCtxt *)TtaGetMemory (sizeof (InstanciationCtxt));
		ctx->templatePath = templatename;
		ctx->instancePath = docname;
		ctx->schemaName   = schemaname;
		ctx->docType      = docType;
		
		Document newdoc = GetAmayaDoc (templatename, NULL, templateDoc, templateDoc,
			CE_MAKEBOOK, FALSE, 
			(void (*)(int, int, char*, char*, const AHTHeaders*, void*)) InstantiateTemplate_callback,
			(void *) ctx);
    }
#endif /* TEMPLATES */
}

#ifdef TODO
/*----------------------------------------------------------------------
----------------------------------------------------------------------*/
void CreateInstanceDocument (Document doc, char *templatename, char *docname,
                             DocumentType docType)
{
#ifdef TEMPLATES
	Document newdoc;
	
	if (!IsW3Path (docname) && TtaFileExist (docname))
    {
		char *s = (char *)TtaGetMemory (strlen (docname) +
			strlen (TtaGetMessage (AMAYA, AM_OVERWRITE_CHECK)) + 2);
		sprintf (s, TtaGetMessage (AMAYA, AM_OVERWRITE_CHECK), docname);
		InitConfirm (0, 0, s);
		TtaFreeMemory (s);
		if (!UserAnswer)
			return;
    }
	
	if (InNewWindow || DontReplaceOldDoc)
    {
		newdoc = InitDocAndView (doc,
			!DontReplaceOldDoc, // replaceOldDoc
			InNewWindow, // inNewWindow
			DocumentName, docType, 0, FALSE,
			L_Other, CE_ABSOLUTE);
		DontReplaceOldDoc = FALSE;
    }
	
	else
    {
		// record the current position in the history
		AddDocHistory (doc, DocumentURLs[doc], 
			DocumentMeta[doc]->initial_url,
			DocumentMeta[doc]->form_data,
			DocumentMeta[doc]->method);
		
		newdoc = InitDocAndView (doc,
			!DontReplaceOldDoc, // replaceOldDoc
			InNewWindow, // inNewWindow
			DocumentName, docType, 0, FALSE,
			L_Other, CE_ABSOLUTE);
    }
	
	if (newdoc != 0)
    {	
		TtaExtractName (templatename, DirectoryName, DocumentName);
		
		TtaSetEnvString ("TEMPLATES_DIRECTORY", DirectoryName, TRUE);
		
		LoadDocument (newdoc, templatename, NULL, NULL, CE_ABSOLUTE,
			"", DocumentName, NULL, FALSE, &DontReplaceOldDoc, NULL);
		
		/* change its URL */
		TtaFreeMemory (DocumentURLs[newdoc]);
		DocumentURLs[newdoc] = TtaStrdup (docname);
		DocumentSource[newdoc] = 0;
		
		/* add the URI in the combobox string */
		AddURLInCombobox (docname, NULL, FALSE);
		TtaSetTextZone (newdoc, 1, URL_list);
		
		TtaExtractName (docname, DirectoryName, DocumentName);
		/* change its directory name */
		TtaSetDocumentDirectory (newdoc, DirectoryName);
		TtaSetDocumentName(newdoc, DocumentName);
		
		TtaSetDocumentModified (newdoc);
		W3Loading = 0;		/* loading is complete now */
		DocNetworkStatus[newdoc] = AMAYA_NET_ACTIVE;
		int stopped_flag = FetchAndDisplayImages (newdoc, AMAYA_LOAD_IMAGE, NULL);
		
		if (!stopped_flag)
        {
			DocNetworkStatus[newdoc] = AMAYA_NET_INACTIVE;
			/* almost one file is restored */
			TtaSetStatus (newdoc, 1, TtaGetMessage (AMAYA, AM_DOCUMENT_LOADED),NULL);		 
        }
		
		/* Set elements access rights according to template elements */
		InstanciateTemplate(templatename, newdoc);
		
		TtaSaveDocument(newdoc, docname);
		
		/* check parsing errors */
		CheckParsingErrors (newdoc);
    }
#endif /* TEMPLATES */
}
#endif /* TODO */

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
			//Create the min number of repetitions
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
	XTigerTemplate t;
	Element el;
	
	//Add link to template
	
	//Instanciate all elements
	t	=	(XTigerTemplate) Get(templates,templatename);
	el	=	TtaGetMainRoot (doc);
	ParseTemplate(t, el, doc);
#endif /* TEMPLATES */
}
