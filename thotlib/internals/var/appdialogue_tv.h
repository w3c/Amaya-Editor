/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifndef THOT_APPDIALOGUE_VAR
#define THOT_APPDIALOGUE_VAR

extern int UserErrorCode;

/* Table of internal optional Thot operations (actions)		    */
/* Calling an action: (*ThotLocalActions[i])(ref, datatype, data);  */

EXPORT Proc		ThotLocalActions[MAX_LOCAL_ACTIONS];

#ifndef NODISPLAY

#ifdef _WINDOWS
#include <windows.h>
#ifndef __GNUC__
#include <commctrl.h>
#endif
#endif /* _WINDOWS */

#ifdef _WINDOWS

EXPORT WNDCLASSEX       RootShell;

#else /* _WINDOWS */

EXPORT XtAppContext	app_cont;
EXPORT ThotWidget	RootShell;

#endif /* !_WINDOWS */

EXPORT Proc		CurrentCallbackAPI;

/* Table of actions related to the menus attached to Application and	  */
/* Documents windows.							  */
/* For each action, the following items are stored: name, address,	  */
/* keyboard equivalent, windows supposed to call the action.		  */
/* Calling an action: (*actionptr->Call_Action)(document, view);	  */

EXPORT Action_Ctl	*MenuActionList;
EXPORT int		MaxMenuAction;

/* return events for Application and Document windows			  */

EXPORT int      ClickIsDone;

#endif /* !NODISPLAY */
#endif /* THOT_APPDIALOGUE_VAR */
