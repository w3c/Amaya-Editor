#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "modif.h"
#include "app.h"
#include "document.h"

#undef EXPORT
#define EXPORT extern
#include "edit.var"

#include "readstr_f.h"
#include "callbackinit_f.h"

/* Global variable pointing to the list of action-definitions. */
extern PtrEventsSet applicationList;
extern PtrEventsSet applicationEditor;

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
/* |            It returns TRUE if the executed action takes            | */
/* |            place of the editor action else it returns FALSE.       | */
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
  PtrActionEvent      pactevent;
  PtrEventsSet        appliActions;
  boolean             status;
  Proc                proctodo;
  Func                functodo;

  proctodo = NULL;
  functodo = NULL;

  /* See all actions linked with this event in different schemas */
  while (schStruct != NULL && proctodo == NULL && functodo == NULL)
    {
      appliActions = schStruct->SsActionList;
      if (appliActions != NULL)
	{
	  /* take the concerned actions list */
	  pactevent = appliActions->EvSList[event];
	  while (pactevent != NULL)
	    {
	      if (pactevent->AEvPre == pre && (pactevent->AEvType == 0 || pactevent->AEvType == type))
		{
		  if (pre)
		    functodo = (Func) pactevent->AEvAction->ActAction;
		  else
		    proctodo = pactevent->AEvAction->ActAction;
		  pactevent = NULL;	/* end of research */
		}
	      else
		pactevent = pactevent->AEvNext;	/* continue */
	    }
	}
      
      /* See in the parent schema */
      if (proctodo == NULL && functodo == NULL)
	{
	  status = TRUE;	/* still in the same schema */
	  if (element != 0)
	    element = (Element) ((PtrElement) element)->ElParent;
	  while (status && element != 0)
	    {
	      status = (schStruct == ((PtrElement) element)->ElSructSchema);
	      if (!status)
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
  if (proctodo == NULL && functodo == NULL)
    {
      appliActions = applicationEditor;
      if (appliActions != NULL)
	{
	  /* take the concerned actions list */
	  pactevent = appliActions->EvSList[event];
	  while (pactevent != NULL)
	    {
	      if (pactevent->AEvPre == pre && (pactevent->AEvType == 0 || pactevent->AEvType == type))
		{
		  if (pre)
		    functodo = (Func) pactevent->AEvAction->ActAction;
		  else
		    proctodo = pactevent->AEvAction->ActAction;
		  pactevent = NULL;	/* end of research */
		}
	      else
		pactevent = pactevent->AEvNext;
	    }
	}
    }
  
  status = FALSE;
  if (functodo != NULL || proctodo != NULL)
    {
      if (functodo != NULL)
	status = (*functodo) (notifyEvent);
      else
	(*proctodo) (notifyEvent);
    }
  return status;
}


/* ---------------------------------------------------------------------- */
/* |    CallEventAttribute notifies the possible application that     | */
/* |            an attribute has been created, deleted, modified, read  | */
/* |            or saved.                                               | */
/* |            It returns TRUE if it executed an action,               | */
/* |            else it returns FALSE.                                  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             CallEventAttribute (NotifyAttribute * notifyAttr, boolean pre)
#else  /* __STDC__ */
boolean             CallEventAttribute (notifyAttr, pre)
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
      return FALSE;
}


/* ---------------------------------------------------------------------- */
/* |    CallEventType sends a pointer to a actionstruct               | */
/* |            and a structure-element and executes the                | */
/* |            corresponding action (if any). If an action was         | */
/* |            executed the function returns 'TRUE' else it returns    | */
/* |            'FALSE'.                                                | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             CallEventType (NotifyEvent * notifyEvent, boolean pre)

#else  /* __STDC__ */
boolean             CallEventType (notifyEvent, pre)
NotifyEvent        *notifyEvent;
boolean             pre;

#endif /* __STDC__ */

{
   int                 elType;
   Element             element;
   PtrSSchema        schStruct;

   if (notifyEvent == NULL)
      return FALSE;

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
