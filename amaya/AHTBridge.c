/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
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
 *
 */
#ifdef _GTK
#include <glib.h>
#endif /* _GTK */

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

#ifndef _WINDOWS
/* Amaya's X appcontext */
extern ThotAppContext app_cont;
#ifndef _GTK

/* Private functions */
#ifdef __STDC__
static void         RequestRegisterReadXtevent (SOCKET);
static void         RequestKillReadXtevent (SOCKET);
static void         RequestRegisterWriteXtevent ( SOCKET);
static void         RequestKillWriteXtevent (SOCKET);
static void         RequestRegisterExceptXtevent ( SOCKET);
static void         RequestKillExceptXtevent (SOCKET);
#else /* __STDC__ */
static void         RequesAddReadXtevent ();
static void         RequestKillReadXtevent ();
static void         RequesAddWriteXtevent ();
static void         RequestKillWriteXtevent ();
static void         RequestRegisterExceptXtevent ();
static void         RequestKillExceptXtevent ();
#endif /* __STDC__ */

#endif /* _GTK */
#endif /* !_WINDOWS */

#ifdef _GTK
#define WWW_HIGH_PRIORITY (G_PRIORITY_HIGH_IDLE + 50)
#define WWW_LOW_PRIORITY G_PRIORITY_LOW
#define WWW_SCALE_PRIORITY(p) ((WWW_HIGH_PRIORITY - WWW_LOW_PRIORITY) * p \
                          / HT_PRIORITY_MAX + WWW_LOW_PRIORITY)
     
#define READ_CONDITION (G_IO_IN | G_IO_HUP | G_IO_ERR)
#define WRITE_CONDITION (G_IO_OUT | G_IO_ERR)
#define EXCEPTION_CONDITION (G_IO_PRI)
     
 typedef struct _SockEventInfo SockEventInfo;
 struct _SockEventInfo {
   SOCKET s;
   HTEventType type;
   HTEvent *event;
   guint io_tag;
   guint timer_tag;
 };
 
 typedef struct _SockInfo SockInfo;
 struct _SockInfo {
   SOCKET s;
   GIOChannel *io;
   SockEventInfo ev[HTEvent_TYPES];
 };
 
 static GHashTable *sockhash = NULL;
 
 
 static SockInfo *
 get_sock_info(SOCKET s, gboolean create)
 {
   SockInfo *info;
 
   if (!sockhash)
     sockhash = g_hash_table_new(g_direct_hash, g_direct_equal);
 
   info = g_hash_table_lookup(sockhash, GINT_TO_POINTER(s));
   if (!info && create) {
     info = g_new0(SockInfo, 1);
     info->s = s;
     info->io = g_io_channel_unix_new(s);
     info->ev[0].s = info->ev[1].s = info->ev[2].s = s;
     info->ev[0].type = HTEvent_READ;
     info->ev[1].type = HTEvent_WRITE;
     info->ev[2].type = HTEvent_OOB;
     g_hash_table_insert(sockhash, GINT_TO_POINTER(s), info);
   }
   return info;
 }
 
 static gboolean glibwww_timeout_func (gpointer data);
 static gboolean glibwww_io_func(GIOChannel *source, GIOCondition condition,
                                 gpointer data);
#endif /* _GTK */

/* Private variables */

/*
 * this set of HTEventType map our WinSock "socket event HTEventType" into 
 * our read and write sets. Note that under the canonical Unix model,
 * a non-blocking socket passed to an accept() call will appear as readable, 
 * whilst a non-blocking call to connect() will appear as writeable. In add.
 * if the connection has been closed, the socket will appear readable under
 * BSD Unix semantics 
 */

#ifndef _GTK
static const HTEventType ReadBits = HTEvent_READ | HTEvent_ACCEPT | HTEvent_CLOSE;
static const HTEventType WriteBits = HTEvent_WRITE | HTEvent_CONNECT;
static const HTEventType ExceptBits = HTEvent_OOB;

typedef struct sStatus {
  XtInputId read;             /* the different XtId's */
  XtInputId write;
  XtInputId except;
} SocketStatus;

