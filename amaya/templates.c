/*
 *
 *  COPYRIGHT INRIA and W3C, 1996-2007
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

#include "undo.h"

#include "containers.h"
#include "Elemlist.h"
#include "templates.h"


#ifdef TEMPLATES
#include "Template.h"
#include "templateDeclarations.h"
#include "templateUtils_f.h"

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
  return FALSE;
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
static void CopyTemplateRepositoryList (const Prop_Templates_Path** src,
                                        Prop_Templates_Path** dst)
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
  
  printf("UseToBeCreated\n");
  
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
  Template_IncrementRepeatOccurNumber
  Increment the number of occurs of a xt:repeat
  @param el element (xt:repeat)
  ----------------------------------------------------------------------*/
void Template_IncrementRepeatOccurNumber(Element el)
{
#ifdef TEMPLATES
  char* current;
  char  newVal[8];
  int curVal;
  
  current = GetAttributeStringValue(el, Template_ATTR_currentOccurs, NULL);
  curVal = atoi(current);
  curVal++;
  TtaFreeMemory(current);
  sprintf(newVal, "%d", curVal);
  SetAttributeStringValue(el, Template_ATTR_currentOccurs, newVal);
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Template_DecrementRepeatOccurNumber
  Decrement the number of occurs of a xt:repeat
  @param el element (xt:repeat)
  ----------------------------------------------------------------------*/
void Template_DecrementRepeatOccurNumber(Element el)
{
#ifdef TEMPLATES
  char* current;
  char  newVal[8];
  int curVal;
  
  current = GetAttributeStringValue(el, Template_ATTR_currentOccurs, NULL);
  curVal = atoi(current);
  curVal--;
  TtaFreeMemory(current);
  sprintf(newVal, "%d", curVal);
  SetAttributeStringValue(el, Template_ATTR_currentOccurs, newVal);
#endif /* TEMPLATES */
}


/*----------------------------------------------------------------------
  Template_CanInsertRepeatChild
  Test if a xt:repeat child can be inserted (number between params min and max).
  @param el element (xt:repeat) to test
  @return True if an element can be inserted.
  ----------------------------------------------------------------------*/
ThotBool Template_CanInsertRepeatChild(Element el)
{
#ifdef TEMPLATES
  char* max;
  char* current;
  int maxVal, curVal;
  
  max = GetAttributeStringValue(el, Template_ATTR_maxOccurs, NULL);
  if(!strcmp(max, "*")){
    return TRUE;
  }

  current = GetAttributeStringValue(el, Template_ATTR_currentOccurs, NULL);
  curVal = atoi(current);
  maxVal = atoi(max);

  return curVal<maxVal;

#endif /* TEMPLATES */
  return FALSE;
}


/*----------------------------------------------------------------------
  Template_InsertRepeatChildAfter
  Insert a child to a xt:repeat
  The decl parameter must be valid and will not be verified. It must be a
    direct child element or the "use in the use" for union elements.
  @param el element (xt:repeat) in which insert a new element
  @param decl Template declaration of the element to insert
  @param elPrev Element (xt:use) after which insert the new elem, NULL if first.
  @return The inserted element 
  ----------------------------------------------------------------------*/
Element Template_InsertRepeatChildAfter(Document doc, Element el, Declaration decl, Element elPrev)
{
#ifdef TEMPLATES
  Element useFirst; /* First xt:use of the repeat.*/
  Element use;      /* xt:use to insert.*/
  ElementType useType;  /* type of xt:use.*/
  
  /* Copy xt:use with xt:types param */
  useFirst = TtaGetFirstChild(el);
  useType = TtaGetElementType(useFirst);
  use = TtaCopyElement(useFirst, doc, doc, el);

  Template_InsertUseChildren(doc, use, decl);

  /* insert it */
  if(elPrev)
  {
    TtaInsertSibling(use, elPrev, FALSE, doc);
  }
  else
  {
    TtaInsertSibling(use, useFirst, TRUE, doc);
  }

  TtaRegisterElementCreate(use, doc);
  
  Template_IncrementRepeatOccurNumber(el);
  
  return use;
  
#else /* TEMPLATES */
  return NULL;
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Template_InsertRepeatChild
  Insert a child to a xt:repeat
  The decl parameter must be valid and will not be verified. It must be a
    direct child element or the "use in the use" for union elements.
  @param el element (repeat) in which insert a new element
  @param decl Template declaration of the element to insert
  @param pos Position of insertion (0 before all, 1 after first ... -1 after all)
  @return The inserted element
  ----------------------------------------------------------------------*/
Element Template_InsertRepeatChild(Document doc, Element el, Declaration decl, int pos)
{
  if(pos==0)
  {
    return Template_InsertRepeatChildAfter(doc, el, decl, NULL);
  }
  else if(pos==-1)
  {
    return Template_InsertRepeatChildAfter(doc, el, decl, TtaGetLastChild(el));
  }
  else
  {
    Element elem = TtaGetFirstChild(el);
    pos--;
    while(pos>0)
    {
      TtaNextSibling(&elem);
      pos--;
    }
    return Template_InsertRepeatChildAfter(doc, el, decl, elem);
  }
}

#ifdef TEMPLATES
/*----------------------------------------------------------------------
  QueryMenu
  Show a context menu to query a choice.
  @param items space-separated choice list string.
  @return The choosed item 0-based index or -1 if none. 
  ----------------------------------------------------------------------*/
static int QueryMenu(Document doc, char* items)
{
  int nbitems, size;
  struct menuType *itemlist;
  char *menuString;
  
  size = strlen(items);
  giveItems (items, size, &itemlist, &nbitems);
  menuString = createMenuString (itemlist, nbitems);
  TtaNewScrollPopup (BaseDialog + OptionMenu, TtaGetViewFrame (doc, 1), NULL, 
                     nbitems, menuString , NULL, false, 'L');
  TtaFreeMemory (menuString);
  ReturnOption = -1;
  TtaShowDialogue (BaseDialog + OptionMenu, FALSE);
  TtaWaitShowProcDialogue ();
  TtaDestroyDialogue (BaseDialog + OptionMenu);
  TtaFreeMemory (itemlist);
  return ReturnOption;
}

/*----------------------------------------------------------------------
  QueryStringFromMenu
  Show a context menu to query a choice.
  @param items space-separated choice list string.
  @return The choosed item string or NULL if none.
  ----------------------------------------------------------------------*/
static char* QueryStringFromMenu(Document doc, char* items)
{
  int nbitems, size;
  struct menuType *itemlist;
  char *menuString;
  char *result = NULL;
  
  size = strlen(items);
  giveItems (items, size, &itemlist, &nbitems);
  menuString = createMenuString (itemlist, nbitems);
  TtaNewScrollPopup (BaseDialog + OptionMenu, TtaGetViewFrame (doc, 1), NULL, 
                     nbitems, menuString , NULL, false, 'L');
  TtaFreeMemory (menuString);
  ReturnOption = -1;
  TtaShowDialogue (BaseDialog + OptionMenu, FALSE);
  TtaWaitShowProcDialogue ();
  TtaDestroyDialogue (BaseDialog + OptionMenu);
  
  if(ReturnOption!=-1)
  {
    result = TtaStrdup(itemlist[ReturnOption].label);
  }
  
  TtaFreeMemory (itemlist);
  return result;
}
#endif /* TEMPLATES */

/*----------------------------------------------------------------------
  RepeatButtonClicked
  Called when a repeat button is clicked.
  Can be called for useEl, useSimple or repeat.
  If called for useEl or useSimple, the new element must be added after.
  If called for repeat, the element must be added before all.
   
  Shows a menu with all the types that can be used in a use element.
  ----------------------------------------------------------------------*/
ThotBool RepeatButtonClicked (NotifyElement *event)
{
#ifdef TEMPLATES
  Document        doc = event->document;
  Element         el = event->element;
  ElementType     elType;
  XTigerTemplate  t;
  Declaration     decl;
  Element         repeatEl = el;
  Element         firstEl;
  Element         newEl = NULL;
  char*           types;
  ThotBool        oldStructureChecking;
  View            view;
  
  TtaGetActiveView (&doc, &view);
  if (view != 1)
    return FALSE; /* let Thot perform normal operation */

  TtaCancelSelection(doc);
  
  t = (XTigerTemplate) Dictionary_Get (Templates_Dic, DocumentMeta[doc]->template_url);
  elType = TtaGetElementType(el);
  while(elType.ElTypeNum!=Template_EL_repeat)
  {
    repeatEl = TtaGetParent(repeatEl);
    if(repeatEl==NULL)
      break;
    elType = TtaGetElementType(repeatEl);
  }
  if(repeatEl)
  {
    if(Template_CanInsertRepeatChild(repeatEl))
    {
      firstEl = TtaGetFirstChild(repeatEl);
      types = GetAttributeStringValue(firstEl, Template_ATTR_types, NULL);
      
      char* listtypes = Template_ExpandTypes(t, types);
      char* result = QueryStringFromMenu(doc, listtypes);
      if(result)
      {
        decl = GetDeclaration(t, result);
        if(decl)
        {
          /* Prepare insertion.*/          
          oldStructureChecking = TtaGetStructureChecking (doc);
          TtaSetStructureChecking (FALSE, doc);
          TtaOpenUndoSequence(doc, NULL, NULL, 0, 0);
          
          /* Insert. */
          if(el==repeatEl)
            newEl = Template_InsertRepeatChildAfter(doc, repeatEl, decl, NULL);
          else
            newEl = Template_InsertRepeatChildAfter(doc, repeatEl, decl, el);
            
          /* Finish insertion.*/
          TtaCloseUndoSequence(doc);
          TtaSetStructureChecking (oldStructureChecking, doc);
          
          firstEl = GetFirstEditableElement(newEl);
          if(firstEl)
          {
            TtaSelectElement (doc, firstEl);
            TtaSetStatusSelectedElement(doc, view, firstEl);
          }
          else
          {
            TtaSelectElement (doc, newEl);
            TtaSetStatusSelectedElement(doc, view, newEl);
          }
        }
      }
      TtaFreeMemory(listtypes);
      TtaFreeMemory(result);
    }
    else /* if(Template_CanInsertRepeatChild(repeatEl)) */
    {
      TtaSetStatus(doc, view, TtaGetMessage (AMAYA, AM_NUMBER_OCCUR_HAVE_MAX), NULL);
    }
  }
  return TRUE; /* don't let Thot perform normal operation */
#endif /* TEMPLATES */
  return TRUE;
}

/*----------------------------------------------------------------------
  UseButtonClicked
  Shows a menu with all the types that can be used in a use element.
  ----------------------------------------------------------------------*/
ThotBool UseButtonClicked (NotifyElement *event)
{
#ifdef TEMPLATES
  Document        doc = event->document;
  Element         el = event->element;
  Element         child;
  ElementType     elType;
  XTigerTemplate  t;
  Declaration     decl;
  Element         firstEl;
  Element         newEl = NULL;
  char*           types;
  ThotBool        oldStructureChecking;
  View            view;

  TtaGetActiveView (&doc, &view);
  if (view != 1)
    return FALSE; /* let Thot perform normal operation */
  
  TtaCancelSelection(doc);
  
  t = (XTigerTemplate) Dictionary_Get (Templates_Dic, DocumentMeta[doc]->template_url);
  if (!t)
    return FALSE; /* let Thot perform normal operation */
  elType = TtaGetElementType(el);

  firstEl = TtaGetFirstChild(el);
  if(firstEl)
  {
    RepeatButtonClicked(event);
  }
  else
  {
    types = GetAttributeStringValue(el, Template_ATTR_types, NULL);

    char* listtypes = Template_ExpandTypes(t, types);
    char* result = QueryStringFromMenu(doc, listtypes);
    if(result)
    {
      decl = GetDeclaration(t, result);
      if(decl)
      {
        /* Prepare insertion.*/
        oldStructureChecking = TtaGetStructureChecking (doc);
        TtaSetStructureChecking (FALSE, doc);
        TtaOpenUndoSequence(doc, NULL, NULL, 0, 0);
        
        /* Insert */
        newEl = Template_InsertUseChildren(doc, el, decl);
        
        for(child = TtaGetFirstChild(newEl); child; TtaNextSibling(&child))
        {
          TtaRegisterElementCreate(child, doc);
        }
        
        /* Finish insertion. */
        TtaCloseUndoSequence(doc);
        TtaSetStructureChecking (oldStructureChecking, doc);
        
        firstEl = GetFirstEditableElement(newEl);
        if(firstEl)
        {
          TtaSelectElement (doc, firstEl);
          TtaSetStatusSelectedElement(doc, view, firstEl);
        }
        else
        {
          TtaSelectElement (doc, newEl);
          TtaSetStatusSelectedElement(doc, view, newEl);
        }
      }
    }
    TtaFreeMemory(listtypes);
    TtaFreeMemory(result);
  }
  
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

/*----------------------------------------------------------------------
  NonTemplateElementWillBeCreated
  Processed when an element will be created in a template context.
  ----------------------------------------------------------------------*/
ThotBool NonTemplateElementWillBeCreated (NotifyElement *event)
{
  printf("NonTemplateElementWillBeCreated : ");
#ifdef TEMPLATES  
  Element     elem   = event->element;
  ElementType elType = TtaGetElementType(elem);
  Element     parent = TtaGetParent(elem);
  ElementType parentType   = TtaGetElementType(parent);
  Element     gdparent     = TtaGetParent(parent);
  ElementType gdparentType = TtaGetElementType(gdparent);
  
 
  printf("%s %s/", TtaGetSSchemaName(gdparentType.ElSSchema), TtaGetElementTypeName(gdparentType));
  printf("%s %s/", TtaGetSSchemaName(parentType.ElSSchema), TtaGetElementTypeName(parentType));
  printf("%s %s", TtaGetSSchemaName(elType.ElSSchema), TtaGetElementTypeName(elType));
    
  printf("\n");

//  if(parentType.ElTypeNum==Template_EL_bag)
//  {
//    printf("Do it !\n");
//    return TRUE;
//  }
//  
//  return FALSE;
  return TRUE;
#else /* TEMPLATES */
  return FALSE;  
#endif /* TEMPLATES*/
}

/*----------------------------------------------------------------------
  TemplateElementWillBeDeleted
  Processed when an element will be deleted in a template context.
  ----------------------------------------------------------------------*/
ThotBool TemplateElementWillBeDeleted (NotifyElement *event)
{
  printf("TemplateElementWillBeDeleted\n");
  return FALSE;
}


