/***
 *** Copyright (c) 1996 INRIA, All rights reserved
 ***/

/*                                                                  AHTBridge.c
 *      INTERFACE BRIDGE TO XT, LIBWWW, and AMAYA
 *
 *      (c) COPYRIGHT 
 *      Please first read the full copyright statement in the file COPYRIGH.
 *
 *      This module implments the callback setup and handlers between
 *      the Xt, libwww, and Amaya procedures
 *
 * History:
 *      May 02 96 JK    First semi-stable version, Jose Kahan
 *      June 01 96 JK   First almost-complete version, Jose Kahan 
 */

/* Unix/C/X */
#include "thot_gui.h"
#include "thot_sys.h"
#include "message.h"
#include "dialog.h"
#include "application.h"
#include "content.h"
#include "view.h"
#include "interface.h"
#include "amaya.h"

#if 0
#include "AHTCommon.h"
#include "query.h"
#endif

#include "AHTBridge.h"		/* implemented here */

#ifdef WWW_XWINDOWS
/* Amaya's X appcontext */
extern XtAppContext app_cont;

#endif


#ifndef HACK_WWW
extern PUBLIC HTEventCallback *HTEvent_Retrieve (SOCKET, SockOps, HTRequest ** arp);

#endif

/*
 * Private functions
 */
#ifdef __STDC__
static void         RequestKillReadXtevent (AHTReqContext *);
static void         RequestKillWriteXtevent (AHTReqContext *);
static void         RequestKillExceptXtevent (AHTReqContext *);

#else
static void         RequestKillReadXtevent ();
static void         RequestKillWriteXtevent ();
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
PRIVATE const SockOps ReadBits = FD_READ | FD_ACCEPT | FD_CLOSE;
PRIVATE const SockOps WriteBits = FD_WRITE | FD_CONNECT;
PRIVATE const SockOps ExceptBits = FD_OOB;

/*
 * Private functions
 */

/*
 * Callback that acts as a bridge between X and wwwlib.
 * This function is equivalent to the library's __DoCallback()
 * function, but with a different API, to conform to Xt's event loop
 * specifications. For more info, cf. the library's HTEvntrg.c module.
 */

#ifdef WWW_XWINDOWS
#ifdef __STDC__
XtInputCallbackProc AHTCallback_bridge (caddr_t cd, int *s, XtInputId * id)
#else
XtInputCallbackProc AHTCallback_bridge (cd, s, id)
caddr_t             cd;
int                *s;
XtInputId          *id;

#endif /* __STDC__ */

#else  /* WWW_XWINDOWS */
/* some winproc someday? */
LONG                AHTCallback_bridge (caddr_t cd, int *s)
#endif				/* !WWW_XWINDOWS */
{
   int                 status;
   HTRequest          *rqp = NULL;
   AHTReqContext      *me;
   SOCKET              sock;
   SockOps             ops;	/* what value goes here ? Ask eric */

   /* Libwww 4.1 does not take into account the third parameter
      for this function call */

#ifdef HACK_WWW
   HTEventCallback    *cbf;

#else
   HTEventCallback    *cbf = (HTEventCallback *) __RetrieveCBF (*s, ops, &rqp);

#endif
   me = HTRequest_context (rqp);

#if 0
   switch ((XtInputId) cd)
	 {
	    case XtInputReadMask:
	       ops = me->read_ops;
	       if (me->read_xtinput_id)
		 {
		    XtRemoveInput (me->read_xtinput_id);
		    if (THD_TRACE)
		       fprintf (stderr, "(BT) removing Xtinput %lu R (AHTBridge before cbf), sock %d\n", me->read_xtinput_id, *s);
		    me->read_xtinput_id = 0;
		 }
	       break;
	    case XtInputWriteMask:
	       ops = me->write_ops;
	       if (me->write_xtinput_id)
		 {
		    XtRemoveInput (me->write_xtinput_id);
		    if (THD_TRACE)
		       fprintf (stderr, "(BT) removing Xtinput %lu W (AHTBridge before cbf), sock %d\n", me->write_xtinput_id, *s);
		    me->write_xtinput_id = 0;
		 }
	       break;
	    case XtInputExceptMask:
	       ops = me->except_ops;
	       if (me->except_xtinput_id)
		 {
		    XtRemoveInput (me->except_xtinput_id);
		    if (THD_TRACE)
		       fprintf (stderr, "(BT) removing Xtinput %lu E (AHTBridge before cbf), sock %d\n", me->except_xtinput_id, *s);
		    me->except_xtinput_id = 0;
		 }
	       break;
	 }			/* switch */
#endif

   if (THD_TRACE)
      fprintf (stderr, "AHTBridge: Processing url %s \n", me->urlName);


#ifdef WWW_XWINDOWS
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
	 }			/* switch */
