/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Warning:
 * This module is part of the Thot library, which was originally
 * developed in French. That's why some comments are still in
 * French, but their translation is in progress and the full module
 * will be available in English in the next release.
 * 
 */
 
/*
 * Initialization for callback routines handling
 *
 * Author: I. Vatton (INRIA)
 *
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "modif.h"
#include "appstruct.h"
#include "appdialogue.h"

#undef THOT_EXPORT
#define THOT_EXPORT
#include "appevents_tv.h"
#include "appdialogue_tv.h"


#include "memory_f.h"
#include "readstr_f.h"

/*----------------------------------------------------------------------
   InitApplicationSchema						
   Initializes the application pointer in the SSchema	
   if there is an application for this schema. If not, the	
   pointer is set to NULL.					
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InitApplicationSchema (PtrSSchema pSS)
#else  /* __STDC__ */
void                InitApplicationSchema (pSS)
PtrSSchema          pSS;

#endif /* __STDC__ */
{
   char                schemaName[MAX_NAME_LENGTH];
   PtrEventsSet        schemaActions;

   strcpy (schemaName, pSS->SsName);
   pSS->SsActionList = NULL;
   if (pSS->SsName[0] != EOS)
     {
	schemaActions = SchemasEvents;
	while (schemaActions != NULL &&
	       strcmp (schemaActions->EvSName, schemaName) != 0)
	   schemaActions = schemaActions->EvSNext;
	if (schemaActions != NULL)
	   pSS->SsActionList = schemaActions;
     }
}

/*----------------------------------------------------------------------
   FetchAction finds and returns an action with the name actionName 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrAction    FetchAction (STRING actionName)
#else  /* __STDC__ */
PtrAction    FetchAction (actionName)
STRING              actionName;

#endif /* __STDC__ */
{
   PtrAction           pAction;

   pAction = ActionList;
   while (pAction != NULL && ustrcmp (actionName, pAction->ActName) != 0)
      pAction = pAction->ActNext;
   return pAction;
}


/*----------------------------------------------------------------------
   TteAddAction inserts an action in the list of actions           
   pointed to by the global variable ActionList.           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TteAddAction (STRING actionName, Proc doIt)
#else  /* __STDC__ */
void                TteAddAction (actionName, doIt)
STRING              actionName;
Proc                doIt;

#endif /* __STDC__ */
{
   PtrAction           pAction;
   PtrAction           newAction;

   pAction = ActionList;
   if (pAction != NULL)
     {
	/* following actions are treated */
	newAction = pAction->ActNext;
	while (newAction != NULL && ustrcmp (actionName, pAction->ActName) != 0)
	  {
	     pAction = pAction->ActNext;
	     newAction = newAction->ActNext;
	  }
	if (ustrcmp (actionName, pAction->ActName) == 0)
	   newAction = pAction;
     }
   else
      newAction = NULL;		/* First action inserted here */

   if (newAction == NULL)
     {
	newAction = (PtrAction) TtaGetMemory (sizeof (APP_action));
	newAction->ActName = actionName;
	newAction->ActAction = doIt;
	newAction->ActUser = NULL;
	newAction->ActArg = NULL;
	newAction->ActPre = FALSE;
	newAction->ActEvent = TteNull;
	newAction->ActNext = NULL;
	if (pAction != NULL)
	   pAction->ActNext = newAction;
	else
	   /* First message inserted here */
	   ActionList = newAction;
     }
}

/*----------------------------------------------------------------------
   TteAddUserAction add dynamically an user action in the list of the
   built-ins actions. This may override or complete an existing
   built-in action, or override a previously defined user action.
   We don't support adding a new action currently !!!
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                TteAddUserAction (STRING actionName, UserProc procedure,
                                      void *arg)

#else  /* __STDC__ */
int                TteAddUserAction (actionName, procedure, arg)
STRING              actionName;
UserProc            procedure;
void               *arg;

