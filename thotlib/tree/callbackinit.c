
/* -- 
 * Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- 
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "modif.h"
#include "app.h"

#include "memory.f"
#include "rdschstr.f"

/* Global variable pointing to the list of action-definitions. */
PtrExternAppliList  applicationList = NULL;
PtrExternAppliList  applicationEditor = NULL;
PtrAction           externalActions = NULL;	/* Global variable that contains a pointer

						   to the first action in the actionlist. */


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
   PtrAction           list;

   list = externalActions;
   while (list != NULL && strcmp (actionName, list->ActionName) != 0)
      list = list->next;
   return list;
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
   PtrAction           list;
   PtrAction           tmpAction;

   list = externalActions;
   if (list != NULL)
     {
	/* following actions are treated */
	tmpAction = list->next;
	while (tmpAction != NULL && strcmp (actionName, list->ActionName) != 0)
	  {
	     list = list->next;
	     tmpAction = tmpAction->next;
	  }
	if (strcmp (actionName, list->ActionName) == 0)
	   tmpAction = list;
     }
   else
      tmpAction = NULL;		/* First action inserted here */

   if (tmpAction == NULL)
     {
	tmpAction = (PtrAction) TtaGetMemory (sizeof (APP_action));
	tmpAction->ActionName = actionName;
	tmpAction->doAction = doIt;
	tmpAction->actPre = False;
	tmpAction->actEvent = TteNull;
	tmpAction->next = NULL;
	if (list != NULL)
	   list->next = tmpAction;
	else
	   externalActions = tmpAction;		/* First message inserted here */
     }
}


/* ---------------------------------------------------------------------- */
/* |    InitEventActions inserts the necessary structure for an         | */
/* |        event/action. This function is called from the app compiler | */
/* |        and the application initialisation.                         | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                InitEventActions (PtrExternAppliList ptrAction, int typeId, APPevent event, boolean pre, char *actionName)

#else  /* __STDC__ */
void                InitEventActions (ptrAction, typeId, event, pre, actionName)
PtrExternAppliList  ptrAction;
int                 typeId;
APPevent            event;
boolean             pre;
char               *actionName;

#endif /* __STDC__ */

{
   PtrAction           action;
   PtrActionBindingList binding, binding1;

   if (event > TteExit)
      return;
   action = FindAction (actionName);
   if (action == NULL)
      return;

   /* See the concerned list of bindings */
   binding = ptrAction->eventsList[event];
   binding1 = (PtrActionBindingList) TtaGetMemory (sizeof (actionbindinglist));
   binding1->action = action;
   binding1->pre = pre;
   /* Previous event implies function action */
   if (pre)
      action->actPre = True;
   action->actEvent = event;
   binding1->type = typeId;
   binding1->next = binding;
   ptrAction->eventsList[event] = binding1;
}



/* ---------------------------------------------------------------------- */
/* |    MakeNewApplicationStruct returns a pointer to a new application | */
/* |            structure. It is added to the list of applications      | */
/* |            applicationList which is a global variable.             | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
PtrExternAppliList  MakeNewApplicationStruct (int structureId, char *applicationName)

#else  /* __STDC__ */
PtrExternAppliList  MakeNewApplicationStruct (structureId, applicationName)
int                 structureId;
char               *applicationName;

#endif /* __STDC__ */

{
   PtrExternAppliList  act, act1;
   int                 i;

   /* Create the new application context */
   act1 = (PtrExternAppliList) TtaGetMemory (sizeof (externapplilist));
   act1->strId = structureId;
   act1->appId = applicationName;
   for (i = 0; i <= TteExit; i++)
      act1->eventsList[i] = NULL;
   act1->next = NULL;

   /* Link it */
   if (strcmp (applicationName, "EDITOR") == 0)
     {
	if (applicationEditor != NULL)
	   TtaFreeMemory ((char *) applicationEditor);	/* delete the previous one */
	applicationEditor = act1;
     }
   else if (applicationList == NULL)
      applicationList = act1;
   else
     {
	act = applicationList;
	while (act->next != NULL)
	   act = act->next;
	act->next = act1;
     }
   return act1;
}
