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
 *
 */

#ifndef AMAYA_JAVA

#define THOT_EXPORT extern
#include "amaya.h"
#include "AHTBridge_f.h"
#include "AHTFWrite_f.h"
#include "query_f.h"
#include "answer_f.h"

#ifndef _WINDOWS
/* Amaya's X appcontext */
extern ThotAppContext app_cont;
#endif


/*
 * Private functions
 */
#ifdef __STDC__
static void         RequestRegisterReadXtevent (AHTReqContext *, SOCKET);
static void         RequestKillReadXtevent (AHTReqContext *);
static void         RequestRegisterWriteXtevent (AHTReqContext *, SOCKET);
static void         RequestKillWriteXtevent (AHTReqContext *);
static void         RequestRegisterExceptXtevent (AHTReqContext *, SOCKET);
static void         RequestKillExceptXtevent (AHTReqContext *);

#else
static void         RequesAddReadXtevent ();
static void         RequestKillReadXtevent ();
static void         RequesAddWriteXtevent ();
static void         RequestKillWriteXtevent ();
static void         RequestRegisterExceptXtevent ();
static void         RequestKillExceptXtevent ();

#endif

/*
 * Private variables 
 */

/*
 * this set of SockOps map our WinSock "socket event SockOps" into 
 * our read and write sets. Note that under the canonical Unix model,
 * a non-blocking socket passed to an accept() call will appear as readable, 
 * whilst a non-blocking call to connect() will appear as writeable. In add.
 * if the connection has been closed, the socket will appear readable under
 * BSD Unix semantics 
 */
static const SockOps ReadBits = FD_READ | FD_ACCEPT | FD_CLOSE;
static const SockOps WriteBits = FD_WRITE | FD_CONNECT;
static const SockOps ExceptBits = FD_OOB;


/*
 * Private functions
 */

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

#ifdef __STDC__
void *AHTCallback_bridge (caddr_t cd, int *s, XtInputId * id)
#else
void *AHTCallback_bridge (cd, s, id)
caddr_t             cd;
int                *s;
XtInputId          *id;
#endif /* __STDC__ */
{
   int                 status;  /* the status result of the libwwww call */
   HTRequest          *rqp = NULL;
   AHTReqContext      *me;
   SockOps             ops;	

   /* Libwww 5.0a does not take into account the ops parameter
      in the invocation of for this function call */

#ifdef HACK_WWW
   HTEventCallback    *cbf;

#else
   HTEventCallback    *cbf;

   ops = FD_WRITE;
   cbf = (HTEventCallback *) __RetrieveCBF (*s, ops, &rqp);
#endif

   me = HTRequest_context (rqp);
   if (THD_TRACE)
     fprintf (stderr, "AHTBridge: Processing url %s \n", me->urlName);

   /* verify if there's any callback associated with the request */
   if (!cbf || !rqp || rqp->priority == HT_PRIORITY_OFF)
     {
	if (THD_TRACE)
	   HTTrace ("Callback.... No callback found\n");
	/* experimental */
	/* remove the Xt input which caused this callback */
#ifdef WWW_XWINDOWS
	XtRemoveInput (*id);
#endif
	/* put some more code to correctly destroy this request ?*/
	return (0);
     }

#ifndef _WINDOWS
   switch ((XtInputId) cd)
	 {
	    case XtInputReadMask:
	       ops = me->read_ops;
	       ops = FD_READ;
	       break;
	    case XtInputWriteMask:
	       ops = me->write_ops;
	       ops = FD_WRITE;
	       break;
	    case XtInputExceptMask:
	       ops = me->except_ops;
	       ops = FD_OOB;
	       break;
	 default:
	   break;
	 }			/* switch */
#endif /* !_WINDOWS */

     /* Invokes the callback associated to the requests */
     
     /* first we change the status of the request, to say it
	has entered a critical section */

     if((HTRequest_outputStream(me->request) == (HTStream *) NULL))
       fprintf(stderr,"\n **ERROR** opening %s\n\n",me->urlName);

     me->reqStatus = HT_BUSY;
     if ((status = (*cbf) (*s, rqp, ops)) != HT_OK)
       HTTrace ("Callback.... received != HT_OK");

   /* Several states can happen after this callback. They
    * are indicated by the me->reqStatus structure member and
    * the fds external variables. The following lines examine
    * the states and correspondly update the Xt event register
    *
    * Regarding the me->reqStatus member, we have the following
    * possible states:
    *   
    * HT_BUSY:    Request has blocked
    * HT_WAITING: Request has been reissued
    * HT_ABORT:   Request has been stopped
    * HT_END:     Request has ended
    */

#ifndef _WINDOWS
   if (me->reqStatus == HT_ABORT)
   /* Has the user stopped the request? */
     {
	me->reqStatus = HT_WAITING;
	StopRequest (me->docid);
	return (0);
     }
#endif /* !_WINDOWS */

   if (me->reqStatus == HT_WAITING)
   /* the request is being reissued */
     {
	/*
	 * (1) The old request has ended and the library
	 * assigned the old socket number to a pending
	 * request.
	 *
	 * (2) The request has been reissued after an 
	 * authentication or redirection directive and
	 * we are using the same old socket number.
	 */
       
       if (THD_TRACE)
	 fprintf (stderr, "*** detected a reissue of request \n");
       return (0);
     }

    /* we verify if the request exists. If it has ended, we will have
       a reqStatus with an HT_END value */

   if ((me->request->net == (HTNet *) NULL) || (me->reqStatus == HT_END || me->reqStatus == HT_ERR))
     /* request has ended */
     {
#ifndef _WINDOWS
	if (THD_TRACE)
	  fprintf (stderr, "(BF) removing Xtinput %lu !RWE, sock %d (Request has ended)\n", *id, *s);
#endif
	if ((me->mode & AMAYA_ASYNC) || (me->mode & AMAYA_IASYNC))
	  /* free the memory allocated for async requests */
	  {
	     AHTPrintPendingRequestStatus (me->docid, YES);
	     AHTReqContext_delete (me);
	  }
	else if (me->reqStatus != HT_END && HTError_hasSeverity (HTRequest_error (me->request), ERR_NON_FATAL))
	  /* did the SYNC request end because of an error? If yes, report it back to the caller */
	   me->reqStatus = HT_ERR;
	return (0);
     }

   /* The request is still alive, so change it's status to indicate it's out of the
      critical section */
   me->reqStatus = HT_WAITING;
   return (0);
}


