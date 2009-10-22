/*
 *
 *  COPYRIGHT INRIA and W3C, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Authors: Francesc Campoy Flores
 *          Emilien Kia
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
#include "HTMLedit_f.h"
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
Prop_Templates_Path *TemplateRepositoryPaths = NULL;
// register the parent repeat of the new created use
static Element       Creating_repeat = NULL;


/*----------------------------------------------------------------------
  IsTemplateInstanceDocument: Test if a document is a template instance
  doc : Document to test
  return : TRUE if the document is a template instance
  ----------------------------------------------------------------------*/
ThotBool IsTemplateInstanceDocument(Document doc)
{
#ifdef TEMPLATES
  // check first indicators
  if (DocumentMeta[doc])
    {
    if (DocumentMeta[doc]->method == CE_INSTANCE)
      return TRUE;
    else if (DocumentMeta[doc]->method == CE_TEMPLATE)
      return FALSE;
    }
  XTigerTemplate t = GetXTigerDocTemplate(doc);
  if (t)
    return ((t->state & templInstance) != 0);
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
ThotBool IsTemplateDocument (Document doc)
{
#ifdef TEMPLATES
  XTigerTemplate t = GetXTigerDocTemplate(doc);
  if (DocumentMeta[doc])
    {
    if (DocumentMeta[doc]->method == CE_INSTANCE)
      return FALSE;
    else if (DocumentMeta[doc]->method == CE_TEMPLATE)
      return TRUE;
    }
  if (t)
    return ((t->state & templInstance) == 0);
  else
    return FALSE;
#endif /* TEMPLATES */
  return FALSE;
}


/*----------------------------------------------------------------------
  GetUsedTypeName returns the name of the current used type or the first
  name of the types attribute
  The returned string must be freed
  ----------------------------------------------------------------------*/
char *GetUsedTypeName (Element el)
{
  char        *name = NULL;
#ifdef TEMPLATES
  char        *ptr;

  if (IsTemplateElement (el))
    {
      name = GetAttributeStringValueFromNum (el, Template_ATTR_currentType, NULL);
      if (name == NULL)
        {
          // use the first type
          name = GetAttributeStringValueFromNum (el, Template_ATTR_types, NULL);
          if (name)
            {
              ptr = strstr (name, " ");
              if (ptr)
                *ptr = EOS;
            }
          return name;
        }
    }
#endif /* TEMPLATES */
  return name;
}

/*----------------------------------------------------------------------
  IsInLineTemplateElement returns TRUE if the template element can be
  inserted into a paragraph
  ----------------------------------------------------------------------*/
ThotBool IsInLineTemplateElement (Element el, Document doc)
{
#ifdef TEMPLATES
  XTigerTemplate t;
  ElementType    elType;
  Declaration    dec;
  char          *name = NULL;

  elType = TtaGetElementType (el);
  if (elType.ElTypeNum == Template_EL_useEl ||
      elType.ElTypeNum == Template_EL_useSimple)
    {
      t = GetXTigerDocTemplate(doc);
      if (t)
        {
          name = GetUsedTypeName (el);
          dec = Template_GetDeclaration (t, name);
          TtaFreeMemory (name);
          if (dec)
            return !dec->blockLevel;
          else
            return FALSE;
        }
    }
#endif /* TEMPLATES */
  return FALSE;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
Element GetParentLine (Element el, SSchema templateSSchema)
{
  Element        parent = TtaGetParent(el);
#ifdef TEMPLATES
  ElementType    parentType;
  char          *name;

  // look for the enclosing parent line element
  parentType = TtaGetElementType(parent);
  while (parent && parentType.ElSSchema == templateSSchema)
    {
      parent = TtaGetParent(parent);
      parentType = TtaGetElementType(parent);
    }
  if (parent && parentType.ElSSchema)
    {
      name = TtaGetSSchemaName (parentType.ElSSchema);
      if (name == NULL || strcmp (name, "HTML") ||
          (parentType.ElTypeNum != HTML_EL_Pseudo_paragraph &&
           parentType.ElTypeNum != HTML_EL_Paragraph))
        parent = NULL;
    }
#endif /* TEMPLATES */
  return parent;
}

/*----------------------------------------------------------------------
  Test if a document is an internal template.
  (no instance is opened and it is not edited)
  ----------------------------------------------------------------------*/
ThotBool IsInternalTemplateDocument(Document doc)
{
#ifdef TEMPLATES
  XTigerTemplate t = GetXTigerDocTemplate(doc);
  if (t)
    return (t->state & templInternal) != 0;
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
  if (t)
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

  if (!IsTemplateInstanceDocument(doc))
    /* let Thot perform normal operation */
    return FALSE;
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
              if (TtaGetElementVolume (el) == 0)
                TtaSelectElement (doc, parent);
              else
                TtaSelectElement (doc, el);
              return TRUE;
            }
        }
    }
#endif /* TEMPLATES */
  /* let Thot perform normal operation */
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
  element->Path = TtaStrdup (path);
  if (prevElement)
    {
      element->NextPath = ((Prop_Templates_Path*)prevElement)->NextPath;
      ((Prop_Templates_Path*)prevElement)->NextPath = element;
    }
  return element;
}


/*----------------------------------------------------------------------
  FreeTemplateRepositoryList: Free the list of template repositories.
  ----------------------------------------------------------------------*/
void FreeTemplateRepositoryList ()
{
  Prop_Templates_Path  *element = TemplateRepositoryPaths;

  while (element)
    {
      Prop_Templates_Path* next = element->NextPath;
      TtaFreeMemory (element->Path);
      TtaFreeMemory (element);
      element = next;
    }
  TemplateRepositoryPaths = NULL;
}

/*----------------------------------------------------------------------
  SaveTemplateRepositoryList: Save the list of template repositories.
  ----------------------------------------------------------------------*/
void SaveTemplateRepositoryList ()
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
      element = TemplateRepositoryPaths;
      while (element)
        {
          fprintf(file, "%s\n", element->Path);
          element = element->NextPath;
        }
      TtaWriteClose (file);
    }
}

/*----------------------------------------------------------------------
  LoadTemplateRepositoryList: Load the list of template repositories.
  return : the number of readed repository paths.
  ----------------------------------------------------------------------*/
static int LoadTemplateRepositoryList ()
{
  Prop_Templates_Path *element, *current = NULL;
  char                *path, *homePath;
  unsigned char       *c;
  int                  nb = 0;
  FILE                *file;

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
  TemplateRepositoryPaths = NULL;
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
              element->Path = TtaStrdup (path);
              if (TemplateRepositoryPaths == NULL)
                TemplateRepositoryPaths = element;
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
          element->Path = TtaStrdup (path);
          element->NextPath = NULL;

          if (TemplateRepositoryPaths == NULL)
            TemplateRepositoryPaths = element;
          else
            current->NextPath = element;
          nb++;
        }
      TtaReadClose (file);
    }
  TtaFreeMemory(path);
  return nb;
}

/*-----------------------------------------------------------------------
  InitTemplates
  Initializes the annotation library
  -----------------------------------------------------------------------*/
