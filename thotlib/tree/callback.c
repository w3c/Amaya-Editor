/* -- 
 * Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- 
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "modif.h"
#include "app.h"
#include "document.h"

#undef EXPORT
#define EXPORT extern
#include "edit.var"

#include "rdschstr.f"
#include "appinit.f"

/* Global variable pointing to the list of action-definitions. */
extern PtrExternAppliList applicationList;
extern PtrExternAppliList applicationEditor;

/* FUNCTIONS FOR GENERATING THE ACTION-LISTS */
#ifdef __STDC__
void                NoOp (PtrElement pEl)
#else  /* __STDC__ */
void                NoOp (pEl)
PtrElement          pEl;

#endif /* __STDC__ */
{
}


/* ---------------------------------------------------------------------- */
/* |    ExecuteAction looks for the concerned action in schemas.        | */
/* |            It returns True if the executed action takes            | */
/* |            place of the editor action else it returns False.       | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean      ExecuteAction (NotifyEvent * notifyEvent, APPevent event, boolean pre, int type, Element element, PtrSSchema schStruct)

#else  /* __STDC__ */
static boolean      ExecuteAction (notifyEvent, event, pre, type, element, schStruct)
NotifyEvent        *notifyEvent;
APPevent            event;
boolean             pre;
int                 type;
Element             element;
PtrSSchema        schStruct;

#endif /* __STDC__ */
{
   PtrActionBindingList binding;
   PtrExternAppliList  appliActions;
   boolean             ret;
   Proc                doitProc;
   Func                doitFunc;

   doitProc = NULL;
   doitFunc = NULL;

   /* See all actions linked with this event in different schemas */
   while (schStruct != NULL && doitProc == NULL && doitFunc == NULL)
     {
	appliActions = schStruct->SsActionList;
	if (appliActions != NULL)
	  {
	     /* take the concerned actions list */
	     binding = appliActions->eventsList[event];
	     while (binding != NULL)
	       {
		  if (binding->pre == pre && (binding->type == 0 || binding->type == type))
		    {
		       if (pre)
			  doitFunc = (Func) binding->action->doAction;
		       else
			  doitProc = binding->action->doAction;
		       binding = NULL;	/* end of research */
		    }
		  else
		     binding = binding->next;	/* continue */
	       }
	  }

	/* See in the parent schema */
	if (doitProc == NULL && doitFunc == NULL)
	  {
	     ret = True;	/* still in the same schema */
	     if (element != 0)
		element = (Element) ((PtrElement) element)->ElParent;
	     while (ret && element != 0)
	       {
		  ret = (schStruct == ((PtrElement) element)->ElSructSchema);
		  if (!ret)
		     /* a new schema */
		     schStruct = ((PtrElement) element)->ElSructSchema;
		  else
		     element = (Element) ((PtrElement) element)->ElParent;
	       }

	     if (element == 0)
		schStruct = NULL;	/* no more schema */
	  }
     }

   /* See all actions linked with this event in EDITOR application */
   if (doitProc == NULL && doitFunc == NULL)
     {
	appliActions = applicationEditor;
	if (appliActions != NULL)
	  {
	     /* take the concerned actions list */
	     binding = appliActions->eventsList[event];
	     while (binding != NULL)
	       {
		  if (binding->pre == pre && (binding->type == 0 || binding->type == type))
		    {
		       if (pre)
			  doitFunc = (Func) binding->action->doAction;
		       else
			  doitProc = binding->action->doAction;
		       binding = NULL;	/* end of research */
		    }
		  else
		     binding = binding->next;	/* continue */
	       }
	  }
     }

   ret = False;
   if (doitFunc != NULL || doitProc != NULL)
     {
	if (doitFunc != NULL)
	   ret = (*doitFunc) (notifyEvent);
	else
	   (*doitProc) (notifyEvent);
     }
   return ret;
}


/* ---------------------------------------------------------------------- */
/* |    SendAttributeMessage notifies the possible application that     | */
/* |            an attribute has been created, deleted, modified, read  | */
/* |            or saved.                                               | */
/* |            It returns True if it executed an action,               | */
/* |            else it returns False.                                  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             SendAttributeMessage (NotifyAttribute * notifyAttr, boolean pre)

#else  /* __STDC__ */
boolean             SendAttributeMessage (notifyAttr, pre)
NotifyAttribute    *notifyAttr;
boolean             pre;

#endif /* __STDC__ */
{
   Element             element;
   PtrSSchema        schStruct;

   if (notifyAttr != NULL)
     {
	element = notifyAttr->element;
	schStruct = (PtrSSchema) ((notifyAttr->attributeType).AttrSSchema);
	return ExecuteAction ((NotifyEvent *) notifyAttr, notifyAttr->event, pre,
		 notifyAttr->attributeType.AttrTypeNum, element, schStruct);
     }
   else
      return False;
}


