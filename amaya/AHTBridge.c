/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
  
/*
 * AHTBridge.c : This module implements the callback setup and
 * handlers between the Xt, libwww, and Amaya procedures. It's responsible
 * for assigning, modifying, and supressing Xt events to the active
 * requests.
 *
 * Author: J Kahan
 *         J. K./R. Guetari. Windows NT/95 routines
 *         J. K./S. Gully GTK routines
 *
 */

#ifdef _WX
#include "wxAmayaTimer.h"
#include "wxAmayaSocketEvent.h"
#endif /* _WX */

#define THOT_EXPORT extern
#include "amaya.h"
#include "AHTBridge_f.h"
#include "AHTFWrite_f.h"
#include "query_f.h"
#include "answer_f.h"
#include "HTEvtLst.h"

#if 0
#define DEBUG_LIBWWW
#define THD_TRACE 1
#endif
#ifdef _WX
/* Private functions */
static void         RequestRegisterReadXtevent (SOCKET);
static void         RequestKillReadXtevent (SOCKET);
static void         RequestRegisterWriteXtevent ( SOCKET);
static void         RequestKillWriteXtevent (SOCKET);
static void         RequestRegisterExceptXtevent ( SOCKET);
static void         RequestKillExceptXtevent (SOCKET);
#endif /* _WX */

/* Private variables */
/*
 * this set of HTEventType map our WinSock "socket event HTEventType" into 
 * our read and write sets. Note that under the canonical Unix model,
 * a non-blocking socket passed to an accept() call will appear as readable, 
 * whilst a non-blocking call to connect() will appear as writeable. In add.
 * if the connection has been closed, the socket will appear readable under
 * BSD Unix semantics 
 */

static const HTEventType ReadBits = (HTEventType)(HTEvent_READ | HTEvent_ACCEPT | HTEvent_CLOSE);
static const HTEventType WriteBits = (HTEventType)(HTEvent_WRITE | HTEvent_CONNECT);
static const HTEventType ExceptBits = (HTEventType)HTEvent_OOB;

#ifdef _WX
typedef struct sStatus {
  int read;
  int write;
  int except;
} SocketStatus;
#endif /* _WX */
#ifdef _WX
#define SOCK_TABLE_SIZE 67
#define HASH(s) ((s) % SOCK_TABLE_SIZE)
static SocketStatus persSockets[SOCK_TABLE_SIZE];
#endif /* _WX */

/*--------------------------------------------------------------------
  AHTCallback_bridge
  this function acts as a bridge between Xt and libwww. From the Xt
  point of view, this function is the callback handler whenever there's
  any activity on the sockets associated with the active requests. From
  the libwww point of view, this is the function that takes the initiative
  to invoke the callback function associated with an active request,
  whenever there's an activity in the socket associated to it.
  In this latter  aspect, this function is similar to the library's
  own __DoCallback()  function.
  Upon activation, the function looks up the request associated with the
  active socket and then looks up the cbf associated with that request.
  Upon completion of the execution of the request's cbf, it verifies
  the state of the request and, if it's an asynchronous request, deletes
  the memory allocated to it.
  -------------------------------------------------------------------*/
void *AHTCallback_bridge (caddr_t cd, int *s, XtInputId * id)
{
  return (0);
}

#if defined(_WX)
static void AHTCallback_bridgeWX ( int register_id,  int socket, wxAmayaSocketCondition condition)
{
  int                 status;  /* the status result of the libwwww call */
  HTEventType         type  = HTEvent_ALL;	
  int                 v;
  ms_t                now = HTGetTimeInMillis();
   
  v = HASH (socket);

  /* convert the FD into an HTEventType which will allow us to find the
     request associated with the socket */

  /* I could send some other data here, like the event itself, right */
  switch (condition) 
    {
    case WXAMAYASOCKET_READ:
      type = HTEvent_READ;
      break;
    case WXAMAYASOCKET_WRITE:
      type = HTEvent_WRITE;
      break;
    case WXAMAYASOCKET_EXCEPTION:
      type = HTEvent_OOB;
      break;
    default:
      type = HTEvent_ALL; 
      break;
    } /* switch */
   
  /* Invokes the callback associated to the requests */
   
  /**   CanDoStop_set (FALSE); **/
  if ((status = HTEventList_dispatch (socket, type, now)) != HT_OK)
    {
#ifdef DEBUG_LIBWWW
      HTTrace ("Callback.... returned a value != HT_OK");
#endif
    }
  /***   CanDoStop_set (TRUE); **/
}
#endif /* _WX  */

