#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "modif.h"
#include "app.h"

#include "memory.f"
#include "rdschstr.f"

/* Global variable pointing to the list of action-definitions. */
PtrEventsSet  applicationList = NULL;
PtrEventsSet  applicationEditor = NULL;
PtrAction     externalActions = NULL;


/* ---------------------------------------------------------------------- */
/* |    FindAction finds and returns an action with the name actionName | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static PtrAction    FindAction (char *actionName)
#else  /* __STDC__ */
static PtrAction    FindAction (actionName)
char               *actionName;
#endif /* __STDC__ */
{
   PtrAction           paction;

   paction = externalActions;
   while (paction != NULL && strcmp (actionName, paction->ActName) != 0)
      paction = paction->ActNext;
   return paction;
}


/* ---------------------------------------------------------------------- */
/* |    InsertAction inserts an action structure in list of actions     | */
/* |            pointed to by the global variable externalActions.      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                InsertAction (char *actionName, Proc doIt)
#else  /* __STDC__ */
void                InsertAction (actionName, doIt)
char               *actionName;
Proc                doIt;
#endif /* __STDC__ */
{
   PtrAction           paction;
   PtrAction           newaction;

   paction = externalActions;
   if (paction != NULL)
     {
	/* following actions are treated */
	newaction = paction->ActNext;
	while (newaction != NULL && strcmp (actionName, paction->ActName) != 0)
	  {
	     paction = paction->ActNext;
	     newaction = newaction->ActNext;
	  }
	if (strcmp (actionName, paction->ActName) == 0)
	   newaction = paction;
     }
   else
      newaction = NULL;		/* First action inserted here */

   if (newaction == NULL)
     {
	newaction = (PtrAction) TtaGetMemory (sizeof (APP_action));
	newaction->ActName = actionName;
	newaction->ActAction = doIt;
	newaction->ActPre = FALSE;
	newaction->ActEvent = TteNull;
	newaction->ActNext = NULL;
	if (paction != NULL)
	   paction->ActNext = newaction;
	else
	  /* First message inserted here */
	   externalActions = newaction;
     }
}


/* ---------------------------------------------------------------------- */
/* |    TteNewEventsSet returns a pointer to a new events set.		| */
/* |            It is added to the global list.       			| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
PtrEventsSet  TteNewEventsSet (int structureId, char *name)
#else  /* __STDC__ */
PtrEventsSet  TteNewEventsSet (structureId, name)
int                 structureId;
char               *name;
#endif /* __STDC__ */
{
  PtrEventsSet  pevset, newevset;
  int           event;

  /* Create the new application context */
  newevset = (PtrEventsSet) TtaGetMemory (sizeof (EventsSet));
  newevset->EvSStructId = structureId;
  newevset->EvSName = name;
  for (event = 0; event <= TteExit; event++)
    newevset->EvSList[event] = NULL;
  newevset->EvSNext = NULL;
  
  /* Link it */
  if (strcmp (name, "EDITOR") == 0)
    {
      if (applicationEditor != NULL)
	/* delete the previous one */
	TtaFreeMemory ((char *) applicationEditor);
      applicationEditor = newevset;
    }
  else if (applicationList == NULL)
    applicationList = newevset;
  else
    {
      pevset = applicationList;
      while (pevset->EvSNext != NULL)
	pevset = pevset->EvSNext;
      pevset->EvSNext = newevset;
    }
  return newevset;
}


/* ---------------------------------------------------------------------- */
/* |    TteAddActionEvent inserts a new structure event/action into     | */
/* |        eventsList.				                        | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TteAddActionEvent (PtrEventsSet eventsList, int typeId, APPevent event, boolean pre, char *actionName)
#else  /* __STDC__ */
void                TteAddActionEvent (eventsList, typeId, event, pre, actionName)
PtrEventsSet  eventsList;
int                 typeId;
APPevent            event;
boolean             pre;
char               *actionName;
#endif /* __STDC__ */
{
  PtrAction           action;
  PtrActionEvent      pactevent, newactevent;

  if (event > TteExit)
    return;
  action = FindAction (actionName);
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