#endif /* __STDC__ */
{
   PtrAction           pAction;
   PtrAction           newAction;

   int                 lg;

   /*
    * We need a name !
    */
   if (actionName == NULL)
      return(-1);
   lg = ustrlen (actionName);
   if (lg == 0) return(-1);

   pAction = ActionList;
   if (pAction != NULL)
     {
	/* following actions are treated */
	newAction = pAction->ActNext;
	while (newAction != NULL && ustrcmp (actionName, pAction->ActName) != 0)
	  {
	     pAction = pAction->ActNext;
	     newAction = newAction->ActNext;
	  }
	if (ustrcmp (actionName, pAction->ActName) == 0)
	   newAction = pAction;
     }
   else
      newAction = NULL;		/* First action inserted here */

   if (newAction == NULL)
     {
	newAction = (PtrAction) TtaGetMemory (sizeof (APP_action));
	newAction->ActName = actionName;
	newAction->ActAction = NULL;
	newAction->ActUser = NULL;
	newAction->ActArg = NULL;
	newAction->ActPre = FALSE;
	newAction->ActEvent = TteNull;
	newAction->ActNext = NULL;
	if (pAction != NULL)
	   pAction->ActNext = newAction;
	else
	   /* First message inserted here */
	   ActionList = newAction;
     }

   newAction->ActUser = procedure;
   newAction->ActArg = arg;
   return(0);
}


/*----------------------------------------------------------------------
   TteGetEventsSet returns a pointer to an existing events set.		
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrEventsSet        TteGetEventsSet (char* name)
#else  /* __STDC__ */
PtrEventsSet        TteGetEventsSet (name)
char*               name;

#endif /* __STDC__ */
{
   PtrEventsSet        pevset;

   /* Find it */
   if (strcmp (name, "EDITOR") == 0)
     /* it is the global set */
     return(EditorEvents);
   else
     {
       pevset = SchemasEvents;
       while (pevset != NULL)
	 {
	   if (!strcmp(pevset->EvSName, name))
	     return(pevset);
	   pevset = pevset->EvSNext;
	 }
     }
   return NULL;
}


/*----------------------------------------------------------------------
   TteNewEventsSet returns a pointer to a new events set.		
   It is added to the global list.       			
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrEventsSet        TteNewEventsSet (int structureId, char* name)
#else  /* __STDC__ */
PtrEventsSet        TteNewEventsSet (structureId, name)
int                 structureId;
char*               name;

#endif /* __STDC__ */
{
   PtrEventsSet        pevset, newEvSet;
   int                 event;

   if (ThotLocalActions[T_initevents] == NULL)
     /* enable event callbacks */
     TteConnectAction (T_initevents, (Proc) InitApplicationSchema);

   /* Create the new events set */
   newEvSet = (PtrEventsSet) TtaGetMemory (sizeof (EventsSet));
   newEvSet->EvSStructId = structureId;
   newEvSet->EvSName = name;
   for (event = 0; event <= TteExit; event++)
      newEvSet->EvSList[event] = NULL;
   newEvSet->EvSNext = NULL;

   /* Link it */
   if (strcmp (name, "EDITOR") == 0)
     {
	/* it is the global set */
	EditorEvents = newEvSet;
	/* it is the first one: initialize other sets */
	SchemasEvents = NULL;
     }
   else if (SchemasEvents == NULL)
      /* it is specific to a schema */
      SchemasEvents = newEvSet;
   else
     {
	pevset = SchemasEvents;
	while (pevset->EvSNext != NULL)
	   pevset = pevset->EvSNext;
	pevset->EvSNext = newEvSet;
     }
   return newEvSet;
}


/*----------------------------------------------------------------------
   TteAddActionEvent inserts a new structure event/action into     
   eventsList.				                        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TteAddActionEvent (PtrEventsSet eventsList, int typeId, APPevent event, ThotBool pre, STRING actionName)
#else  /* __STDC__ */
void                TteAddActionEvent (eventsList, typeId, event, pre, actionName)
PtrEventsSet        eventsList;
int                 typeId;
APPevent            event;
ThotBool            pre;
STRING              actionName;

#endif /* __STDC__ */
{
   PtrAction           action;
   PtrActionEvent      pactevent, newactevent;

   if (event > TteExit)
      return;
   action = FetchAction (actionName);
   if (action == NULL)
      return;

   /* See the concerned list of couples (action/event) */
   pactevent = eventsList->EvSList[event];
   newactevent = (PtrActionEvent) TtaGetMemory (sizeof (ActionEvent));
   newactevent->AEvAction = action;
   newactevent->AEvPre = pre;
   /* Previous event implies function action */
   if (pre)
      action->ActPre = TRUE;
   action->ActEvent = event;
   newactevent->AEvType = typeId;
   newactevent->AEvNext = pactevent;
   eventsList->EvSList[event] = newactevent;
}