/*--------------------------------------------------------------------
  ProcessTerminateRequest
  This function is called whenever a request has ended. If the requested
  ended normally, the function will call any callback associated to the
  request. Otherwise, it will just mark the request as over.
  -------------------------------------------------------------------*/
void  ProcessTerminateRequest (HTRequest *request, HTResponse *response,
                               void *param, int status)
{   
  AHTReqContext *me = (AHTReqContext *)HTRequest_context (request);

  /* choose a correct treatment in function of the request's
     being associated with an error, with an interruption, or with a
     succesful completion */

#ifdef DEBUG_LIBWWW  
  if (THD_TRACE)
    fprintf (stderr,"ProcessTerminateRequest: processing req %p, url %s,\
 status %d\n", me, me->urlName, me->reqStatus);  
#endif /* DEBUG_LIBWWW */
  if (me->reqStatus == HT_END)
    {
      if (AmayaIsAlive ()  && me->terminate_cbf)
        (*me->terminate_cbf) (me->docid, 0, me->urlName, me->outputfile,
                              NULL, &(me->http_headers), me->context_tcbf);

    }
  else if (me->reqStatus == HT_ABORT)
    /* either the application ended or the user pressed the stop 
       button. We erase the incoming file, if it exists */
    {
      if (AmayaIsAlive () && me->terminate_cbf)
        (*me->terminate_cbf) (me->docid, -1, me->urlName, me->outputfile,
                              NULL, &(me->http_headers), me->context_tcbf);
      if (me->outputfile && me->outputfile[0] != EOS)
        {
          TtaFileUnlink (me->outputfile);
          me->outputfile[0] = EOS; 
        } 
    }
  else if (me->reqStatus == HT_ERR)
    {
      /* there was an error */
      if (AmayaIsAlive () && me->terminate_cbf)
        (*me->terminate_cbf) (me->docid, -2, me->urlName, me->outputfile,
                              (char *)HTRequest_proxy (request), &(me->http_headers), me->context_tcbf);
      
      if (me->outputfile && me->outputfile[0] != EOS)
        {
          TtaFileUnlink (me->outputfile);
          me->outputfile[0] = EOS;
        }
    }

  /* we erase the context if we're dealing with an asynchronous request */
  if ((me->mode & AMAYA_ASYNC) || (me->mode & AMAYA_IASYNC))
    {
      Document doc = me->docid;
      me->reqStatus = HT_END;
      /*** @@@ do we need this? yes!! **/
      AHTLoadTerminate_handler (request, response, param, status);
      AHTReqContext_delete (me);
      AHTPrintPendingRequestStatus (doc, YES);
    }
}

#ifdef _WINDOWS
/*----------------------------------------------------------------
  WIN_Activate_Request
  when there are more open requests than available sockets, the 
  requests are put in a "pending state." When a socket becomes
  available, libwww associates it with a pending request and then
  calls this callback function. This function is responsible for
  opening the temporary file where the GET and POST  results
  will be stored. The function is also responsible for 
  registering the socket with the Xt event loop.
  Consult the libwww manual for more details on the signature
  of this function.
  ----------------------------------------------------------------*/
int WIN_Activate_Request (HTRequest * request, HTAlertOpcode op,
                          int msgnum, const char *dfault, void *input,
                          HTAlertPar * reply)
{
  AHTReqContext      *me = (AHTReqContext *)HTRequest_context (request);

  if (me->reqStatus == HT_NEW) 
    {
      if (!(me->output)
          && (me->output != stdout) 
          && me->outputfile
          &&  ((me->output = TtaWriteOpen (me->outputfile)) == NULL))
        {
          /* the request is associated with a file */
          me->outputfile[0] = EOS;	/* file could not be opened */
          TtaSetStatus (me->docid, 1,
                        TtaGetMessage (AMAYA, AM_CANNOT_CREATE_FILE), me->outputfile);
          me->reqStatus = HT_ERR;

          if (me->error_html)
            /* so we can show the error message */
            DocNetworkStatus[me->docid] |= AMAYA_NET_ERROR;
        } 
      else
        {
#ifdef DEBUG_LIBWWW
          if (THD_TRACE)
            fprintf (stderr, "WIN_Activate_Request: Activating pending %s. "
                     "Open fd %d\n", me->urlName, (int) me->output);
#endif /* DEBUG_LIBWWW */
          HTRequest_setOutputStream (me->request,
                                     AHTFWriter_new (me->request, me->output, YES));    
          /*change the status of the request */
          me->reqStatus = HT_WAITING;
        }
    } 
  else if (me->reqStatus == HT_WAITING)
    {
      /*change the status of the request */
      rewind (me->output);
      if (HTRequest_outputStream (me->request) == NULL)
        HTRequest_setOutputStream (me->request, AHTFWriter_new (me->request, me->output, YES));
    } 
  else {
    me->reqStatus = HT_ERR;
  }
   
  return ((me->reqStatus != HT_ERR) ? HT_OK : HT_ERROR);
}

