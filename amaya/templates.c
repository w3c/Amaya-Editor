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
#include "templateDeclarations.h"
#include "templateLoad.h"
#include "templateInstanciation.h"

struct menuType
{
	char *label;
	int   type;
};

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
  ThotBool dontRemplace = DontReplaceOldDoc;

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

  LoadTemplate(doc, templatename);
  DontReplaceOldDoc = dontRemplace;
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
      menu[i].type = SIMPLE_TYPE;
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
	int              nbitems, size;
	struct menuType *items;
  char            *types, *menuString;
	
	att.AttrSSchema = elt.ElSSchema;
	att.AttrTypeNum = Template_ATTR_types;
	at = TtaGetAttribute (el, att);

	size = TtaGetTextAttributeLength (at);
	types = (char *) TtaGetMemory (size+1);	
	TtaGiveTextAttributeValue (at, types, &size);

	giveItems (types, size, &items, &nbitems);
	menuString = createMenuString (items, nbitems);
	TtaNewScrollPopup (BaseDialog + OptionMenu, TtaGetViewFrame (doc, 1), NULL, 
                     nbitems, menuString , NULL, false, 'L');

	TtaFreeMemory (menuString);
	TtaFreeMemory (types);

	TtaShowDialogue (BaseDialog + OptionMenu, FALSE);
	TtaWaitShowProcDialogue();
	TtaDestroyDialogue (BaseDialog + OptionMenu);
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
