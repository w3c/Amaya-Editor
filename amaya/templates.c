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

#ifdef TEMPLATES
#include "Template.h"
#include "templates.h"
#include "templateDeclarations.h"

#include "mydictionary_f.h"
#include "templateLoad_f.h"
#include "templateInstanciation_f.h"
#include "templateDeclarations_f.h"
#include "appdialogue_wx.h"
#include "init_f.h"
#include "wxdialogapi_f.h"
#include "AHTURLTools_f.h"

#endif /* TEMPLATES */

/*----------------------------------------------------------------------
  NewTemplate: Create the "new document from template" dialog
  ----------------------------------------------------------------------*/
void NewTemplate (Document doc, View view)
{
#ifdef TEMPLATES
  char *templateDir = TtaGetEnvString ("TEMPLATES_DIRECTORY");
  ThotBool created;

  if (templates == NULL)
    InitializeTemplateEnvironment();
  created = CreateNewTemplateDocDlgWX(BaseDialog + OpenTemplate,
                                      /*TtaGetViewFrame (doc, view)*/NULL, doc,
                                      TtaGetMessage (AMAYA, AM_NEW_TEMPLATE),templateDir);
  
  if (created)
    {
      TtaSetDialoguePosition ();
      TtaShowDialogue (BaseDialog + OpenTemplate, TRUE);
    }

#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Load a template and create the instance file - update images and 
  stylesheets related to the template.
  ----------------------------------------------------------------------*/
void CreateInstanceOfTemplate (Document doc, char *templatename, char *docname)
{
#ifdef TEMPLATES

  char *s;
  ThotBool dontReplace = DontReplaceOldDoc;

  if (!IsW3Path (docname) && TtaFileExist (docname))
    {
      s = (char *)TtaGetMemory (strlen (docname) +
                                strlen (TtaGetMessage (AMAYA, AM_OVERWRITE_CHECK)) + 2);
      sprintf (s, TtaGetMessage (AMAYA, AM_OVERWRITE_CHECK), docname);
      InitConfirm (0, 0, s);
      TtaFreeMemory (s);      
      if (!UserAnswer)
        return;
    }    

  LoadTemplate(0, templatename);
  DontReplaceOldDoc = dontReplace;
  CreateInstance(templatename, docname);
  
#endif /* TEMPLATES */
}

#ifdef TEMPLATES
/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void giveItems(char *text, int size, struct menuType **items, int *nbitems)
{
	ThotBool         inElement = TRUE;
  struct menuType *menu;
  char            *iter;
	char             temp[128];
  int              i;
	int              labelSize;

	*nbitems = 1;
	for (i = 0; i < size; i++)
    {
      if (isEOSorWhiteSpace (text[i]))
        {
          if (inElement)
            inElement = FALSE;
        }
      else if (!inElement)
        {
          inElement = TRUE;
          (*nbitems)++;
        }
    }

	menu = (struct menuType*) TtaGetMemory(sizeof(struct menuType)* *nbitems);
	iter = text;
	for (i = 0; i < *nbitems; i++)
    {		
      labelSize = 0;
      while (isEOSorWhiteSpace (*iter))
        iter++;

      while (!isEOSorWhiteSpace (*iter))
        {
          temp[labelSize++] = *iter;
          iter++;
        }

      temp[labelSize] = EOS;
      menu[i].label = (char *) TtaStrdup(temp);
      menu[i].type = SimpleTypeNat;  /* @@@@@ ???? @@@@@ */
      *items = menu;
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static char *createMenuString (const struct menuType* items, const int nbItems)
{
  char *result, *iter;
	int   size = 0;
  int   i;

	for (i=0; i < nbItems; i++)
		size += 2 + strlen (items[i].label);

	result = (char *) TtaGetMemory(size);
	iter = result;
	for (i=0; i < nbItems; i++)
    {
      *iter = 'B';
      ++iter;
		
      strcpy (iter, items[i].label);
      iter += strlen (items[i].label)+1;
    }
	return result;
}

#endif /* TEMPLATES */

/*----------------------------------------------------------------------
  UseMenuClicked
  Shows a menu with all the types that can be used in a use element.
  ----------------------------------------------------------------------*/
ThotBool UseMenuClicked (NotifyElement *event)
{
#ifdef TEMPLATES
	Document         doc = event->document;
	Element          el = event->element;
	ElementType      elt = TtaGetElementType(el);
	Attribute        at;
	AttributeType    att;
  XTigerTemplate   t;
  Declaration      dec;
	int              nbitems, size;
	struct menuType *items;
  char            *types, *menuString;
	
  t = (XTigerTemplate) Get(templates, DocumentMeta[doc]->template_url);
  if (!t)
    return FALSE; // no template ?!?!

	att.AttrSSchema = elt.ElSSchema;
	att.AttrTypeNum = Template_ATTR_types;
	at = TtaGetAttribute (el, att);

	size = TtaGetTextAttributeLength (at);
	types = (char *) TtaGetMemory (size+1);	
	TtaGiveTextAttributeValue (at, types, &size);
  
	giveItems (types, size, &items, &nbitems);
  if (nbitems == 1)
    {
      dec = GetDeclaration(t, items[0].label);
      /* if it's a union, display the menu of this union */
      if (dec && dec->nature == UnionNat)
        {
          /*
          nbitems = 0;
          dec->unionType.include.first */
          /* @@@@@@@ */
        }
    }
	menuString = createMenuString (items, nbitems);
	TtaNewScrollPopup (BaseDialog + OptionMenu, TtaGetViewFrame (doc, 1), NULL, 
                     nbitems, menuString , NULL, false, 'L');

	TtaFreeMemory (menuString);
	TtaFreeMemory (types);

	TtaShowDialogue (BaseDialog + OptionMenu, FALSE);
	TtaWaitShowProcDialogue();
	TtaDestroyDialogue (BaseDialog + OptionMenu);
  
  /* result: items[ReturnOption].label @@@@@ */
  dec = GetDeclaration(t, items[ReturnOption].label);
  if (dec)
    {
      size = 1; /* @@@@ */
    }
  TtaFreeMemory (items);
  return FALSE;
#endif /* TEMPLATES */
	//ReturnOption
	return TRUE;
}

/*----------------------------------------------------------------------
  OptionMenuClicked
  ----------------------------------------------------------------------*/
ThotBool OptionMenuClicked (NotifyElement *event)
{
#ifdef TEMPLATES
  return FALSE;
#endif /* TEMPLATES */
	return TRUE;
}

/*----------------------------------------------------------------------
  RepeatMenuClicked
  Shows a menu with all the types that can be used in a use element.
  ----------------------------------------------------------------------*/
ThotBool RepeatMenuClicked (NotifyElement *event)
{
#ifdef TEMPLATES
	Document         doc = event->document;
	int              nbitems, size;
	struct menuType *items;
  char            *types, *menuString;

	types = "top end";	
	size = strlen (types);
	giveItems (types, size, &items, &nbitems);
	menuString = createMenuString (items, nbitems);
	TtaNewScrollPopup (BaseDialog + OptionMenu, TtaGetViewFrame (doc, 1), NULL, 
                     nbitems, menuString , NULL, false, 'L');
	TtaFreeMemory (menuString);
	TtaShowDialogue (BaseDialog + OptionMenu, FALSE);
	TtaWaitShowProcDialogue();
	TtaDestroyDialogue (BaseDialog + OptionMenu);
  return FALSE;
#endif /* TEMPLATES */
	return TRUE;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void OpeningInstance(Document doc)
{
#ifdef TEMPLATES
  char            aux[MAX_LENGTH], content[MAX_LENGTH];
	ElementType		  piType, lineType, textType;
	Element			    root, pi, line, text;
  Language        language;
	char		        *s;
  int             size;
  
  if (DocumentURLs[doc] == NULL)
    return;
  //If it is a template we must ignore it
  strcpy (aux, DocumentURLs[doc]);
  strcpy (content, &aux[strlen(aux)-4]);
  if(strncasecmp (content, ".XTD", strlen(content))==0)
    return;

  content[0] = EOS;
	//Instanciate all elements
	root =	TtaGetMainRoot (doc);
  //Look for PIs
  /* check if the document has a DOCTYPE declaration */
#ifdef ANNOTATIONS
  if (DocumentTypes[doc]  == docAnnot)
    piType = TtaGetElementType (root);
  else
#endif /* ANNOTATIONS */
    piType = TtaGetElementType (root);
  
  lineType.ElSSchema = piType.ElSSchema;
  textType.ElSSchema = piType.ElSSchema;
  
  s = TtaGetSSchemaName (piType.ElSSchema);
  
  if (strcmp (s, "HTML") == 0)
    {
      piType.ElTypeNum = HTML_EL_XMLPI;  
      lineType.ElTypeNum = HTML_EL_PI_line;
      textType.ElTypeNum = HTML_EL_TEXT_UNIT;
    }
#ifdef _SVG
  else if (strcmp (s, "SVG") == 0)    
    {
      piType.ElTypeNum = SVG_EL_XMLPI;  
      lineType.ElTypeNum = SVG_EL_XMLPI_line;
      textType.ElTypeNum = SVG_EL_TEXT_UNIT;
    }
#endif /* _SVG */
  else if (strcmp (s, "MathML") == 0)
    {
      piType.ElTypeNum = MathML_EL_XMLPI;  
      lineType.ElTypeNum = MathML_EL_XMLPI_line;
      textType.ElTypeNum = MathML_EL_TEXT_UNIT;
    }
  else
    {
      piType.ElTypeNum = XML_EL_xmlpi;
      lineType.ElTypeNum = XML_EL_xmlpi_line;
      textType.ElTypeNum = XML_EL_TEXT_UNIT;
    }
  
  pi = TtaSearchTypedElement (piType, SearchInTree, root);  
  while(pi!=NULL)
    {
      content[0] = '\0';
      line = TtaSearchTypedElement (lineType, SearchInTree, pi);
      while(line!=NULL)
        {
          text = TtaSearchTypedElement (textType, SearchInTree, line);
          size = MAX_LENGTH;
          TtaGiveTextContent(text, (unsigned char*)aux, &size, &language);
          strcat(content, aux);

          //This is not an XTiger PI
          if(!strstr(content,"xtiger")) break;            
 
          line = TtaSearchTypedElement (lineType, SearchForward, line);
        }
      pi = TtaSearchTypedElement (piType, SearchForward, pi);
    }

  DocumentMeta[doc]->template_url = NULL;

  if(content[0]=='\0')
    return;
    
  char *pointer;
  
  //xtiger
  strcpy(aux, content);
  aux[6]='\0';
  if(strcmp(aux,"xtiger")!=0)
    return;
  
  //template
  pointer = strstr(content, "template");
  if(pointer==NULL)
    return;

  //=
  pointer = strstr(pointer, "=");
  if(pointer==NULL)
    return;
  
  //"
  pointer = strstr(pointer, "\"");
  if(pointer==NULL)
    return;
  
  //content
  strcpy(aux, pointer+1);
  pointer = strstr(aux, "\"");
  if(pointer==NULL)
    return;
  *pointer = '\0';
  
  //and finally
  DocumentMeta[doc]->template_url = TtaStrdup(aux);

  if(!templates) InitializeTemplateEnvironment();

  XTigerTemplate t = (XTigerTemplate)Get(templates, aux);

  if(!t)
    {
      LoadTemplate(0, aux);
      t = (XTigerTemplate)Get(templates, aux);
    }
  AddUser(t);

#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  ClosingInstance
  Callback called before closing a document. Checks for unused templates.
  ----------------------------------------------------------------------*/
ThotBool ClosingInstance(NotifyDialog* dialog)
{
#ifdef TEMPLATES
  //If it is a template all has been already freed
  if(DocumentMeta[dialog->document] == NULL) return FALSE;

  char *turl = DocumentMeta[dialog->document]->template_url;
  if(turl)
    {
      XTigerTemplate t = (XTigerTemplate)Get(templates, turl);
      if(t)
        RemoveUser(t);
      TtaFreeMemory(turl);
    }
#endif /* TEMPLATES */
  return FALSE;
}
