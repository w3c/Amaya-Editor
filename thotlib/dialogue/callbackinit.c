/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Initialization for callback routines handling
 *
 * Author: I. Vatton (INRIA)
 *
 */
#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "appstruct.h"
#include "appdialogue.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
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
void InitApplicationSchema (PtrSSchema pSS)
{
  PtrEventsSet     schemaActions;

  pSS->SsActionList = NULL;
  if (pSS->SsName && pSS->SsName[0] != EOS)
    {
      schemaActions = SchemasEvents;
      while (schemaActions && strcmp (schemaActions->EvSName, pSS->SsName))
	schemaActions = schemaActions->EvSNext;
      if (schemaActions != NULL)
	pSS->SsActionList = schemaActions;
    }
}

/*----------------------------------------------------------------------
   FetchAction finds and returns an action with the name actionName 
  ----------------------------------------------------------------------*/
PtrAction FetchAction (const char *actionName)
{
  PtrAction           pAction;

  pAction = ActionList;
  while (pAction != NULL && strcmp (actionName, pAction->ActName) != 0)
    pAction = pAction->ActNext;
  return pAction;
}


/*----------------------------------------------------------------------
   TteAddAction inserts an action in the list of actions           
   pointed to by the global variable ActionList.           
  ----------------------------------------------------------------------*/
void TteAddAction (const char *actionName, Proc doIt)
{
  PtrAction           pAction;
  PtrAction           newAction;

  pAction = ActionList;
  if (pAction != NULL)
    {
      /* following actions are treated */
      newAction = pAction->ActNext;
      while (newAction && strcmp (actionName, pAction->ActName))
	{
	  pAction = pAction->ActNext;
	  newAction = newAction->ActNext;
	}
      if (strcmp (actionName, pAction->ActName) == 0)
	newAction = pAction;
    }
  else
    newAction = NULL;		/* First action inserted here */

  if (newAction == NULL)
    {
      newAction = (PtrAction) TtaGetMemory (sizeof (APP_action));
      newAction->ActName = actionName;
      newAction->ActAction = doIt;
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
   TteGetEventsSet returns a pointer to an existing events set.		
  ----------------------------------------------------------------------*/
PtrEventsSet TteGetEventsSet (const char *name)
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
PtrEventsSet TteNewEventsSet (int structureId, const char *name)
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
void TteAddActionEvent (PtrEventsSet eventsList, int typeId,
			APPevent event, ThotBool pre, const char *actionName)
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

/*----------------------------------------------------------------------
   TteFreeEvents frees all event lists and the event/action context pointers.			                        
  ----------------------------------------------------------------------*/
static void TteFreeEventsList (PtrEventsSet eventsList)
{
  PtrEventsSet   current;
  PtrActionEvent actevent, nextactevent;
  int            event;

  current = eventsList;
  while (current)
  {
   for (event = 0; event <= TteExit; event++)
   {
      if (current->EvSList[event])
	{
	  /* free lists of couples action/event */
	  actevent = current->EvSList[event];
	  while (actevent)
	    {
	      nextactevent = actevent->AEvNext;
	      TtaFreeMemory (actevent);
	      actevent = nextactevent;
	    }
	}
  }
   eventsList = current->EvSNext;
   TtaFreeMemory (current);
   current = eventsList;
  }
}

/*----------------------------------------------------------------------
   TteFreeAllEventsList frees all event lists and the event/action
   context pointers.			                        
  ----------------------------------------------------------------------*/
void TteFreeAllEventsList (void)
{
  TteFreeEventsList (EditorEvents);
  EditorEvents = NULL;
  TteFreeEventsList (SchemasEvents);
  SchemasEvents = NULL;
}
