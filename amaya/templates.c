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
  t = (XTigerTemplate) Get (Templates_Dic, DocumentMeta[doc]->template_url);
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
  t = (XTigerTemplate) Get (Templates_Dic, DocumentMeta[doc]->template_url);
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
      t = (XTigerTemplate) Get (Templates_Dic, DocumentMeta[doc]->template_url);
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
  t = (XTigerTemplate) Get (Templates_Dic, DocumentMeta[doc]->template_url);
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
              t = (XTigerTemplate) Get (Templates_Dic, content);
              if (!t)
                {
                  LoadTemplate (0, content);
                  t = (XTigerTemplate) Get (Templates_Dic, content);
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
      XTigerTemplate t = (XTigerTemplate) Get (Templates_Dic, turl);
      if (t)
        RemoveUser (t);
      TtaFreeMemory (turl);
    }
#endif /* TEMPLATES */
  return FALSE;
}
