
#ifndef THOT_APPDIALOGUE_VAR
#define THOT_APPDIALOGUE_VAR

extern int UserErrorCode; /* defini dans thotmsg.c */

/* TABLE DES TRAITEMENTS INTERNES et optionnels de Thot		*/
/* Appel action: (*ThotLocalActions[i])(ref, typedata, data);   */
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

/* TABLE DES ACTIONS liees aux menus des fenetres Application et Documents */
/* On conserve pour chaque action : nom, adresse , equivalent clavier	   */
/* et indicateur pour chaque fenetre susceptible d'appeler cette action	   */
/* Appel action: (*ptraction->Call_Action)(document, vue);   		   */
EXPORT Action_Ctl	*MenuActionList;
EXPORT int		MaxMenuAction;

/* Gestion des evenements de retour sur les fenetres Application et Documents */
EXPORT int      ClickIsDone;

#endif /* !NODISPLAY */

#endif /* THOT_APPDIALOGUE_VAR */
