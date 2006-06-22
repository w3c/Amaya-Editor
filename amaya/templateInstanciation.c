#include "templateInstanciation.h"

#include "HTMLactions_f.h"
#include "init_f.h"

#include "templateUtils.h"
#include "templateDeclarations.h"

#ifdef TEMPLATES

#include "Template.h"

typedef struct _InstanciationCtxt
{
	char *			templatePath;
	char *			instancePath;
	char *			schemaName;
	DocumentType	docType;
	ThotBool		dontReplace;
} InstanciationCtxt;

#endif

void ReallyInstanciateTemplate (char *instancePath, char *templatePath, int doc, DocumentType docType)
{
#ifdef TEMPLATES
	//We copy the templatePath so the context can be freed later.
	InstanciateTemplate(strdup(templatePath), doc);

#ifdef AMAYA_DEBUG

	char localname[MAX_LENGTH];
	FILE *file;

    strcpy (localname, TempFileDirectory);
    strcat (localname, DIR_STR);
    strcat (localname, "template.debug");
    file = TtaWriteOpen (localname);
		
	TtaListAbstractTree (TtaGetRootElement(doc), file);

	TtaWriteClose (file);

#endif /* AMAYA_DEBUG */

	switch (docType)
	{
	case docSVG :
		TtaExportDocumentWithNewLineNumbers (doc, instancePath, "SVGT");
		break;
	case docMath :
		TtaExportDocumentWithNewLineNumbers (doc, instancePath, "MathMLT");
		break;
	case docHTML :
		if(TtaGetDocumentProfile(doc)==L_Xhtml11)
			TtaExportDocumentWithNewLineNumbers (doc, instancePath, "HTMLT11");
		else
			TtaExportDocumentWithNewLineNumbers (doc, instancePath, "HTMLTX");
		break;
	default :
		TtaExportDocumentWithNewLineNumbers (doc, instancePath, NULL);
		break;
	}

/* TODO: Must we free the resources??
	//Free the template document
	TtaCloseDocument(doc);
	FreeDocumentResource(doc);
*/
	//Open the instance
	DontReplaceOldDoc = FALSE;
	TtaExtractName (instancePath, DirectoryName, DocumentName);
	CallbackDialogue (BaseDialog + OpenForm, INTEGER_DATA, (char *) 1);

#endif /* TEMPLATES */
}

void InstantiateTemplate_callback (int newdoc, int status,  char *urlName,
								   char *outputfile, AHTHeaders *http_headers,
								   void * context)
{
#ifdef TEMPLATES
	InstanciationCtxt *ctx = (InstanciationCtxt*)context;
	ReallyInstanciateTemplate(ctx->instancePath, ctx->templatePath, newdoc, ctx->docType);
#endif /* TEMPLATES */
}


/*----------------------------------------------------------------------
----------------------------------------------------------------------*/
void InstanciateTemplate (Document doc, char *templatename, char *docname, DocumentType docType, ThotBool loaded)
{
#ifdef TEMPLATES
	if(!loaded)
	{
		//Creation of the callback context
		InstanciationCtxt *ctx	= (InstanciationCtxt *)TtaGetMemory (sizeof (InstanciationCtxt));
		ctx->templatePath		= templatename;
		ctx->instancePath		= docname;
		ctx->schemaName			= GetSchemaFromDocType(docType);
		ctx->docType			= docType;
		
		Document newdoc = GetAmayaDoc (templatename, NULL, doc, doc, CE_MAKEBOOK, FALSE, 
			(void (*)(int, int, char*, char*, const AHTHeaders*, void*)) InstantiateTemplate_callback,
			(void *) ctx);
	}
	else
		ReallyInstanciateTemplate(docname, templatename, doc, docType);

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
	//ElementType		elType;
	Element			root/*, piElem, piLine, piLeaf*/;
	//char		*piValue;

	//Instanciate all elements
	t		=	(XTigerTemplate) Get(templates,templatename);
	root	=	TtaGetMainRoot (doc);
	ParseTemplate(t, root, doc);
/*
	//Add link to template	
	elType = TtaGetElementType(root);	
	elType.ElTypeNum = HTML_EL_XMLPI;
	piElem = TtaNewElement (doc, elType);

	TtaInsertFirstChild(&piElem, root, doc);
	
	// Create a XMLPI_line element as the first child of element XMLPI
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
*/
#endif /* TEMPLATES */
}
