/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Handle callbacks for Thot events
 *
 * Author: I. Vatton (INRIA)
 *
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "modif.h"
#include "appstruct.h"
#include "document.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "edit_tv.h"
#include "appevents_tv.h"

#include "readstr_f.h"
#include "callbackinit_f.h"

/*----------------------------------------------------------------------
   ElementHasAction
   It returns TRUE if element pEl has an action associated with event event/pre
   else it returns FALSE.
   Only Users action are considered.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool  ElementHasAction (PtrElement pEl, APPevent event, ThotBool pre)
#else  /* __STDC__ */
ThotBool  ElementHasAction (pEl, event, pre)
PtrElement pEl;
APPevent event;
ThotBool pre;

#endif /* __STDC__ */
{
   PtrActionEvent	pActEvent;
   ThotBool		hasAction;

   hasAction = FALSE;

   if (pEl->ElStructSchema != NULL)
     if (pEl->ElStructSchema->SsActionList != NULL)
	{
	/* take the concerned actions list */
	pActEvent = pEl->ElStructSchema->SsActionList->EvSList[event];
	while (pActEvent != NULL && !hasAction)
	    if (pActEvent->AEvPre == pre &&
	        pActEvent->AEvType == pEl->ElTypeNumber)
		hasAction = TRUE;
	    else
		pActEvent = pActEvent->AEvNext;
	}
   return hasAction;
}

/*----------------------------------------------------------------------
   CallAction looks for the concerned action in event list.
   It returns TRUE if the event action takes place of the editor action
   else it returns FALSE.
   If an User action has been defined, it is executed first, if the
   result is zero, the built-in action is also triggered.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     CallAction (NotifyEvent * notifyEvent, APPevent event, ThotBool pre, int type, Element element, PtrSSchema schStruct, ThotBool attr)
#else  /* __STDC__ */
static ThotBool     CallAction (notifyEvent, event, pre, type, element, schStruct, attr)
NotifyEvent        *notifyEvent;
APPevent            event;
ThotBool            pre;
int                 type;
ment             element;
SSchema          schStruct;
ThotBool		    attr;