#endif /* _WINDOWS */

/*----------------------------------------------------------------------
  AHTEvent_register
  callback called by libwww whenever a socket is open and associated
  to a request. It sets the pertinent Xt events so that the Xt Event
  loops gets an interruption whenever there's action of the socket. 
  In addition, it registers the request with libwww.
  ----------------------------------------------------------------------*/
int AHTEvent_register (SOCKET sock, HTEventType type, HTEvent *event)
{
  int  status = 0;

  if (sock == INVSOC)
    {
#ifdef DEBUG_LIBWWW
      fprintf(stderr, "AHTEvent_register: sock = INVSOC\n");
#endif /* DEBUG_LIBWWW */
      return (0);
    }
  else
    {
#ifdef _WX
      /* need something special for HTEvent_CLOSE */
      if (type & ReadBits)
        RequestRegisterReadXtevent (sock);
  
      if (type & WriteBits)
        RequestRegisterWriteXtevent (sock);
  
      if (type & ExceptBits)
        RequestRegisterExceptXtevent (sock);
#endif	 /* _WX */
#ifdef _WINDOWS
      /* under windows, libwww requires an explicit FD_CLOSE registration 
         to detect HTTP responses not having a Content-Length header */
      status = HTEventList_register (sock, (HTEventType)(type | HTEvent_CLOSE) , event);
#endif /* _WINDOWS */
#ifdef _UNIX
      status = HTEventList_register (sock, type, event);
#endif /* _UNIX */
      return (status);
    }
}

/*----------------------------------------------------------------------
  AHTEvent_unregister
  callback called by libwww each time a request is unregistered. This
  function takes care of unregistering the pertinent Xt events
  associated with the request's socket. In addition, it unregisters
  the request from libwww.
  ----------------------------------------------------------------------*/
int AHTEvent_unregister (SOCKET sock, HTEventType type)
{
  int    status;
  
  if (sock == INVSOC)
    return HT_OK;

#ifdef _WX
  /* remove the Xt event hooks */
  if (type & ReadBits) 
    RequestKillReadXtevent (sock);
   
  if (type & WriteBits)
    RequestKillWriteXtevent (sock);
   
  if (type & ExceptBits) 
    RequestKillExceptXtevent (sock);
#endif /* _WX */

  /* @@@ if this is the default for windows, no need to have AHTEvent_..
     in windows!! */
  /* call libwww's default routine */
  status = HTEventList_unregister (sock, type);
  return (status);
}

/* Private functions */
/*----------------------------------------------------------------------
  RequestKillAllXtevents
  front-end for kill all Xt events associated with the request pointed
  to by "me".
  ----------------------------------------------------------------------*/
void RequestKillAllXtevents (AHTReqContext * me)
{
#ifdef _WX
  int sock = INVSOC;

  return;
  /* @@@ what to do with this one? @@@ */
  if (me->read_sock != INVSOC)
    sock = me->read_sock;
  else
    if (me->write_sock != INVSOC)
      sock = me->write_sock;
    else
      if (me->except_sock != INVSOC)
        sock = me->except_sock;

#ifdef DEBUG_LIBWWW
  if (THD_TRACE)
    fprintf (stderr, "RequestKillAllXtEvents: Clearing XtInputs\n");
#endif /* DEBUG_LIBWWW */

  RequestKillReadXtevent (sock);
  RequestKillWriteXtevent (sock);
  RequestKillExceptXtevent (sock);
#endif /* _WX */   
}

/*----------------------------------------------------------------------
  RequestRegisterReadXtevent
  Registers with Xt the read events associated with socket sock
  ----------------------------------------------------------------------*/
static void RequestRegisterReadXtevent (SOCKET sock)
{
#ifdef _WX
  int v;

  v = HASH (sock);
  if (!persSockets[v].read)
    {
      persSockets[v].read  = wxAmayaSocketEvent::RegisterSocket( sock,
                                                                 WXAMAYASOCKET_READ,
                                                                 AHTCallback_bridgeWX );
#ifdef DEBUG_LIBWWW
      if (THD_TRACE)
        fprintf (stderr, "RegisterReadXtEvent: adding XtInput %lu Socket %d\n",
                 persSockets[v].read, sock);
#endif /* DEBUG_LIBWWW */
    }
#endif /* _WX */
}

