/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * eventloop.c : contains the core of the event loop for amaya.
 *      This contols all the asynchronous handling for fetching
 *      documents using the libWWW as well as the X-Windows 
 *      event loop.
 *      This module contains all the routines needing access to
 *      the automaton contol variables, especially the libWWW
 *      callback for Progress, Terminate, Redirect and Authentification.
 */

#define THOT_EXPORT
#include "amaya.h"
#include "netamaya.h"
#include "eventamaya.h"
#include "interface.h"
/* #include "netamaya_f.h" */
#include "eventamaya_f.h"

#ifdef _WINDOWS
#include "wininclude.h"
#endif /* _WINDOWS */

/************************************************************************
 *									*
 *		FRONT END for the APPLICATION				*
 *									*
 ************************************************************************/

/*----------------------------------------------------------------------
  StopActiveNetRequests
  Stops all the active network requests associated to document doc
  Must call the associated callbacks with the corresponding error
  code.
  ----------------------------------------------------------------------*/
 
#ifdef __STDC__
void                StopActiveNetRequests (int doc)
#else
void                StopActiveNetRequests (doc)
int doc;
#endif
{
}
 
/*----------------------------------------------------------------------
  HandleQueuedNetRequests
  The application signify the event core that Pending Net Requests
  exists. If ressources (sockets) are available, these should be fetched
  by calling NextNetRequest. Once completed, the associated callback
  has to be called with the corresponding error code.

  IMPORTANT NOTE : Do not call the associated callback immediately,
     this might break the associated client code, since the callback
     is called BEFORE the queuing request completed !
  ----------------------------------------------------------------------*/
 
static int NetRequestsAvailable = 0;

#ifdef __STDC__
void                HandleQueuedNetRequests (void)
#else
void                HandleQueuedNetRequests ()
#endif
{
    /* Dumb code before integrating with libWWW, Daniel */
    NetRequestsAvailable = 1;
}
 
/************************************************************************
 *									*
 *	CORE of the EVENT HANDLING : Fetch and Dispatch Events		*
 *									*
 ************************************************************************/

#ifndef AMAYA_JAVA
static int AmayaEventLoopInitialized = 0;
static int NbAmayaSelect = 0;

/*----------------------------------------------------------------------
  AmayaSelect

  This routine provide a shared select() syscall needed to multiplex
  the various packages (libWWW, Java, X-Windows ...) needing I/O in
  the Amaya program.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
int                AmayaSelect (int  n,  fd_set  *readfds,  fd_set  *writefds,
       fd_set *exceptfds, struct timeval *timeout)
#else
int                AmayaSelect (n, readfds, writefds, exceptfds, timeout)
int  n;
fd_set  *readfds;
fd_set  *writefds;
fd_set *exceptfds;
struct timeval *timeout;
ThotEvent *ev;
#endif
{
    int res;

    NbAmayaSelect++;

    /* Just a test for now ...
    if (n <= x_window_socket) n = x_window_socket + 1; */
    res = select(n, readfds, writefds, exceptfds, timeout);

    return(res);
}

/*----------------------------------------------------------------------
  AmayaHandleOneEvent

  This routine handle one event fetched from the X-Window socket.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                AmayaHandleOneEvent (ThotEvent *ev)
#else
void                AmayaHandleOneEvent (ev)
ThotEvent *ev;
#endif
{
    TtaHandleOneEvent(ev);
}

/*----------------------------------------------------------------------
  AmayaFetchEvent

  This routine poll both the socket used by the Network interface and
  the X-Windows event queue. As long as no X-Window event is available,
  it has to handle network traffic. If an X-Window event is available,
  the routine should fetch it from the queue in the ev argument and return.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                AmayaFetchEvent (ThotAppContext app_ctxt, ThotEvent *ev)
#else
void                AmayaFetchEvent (app_ctxt, ev)
ThotAppContext app_ctxt;
ThotEvent *ev;
#endif
{
#ifdef _WINDOWS
#else  /* _WINDOWS */
  int status;

  status = XtAppPending (app_ctxt);
  while (status & XtIMXEvent) {
     XtAppProcessEvent (app_ctxt, XtIMXEvent);
     status = XtAppPending (app_ctxt);
  }

  /*
   * Need to check whether something else has to be scheduled.
   */
  /* blockOnFile(x_window_socket, 0); */
  XtAppNextEvent (app_ctxt, ev);
#endif /* !_WINDOWS */
}

#ifndef _WINDOWS
/*----------------------------------------------------------------------
  AmayaFetchAvailableEvent

  This routine look at the socket used by the Network interface and
  the X-Windows event queue. It first handle changes in the network
  socket status, and handle them (atomic operations). Once done,
  if an X-Window event is available, the routine should fetch it from
  the queue in the ev argument and return TRUE, it returns FALSE otherwise.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            AmayaFetchAvailableEvent (ThotAppContext app_ctxt, ThotEvent *ev)
#else
ThotBool            AmayaFetchAvailableEvent (app_ctxt, ev)
ThotAppContext app_ctxt;
ThotEvent *ev;
#endif
{
  int status;

  status = XtAppPending (app_ctxt);
  while (status & XtIMXEvent) {
     XtAppProcessEvent (app_ctxt, XtIMXEvent);
     status = XtAppPending (app_ctxt);
  }
  if (status) {
     XtAppNextEvent (app_ctxt, ev);
     return(TRUE);
  }
  return(FALSE);
}
#endif /* !_WINDOWS */

/*----------------------------------------------------------------------
  AmayaEventLoop

  The point where events arriving from the X-Windows socket as well as
  the network sockets are fetched and dispatched to the correct handlers.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                AmayaEventLoop (ThotAppContext app_ctxt)
#else
void                AmayaEventLoop (app_ctxt)
ThotAppContext app_ctxt;
#endif
{
#ifndef _WINDOWS
   ThotEvent           ev;
#endif /* _WINDOWS */
#ifdef _WINDOWS
   MSG                 msg;
#endif

   /*
    * initialize the whole context if needed.
    */
   if (!AmayaEventLoopInitialized) 
      InitAmayaEventLoop();

   /* Loop waiting for the events */
   while (1)
     {
        if (NetRequestsAvailable) {
        }
#ifdef _WINDOWS
	WIN_ProcessSocketActivity ();
	if (GetMessage (&msg, NULL, 0, 0))
	    TtaHandleOneWindowEvent (&msg);
#else  /* !_WINDOWS */
        AmayaFetchEvent (app_ctxt, &ev);
        AmayaHandleOneEvent (&ev);
#endif /* _WINDOWS */
     }
}
#endif /* !AMAYA_JAVA */

 
/************************************************************************
 *									*
 *	FRONT END to the NETWORK ACCESSES using LIBWWW			*
 *									*
 ************************************************************************/

/*----------------------------------------------------------------------
  QueryInit
  initializes the low-level network interface
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                QueryInit ()
#else
void                QueryInit ()
#endif
{
   /*********
   TtaSetMainLoop (AmayaEventLoop, AmayaFetchEvent, AmayaFetchAvailableEvent);
    *********/
}

/*----------------------------------------------------------------------
  QueryClose
  shutdown the low-level network interface
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                QueryClose ()
#else
void                QueryClose ()
#endif
{
}

