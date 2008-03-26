/*
 *
 *  COPYRIGHT INRIA and W3C, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * Authors: Francesc Campoy Flores
 *          Émilien Kia
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

#include "html2thot_f.h"
#include "templates_f.h"
#include "templateUtils_f.h"
#include "templateLoad_f.h"
#include "templateDeclarations_f.h"
#include "templateInstantiate_f.h"
#include "Templatebuilder_f.h"
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
ThotBool IsTemplateInstanceDocument(Document doc)
{
#ifdef TEMPLATES
  XTigerTemplate t = GetXTigerDocTemplate(doc);
  if(t)
    return (t->state&templInstance)!=0;
  else
    return FALSE;
#else /* TEMPLATES */
  return FALSE;
#endif /* TEMPLATES */ 
}

/*----------------------------------------------------------------------
  IsTemplateDocument: Test if a document is a template (not an instance)
  doc : Document to test
  return : TRUE if the document is a template
  ----------------------------------------------------------------------*/
ThotBool IsTemplateDocument(Document doc)
{
#ifdef TEMPLATES
  XTigerTemplate t = GetXTigerDocTemplate(doc);
  if(t)
    return (t->state&templTemplate)!=0;
  else
    return FALSE;
#else /* TEMPLATES */
  return FALSE;
#endif /* TEMPLATES */ 
}


/*----------------------------------------------------------------------
  Test if a document is an internal template.
  (no instance is opened and it is not edited)
  ----------------------------------------------------------------------*/
ThotBool IsInternalTemplateDocument(Document doc)
{
#ifdef TEMPLATES
  XTigerTemplate t = GetXTigerDocTemplate(doc);
  if(t)
    return (t->state&templInternal)!=0;
  else
    return FALSE;
#else /* TEMPLATES */
  return FALSE;
#endif /* TEMPLATES */ 
}

/*----------------------------------------------------------------------
  Return the URL of an instance template.
  ----------------------------------------------------------------------*/
char* GetDocumentInstanceTemplateUrl(Document doc)
{
#ifdef TEMPLATES
  XTigerTemplate t = GetXTigerDocTemplate(doc);
  if(t)
    return t->base_uri;
  else
    return FALSE;
#else /* TEMPLATES */
  return NULL;
#endif /* TEMPLATES */ 
}

/*----------------------------------------------------------------------
  CheckPromptIndicator checks if the element is a prompt text unit
  ----------------------------------------------------------------------*/
ThotBool CheckPromptIndicator (Element el, Document doc)
{
#ifdef TEMPLATES
  ElementType     elType;
  Element         parent;
  AttributeType   attrType;
  Attribute       att;
  SSchema         templateSSchema;

  elType = TtaGetElementType (el);
  templateSSchema = TtaGetSSchema ("Template", doc);
	if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
    {
      parent = TtaGetParent (el);
      elType = TtaGetElementType (parent);
      while (parent && elType.ElSSchema != templateSSchema)
        {
          parent = TtaGetParent (parent);
          elType = TtaGetElementType (parent);
        }
      if (parent &&
          (elType.ElTypeNum == Template_EL_useEl ||
           elType.ElTypeNum == Template_EL_useSimple))
        {
          // there is a parent template use
          attrType.AttrSSchema = elType.ElSSchema;
          attrType.AttrTypeNum = Template_ATTR_prompt;
          att = TtaGetAttribute (parent, attrType);
          if (att)
            {
              TtaSelectElement (doc, el);
              return TRUE;
            }
        }
    }
#endif /* TEMPLATES */
  return FALSE;
}

/*----------------------------------------------------------------------
  RemovePromptIndicator removes the enclosing prompt indicator
  ----------------------------------------------------------------------*/
ThotBool RemovePromptIndicator (NotifyOnTarget *event)
{
#ifdef TEMPLATES
  ElementType     elType;
  Element         parent, el;
  AttributeType   attrType;
  Attribute       att;
  Document        doc;
  SSchema         templateSSchema;

  el = event->element;
  doc = event->document;
  elType = TtaGetElementType (el);
  templateSSchema = TtaGetSSchema ("Template", doc);
  parent = TtaGetParent (el);
  elType = TtaGetElementType (parent);
  while (parent && elType.ElSSchema != templateSSchema)
    {
      parent = TtaGetParent (parent);
      elType = TtaGetElementType (parent);
    }
  if (parent &&
      (elType.ElTypeNum == Template_EL_useEl ||
       elType.ElTypeNum == Template_EL_useSimple))
    {
      // there is a parent template use
      attrType.AttrSSchema = elType.ElSSchema;
      attrType.AttrTypeNum = Template_ATTR_prompt;
      att = TtaGetAttribute (parent, attrType);
      if (att)
        {
          TtaRegisterAttributeDelete (att, parent, doc);
          TtaRemoveAttribute (parent, att, doc);
        }
    }
#endif /* TEMPLATES */
  return FALSE;		/* let Thot perform normal operation */
}