/*----------------------------------------------------------------
  Add_NewSocket_to_Loop
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
int                 Add_NewSocket_to_Loop (HTRequest * request, HTAlertOpcode op, int msgnum, const char *dfault, void *input, HTAlertPar * reply)
#else
int                 Add_NewSocket_to_Loop (request, op, msgnum, dfault, input, reply)
HTRequest          *request;
HTAlertOpcode       op;
int                 msgnum;
const char         *dfault;
void               *input;
HTAlertPar         *reply;

#endif /* __STDC__ */
{
   AHTReqContext      *me = HTRequest_context (request);

   if (me->reqStatus == HT_BUSY)
     /* request was aborted and now is is being reissued */
     {
       rewind (me->output);
       HTRequest_setOutputStream (me->request, AHTFWriter_new (me->request, me->output, YES));
     } else if (me->reqStatus == HT_NEW_PENDING)
       {
	 /* we are dequeing a pending request */
	 if (me->outputfile && (me->output = fopen (me->outputfile, "w")) == NULL)
	   {
	     /* the request is associated with a file */
	     me->outputfile[0] = '\0';	/* file could not be opened */
	     TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_CANNOT_CREATE_FILE),
			   me->outputfile);
	     me->reqStatus = HT_ERR;
	     return (HT_ERROR);
	   }
	 if (THD_TRACE)
	   fprintf (stderr, "Add_NewSocket_to_Loop: Activating pending %s . Open fd %d\n", me->urlName, (int) me->output);
	 HTRequest_setOutputStream (me->request,
				    AHTFWriter_new (me->request, me->output, YES));
       }

   /*change the status of the request */
   me->reqStatus = HT_WAITING;

   if (THD_TRACE)
     fprintf (stderr, "(Activating a pending request\n");

   return (HT_OK);
}


