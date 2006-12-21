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
#include "templateDeclarations_f.h"
#include "templateInstantiate_f.h"
#include "appdialogue_wx.h"
#include "init_f.h"
#include "wxdialogapi_f.h"
#include "AHTURLTools_f.h"

#endif /* TEMPLATES */


#include "fetchXMLname_f.h"
#include "MENUconf.h"

#include "containers.h"
#include "Elemlist.h"


/* Paths from which looking for templates.*/
static Prop_Templates_Path *TemplateRepositoryPaths;


/*----------------------------------------------------------------------
  IsTemplateInstanceDocument: Test if a document is a template instance
  doc : Document to test
  return : TRUE if the document is a template instance
  ----------------------------------------------------------------------*/
ThotBool IsTemplateInstanceDocument(Document doc){
#ifdef TEMPLATES
  return (DocumentMeta[doc]!=NULL) && (DocumentMeta[doc]->template_url!=NULL);
#else  /* TEMPLATES */
  return false;
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  AllocTemplateRepositoryListElement: alloc an element for the list of template repositories.
  path : path of the new element
  return : address of the new element
  ----------------------------------------------------------------------*/
void* AllocTemplateRepositoryListElement (const char* path, void* prevElement)
{
  Prop_Templates_Path *element = (Prop_Templates_Path*)TtaGetMemory (sizeof(Prop_Templates_Path));
  element->NextPath = NULL;
  strcpy (element->Path, path);
  if (prevElement)
  {
    element->NextPath = ((Prop_Templates_Path*)prevElement)->NextPath;
    ((Prop_Templates_Path*)prevElement)->NextPath = element;
  }
  return element;
}


/*----------------------------------------------------------------------
  FreeTemplateRepositoryList: Free the list of template repositories.
  list : address of the list (address of the first element).
  ----------------------------------------------------------------------*/
void FreeTemplateRepositoryList (void* list)
{
  Prop_Templates_Path** l = (Prop_Templates_Path**) list;
  
  Prop_Templates_Path* element = *l;
  l = NULL;
  while (element)
  {
    Prop_Templates_Path* next = element->NextPath;
    TtaFreeMemory(element);
    element = next;
  }
}

/*----------------------------------------------------------------------
  CopyTemplateRepositoryList: Copy a list of template repositories.
  src : address of the list (address of the first element).
  dst : address where copy the list
  ----------------------------------------------------------------------*/
static void CopyTemplateRepositoryList (const Prop_Templates_Path** src, Prop_Templates_Path** dst)
{
  Prop_Templates_Path *element=NULL, *current=NULL;
  
  if(*src!=NULL)
  {
    *dst = (Prop_Templates_Path*) TtaGetMemory (sizeof(Prop_Templates_Path));
    (*dst)->NextPath = NULL;
    strcpy((*dst)->Path, (*src)->Path);
    
    element = (*src)->NextPath;
    current = *dst;
  }

  while (element){
    current->NextPath = (Prop_Templates_Path*) TtaGetMemory (sizeof(Prop_Templates_Path));
    current = current->NextPath; 
    current->NextPath = NULL;
    strcpy(current->Path, element->Path);
    element = element->NextPath;
  }
}

/*----------------------------------------------------------------------
  LoadTemplateRepositoryList: Load the list of template repositories.
  list   : address of the list (address of the first element).
  return : the number of readed repository paths.
  ----------------------------------------------------------------------*/
static int LoadTemplateRepositoryList (Prop_Templates_Path** list)
{
  Prop_Templates_Path *element, *current = NULL;
  char *path, *homePath;
  unsigned char *c;
  int nb = 0;
  FILE *file;
  
  FreeTemplateRepositoryList(list);
  
  path = (char *) TtaGetMemory (MAX_LENGTH);
  homePath       = TtaGetEnvString ("APP_HOME");
  sprintf (path, "%s%ctemplate-repositories.dat", homePath, DIR_SEP);
  
  file = TtaReadOpen ((char *)path);
  if (!file)
  {
    /* The config file dont exist, create it. */
    file = TtaWriteOpen ((char *)path);
    fprintf(file, "%s%ctemplates%cen\n", homePath, DIR_SEP, DIR_SEP);
    TtaWriteClose (file);
    /* Retry to open it.*/
    file = TtaReadOpen ((char *)path);
  }
  
  if (file)
  {
    c = (unsigned char*)path;
    *c = EOS;
    while (TtaReadByte (file, c)){
      if (*c==13 || *c==EOL)
        *c = EOS;
      if (*c==EOS && c!=(unsigned char*)path )
      {
        element = (Prop_Templates_Path*) TtaGetMemory (sizeof(Prop_Templates_Path));
        element->NextPath = NULL;
        strcpy (element->Path, path);
        
        if (*list == NULL)
          *list = element; 
        else
          current->NextPath = element;
        current = element;
        nb++;

        c = (unsigned char*) path;
        *c = EOS;
      }
      else
        c++;
    }
    if (c!=(unsigned char*)path && *path!=EOS)
    {
      element = (Prop_Templates_Path*) TtaGetMemory (sizeof(Prop_Templates_Path));
      *(c+1) = EOS;
      strcpy (element->Path, path);
      element->NextPath = NULL;
      
      if (*list == NULL)
        *list = element; 
      else
        current->NextPath = element;
      nb++;
    }
    TtaReadClose (file);
  }
  TtaFreeMemory(path);
  return nb;
}

/*----------------------------------------------------------------------
  SaveTemplateRepositoryList: Save the list of template repositories.
  list   : address of the list (address of the first element).
  ----------------------------------------------------------------------*/
static void SaveTemplateRepositoryList (const Prop_Templates_Path** list)
{
  const Prop_Templates_Path *element;
  char *path, *homePath;
  unsigned char *c;
  FILE *file;

  path = (char *) TtaGetMemory (MAX_LENGTH);
  homePath       = TtaGetEnvString ("APP_HOME");
  sprintf (path, "%s%ctemplate-repositories.dat", homePath, DIR_SEP);

  file = TtaWriteOpen ((char *)path);
  c = (unsigned char*)path;
  *c = EOS;
  if (file)
  {
    element = *list;
    while (element)
    {
      fprintf(file, "%s\n", element->Path);
      element = element->NextPath;
    }
    TtaWriteClose (file);
  }
}

/*----------------------------------------------------------------------
  GetTemplateRepositoryList: Get the list of template repositories from template environment.
  list : address of the list (address of the first element).
  ----------------------------------------------------------------------*/
void GetTemplateRepositoryList (void* list)
{
  Prop_Templates_Path** l = (Prop_Templates_Path**) list;
  CopyTemplateRepositoryList((const Prop_Templates_Path**)&TemplateRepositoryPaths, l);
}

/*----------------------------------------------------------------------
  SetTemplateRepositoryList: Set the list of template repositories environment.
  list : address of the list (address of the first element).
  ----------------------------------------------------------------------*/
void SetTemplateRepositoryList (const void* list)
{
  const Prop_Templates_Path** l = (const Prop_Templates_Path**) list;
  CopyTemplateRepositoryList((const Prop_Templates_Path**)l, &TemplateRepositoryPaths);
  SaveTemplateRepositoryList((const Prop_Templates_Path**)&TemplateRepositoryPaths);
}

/*-----------------------------------------------------------------------
   InitTemplates
   Initializes the annotation library
  -----------------------------------------------------------------------*/
void InitTemplates ()
{
  TtaSetEnvBoolean ("SHOW_TEMPLATES", TRUE, FALSE);
  LoadTemplateRepositoryList(&TemplateRepositoryPaths);
}






/*----------------------------------------------------------------------
  NewTemplate: Create the "new document from template" dialog
  ----------------------------------------------------------------------*/
void NewTemplate (Document doc, View view)
{
#ifdef TEMPLATES
  char        *templateDir = TtaGetEnvString ("TEMPLATES_DIRECTORY");
  ThotBool     created;

  if (Templates_Dic == NULL)
    InitializeTemplateEnvironment ();
  created = CreateNewTemplateDocDlgWX (BaseDialog + OpenTemplate,
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

  LoadTemplate (0, templatename);
  DontReplaceOldDoc = dontReplace;
  CreateInstance (templatename, docname);
  
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  giveItems : Lists type items from string
  example : "one two three" is extracted to {one, two, three}
  note : item type are setted to SimpleTypeNat
  text : text from which list items
  size : size of text in characters
  items : address of exctracted item list
  nbitems : items number in items list
  ----------------------------------------------------------------------*/
void giveItems (char *text, int size, struct menuType **items, int *nbitems)
{
#ifdef TEMPLATES
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

	menu = (struct menuType*) TtaGetMemory (sizeof (struct menuType)* *nbitems);
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
      menu[i].label = (char *) TtaStrdup (temp);
      menu[i].type = SimpleTypeNat;  /* @@@@@ ???? @@@@@ */
      *items = menu;
    }
#endif /* TEMPLATES */
}

#ifdef TEMPLATES
/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static char *createMenuString (const struct menuType* items, const int nbItems)
{
  char *result, *iter;
	int   size = 0;
  int   i;

	for (i=0; i < nbItems; i++)
		size += 2 + strlen (items[i].label);

	result = (char *) TtaGetMemory (size);
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
  UseToBeCreated
  An new use element will be created by the user through some generic editing
  command
  -----------------------------------------------------------------------*/
ThotBool UseToBeCreated (NotifyElement *event)
{
#ifdef TEMPLATES
  Element        el;
	Document       doc;

  el = event->element;
  doc = event->document;
  /* is there a limit to the number of elements in the xt:repeat ? */
  /* @@@@@ */
#endif /* TEMPLATES */
  return FALSE; /* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
  UseCreated
  A new "use" element has just been created by the user with a generic editing
  command.
  -----------------------------------------------------------------------*/
void UseCreated (NotifyElement *event)
{
#ifdef TEMPLATES
	Document         doc;
	Element          el;
  XTigerTemplate   t;

	doc = event->document;
  el = event->element;
  if (TtaGetFirstChild (el))
    /* this Use element has already some content. It has already been
       instanciated */
    return;
  t = (XTigerTemplate) Dictionary_Get (Templates_Dic, DocumentMeta[doc]->template_url);
  if (!t)
    return; // no template ?!?!
  InstantiateUse (t, el, doc, TRUE);
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  UseButtonClicked
  Shows a menu with all the types that can be used in a use element.
  ----------------------------------------------------------------------*/
ThotBool UseButtonClicked (NotifyElement *event)
{
#ifdef TEMPLATES
	Document         doc;
	Element          el, comp;
	ElementType      elType;
	Attribute        att;
	AttributeType    attributeType;
  XTigerTemplate   t;
  Declaration      dec;
  Record           rec, first;
	int              nbitems, size;
	struct menuType *items;
  char            *types, *menuString;
  View            view;

  TtaGetActiveView (&doc, &view);
  if (view != 1)
    return FALSE; /* let Thot perform normal operation */

	doc = event->document;
  t = (XTigerTemplate) Dictionary_Get (Templates_Dic, DocumentMeta[doc]->template_url);
  if (!t)
    return FALSE; /* let Thot perform normal operation */

	el = event->element;
  if (TtaGetFirstChild (el))
    /* this Use element has already some content. Do not do anything */
    return FALSE; /* let Thot perform normal operation */

	elType = TtaGetElementType (el);
  // give the list of possible items
	attributeType.AttrSSchema = elType.ElSSchema;
	attributeType.AttrTypeNum = Template_ATTR_types;
	att = TtaGetAttribute (el, attributeType);
	size = TtaGetTextAttributeLength (att);
	types = (char *) TtaGetMemory (size+1);	
	TtaGiveTextAttributeValue (att, types, &size);
	giveItems (types, size, &items, &nbitems);
	TtaFreeMemory (types);

  if (nbitems == 1)
    {
      dec = GetDeclaration (t, items[0].label);
      /* if it's a union, display the menu of this union */
      if (dec)
        switch (dec->nature)
          {
          case SimpleTypeNat :
            nbitems = 0;
            break;
          case XmlElementNat :
            nbitems = 0;
            break;
          case ComponentNat :
            nbitems = 0;
            break;
          case UnionNat :
            first = dec->unionType.include->first;
            rec = first;
            /* count the number of elements in the union */
            nbitems = 0;
            while (rec)
              {
                nbitems++;
                rec = rec->next;
              }
            if (nbitems > 0)
              {
                items = (menuType*) TtaGetMemory (sizeof (struct menuType)* nbitems);
                rec = first;
                nbitems = 0;
                while (rec)
                  {
                    items[nbitems].label = (char *) TtaStrdup (rec->key);
                    items[nbitems].type = SimpleTypeNat;  /* @@@@@ ???? @@@@@ */
                    nbitems++;
                    rec = rec->next;
                  }
              }
            break;
          default :
            //Impossible
            break;   
          }
    }
  if (nbitems > 0)
    {
      TtaCancelSelection (doc);
      menuString = createMenuString (items, nbitems);
      TtaNewScrollPopup (BaseDialog + OptionMenu, TtaGetViewFrame (doc, 1),
                         NULL, nbitems, menuString , NULL, false, 'L');
      TtaFreeMemory (menuString);
      ReturnOption = -1; // no selection yet
      TtaShowDialogue (BaseDialog + OptionMenu, FALSE);
      TtaWaitShowProcDialogue ();
      TtaDestroyDialogue (BaseDialog + OptionMenu);
      if (ReturnOption != -1)
        dec = GetDeclaration (t, items[ReturnOption].label);
      TtaFreeMemory (items);
      if (ReturnOption == -1)
        return FALSE;
      if (dec)
        {
          switch (dec->nature)
            {
            case SimpleTypeNat :
              /* @@@@@ */
              break;
            case XmlElementNat :
              /* @@@@@ */
              break;
            case ComponentNat :
              /* copy element dec->componentType.content */
              comp = TtaCopyTree (dec->componentType.content, doc, doc, el);
              TtaInsertFirstChild (&comp, el, doc);
              el = comp;
              /* @@@@@ */
              break;
            case UnionNat :
              /* @@@@@ */
              break;
            default :
              //Impossible
              break;   
            }
        }
    }
  TtaSelectElement (doc, el);
  return TRUE;
#endif /* TEMPLATES */
	return TRUE;
}

/*----------------------------------------------------------------------
  OptionButtonClicked
  ----------------------------------------------------------------------*/
ThotBool OptionButtonClicked (NotifyElement *event)
{
#ifdef TEMPLATES
  Element         child, grandChild, next;
  ElementType     elType, elType1;
  Document        doc;
  XTigerTemplate  t;
  View            view;

  TtaGetActiveView (&doc, &view);
  if (view != 1)
    return FALSE; /* let Thot perform normal operation */
  doc = event->document;
  child = TtaGetFirstChild (event->element);
  if (!child)
    return FALSE; /* let Thot perform normal operation */
  elType = TtaGetElementType (child);
  elType1 = TtaGetElementType (event->element);
  if ((elType.ElTypeNum != Template_EL_useEl &&
       elType.ElTypeNum != Template_EL_useSimple) ||
      elType.ElSSchema != elType1.ElSSchema)
    return FALSE;

  TtaCancelSelection (doc);
  grandChild = TtaGetFirstChild (child);
  if (!grandChild)
    /* the "use" element is empty. Instantiate it */
    {
      t = (XTigerTemplate) Dictionary_Get (Templates_Dic, DocumentMeta[doc]->template_url);
      if (!t)
        return FALSE; // no template ?!?!
      InstantiateUse (t, child, doc, TRUE);
    }
  else
    /* remove the content of the "use" element */
    {
      do
        {
          next = grandChild;
          TtaNextSibling (&next);
          TtaDeleteTree (grandChild, doc);
          grandChild = next;
        }
      while (next);
    }
  TtaSelectElement (doc, event->element);
  return TRUE; /* don't let Thot perform normal operation */
#endif /* TEMPLATES */
	return TRUE;
}

/*----------------------------------------------------------------------
  RepeatButtonClicked
  Shows a menu with all the types that can be used in a use element.
  ----------------------------------------------------------------------*/
ThotBool RepeatButtonClicked (NotifyElement *event)
{
#ifdef TEMPLATES
  XTigerTemplate   t;
	Document         doc;
  Element          el, child, newEl;
  ElementType      elt, elt1;
	int              nbitems, size;
	struct menuType *items;
  char            *types, *menuString;
  ThotBool          oldStructureChecking;
  View            view;

  TtaGetActiveView (&doc, &view);
  if (view != 1)
    return FALSE; /* let Thot perform normal operation */
  doc = event->document;
  t = (XTigerTemplate) Dictionary_Get (Templates_Dic, DocumentMeta[doc]->template_url);
  if (!t)
    return FALSE; // no template ?!?!

  TtaCancelSelection (doc);
	types = "begining end";	
	size = strlen (types);
	giveItems (types, size, &items, &nbitems);
	menuString = createMenuString (items, nbitems);
	TtaNewScrollPopup (BaseDialog + OptionMenu, TtaGetViewFrame (doc, 1), NULL, 
                     nbitems, menuString , NULL, false, 'L');
	TtaFreeMemory (menuString);
  ReturnOption = -1; // no selection yet
	TtaShowDialogue (BaseDialog + OptionMenu, FALSE);
	TtaWaitShowProcDialogue ();
	TtaDestroyDialogue (BaseDialog + OptionMenu);
  TtaFreeMemory (items);
  el = event->element;
  if (ReturnOption == 0 || ReturnOption == 1)
    {
      child = TtaGetFirstChild (el);
      if (child)
        {
          elt = TtaGetElementType (el);
          elt1 = TtaGetElementType (child);
          if (elt.ElSSchema == elt1.ElSSchema)
            {
              if (elt1.ElTypeNum == Template_EL_useEl ||
                  elt1.ElTypeNum == Template_EL_useSimple)
                newEl = InstantiateUse (t, child, doc, FALSE);
              else if (elt1.ElTypeNum == Template_EL_folder)
                newEl = TtaCopyTree (child, doc, doc, el);
              else
                newEl = NULL;
              if (newEl)
                {
                  oldStructureChecking = TtaGetStructureChecking (doc);
                  TtaSetStructureChecking (FALSE, doc);
                  if (ReturnOption == 0)
                    TtaInsertFirstChild (&newEl, el, doc);
                  else
                    {
                      child = TtaGetLastChild (el);
                      TtaInsertSibling (newEl, child, FALSE, doc);
                      el = newEl;
                    }
                  TtaSetStructureChecking (oldStructureChecking, doc);
                }
            }
        }
    }
  TtaSelectElement (doc, el);
  return TRUE; /* don't let Thot perform normal operation */
#endif /* TEMPLATES */
	return TRUE;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void OpeningInstance (char *fileName, Document doc)
{
#ifdef TEMPLATES
  XTigerTemplate   t;
  char            *content, *ptr;
  gzFile           stream;
  char             buffer[2000];
  int              res;

  stream = TtaGZOpen (fileName);
  if (stream != 0)
    {
      res = gzread (stream, buffer, 1999);
      if (res >= 0)
        {
          buffer[res] = EOS;
          ptr = strstr (buffer, "<?xtiger");
          if (ptr)
            ptr = strstr (ptr, "template");
          if (ptr)
            ptr = strstr (ptr, "=");
          if (ptr)
            ptr = strstr (ptr, "\"");
          if (ptr)
            {
              // template URI
              content = &ptr[1];
              ptr = strstr (content, "\"");
            }
          if (ptr)
            {
              *ptr = EOS;
              //Get now the template URI
              DocumentMeta[doc]->template_url = TtaStrdup (content);
              if (Templates_Dic == NULL)
                InitializeTemplateEnvironment ();
              t = (XTigerTemplate) Dictionary_Get (Templates_Dic, content);
              if (!t)
                {
                  LoadTemplate (0, content);
                  t = (XTigerTemplate) Dictionary_Get (Templates_Dic, content);
                }
              AddUser (t);
            }
        }
    }
  TtaGZClose (stream);
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
  if (DocumentMeta[dialog->document] == NULL)
    return FALSE;

  char *turl = DocumentMeta[dialog->document]->template_url;
  if (turl)
    {
      XTigerTemplate t = (XTigerTemplate) Dictionary_Get (Templates_Dic, turl);
      if (t)
        RemoveUser (t);
      TtaFreeMemory (turl);
    }
#endif /* TEMPLATES */
  return FALSE;
}


/*----------------------------------------------------------------------
  GetFirstTemplateParentElement
  Return the first element wich has "Template" as SShema name or null if none.
  ----------------------------------------------------------------------*/
ThotBool IsTemplateElement(Element elem)
{
#ifdef TEMPLATES
  return strcmp(TtaGetSSchemaName(TtaGetElementType(elem).ElSSchema)
                                                    , TEMPLATE_SSHEMA_NAME)==0;
#else
  return FALSE;
#endif /* TEMPLATES */
}


/*----------------------------------------------------------------------
  GetFirstTemplateParentElement
  Return the first element wich has "Template" as SShema name or null if none.
  ----------------------------------------------------------------------*/
Element GetFirstTemplateParentElement(Element elem)
{
#ifdef TEMPLATES
  elem = TtaGetParent(elem);
  while(elem!=NULL && strcmp(TtaGetSSchemaName(TtaGetElementType(elem).ElSSchema)
                                                    , TEMPLATE_SSHEMA_NAME)!=0)
  {
    elem = TtaGetParent(elem);
  }
  return elem;
#else
  return NULL;
#endif /* TEMPLATES */
}
