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

#define EXPORT
#include "amaya.h"
#include "netamaya.h"
#include "eventamaya.h"

#include "netamaya_f.h"
#include "eventamaya_f.h"

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


/************************************************************************
 *									*
 *	FRONT END to the NETWORK ACCESSES using LIBWWW			*
 *									*
 ************************************************************************/