/*----------------------------------------------------------------------
  AHTEvent_register
  callback called by libwww whenever a socket is open and associated
  to a request. It sets the pertinent Xt events so that the Xt Event
  loops gets an interruption whenever there's action of the socket. 
  In addition, it registers the request with libwww.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 AHTEvent_register (SOCKET sock, HTRequest * rqp, SockOps ops, HTEventCallback * cbf, HTPriority p)
#else
int                 AHTEvent_register (sock, rqp, ops, cbf, p)
SOCKET              sock;
HTRequest          *rqp;
SockOps             ops;
HTEventCallback    *cbf;
HTPriority          p;

#endif /* __STDC__ */
{
   AHTReqContext      *me;      /* current request */
   int                 status;  /* libwww status associated with the socket number */

   if (sock == INVSOC)
      return (0);

   /* get the request associated to the socket number */

   if ((status = HTEventrg_register (sock, rqp, ops,
				     cbf, p)) != HT_OK)
      return (status);

   if (rqp)
     {
	me = HTRequest_context (rqp);

	/* verify if we need to open the fd */
	if (me->reqStatus == HT_NEW_PENDING)
	  {
	     /* we are opening a pending request */
	     if ((me->output = fopen (me->outputfile, "w")) == NULL)
	       {
		  me->outputfile[0] = '\0';	/* file could not be opened */
		  TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_CANNOT_CREATE_FILE),
				me->outputfile);
		  me->reqStatus = HT_ERR;
		  return (HT_ERROR);
	       }
	     HTRequest_setOutputStream (me->request,
			     AHTFWriter_new (me->request, me->output, YES));
	     me->reqStatus = HT_WAITING;

	     if (THD_TRACE)
		fprintf (stderr, "AHTEvent_register: Activating pending request url %s, fd %d\n", me->urlName, (int) me->output);
	  }

	if (THD_TRACE)
	   fprintf (stderr, "AHTEvent_register: url %s, sock %d, ops %lu \n",
		    me->urlName, sock, ops);

	/* add the input */
	if (me->reqStatus == HT_NEW)
	   me->reqStatus = HT_WAITING;

	if (ops & ReadBits)
	  {
	     me->read_ops = ops;
#ifndef _WINDOWS
	     RequestRegisterReadXtevent (me, sock);
#endif /* !_WINDOWS */
	  }

	if (ops & WriteBits)
	  {
	     me->write_ops = ops;
#ifndef _WINDOWS
	     RequestRegisterWriteXtevent (me, sock);
#endif /* !_WINDOWS */
	  }

	if (ops & ExceptBits)
	  {
	     me->except_ops = ops;
#ifndef _WINDOWS
	     RequestRegisterExceptXtevent (me, sock);
#endif /* !_WINDOWS */
	  }
     }

   return (status);
}


/*----------------------------------------------------------------------
  AHTEvent_unregister
  callback called by libwww each time a request is unregistered. This
  function takes care of unregistering the pertinent Xt events
  associated with the request's socket. In addition, it unregisters
  the request from libwww.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 AHTEvent_unregister (SOCKET sock, SockOps ops)
#else
int                 AHTEvent_unregister (sock, ops)
SOCKET              sock;
SockOps             ops;

#endif /* __STDC__ */
{
   int                 status;

   HTRequest          *rqp = NULL;
   AHTReqContext      *me;

   /* Libwww 5.0a does not take into account the third parameter
      **  for this function call */

   HTEventCallback    *cbf = (HTEventCallback *) __RetrieveCBF (sock, (SockOps) NULL, &rqp);

#ifndef _WINDOWS
   if (cbf)
     {
	if (rqp)
	  {
	     me = HTRequest_context (rqp);

	     if (ops & ReadBits)
		RequestKillReadXtevent (me);

	     if (ops & WriteBits)
		RequestKillWriteXtevent (me);

	     if (ops & ExceptBits)
		RequestKillExceptXtevent (me);
	  }
     }

   status = HTEventrg_unregister (sock, ops);
#endif /* !_WINDOWS */
   return (status);
}


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
#ifndef _WINDOWS
   if (THD_TRACE)
      fprintf (stderr, "Request_kill: Clearing Xtinputs\n");

   RequestKillReadXtevent (me);
   RequestKillWriteXtevent (me);
   RequestKillExceptXtevent (me);
#endif /* !_WINDOWS */
}

/*----------------------------------------------------------------------
  RequestRegisterReadXtevent
  Registers with Xt the read events associated with socket sock
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         RequestRegisterReadXtevent (AHTReqContext * me, SOCKET sock)
#else
static void         RequestRegisterReadXtevent (me, sock)
AHTReqContext      *me;
SOCKET sock;

#endif /* __STDC__ */
{
#ifndef _WINDOWS
  if (me->read_xtinput_id)
    {
      if (THD_TRACE)
	fprintf (stderr, "Request_kill: Clearing Xtinput %lu Socket %d R\n", me->read_xtinput_id, sock);
      XtRemoveInput (me->read_xtinput_id);
    }

  me->read_xtinput_id =
    XtAppAddInput (app_cont,
		   sock,
		   (XtPointer) XtInputReadMask,
		   (XtInputCallbackProc) AHTCallback_bridge,
		   (XtPointer) XtInputReadMask);

  if (THD_TRACE)
    fprintf (stderr, "(BT) adding Xtinput %lu Socket %d R\n",
	     me->read_xtinput_id, sock);
#endif /* !_WINDOWS */

}