/*----------------------------------------------------------------------
  AllocTemplateRepositoryListElement: allocates an element for the list
  of template repositories.
  path : path of the new element
  return : address of the new element
  ----------------------------------------------------------------------*/
void* AllocTemplateRepositoryListElement (const char* path, void* prevElement)
{
  Prop_Templates_Path *element;

  element  = (Prop_Templates_Path*)TtaGetMemory (sizeof(Prop_Templates_Path));
  memset (element, 0, sizeof(Prop_Templates_Path));
  strncpy (element->Path, path, MAX_LENGTH - 1);
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
  Prop_Templates_Path **l = (Prop_Templates_Path**) list;
  Prop_Templates_Path  *element = *l;

  l = NULL;
  while (element)
  {
    Prop_Templates_Path* next = element->NextPath;
    TtaFreeMemory (element);
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
  
  if (*src)
  {
    *dst = (Prop_Templates_Path*) TtaGetMemory (sizeof(Prop_Templates_Path));
    (*dst)->NextPath = NULL;
    strcpy((*dst)->Path, (*src)->Path);
    
    element = (*src)->NextPath;
    current = *dst;
  }

  while (element)
    {
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
  
  //clean up the curent list
  FreeTemplateRepositoryList (list);

  // open the file
  path = (char *) TtaGetMemory (MAX_LENGTH);
  homePath       = TtaGetEnvString ("APP_HOME");
  sprintf (path, "%s%ctemplates.dat", homePath, DIR_SEP);
  file = TtaReadOpen ((char *)path);
  if (!file)
  {
    /* The config file dont exist, create it. */
    file = TtaWriteOpen ((char *)path);
    fprintf (file, "http://www.w3.org/Amaya/Templates/cv.xtd\n");
    fprintf (file, "http://www.w3.org/Amaya/Templates/slides.xtd\n");
    fprintf (file, "http://www.w3.org/Amaya/Templates/ACM-Proc-Article.xtd\n");
    TtaWriteClose (file);
    /* Retry to open it.*/
    file = TtaReadOpen ((char *)path);
  }
  
  if (file)
    {
      // read the file
      c = (unsigned char*)path;
      *c = EOS;
      while (TtaReadByte (file, c))
        {
          if (*c == 13 || *c == EOL)
            *c = EOS;
          if (*c == EOS && c != (unsigned char*)path )
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
      if (c != (unsigned char*)path && *path != EOS)
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
  sprintf (path, "%s%ctemplates.dat", homePath, DIR_SEP);

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
  LoadTemplateRepositoryList (&TemplateRepositoryPaths);
}


/*----------------------------------------------------------------------
  Load a template and create the instance file - update images and 
  stylesheets related to the template.
  ----------------------------------------------------------------------*/
void CreateInstanceOfTemplate (Document doc, char *templatename, char *docname)
{
#ifdef TEMPLATES
  DocumentType docType;
  int          len, i;
  char        *s;
  char  	     suffix[6];
  ThotBool     dontReplace = DontReplaceOldDoc;

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
  docType = LoadTemplate (0, templatename);
  if (docType != docFree)
    {
      /* check if the file suffix is conform to the document type */
      s = (char *)TtaGetMemory (strlen (docname) + 10);
      strcpy (s, docname);
      if (!IsXMLName (docname))
        {
          // by default no suffix is added
          suffix[0] = EOS;
          if (IsMathMLName (docname) && docType != docMath)
            strcpy (suffix, "mml");
          else if (IsSVGName (docname) && docType != docSVG)
            strcpy (suffix, "svg");
          else if (IsHTMLName (docname) && docType != docHTML)
            strcpy (suffix, "xml");
          if (suffix[0] != EOS)
            {
              // change or update the suffix
              len = strlen (s);
              for (i = len-1; i > 0 && s[i] != '.'; i--);
              if (s[i] != '.')
                {
                  /* there is no suffix */
                  s[i++] = '.';
                  strcpy (&s[i], suffix);
                }
              else
                {
                  /* there is a suffix */
                  i++;
                  strcpy (&s[i], suffix);
                }
            }
        }
      // now create the instance
      DontReplaceOldDoc = dontReplace;
      CreateInstance (templatename, s, doc);
      TtaFreeMemory (s);
    }
#endif /* TEMPLATES */
}



/*----------------------------------------------------------------------
  PreventReloadingTemplate
  Prevent reloading a template.
  You must call AllowReloadingTemplate when finish.
  Usefull for reload an instance without reloading the template.
  ----------------------------------------------------------------------*/
void PreventReloadingTemplate(char* template_url)
{
#ifdef TEMPLATES
  XTigerTemplate t = GetXTigerTemplate (template_url);
  if (t)
    t->users++;
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  AllowReloadingTemplate
  Allow reloading a template.
  You must call it after each PreventReloadingTemplate call.
  ----------------------------------------------------------------------*/
void AllowReloadingTemplate(char* template_url)
{
#ifdef TEMPLATES
  XTigerTemplate t = GetXTigerTemplate (template_url);
  if (t)
    t->users--;  
#endif /* TEMPLATES */  
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
ThotBool isEOSorWhiteSpace (const char c)
{
  return c == SPACE || c == TAB || c ==  EOL || c ==__CR__ || c == EOS;
}
ThotBool isWhiteSpace (const char c)
{
  return c == SPACE || c == TAB || c == EOL || c ==__CR__;
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
      while (isWhiteSpace (*iter))
        iter++;
      if (*iter != EOS)
        {
          while (!isEOSorWhiteSpace (*iter))
            {
              temp[labelSize++] = *iter;
              iter++;
            }

          temp[labelSize] = EOS;
          menu[i].label = (char *) TtaStrdup (temp);
          menu[i].type = SimpleTypeNat;  /* @@@@@ ???? @@@@@ */
        }
    }
  *items = menu;
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
  ElementType   parentType;
  SSchema       templateSSchema = TtaGetSSchema ("Template", event->document);  
  if (templateSSchema)
  {
    parentType = TtaGetElementType (event->element);
    if (parentType.ElSSchema == templateSSchema &&
        parentType.ElTypeNum == Template_EL_repeat)
      return !Template_CanInsertRepeatChild (event->element);
    return TemplateElementWillBeCreated (event);
  }
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
  Document        doc = event->document;
  Element         el = event->element;
  Element         parent;
  Element         first;
  ElementType     parentType;
  XTigerTemplate  t;
  SSchema         templateSSchema;
  char*           types, *text = NULL;

  if (!TtaGetDocumentAccessMode(doc))
    return;

  if (TtaGetFirstChild (el))
    /* this Use element has already some content. It has already been
       instanciated */
    return;

  t = GetXTigerDocTemplate (doc);
  if (!t)
    return; // no template ?!?!

  templateSSchema = TtaGetSSchema ("Template", doc);
  parent = TtaGetParent(el);
  parentType = TtaGetElementType(parent);

  if (parentType.ElSSchema == templateSSchema &&
      parentType.ElTypeNum == Template_EL_repeat)
  {
    first = TtaGetFirstChild (parent);
    if (first)
    {
      types = GetAttributeStringValueFromNum (first, Template_ATTR_types, NULL);
      if (types)
      {
        SetAttributeStringValueWithUndo (el, Template_ATTR_types, types);
        text = GetAttributeStringValueFromNum (el, Template_ATTR_title, NULL);
        SetAttributeStringValueWithUndo (first, Template_ATTR_title, text);
        TtaFreeMemory (text);
        text = GetAttributeStringValueFromNum (el, Template_ATTR_currentType, NULL);
        SetAttributeStringValueWithUndo (first, Template_ATTR_currentType, text);
        TtaFreeMemory (text);
        TtaFreeMemory (types);
      }
    }
  }

  InstantiateUse (t, el, doc, TRUE);
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
  char     *max;
  int       maxVal, curVal;
  Element   child;

  max = GetAttributeStringValueFromNum(el, Template_ATTR_maxOccurs, NULL);
  if (max)
    {
      if (!strcmp(max, "*"))
        {
          TtaFreeMemory(max);
          return TRUE;
        }
      maxVal = atoi (max);
      TtaFreeMemory (max);
      curVal = 0;
      for (child = TtaGetFirstChild(el); child; TtaNextSibling(&child))
        curVal++;
      return (curVal < maxVal);
    }
  else
    return TRUE;
#endif /* TEMPLATES */
  return FALSE;
}


#ifdef TEMPLATES
/*----------------------------------------------------------------------
  QueryStringFromMenu
  Show a context menu to query a choice.
  @param items space-separated choice list string.
  @return The choosed item string or NULL if none.
  ----------------------------------------------------------------------*/
static char* QueryStringFromMenu (Document doc, char* items)
{
  int nbitems, size;
  struct menuType *itemlist;
  char *menuString;
  char *result = NULL;

  if (!TtaGetDocumentAccessMode (doc))
    return NULL;
  if (items == NULL)
    return NULL;
  size = strlen (items);
  if (size == 0)
    return NULL;
  giveItems (items, size, &itemlist, &nbitems);
  menuString = createMenuString (itemlist, nbitems);
  TtaNewScrollPopup (BaseDialog + OptionMenu, TtaGetViewFrame (doc, 1), NULL, 
                     nbitems, menuString , NULL, false, 'L');
  TtaFreeMemory (menuString);
  ReturnOption = -1;
  TtaShowDialogue (BaseDialog + OptionMenu, FALSE);
  TtaWaitShowProcDialogue ();
  TtaDestroyDialogue (BaseDialog + OptionMenu);

  if (ReturnOption!=-1)
  {
    result = TtaStrdup(itemlist[ReturnOption].label);
  }
  
  TtaFreeMemory (itemlist);
  return result;
}
#endif /* TEMPLATES */

#ifdef AMAYA_DEBUG
void FillInsertableElemList (Document doc, Element elem, DLList list);
#endif /* AMAYA_DEBUG */
/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
char *Template_GetListTypes (XTigerTemplate t, Element el)
{
#ifdef TEMPLATES
  char  *listtypes = NULL, *types;

  types = GetAttributeStringValueFromNum (el, Template_ATTR_types, NULL);
  if (types)
    listtypes = Template_ExpandTypes (t, types, el, FALSE);
  return listtypes;
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  BagButtonClicked
  Called when a bag button is clicked.
  Can be called for useEl, useSimple or bag.
  If called for useEl or useSimple, the new element must be added after.
  If called for bag, the element must be added before all.
   
  Shows a menu with all the types that can be used in the bag.
  ----------------------------------------------------------------------*/
ThotBool BagButtonClicked (NotifyElement *event)
{
#ifdef TEMPLATES
  Document        doc = event->document;
  Element         el = event->element;
  ElementType     elType;
  XTigerTemplate  t;
  Declaration     decl;
  Element         bagEl = el;
  Element         firstEl;
  Element         newEl = NULL;
  View            view;
  SSchema         templateSSchema;
  char           *listtypes = NULL;
  char           *result = NULL;
  ThotBool        oldStructureChecking;
  DisplayMode     dispMode;

  if (!TtaGetDocumentAccessMode(doc))
    return TRUE;

  TtaGetActiveView (&doc, &view);
  if (view != 1)
    return FALSE; /* let Thot perform normal operation */

  TtaCancelSelection (doc);
  templateSSchema = TtaGetSSchema ("Template", doc);  
  t = GetXTigerDocTemplate(doc);
  elType = TtaGetElementType (el);
  while (bagEl &&
         (elType.ElSSchema != templateSSchema ||
          elType.ElTypeNum != Template_EL_bag))
    {
      bagEl = TtaGetParent (bagEl);
      elType = TtaGetElementType (bagEl);
    }

  if (bagEl)
  {
    listtypes = Template_GetListTypes (t, bagEl);
    if (listtypes)
      {
#ifdef AMAYA_DEBUG
      printf("BagButtonClicked : \n  > %s\n", listtypes);
//      {
//        DLList list = DLList_Create();
//        FillInsertableElemList (doc, TtaGetFirstChild(bagEl), list);
//        DLListNode node;
//        ForwardIterator iter = DLList_GetForwardIterator(list);
//        ITERATOR_FOREACH(iter, DLListNode, node)
//        {
//          ElemListElement elem = (ElemListElement)node->elem;
//          printf("  + %s\n", ElemListElement_GetName(elem));
//        }
//        DLList_Destroy(list);
//      }
#endif /* AMAYA_DEBUG */
        result = QueryStringFromMenu (doc, listtypes);
        TtaFreeMemory (listtypes);
        if (result)
          {
            decl = Template_GetDeclaration (t, result);
            if (decl)
              {
                dispMode = TtaGetDisplayMode (doc);
                if (dispMode == DisplayImmediately)
                  /* don't set NoComputedDisplay
                     -> it breaks down views formatting when Enter generates new elements  */
                  TtaSetDisplayMode (doc, DeferredDisplay);

                /* Prepare insertion.*/          
                oldStructureChecking = TtaGetStructureChecking (doc);
                TtaSetStructureChecking (FALSE, doc);
                TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);

                /* Insert */
                if (el == bagEl)
                  {
                    el = TtaGetFirstChild (el);
                    TtaSelectElement (doc, el);
                    TtaInsertAnyElement (doc, TRUE);
                  }
                else
                  {
                    TtaSelectElement (doc, el);
                    TtaInsertAnyElement (doc, FALSE);
                  }
                newEl = Template_InsertBagChild (doc, bagEl, decl, FALSE);

                /* Finish insertion.*/
                TtaCloseUndoSequence (doc);
                TtaSetDocumentModified (doc);
                TtaSetStructureChecking (oldStructureChecking, doc);
                // restore the display
                TtaSetDisplayMode (doc, dispMode);
                firstEl = GetFirstEditableElement (newEl);
                if (firstEl)
                  {
                    TtaSelectElement (doc, firstEl);
                    TtaSetStatusSelectedElement (doc, view, firstEl);
                  }
                else
                  {
                    TtaSelectElement (doc, newEl);
                    TtaSetStatusSelectedElement (doc, view, newEl);
                  }
              }
          }
      }
    TtaFreeMemory (result);
  }
#endif /* TEMPLATES */
  return TRUE; /* don't let Thot perform normal operation */
}

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
  View            view;
  char           *listtypes = NULL;
  char           *result = NULL;
  ThotBool        oldStructureChecking;
  DisplayMode     dispMode;

  if (!TtaGetDocumentAccessMode(doc))
    return TRUE;

  TtaGetActiveView (&doc, &view);
  if (view != 1)
    return FALSE; /* let Thot perform normal operation */

  TtaCancelSelection(doc);

  t = GetXTigerDocTemplate(doc);
  elType = TtaGetElementType(el);
  while (elType.ElTypeNum != Template_EL_repeat)
  {
    repeatEl = TtaGetParent(repeatEl);
    if (repeatEl == NULL)
      break;
    elType = TtaGetElementType(repeatEl);
  }
  if (repeatEl)
  {
    if (Template_CanInsertRepeatChild (repeatEl))
    {
      firstEl = TtaGetFirstChild (repeatEl);
    listtypes = Template_GetListTypes (t, firstEl);
    if (listtypes)
      {
#ifdef AMAYA_DEBUG
      printf("RepeatButtonClicked : \n  > %s\n", listtypes);
#endif /* AMAYA_DEBUG */
        
        result = QueryStringFromMenu (doc, listtypes);
        TtaFreeMemory (listtypes);
        if (result)
        {
          decl = Template_GetDeclaration (t, result);
          if (decl)
          {
            dispMode = TtaGetDisplayMode (doc);
            if (dispMode == DisplayImmediately)
              /* don't set NoComputedDisplay
                 -> it breaks down views formatting when Enter generates new elements  */
              TtaSetDisplayMode (doc, DeferredDisplay);

            /* Prepare insertion.*/          
            oldStructureChecking = TtaGetStructureChecking (doc);
            TtaSetStructureChecking (FALSE, doc);
            TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
            /* Insert. */
            if (el == repeatEl)
              newEl = Template_InsertRepeatChildAfter (doc, repeatEl, decl, NULL);
            else
              newEl = Template_InsertRepeatChildAfter (doc, repeatEl, decl, el);
              
            /* Finish insertion.*/
            TtaCloseUndoSequence(doc);
            TtaSetDocumentModified (doc);
            TtaSetStructureChecking (oldStructureChecking, doc);

            // restore the display
            TtaSetDisplayMode (doc, dispMode);
            firstEl = GetFirstEditableElement (newEl);
            if (firstEl)
            {
              TtaSelectElement (doc, firstEl);
              TtaSetStatusSelectedElement (doc, view, firstEl);
            }
            else
            {
              TtaSelectElement (doc, newEl);
              TtaSetStatusSelectedElement (doc, view, newEl);
            }
          }
        }
      }
      TtaFreeMemory(result);
    }
    else /* if (Template_CanInsertRepeatChild(repeatEl)) */
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
  char*           listtypes = NULL;
  char*           result = NULL;

  if (!TtaGetDocumentAccessMode(doc))
    return TRUE;
    
  TtaGetActiveView (&doc, &view);
  if (view != 1)
    return FALSE; /* let Thot perform normal operation */

  TtaCancelSelection(doc);
  
  t = GetXTigerDocTemplate(doc);
  if (!t)
    return FALSE; /* let Thot perform normal operation */
  elType = TtaGetElementType(el);

  firstEl = TtaGetFirstChild(el);
  if (firstEl)
  {
    RepeatButtonClicked(event);
  }
  else
  {
    types = GetAttributeStringValueFromNum(el, Template_ATTR_types, NULL);
    if (types)
    {
      listtypes = Template_ExpandTypes(t, types, NULL, FALSE);
#ifdef AMAYA_DEBUG
      printf("UseButtonClicked : \n  > %s\n", listtypes);
#endif /* AMAYA_DEBUG */
      
      result = QueryStringFromMenu(doc, listtypes);
      if (result)
      {
        decl = Template_GetDeclaration(t, result);
        if (decl)
        {
          /* Prepare insertion.*/
          oldStructureChecking = TtaGetStructureChecking (doc);
          TtaSetStructureChecking (FALSE, doc);
          TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
          
          /* Insert */
          newEl = Template_InsertUseChildren(doc, el, decl);
          for(child = TtaGetFirstChild(newEl); child; TtaNextSibling(&child))
          {
            TtaRegisterElementCreate(child, doc);
          }
          
          TtaChangeTypeOfElement(el, doc, Template_EL_useSimple);
          TtaRegisterElementTypeChange(el, Template_EL_useEl, doc);
          
          /* xt:currentType attribute.*/
          SetAttributeStringValueWithUndo(el, Template_ATTR_currentType, result);
          
          /* Finish insertion. */
          TtaCloseUndoSequence(doc);
          TtaSetDocumentModified (doc);
          TtaSetStructureChecking (oldStructureChecking, doc);
          
          firstEl = GetFirstEditableElement(newEl);
          if (firstEl)
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
    }
    TtaFreeMemory(types);
    TtaFreeMemory(listtypes);
    TtaFreeMemory(result);
  }

  return TRUE;
#endif /* TEMPLATES */
	return TRUE;
}


/*----------------------------------------------------------------------
  UseSimpleButtonClicked
  ----------------------------------------------------------------------*/
ThotBool UseSimpleButtonClicked (NotifyElement *event)
{
#ifdef TEMPLATES
  if (!TtaGetDocumentAccessMode(event->document))
    return TRUE;

  ElementType parentType = TtaGetElementType (TtaGetParent( event->element));
  if (parentType.ElTypeNum == Template_EL_repeat)
    return RepeatButtonClicked (event);
  else if (parentType.ElTypeNum == Template_EL_bag)
    return BagButtonClicked (event);
#endif /* TEMPLATES */  
  return FALSE;
}

/*----------------------------------------------------------------------
  OptionButtonClicked
  ----------------------------------------------------------------------*/
ThotBool OptionButtonClicked (NotifyElement *event)
{
#ifdef TEMPLATES
  Element         useEl, contentEl, next;
  ElementType     useType, optType;
  Document        doc;
  XTigerTemplate  t;
  View            view;

  if (!TtaGetDocumentAccessMode(event->document))
    return TRUE;

  TtaGetActiveView (&doc, &view);
  if (view != 1)
    return FALSE; /* let Thot perform normal operation */

  doc = event->document;
  useEl = TtaGetFirstChild (event->element);
  if (!useEl)
    return FALSE; /* let Thot perform normal operation */
  useType = TtaGetElementType (useEl);
  optType = TtaGetElementType (event->element);
  if ((useType.ElTypeNum != Template_EL_useEl &&
      useType.ElTypeNum != Template_EL_useSimple) ||
      useType.ElSSchema != optType.ElSSchema)
    return FALSE;

  TtaOpenUndoSequence(doc, NULL, NULL, 0, 0);

  TtaCancelSelection (doc);

  contentEl = TtaGetFirstChild (useEl);
  if (!contentEl)
    /* the "use" element is empty. Instantiate it */
    {
      t = GetXTigerDocTemplate (doc);
      if (!t)
        return FALSE; // no template ?!?!
      InstantiateUse (t, useEl, doc, TRUE);
    }
  else
    /* remove the content of the "use" element */
    {
      do
        {
          next = contentEl;
          TtaNextSibling (&next);
          TtaRegisterElementDelete(contentEl, doc);
          TtaDeleteTree (contentEl, doc);
          contentEl = next;
        }
      while (next);
      if (NeedAMenu (useEl, doc))
        {
          TtaChangeTypeOfElement (useEl, doc, Template_EL_useEl);
          TtaRegisterElementTypeChange(useEl, Template_EL_useSimple, doc);
        }
    }
  TtaSelectElement (doc, event->element);
  TtaCloseUndoSequence(doc);
  return TRUE; /* don't let Thot perform normal operation */
#endif /* TEMPLATES */
  return TRUE;
}

/*----------------------------------------------------------------------
  CheckTemplate checks if the template of the instance is loaded
  Return TRUE if the template is loaded
  ----------------------------------------------------------------------*/
void CheckTemplate (Document doc)
{
#ifdef TEMPLATES
  Element    root;
  
  if(IsTemplateInstanceDocument(doc))
    {
      XTigerTemplate   t;

      root = TtaGetRootElement (doc);
      TtaSetAccessRight (root, ReadOnly, doc);
      t = GetXTigerDocTemplate (doc);
      if (t == NULL)
        {
          // the template cannot be loaded
          InitConfirm (doc, 1, TtaGetMessage (AMAYA, AM_BAD_TEMPLATE));
          TtaSetDocumentAccessMode (doc, 0); // document readonly
        }
      else
        {
          // fix all access rights in the instance
          Template_PrepareTemplate(t);
          Template_FixAccessRight (t, root, doc);
          TtaUpdateAccessRightInViews (doc, root);
        }
    }
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  OpeningInstance checks if it is a template instance needs.
  If it's an instance and the template is not loaded, load it into a
  temporary file
  ----------------------------------------------------------------------*/
void OpeningInstance (char *localFileName, Document doc, char* docURL)
{
#ifdef TEMPLATES
  XTigerTemplate   t;
  char            *content, *ptr, *begin;
  gzFile           stream;
  char             buffer[2000];
  int              res;
  char            *template_version = NULL,
                  *template_url = NULL;

  stream = TtaGZOpen (localFileName);
  if (stream != 0)
    {
      res = gzread (stream, buffer, 1999);
      if (res >= 0)
        {
          buffer[res] = EOS;
          begin = strstr (buffer, "<?xtiger");
          
          if (begin)
          {
            // Search for template version
            ptr = strstr (begin, "templateVersion");
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
                template_version = TtaStrdup (content);
                *ptr = '"';
              }
           
            // Search for template uri
            ptr = strstr (begin, "template");
            if (ptr && ptr[8] != 'V')
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
                template_url = TtaStrdup (content);

                t = GetXTigerTemplate (template_url);
                if (!t)
                  {
                    LoadTemplate (doc, template_url);
                    t = GetXTigerTemplate(template_url);
                  }
                Template_PrepareInstance(docURL, doc, template_version, template_url);
                template_version = NULL;
                template_url     = NULL;
                Template_AddReference (t);
                *ptr = '"';
              }
          }
        }
    }
  TtaFreeMemory(template_version);
  TtaFreeMemory(template_url);
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
  XTigerTemplate t = GetXTigerDocTemplate(dialog->document);
  if(t)
    Template_RemoveReference(t);
#endif /* TEMPLATES */
  return FALSE;
}

/*----------------------------------------------------------------------
  IsTemplateElement
  Test if an element is a template element.
  ----------------------------------------------------------------------*/
ThotBool IsTemplateElement (Element elem)
{
#ifdef TEMPLATES
  ElementType     elType;

  elType = TtaGetElementType(elem);
  if (elType.ElSSchema)
    return (strcmp(TtaGetSSchemaName(elType.ElSSchema) , "Template") == 0);
#endif /* TEMPLATES */
  return FALSE;
}


/*----------------------------------------------------------------------
  GetFirstTemplateParentElement
  Return the first element which has "Template" as schema name or null.
  ----------------------------------------------------------------------*/
Element GetFirstTemplateParentElement(Element elem)
{
#ifdef TEMPLATES
  ElementType     elType;

  elem = TtaGetParent (elem);
  elType = TtaGetElementType(elem);
  while (elem && strcmp(TtaGetSSchemaName(elType.ElSSchema), "Template"))
{
    elem = TtaGetParent (elem);
    elType = TtaGetElementType(elem);
  }
  return elem;
#else
  return NULL;
#endif /* TEMPLATES */
}


/*----------------------------------------------------------------------
  TemplateElementWillBeCreated
  Processed when an element will be created in a template context.
  ----------------------------------------------------------------------*/
ThotBool TemplateElementWillBeCreated (NotifyElement *event)
{
#ifdef TEMPLATES
  ElementType elType = event->elementType;
  Element     parent = event->element;
  ElementType parentType = TtaGetElementType(parent);
  Element     ancestor;
  ElementType ancestorType;
  SSchema     templateSSchema;
  char*       types;
  ThotBool    b;

  if(event->info==1)
    return FALSE;

  if (!TtaGetDocumentAccessMode(event->document))
    return TRUE;

  templateSSchema = TtaGetSSchema ("Template", event->document);
  if (templateSSchema == NULL)
    return FALSE; // let Thot do the job

  // Fisrt, test if in a xt:bag or in a base-element xt:use
  if(parentType.ElSSchema == templateSSchema)
    ancestor = parent;
  else
    ancestor = GetFirstTemplateParentElement (parent);

  if (ancestor)
  {
    ancestorType = TtaGetElementType(ancestor);
    if (ancestorType.ElTypeNum == Template_EL_bag)
    {
      // only check the use child
      if (ancestor != parent)
        return  FALSE; // let Thot do the job
      if (elType.ElSSchema == templateSSchema &&
          (elType.ElTypeNum == Template_EL_useSimple ||
           elType.ElTypeNum == Template_EL_useEl))
        return FALSE;
      return !Template_CanInsertElementInBagElement (event->document, elType, ancestor);      
    }
    else if(ancestorType.ElTypeNum == Template_EL_useSimple ||
            ancestorType.ElTypeNum == Template_EL_useEl)
    {
      // only check the bag child @@@ will be check exclude/include later
      //if (ancestor != parent)
      //  return  FALSE; // let Thot do the job
      types = GetAttributeStringValueFromNum(ancestor, Template_ATTR_currentType, NULL);
      b = Template_CanInsertElementInUse(event->document, elType, types, parent, event->position);
      TtaFreeMemory(types);
      return !b;
      
    }
  }
  
  if (elType.ElSSchema == templateSSchema && elType.ElTypeNum == Template_EL_TEXT_UNIT)
  {
    return FALSE;
  }
  
  // Can not insert.
  return TRUE;
#endif /* TEMPLATES*/
  return FALSE;
}

/*----------------------------------------------------------------------
  TemplateElementWillBeDeleted
  Processed when an element will be deleted in a template context.
  ----------------------------------------------------------------------*/
ThotBool TemplateElementWillBeDeleted (NotifyElement *event)
{
#ifdef TEMPLATES
  Document       doc = event->document;
  Element        elem = event->element;
  Element        xtElem, parent = NULL, sibling;
  ElementType    xtType, elType;
  char*          type;
  Declaration    dec;
  SSchema        templateSSchema;
  XTigerTemplate t;
  ThotBool       selparent = FALSE;

  if(event->info==1)
    return FALSE;

  if (!TtaGetDocumentAccessMode(event->document))
    return TRUE;

  templateSSchema = TtaGetSSchema ("Template", event->document);
  if (templateSSchema == NULL)
    return FALSE; // let Thot do the job

  xtElem = GetFirstTemplateParentElement(elem);
  if (xtElem)
  {
    xtType = TtaGetElementType(xtElem);
    
    t = GetXTigerDocTemplate(doc);

    if (xtType.ElTypeNum==Template_EL_bag)
    {
      elType = TtaGetElementType(elem);
      if(elType.ElSSchema==templateSSchema &&
        (elType.ElTypeNum==Template_EL_useSimple || elType.ElTypeNum==Template_EL_useEl))
      {
        // Remove element manually.
        TtaOpenUndoSequence(doc, elem, elem, 0, 0);
        TtaRegisterElementDelete(elem, doc);
        TtaDeleteTree(elem, doc);
        TtaCloseUndoSequence(doc);
        return TRUE;
      }
      else
        return FALSE; // xt:bag always allow remove children.
    }
    else if (xtType.ElTypeNum==Template_EL_useSimple || xtType.ElTypeNum==Template_EL_useEl)
    {
      parent = TtaGetParent(elem);
      if (xtElem!=parent)
      {
        type = GetAttributeStringValueFromNum(xtElem, Template_ATTR_currentType, NULL);
        dec = Template_GetDeclaration(t, type);
        TtaFreeMemory(type);
        
        if (dec && dec->nature == XmlElementNat)
          return FALSE; // Can remove element only if in xt:use current type is base language element. 
        else
          return TRUE;
      }
    }
    else if (xtType.ElTypeNum==Template_EL_repeat)
    {
      sibling = TtaGetSuccessor (elem);
      if (sibling == NULL)
        {
          // there is no next element
          sibling = TtaGetPredecessor (elem);
          if (sibling == NULL)
            selparent = TRUE;
        }
      TtaRegisterElementDelete(elem, doc);
      TtaDeleteTree(elem, doc);
      InstantiateRepeat(t, xtElem, doc, TRUE);
      if (selparent)
        // look for the new sibling
        sibling = TtaGetFirstChild (parent);
      if (sibling)
        TtaSelectElement(doc, sibling);
      else
        TtaSelectElement(doc, parent);
      return TRUE;
    }
  }
  
  //TODO Test if current element is use or repeat.
  // Because if an element is delete and it is the unique child of its parent,
  // the parent intends to destroy itself. 
  
  return TRUE;
#else /* TEMPLATES */
  return FALSE;
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  CurrentTypeWillBeExported
  Check if the xt:currentType attribute can be exported
  ----------------------------------------------------------------------*/
ThotBool CurrentTypeWillBeExported (NotifyAttribute *event)
{
#ifdef TEMPLATES

  if (!TtaGetDocumentAccessMode(event->document))
    return TRUE;

  if (IsTemplateDocument(event->document))
    return TRUE;
#endif /* TEMPLATES */
  return FALSE;
}

/*----------------------------------------------------------------------
  TemplateAttrInMenu
  Called by Thot when building the Attributes menu for template elements.
  ----------------------------------------------------------------------*/
ThotBool TemplateAttrInMenu (NotifyAttribute * event)
{
#ifdef TEMPLATES
  // Prevent from showing attributes for template instance but not templates.
  if(IsTemplateInstanceDocument(event->document))
    return TRUE;
  else
#endif /* TEMPLATES */
    return FALSE;
}

/*----------------------------------------------------------------------
  CreateTemplateFromDocument
  Create a template from the current document.
  ----------------------------------------------------------------------*/
void CreateTemplateFromDocument(Document doc, View view)
{
#ifdef TEMPLATES
  char buffer[MAX_LENGTH];
  strcpy(buffer, DocumentURLs[doc]);
  strcat(buffer, ".xtd");
  DontReplaceOldDoc = TRUE;
  CreateTemplate(doc, buffer);
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  UpdateTemplateMenus
  ----------------------------------------------------------------------*/
void UpdateTemplateMenus (Document doc)
{
  if(IsTemplateInstanceDocument(doc) || 
      IsTemplateDocument(doc))
    TtaSetItemOff (doc, 1, Tools, BCreateTemplateFromDocument);
  else
    TtaSetItemOn (doc, 1, Tools, BCreateTemplateFromDocument);
}

/*----------------------------------------------------------------------
  UninstanciateTemplateDocument
  An instance of a template is tranformed into a template-less docuemnt.
  Remove link between XTigerTemplate structure and document.
  ----------------------------------------------------------------------*/
void UninstanciateTemplateDocument(Document doc)
{
#ifdef TEMPLATES
  XTigerTemplate t = GetXTigerDocTemplate(doc);
  if(t)
    Template_Close(t);
#endif /* TEMPLATES */  
}


/*----------------------------------------------------------------------
  Template_PrepareInstance
  Allocate XTigerTemplate structure for instance and initialize template
  url and template version.
  ----------------------------------------------------------------------*/
void Template_PrepareInstance(char *fileName, Document doc, char* template_version, char* template_url)
{
#ifdef TEMPLATES
  XTigerTemplate t = GetXTigerTemplate(fileName);
  if(!t)
    t = NewXTigerTemplate(fileName);
  t->state           = templInstance;
  t->templateVersion = template_version;
  t->base_uri        = template_url;
  t->doc             = doc;
  
#endif /* TEMPLATES */
}


/*----------------------------------------------------------------------
  SetDocumentAsXTigerTemplate
  Set the document template structure as template.
  ----------------------------------------------------------------------*/
void SetDocumentAsXTigerTemplate(Document doc)
{
#ifdef TEMPLATES
  XTigerTemplate t = GetXTigerDocTemplate(doc);
  if(t)
    t->state |= templTemplate;
#endif /* TEMPLATES */  
}

/*----------------------------------------------------------------------
  SetDocumentAsXTigerLibrary
  Set the document template structure as template library.
  ----------------------------------------------------------------------*/
void SetDocumentAsXTigerLibrary(Document doc)
{
#ifdef TEMPLATES
  XTigerTemplate t = GetXTigerDocTemplate(doc);
  if(t)
    t->state |= templLibrary;
#endif /* TEMPLATES */  
}
