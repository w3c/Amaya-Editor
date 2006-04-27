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
#endif
#include "appdialogue_wx.h"
#include "init_f.h"
#include "wxdialogapi_f.h"

/*----------------------------------------------------------------------
  NewTemplate: Create the "new document from template" dialog
  ----------------------------------------------------------------------*/
void NewTemplate (Document doc, View view)
{
#ifdef TEMPLATES
  int window_id  = TtaGetDocumentWindowId( doc, view );

  ThotWindow p_window = (ThotWindow) TtaGetWindowFromId(window_id);

  char *templateDir = TtaGetEnvString ("TEMPLATES_DIRECTORY");
  
  ThotBool created = CreateNewTemplateDocDlgWX(BaseDialog + OpenTemplate,
	p_window, doc, TtaGetMessage (AMAYA, AM_NEW_TEMPLATE),templateDir);
  
  if (created)
    {
      TtaSetDialoguePosition ();
      TtaShowDialogue (BaseDialog + OpenTemplate, TRUE);
    }

  if(templateDir) TtaFreeMemory(templateDir);
#endif
}

#ifdef TEMPLATES

#define COMPONENT 0;
#define SIMPLE_TYPE 1;
#define ELEMENT 2;
#define UNION 3;

struct menuType
{
	char *label;
	int type;
};

static inline bool isEOSorWhiteSpace(char c)
{
	return c==' ' || c=='\t' || c=='\n' || c=='\0';
}

void giveItems(char* text, int size, struct menuType* &items, int &nbitems) {
	bool inElement = true;
	nbitems = 1;

	for(int i=0; i<size; i++) {
		if(isEOSorWhiteSpace(text[i])) {
			if(inElement)
				inElement = false;
		} else if(!inElement) {
				inElement = true;
				++nbitems;
			}
	}

	items = (struct menuType*) TtaGetMemory(sizeof(struct menuType)*nbitems);
	
	char* iter = text;

	char temp[128];
	int labelSize;

	for(i=0; i<nbitems; i++) {		
		labelSize = 0;
		
		while(isEOSorWhiteSpace(*iter)) ++iter;

		while(!isEOSorWhiteSpace(*iter))
			temp[labelSize++]=*iter++;

		temp[labelSize]='\0';

		items[i].label = (char *) TtaStrdup(temp);
		items[i].type = SIMPLE_TYPE;		
	}
}

char *createMenuString(const struct menuType* items, const int nbItems)
{		
	int size = 0;
	for(int i=0; i<nbItems; i++)
		size += 2+strlen(items[i].label);

	char* result = (char *) TtaGetMemory(size);
	char* iter = result;
	
	for(i=0; i<nbItems; i++) {
		*iter = 'B';
		++iter;
		
		strcpy(iter, items[i].label);
		iter += strlen(items[i].label)+1;
	}
	return result;
}

#endif

/*----------------------------------------------------------------------
  ThotBool ShowUseTypes (Document doc) 
  Shows a menu with all the types that can be used in a use element.
  ----------------------------------------------------------------------*/
ThotBool ShowUseTypes (NotifyElement *event)
{
#ifdef TEMPLATES
	Document doc = event->document;
	Element el = TtaGetParent(event->element);
	ElementType elt = TtaGetElementType(el);

	Attribute at;

	AttributeType att;
	
	att.AttrSSchema = elt.ElSSchema;
	att.AttrTypeNum = Template_ATTR_types;

	at = TtaGetAttribute(el, att);


	int size = TtaGetTextAttributeLength(at);
	char *types = (char *) TtaGetMemory (size+1);	
	TtaGiveTextAttributeValue(at, types, &size);
	
	int nbitems;
	struct menuType *items;

	giveItems(types, size, items, nbitems);

	char* menuString = createMenuString(items, nbitems);

	TtaNewScrollPopup (BaseDialog + OptionMenu, TtaGetViewFrame (doc, 1), NULL, 
			       nbitems, menuString , NULL, false, 'L');

	TtaFreeMemory (menuString);
	TtaFreeMemory (types);

	TtaShowDialogue (BaseDialog + OptionMenu, FALSE);
	TtaWaitShowProcDialogue();
	TtaDestroyDialogue (BaseDialog + OptionMenu);

	//ReturnOption

	return true;
#endif
}

