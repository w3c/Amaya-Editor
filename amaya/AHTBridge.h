/*                                  Amaya Source Code

                  BRIDGE INTERFACE WITH XT, AMAYA AND LIBWWW

*/

/*
**      (c) COPYRIGHT 
**      Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This module provides callback setup/handler interfaces between Xt,
Amaya, and libwww modules.

This module is implemented by AHTBridge.c, and it is a part of the
AMAYA Source Code.

*/

#ifndef AHTBRIDGE_H
#define AHTBRIDGE_H

#include "amaya.h"
/*
** Function prototypes
**/

/* libwww invokes this callback each time it activates a new request.
** This function registers the socket associated to the request with
** the Xt event manager.
*/

#ifdef __STDC__
#ifdef WWW_XWINDOWS
extern XtInputCallbackProc AHTCallback_bridge (caddr_t cd, int* s, XtInputId* id) ;
#else  /* WWW_XWINDOWS */
extern LONG                AHTCallback_bridge (caddr_t cd, int* s) ;
#endif /* WWW_XWINDOWS */
extern int                 Add_NewSocket_to_Loop (HTRequest*, HTAlertOpcode, int, const char*, void*, HTAlertPar*) ;
extern int                 AHTEvent_register(SOCKET sock, HTRequest* rqp, SockOps ops, HTEventCallback* cbf, HTPriority p) ;
extern int                 AHTEvent_unregister (SOCKET sock, SockOps ops) ;
extern void                RequestKillAllXtevents (AHTReqContext* me) ;

#else  /* __STDC__ */
#ifdef WWW_XWINDOWS
extern XtInputCallbackProc AHTCallback_bridge () ;
#else  /* WWW_XWINDOWS */
extern LONG                AHTCallback_bridge () ;
#endif /* WWW_XWINDOWS */
extern int                 Add_NewSocket_to_Loop () ;
extern int                 AHTEvent_register () ;
extern int                 AHTEvent_unregister () ;
extern void                RequestKillAllXtevents () ;
#endif /* __STDC__ */

#endif /* AHTBRIDGE_H */

