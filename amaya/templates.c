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

struct menuType
{
	char *label;
	int   type;
};

#include "appdialogue_wx.h"
#include "init_f.h"
#include "wxdialogapi_f.h"
#include "templateDeclarations.h"
#endif /* TEMPLATES */

/*----------------------------------------------------------------------
  NewTemplate: Create the "new document from template" dialog
  ----------------------------------------------------------------------*/
void NewTemplate (Document doc, View view)
{
#ifdef TEMPLATES
  int window_id  = TtaGetDocumentWindowId( doc, view );
  ThotWindow p_window = (ThotWindow) TtaGetWindowFromId(window_id);
  char *templateDir = TtaGetEnvString ("TEMPLATES_DIRECTORY");
  ThotBool created;

  if (templates == NULL)
    templates = InitializeTemplateEnvironment();
  created = CreateNewTemplateDocDlgWX(BaseDialog + OpenTemplate,
                                      p_window, doc,
                                      TtaGetMessage (AMAYA, AM_NEW_TEMPLATE),templateDir);
  
  if (created)
    {
      TtaSetDialoguePosition ();
      TtaShowDialogue (BaseDialog + OpenTemplate, TRUE);
    }

  if(templateDir) TtaFreeMemory(templateDir);
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
  ThotBool ShowUseTypes (Document doc) 
  Shows a menu with all the types that can be used in a use element.
  ----------------------------------------------------------------------*/
ThotBool ShowUseTypes (NotifyElement *event)
{
#ifdef TEMPLATES
	Document         doc = event->document;
	Element          el = TtaGetParent(event->element);
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

	//ReturnOption
	return TRUE;
#endif /* TEMPLATES */
}