#endif /* WWW_XWINDOWS */

   /* 
    * Liberate the input, so that when a pending socket is activated,
    * the socket status will be ... available 
    * 
    * verify if I can CHKR_LIMIT this to the unregister function 
    * does not look so
    *
    * although it makes no sense, callbacks can be null 
    */

   if (!cbf || !rqp || rqp->priority == HT_PRIORITY_OFF)
     {
	if (THD_TRACE)
	   HTTrace ("Callback.... No callback found\n");
	/* put some more code to correctly destroy this request */
	return (0);
     }

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

   /* Has the request been stopped? 

    * we verify if the request exists. If it has ended, me will have
    *  a reqStatus with an HT_END value */

   /* Was the stop button pressed? */

#ifdef WWW_XWINDOWS
   if (me->reqStatus == HT_ABORT)
     {
	me->reqStatus = HT_WAITING;
	StopRequest (me->docid);
	if (THD_TRACE)
	   fprintf (stderr, "(BF) removing Xtinput %lu !RWE (Stop buttonl), sock %d\n", me->read_xtinput_id, sock);
	return (0);
     }
#endif /* WWW_XWINDOWS */

   /* the request is being reissued */

   if (me->reqStatus == HT_WAITING)
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


   /* verify if the request is still alive !! */

   if ((me->request->net == (HTNet *) NULL) || (me->reqStatus == HT_END || me->reqStatus == HT_ERR))
     {
	/* the socket is now being used by a different request, so the request has ended */
#ifdef WWW_XWINDOWS
	if (THD_TRACE)
	   fprintf (stderr, "(BF) removing Xtinput %lu !RWE, sock %d (Request has ended)\n", *id, *s);
#endif
	if ((me->mode & AMAYA_ASYNC) || (me->mode & AMAYA_IASYNC))
	  {
	     AHTPrintPendingRequestStatus (me->docid, YES);
	     AHTReqContext_delete (me);
	  }
	else if (me->reqStatus != HT_END && HTError_hasSeverity (HTRequest_error (me->request), ERR_NON_FATAL))
	   me->reqStatus = HT_ERR;
	return (0);
     }
   me->reqStatus = HT_WAITING;
   return (0);
}


/* 
 * This function is called whenever a socket is available
 * for a request. It  the necessary events to the Xt
 * A small interface to the HTLoadAnchor libwww function.
 * It prepares Xt to handle the asynchronous data requests.
 */

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
   SOCKET              req_socket;
   AHTReqContext      *me = HTRequest_context (request);

   /* AmayaOpenRequests *reqState; */

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
	if (THD_TRACE)
	   fprintf (stderr, "Add_NewSocket_to_Loop: Activating pending %s . Open fd %d\n", me->urlName, (int) me->output);
	HTRequest_setOutputStream (me->request,
			     AHTFWriter_new (me->request, me->output, YES));
     }

   me->reqStatus = HT_WAITING;

   if (THD_TRACE)
      fprintf (stderr, "(Activating a pending request\n");

   /* reusing this function to save on file descriptors */


   return (HT_OK);

    /***
    if(me->method == METHOD_PUT || me->method == METHOD_POST)
	return (HT_OK);
    ***/

   /* get the socket number associated to the request */

   req_socket = HTNet_socket (request->net);

   if (req_socket == INVSOC)
     {
	/* this should never be true */
	return (HT_ERROR);
     }

   /* add the input */

#ifdef WWW_XWINDOWS
   me->write_xtinput_id =
      XtAppAddInput (app_cont, req_socket, (XtPointer) XtInputWriteMask,
		     (XtInputCallbackProc) AHTCallback_bridge, NULL);
   if (THD_TRACE)
      fprintf (stderr, "(BT) adding   Xtinput %lu Socket %d W \n", me->write_xtinput_id, req_socket);

   if (me->write_xtinput_id == (XtInputId) NULL)
     {
	TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_XT_ERROR), me->urlName);

	/* I still need to add some error treatment here, to liberate memory */
	return (HT_ERROR);
     }

#endif /* WWW_XWINDOWS */
   /* To speed up the stop performances, we move the active requests to the top of the Amaya list */

   /*
      reqState  = DocRequestState(Amaya->open_requests, me->docid);

      if(reqState->counter > 1) {
      HTList_removeObject (Amaya->reqlist, (void *) me);
      HTList_addObject (Amaya->reqlist, (void *) me);
      }
    */
   return (HT_OK);
}


/* 
 * This function is called whenever a socket is available
 * for a request. It  the necessary events to the Xt
 * A small interface to the HTLoadAnchor libwww function.
 * It prepares Xt to handle the asynchronous data requests.
 */

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
   AHTReqContext      *me;
   int                 status;

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