#define SOCK_TABLE_SIZE 67
#define HASH(s) ((s) % SOCK_TABLE_SIZE)
static SocketStatus persSockets[SOCK_TABLE_SIZE];
#endif /* _GTK */

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
#ifndef _WINDOWS
#ifndef _GTK
#ifdef __STDC__
void *AHTCallback_bridge (caddr_t cd, int *s, XtInputId * id)
#else  /* __STDC__ */
void *AHTCallback_bridge (cd, s, id)
caddr_t             cd;
int                *s;
XtInputId          *id;
#endif /* __STDC__ */
{
   int                 status;  /* the status result of the libwwww call */
   HTEventType         type  = HTEvent_ALL;	
   int                 v;
   int 		       socket = *s;
   ms_t                now = HTGetTimeInMillis();
   
   v = HASH (socket);

   /* convert the FD into an HTEventType which will allow us to find the
      request associated with the socket */

   /* I could send some other data here, like the event itself, right */
   switch ((XtInputId) cd) 
     {
     case XtInputReadMask:
       type = HTEvent_READ;
       break;
     case XtInputWriteMask:
       type = HTEvent_WRITE;
       break;
     case XtInputExceptMask:
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

   return (0);
}
#endif /* _GTK */
#endif /* !_WINDOWS */

/*--------------------------------------------------------------------
  ProcessTerminateRequest
  This function is called whenever a request has ended. If the requested
  ended normally, the function will call any callback associated to the
  request. Otherwise, it will just mark the request as over.
  -------------------------------------------------------------------*/
#ifdef __STDC__
void  ProcessTerminateRequest (HTRequest * request, HTResponse * response, void *param, int status)
#else
void ProcessTerminateRequest (request, response, param, status)
HTRequest *request;
HTResponse *response;
void *param;
int status;
#endif
{   
  AHTReqContext *me = HTRequest_context (request);

  /* choose a correct treatment in function of the request's
     being associated with an error, with an interruption, or with a
     succesful completion */

#ifdef DEBUG_LIBWWW  
  if (THD_TRACE)
    fprintf (stderr,"ProcessTerminateRequest: processing req %p, url %s, status %d\n", me, me->urlName, me->reqStatus);  
#endif /* DEBUG_LIBWWW */
  if (me->reqStatus == HT_END)
    {
      if (AmayaIsAlive ()  && me->terminate_cbf)
	(*me->terminate_cbf) (me->docid, 0, me->urlName, me->outputfile, &(me->http_headers), me->context_tcbf);

    }
  else if (me->reqStatus == HT_ABORT)
    /* either the application ended or the user pressed the stop 
       button. We erase the incoming file, if it exists */
    {
      if (AmayaIsAlive () && me->terminate_cbf)
	(*me->terminate_cbf) (me->docid, -1, me->urlName, me->outputfile, &(me->http_headers), me->context_tcbf);
      if (me->outputfile && me->outputfile[0] != WC_EOS)
	{
	  TtaFileUnlink (me->outputfile);
	  me->outputfile[0] = WC_EOS; 
	} 
    }
  else if (me->reqStatus == HT_ERR)
    {
      /* there was an error */
      if (AmayaIsAlive && me->terminate_cbf)
	(*me->terminate_cbf) (me->docid, -1, me->urlName, me->outputfile, &(me->http_headers), me->context_tcbf);
      
      if (me->outputfile && me->outputfile[0] != WC_EOS)
	{
	  TtaFileUnlink (me->outputfile);
	  me->outputfile[0] = WC_EOS;
	}
    }

   /* we erase the context if we're dealing with an asynchronous request */
  if ((me->mode & AMAYA_ASYNC)
      || (me->mode & AMAYA_IASYNC))
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
#ifdef __STDC__
int                 WIN_Activate_Request (HTRequest * request, HTAlertOpcode op, int msgnum, const char *dfault, void *input, HTAlertPar * reply)
#else
int                 WIN_Activate_Request (request, op, msgnum, dfault, input, reply)
HTRequest          *request;
HTAlertOpcode       op;
int                 msgnum;
const char         *dfault;
void               *input;
HTAlertPar         *reply;

#endif /* __STDC__ */
{
   AHTReqContext      *me = HTRequest_context (request);

   if (me->reqStatus == HT_NEW) 
     {
       if (!(me->output)
	   && (me->output != stdout) 
	   && me->outputfile
	   &&  (me->output = ufopen (me->outputfile, TEXT("wb"))) == NULL) {
	 /* the request is associated with a file */
	 me->outputfile[0] = WC_EOS;	/* file could not be opened */
	 TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_CANNOT_CREATE_FILE), me->outputfile);
	 me->reqStatus = HT_ERR;

	 if (me->error_html)
	   DocNetworkStatus[me->docid] |= AMAYA_NET_ERROR; /* so we can show the error message */
       } 
       else
	 {
#ifdef DEBUG_LIBWWW
	   if (THD_TRACE)
	     fprintf (stderr, "WIN_Activate_Request: Activating pending %s. "
		      "Open fd %d\n", me->urlName, (int) me->output);
#endif /* DEBUG_LIBWWW */
	   HTRequest_setOutputStream (me->request, AHTFWriter_new (me->request, me->output, YES));    
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

#endif /* WINDOWS but should be an else */
/* #else  */ /* _WINDOWS */

#ifndef _WINDOWS
#ifndef _GTK

/*----------------------------------------------------------------------
  AHTEvent_register
  callback called by libwww whenever a socket is open and associated
  to a request. It sets the pertinent Xt events so that the Xt Event
  loops gets an interruption whenever there's action of the socket. 
  In addition, it registers the request with libwww.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int AHTEvent_register (SOCKET sock, HTEventType type, HTEvent *event)
#else
int AHTEvent_register (sock, type, event)
SOCKET sock;
HTEventType type;
HTEvent *event;
#endif /* __STDC__ */
{
  int  status;

  if (sock == INVSOC)
    {
#ifdef DEBUG_LIBWWW
      fprintf(stderr, "AHTEvent_register: sock = INVSOC\n");
#endif /* DEBUG_LIBWWW */
      return (0);
    }

#ifndef _WINDOWS
	/* need something special for HTEvent_CLOSE */
  if (type & ReadBits)
    RequestRegisterReadXtevent (sock);
  
  if (type & WriteBits)
    RequestRegisterWriteXtevent (sock);
  
  if (type & ExceptBits)
    RequestRegisterExceptXtevent (sock);
#endif	 /* !_WINDOWS */
  
#ifdef _WINDOWS   
  /* under windows, libwww requires an explicit FD_CLOSE registration 
     to detect HTTP responses not having a Content-Length header */
  status = HTEventList_register (sock, type | HTEvent_CLOSE , event);
#else
  status = HTEventList_register (sock, type, event);
#endif /* _WINDOWS */
  
  return (status);
}
#endif /* _GTK */
#endif /* _WINDOWS */

#ifdef _GTK
/*----------------------------------------------------------------------
  AHTEvent_register FOR GTK
  callback called by libwww whenever a socket is open and associated
  to a request. It sets the pertinent Xt events so that the Xt Event
  loops gets an interruption whenever there's action of the socket. 
  In addition, it registers the request with libwww.
  ----------------------------------------------------------------------*/
int AHTEvent_register (SOCKET s, HTEventType type, HTEvent *event)
 {
   SockInfo *info;
   gint priority = G_PRIORITY_DEFAULT;
   GIOCondition condition;
 
   if (s == INVSOC || HTEvent_INDEX(type) >= HTEvent_TYPES)
     return 0;
 
   info = get_sock_info(s, TRUE);
   info->ev[HTEvent_INDEX(type)].event = event;
 
   switch (HTEvent_INDEX(type)) {
   case HTEvent_INDEX(HTEvent_READ):
     condition = READ_CONDITION;      break;
   case HTEvent_INDEX(HTEvent_WRITE):
     condition = WRITE_CONDITION;     break;
   case HTEvent_INDEX(HTEvent_OOB):
     condition = EXCEPTION_CONDITION; break;
   }
   if (event->priority != HT_PRIORITY_OFF)
     priority = WWW_SCALE_PRIORITY(event->priority);
 
   info->ev[HTEvent_INDEX(type)].io_tag =
     g_io_add_watch_full(info->io, priority, condition, glibwww_io_func,
                          &info->ev[HTEvent_INDEX(type)], NULL);
 
   if (event->millis >= 0)
     info->ev[HTEvent_INDEX(type)].timer_tag =
       g_timeout_add_full(priority, event->millis, glibwww_timeout_func,
                          &info->ev[HTEvent_INDEX(type)], NULL);
 
   return HT_OK;
 }
#endif /* _GTK */

#ifndef _GTK
/*----------------------------------------------------------------------
  AHTEvent_unregister
  callback called by libwww each time a request is unregistered. This
  function takes care of unregistering the pertinent Xt events
  associated with the request's socket. In addition, it unregisters
  the request from libwww.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int AHTEvent_unregister (SOCKET sock, HTEventType type)
#else
int AHTEvent_unregister (sock, type)
SOCKET              sock;
HTEventType         type;

#endif /* __STDC__ */
{
  int    status;

#ifndef _WINDOWS   
   /* remove the Xt event hooks */
   if (type & ReadBits) 
     RequestKillReadXtevent (sock);
   
   if (type & WriteBits)
     RequestKillWriteXtevent (sock);
   
   if (type & ExceptBits) 
     RequestKillExceptXtevent (sock);
#endif /* !_WINDOWS */

   /* @@@ if this is the default for windows, no need to have AHTEvent_..
      in windows!! */

   /* call libwww's default routine */
   status = HTEventList_unregister (sock, type);
   
   return (status);
}
#endif /* _GTK */

#ifdef _GTK
/*----------------------------------------------------------------------
  AHTEvent_unregister FOR GTK
  callback called by libwww each time a request is unregistered. This
  function takes care of unregistering the pertinent Xt events
  associated with the request's socket. In addition, it unregisters
  the request from libwww.
  ----------------------------------------------------------------------*/
int AHTEvent_unregister (SOCKET s, HTEventType type)
 {
   SockInfo *info = get_sock_info(s, FALSE);
 
   if (info) {
     if (info->ev[HTEvent_INDEX(type)].io_tag)
       g_source_remove(info->ev[HTEvent_INDEX(type)].io_tag);
     if (info->ev[HTEvent_INDEX(type)].timer_tag)
       g_source_remove(info->ev[HTEvent_INDEX(type)].timer_tag);
 
     info->ev[HTEvent_INDEX(type)].event = NULL;
     info->ev[HTEvent_INDEX(type)].io_tag = 0;
     info->ev[HTEvent_INDEX(type)].timer_tag = 0;
 
     /* clean up sock hash if needed */
     if (info->ev[0].event == NULL &&
         info->ev[1].event == NULL &&
         info->ev[2].event == NULL) {
       g_hash_table_remove(sockhash, GINT_TO_POINTER(s));
       g_io_channel_unref(info->io);
       g_free(info);
     }
     
     return HT_OK;
   }
   return HT_ERROR;
 }
static gboolean
glibwww_timeout_func (gpointer data)
 {
   SockEventInfo *info = (SockEventInfo *)data;
   HTEvent *event = info->event;
 
   (* event->cbf) (info->s, event->param, HTEvent_TIMEOUT);
   return TRUE;
 }
 
static gboolean
glibwww_io_func(GIOChannel *source, GIOCondition condition, gpointer data)
 {
   SockEventInfo *info = (SockEventInfo *)data;
   HTEvent *event = info->event;
 
   if (info->timer_tag)
       g_source_remove(info->timer_tag);
   if (info->event->millis >= 0) {
     gint priority = G_PRIORITY_DEFAULT;
 
     if (event->priority != HT_PRIORITY_OFF)
       priority = WWW_SCALE_PRIORITY(event->priority);
     info->timer_tag =
       g_timeout_add_full(priority, info->event->millis, glibwww_timeout_func,
                          info, NULL);
   }
 
   (* event->cbf) (info->s, event->param, info->type);
   return TRUE;
 }

#endif /* _GTK */

#ifndef _WINDOWS

/* Private functions */

/*----------------------------------------------------------------------
  RequestKillAllXtevents
  front-end for kill all Xt events associated with the request pointed
  to by "me".
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                RequestKillAllXtevents (AHTReqContext * me)
#else
void                RequestKillAllXtevents (me)
AHTReqContext      *me;
#endif /* __STDC__ */
{
#ifndef _GTK
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
#endif /* _GTK */
}

#ifndef _GTK
/*----------------------------------------------------------------------
  RequestRegisterReadXtevent
  Registers with Xt the read events associated with socket sock
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         RequestRegisterReadXtevent (SOCKET sock)
#else
static void         RequestRegisterReadXtevent (sock)
SOCKET sock;
#endif /* __STDC__ */
{
  int v;

  v = HASH (sock);

  if (!persSockets[v].read)
    {
      persSockets[v].read  =
	XtAppAddInput (app_cont,
		       sock,
		       (XtPointer) XtInputReadMask,
		       (XtInputCallbackProc) AHTCallback_bridge,
		       (XtPointer) XtInputReadMask);
      
#ifdef DEBUG_LIBWWW
      if (THD_TRACE)
	fprintf (stderr, "RegisterReadXtEvent: adding XtInput %lu Socket %d\n",
		 persSockets[v].read, sock);
#endif /* DEBUG_LIBWWW */
    }

}

/*----------------------------------------------------------------------
  RequestKillReadXtevent
  kills any read Xt event associated with the request pointed to by "me".
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         RequestKillReadXtevent (SOCKET sock)
#else
static void         RequestKillReadXtevent (sock)
SOCKET              sock;
#endif /* __STDC__ */
{
  int v;

  v = HASH (sock);

  if (persSockets[v].read)
    {
#ifdef DEBUG_LIBWWW
      if (THD_TRACE)
	fprintf (stderr, "UnregisterReadXtEvent: Clearing XtInput %lu\n",
		 persSockets[v].read);
#endif /* DEBUG_LIBWWW */
      XtRemoveInput (persSockets[v].read);
      persSockets[v].read = (XtInputId) NULL;
    }
}

/*----------------------------------------------------------------------
  RequestRegisterWriteXtevent
  Registers with Xt the write events associated with socket sock
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         RequestRegisterWriteXtevent (SOCKET sock)
#else
static void         RequestRegisterWriteXtevent (sock)
SOCKET              sock;

#endif /* __STDC__ */
{
  int v;
  v = HASH (sock);

  if (!persSockets[v].write)
    {   
      persSockets[v].write =
	XtAppAddInput (app_cont,
		       sock,
		   (XtPointer) XtInputWriteMask,
		   (XtInputCallbackProc) AHTCallback_bridge,
		   (XtPointer) XtInputWriteMask);
#ifdef DEBUG_LIBWWW   
  if (THD_TRACE)
    fprintf (stderr, "RegisterWriteXtEvent: Adding XtInput %lu Socket %d\n",
	     persSockets[v].write, sock);
#endif /* DEBUG_LIBWWW */
  
    }
}

/*----------------------------------------------------------------------
  RequestKillWriteXtevent
  kills any write Xt event associated with the request pointed to
  by "me".
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         RequestKillWriteXtevent (SOCKET sock)
#else
static void         RequestKillWriteXtevent (sock)
SOCKET sock;
#endif /* __STDC__ */
{
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
      XtRemoveInput (persSockets[v].write);
      persSockets[v].write =  (XtInputId) NULL;
    }
}

/*----------------------------------------------------------------------
  RequestRegisterExceptXtevent
  Registers with Xt the except events associated with socket sock
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         RequestRegisterExceptXtevent (SOCKET sock)
#else
static void         RequestRegisterExceptXtevent (sock)
SOCKET              sock;

#endif /* __STDC__ */
{
  int v;

  v = HASH (sock);

   if (!persSockets[v].except)
     {
   
       persSockets[v].except =
	 XtAppAddInput (app_cont,
			sock,
			(XtPointer) XtInputExceptMask,
			(XtInputCallbackProc) AHTCallback_bridge,
			(XtPointer) XtInputExceptMask);
#ifdef DEBUG_LIBWWW      
   if (THD_TRACE)
     fprintf (stderr, "RegisterExceptXtEvent: adding XtInput %lu Socket %d\n",
	      persSockets[v].except, sock);
#endif /* DEBUG_LIBWWW */
     }
}

/*----------------------------------------------------------------------
  RequestKillExceptXtevent
  kills any exception Xt event associated with the request pointed to
  by "me".
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         RequestKillExceptXtevent (SOCKET sock)
#else
static void         RequestKillExceptXtevent (sock)
SOCKET sock;
#endif /* __STDC__ */
{
  int v;

  v = HASH (sock);
  if (persSockets[v].except)
    {
#ifdef DEBUG_LIBWWW   
      if (THD_TRACE)
	fprintf (stderr, "UnregisterExceptXtEvent: Clearing Except XtInputs "
		 "%lu\n", persSockets[v].except);
#endif /* DEBUG_LIBWWW */
      XtRemoveInput (persSockets[v].except);
      persSockets[v].except = (XtInputId) NULL;
    }
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
  XtIntervalId xt_timer;
};

typedef struct _AmayaTimer AmayaTimer;

static HTList *Timers = NULL;

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void *TimerCallback (XtPointer cdata, XtIntervalId *id)
{
  HTList *cur, *last;
  AmayaTimer *me;
  HTTimer *libwww_timer;

  if (!AmayaIsAlive () 
      || Timers == NULL)
    return (0);

  /* find the timer from the uid */
  last = cur = Timers;
  while ((me = (AmayaTimer * ) HTList_nextObject (cur)))
    {
      if (me->xt_timer == *id)
	break;
      last = cur;
    }

  if (me)
    {
      libwww_timer = me->libwww_timer;
      /* remove the element from the list @@@ can be optimized later */
      HTList_quickRemoveElement(cur, last);
      TtaFreeMemory (me);
      HTTimer_dispatch (libwww_timer);
    }

  return (0);
}

/*----------------------------------------------------------------------
  KillAllTimers
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void KillAllTimers (void)
#else
void KillAllTimers ()
#endif /* __STDC__ */
{
  /* @@@ maybe add something else to kill the Xt things */
  if (Timers)
    HTList_delete (Timers);
  Timers = NULL;
}

/*----------------------------------------------------------------------
 AMAYA_SetTimer
 ----------------------------------------------------------------------*/
#ifdef __STDC__
void AMAYA_SetTimer (HTTimer *libwww_timer)
#else
void AMAYA_SetTimer (libwww_timer)
HTTimer *libwww_timer;
#endif /* __STDC__ */
{
  HTList *cur, *last;
  AmayaTimer *me;

  if (!AmayaIsAlive 
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
	  XtRemoveTimeOut (me->xt_timer);
	  me->xt_timer = (XtIntervalId) NULL;
	}
    }
  else
    {
      /* create a new element */
      me = TtaGetMemory (sizeof (AmayaTimer));
      /* and add it to the list */
      HTList_addObject(last, (void *) me);
      me->libwww_timer = libwww_timer;
    }

  /* add a new time out */
  me->xt_timer = XtAppAddTimeOut (app_cont,
				 me->libwww_timer->millis,
				 (XtTimerCallbackProc) TimerCallback,
				 (XtPointer *) (void *) me);

}

/*----------------------------------------------------------------------
  AMAYA_DeleteTimer
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void AMAYA_DeleteTimer (HTTimer *libwww_timer)
#else
void AMAYA_DeleteTimer (libwww_timer)
HTTimer *libwww_timer;
#endif /* __STDC__ */
{
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
      XtRemoveTimeOut (me->xt_timer);
      /* and the element from the list */
      HTList_removeObject (Timers, me);
      TtaFreeMemory (me);
    }
}
#endif /* _GTK */

#endif /* !_WINDOWS */