/* ---------------------------------------------------------------------- */
/* |    ThotSendMessage sends a pointer to a actionstruct               | */
/* |            and a structure-element and executes the                | */
/* |            corresponding action (if any). If an action was         | */
/* |            executed the function returns 'True' else it returns    | */
/* |            'False'.                                                | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             ThotSendMessage (NotifyEvent * notifyEvent, boolean pre)

#else  /* __STDC__ */
boolean             ThotSendMessage (notifyEvent, pre)
NotifyEvent        *notifyEvent;
boolean             pre;

#endif /* __STDC__ */

{
   int                 elType;
   Element             element;
   PtrSSchema        schStruct;

   if (notifyEvent == NULL)
      return False;

   elType = 0;
   schStruct = NULL;
   element = NULL;

   switch (notifyEvent->event)
	 {
	    case TteElemNew:
	    case TteElemRead:
	    case TteElemInclude:
	    case TteElemMenu:
	       if (pre)
		 {
		    elType = ((NotifyElement *) notifyEvent)->elementType.ElTypeNum;
		    element = ((NotifyElement *) notifyEvent)->element;
		    schStruct = (PtrSSchema) (((NotifyElement *) notifyEvent)->elementType.ElSSchema);
		 }
	       else
		 {
		    element = ((NotifyElement *) notifyEvent)->element;
		    elType = ((PtrElement) element)->ElTypeNumber;
		    schStruct = ((PtrElement) element)->ElSructSchema;
		 }
	       break;
	    case TteElemDelete:
	       if (pre)
		 {
		    element = ((NotifyElement *) notifyEvent)->element;
		    elType = ((PtrElement) element)->ElTypeNumber;
		    schStruct = ((PtrElement) element)->ElSructSchema;
		 }
	       else
		 {
		    element = ((NotifyElement *) notifyEvent)->element;
		    elType = ((NotifyElement *) notifyEvent)->elementType.ElTypeNum;
		    schStruct = (PtrSSchema) (((NotifyElement *) notifyEvent)->elementType.ElSSchema);
		 }
	       break;
	    case TteElemSave:
	    case TteElemExport:
	    case TteElemSelect:
	    case TteElemActivate:
	    case TteElemExtendSelect:
	    case TteElemCopy:
	    case TteElemChange:
	    case TteElemMove:
	       element = ((NotifyElement *) notifyEvent)->element;
	       elType = ((PtrElement) element)->ElTypeNumber;
	       schStruct = ((PtrElement) element)->ElSructSchema;
	       break;
	    case TteElemSetReference:
	    case TteElemTextModify:
	       element = ((NotifyOnTarget *) notifyEvent)->element;
	       elType = ((PtrElement) element)->ElTypeNumber;
	       schStruct = ((PtrElement) element)->ElSructSchema;
	       break;
	    case TteElemPaste:
	       if (pre)
		 {
		    element = ((NotifyOnValue *) notifyEvent)->target;
		    elType = ((PtrElement) element)->ElTypeNumber;
		    schStruct = ((PtrElement) element)->ElSructSchema;
		 }
	       else
		 {
		    element = ((NotifyElement *) notifyEvent)->element;
		    elType = ((PtrElement) element)->ElTypeNumber;
		    schStruct = ((PtrElement) element)->ElSructSchema;
		 }
	       break;
	    case TteElemGraphModify:
	       element = ((NotifyOnValue *) notifyEvent)->element;
	       elType = ((PtrElement) element)->ElTypeNumber;
	       schStruct = ((PtrElement) element)->ElSructSchema;
	       break;
	    case TtePRuleCreate:
	    case TtePRuleModify:
	    case TtePRuleDelete:
	       element = ((NotifyPresentation *) notifyEvent)->element;
	       elType = ((PtrElement) element)->ElTypeNumber;
	       schStruct = ((PtrElement) element)->ElSructSchema;
	       break;
	    case TteDocOpen:
	    case TteDocCreate:
	    case TteDocClose:
	    case TteDocSave:
	    case TteDocExport:
	    case TteViewOpen:
	    case TteViewClose:
	    case TteViewResize:
	    case TteViewScroll:
	       element = 0;
	       if (((NotifyDialog *) notifyEvent)->document != 0)
		  if (TabDocuments[((NotifyDialog *) notifyEvent)->document - 1] != NULL)
		    {
		       schStruct = TabDocuments[((NotifyDialog *) notifyEvent)->document - 1]->DocSSchema;
		       if (schStruct != NULL)
			  elType = schStruct->SsRootElem;
		    }
	       break;
	    case TteInit:
	    case TteExit:
	       break;
	    default:
	       break;
	 }

   return ExecuteAction (notifyEvent, notifyEvent->event, pre, elType, element, schStruct);
}