#endif /* __STDC__ */
{
   PtrActionEvent      pActEvent;
   PtrEventsSet        eventsSet;
   ThotBool            status;
   Proc                procEvent;
   UserProc            userProcEvent = (UserProc)0;
   void               *userProcArg;
   Func                funcEvent;
   UserProc            userFuncEvent;
   void               *userFuncArg;
   int                 userResult;

   procEvent = NULL;
   userProcEvent = NULL;
   userProcArg = NULL;
   funcEvent = NULL;
   userFuncEvent = NULL;
   userFuncArg = NULL;

   /* See all actions linked with this event in different event lists */
   while (schStruct != NULL && procEvent == NULL && funcEvent == NULL)
     {
       eventsSet = schStruct->SsActionList;
       if (eventsSet != NULL)
	 {
	   /* take the concerned actions list */
	   pActEvent = eventsSet->EvSList[event];
	   while (pActEvent != NULL)
	     {
	       if (pActEvent->AEvPre == pre && (pActEvent->AEvType == 0 || pActEvent->AEvType == type))
		 {
		   if (pre)
		     {
		       funcEvent = (Func) pActEvent->AEvAction->ActAction;
		       userFuncEvent = pActEvent->AEvAction->ActUser;
		       userFuncArg = pActEvent->AEvAction->ActArg;
		     }
		   else
		     {
		       procEvent = pActEvent->AEvAction->ActAction;
		       userProcEvent = pActEvent->AEvAction->ActUser;
		       userProcArg = pActEvent->AEvAction->ActArg;
		     }
		   pActEvent = NULL;	/* end of research */
		 }
	       else
		 pActEvent = pActEvent->AEvNext;	/* continue */
	     }
	 }

       /* See in the parent schema, except for attributes */
       if (attr)
           schStruct = NULL;
       else if (procEvent == NULL && funcEvent == NULL)
	 {
	   status = TRUE;	/* still in the same schema */
	   if (element != 0)
	     element = (Element) ((PtrElement) element)->ElParent;
	   while (status && element != 0)
	     {
	       status = (schStruct == ((PtrElement) element)->ElStructSchema);
	       if (!status)
		 {
		   /* a new schema */
		   schStruct = ((PtrElement) element)->ElStructSchema;
		   /* do not consider specific types of the previous schema */
		   if (type > MAX_BASIC_TYPE)
		     type = 0;
		 }
	       else
		 element = (Element) ((PtrElement) element)->ElParent;
	       }
	   
	   if (element == 0)
	     schStruct = NULL;	/* no more schema */
	 }
     }

   /* See all actions linked with this event in EDITOR application */
   if (procEvent == NULL && funcEvent == NULL)
     {
       eventsSet = EditorEvents;
       if (eventsSet != NULL)
	 {
	   /* take the concerned actions list */
	   pActEvent = eventsSet->EvSList[event];
	   while (pActEvent != NULL)
	     {
	       if (pActEvent->AEvPre == pre && (pActEvent->AEvType == 0 || pActEvent->AEvType == type))
		 {
		   if (pre)
		     {
		       funcEvent = (Func) pActEvent->AEvAction->ActAction;
		       userFuncEvent = pActEvent->AEvAction->ActUser;
		       userFuncArg = pActEvent->AEvAction->ActArg;
		     }
		   else
		     {
		       procEvent = pActEvent->AEvAction->ActAction;
		       userProcEvent = pActEvent->AEvAction->ActUser;
		       userProcArg = pActEvent->AEvAction->ActArg;
		     }
		   pActEvent = NULL;	/* end of research */
		 }
	       else
		 pActEvent = pActEvent->AEvNext;
	     }
	 }
     }
   else if (userFuncEvent == NULL)
     {
       eventsSet = EditorEvents;
       if (eventsSet != NULL)
	 {
	   /* take the concerned actions list */
	   pActEvent = eventsSet->EvSList[event];
	   while (pActEvent != NULL)
	     {
	       if (pActEvent->AEvPre == pre && (pActEvent->AEvType == 0 || pActEvent->AEvType == type))
		 {
		   if (pre)
		     {
		       userFuncEvent = pActEvent->AEvAction->ActUser;
		       userFuncArg = pActEvent->AEvAction->ActArg;
		     }
		   else
		     {
		       userProcEvent = pActEvent->AEvAction->ActUser;
		       userProcArg = pActEvent->AEvAction->ActArg;
		     }
		   pActEvent = NULL;	/* end of research */
		 }
	       else
		 pActEvent = pActEvent->AEvNext;
	     }
	 }
     }

   status = FALSE;
   if ((funcEvent != NULL) || (procEvent != NULL) ||
       (userProcEvent != NULL) || (userFuncEvent != NULL))
     {
       if ((funcEvent != NULL) || (userFuncEvent != NULL))
	 {
	   if (userFuncEvent != NULL)
	     {
	       userResult = (*userFuncEvent) (userFuncArg, notifyEvent);
	       if (userResult == 0) 
		 status = TRUE;
	       else if (funcEvent != NULL)
		 status = (*funcEvent) (notifyEvent);
	     }
	   else
	     status = (*funcEvent) (notifyEvent);
	 }
       else
	 {
	   if (userProcEvent != NULL)
	     {
	       userResult = (*userProcEvent) (userProcArg, notifyEvent);
	       if ((userResult != 0) && (procEvent != NULL))
		 (*procEvent) (notifyEvent);
	     }
	   else
	     (*procEvent) (notifyEvent);
	 }
     }
   return status;
}

/*----------------------------------------------------------------------
   CallEventAttribute notifies the possible application that     
   an attribute has been created, deleted, modified, read  
   or saved.                                               
   It returns TRUE if it executed an action,               
   else it returns FALSE.                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            CallEventAttribute (NotifyAttribute * notifyAttr, ThotBool pre)
#else  /* __STDC__ */
ThotBool            CallEventAttribute (notifyAttr, pre)
NotifyAttribute    *notifyAttr;
ThotBool            pre;