/*----------------------------------------------------------------------
  RequestKillReadXtevent
  kills any read Xt event associated with the request pointed to by "me".
  ----------------------------------------------------------------------*/
static void RequestKillReadXtevent (SOCKET sock)
{
#ifdef _WX
  int v;

  v = HASH (sock);
  if (persSockets[v].read)
    {
#ifdef DEBUG_LIBWWW
      if (THD_TRACE)
        fprintf (stderr, "UnregisterReadXtEvent: Clearing XtInput %lu\n",
                 persSockets[v].read);
#endif /* DEBUG_LIBWWW */
      wxAmayaSocketEvent::UnregisterSocket( persSockets[v].read );
      persSockets[v].read = 0;
    }
#endif /* _WX */  
}

/*----------------------------------------------------------------------
  RequestRegisterWriteXtevent
  Registers with Xt the write events associated with socket sock
  ----------------------------------------------------------------------*/
static void RequestRegisterWriteXtevent (SOCKET sock)
{
#ifdef _WX
  int v;

  v = HASH (sock);
  if (!persSockets[v].write)
    {   
      persSockets[v].write  = wxAmayaSocketEvent::RegisterSocket( sock,
                                                                  WXAMAYASOCKET_WRITE,
                                                                  AHTCallback_bridgeWX );
#ifdef DEBUG_LIBWWW   
      if (THD_TRACE)
        fprintf (stderr, "RegisterWriteXtEvent: Adding XtInput %lu Socket %d\n",
                 persSockets[v].write, sock);
#endif /* DEBUG_LIBWWW */
  
    }
#endif /* _WX */  
}

/*----------------------------------------------------------------------
  RequestKillWriteXtevent
  kills any write Xt event associated with the request pointed to
  by "me".
  ----------------------------------------------------------------------*/
static void RequestKillWriteXtevent (SOCKET sock)
{
#ifdef _WX
  int v;

  v = HASH (sock);
  if (persSockets[v].write)
    {
#ifdef DEBUG_LIBWWW   
      if (THD_TRACE)
        fprintf (stderr, "UnRegisterWriteXtEvent: Clearing Write XtInputs "
                 "%lu\n",
                 persSockets[v].write);
#endif /* DEBUG_LIBWWW */
      wxAmayaSocketEvent::UnregisterSocket( persSockets[v].write );
      persSockets[v].write = 0;
    }
#endif /* _WX */  
}

/*----------------------------------------------------------------------
  RequestRegisterExceptXtevent
  Registers with Xt the except events associated with socket sock
  ----------------------------------------------------------------------*/
static void RequestRegisterExceptXtevent (SOCKET sock)
{
#ifdef _WX
  int v;

  v = HASH (sock);
  if (!persSockets[v].except)
    {
      persSockets[v].except =
        wxAmayaSocketEvent::RegisterSocket( sock,
                                            WXAMAYASOCKET_EXCEPTION,
                                            AHTCallback_bridgeWX );
#ifdef DEBUG_LIBWWW
      if (THD_TRACE)
        fprintf (stderr, "RegisterExceptXtEvent: adding XtInput %lu Socket %d\n",
                 persSockets[v].except, sock);
#endif /* DEBUG_LIBWWW */
    }
#endif /* _WX */   
}

/*----------------------------------------------------------------------
  RequestKillExceptXtevent
  kills any exception Xt event associated with the request pointed to
  by "me".
  ----------------------------------------------------------------------*/
static void RequestKillExceptXtevent (SOCKET sock)
{
#ifdef _WX
  int v;

  v = HASH (sock);
  if (persSockets[v].except)
    {
#ifdef DEBUG_LIBWWW   
      if (THD_TRACE)
        fprintf (stderr, "UnregisterExceptXtEvent: Clearing Except XtInputs "
                 "%lu\n", persSockets[v].except);
#endif /* DEBUG_LIBWWW */
      wxAmayaSocketEvent::UnregisterSocket( persSockets[v].except );
      persSockets[v].except = 0;
    }
#endif /* _WX */
}

/*----------------------------------------------------------------------
  Xt Timer functions 
  ----------------------------------------------------------------------*/

struct _HTTimer {
  ms_t        millis;         /* Relative value in millis */
  ms_t        expires;        /* Absolute value in millis */
  BOOL        relative;
  BOOL        repetitive;
  void *      param;          /* Client supplied context */
  HTTimerCallback * cbf;
};

