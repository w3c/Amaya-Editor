/*                                         W3C Reference Library libwww Default Event Manager
                                  DEFAULT EVENT MANAGER
                                             
 */
/*
**      (c) COPYRIGHT MIT 1995.
**      Please first read the full copyright statement in the file COPYRIGH.
*/
/*

   This module provides an event registry and a multi-threaded event loop. An application
   may use this module for:
   
      event loop and registry - Application registers HTEvntrg_register and
      HTEvntrg_unregister and calls HTEventrg_loop to dispatch events as they occur.
      
      event registry - Application just registers its own event handlers and chains them
      to HTEvntrg_register and HTEvntrg_unregister. When the application's event loop gets
      activity on a socket, it calls HTEvent_dispatch to handle it.
      
      nothing - Application registers its own event handler uses its own event loop to
      dispatch those events.
      
   This module is implemented by HTEvntrg.c, and it is a part of the W3C Reference
   Library.
   
 */
#ifndef HTEVNTRG_H
#define HTEVNTRG_H

#include "sysdep.h"
#include "HTEvent.h"
#include "HTReq.h"
/*

  Windows Specific Handles
  
 */
#if defined(WWW_WIN_ASYNC) || defined(WWW_WIN_DLL)
extern BOOL HTEventrg_winHandle (HTRequest * request);
extern BOOL HTEventrg_setWinHandle (HWND window, unsigned long message);
extern HWND HTEventrg_getWinHandle (unsigned long * pMessage);
extern LRESULT CALLBACK AsyncWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam
);
#endif
/*

EVENT REAGISTRY

   The libwww's event registry binds a socket and operation (FD_READ, FD_WRITE...) to a
   callback function. Event are registered, unregistered, and dispatched.
   
  Register an Event Handler
  
   For a given socket, reqister a request structure, a set of operations, a
   HTEventCallback function, and a priority. For this implementation, we allow only a
   single HTEventCallback function for all operations. and the priority field is ignored.
   
 */
extern int HTEventrg_register   (SOCKET, HTRequest *,
                                 SockOps, HTEventCallback *,
                                 HTPriority);
/*

  Unregister an Event Handler
  
   Remove the registered information for the specified socket for the actions specified in
   ops. if no actions remain after the unregister, the registered info is deleted, and, if
   the socket has been registered for notification, the HTEventCallback will be invoked.
   
 */
extern int HTEventrg_unregister (SOCKET, SockOps);
/*

  Unregister ALL Event Handlers
  
   Unregister all sockets. N.B. we just remove them for our internal data structures: it
   is up to the application to actually close the socket.
   
 */
extern int HTEventrg_unregisterAll (void);
/*

  Register a TTY Event Handler
  
   Register the tty (console) as having events. If the TTY is select()-able (as is true
   under Unix), then we treat it as just another socket. Otherwise, take steps depending
   on the platform. This is the function to use to register user events!
   
 */
extern int HTEventrg_registerTTY        (SOCKET, HTRequest *, SockOps,
                                 HTEventCallback *, HTPriority);
/*

  Unregister a TTY Event Handler
  
   Unregisters TTY I/O channel. If the TTY is select()-able (as is true under Unix), then
   we treat it as just another socket.
   
 */
extern int HTEventrg_unregisterTTY (SOCKET, SockOps);
/*

  HTEventrg_dispatch
  
   Dispatches a callback based on the socket and operation (read/write/oob)
   
 */
extern int HTEventrg_dispatch( SOCKET, SockOps);
/*

HANDLER FOR TIMEOUT ON SOCKETS

   This function sets the timeout for sockets in the select()call and registers a timeout
   function that is called if select times out. This does only works on NON windows
   platforms as we need to poll for the console on windows If tv = NULLthen timeout is
   disabled. Default is no timeout. If always=YESthen the callback is called at all times,
   if NO then only when Library sockets are active. Returns YES if OK else NO.
   
 */
typedef int HTEventTimeout (HTRequest *);

extern BOOL HTEventrg_registerTimeout (struct timeval *tp, HTRequest * request,
                                     HTEventTimeout *tcbf, BOOL always);
/*

EVENT LOOP

   The libwww's default event loop dispatches events to the event registry.
   
  Start and Stop the Event Manager
  
 */
extern BOOL HTEventInit (void);
extern BOOL HTEventTerminate (void);
/*

  Start the Event Loop
  
   That is, we wait for activity from one of our registered channels, and dispatch on
   that. Under Windows/NT, we must treat the console and sockets as distinct. That means
   we can't avoid a busy wait, but we do our best.
   
 */
extern int HTEventrg_loop (HTRequest * request);
/*

  Stop the Event Loop
  
   Stops the (select based) event loop. The function does not guarantee that all requests
   have terminated. This is for the app to do
   
 */
extern void HTEventrg_stopLoop (void);


/*
** __RetrieveCBF
** given a socket, return the HTEventCallback function registered for it
** and return the HTRequest pointer associated with it.
** If the socket isn't found, the function returns NULL
*/

/* JK: 15/oct/96: Made this function public */

extern HTEventCallback *__RetrieveCBF(SOCKET s, SockOps ops,HTRequest **arp);


#endif /* HTEVENTRG_H */
/*

   
   ___________________________________
   
                             @(#) $Id$
                                                                                          
    */