#endif /* __STDC__ */
{
   Element             element;
   PtrSSchema          schStruct;

   if (notifyAttr != NULL)
     {
	element = notifyAttr->element;
	schStruct = (PtrSSchema) ((notifyAttr->attributeType).AttrSSchema);
	return CallAction ((NotifyEvent *) notifyAttr, notifyAttr->event, pre,
	      notifyAttr->attributeType.AttrTypeNum, element, schStruct, TRUE);
     }
   else
      return FALSE;
}

/*----------------------------------------------------------------------
   CallEventType sends a pointer to a actionstruct               
   and a structure-element and executes the                
   corresponding action (if any). If an action was         
   executed the function returns 'TRUE' else it returns    
   'FALSE'.                                                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            CallEventType (NotifyEvent * notifyEvent, ThotBool pre)

#else  /* __STDC__ */
ThotBool            CallEventType (notifyEvent, pre)
NotifyEvent        *notifyEvent;
ThotBool            pre;

#endif /* __STDC__ */

{
   int                 elType;
   Element             element;
   PtrSSchema          schStruct;

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
		    schStruct = ((PtrElement) element)->ElStructSchema;
		 }
	       break;
	    case TteElemDelete:
	       if (pre)
		 {
		    element = ((NotifyElement *) notifyEvent)->element;
		    elType = ((PtrElement) element)->ElTypeNumber;
		    schStruct = ((PtrElement) element)->ElStructSchema;
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
	    case TteElemClick:
	    case TteElemActivate:
	    case TteElemExtendSelect:
	    case TteElemCopy:
	    case TteElemChange:
	    case TteElemMove:
	    case TteElemMouseOver:
	    case TteElemMouseOut:
	       element = ((NotifyElement *) notifyEvent)->element;
	       elType = ((PtrElement) element)->ElTypeNumber;
	       schStruct = ((PtrElement) element)->ElStructSchema;
	       break;
	    case TteElemSetReference:
	    case TteElemFetchInclude:
	    case TteElemTextModify:
	       element = ((NotifyOnTarget *) notifyEvent)->element;
	       elType = ((PtrElement) element)->ElTypeNumber;
	       schStruct = ((PtrElement) element)->ElStructSchema;
	       break;
	    case TteElemPaste:
	       if (pre)
		 {
		    element = ((NotifyOnValue *) notifyEvent)->target;
		    elType = ((PtrElement) element)->ElTypeNumber;
		    schStruct = ((PtrElement) element)->ElStructSchema;
		 }
	       else
		 {
		    element = ((NotifyElement *) notifyEvent)->element;
		    elType = ((PtrElement) element)->ElTypeNumber;
		    schStruct = ((PtrElement) element)->ElStructSchema;
		 }
	       break;
	    case TteElemGraphModify:
	       element = ((NotifyOnValue *) notifyEvent)->element;
	       elType = ((PtrElement) element)->ElTypeNumber;
	       schStruct = ((PtrElement) element)->ElStructSchema;
	       break;
	    case TtePRuleCreate:
	    case TtePRuleModify:
	    case TtePRuleDelete:
	       element = ((NotifyPresentation *) notifyEvent)->element;
	       elType = ((PtrElement) element)->ElTypeNumber;
	       schStruct = ((PtrElement) element)->ElStructSchema;
	       break;
	    case TteDocOpen:
	    case TteDocTmpOpen:
	    case TteDocCreate:
	    case TteDocClose:
	    case TteDocSave:
	    case TteDocExport:
	    case TteDocNatPresent:
	    case TteViewOpen:
	    case TteViewClose:
	    case TteViewResize:
	    case TteViewScroll:
	       element = 0;
	       if (((NotifyDialog *) notifyEvent)->document != 0)
		  if (LoadedDocument[((NotifyDialog *) notifyEvent)->document - 1] != NULL)
		    {
		       schStruct = LoadedDocument[((NotifyDialog *) notifyEvent)->document - 1]->DocSSchema;
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

   return CallAction (notifyEvent, notifyEvent->event, pre, elType, element, schStruct, FALSE);
}