void InitTemplates ()
{
  TtaSetEnvBoolean ("SHOW_TEMPLATES", TRUE, FALSE);
  LoadTemplateRepositoryList ();
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
      CreateInstance (templatename, s, docname, docType, doc);
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
  Template_AddReference(GetXTigerTemplate (template_url));
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
  Template_RemoveReference(GetXTigerTemplate (template_url));
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
  The parameter option adds an empty string
  ----------------------------------------------------------------------*/
static char *createMenuString (const struct menuType* items, const int nbItems,
                               ThotBool option)
{
  char *result, *iter;
  int   size = 0;
  int   i;

  if (option)
    size += 3;
  for (i = 0; i < nbItems; i++)
    size += 2 + strlen (items[i].label);

  result = (char *) TtaGetMemory (size);
  iter = result;
  if (option)
    {
      strcpy (iter, "B ");
      iter +=  3;
    }
  for (i = 0; i < nbItems; i++)
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
        {
          if (Template_CanInsertRepeatChild (event->element))
            return TemplateElementWillBeCreated (event);
          else
            return TRUE; //don't let Thot do the job
        }
      else
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
      if (first == el)
        TtaNextSibling (&first);
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
  // look for the enclosing target element
  parent = GetParentLine (parent, templateSSchema);
  InstantiateUse (t, el, doc, parent, TRUE, FALSE);
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
  if (max && max[0]!=EOS)
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
  The parameter items is a space-separated choice list string.
  The parameter option adds an empty string
  Return The choosed item string or NULL if none.
  ----------------------------------------------------------------------*/
static char *QueryStringFromMenu (Document doc, char* items, ThotBool option)
{
  int              nbitems, size;
  struct menuType *itemlist;
  char            *menuString;
  char            *result = NULL;

  if (!TtaGetDocumentAccessMode (doc))
    return NULL;
  if (items == NULL)
    return NULL;
  size = strlen (items);
  if (size == 0)
    return NULL;
  giveItems (items, size, &itemlist, &nbitems);
  menuString = createMenuString (itemlist, nbitems, option);
  TtaNewScrollPopup (BaseDialog + OptionMenu, TtaGetViewFrame (doc, 1), NULL,
                     nbitems, menuString , NULL, false, 'L');
  TtaFreeMemory (menuString);
  ReturnOption = -1;
  TtaShowDialogue (BaseDialog + OptionMenu, FALSE, TRUE);
  TtaWaitShowProcDialogue ();
  TtaDestroyDialogue (BaseDialog + OptionMenu);

  if (ReturnOption != -1)
    {
      if (option)
        {
          if (ReturnOption == 0)
            result = TtaStrdup(" ");
          else
            result = TtaStrdup(itemlist[ReturnOption-1].label);
        }
      else
        result = TtaStrdup(itemlist[ReturnOption].label);
    }

  TtaFreeMemory (itemlist);
  return result;
}
#endif /* TEMPLATES */

#ifdef TEMPLATE_DEBUG
void FillInsertableElemList (Document doc, Element elem, DLList list);
#endif /* TEMPLATE_DEBUG */
/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
char *Template_GetListTypes (XTigerTemplate t, Element el)
{
#ifdef TEMPLATES
  char  *listtypes = NULL, *types;

  types = GetAttributeStringValueFromNum (el, Template_ATTR_types, NULL);
  if (types)
    {
      listtypes = Template_ExpandTypes (t, types, el, FALSE);
      TtaFreeMemory (types);
    }
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
  SSchema         templateSSchema;
  char           *listtypes = NULL;
  char           *result = NULL;
  ThotBool        oldStructureChecking;
  DisplayMode     dispMode;

  if (!TtaGetDocumentAccessMode (doc))
    return TRUE;
  if (!IsTemplateInstanceDocument (doc))
    return FALSE; /* let Thot perform normal operation */

#ifdef IV
  View            view;
  TtaGetActiveView (&doc, &view);
  if (view == 1)
    return FALSE; /* let Thot perform normal operation */
#endif
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
          result = QueryStringFromMenu (doc, listtypes, FALSE);
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

                  /* Insert */
                  newEl = Template_InsertBagChild (doc, el, bagEl, decl, FALSE);

                  /* Finish insertion.*/
                  TtaSetDocumentModified (doc);
                  TtaSetStructureChecking (oldStructureChecking, doc);
                  // restore the display
                  TtaSetDisplayMode (doc, dispMode);
                  firstEl = GetFirstEditableElement (newEl);
                  if (firstEl)
                    {
                      TtaSelectElement (doc, firstEl);
                      TtaSetStatusSelectedElement (doc, 1, firstEl);
                    }
                  else
                    {
                      TtaSelectElement (doc, newEl);
                      TtaSetStatusSelectedElement (doc, 1, newEl);
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
  DoReplicateUseElement insert a new element after the el child of
  repeatEl or as the first child of repeatEl.
  ----------------------------------------------------------------------*/
void DoReplicateUseElement (XTigerTemplate t, Document doc, int view,
                            Element el, Element repeatEl, char *name)
{
  Declaration     decl;
  Element         newEl, firstEl, prevRepeat, parentLine;
  ElementType     elType;
  DisplayMode     dispMode;
  ThotBool        oldStructureChecking;

  if (repeatEl == Creating_repeat)
    return;
  prevRepeat = Creating_repeat;
  Creating_repeat = repeatEl;

  decl = Template_GetDeclaration (t, name);
  elType = TtaGetElementType (el);
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

      parentLine = GetParentLine (el, elType.ElSSchema);
      if (parentLine)
        // display the element in line
        Template_SetInline (el, elType.ElSSchema, doc, TRUE);
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
  Creating_repeat = prevRepeat;
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
  Element         repeatEl = el;
  Element         firstEl;
  View            view;
  char           *listtypes = NULL;
  char           *result = NULL;

  if (!TtaGetDocumentAccessMode(doc))
    return TRUE;
  if (!IsTemplateInstanceDocument (doc))
    return FALSE; /* let Thot perform normal operation */

  TtaGetActiveView (&doc, &view);
#ifdef IV
  if (view == 1)
    return FALSE; /* let Thot perform normal operation */
#endif
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
          if (firstEl)
            listtypes = Template_GetListTypes (t, firstEl);
          else
            listtypes = Template_GetListTypes (t, repeatEl);
          if (listtypes)
            {
#ifdef TEMPLATE_DEBUG
              printf("RepeatButtonClicked : \n  > %s\n", listtypes);
#endif /* TEMPLATE_DEBUG */
              result = QueryStringFromMenu (doc, listtypes, FALSE);
              TtaFreeMemory (listtypes);
              if (result)
                {
                  if (event->position == 1)
                    {
                      // force the insert before
                      if (el != repeatEl)
                        TtaPreviousSibling (&el);
                      if (el)
                        DoReplicateUseElement (t, doc, view, el, repeatEl, result);
                      else
                        DoReplicateUseElement (t, doc, view, repeatEl, repeatEl, result);
                    }
                  else
                    DoReplicateUseElement (t, doc, view, el, repeatEl, result);
                }
            }
          TtaFreeMemory (result);
          DumpSubtree (repeatEl, doc, 0);

        }
      else /* if (Template_CanInsertRepeatChild(repeatEl)) */
        TtaSetStatus(doc, view, TtaGetMessage (AMAYA, AM_NUMBER_OCCUR_HAVE_MAX), NULL);
    }
  return TRUE; /* don't let Thot perform normal operation */
#endif /* TEMPLATES */
  return TRUE;
}

/*----------------------------------------------------------------------
  ElementIsOptional
  Return TRUE if the element is optional
  ----------------------------------------------------------------------*/
ThotBool ElementIsOptional (Element el)
{
  ElementType	     elType;
	AttributeType    attType;
  Attribute        att;

  elType = TtaGetElementType (el);
  attType.AttrSSchema = elType.ElSSchema;
  attType.AttrTypeNum = Template_ATTR_option;
  att = TtaGetAttribute (el, attType);
  return (att != NULL);
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
  Element         child, parent;
  ElementType     elType, parentType;
  View            view = 1;
  XTigerTemplate  t;
  Declaration     decl;
  Element         firstEl, newEl = NULL;
  char           *types, *listtypes = NULL, *result = NULL;
  ThotBool        oldStructureChecking, option;

  if (!TtaGetDocumentAccessMode (doc))
    return TRUE;
  if (!IsTemplateInstanceDocument (doc))
    return FALSE; /* let Thot perform normal operation */
#ifdef IV
  TtaGetActiveView (&doc, &view);
  if (view == 1)
    return FALSE; /* let Thot perform normal operation */
#endif
  TtaCancelSelection(doc);
  t = GetXTigerDocTemplate(doc);
  if (!t)
    return FALSE; /* let Thot perform normal operation */

  elType = TtaGetElementType (el);
  parent = TtaGetParent (el);
  parentType = TtaGetElementType (parent);
  if (parent &&
      // TemplateObject is just a place holder
      (parentType.ElSSchema == elType.ElSSchema ||
       parentType.ElTypeNum == Template_EL_useEl))
    RepeatButtonClicked(event);
  else
    {
      types = GetAttributeStringValueFromNum(el, Template_ATTR_types, NULL);
      if (types)
        {
          listtypes = Template_ExpandTypes(t, types, NULL, FALSE);
#ifdef TEMPLATE_DEBUG
          printf("UseButtonClicked : \n  > %s\n", listtypes);
#endif /* TEMPLATE_DEBUG */
          option = ElementIsOptional(el);
          result = QueryStringFromMenu(doc, listtypes, option);
          if (result)
            {
              decl = Template_GetDeclaration (t, result);
              if (decl || !strcmp (result, " "))
                {
                  /* Prepare insertion.*/
                  oldStructureChecking = TtaGetStructureChecking (doc);
                  TtaSetStructureChecking (FALSE, doc);
                  TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
                  // clean up the current content
                  firstEl = TtaGetFirstChild (el);
                  while (firstEl)
                    {
                      TtaRegisterElementDelete(firstEl, doc);
                      TtaDeleteTree (firstEl, doc);
                      firstEl = TtaGetFirstChild (el);
                    }
             
                  // look for the enclosing target element
                  parent = GetParentLine (el, elType.ElSSchema);
                  if (decl)
                    /* Insert */
                    newEl = Template_InsertUseChildren(doc, el, decl, parent, TRUE);
                  else
                    {
                      newEl = Template_GetNewSimpleTypeInstance(doc);
                      newEl = InsertWithNotify (newEl, NULL, el, doc);
                    }
                  for (child = TtaGetFirstChild(newEl); child; TtaNextSibling(&child))
                    TtaRegisterElementCreate (child, doc);

                  /* xt:currentType attribute.*/
                  SetAttributeStringValueWithUndo (el, Template_ATTR_currentType, result);
                  /* Finish insertion. */
                  TtaCloseUndoSequence(doc);
                  TtaSetDocumentModified (doc);
                  TtaSetStructureChecking (oldStructureChecking, doc);

                  firstEl = GetFirstEditableElement (newEl);
                  if (firstEl)
                    {
                      TtaSelectElement (doc, firstEl);
                      TtaSetStatusSelectedElement (doc, view, firstEl);
                    }
                  else
                    {
                      TtaSelectElement (doc, newEl);
                      TtaSetStatusSelectedElement(doc, view, newEl);
                    }
                }
            }
        }
      TtaFreeMemory (types);
      TtaFreeMemory (listtypes);
      TtaFreeMemory (result);
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
  ElementType     elType, parentType;
  AttributeType   attrType;
  Attribute       att;

  if (!TtaGetDocumentAccessMode (event->document))
    return TRUE;
  if (!IsTemplateInstanceDocument (event->document))
    return FALSE; /* let Thot perform normal operation */

  if (event->position == 0)
    return FALSE; /* let Thot perform normal operation */
  elType = TtaGetElementType (event->element);
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = Template_ATTR_option;
  att = TtaGetAttribute (event->element, attrType);
  if (att && event->position == 2)
    return OptionButtonClicked (event);

  parentType = TtaGetElementType (TtaGetParent( event->element));
  if (parentType.ElTypeNum == Template_EL_repeat)
    return RepeatButtonClicked (event);
  else if (parentType.ElTypeNum == Template_EL_bag)
    return BagButtonClicked (event);
  else
    {
      // select to the whole element
      TtaSelectElement (event->document, event->element);
      return TRUE;
    }
#endif /* TEMPLATES */
  return FALSE;
}

/*----------------------------------------------------------------------
  OptionButtonClicked
  ----------------------------------------------------------------------*/
ThotBool OptionButtonClicked (NotifyElement *event)
{
#ifdef TEMPLATES
  Element         el, child, parent;
  ElementType     useType, childType;
  Document        doc;
  XTigerTemplate  t;

  if (!TtaGetDocumentAccessMode (event->document))
    return TRUE;
  if (!IsTemplateInstanceDocument (event->document))
    return FALSE; /* let Thot perform normal operation */
#ifdef IV
  View            view;
  TtaGetActiveView (&doc, &view);
  if (view != 1)
    return FALSE; /* let Thot perform normal operation */
#endif
  doc = event->document;
  el = event->element;
  if (!el)
    return FALSE; /* let Thot perform normal operation */
  useType = TtaGetElementType (el);
  if (useType.ElTypeNum != Template_EL_useEl &&
      useType.ElTypeNum != Template_EL_useSimple)
    return FALSE;

  TtaOpenUndoSequence(doc, NULL, NULL, 0, 0);
  TtaCancelSelection (doc);
  child = TtaGetFirstChild (el);
  childType = TtaGetElementType (child);
  if (child == NULL ||
      (childType.ElSSchema == useType.ElSSchema &&
       childType.ElTypeNum == Template_EL_TemplateObject))
    /* the "use" element is empty. Instantiate it */
    {
      if (child)
        {
          TtaRegisterElementDelete (child, doc);
          TtaDeleteTree (child, doc);
        }
      t = GetXTigerDocTemplate (doc);
      if (!t)
        return FALSE; // no template ?!?!

      // look for the enclosing target element
      parent = GetParentLine (el, useType.ElSSchema);
      InstantiateUse (t, el, doc, parent, TRUE, FALSE);
      SetAttributeIntValue (el, Template_ATTR_option,
                            Template_ATTR_option_VAL_option_set, TRUE);
    }
  else
    /* remove the content of the "use" element */
    {
      child = Template_FillEmpty (el, doc, TRUE);
      SetAttributeIntValue (el, Template_ATTR_option,
                            Template_ATTR_option_VAL_option_unset, TRUE);
    }
  child = TtaGetFirstChild (el);
  TtaSelectElement (doc, child);
  TtaCloseUndoSequence (doc);
  TtaSetDocumentModified (doc);
  return TRUE; /* don't let Thot perform normal operation */
#endif /* TEMPLATES */
  return TRUE;
}


/*----------------------------------------------------------------------
  Template_FillFromDocument
  Fill XTigerTemplate structure with the content of the document.
  Load dependencies if needed.
  ----------------------------------------------------------------------*/
void Template_FillFromDocument (Document doc)
{
#ifdef TEMPLATES
  XTigerTemplate t = GetXTigerTemplate (DocumentURLs[doc]);
  Element        root;

  if (t)
    {
      SetTemplateDocument (t, doc);
#ifdef TEMPLATE_DEBUG
      printf("Template_FillFromDocument state: %d ", t->doc);
#endif
      Template_PrepareTemplate(t, doc);
      if (IsTemplateInstanceDocument (doc))
        {
#ifdef TEMPLATE_DEBUG
          printf("  -> instance\n");
#endif
          // fix all access rights in the instance
          root = TtaGetRootElement (doc);
          TtaSetAccessRight (root, ReadOnly, doc);
          Template_FixAccessRight (t, root, doc);
          TtaUpdateAccessRightInViews (doc, root);
          // Parse template to fill structure and remove extra data
          ParseTemplate (t, root, doc, NULL, TRUE);
        }
#ifdef TEMPLATE_DEBUG
      else if (t->state & templLibraryFlag)
        printf("  -> library\n");
      else if (t->state & templTemplate)
        printf("  -> template\n");
#endif
      // Mark loaded
      t->state |= templloaded;
      TtaSetDocumentUnmodified (doc);
      UpdateTemplateMenus (doc);

#ifdef TEMPLATE_DEBUG
      DumpAllDeclarations();
#endif /* TEMPLATE_DEBUG */
    }
#endif /* TEMPLATES */
}


/*----------------------------------------------------------------------
  Template_CheckAndPrepareTemplate checks if the document is a template
  or a library and prepare XTigerTemplate structure to use it.
  ----------------------------------------------------------------------*/
ThotBool Template_CheckAndPrepareTemplate (char* docURL)
{
#ifdef TEMPLATES
  XTigerTemplate t = NULL; //GetXTigerTemplate(docURL);

  if (IsXTiger (docURL))
    {
#ifdef TEMPLATE_DEBUG
      printf("Template_CheckAndPrepareTemplate %s templTemplate\n", docURL);
#endif
      t = LookForXTigerTemplate (docURL);
      t->state |= templTemplate;
    }
  else if (IsXTigerLibrary (docURL))
    {
#ifdef TEMPLATE_DEBUG
      printf("Template_CheckAndPrepareTemplate %s templLibrary\n", docURL);
#endif
      t = LookForXTigerLibrary (docURL);
      t->state |= templLibrary;
    }
  return t != NULL;
#endif /* TEMPLATES */
  return FALSE;
}


/*----------------------------------------------------------------------
  Template_CheckAndPrepareInstance checks if it is a template instance.
  If it's an instance and the template is not loaded, load it into a
  temporary file
  ----------------------------------------------------------------------*/
void Template_CheckAndPrepareInstance (char *localFileName, Document doc,
                                       char* docURL)
{
#ifdef TEMPLATES
  XTigerTemplate   t;
  char            *content, *ptr, *begin;
  gzFile           stream;
  char             buffer[2000];
  int              res;
  char            *template_version = NULL, *template_url = NULL;

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
                      t = GetXTigerTemplate (template_url);
                    }
                  Template_PrepareInstance (docURL, doc, template_version, template_url);
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
  ClosingTemplateDocument
  Callback called before closing a document which uses templates.
  ----------------------------------------------------------------------*/
ThotBool ClosingTemplateDocument (NotifyDialog* dialog)
{
#ifdef TEMPLATES
  XTigerTemplate t = GetXTigerDocTemplate (dialog->document);
  if (t)
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

  elType = TtaGetElementType (elem);
  if (elType.ElSSchema)
    return (strcmp (TtaGetSSchemaName(elType.ElSSchema) , "Template") == 0);
#endif /* TEMPLATES */
  return FALSE;
}


/*----------------------------------------------------------------------
  GetFirstTemplateParentElement
  Return the first element which has "Template" as schema name or null.
  ----------------------------------------------------------------------*/
Element GetFirstTemplateParentElement (Element elem)
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
  IsBeginningSelected
  Returns TRUE if the selection is athe beginning of an element
  ----------------------------------------------------------------------*/
ThotBool IsBeginningSelected (Element el, Document doc)
{
  Element     selElem, prev;
  int         firstChar, lastChar;

  TtaGiveFirstSelectedElement(doc, &selElem, &firstChar, &lastChar);
  if (firstChar <= 1 && lastChar < firstChar)
    {
      while (selElem && selElem != el)
        {
          prev = selElem;
          TtaPreviousSibling (&prev);
          if (prev)
            return FALSE;
          selElem = TtaGetParent (selElem);
        }
      return TRUE;
    }
  return FALSE;
}

/*----------------------------------------------------------------------
  TemplateElementWillBeCreated
  Processed when an element will be created in a template context.
  ----------------------------------------------------------------------*/
ThotBool TemplateElementWillBeCreated (NotifyElement *event)
{
#ifdef TEMPLATES
  ElementType     elType = event->elementType;
  Element         parent = event->element;
  ElementType     parentType = TtaGetElementType(parent), childType;
  Element         ancestor, el, next, child;
  ElementType     ancestorType;
  SSchema         templateSSchema;
  XTigerTemplate  t;
  char           *types, *ptr, *name = NULL;
  int             len, view, i, doc = event->document;
  ThotBool        b, isInstance;

  if (event->info == 1)
    return FALSE;

#ifdef TEMPLATE_DEBUG
  printf("TemplateElementWillBeCreated\n");
#endif
  if (!TtaGetDocumentAccessMode(doc))
    return TRUE;

  templateSSchema = TtaGetSSchema ("Template", doc);
  if (templateSSchema == NULL)
    return FALSE; // let Thot do the job
  isInstance = IsTemplateInstanceDocument(doc);

  // Fisrt, test if in a xt:bag or in a base-element xt:use
  if (parentType.ElSSchema == templateSSchema)
    ancestor = parent;
  else
    ancestor = GetFirstTemplateParentElement (parent);

  if (ancestor)
    {
      ancestorType = TtaGetElementType(ancestor);
      if (ancestorType.ElTypeNum == Template_EL_component)
        return FALSE; // let Thot do the job
      if (ancestorType.ElTypeNum == Template_EL_bag)
        {
          // only check the use child
          if (ancestor != parent)
            return FALSE; // let Thot do the job
          if (elType.ElSSchema == templateSSchema &&
              (elType.ElTypeNum == Template_EL_useSimple ||
               elType.ElTypeNum == Template_EL_useEl))
            return FALSE;
          return !Template_CanInsertElementInBagElement (doc, elType, ancestor);
        }
      else if (ancestorType.ElTypeNum == Template_EL_repeat)
        {
          if (elType.ElSSchema != templateSSchema ||
              (elType.ElTypeNum != Template_EL_useSimple &&
               elType.ElTypeNum != Template_EL_useEl))
            return TRUE; // don't let Thot do the job
          t = GetXTigerDocTemplate (doc);
          el = NULL;
          i = 0;
          next = TtaGetFirstChild (ancestor);
          while (next && i < event->position)
            {
              el = next;
              TtaNextSibling (&next);
              i++;
            }
          if (el)
            next = el;
          name = GetUsedTypeName (next);
          DoReplicateUseElement (t, doc, 1, el, ancestor, name);
          TtaFreeMemory (name);
          return TRUE; // don't let Thot do the job
        }
      else if (ancestorType.ElTypeNum == Template_EL_useSimple ||
               ancestorType.ElTypeNum == Template_EL_useEl)
        {
          // only check the bag child @@@ will be check exclude/include later
          if (elType.ElSSchema == templateSSchema &&
              (elType.ElTypeNum == Template_EL_useSimple ||
               elType.ElTypeNum == Template_EL_useEl))
            return TRUE; // don't let Thot do the job
          if (ancestor != parent)
            return  (TtaIsReadOnly (parent) != 0); // let or not Thot do the job
          if (isInstance)
            types = GetAttributeStringValueFromNum (ancestor, Template_ATTR_currentType, NULL);
          else
            types = GetAttributeStringValueFromNum (ancestor, Template_ATTR_types, NULL);
          b = Template_CanInsertElementInUse (doc, elType, types,
                                              parent, event->position);
          if (types && !b)
            {
              child = TtaGetFirstChild (ancestor);
              childType = TtaGetElementType(child);
              if (childType.ElSSchema != templateSSchema ||
                  childType.ElTypeNum != Template_EL_TemplateObject)
                {
                  parent = TtaGetParent (ancestor);
                  elType = TtaGetElementType(parent);
                  if (elType.ElSSchema == templateSSchema &&
                      elType.ElTypeNum == Template_EL_repeat)
                    {
                      t = GetXTigerDocTemplate (doc);
                      name = GetUsedTypeName (ancestor);
                      DoReplicateUseElement (t, doc, 1, ancestor, parent, name);
                      TtaFreeMemory (name);
                    }
                  TtaFreeMemory(types);
                  return TRUE; // don't let Thot do the job
                }

              // check with equivalent component
              name = (char *)GetXMLElementName (elType, doc);
              if (name)
                {
                  len = strlen (name);
                  ptr = strstr (types, name);
                  if (ptr && len &&
                      (ptr == types || ptr[-1] == SPACE) &&
                      (ptr[len] == EOS || ptr[len] == SPACE))
                    {
                      parent = TtaGetParent (ancestor);
                      parentType = TtaGetElementType(parent);
                      while (parentType.ElSSchema == templateSSchema &&
                          parentType.ElTypeNum == Template_EL_useSimple)
                        {
                          // move up to the inclosing use
                          ancestor = parent;
                           parent = TtaGetParent (ancestor);
                           parentType = TtaGetElementType(parent);
                           name = NULL;
                        }
                      if (parentType.ElSSchema == templateSSchema &&
                          parentType.ElTypeNum == Template_EL_repeat)
                        {
                          // duplicate the current use
                            TtaGetActiveView (&doc, &view);
                            t = GetXTigerDocTemplate(doc);
                            if (IsBeginningSelected (ancestor, doc))
                              TtaPreviousSibling (&ancestor);
                            if (name == NULL)
                              {
                                name = GetUsedTypeName (ancestor);
                                DoReplicateUseElement (t, doc, view, ancestor,
                                                     parent, name);
                                TtaFreeMemory (name);
                              }
                            else
                              DoReplicateUseElement (t, doc, view, ancestor,
                                                     parent, name);
                        }
                    }
                }
            }
          TtaFreeMemory(types);
          return !b;
        }
      else if (ancestorType.ElTypeNum == Template_EL_component)
        // allow all changes
        return FALSE;
    }

  if (!IsTemplateInstanceDocument(doc) &&
       elType.ElSSchema == templateSSchema &&
      (elType.ElTypeNum == Template_EL_TEXT_UNIT ||
       elType.ElTypeNum == Template_EL_component))
      return FALSE;
  // Cannot insert.
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
  Element        el = event->element;
  Element        xtEl, parent = NULL, sibling;
  ElementType    xtType, elType;
  SSchema        templateSSchema;
  XTigerTemplate t;

  if (event->info == 1)
    return FALSE;

  if (!TtaGetDocumentAccessMode(doc))
    return TRUE;
  if (!IsTemplateInstanceDocument (doc))
    return FALSE; // If template or library, pass to specialized functions.

  templateSSchema = TtaGetSSchema ("Template", doc);
  if (templateSSchema == NULL)
    return FALSE; // let Thot do the job

  t = GetXTigerDocTemplate(doc);
  elType = TtaGetElementType (el);
  xtEl = GetFirstTemplateParentElement (el);
  if (elType.ElSSchema == templateSSchema &&
      elType.ElTypeNum == Template_EL_repeat)
    {
      TtaOpenUndoSequence (doc, el, el, 0, 0);
      CleanUpRepeat (el, doc, TRUE);
      TtaCloseUndoSequence (doc);
      TtaSelectElement (doc, el);
      TtaSetDocumentModified (doc);
    }
  else if (elType.ElSSchema == templateSSchema &&
           elType.ElTypeNum == Template_EL_bag)
    {
      // clean up the content of the bag or repeat
      sibling = TtaGetFirstChild (el);
      xtEl = el;
      TtaOpenUndoSequence (doc, el, el, 0, 0);
      while (sibling)
        {
          el = sibling;
          TtaNextSibling (&sibling);
          TtaRegisterElementDelete (el, doc);
          TtaDeleteTree (el, doc);
        }
      TtaCloseUndoSequence (doc);
      TtaSelectElement (doc, xtEl);
      TtaSetDocumentModified (doc);
    }
  else if (xtEl)
    {
      xtType = TtaGetElementType (xtEl);
      if (xtType.ElTypeNum==Template_EL_bag)
        return FALSE; // xt:bag always allow remove children.
      else
        {
          // look for the enclosing use
          if (elType.ElSSchema != templateSSchema)
            {
              // check if the element is alone
              sibling = el;
              TtaNextSibling (&sibling);
              while (sibling == NULL)
                {
                  // there is no next element
                  sibling = el;
                  TtaPreviousSibling (&sibling);
                  if (parent == xtEl)
                    break;
                  if (sibling == NULL)
                    {
                      el = parent;
                      parent = TtaGetParent (el);
                      sibling = el;
                      TtaNextSibling (&sibling);
                    }
                }
              if (sibling)
                return TRUE; // cannot delete
            }

          TtaOpenUndoSequence (doc, el, el, 0, 0);
          xtEl = TtaGetParent (el);
          xtType = TtaGetElementType (xtEl);
          if (xtType.ElSSchema == templateSSchema &&
              xtType.ElTypeNum == Template_EL_repeat)
            {
              // check if the repeat becomes empty
              elType = TtaGetElementType (el);
              sibling = el;
              TtaNextSibling (&sibling);
              if (sibling == NULL)
                {
                  // there is no next element
                  sibling = el;
                  TtaPreviousSibling (&sibling);
                }
              if (sibling)
                {
                  // delete the use within a bag or a repeat
                  TtaRegisterElementDelete (el, doc);
                  TtaDeleteTree (el, doc);
                }
              else
                // keep an empty use element
                Template_FillEmpty (el, doc, TRUE);
              TtaSetDocumentModified (doc);
            }
          else if (xtType.ElSSchema == templateSSchema &&
                   xtType.ElTypeNum == Template_EL_bag)
            {
              // delete the use within a bag or a repeat
              TtaRegisterElementDelete (el, doc);
              TtaDeleteTree (el, doc);
              TtaSetDocumentModified (doc);
            }
          TtaCloseUndoSequence (doc);
          TtaSelectElement (doc, xtEl);
        }
    }
  return TRUE; // don't let thot do something
#else /* TEMPLATES */
  return FALSE;
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  TemplateAttrInMenu
  Called by Thot when building the Attributes menu for template elements.
  ----------------------------------------------------------------------*/
ThotBool TemplateAttrInMenu (NotifyAttribute * event)
{
#ifdef TEMPLATES
  // Prevent from showing attributes for template instance but not templates.
  if (IsTemplateInstanceDocument(event->document))
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
  char     buffer[MAX_LENGTH], suffix[10];

  if (IsW3Path (DocumentURLs[doc]) &&
      DocumentMeta[doc] && DocumentMeta[doc]->full_content_location)
    {
      // use the location instead of the current URI

      strcpy (buffer, DocumentMeta[doc]->full_content_location);
    }
  else
    strcpy (buffer, DocumentURLs[doc]);

  // remove the current suffix
  TtaExtractSuffix (buffer, suffix);
  // the new suffix
  strcat(buffer, ".xtd");
  DontReplaceOldDoc = TRUE;
  CreateTemplate (doc, buffer);
  // by default .xtd files are xml files
  TtaSetDocumentCharset (doc, UTF_8, FALSE);
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  UpdateTemplateMenus
  ----------------------------------------------------------------------*/
void UpdateTemplateMenus (Document doc)
{
  if (IsTemplateInstanceDocument(doc))
    {
      // Instance document
      TtaSetItemOff (doc, 1, Tools, BCreateTemplateFromDocument);
      TtaSetItemOff (doc, 1, Tools, BTemplateCreateTextBox);
      TtaSetItemOff (doc, 1, Tools, BTemplateCreateUseBox);
      TtaSetItemOff (doc, 1, Tools, BTemplateCreateUseCompBox);
      TtaSetItemOff (doc, 1, Tools, BTemplateCreateRepeat);
      TtaSetItemOff (doc, 1, Tools, BTemplateCreateRepeatComp);
      TtaSetItemOff (doc, 1, Tools, BTemplateCreateFreeBox);
      TtaSetItemOff (doc, 1, Tools, BTemplateCreateUnion);
      TtaSetItemOff (doc, 1, Tools, BTemplateCreateImport);
    }
  else if (DocumentURLs[doc] && DocumentTypes[doc] != docSource &&
           (IsXTiger (DocumentURLs[doc]) || IsXTigerLibrary (DocumentURLs[doc])))
    {
      // Template document
      TtaSetItemOff (doc, 1, Tools, BCreateTemplateFromDocument);
      TtaSetItemOn (doc, 1, Tools, BTemplateCreateTextBox);
      TtaSetItemOn (doc, 1, Tools, BTemplateCreateUseBox);
      TtaSetItemOn (doc, 1, Tools, BTemplateCreateUseCompBox);
      TtaSetItemOn (doc, 1, Tools, BTemplateCreateRepeat);
      TtaSetItemOn (doc, 1, Tools, BTemplateCreateRepeatComp);
      TtaSetItemOn (doc, 1, Tools, BTemplateCreateFreeBox);
      TtaSetItemOn (doc, 1, Tools, BTemplateCreateUnion);
      TtaSetItemOn (doc, 1, Tools, BTemplateCreateImport);
    }
  else
    {
      //Standard document
      TtaSetItemOn (doc, 1, Tools, BCreateTemplateFromDocument);
      TtaSetItemOff (doc, 1, Tools, BTemplateCreateTextBox);
      TtaSetItemOff (doc, 1, Tools, BTemplateCreateUseBox);
      TtaSetItemOff (doc, 1, Tools, BTemplateCreateUseCompBox);
      TtaSetItemOff (doc, 1, Tools, BTemplateCreateRepeat);
      TtaSetItemOff (doc, 1, Tools, BTemplateCreateRepeatComp);
      TtaSetItemOff (doc, 1, Tools, BTemplateCreateFreeBox);
      TtaSetItemOff (doc, 1, Tools, BTemplateCreateUnion);
      TtaSetItemOff (doc, 1, Tools, BTemplateCreateImport);
    }
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
  if (t)
    Template_Close(t);
#endif /* TEMPLATES */
}


/*----------------------------------------------------------------------
  Template_PrepareInstance
  Allocate XTigerTemplate structure for instance and initialize template
  url and template version.
  ----------------------------------------------------------------------*/
void Template_PrepareInstance (char *url, Document doc,
                               char* template_version, char* template_url)
{
#ifdef TEMPLATES
  XTigerTemplate t = GetXTigerTemplate (url);
  if (!t)
    t = NewXTigerTemplate (url);
  t->state           = templInstance;
  t->templateVersion = TtaStrdup (template_version);
  //t->uri             = TtaStrdup (fileName); // already done
  t->base_uri        = TtaStrdup (template_url);
  t->doc             = doc;
  t->ref             = 1;
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Template_PrepareLibrary
  Allocate XTigerTemplate structure for a library and initialize template
  url and template version.
  ----------------------------------------------------------------------*/
void Template_PrepareLibrary (char *url, Document doc, char* template_version)
{
#ifdef TEMPLATES
  XTigerTemplate t = GetXTigerTemplate(url);
  if (!t)
    t = NewXTigerTemplate (url);
  t->state           = templLibrary;
  t->templateVersion = TtaStrdup (template_version);
  //t->uri             = TtaStrdup (fileName); // already done
  t->doc             = doc;
  t->ref             = 1;
#endif /* TEMPLATES */
}


/*----------------------------------------------------------------------
  SetDocumentAsXTigerTemplate
  Set the document template structure as template.
  ----------------------------------------------------------------------*/
void SetDocumentAsXTigerTemplate (Document doc)
{
#ifdef TEMPLATES
  XTigerTemplate t = GetXTigerDocTemplate (doc);
  if (t)
    t->state |= templTemplate;
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  SetDocumentAsXTigerLibrary
  Set the document template structure as template library.
  ----------------------------------------------------------------------*/
void SetDocumentAsXTigerLibrary (Document doc)
{
#ifdef TEMPLATES
  XTigerTemplate t = GetXTigerDocTemplate (doc);
  if (t)
    t->state |= templLibrary;
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  TemplateCreateTextBox
  Create a xt:use types="string" box around the selection.
  ----------------------------------------------------------------------*/
void Template_CreateTextBox (Document doc, ThotBool createComp)
{
#ifdef TEMPLATES
  Element     selElem, comp, use, child, copy, head;
  ElementType selType, useType, compType;
  XTigerTemplate t;
  SSchema     sstempl = TtaGetSSchema ("Template", doc);
  int         firstChar, lastChar, sz = 128;
  char        buffer[128];
  const char *title, *label;
  ThotBool    oldStructureChecking, open;

  if (!TtaGetDocumentAccessMode(doc))
    return;

  if (doc && TtaGetDocumentAccessMode(doc) && sstempl &&
      IsTemplateDocument(doc) && !IsTemplateInstanceDocument(doc))
    {
      TtaGiveFirstSelectedElement(doc, &selElem, &firstChar, &lastChar);
      if (selElem)
        {
          selType =  TtaGetElementType(selElem);
          if (!TtaIsLeaf(selType))
            {
              selElem = TtaGetFirstLeaf(selElem);
              selType = TtaGetElementType(selElem);
              firstChar = lastChar = 0;
            }
          if (selType.ElTypeNum == 1)
            {
              // request the element label
              if (createComp)
                {
                  label = TtaGetMessage (AMAYA, AM_NAME);
                  title = TtaGetMessage (AMAYA, AM_TEMPLATE_NEWCOMP);
                }
              else
                {
                  label = TtaGetMessage (AMAYA, AM_TEMPLATE_LABEL);
                  title = TtaGetMessage (AMAYA, AM_TEMPLATE_USESTRING);
                }
              QueryStringFromUser(label, title, buffer, 32);
              if (buffer[0] == EOS)
                // stop the creation
                return;

              open = TtaHasUndoSequence (doc);
              useType.ElSSchema = sstempl;
              useType.ElTypeNum = Template_EL_useSimple;
              oldStructureChecking = TtaGetStructureChecking (doc);
              TtaSetStructureChecking (FALSE, doc);
              if (firstChar == 0)
                {
                  // the whole string is selected
                  use = TtaNewElement(doc, useType);
                  if (use)
                    {
                      if (!open)
                        TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
                      // create the use element
                      TtaInsertSibling (use, selElem, FALSE, doc);
                      SetAttributeStringValue (use, Template_ATTR_types, "string");
                      Template_SetLabel (doc, use, buffer, FALSE);
                      TtaRegisterElementCreate (use, doc);
                      // move the string
                      TtaRegisterElementDelete (selElem, doc);
                      TtaRemoveTree (selElem, doc);
                      TtaInsertFirstChild (&selElem, use, doc);
                      TtaRegisterElementCreate (selElem, doc);
                      if (!open)
                        TtaCloseUndoSequence (doc);
                      TtaSelectElement (doc, use);
		      TtaSetDocumentModified (doc);
                    }
                }
              else if (GenerateInlineElement (Template_EL_useSimple, sstempl, 0, "", TRUE))
                {
                  // only a piece of the string is selected
                  TtaGiveFirstSelectedElement (doc, &use, &firstChar, &lastChar);
                  selType = TtaGetElementType (use);
                  while (use &&
                         (selType.ElSSchema != sstempl ||
                          selType.ElTypeNum != Template_EL_useSimple))
                    {
                      // look for the enclosing use element
                      use = TtaGetParent (use);
                      selType = TtaGetElementType (use);
                    }
                  if (selType.ElSSchema == sstempl &&
                      selType.ElTypeNum == Template_EL_useSimple)
                    {
                      if (!open)
                        TtaExtendUndoSequence (doc);
                      Template_SetLabel (doc, use, buffer, TRUE);
                      if (createComp)
                        {
                         // Create the component
                          head = TemplateFindHead (doc);
                          child = TtaGetLastChild (head);
                          compType.ElSSchema = sstempl;
                          compType.ElTypeNum = Template_EL_component;
                          comp = TtaNewElement(doc, compType);
                          if (child)
                            TtaInsertSibling(comp, child, FALSE, doc);
                          else
                            TtaInsertFirstChild (&comp, head, doc);
                          child = TtaGetFirstChild (use);
                          copy = TtaCopyTree (child, doc, doc, comp);
                          TtaInsertFirstChild (&copy, comp, doc);
                          // register and update the component name
                          Template_SetName (doc, comp, buffer, FALSE);
                          TtaRegisterElementCreate (comp, doc);
                          GiveAttributeStringValueFromNum (comp, Template_ATTR_name, buffer, &sz);
                          // Declare the new component
                          t = GetXTigerDocTemplate(doc);
                          Template_DeclareNewComponent (t, buffer, comp, 1);
                          SetAttributeStringValueWithUndo (use, Template_ATTR_types, buffer);
                          SetAttributeStringValueWithUndo (use, Template_ATTR_currentType, buffer);
                          TtaSelectElement (doc, use);
                        }
                      else
                        {
                          SetAttributeStringValueWithUndo (use, Template_ATTR_types, "string");
                          // update the status bar
                          TtaSetStatusSelectedElement (doc, 1, use);
                        }
                      if (!open)
                        TtaCloseUndoSequence (doc);
		      TtaSetDocumentModified (doc);
                    }
                }
              TtaSetStructureChecking (oldStructureChecking, doc);
            }
        }
    }
#endif /* TEMPLATES */
}


/*----------------------------------------------------------------------
  TemplateCreateTextBox
  Create a xt:use types="string" box around the selection.
  ----------------------------------------------------------------------*/
void TemplateCreateTextBox (Document doc, View view)
{
#ifdef TEMPLATES
  Template_CreateTextBox (doc, FALSE);
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  TemplateCreateImport
  Create a xt:import
  ----------------------------------------------------------------------*/
void TemplateCreateImport (Document doc, View view)
{
#ifdef TEMPLATES
  Element        selElem, selElem2, parent;
  Element        el, prev, next;
  ElementType    elType;
  SSchema        sstempl;
  XTigerTemplate t;
  int            firstChar, lastChar, firstChar2, lastChar2;

  if (!TtaGetDocumentAccessMode(doc))
    return;

  sstempl = TtaGetSSchema ("Template", doc);
  if (sstempl && IsTemplateDocument(doc) && !IsTemplateInstanceDocument(doc))
    {
      TtaGiveFirstSelectedElement (doc, &selElem, &firstChar, &lastChar);
      TtaGiveLastSelectedElement (doc, &selElem2, &firstChar2, &lastChar2);
      t = GetXTigerDocTemplate (doc);
      if (selElem == NULL || t == NULL || selElem != selElem2 || lastChar > firstChar)
        return;

      elType =  TtaGetElementType(selElem);
      if (elType.ElSSchema == sstempl)
        {
          // locate the position of the new import
          prev = next = NULL;
          if (elType.ElSSchema == sstempl &&
              elType.ElTypeNum == Template_EL_component)
            next = selElem;
          else if (elType.ElSSchema == sstempl &&
                   (elType.ElTypeNum == Template_EL_union ||
                    elType.ElTypeNum == Template_EL_import))
            prev = selElem;
          else if (elType.ElSSchema == sstempl &&
                   (elType.ElTypeNum == Template_EL_head ||
                    elType.ElTypeNum == Template_EL_Template))
            parent = selElem;
          else
            {
              parent = TemplateGetParentHead (selElem, doc);
              if (parent == NULL)
                return;
              elType.ElSSchema = sstempl;
              elType.ElTypeNum = Template_EL_component;
              prev = TtaGetExactTypedAncestor (selElem, elType);
            }
          // create the import
          elType.ElTypeNum = Template_EL_import;
          el = TtaNewElement (doc, elType);
          if (prev)
            TtaInsertSibling (el, prev, FALSE, doc);
          else if (next)
            TtaInsertSibling (el, next, TRUE, doc);
          else
            TtaInsertFirstChild (&el, parent, doc);
          TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
          TtaRegisterElementCreate (el, doc);
          TtaSelectElement (doc, el);
          LinkAsImport = TRUE;
          SelectDestination (doc, el, TRUE, FALSE);
        }
    }
#endif /* TEMPLATES */
}


/*----------------------------------------------------------------------
  TemplateCreateFreeBox
  Create a xt:bag types="string" box around the selection.
  ----------------------------------------------------------------------*/
void TemplateCreateFreeBox (Document doc, View view)
{
#ifdef TEMPLATES
  ThotBool        oldStructureChecking;
  DisplayMode     dispMode;
  Element         selElem, selElem2, parent, parent2, current;
  Element         bag, child = NULL;
  ElementType     selType, selType2, bagType;
  SSchema         sstempl, sshtml;
  int             firstChar, lastChar, firstChar2, lastChar2;
  char        buffer[128],  *title, *label;

  if (!TtaGetDocumentAccessMode(doc))
    return;

  sstempl = TtaGetSSchema ("Template", doc);
  if (doc && sstempl &&
      IsTemplateDocument(doc) && !IsTemplateInstanceDocument(doc))
    {
      TtaGiveFirstSelectedElement(doc, &selElem, &firstChar, &lastChar);
      if (selElem)
        {
          TtaGiveLastSelectedElement(doc, &selElem2, &firstChar2, &lastChar2);
          sshtml  = TtaGetSSchema ("HTML", doc);
          selType = TtaGetElementType(selElem);
          selType2 = TtaGetElementType(selElem2);
          bagType.ElSSchema = sstempl;
          bagType.ElTypeNum = Template_EL_bag;
          parent = TtaGetParent(selElem);
          parent2 = TtaGetParent(selElem2);
          if (firstChar != 0 || firstChar2 != 0 || parent != parent2)
            {
              TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_INVALID_SELECTION);
              return;
            }
          else if ((selType.ElSSchema == sshtml &&
                    (selType.ElTypeNum == HTML_EL_List_Item ||
                     selType.ElTypeNum == HTML_EL_Term ||
                     selType.ElTypeNum == HTML_EL_Definition ||
                     selType.ElTypeNum == HTML_EL_Option_item ||
                     selType.ElTypeNum == HTML_EL_CAPTION ||
                     selType.ElTypeNum == HTML_EL_Table_row ||
                     selType.ElTypeNum == HTML_EL_thead ||
                     selType.ElTypeNum == HTML_EL_tbody ||
                     selType.ElTypeNum == HTML_EL_tfoot)) ||
                   (selType2.ElSSchema == sshtml &&
                    (selType2.ElTypeNum == HTML_EL_List_Item ||
                     selType2.ElTypeNum == HTML_EL_Term ||
                     selType2.ElTypeNum == HTML_EL_Definition ||
                     selType2.ElTypeNum == HTML_EL_Option_item ||
                     selType2.ElTypeNum == HTML_EL_CAPTION ||
                     selType2.ElTypeNum == HTML_EL_Table_row ||
                     selType2.ElTypeNum == HTML_EL_thead ||
                     selType2.ElTypeNum == HTML_EL_tbody ||
                     selType2.ElTypeNum == HTML_EL_tfoot)))
            {
              TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_INVALID_SELECTION);
              return;
            }
          else if (selElem != selElem2 &&
                   ((selType.ElSSchema == sshtml &&
                    (selType.ElTypeNum == HTML_EL_Table_cell ||
                     selType.ElTypeNum == HTML_EL_Data_cell ||
                     selType.ElTypeNum == HTML_EL_Heading_cell)) ||
                   (selType2.ElSSchema == sshtml &&
                    (selType2.ElTypeNum == HTML_EL_Table_cell ||
                     selType2.ElTypeNum == HTML_EL_Data_cell ||
                     selType2.ElTypeNum == HTML_EL_Heading_cell))))
            {
              TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_INVALID_SELECTION);
              return;
            }

          // request the element label
          title = TtaGetMessage (AMAYA, AM_TEMPLATE_BAGANY);
          label = TtaGetMessage (AMAYA, AM_TEMPLATE_LABEL);
          QueryStringFromUser(label, title, buffer, 32);
          if (buffer[0] == EOS)
            return;

          dispMode = TtaGetDisplayMode (doc);
          if (dispMode == DisplayImmediately)
            TtaSetDisplayMode (doc, DeferredDisplay);
          oldStructureChecking = TtaGetStructureChecking (doc);
          TtaSetStructureChecking (FALSE, doc);

          // Create and insert xt:bag element
          bag = TtaNewElement(doc, bagType);
          TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
          if (selType.ElSSchema == sshtml &&
              (selType.ElTypeNum == HTML_EL_Table_cell ||
               selType.ElTypeNum == HTML_EL_Data_cell ||
               selType.ElTypeNum == HTML_EL_Heading_cell))
            {
              // change the selection to children
              selElem = TtaGetFirstChild (selElem);
              selElem2 = TtaGetLastChild (selElem2);
            }

          TtaInsertSibling (bag, selElem, TRUE, doc);
          SetAttributeStringValue (bag, Template_ATTR_types, "any");
          Template_SetLabel (doc, bag, buffer, FALSE);
          TtaRegisterElementCreate (bag, doc);
          // move the selection into the new bag
          TtaNextSibling (&selElem2);
          while (selElem != selElem2)
            {
              current = selElem;
              TtaNextSibling(&selElem);
              TtaRegisterElementDelete (current, doc);
              TtaRemoveTree (current, doc);
              if (child)
                TtaInsertSibling(current, child, FALSE, doc);
              else
                TtaInsertFirstChild(&current, bag, doc);
              TtaRegisterElementDelete (current, doc);
              child = current;
            }

          TtaSetDisplayMode (doc, dispMode);
          TtaCloseUndoSequence(doc);
          TtaSelectElement (doc, bag);
          // update the status bar
          TtaSetStatusSelectedElement (doc, 1, bag);
          TtaSetStructureChecking (oldStructureChecking, doc);
        }
    }
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  TemplateCreateUnion
  Create a xt:union around the selection.
  ----------------------------------------------------------------------*/
void TemplateCreateUnion (Document doc, View view)
{
#ifdef TEMPLATES
  SSchema        sstempl = TtaGetSSchema ("Template", doc);
  Element        head, sibling, unionEl, selElem;
  ElementType    unionType, selType;
  XTigerTemplate t = GetXTigerDocTemplate(doc);
  char          *proposed, *name = NULL, *types=NULL;
  int            firstChar, lastChar;

  if (t && sstempl)
    {
      TtaGiveFirstSelectedElement(doc, &selElem, &firstChar, &lastChar);
      selType =  TtaGetElementType(selElem);
      if (selType.ElSSchema == sstempl && selType.ElTypeNum == Template_EL_union)
        Template_ModifyUnionElement(doc, selElem);
      else
        {
          proposed = Template_GetAllDeclarations(t, TRUE, FALSE, TRUE);
          if (QueryUnionFromUser(proposed, NULL, &name, &types, TRUE))
            {
              TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
              head = TemplateFindHead(doc);
              sibling = TtaGetLastChild(head);
              unionType.ElSSchema = sstempl;
              unionType.ElTypeNum = Template_EL_union;
              unionEl = TtaNewElement(doc, unionType);

              if (sibling)
                TtaInsertSibling(unionEl, sibling, FALSE, doc);
              else
                {
                  sibling = unionEl;
                  TtaInsertFirstChild(&sibling, head, doc);
                }
              Template_SetName(doc, unionEl, name, FALSE);
              SetAttributeStringValue(unionEl, Template_ATTR_includeAt, types);
              TtaSetAccessRight(unionEl, ReadOnly, doc);
              TtaRegisterElementCreate(unionEl, doc);
              TtaSelectElement(doc, unionEl);
              TtaCloseUndoSequence(doc);
              Template_DeclareNewUnion (t, name, types, "");
              TtaFreeMemory(proposed);
              TtaFreeMemory(name);
              TtaFreeMemory(types);
            }
        }
    }
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Template_ModifyUnionElement
  Query the user to modify an xt:union
  ----------------------------------------------------------------------*/
void Template_ModifyUnionElement(Document doc, Element unionEl)
{
  XTigerTemplate t = GetXTigerDocTemplate(doc);
  SSchema        sstempl = TtaGetSSchema ("Template", doc);
  ElementType    unionType;
  char          *proposed, *checked, *name, *types = NULL;

  if (doc && unionEl && t && sstempl)
    {
      unionType = TtaGetElementType(unionEl);
      if (unionType.ElSSchema == sstempl && unionType.ElTypeNum == Template_EL_union)
        {
          proposed = Template_GetAllDeclarations(t, TRUE, FALSE, TRUE);
          checked = GetAttributeStringValueFromNum(unionEl, Template_ATTR_includeAt, NULL);
          name = GetAttributeStringValueFromNum(unionEl, Template_ATTR_name, NULL);
          if (QueryUnionFromUser(proposed, checked, &name, &types, FALSE))
            {
              TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
              SetAttributeStringValueWithUndo(unionEl, Template_ATTR_includeAt, types);
              TtaCloseUndoSequence(doc);
            }
          TtaFreeMemory(proposed);
          TtaFreeMemory(checked);
          TtaFreeMemory(name);
          TtaFreeMemory(types);
        }
    }
}

/*----------------------------------------------------------------------
  TemplateCreateRepeat
  Create a xt:repeat around the selection.
  ----------------------------------------------------------------------*/
void TemplateCreateRepeat (Document doc, View view)
{
  Template_CreateRepeatFromSelection (doc, view, FALSE);
}

/*----------------------------------------------------------------------
  TemplateCreateRepeatComp
  Create a xt:component with the selection and use it in a new xt:repeat
  ----------------------------------------------------------------------*/
void TemplateCreateRepeatComp (Document doc, View view)
{
  Template_CreateRepeatFromSelection (doc, view, TRUE);
}

/*----------------------------------------------------------------------
  Template_CreateRepeatFromSelection
  Create a xt:repeat with the selection.
  If selection is empty, insert an inline xt:use.
  If createComp is false, create a xt:use with types at the selected block.
  If createComp is true, create a component with the selection.
  Return the xt:use element.
  ----------------------------------------------------------------------*/
Element Template_CreateRepeatFromSelection (Document doc, int view,
                                            ThotBool createComp)
{
#ifdef TEMPLATES
  ThotBool    oldStructureChecking;
  DisplayMode dispMode;
  Element     selElem, selElem2, parent, parent2, rep = NULL, use;
  ElementType selType, selType2, repType;
  int         firstChar, lastChar, firstChar2, lastChar2;
  SSchema     sstempl = TtaGetSSchema ("Template", doc);
  const char *title, *label;
  char        buffer[128], *types;

  if (!TtaGetDocumentAccessMode(doc))
    return NULL;

  if (doc && TtaGetDocumentAccessMode(doc) && sstempl &&
      IsTemplateDocument(doc) && !IsTemplateInstanceDocument(doc))
    {
      TtaGiveFirstSelectedElement(doc, &selElem, &firstChar, &lastChar);
      selType =  TtaGetElementType(selElem);
      TtaGiveLastSelectedElement(doc, &selElem2, &firstChar2, &lastChar2);
      selType2 =  TtaGetElementType(selElem2);
      repType.ElSSchema = sstempl;
      repType.ElTypeNum = Template_EL_repeat;
      parent  = TtaGetParent(selElem);
      parent2  = TtaGetParent(selElem2);
      if (selElem && selElem2 && firstChar == 0 && firstChar2 == 0 &&
          parent == parent2)
        {
          oldStructureChecking = TtaGetStructureChecking (doc);
          if (selElem == selElem2 && selType.ElSSchema == sstempl &&
              (selType.ElTypeNum == Template_EL_useEl ||
               selType.ElTypeNum == Template_EL_useSimple))
            use = selElem;
          else
            // create first the use element
            use = Template_CreateUseFromSelection (doc, view, createComp);
          if (use)
            {
              // avoid to repeat a use string
              types = GetAttributeStringValueFromNum (use, Template_ATTR_types, NULL);
              if (types && strcmp (types, "string"))
                {
                  TtaFreeMemory (types);
                  // request the element label
                  title = TtaGetMessage (AMAYA, AM_TEMPLATE_REPEATCOMP);
                  label = TtaGetMessage (AMAYA, AM_TEMPLATE_REPEATLABEL);
                  QueryStringFromUser(label, title, buffer, 32);
                  if (buffer[0] == EOS)
                    return NULL;
                  if (use == selElem)
                    TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
                  else
                    TtaExtendUndoSequence(doc);
                  dispMode = TtaGetDisplayMode (doc);
                  if (dispMode == DisplayImmediately)
                    TtaSetDisplayMode (doc, DeferredDisplay);
                  TtaSetStructureChecking (FALSE, doc);
                  // create the repeat element
                  rep = TtaNewElement(doc, repType);
                  TtaInsertSibling(rep, use, FALSE, doc);
                  Template_SetLabel (doc, rep, buffer, FALSE);
                  TtaRegisterElementCreate (rep, doc);
                  // move the use element inot the repeat
                  TtaRegisterElementDelete (use, doc);
                  TtaRemoveTree(use, doc);
                  TtaInsertFirstChild (&use, rep, doc);
                  TtaRegisterElementCreate (use, doc);
                  TtaSetDisplayMode (doc, dispMode);
                  TtaSelectElement (doc, use);
                  TtaCloseUndoSequence (doc);
                  TtaSetStructureChecking (oldStructureChecking, doc);
                  // register document modification
                  TtaSetDocumentModified (doc);
                }
            }
          return rep;
        }
      else
        {
          TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_INVALID_SELECTION);
          return NULL;
        }
    }
#endif /* TEMPLATES */
  return NULL;
}

/*----------------------------------------------------------------------
  TemplateCreateUseBox
  Create a xt:use around the selection.
  ----------------------------------------------------------------------*/
void TemplateCreateUseBox (Document doc, View view)
{
#ifdef TEMPLATES
  Template_CreateUseFromSelection (doc, view, FALSE);
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  TemplateCreateUseCompBox
  Create a xt:use around the selection.
  ----------------------------------------------------------------------*/
void TemplateCreateUseCompBox (Document doc, View view)
{
#ifdef TEMPLATES
  Template_CreateUseFromSelection (doc, view, TRUE);
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Template_CreateInlineUse
  Create an inline xt:use with the selection.
  ----------------------------------------------------------------------*/
static Element Template_CreateInlineUse (Document doc)
{
#ifdef TEMPLATES
  SSchema        sstempl = TtaGetSSchema ("Template", doc);
  Element        selElem, use = NULL;
  ElementType    selType;
  XTigerTemplate t = GetXTigerDocTemplate(doc);
  Declaration    dec;
  char          *proposed, *label = NULL, *types=NULL;
  int            firstChar, lastChar;
  ThotBool       option;

  if (t && sstempl)
    {
      TtaGiveFirstSelectedElement (doc, &selElem, &firstChar, &lastChar);
      if (selElem == NULL)
        return NULL;
      proposed = Template_GetInlineLevelDeclarations(t, TRUE, TRUE);
      if (proposed &&
          QueryNewUseFromUser (proposed, &label, &types, &option))
        {
          if (label && types)
            {
              TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
              if (strstr (types, " ") != NULL)
                GenerateInlineElement (Template_EL_useEl, sstempl, 0, NULL, TRUE);
              else
                {
                  // check if it's an union
                  dec = Template_GetUnionDeclaration (t, types);
                  if (dec)
                    GenerateInlineElement (Template_EL_useEl, sstempl, 0, NULL, TRUE);
                  else
                    GenerateInlineElement (Template_EL_useSimple, sstempl, 0, NULL, TRUE);
                }

              // complete the creation of the use element
              TtaGiveFirstSelectedElement (doc, &use, &firstChar, &lastChar);
              selType = TtaGetElementType (use);
              while (use &&
                     (selType.ElSSchema != sstempl ||
                      selType.ElTypeNum != Template_EL_useSimple))
                {
                  // look for the enclosing use element
                  use = TtaGetParent (use);
                  selType = TtaGetElementType (use);
                }
              SetAttributeStringValueWithUndo (use, Template_ATTR_types, types);
              Template_SetLabel (doc, use, label, TRUE);
              TtaSelectElement (doc, use);
              TtaCloseUndoSequence (doc);
            }
        }
      TtaFreeMemory (proposed);
      TtaFreeMemory (label);
      TtaFreeMemory (types);
      return use;
    }
#endif /* TEMPLATES */
  return NULL;
}

/*----------------------------------------------------------------------
  Template_CreateEmptyBlockUse
  Create a block xt:use with the selection.
  ----------------------------------------------------------------------*/
static Element Template_CreateEmptyBlockUse (Document doc)
{
#ifdef TEMPLATES
  SSchema        sstempl = TtaGetSSchema ("Template", doc);
  ElementType    useType;
  Element        selElem, use = NULL;
  XTigerTemplate t = GetXTigerDocTemplate(doc);
  Declaration    dec;
  char          *proposed, *label = NULL, *types=NULL;
  int            firstChar, lastChar;
  ThotBool       option;

  if (t && sstempl)
    {
      TtaGiveFirstSelectedElement (doc, &selElem, &firstChar, &lastChar);
      if (selElem == NULL)
        return NULL;

      proposed = Template_GetBlockLevelDeclarations(t, TRUE);
      if (proposed &&
          QueryNewUseFromUser(proposed, &label, &types, &option))
        {
          if (label && types)
            {
              TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
              // create the use element
              useType.ElSSchema = sstempl;
              if (strstr (types, " ") != NULL)
                useType.ElTypeNum = Template_EL_useEl;
              else
                {
                  // check if it's an union
                  dec = Template_GetUnionDeclaration (t, types);
                  if (dec)
                    useType.ElTypeNum = Template_EL_useEl;
                  else
                    useType.ElTypeNum = Template_EL_useSimple;
                }
              use = TtaNewElement (doc, useType);
              TtaInsertSibling (use, selElem, FALSE, doc);
              SetAttributeStringValue (use, Template_ATTR_types, types);
              Template_SetLabel (doc, use, label, FALSE);
              TtaRegisterElementCreate (use, doc);
              // remove the current selection
              TtaDeleteTree (selElem, doc);
              TtaSelectElement (doc, use);
              TtaCloseUndoSequence (doc);
            }
        }
      TtaFreeMemory (proposed);
      TtaFreeMemory (label);
      TtaFreeMemory (types);
      return use;
    }

#endif /* TEMPLATES */
  return NULL;
}

/*----------------------------------------------------------------------
  Template_CreateUseFromSelection
  Create a xt:use with the selection.
  If selection is empty, insert an inline xt:use.
  If createComp is false, create a xt:use with types at the selected block.
  If createComp is true, create a component with the selection.
  Return the xt:use element.
  ----------------------------------------------------------------------*/
Element Template_CreateUseFromSelection (Document doc, int view, ThotBool createComp)
{
#ifdef TEMPLATES
  DisplayMode    dispMode;
  Element        selElem, selElem2, parent, parent2;
  Element        use = NULL, comp, prev, next;
  ElementType    selType, selType2, useType, compType;
  SSchema        sstempl, sshtml;
  XTigerTemplate t;
  int            firstChar, lastChar, firstChar2, lastChar2;
  int            sz = 128, option;
  char          *proposed, *checked, *types = NULL;
  char           buffer[128];
  const char    *title, *label, *name = NULL;
  ThotBool       oldStructureChecking, opt = FALSE, getContent = FALSE;

  if (!TtaGetDocumentAccessMode(doc))
    return NULL;

  sstempl = TtaGetSSchema ("Template", doc);
  sshtml  = TtaGetSSchema ("HTML", doc);
  buffer[0] = EOS;
  if (doc && TtaGetDocumentAccessMode(doc) && sstempl &&
      IsTemplateDocument(doc) && !IsTemplateInstanceDocument(doc))
    {
      TtaGiveFirstSelectedElement (doc, &selElem, &firstChar, &lastChar);
      TtaGiveLastSelectedElement (doc, &selElem2, &firstChar2, &lastChar2);
      t = GetXTigerDocTemplate (doc);
      if (selElem == NULL || t == NULL)
        return NULL;

      selType =  TtaGetElementType(selElem);
      selType2 =  TtaGetElementType(selElem2);
      parent  = TtaGetParent(selElem);
      parent2 = TtaGetParent(selElem2);
      dispMode = TtaGetDisplayMode (doc);
      oldStructureChecking = TtaGetStructureChecking (doc);
      TtaSetStructureChecking (FALSE, doc);
      TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);

      if (firstChar == 0 && firstChar2 == 0 && parent == parent2)
        {
          if (selType.ElSSchema == sstempl &&
              (selType.ElTypeNum == Template_EL_useEl ||
               selType.ElTypeNum == Template_EL_useSimple))
            {
              // The selection is a template element.
              // TODO query to add/remove some types
              if (TtaGetElementVolume(selElem) == 0)
                {
                  proposed = Template_GetAllDeclarations(t, TRUE, TRUE, TRUE);
                  checked = GetAttributeStringValueFromNum (selElem, Template_ATTR_types, NULL);
                  option = GetAttributeIntValueFromNum (selElem, Template_ATTR_option);
                  opt = (option == Template_ATTR_option_VAL_option_set);
                  if (QueryUseFromUser (proposed, checked, buffer, 128, &types, &opt))
                    {
                      SetAttributeStringValueWithUndo(selElem, Template_ATTR_types, types);
                      SetAttributeStringValueWithUndo(selElem, Template_ATTR_title, buffer);
                      if (opt)
                        SetAttributeIntValue (selElem, Template_ATTR_option, Template_ATTR_option_VAL_option_set, TRUE);
                      else
                        SetAttributeIntValue (selElem, Template_ATTR_option, Template_ATTR_option_VAL_option_unset, TRUE);
                      // register document modification
                      TtaSetDocumentModified (doc);
                    }
                  TtaFreeMemory (proposed);
                  TtaFreeMemory (checked);
                  TtaFreeMemory (types);
                  createComp = FALSE;
                }
            }
          else if (selElem != selElem2 &&
                   ((selType.ElSSchema == sshtml &&
                    (selType.ElTypeNum == HTML_EL_List_Item ||
                     selType.ElTypeNum == HTML_EL_Term ||
                     selType.ElTypeNum == HTML_EL_Definition ||
                     selType.ElTypeNum == HTML_EL_Option_item ||
                     selType.ElTypeNum == HTML_EL_CAPTION ||
                     selType.ElTypeNum == HTML_EL_Table_row ||
                     selType.ElTypeNum == HTML_EL_Table_cell ||
                     selType.ElTypeNum == HTML_EL_Data_cell ||
                     selType.ElTypeNum == HTML_EL_Heading_cell ||
                     selType.ElTypeNum == HTML_EL_thead ||
                     selType.ElTypeNum == HTML_EL_tbody ||
                     selType.ElTypeNum == HTML_EL_tfoot)) ||
                   (selType2.ElSSchema == sshtml &&
                    (selType2.ElTypeNum == HTML_EL_List_Item ||
                     selType2.ElTypeNum == HTML_EL_Term ||
                     selType2.ElTypeNum == HTML_EL_Definition ||
                     selType2.ElTypeNum == HTML_EL_Option_item ||
                     selType2.ElTypeNum == HTML_EL_CAPTION ||
                     selType2.ElTypeNum == HTML_EL_Table_row ||
                     selType2.ElTypeNum == HTML_EL_Table_cell ||
                     selType2.ElTypeNum == HTML_EL_Data_cell ||
                     selType2.ElTypeNum == HTML_EL_Heading_cell ||
                     selType2.ElTypeNum == HTML_EL_thead ||
                     selType2.ElTypeNum == HTML_EL_tbody ||
                     selType2.ElTypeNum == HTML_EL_tfoot))))
            {
              TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_INVALID_SELECTION);
              return NULL;
            }
          else if (selType.ElSSchema == sshtml &&
                   selElem == selElem2 &&
                   (selType.ElTypeNum == HTML_EL_Element ||
                    selType.ElTypeNum == HTML_EL_Basic_Elem ||
                    (selType.ElTypeNum == HTML_EL_Paragraph &&
                     TtaGetElementVolume(selElem) == 0)))
            // Create a block-level xt:use instead of selection.
            use = Template_CreateEmptyBlockUse (doc);
          else if (selType.ElSSchema == sstempl)
            {
              // locate the position of the new component
              prev = next = NULL;
              if (selType.ElSSchema == sstempl &&
                  selType.ElTypeNum == Template_EL_component)
                next = selElem;
              else if (selType.ElSSchema == sstempl &&
                       (selType.ElTypeNum == Template_EL_union ||
                        selType.ElTypeNum == Template_EL_import))
                prev = selElem;
              else if (selType.ElSSchema == sstempl &&
                       (selType.ElTypeNum == Template_EL_head ||
                        selType.ElTypeNum == Template_EL_Template))
                parent = selElem;
              else
                {
                  selType.ElSSchema = sstempl;
                  selType.ElTypeNum = Template_EL_head;
                  parent = TtaGetExactTypedAncestor (selElem, selType);
                  if (parent == NULL)
                    {
                      selType.ElTypeNum = Template_EL_Template;
                      parent = TtaGetExactTypedAncestor (selElem, selType);
                    }
                  if (parent == NULL)
                    return NULL;
                  selType.ElTypeNum = Template_EL_component;
                  prev = TtaGetExactTypedAncestor (selElem, selType);
                }
              
              if (QueryComponentFromUser (buffer, 128))
                {
                  // create the component
                  compType.ElSSchema = sstempl;
                  compType.ElTypeNum = Template_EL_component;
                  comp = TtaNewElement (doc, compType);
                  if (prev)
                    TtaInsertSibling (comp, prev, FALSE, doc);
                  else if (next)
                    TtaInsertSibling (comp, next, TRUE, doc);
                  else
                    TtaInsertFirstChild (&comp, parent, doc);
                  SetAttributeStringValue (comp, Template_ATTR_name, buffer);
                  // generate a content
                  selElem = Template_FillEmpty (comp, doc, FALSE);
                  TtaRegisterElementCreate (comp, doc);
                  TtaSelectElement (doc, selElem);
                  // register document modification
                  TtaSetDocumentModified (doc);
                }
            }
          else
            {
              if (dispMode == DisplayImmediately)
                TtaSetDisplayMode (doc, DeferredDisplay);
              prev = selElem;
              TtaPreviousSibling(&prev);
              next = selElem2;
              TtaNextSibling (&next);
              if (selElem == selElem2 &&
                  selType2.ElSSchema == sshtml &&
                  (selType.ElTypeNum == HTML_EL_List_Item ||
                   selType.ElTypeNum == HTML_EL_Term ||
                   selType.ElTypeNum == HTML_EL_Definition ||
                   selType.ElTypeNum == HTML_EL_Option_item ||
                   selType.ElTypeNum == HTML_EL_CAPTION ||
                   selType.ElTypeNum == HTML_EL_Table_row ||
                   selType.ElTypeNum == HTML_EL_Table_cell ||
                   selType.ElTypeNum == HTML_EL_Data_cell ||
                   selType.ElTypeNum == HTML_EL_Heading_cell ||
                   selType.ElTypeNum == HTML_EL_thead ||
                   selType.ElTypeNum == HTML_EL_tbody ||
                   selType.ElTypeNum == HTML_EL_tfoot))
                {
                  // special management
                  createComp = FALSE;
                  if (selType.ElTypeNum == HTML_EL_Table_cell ||
                      selType.ElTypeNum == HTML_EL_Data_cell ||
                      selType.ElTypeNum == HTML_EL_Heading_cell)
                    // generate the use element around the first child
                    // instead of the cell
                    getContent = TRUE;
                }
              if (createComp)
                {
                  // Create a component from selection and a use using it
                  comp = Template_CreateComponentFromSelection (doc);
                  if (comp)
                    {
                      useType.ElSSchema = sstempl;
                      useType.ElTypeNum = Template_EL_useSimple;
                      use = TtaNewElement (doc, useType);
                      if (prev)
                        TtaInsertSibling (use, prev, FALSE, doc);
                      else if (next)
                        TtaInsertSibling (use, next, TRUE, doc);
                      else
                        TtaInsertFirstChild (&use, parent, doc);
                      GiveAttributeStringValueFromNum (comp, Template_ATTR_name, buffer, &sz);
                      // Declare the new component
                      Template_DeclareNewComponent (t, buffer, comp, 2);
                      SetAttributeStringValue (use, Template_ATTR_types, buffer);
                      SetAttributeStringValue (use, Template_ATTR_currentType, buffer);
                      SetAttributeStringValue (use, Template_ATTR_title, buffer);
                      TtaRegisterElementCreate (use, doc);
                      TtaSelectElement (doc, use);
                      // register document modification
                      TtaSetDocumentModified (doc);
                    }
                }
              else
                {
                  title = TtaGetMessage (AMAYA, AM_TEMPLATE_USELABEL);
                  label = TtaGetMessage (AMAYA, AM_NAME);
                  QueryStringFromUser (label, title, buffer, 32);
                  if (buffer[0] != EOS)
                    {
                      // Create a xt:use around the selection
                      useType.ElSSchema = sstempl;
                      useType.ElTypeNum = Template_EL_useSimple;
                      use = TtaNewElement (doc, useType);
                      if (getContent)
                        {
                          parent = selElem;
                          selElem = TtaGetFirstChild (parent);
                          selElem2 = TtaGetLastChild (parent);
                          prev = next = NULL;
                        }
                      if (selElem)
                        {
                          name = TtaGetElementTypeName(TtaGetElementType(selElem));
                          TtaRegisterElementDelete (selElem, doc);
                          TtaRemoveTree(selElem, doc);
                          TtaInsertFirstChild (&selElem, use, doc);
                        }
                      else
                        {
                          useType.ElTypeNum = Template_EL_TEXT_UNIT;
                          selElem = TtaNewElement (doc, useType);
                          TtaInsertFirstChild (&selElem, use, doc);
                        }
                      if (prev)
                        TtaInsertSibling(use, prev, FALSE, doc);
                      else if (next)
                        TtaInsertSibling(use, next,TRUE, doc);
                      else
                        TtaInsertFirstChild(&use, parent, doc);
                      if (name && strcmp (name, "???") &&
                          strcmp (name, "Pseudo_paragraph"))
                        {
                          SetAttributeStringValue (use, Template_ATTR_types, name);
                          SetAttributeStringValue (use, Template_ATTR_currentType, name);
                        }
                      else
                        {
                          SetAttributeStringValue (use, Template_ATTR_types, "string");
                          SetAttributeStringValue (use, Template_ATTR_currentType, "string");
                        }
                      Template_SetLabel (doc, use, buffer, FALSE);
                      TtaRegisterElementCreate (use, doc);
                      TtaSelectElement (doc, use);
                      // register document modification
                      TtaSetDocumentModified (doc);
                    }
                }
              if (dispMode == DisplayImmediately)
                TtaSetDisplayMode (doc, dispMode);
            }
        }
      else
        {
          if (lastChar < firstChar && !createComp)
            use = Template_CreateInlineUse (doc);
          else
            Template_CreateTextBox (doc, createComp);
        }
      TtaSetStructureChecking (oldStructureChecking, doc);
      TtaCloseUndoSequence(doc);
    }
  return use;
#else /* TEMPLATES */
  return NULL;
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Template_CreateComponentFromSelection
  Create a xt:component with the selection and move it into the xt:head.
  Return the xt:component element.
  ----------------------------------------------------------------------*/
Element Template_CreateComponentFromSelection (Document doc)
{
#ifdef TEMPLATES
  ThotBool       oldStructureChecking;
  DisplayMode    dispMode;
  Element        selElem, selElem2, parent, parent2, current, child, head;
  Element        comp = NULL;
  ElementType    selType, selType2, compType;
  SSchema        sstempl = TtaGetSSchema ("Template", doc);
  XTigerTemplate t = GetXTigerDocTemplate(doc);
  int            firstChar, lastChar, firstChar2, lastChar2;
  char           buffer[128];
  const char    *title, *label;
  ThotBool       closeUndo = FALSE;

  if (doc && t && TtaGetDocumentAccessMode(doc) &&
      TtaGetDocumentAccessMode(doc) && sstempl &&
      IsTemplateDocument(doc) && !IsTemplateInstanceDocument(doc))
    {
      TtaGiveFirstSelectedElement (doc, &selElem, &firstChar, &lastChar);
      if (selElem == NULL)
        {
          TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_INVALID_SELECTION);
          return NULL;
        }
      TtaGiveLastSelectedElement (doc, &selElem2, &firstChar2, &lastChar2);
      selType = TtaGetElementType(selElem);
      selType2 = TtaGetElementType(selElem2);
      parent  = TtaGetParent(selElem);
      parent2 = TtaGetParent(selElem2);
      label = TtaGetMessage (AMAYA, AM_NAME);
      title = TtaGetMessage (AMAYA, AM_TEMPLATE_NEWCOMP);
      QueryStringFromUser(label, title, buffer, 32);
      if (buffer[0] != EOS)
        {
          head = TemplateFindHead (doc);
          compType.ElSSchema = sstempl;
          compType.ElTypeNum = Template_EL_component;
          if (head && firstChar == 0 && firstChar2 == 0 && parent == parent2)
            {
              dispMode = TtaGetDisplayMode (doc);
              oldStructureChecking = TtaGetStructureChecking (doc);
              TtaSetStructureChecking (FALSE, doc);
              child = TtaGetLastChild (head);
              if (TtaHasUndoSequence (doc))
                closeUndo = FALSE;
              else
                {
                  closeUndo = TRUE;
                  TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
                }

              if (dispMode == DisplayImmediately)
                TtaSetDisplayMode (doc, DeferredDisplay);
              // Create the component
              comp = TtaNewElement(doc, compType);
              if (child)
                TtaInsertSibling(comp, child, FALSE, doc);
              else
                TtaInsertFirstChild (&comp, head, doc);
              // register and update the component name
              Template_SetName (doc, comp, buffer, FALSE);

              TtaNextSibling (&selElem2);
              child = NULL;
              while (selElem != selElem2)
                {
                  current = selElem;
                  TtaNextSibling (&selElem);
                  TtaRegisterElementDelete (current, doc);
                  TtaRemoveTree (current, doc);
                  if (child)
                    TtaInsertSibling (current, child, FALSE, doc);
                  else
                    TtaInsertFirstChild (&current, comp, doc);
                  //TtaRegisterElementCreate (current, doc);
                  child = current;
                }
              TtaRegisterElementCreate (comp, doc);
              if (closeUndo)
                TtaCloseUndoSequence(doc);
              TtaSetStructureChecking (oldStructureChecking, doc);
              if (dispMode == DisplayImmediately)
                TtaSetDisplayMode (doc, dispMode);
            }
        }
    }
  return comp;
#else /* TEMPLATES */
  return NULL;
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  TemplateComponentWillBeDeleted
  Processed when a component element will be deleted in a template context.
  ----------------------------------------------------------------------*/
ThotBool TemplateComponentWillBeDeleted (NotifyElement *event)
{
#ifdef TEMPLATES
  XTigerTemplate t = GetXTigerDocTemplate(event->document);
  char          *elemName;

  elemName = GetAttributeStringValueFromNum(event->element, Template_ATTR_name, NULL);
  if (Template_IsUsedComponent (t, event->document, elemName))
    {
      TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_TEMPLATE_USEDCOMP_CANTREMOVE);
      return TRUE;
    }
#endif /* TEMPLATES */
  return FALSE;
}

/*----------------------------------------------------------------------
  UnionClicked
  Called when a xt:union is clicked
  ----------------------------------------------------------------------*/
void UnionClicked(NotifyElement* event)
{
  if (event->document && event->element)
    TtaSelectElement (event->document, event->element);
}

/*----------------------------------------------------------------------
  UnionDoubleClicked
  Called when a xt:union is double clicked
  ----------------------------------------------------------------------*/
ThotBool UnionDoubleClicked(NotifyElement* event)
{
  Template_ModifyUnionElement(event->document, event->element);
  TtaSelectElement(event->document, event->element);
  return TRUE;
}

/*----------------------------------------------------------------------
  TemplateNameAttributeDeleted
  Called when a xt:name will be deleted
  ----------------------------------------------------------------------*/
ThotBool TemplateNameAttributeDeleted(NotifyAttribute* event)
{
  // Prevent xt:name deletion
  return TRUE;
}

/*----------------------------------------------------------------------
  TemplateNameAttributeModified
  Called when a xt:name will be modified
  ----------------------------------------------------------------------*/
ThotBool TemplateNameAttributeModified(NotifyAttribute* event)
{
  // Prevent xt:name modification
  return TRUE;
}

/*----------------------------------------------------------------------
  TemplateNameAttributeCreated
  Called when a xt:name have been created
  ----------------------------------------------------------------------*/
void TemplateNameAttributeCreated(NotifyAttribute* event)
{
  MakeUniqueName(event->element, event->document, TRUE, FALSE);
}


/*----------------------------------------------------------------------
  TemplateNameAttrInMenu
  Called by Thot when building the Attributes menu for template elements.
  ----------------------------------------------------------------------*/
ThotBool TemplateNameAttrInMenu (NotifyAttribute * event)
{
#ifdef TEMPLATES
  ElementType type = TtaGetElementType(event->element);
  if (type.ElTypeNum==Template_EL_component ||
      type.ElTypeNum==Template_EL_union)
    return TRUE;
#endif /* TEMPLATES */
  return FALSE;
}

