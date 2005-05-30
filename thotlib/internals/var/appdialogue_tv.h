/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifndef THOT_APPDIALOGUE_VAR
#define THOT_APPDIALOGUE_VAR

/* Table of internal optional Thot operations (actions)		    */
/* Calling an action: (*ThotLocalActions[i])(ref, datatype, data);  */

THOT_EXPORT Proc	       ThotLocalActions[MAX_LOCAL_ACTIONS];

/* variables defined into appdialogue.c */
THOT_EXPORT Menu_Ctl    *	DocumentMenuList;

#ifndef NODISPLAY

#ifdef _WINGUI
  #include <windows.h>
  #ifndef __GNUC__
    #include <commctrl.h>
  #endif /* __GNUC__ */
  THOT_EXPORT WNDCLASSEX         RootShell;
  THOT_EXPORT HMENU              currentMenu;
  THOT_EXPORT HINSTANCE          hInstance;
  THOT_EXPORT int                currentFrame;
  THOT_EXPORT BOOL               buttonCommand;
#endif /* _WINGUI */

//#if defined(_MOTIF) || defined(_GTK)
//THOT_EXPORT ThotWidget	       RootShell;
//#endif /* #if defined(_MOTIF) || defined(_GTK) */
#ifdef _MOTIF
THOT_EXPORT ThotTranslations   TextTranslations;
#endif /* #ifdef _MOTIF */

THOT_EXPORT ThotAppContext     app_cont;
/*THOT_EXPORT Proc	       CurrentCallbackAPI;*/

/* Table of actions related to the menus attached to Application and	  */
/* Documents windows.							  */
/* For each action, the following items are stored: name, address,	  */
/* keyboard equivalent, windows supposed to call the action.		  */
/* Calling an action: (*actionptr->Call_Action)(document, view);	  */
THOT_EXPORT Action_Ctl	      *MenuActionList;
THOT_EXPORT int		       MaxMenuAction;
/* return events for Application and Document windows			  */
/*THOT_EXPORT int                ClickIsDone;*/
THOT_EXPORT char              *servername ;
#endif /* !NODISPLAY */
#endif /* THOT_APPDIALOGUE_VAR */