struct _AmayaTimer {
  HTTimer *libwww_timer;
#ifdef _WX
  wxAmayaTimer * xt_timer;
#endif /* _WX */
};
typedef struct _AmayaTimer AmayaTimer;

static HTList *Timers = NULL;

/*----------------------------------------------------------------------
  TimerCallback
  called by the system event loop. Timers shouldn't be restarted
  on exiting.
  ----------------------------------------------------------------------*/
void *TimerCallback (XtPointer cdata, XtIntervalId *id)
{
  return (0);
}

/*----------------------------------------------------------------------
  TimerCallbackWX
  called when a timer is throw
  ----------------------------------------------------------------------*/
void TimerCallbackWX( void * p_context )
{
#ifdef _WX
  HTList *cur, *last;
  AmayaTimer *me;
  HTTimer *libwww_timer;
  AmayaTimer *data;

  data = (AmayaTimer *)p_context;

  if (!AmayaIsAlive () 
      || Timers == NULL)
    return;

  /* find the timer from the uid */
  last = cur = Timers;
  while ((me = (AmayaTimer * ) HTList_nextObject (cur)))
    {
      if (me == data)
        break;
      last = cur;
    }

  if (me)
    {
      libwww_timer = me->libwww_timer;
      /* remove the element from the list @@@ can be optimized later */
      HTList_quickRemoveElement(cur, last);

      /* delete explicitely the AmayaTimer */
      wxDynamicCast(me->xt_timer, wxAmayaTimer)->Stop();
      delete wxDynamicCast(me->xt_timer, wxAmayaTimer);

      TtaFreeMemory (me);
      HTTimer_dispatch (libwww_timer);
    }
#endif /* _WX */
}

/*----------------------------------------------------------------------
  KillAllTimers
  ----------------------------------------------------------------------*/
void KillAllTimers (void)
{
#ifdef _WX
  /* @@@ maybe add something else to kill the Xt things */
  if (Timers)
    HTList_delete (Timers);
  Timers = NULL;
#endif /* _WX */
}

/*----------------------------------------------------------------------
  AMAYA_SetTimer
  ----------------------------------------------------------------------*/
void AMAYA_SetTimer (HTTimer *libwww_timer)
{
#ifdef _WX
  HTList *cur, *last;
  AmayaTimer *me;

  if (!AmayaIsAlive () 
      || libwww_timer == NULL
      || libwww_timer->expires == 0)
    return;

  if (Timers == NULL)
    Timers = HTList_new ();

  /* see if this timer existed already */
  last = cur = Timers;
  while ((me = (AmayaTimer * ) HTList_nextObject (cur)))
    {
      if (me->libwww_timer == libwww_timer)
        break;
      last = cur;
    }

  if (me)
    {
      /* remove the old timer */
      if (me->xt_timer) 
        {
          wxDynamicCast(me->xt_timer, wxTimer)->Stop();
          delete wxDynamicCast(me->xt_timer, wxTimer);
          me->xt_timer = NULL;
        }
    }
  else
    {
      /* create a new element */
      me = (AmayaTimer*)TtaGetMemory (sizeof (AmayaTimer));
      /* and add it to the list */
      HTList_addObject(last, (void *) me);
      me->libwww_timer = libwww_timer;
    }

  /* add a new time out */
  me->xt_timer = new wxAmayaTimer( TimerCallbackWX, me);
  /* start a one shot timer */
  me->xt_timer->Start( me->libwww_timer->millis, TRUE );
#endif /* _WX */
}

/*----------------------------------------------------------------------
  AMAYA_DeleteTimer
  ----------------------------------------------------------------------*/
void AMAYA_DeleteTimer (HTTimer *libwww_timer)
{
#ifdef _WX
  HTList *cur, *last;
  AmayaTimer *me;

  if (Timers == NULL || libwww_timer == NULL)
    return;

  /* find the id */
  last = cur = Timers;
  while ((me = (AmayaTimer * ) HTList_nextObject (cur)))
    {
      if (me->libwww_timer == libwww_timer)
        break;
      last = cur;
    }

  if (me)
    {
      /* remove the Xt timer */
      wxDynamicCast(me->xt_timer, wxTimer)->Stop();
      delete wxDynamicCast(me->xt_timer, wxTimer);
      /* and the element from the list */
      HTList_removeObject (Timers, me);
      TtaFreeMemory (me);
    }
#endif /* _WX */
}