/*----------------------------------------------------------------------
  RequestKillReadXtevent
  kills any read Xt event associated with the request pointed to by "me".
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         RequestKillReadXtevent (AHTReqContext * me)
#else
static void         RequestKillReadXtevent (me)
AHTReqContext      *me;

#endif /* __STDC__ */
{
#ifndef _WINDOWS
   if (me->read_xtinput_id)
     {
	if (THD_TRACE)
	   fprintf (stderr, "Request_kill: Clearing Xtinput %lu R\n", me->read_xtinput_id);
	XtRemoveInput (me->read_xtinput_id);
	me->read_xtinput_id = (XtInputId) NULL;
     }
#endif /* !_WINDOWS */
}

/*----------------------------------------------------------------------
  RequestRegisterWriteXtevent
  Registers with Xt the write events associated with socket sock
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         RequestRegisterWriteXtevent (AHTReqContext * me, SOCKET sock)
#else
static void         RequestRegisterWriteXtevent (me, sock)
AHTReqContext      *me;
SOCKET              sock;

#endif /* __STDC__ */
{
#ifndef _WINDOWS
   if (me->write_xtinput_id)
    {
      if (THD_TRACE)
	fprintf (stderr, "Request_kill: Clearing Xtinput %lu Socket %d W\n", me->write_xtinput_id, sock);
      XtRemoveInput (me->write_xtinput_id);
    }

  me->write_xtinput_id =
    XtAppAddInput (app_cont,
		   sock,
		   (XtPointer) XtInputWriteMask,
		   (XtInputCallbackProc) AHTCallback_bridge,
		   (XtPointer) XtInputWriteMask);

  if (THD_TRACE)
    fprintf (stderr, "(BT) adding Xtinput %lu Socket %d W\n",
	     me->write_xtinput_id, sock);
#endif /* !_WINDOWS */
}

/*----------------------------------------------------------------------
  RequestKillWriteXtevent
  kills any write Xt event associated with the request pointed to
  by "me".
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         RequestKillWriteXtevent (AHTReqContext * me)
#else
static void         RequestKillWriteXtevent (me)
AHTReqContext      *me;

#endif /* __STDC__ */
{
#ifndef _WINDOWS
   if (me->write_xtinput_id)
     {
	if (THD_TRACE)
	   fprintf (stderr, "Request_kill: Clearing Write Xtinputs %lu\n", me->write_xtinput_id);
	XtRemoveInput (me->write_xtinput_id);
	me->write_xtinput_id = (XtInputId) NULL;
     }
#endif /* !_WINDOWS */
}

/*----------------------------------------------------------------------
  RequestRegisterExceptXtevent
  Registers with Xt the except events associated with socket sock
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         RequestRegisterExceptXtevent (AHTReqContext * me, SOCKET sock)
#else
static void         RequestRegisterExceptXtevent (me, sock)
AHTReqContext      *me;
SOCKET              sock;

#endif /* __STDC__ */
{
#ifndef _WINDOWS
   if (me->except_xtinput_id)
     {
       if (THD_TRACE)
	   fprintf (stderr, "Request_kill: Clearing Xtinput %lu Socket %d E\n", me->except_xtinput_id, sock);
	XtRemoveInput (me->except_xtinput_id);
     }

  me->except_xtinput_id =
    XtAppAddInput (app_cont,
		   sock,
		   (XtPointer) XtInputExceptMask,
		   (XtInputCallbackProc) AHTCallback_bridge,
		   (XtPointer) XtInputExceptMask);

  if (THD_TRACE)
    fprintf (stderr, "(BT) adding Xtinput %lu Socket %d E\n",
	     me->write_xtinput_id, sock);

#endif /* !_WINDOWS */
}

/*----------------------------------------------------------------------
  RequestKillExceptXtevent
  kills any exception Xt event associated with the request pointed to
  by "me".
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         RequestKillExceptXtevent (AHTReqContext * me)
#else
static void         RequestKillExceptXtevent (me)
AHTReqContext      *me;

#endif /* __STDC__ */
{
#ifndef _WINDOWS
   if (me->except_xtinput_id)
     {
	if (THD_TRACE)
	   fprintf (stderr, "Request_kill: Clearing Except Xtinputs %lu\n", me->except_xtinput_id);
	XtRemoveInput (me->except_xtinput_id);
	me->except_xtinput_id = (XtInputId) NULL;
     }
#endif /* !_WINDOWS */
}

/*
  End of Module AHTBridge.c
*/

#endif /* !AMAYA_JAVA */