#ifdef WWW_XWINDOWS
	     if (me->read_xtinput_id)
		XtRemoveInput (me->read_xtinput_id);
	     me->read_xtinput_id =
		XtAppAddInput (app_cont,
			       sock,
			       (XtPointer) XtInputReadMask,
			       (XtInputCallbackProc) AHTCallback_bridge,
			       (XtPointer) XtInputReadMask);
	     if (THD_TRACE)
		fprintf (stderr, "(BT) adding Xtinput %lu Socket %d R\n",
			 me->read_xtinput_id, sock);
#endif /* WWW_XWINDOWS */
	  }

	if (ops & WriteBits)
	  {
	     me->write_ops = ops;
#ifdef WWW_XWINDOWS
	     if (me->write_xtinput_id)
		XtRemoveInput (me->write_xtinput_id);
	     me->write_xtinput_id = XtAppAddInput (app_cont, sock,
					       (XtPointer) XtInputWriteMask,
				   (XtInputCallbackProc) AHTCallback_bridge,
					      (XtPointer) XtInputWriteMask);
	     if (THD_TRACE)
		fprintf (stderr, "(BT) adding Xtinput %lu Socket %d W\n",
			 me->write_xtinput_id, sock);
#endif /* WWW_XWINDOWS */
	  }

	if (ops & ExceptBits)
	  {
	     me->except_ops = ops;
#ifdef WWW_XWINDOWS
	     if (me->except_xtinput_id)
		XtRemoveInput (me->except_xtinput_id);

	     me->except_xtinput_id = XtAppAddInput (app_cont, sock,
					      (XtPointer) XtInputExceptMask,
				   (XtInputCallbackProc) AHTCallback_bridge,
					     (XtPointer) XtInputExceptMask);
	     if (THD_TRACE)
		fprintf (stderr, "(BT) adding Xtinput %lu Socket %d E\n", me->except_xtinput_id, sock);
#endif /* WWW_XWINDOWS */
	  }
     }



#if 0
   if (me->xtinput_id == (XtInputId) NULL)
     {
	TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_XT_ERROR), me->urlName);

	/* I still need to add some error treatment here, to liberate memory */
	return (HT_ERROR);
     }

#endif

   return (status);
}

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

   /* Libwww 4.1 does not take into account the third parameter
      **  for this function call */

   HTEventCallback    *cbf = (HTEventCallback *) __RetrieveCBF (sock, (SockOps) NULL, &rqp);

#ifdef WWW_XWINDOWS
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
#endif /* WWW_XWINDOWS */
   return (status);
}

#ifdef __STDC__
void                RequestKillAllXtevents (AHTReqContext * me)
#else
void                RequestKillAllXtevents (me)
AHTReqContext      *me;

#endif /* __STDC__ */
{
#ifdef WWW_XWINDOWS
   if (THD_TRACE)
      fprintf (stderr, "Request_kill: Clearing Xtinputs\n");

   RequestKillReadXtevent (me);
   RequestKillWriteXtevent (me);
   RequestKillExceptXtevent (me);
#endif /* WWW_XWINDOWS */
}

#ifdef __STDC__
static void         RequestKillReadXtevent (AHTReqContext * me)
#else
static void         RequestKillReadXtevent (me)
AHTReqContext      *me;

#endif /* __STDC__ */
{
#ifdef WWW_XWINDOWS
   if (me->read_xtinput_id)
     {
	if (THD_TRACE)
	   fprintf (stderr, "Request_kill: Clearing Read Xtinputs%lu\n", me->read_xtinput_id);
	XtRemoveInput (me->read_xtinput_id);
	me->read_xtinput_id = (XtInputId) NULL;
     }
#endif /* WWW_XWINDOWS */
}

#ifdef __STDC__
static void         RequestKillWriteXtevent (AHTReqContext * me)
#else
static void         RequestKillWriteXtevent (me)
AHTReqContext      *me;

#endif /* __STDC__ */
{
#ifdef WWW_XWINDOWS
   if (me->write_xtinput_id)
     {
	if (THD_TRACE)
	   fprintf (stderr, "Request_kill: Clearing Write Xtinputs %lu\n", me->write_xtinput_id);
	XtRemoveInput (me->write_xtinput_id);
	me->write_xtinput_id = (XtInputId) NULL;
     }
#endif /* WWW_XWINDOWS */
}

#ifdef __STDC__
static void         RequestKillExceptXtevent (AHTReqContext * me)
#else
static void         RequestKillExceptXtevent (me)
AHTReqContext      *me;

#endif /* __STDC__ */
{
#ifdef WWW_XWINDOWS
   if (me->except_xtinput_id)
     {
	if (THD_TRACE)
	   fprintf (stderr, "Request_kill: Clearing Except Xtinputs %lu\n", me->except_xtinput_id);
	XtRemoveInput (me->except_xtinput_id);
	me->except_xtinput_id = (XtInputId) NULL;
     }
#endif /* WWW_XWINDOWS */
}
