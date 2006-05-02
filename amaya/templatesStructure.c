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

#ifdef TEMPLATES
#include "Template.h"
/* Information needed for the callback after loading a template.
   Just the path of the template, which identifies it. */
typedef struct _TemplateCtxt
{
	char *path;
} TemplateCtxt;

/*----------------------------------------------------------------------
  Creates an Element type and stores all needed information. 
  ----------------------------------------------------------------------*/
void AddElementDeclaration (XTigerTemplate t, Element el)
{
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
	
	Add(t->declaredTypes,name,dec);

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
}

/*----------------------------------------------------------------------
  Imports all declarations in a library lib to a template t
  ----------------------------------------------------------------------*/
void AddLibraryDeclarations (XTigerTemplate t, XTigerTemplate lib)
{
	lib->documentUsingMe++;
	DicDictionary from = lib->declaredTypes;
	DicDictionary to = t->declaredTypes;
	
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
	XTigerTemplate t = NewXTigerTemplate (FALSE);
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
                               !DontReplaceOldDoc /* replaceOldDoc */,
                               InNewWindow, /* inNewWindow */
                               DocumentName, docType, 0, FALSE,
                               L_Other, CE_ABSOLUTE);
      DontReplaceOldDoc = FALSE;
    }
	
	else
    {
      /* record the current position in the history */
      AddDocHistory (doc, DocumentURLs[doc], 
                     DocumentMeta[doc]->initial_url,
                     DocumentMeta[doc]->form_data,
                     DocumentMeta[doc]->method);
		
      newdoc = InitDocAndView (doc,
                               !DontReplaceOldDoc /* replaceOldDoc */,
                               InNewWindow, /* inNewWindow */
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
		
      /* change its directory name */
      TtaSetDocumentDirectory (newdoc, DirectoryName);
		
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
		
      /* check parsing errors */
      CheckParsingErrors (newdoc);
		
      /* Set elements access rights according to free_* elements */
      /*	Element el = TtaGetMainRoot (newdoc);
		
      if (el)	 
		  {
		  TtaSetAccessRight (el, ReadOnly, newdoc); 		 
		  CheckFreeAreas (newdoc, el);
      }*/
    }
#endif /* TEMPLATES */
}
