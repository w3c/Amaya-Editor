/***
 *** Copyright (c) 1996 INRIA, All rights reserved
 ***/

/* Amaya includes */

#include "amaya.h"
#include "dialog.h"
#include "content.h"
#include "view.h"
#include "interface.h"
#include "amaya.h"
#include "message.h"
#include "application.h"
#include "AHTURLTools.h"
#include "AHTBridge.h"
#include "AHTMemConv.h"

#if defined(__svr4__)
#define CATCH_SIG
#endif

/* local structures coming from libwww and which are
   ** not found in any .h file
 */

struct _HTStream
  {
     HTStreamClass      *isa;
  };

struct _HTError
  {
     HTErrorElement      element;	/* Index number into HTError */
     HTSeverity          severity;	/* A la VMS */
     BOOL                ignore;	/* YES if msg should not go to user */
     void               *par;	/* Explanation, e.g. filename  */
     int                 length;	/* For copying by generic routine */
     char               *where;	/* Which function */
  };


struct _HTHost
  {
     char               *hostname;	/* name of host + optional port */
     time_t              ntime;	/* Creation time */
     char               *type;	/* Peer type */
     int                 version;	/* Peer version */
     HTMethod            methods;	/* Public methods (bit-flag) */
     char               *server;	/* Server name */
     char               *user_agent;	/* User Agent */
     char               *range_units;	/* Acceptable range units */
     HTTransportMode     mode;	/* Supported mode */
     HTChannel          *channel;	/* Persistent channel */
     HTList             *pipeline;	/* Pipe line of net objects */
     HTList             *pending;	/* List of pending Net objects */
     time_t              expires;	/* Persistent channel expires time */
  };

/* Type definitions and global variables etc. local to this module */


/**** Global variables ****/

AmayaContext       *Amaya;	/* Amaya's global context */

/*** private variables ***/

static HTList      *converters = NULL;	/* List of global converters */
static HTList      *encodings = NULL;

/*** private functions ***/

/***
#ifdef __STDC__
char *ExtractFileName(char);
#else
char *ExtractFileName();
#endif *__STDC__*
***/


#ifdef __STDC__
static int          AHTUpload_callback (HTRequest *, HTStream *);
static AHTReqContext *AHTReqContext_new (int);
static void         Thread_deleteAll (void);
static int          authentication_handler (HTRequest *, void *, int);
static int          redirection_handler (HTRequest *, HTResponse *, void *, int);
static int          terminate_handler (HTRequest * request, HTResponse *, void *, int);
static int          AHTLoadTerminate_handler (HTRequest *, HTResponse *, void *, int);
static int          LoopForStop (AHTReqContext *);
static void         AHTProfile_delete (void);
static void         AHTAlertInit (void);

#else
static int          AHTUpload_callback ();
static AHTReqContext *AHTReqContext_new ();
static void         Thread_deleteAll ();
static int          authentication_handler ();
static int          redirection_handler ();
static int          terminate_handler ();
static int          AHTLoadTerminate_handler ();
static int          LoopForStop ();
static void         AHTProfile_delete ();
static void         AHTAlertInit ();

#endif

#ifdef CATCH_SIG

/*----------------------------------------------------------------------
   SetSignal: This function sets up signal handlers. This might not 
   be necessary to call if the application has its own handlers.    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SetSignal (void)
#else
static void         SetSignal ()
#endif
{
   /* On some systems (SYSV) it is necessary to catch the SIGPIPE signal
      ** when attemting to connect to a remote host where you normally should
      ** get `connection refused' back
    */
   if (signal (SIGPIPE, SIG_IGN) == SIG_ERR)
     {
	if (PROT_TRACE)
	   HTTrace ("HTSignal.... Can't catch SIGPIPE\n");
     }
   else
     {
	if (PROT_TRACE)
	   HTTrace ("HTSignal.... Ignoring SIGPIPE\n");
     }
}
#endif /* CATCH_SIG */

/*----------------------------------------------------------------------
   Create a new Amaya Context Object
 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static AHTReqContext *AHTReqContext_new (int docid)
#else
static AHTReqContext *AHTReqContext_new (docid)
int                 docid;

#endif
{
   AHTReqContext      *me;
   AHTDocId_Status    *docid_status;

   if ((me = (AHTReqContext *) TtaGetMemory (sizeof (AHTReqContext))) == NULL)
      outofmem (__FILE__, "Context_new");

   /* Bind the Context object together with the Request Object */

   me->request = HTRequest_new ();

/*** This is for correcting the keep-alive bug. Is this interfering with */
/**** libwww v5? */

/* test 
   me->request->GenMask &= ~((int) HT_G_CONNECTION);
 */

   /* Initialize the other members of the structure */
   me->reqStatus = HT_NEW;
   me->output = NULL;
#ifdef WWW_XWINDOWS
   me->read_xtinput_id = (XtInputId) NULL;
   me->write_xtinput_id = (XtInputId) NULL;
   me->except_xtinput_id = (XtInputId) NULL;
#endif
   me->docid = docid;
   HTRequest_setConversion (me->request, converters, YES);
   HTRequest_setMethod (me->request, METHOD_GET);
   HTRequest_setOutputFormat (me->request, WWW_SOURCE);
   HTRequest_setContext (me->request, me);

   /* to interface with Eric's new routines */
   me->read_ops = 0;
   me->write_ops = 0;
   me->except_ops = 0;

   /* Update the global context */
   HTList_appendObject (Amaya->reqlist, (void *) me);

   docid_status = GetDocIdStatus (docid, Amaya->docid_status);

   if (docid_status == NULL)
     {
	docid_status = (AHTDocId_Status *) TtaGetMemory (sizeof (AHTDocId_Status));
	docid_status->docid = docid;
	docid_status->counter = 1;
	HTList_addObject (Amaya->docid_status, (void *) docid_status);
     }
   else
      docid_status->counter++;


   Amaya->open_requests++;

   /* error stream handling */
   me->error_stream = (char *) NULL;
   me->error_stream_size = 0;

   return me;
}

/*----------------------------------------------------------------------
   Delete an Amaya Context Object                                 
  ----------------------------------------------------------------------*/

#ifdef __STDC__
BOOL                AHTReqContext_delete (AHTReqContext * me)
#else
BOOL                AHTReqContext_delete (me)
AHTReqContext      *me;

#endif
{
   AHTDocId_Status    *docid_status;

   if (me)
     {

	if (Amaya->reqlist)
	   HTList_removeObject (Amaya->reqlist, (void *) me);

	docid_status = GetDocIdStatus (me->docid, Amaya->docid_status);

	if (docid_status)
	  {
	     docid_status->counter--;

	     if (docid_status->counter == 0)
	       {
		  HTList_removeObject (Amaya->docid_status, (void *) docid_status);
		  TtaFreeMemory ((void *) docid_status);
	       }
	  }
	HTRequest_delete (me->request);

	if (me->error_stream != (char *) NULL)
	   HT_FREE (me->error_stream);

	TtaFreeMemory ((void *) me);

	Amaya->open_requests--;

	return YES;

     }
   return NO;
}


#ifdef __STDC__
static int          AHTUpload_callback (HTRequest * request, HTStream * target)
#else
static int          AHTUpload_callback (request, target)
HTRequest          *request;
HTStream           *target;

#endif
{
   AHTReqContext      *me = HTRequest_context (request);
   HTParentAnchor     *entity = HTRequest_entityAnchor (request);
   int                 len = HTAnchor_length (entity);
   int                 status;

   /* Send the data down the pipe */

   status = (*target->isa->put_block) (target, me->mem_ptr, len);

   if (status == HT_LOADED || status == HT_OK)
     {
	if (PROT_TRACE)
	   HTTrace ("Posting Data Target is SAVED\n");
	(*target->isa->flush) (target);
	return HT_LOADED;
     }
   if (status == HT_WOULD_BLOCK)
     {
	if (PROT_TRACE)
	   HTTrace ("Posting Data Target WOULD BLOCK\n");
	return HT_WOULD_BLOCK;
     }
   else if (status == HT_PAUSE)
     {
	if (PROT_TRACE)
	   HTTrace ("Posting Data Target PAUSED\n");
	return HT_PAUSE;
     }
   else if (status > 0)
     {				/* Stream specific return code */
	if (PROT_TRACE)
	   HTTrace ("Posting Data. Target returns %d\n", status);
	return status;
     }
   else
     {				/* we have a real error */
	if (PROT_TRACE)
	   HTTrace ("Posting Data Target ERROR %d\n", status);
	return status;
     }
}


/*----------------------------------------------------------------------
   Gets the status associated to a docid                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
AHTDocId_Status    *GetDocIdStatus (int docid, HTList * documents)
#else
AHTDocID_Status    *GetDocIdStatus (docid, documents)
int                 docid;
HTList             *documents;

#endif
{
   AHTDocId_Status    *me;
   HTList             *cur;

   if (documents)
     {
	cur = documents;

	while ((me = (AHTDocId_Status *) HTList_nextObject (cur)))
	  {
	     if (me->docid == docid)
		return (me);
	  }			/* while */
     }				/* if */
   return (AHTDocId_Status *) NULL;

}

/*----------------------------------------------------------------------
   This function deletes the whole list of active threads.           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         Thread_deleteAll (void)
#else
static void         Thread_deleteAll ()
#endif
{
   if (Amaya && Amaya->reqlist)
     {
	if (Amaya->open_requests > 0)
	  {
	     HTList             *cur = Amaya->reqlist;
	     AHTReqContext      *me;
	     AHTDocId_Status    *docid_status;

	     HTNet_killAll ();
	     /* erase the requests */
	     while ((me = (AHTReqContext *) HTList_nextObject (cur)))
	       {
		  if (me->request)
		    {
#ifdef WWW_XWINDOWS
		       RequestKillAllXtevents (me);
#endif /* WWW_XWINDOWS */
		       /*  HTRequest_kill(me->request);
		          if(me->output)    
		          fclose(me->output);
		        */
		       AHTReqContext_delete (me);
		    }
	       }		/* while */
	     HTList_delete (Amaya->reqlist);
	     /* erase the docid_status entities */
	     while ((docid_status = (AHTDocId_Status *) HTList_removeLastObject ((void *) Amaya->docid_status)))
		TtaFreeMemory ((void *) docid_status);

	     HTList_delete (Amaya->reqlist);
	  }			/* if */
	TtaFreeMemory ((void *) Amaya);
     }
}

/*----------------------------------------------------------------------
   authentication_handler                                         
   This function is registered to handle access authentication,   
   for example for HTTP                                           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          authentication_handler (HTRequest * request, void *context, int status)
#else
static int          authentication_handler (request, context, status)
HTRequest          *request;
void               *context;
int                 status;

#endif
{
   AHTReqContext      *me = HTRequest_context (request);


   /* check how many times we have passed this way, to protect against
      ** traviserrors (tm) 
    */

   /* Ask the authentication module for getting credentials */

   if (HTRequest_retrys (request) /*&& HTBasic_parse (request, context, status) */ )
     {

	/* Make sure we do a reload from cache */
	/*
	   #ifndef HACK_WWW
	   HTRequest_setReloadMode(request, HT_FORCE_RELOAD);
	   #endif
	 */
	/* Log current request */
	if (HTLog_isOpen ())
	   HTLog_add (request, status);

	/* Start request with new credentials */

    /***
    if(me->xtinput_id != (XtInputId) NULL ) {
      if (THD_TRACE)
        fprintf(stderr, "AH: Removing Xtinput: %lu\n",
                me->xtinput_id);
      XtRemoveInput(me->xtinput_id);
      me->xtinput_id = (XtInputId) NULL;
    }
    ***/

	/* Start request with new credentials */
	me->reqStatus = HT_NEW;

	if (me->method == METHOD_PUT || me->method == METHOD_POST)
	   /* PUT, POST etc. */
	   /*      HTCopyAnchor((HTAnchor *) HTRequest_anchor(request), request); */
	   status = HTLoadAbsolute (me->urlName, request);
	else
	   HTLoadAnchor ((HTAnchor *) HTRequest_anchor (request), request);

     }
   else
     {
	TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_ACCESS_DENIED),
		      me->urlName);
	me->reqStatus = HT_ERR;
	if (me->error_html)
	   FilesLoading[me->docid] = 2;		/* so we can show the error message */
     }

   return HT_ERROR;		/* Make sure this is the last callback in the list */
}


/*----------------------------------------------------------------------
   redirection_handler
   **      This function is registered to handle permanent and temporary
   **      redirections
 

   [x] Verfiy Put, Post
   [ ] Verify if NormalizeURL is not redundant
   [ ] Errors, should be done here or in terminate handler??
 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          redirection_handler (HTRequest * request, HTResponse * response, void *param, int status)
#else
static int          redirection_handler (request, context, status)
HTRequest          *request;
HTResponse         *response;
void               *param;
int                 status;

#endif
{

   char               *ref;
   HTAnchor           *new_anchor = HTResponse_redirection (response);
   AHTReqContext      *me = HTRequest_context (request);
   HTMethod            method = HTRequest_method (request);


   if (!new_anchor)
     {
	if (PROT_TRACE)
	   HTTrace ("Redirection. No destination\n");
	return HT_OK;
     }

   /*
      ** Only do redirect on GET and HEAD
    */
   if (!HTMethod_isSafe (method))
     {
	HTAlertCallback    *prompt = HTAlert_find (HT_A_CONFIRM);

	if (prompt)
	  {
	     if ((*prompt) (request, HT_A_CONFIRM, HT_MSG_REDIRECTION,
			    NULL, NULL, NULL) != YES)
		return HT_ERROR;
	  }
     }

   /*
    **  Start new request with the redirect anchor found in the headers.
    **  Note that we reuse the same request object which means that we must
    **  keep this around until the redirected request has terminated. It also           
    **  allows us in an easy way to keep track of the number of redirections
    **  so that we can detect endless loops.
    */
   if (HTRequest_doRetry (request))
     {

	/* Verify if this is not redundant */

	/* do we need to normalize the URL? */
	if (strncmp (new_anchor->parent->address, "http:", 5))
	  {
	     /* Yes, so we use the pre-redirection anchor as a base name */
	     ref = HTParse (new_anchor->parent->address, me->urlName, PARSE_ALL);
	     if (ref)
	       {
		  HT_FREE (new_anchor->parent->address);
		  new_anchor->parent->address = ref;
	       }
	  }

	/* update the current file name */
	if (strlen (new_anchor->parent->address) > (MAX_LENGTH - 1))
	  {
	     /*
	        ** copy MAX_LENGTH cars. The error will be detected later on and shown on the
	        ** screen. This code will be fixed up later on
	      */
	     strncpy (me->urlName, new_anchor->parent->address, MAX_LENGTH);
	     me->urlName[MAX_LENGTH] = '\0';
	  }
	else
	   strcpy (me->urlName, new_anchor->parent->address);

	TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_RED_FETCHING),
		      me->urlName);

	/* Start request with new credentials */

	me->reqStatus = HT_NEW;
	if (me->method == METHOD_PUT || me->method == METHOD_POST)	/* PUT, POST etc. */
	   /*  HTCopyAnchor((HTAnchor *) HTRequest_anchor(request), request); */
	   status = HTLoadAbsolute (me->urlName, request);
	else
	   HTLoadAnchor (new_anchor, request);
     }
   else
     {
	HTRequest_addError (request, ERR_FATAL, NO, HTERR_MAX_REDIRECT,
			    NULL, 0, "HTRedirectFilter");
	TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_REDIRECTIONS_LIMIT),
		      NULL);
	if (me->error_html)
	   FilesLoading[me->docid] = 2;		/* so we can show the error message */
     }

   /*
      **  By returning HT_ERROR we make sure that this is the last handler to be
      **  called. We do this as we don't want any other filter to delete the 
      **  request object now when we have just started a new one ourselves
    */
   return HT_ERROR;
}

/*----------------------------------------------------------------------
   terminate_handler
   **   This function is registered to handle the result of the request
 
  ----------------------------------------------------------------------*/
#if __STDC__
static int          terminate_handler (HTRequest * request, HTResponse * response, void *context, int status)
#else
static int          terminate_handler (request, response, context, status)
HTRequest          *request;
HTResponse         *response;
void               *context;
int                 status;

#endif
{
   AHTReqContext      *me = (AHTReqContext *) HTRequest_context (request);

   if (!me)
      return HT_OK;		/* not an Amaya request */

   /* output any errors from the server */

    /***
    ** me->output = output file which will receive an html file
    ** me->error_html = yes, output HTML errors in the screen
    ** request->error_stack == if there are any errors, they will be here
    ** me->error_stream_size If it's != 0 means an error message has already
    **                       been written to the stack
    */

   /* First, we verify if there are any errors and if they are not
      ** yet written to the error stack. If no, then let's try to write them
      ** ourselves
    */

   if (me->output && me->output != stdout)
     {
	/* we are writing to a file */
	if (me->reqStatus != HT_ABORT)
	  {			/* if the request was not aborted and */
	     if (status != HT_LOADED)
	       {		/* there were some errors */
		  if (me->error_html == YES)
		    {		/* and we want to print errors */
		       if (me->error_stream_size == 0)	/* and the stream is empty */
			  AHTError_MemPrint (request);	/* copy errors from the error stack 
							   ** into a data structure */
		       if (me->error_stream)
			 {	/* if the stream is non-empty */
			    fprintf (me->output, me->error_stream);	/* output the errors */
			    status = HT_LOADED;		/* show it in the HTML window */
			 }
		       else
			  me->reqStatus = HT_ERR;	/* we did not get an error msg, 
							   ** so just
							   **  mark error 
							 */
		    }
		  else
		     me->reqStatus = HT_ERR;	/* we don't want to print the error */
	       }		/* if error_stack */
	  }			/* if it isn't an abort */
	fclose (me->output);
     }
   else
     {
	/* We must be doing a PUT. Verify if there was an error */
	if (status != HT_LOADED)
	   me->reqStatus = HT_ERR;
     }				/* if me-output */

   /* setup the request status and invoke the cbf */

   /* work to be done: verify if we can put join all the terminate cbf in
      only one call after the following lines */

   if (status == HT_LOADED && me->reqStatus != HT_ERR
       && me->reqStatus != HT_ABORT)
     {
	me->reqStatus = HT_END;	/* no errors */
	if (me->terminate_cbf)
	   (*me->terminate_cbf) ((AHTReqContext *) me,
				 HT_LOADED);
     }
   else if (me->reqStatus == HT_ABORT)
     {
	if (me->terminate_cbf)
	   (*me->terminate_cbf) ((AHTReqContext *) me,
				 HT_ERROR);
	if (me->outputfile && me->outputfile[0] != EOS)
	  {
	     RemoveFile (me->outputfile);
	     me->outputfile[0] = EOS;
	  }
     }
   else if (me->reqStatus == HT_ERR)
     {
	/* there was an error */
	if (me->terminate_cbf)
	   (*me->terminate_cbf) ((AHTReqContext *) me,
				 HT_ERROR);

	if (me->outputfile && me->outputfile[0] != EOS)
	  {
	     RemoveFile (me->outputfile);
	     me->outputfile[0] = EOS;
	  }
     }
   else if (status != HT_LOADED &&
	    (me->reqStatus == HT_BUSY || me->reqStatus == HT_WAITING))
     {
	/* there was an error */
	if (me->terminate_cbf)
	   (*me->terminate_cbf) ((AHTReqContext *) me,
				 HT_ERROR);

	if (me->outputfile && me->outputfile[0] != EOS)
	  {
	     RemoveFile (me->outputfile);
	     me->outputfile[0] = EOS;
	     me->reqStatus = HT_ERR;
	  }
     }				/* if-else HT_END, HT_ABORT, HT_ERROR */
   if (HTLog_isOpen ())
      HTLog_add (request, status);

   if ((me->mode & AMAYA_ASYNC) || (me->mode & AMAYA_IASYNC))
     {
	TtaFreeMemory (me->urlName);
	TtaFreeMemory (me->outputfile);
	/*      AHTReqContext_delete(me); */
     }
   /* don't remove or Xt will hang up during the put */

   if (me->method == METHOD_PUT || me->method == METHOD_POST)
     {
	TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_PROG_WRITE),
		      me->urlName);

     }
   return HT_OK;
}

/*----------------------------------------------------------------------
   Application "AFTER" Callback                                 
   This function uses all the functionaly that the app part of  
   the Library gives for handling AFTER a request.              
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static int          AHTLoadTerminate_handler (HTRequest * request, HTResponse * response, void *param, int status)
#else
static int          AHTLoadTerminate_handler (request, response, param, status)
HTRequest          *request;
HTResponse         *response;
void               *param;
int                 status;

#endif
{
   char               *uri = HTAnchor_address ((HTAnchor *) request->anchor);
   AHTReqContext      *me = HTRequest_context (request);
   HTAlertCallback    *cbf;
   AHTDocId_Status    *docid_status;

   switch (status)
	 {
	    case HT_LOADED:
	       if (PROT_TRACE)
		  HTTrace ("Load End.... OK: `%s\' has been accessed\n",
			   uri);

	       docid_status = GetDocIdStatus (me->docid,
					      Amaya->docid_status);

	       if (docid_status != NULL && docid_status->counter > 1)
		  TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA,
						   AM_ELEMENT_LOADED), uri);

	       break;

	    case HT_NO_DATA:
	       if (PROT_TRACE)
		  HTTrace ("Load End.... OK BUT NO DATA: `%s\'\n", uri);
	       TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_LOADED_NO_DATA),
			     uri);
	       break;

	    case HT_INTERRUPTED:
	       if (PROT_TRACE)
		  HTTrace ("Load End.... INTERRUPTED: `%s\'\n", uri);
	       TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_LOAD_ABORT), NULL);

	       break;

	    case HT_RETRY:
	       if (PROT_TRACE)
		  HTTrace ("Load End.... NOT AVAILABLE, RETRY AT %ld\n",
			   HTResponse_retryTime (response));
	       TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_NOT_AVAILABLE_RETRY),
			     uri);
	       break;

	    case HT_ERROR:

	       cbf = HTAlert_find (HT_A_MESSAGE);
	       if (cbf)
		  (*cbf) (request, HT_A_MESSAGE, HT_MSG_NULL, NULL,
			  HTRequest_error (request), NULL);
	       break;

	       if (PROT_TRACE)
		  HTTrace ("Load End.... ERROR: Can't access `%s\'\n",
			   uri ? uri : "<UNKNOWN>");
	       TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_CANNOT_LOAD),
			     uri ? uri : "<UNKNOWN>");
	       break;
	    default:
	       if (PROT_TRACE)
		  HTTrace ("Load End.... UNKNOWN RETURN CODE %d\n", status);
	       break;
	 }

   /* Should we do logging? */
   if (HTLog_isOpen ())
      HTLog_add (request, status);
   HT_FREE (uri);

   return HT_OK;
}

/*
   **   After filter for handling PUT of document. We should now have the 
 */
static int          AHTSaveFilter (HTRequest * request, HTResponse * response, void *param, int status)
{
   AHTReqContext      *me = HTRequest_context (request);

   if (APP_TRACE)
      HTTrace ("Save Filter\n");

   /*
      **  Just ignore authentication in the hope that some other filter will
      **  handle this.
    */

   if (status == HT_NO_ACCESS || status == HT_NO_PROXY_ACCESS)
     {
	if (APP_TRACE)
	   HTTrace ("Save Filter. Waiting for authentication\n");
	status = HT_OK;
     }
   else if (status == HT_TEMP_REDIRECT || status == HT_PERM_REDIRECT)
     {
	/*
	   **  The destination entity has been redirected. Ask the user
	   **  what to do. If there is no user confirmation to go ahead then stop
	 */
	/* put here the equiv. code of redirection handler, and a prompt
	   for user */
	HTAlertCallback    *prompt = HTAlert_find (HT_A_CONFIRM);
	HTAnchor           *redirection = HTResponse_redirection (response);

	if (prompt && redirection)
	  {
	     if ((*prompt) (request, HT_A_CONFIRM, HT_MSG_SOURCE_MOVED,
			    NULL, NULL, NULL) == YES)
	       {
		  return HT_OK;
	       }
	     else
	       {
		  /*
		     ** Make sure that the operation stops
		   */
		  me->reqStatus = HT_ABORT;

		  /* add a message? */

		  status = HT_ERROR;
	       }
	  }
	status = HT_OK;
     }
   else if (status == HT_ERROR)
     {
	if (me->reqStatus != HT_ABORT && me->reqStatus != HT_END)
	   me->reqStatus = HT_ERR;
     }
   else
     {
	/*
	   ** No authentication or redirection was detected ... let's erase this filter
	   **
	 */
	HTRequest_deleteAfter (request, AHTSaveFilter);
	status = HT_OK;
     }

   return status;

}


/*----------------------------------------------------------------------
   BINDINGS BETWEEN A SOURCE MEDIA TYPE AND A DEST MEDIA TYPE (CONVERSION) 
  ----------------------------------------------------------------------*/

static void         AHTConverterInit (HTList * c)
{

   /* Handler for custom http error messages */
   HTConversion_add (c, "*/*", "www/debug", AHTMemConverter, 1.0, 0.0, 0.0);

   /*
    ** These are converters that converts to something other than www/present,
    ** that is not directly outputting someting to the user on the screen
    */

   HTConversion_add (c, "message/rfc822", "*/*", HTMIMEConvert, 1.0, 0.0, 0.0);
   HTConversion_add (c, "message/x-rfc822-foot", "*/*", HTMIMEFooter,
		     1.0, 0.0, 0.0);
   HTConversion_add (c, "message/x-rfc822-head", "*/*", HTMIMEHeader,
		     1.0, 0.0, 0.0);
   HTConversion_add (c, "multipart/*", "*/*", HTBoundary,
		     1.0, 0.0, 0.0);
   HTConversion_add (c, "text/plain", "text/html", HTPlainToHTML,
		     1.0, 0.0, 0.0);


   /*
      ** The following conversions are converting ASCII output from various
      ** protocols to HTML objects.
    */
   HTConversion_add (c, "text/x-http", "*/*", HTTPStatus_new,
		     1.0, 0.0, 0.0);
   HTConversion_add (c, "text/x-nntp-list", "*/*", HTNewsList,
		     1.0, 0.0, 0.0);
   HTConversion_add (c, "text/x-nntp-over", "*/*", HTNewsGroup,
		     1.0, 0.0, 0.0);


   /*
    ** We also register a special content type guess stream that can figure out
    ** the content type by reading the first bytes of the stream
    */
   HTConversion_add (c, "www/unknown", "*/*", HTGuess_new,
		     1.0, 0.0, 0.0);

   /*
      ** Register a persistent cache stream which can save an object to local
      ** file
    */
   HTConversion_add (c, "www/cache", "*/*", HTCacheWriter,
		     1.0, 0.0, 0.0);

   /*
      ** This dumps all other formats to local disk without any further
      ** action taken
    */
   HTConversion_add (c, "*/*", "www/present", HTSaveLocally,
		     0.3, 0.0, 0.0);

#if 0				/* no use to send this in our request */
   HTConversion_add (c, "application/octet-stream", "www/present",
		     HTSaveLocally, 0.1, 0.0, 0.0);
   HTConversion_add (c, "application/x-compressed", "www/present",
		     HTSaveLocally, 0.1, 0.0, 0.0);
#endif

   /* Normal converters */
   /* this one was 0.5 before */
#if 0
   HTConversion_add (c, "*/*", "www/present", HTSaveLocally, 1.0, 0.0, 0.0);
#endif

}

/*      REGISTER BEFORE AND AFTER FILTERS
   **      We register a commonly used set of BEFORE and AFTER filters.
   **      Not done automaticly - may be done by application!
 */

/*      REGISTER ALL AMAYA SUPPORTED PROTOCOLS
 */
static void         AHTProtocolInit (void)
{

   /* NB. Preemptive == YES = Blocking request
      ** Non-preemptive == NO = Non-blocking request
    */

   HTProtocol_add ("http", "buffered_tcp", NO, HTLoadHTTP, NULL);
   /*   HTProtocol_add ("http", "tcp", NO, HTLoadHTTP, NULL); */
   HTProtocol_add ("file", "local", NO, HTLoadFile, NULL);
   HTProtocol_add ("cache", "local", NO, HTLoadCache, NULL);
   HTProtocol_add ("telnet", "", YES, HTLoadTelnet, NULL);
   HTProtocol_add ("tn3270", "", YES, HTLoadTelnet, NULL);
   HTProtocol_add ("rlogin", "", YES, HTLoadTelnet, NULL);

   HTProtocol_add ("ftp", "tcp", NO, HTLoadFTP, NULL);
   HTProtocol_add ("nntp", "tcp", NO, HTLoadNews, NULL);
   HTProtocol_add ("news", "tcp", NO, HTLoadNews, NULL);
}

static void         AHTNetInit (void)
{

/*      Register BEFORE filters
   **      The BEFORE filters handle proxies, caches, rule files etc.
   **      The filters are called in the order by which the are registered
   **      Not done automaticly - may be done by application!
 */


   /*#ifndef HACK_WWW */
   HTNet_addBefore (HTCredentialsFilter, "http://*", NULL, 6);
   HTNet_addBefore (HTProxyFilter, NULL, NULL, 10);

   /*#endif */

/*      register AFTER filters
   **      The AFTER filters handle error messages, logging, redirection,
   **      authentication etc.
   **      The filters are called in the order by which the are registered
   **      Not done automaticly - may be done by application!
 */

#ifndef HACK_WWW

   HTNet_addAfter (HTAuthFilter, "http://*", NULL, HT_NO_ACCESS,
		   5);

   HTNet_addAfter (redirection_handler, "http://*", NULL, HT_TEMP_REDIRECT,
		   5);
   HTNet_addAfter (redirection_handler, "http://*", NULL, HT_PERM_REDIRECT,
		   5);
   HTNet_addAfter (HTUseProxyFilter, "http://*", NULL, HT_USE_PROXY,
		   5);

   HTNet_addAfter (AHTLoadTerminate_handler, NULL, NULL, HT_ALL, HT_FILTER_LAST);	/* handles all errors */
   HTNet_addAfter (terminate_handler, NULL, NULL, HT_ALL, HT_FILTER_LAST);
#endif
}

static void         AHTProfile_newAmaya (const char *AppName, const char *AppVersion)
{
   /* If the Library is not already initialized then do it */
   if (!HTLib_isInitialized ())
      HTLibInit (AppName, AppVersion);

   if (!converters)
      converters = HTList_new ();
   if (!encodings)
      encodings = HTList_new ();

   /* Register the default set of transport protocols */
   HTTransportInit ();

   /* Register the default set of application protocol modules */
#ifndef HACK_WWW
   AHTProtocolInit ();
#endif

   /* Enable the persistent cache */
   /*   HTCacheInit (NULL, 20); */

   /* Register the default set of BEFORE and AFTER filters */
   AHTNetInit ();

   /* Set up the default set of Authentication schemes */
   HTAAInit ();

   /* Get any proxy or gateway environment variables */
   HTProxy_getEnvVar ();

   /* Register the default set of converters */
   AHTConverterInit (converters);
   HTFormat_setConversion (converters);

   /* Register the default set of transfer encoders and decoders */
   HTEncoderInit (encodings);	/* chunks ??? */
   HTFormat_setTransferCoding (encodings);

   /* Register the default set of MIME header parsers */
   HTMIMEInit ();		/* must be called again for language selector */

   /* Register the default set of file suffix bindings */
   HTFileInit ();

   /* Register the default set of Icons for directory listings */
   /*HTIconInit(NULL); *//*is this useful ? */

   /* Register the default set of messages and dialog functions */
   AHTAlertInit ();
   HTAlert_setInteractive (YES);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         AHTProfile_delete (void)
#else
static void         AHTProfile_delete ()
#endif				/* __STDC__ */
{
   if (HTLib_isInitialized ())
     {

	/* Clean up the persistent cache (if any) */
	HTCacheTerminate ();

	/* Clean up all the global preferences */
	HTFormat_deleteAll ();

	HTLibTerminate ();
	/* Terminate libwww */
     }
}

/*      REGISTER CALLBACKS FOR THE ALERT MANAGER
   **      We register a set of alert messages
 */

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         AHTAlertInit (void)
#else
static void         AHTAlertInit ()
#endif
{

   HTAlert_add (AHTProgress, HT_A_PROGRESS);
   HTAlert_add ((HTAlertCallback *) Add_NewSocket_to_Loop, HT_PROG_CONNECT);
   HTAlert_add (AHTError_print, HT_A_MESSAGE);
   HTError_setShow (0xFF);	/* process all messages */
   /*   HTAlert_add (HTError_print, HT_A_MESSAGE); */
   HTAlert_add (AHTConfirm, HT_A_CONFIRM);
   HTAlert_add (AHTPrompt, HT_A_PROMPT);
   HTAlert_add (AHTPromptPassword, HT_A_SECRET);
   HTAlert_add (AHTPromptUsernameAndPassword, HT_A_USER_PW);
   /* Setup a global call to input XtEvents for new sockets */
   /* should not be necessary anymore */

}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                QueryInit ()
#else
void                QueryInit ()
#endif
{

   AHTProfile_newAmaya (HTAppName, HTAppVersion);

   /* New AHTBridge stuff */

   HTEvent_setRegisterCallback (AHTEvent_register);
   HTEvent_setUnregisterCallback (AHTEvent_unregister);

   /* Setup authentication manager */
    /***
      HTAuthCall_add("basic", HTBasic_parse, HTBasic_generate, HTBasic_delete);
      ****/

   /* Trace activation (for debugging) */
   /*
      WWW_TraceFlag = SHOW_APP_TRACE | SHOW_UTIL_TRACE |
      SHOW_BIND_TRACE | SHOW_THREAD_TRACE |
      SHOW_STREAM_TRACE | SHOW_PROTOCOL_TRACE |
      SHOW_URI_TRACE | SHOW_AUTH_TRACE | SHOW_ANCHOR_TRACE |
      SHOW_CORE_TRACE;

    */

   /***
     WWW_TraceFlag = SHOW_CORE_TRACE | SHOW_AUTH_TRACE | SHOW_ANCHOR_TRACE |
     SHOW_PROTOCOL_TRACE| SHOW_APP_TRACE | SHOW_UTIL_TRACE;
     ***/

   HTBind_caseSensitive (FALSE);
   HTBind_addType ("html", "text/html", 0.9);
   HTBind_addType ("htm", "text/html", 0.9);
   HTBind_addType ("gif", "image/gif", 0.9);
   HTBind_addType ("png", "image/png", 0.9);
   HTBind_addType ("jpg", "image/jpeg", 0.9);
   HTBind_addType ("txt", "text/plain", 0.9);

   /* Setting up other user interfaces */
   /* needs a little bit more work */

   /* Setting up handlers */
#ifndef HACK_WWW
/*    HTNetCall_addBefore(HTLoadStart, NULL, 0); */
#endif

   /* Setting up different network parameters */
   HTNet_setMaxSocket (8);
   HTDNS_setTimeout (3600);
   HTCacheMode_setEnabled (0);

   /* Initialization of the global context */
   Amaya = (AmayaContext *) TtaGetMemory (sizeof (AmayaContext));
   Amaya->reqlist = HTList_new ();
   Amaya->docid_status = HTList_new ();
   Amaya->open_requests = 0;

#ifdef CATCH_SIG
   SetSignal ();
#endif


}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void                QueryClose ()
{
/** HTConversion_deleteAll(converters); **/
   /* Later, the following call should use all the active docids. For the mome
      nt, it stops everything */
   /* StopRequest (); */
   Thread_deleteAll ();

#ifndef HACK_WWW
/**  HTAuthInfo_deleteAll (); **/
#endif
   HTProxy_deleteAll ();
   HTNoProxy_deleteAll ();
   HTGateway_deleteAll ();
   AHTProfile_delete ();
}


/*----------------------------------------------------------------------
   GetObjectWWW loads the file designated by urlName in a temporary   
   file.                                                 
   
   4  file retrieval modes are proposed:                              
   AMAYA_SYNC : blocking mode                            
   AMAYA_ISYNC : incremental, blocking mode              
   AMAYA_ASYNC : non-blocking mode                       
   AMAYA_IASYNC : incremental, non-blocking mode         
   
   In the incremental mode, each time a package arrives, it will be   
   stored in the temporary file. In addition, if an                   
   incremental_callback function is defined, this function will be    
   called and handled a copy of the newly received data package.      
   Finally, if a terminate_callback function is defined, it will be   
   invoked when the request terminates. The caller of this function
   can define two different contexts to be passed to the callback
   functions.

   When the function is called with the SYNC mode, the function will
   return only when the requested file has been loaded.
   The ASYNC mode will immediately return after setting up the
   call.

   Notes:
   At the end of a succesful request, the urlName string contains the
   name of the actually retrieved URL. As a URL can change over the time,
   (e.g., be redirected elsewhere), it is advised that the function
   caller verifies the value of the urlName variable at the end of
   a request.

   Inputs:
   - docid  Document identifier for the set of objects being
   retrieved.
   - urlName The URL to be retrieved (MAX_URL_LENGTH chars length)
   - outputfile A pointer to an empty string of MAX_URL_LENGTH.
   - mode The retrieval mode.
   - incremental_cbf 
   - context_icbf
   Callback and context for the incremental modes
   - terminate_cbf 
   - context_icbf
   Callback and context for a terminate handler

   Outputs:
   - urlName The URL that was retrieved
   - outputfile The name of the temporary file which holds the
   retrieved data. (Only in case of success)
   Returns:
   HT_ERROR
   HT_OK
 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 GetObjectWWW (int docid, char *urlName, char *postString,
				  char *outputfile, int mode,
				TIcbf * incremental_cbf, void *context_icbf,
		 TTcbf * terminate_cbf, void *context_tcbf, BOOL error_html)
#else
int                 GetObjectWWW (docid, urlName, postString, outputfile, mode,
		 incremental_cbf, context_icbf, terminate_cbf, context_tcbf,
				  error_html)
int                 docid;
char               *urlName;
char               *postString;
char               *outputfile;
int                 mode;
TIcbf              *incremental_cbf;
void               *context_icbf;
TTcbf              *terminate_cbf;
void               *context_tcbf;
BOOL                error_html;

#endif
{
   AHTReqContext      *me;

   FILE               *tmp_fp;
   char               *tmp_dir;
   char               *ref;
   int                 status;

   static int          object_counter = 0;	/* loaded objects counter */

   HTList             *cur, *pending;

#if 0
   char               *test;

/*** test URL*/
   test = AHTMakeRelativeName ("http://www.w3.org/", "http://www.w3.org/pub/Amaya");
   HT_FREE (test);
   test = AHTMakeRelativeName ("http://www.w3.org", "http://www.w3.org/pub/Amaya");
   HT_FREE (test);
   test = AHTMakeRelativeName ("http://www.w3.org/pub/Amaya", "http://www.w3.org");
   HT_FREE (test);
#endif

   if (urlName == NULL || docid == 0 || outputfile == NULL)
     {

	/* no file to be loaded */
	TtaSetStatus (docid, 1, TtaGetMessage (AMAYA, AM_BAD_URL),
		      urlName);

	if (error_html)
	   FilesLoading[docid] = 2;	/* so we can show the error message */
	return HT_ERROR;

     }
   /* do we support this protocol? */
   if (IsValidProtocol (urlName) == NO)
     {
	/* return error */
	outputfile[0] = EOS;	/* file could not be opened */
	TtaSetStatus (docid, 1, TtaGetMessage (AMAYA, AM_GET_UNSUPPORTED_PROTOCOL),
		      urlName);

	if (error_html)
	   FilesLoading[docid] = 2;	/* so we can show the error message */
	return HT_ERROR;
     }

   /* verify if a docid directory exists */

   tmp_dir = TtaGetMemory (strlen (TempFileDirectory) + 5 + 1);
   sprintf (tmp_dir, "%s/%d", TempFileDirectory, docid);

   tmp_fp = fopen (tmp_dir, "r");
   if (tmp_fp == 0)
     {
	/*directory did not exist */
	if (mkdir (tmp_dir, S_IRWXU) == -1)
	  {
	     /*error */
	     outputfile[0] = EOS;
	     TtaSetStatus (docid, 1, TtaGetMessage (AMAYA, AM_CACHE_ERROR),
			   urlName);

	     if (error_html)
		FilesLoading[docid] = 2;	/* so we can show the error message */

	     return HT_ERROR;
	  }
     }
   else
      fclose (tmp_fp);

   /*create a tempfilename */

   sprintf (outputfile, "%s/%04dAM", tmp_dir, object_counter);

   TtaFreeMemory (tmp_dir);

   /* update the object_counter */
   object_counter++;

   /* normalize the URL */
   ref = HTParse (urlName, "", PARSE_ALL);

   /* should we abort the request if we could not normalize the url? */

   if (ref == (char *) NULL || ref[0] == EOS)
     {
	/*error */
	outputfile[0] = EOS;
	TtaSetStatus (docid, 1, TtaGetMessage (AMAYA, AM_BAD_URL),
		      urlName);

	if (error_html)
	   FilesLoading[docid] = 2;	/* so we can show the error message */

	return HT_ERROR;
     }
   /* verify if that file name existed */
   if (ThotFile_exist (outputfile))
     {
	RemoveFile (outputfile);
     }

   /* try to open the outputfile */

   if ((tmp_fp = fopen (outputfile, "w")) == NULL)
     {
	outputfile[0] = EOS;	/* file could not be opened */
	TtaSetStatus (docid, 1, TtaGetMessage (AMAYA, AM_CANNOT_CREATE_FILE),
		      outputfile);
	HT_FREE (ref);

	if (error_html)
	   FilesLoading[docid] = 2;	/* so we can show the error message */
	return (HT_ERROR);
     }

   /* the terminate_handler closes the above open fp */
   /* Not anymore, we do that in the AHTCallback_bridge, as all
      requests are now asynchronous */

   /* Initialize the request structure */

   me = AHTReqContext_new (docid);

   if (me == NULL)
     {
	fclose (tmp_fp);
	outputfile[0] = EOS;
	/* need an error message here */
	HT_FREE (ref);
	return (HT_ERROR);
     }


   /* Specific initializations for POST and GET */
   if (mode & AMAYA_FORM_POST)
     {
	me->method = METHOD_POST;
	me->mem_ptr = postString;
	me->block_size = strlen (postString);
	HTRequest_setMethod (me->request, METHOD_POST);
	HTRequest_setPostCallback (me->request, AHTUpload_callback);
     }
   else
     {
	me->method = METHOD_GET;
	me->dest = (HTParentAnchor *) NULL;	/*useful only for PUT and POST methods */
     }

   /* Common initialization */

   me->mode = mode;
   me->error_html = error_html;
   me->incremental_cbf = incremental_cbf;
   me->context_icbf = context_icbf;
   me->terminate_cbf = terminate_cbf;
   me->context_tcbf = context_tcbf;
   me->output = tmp_fp;

   HTRequest_setOutputStream (me->request,
			      AHTFWriter_new (me->request, me->output, YES));


   /*for the async. request modes, we need to have our
      own copy of outputfile and urlname
    */

   if ((mode & AMAYA_ASYNC) || (mode & AMAYA_IASYNC))
     {
	char               *tmp;

	tmp = TtaGetMemory (strlen (outputfile) + 1);
	strcpy (tmp, outputfile);
	me->outputfile = tmp;

	tmp = TtaGetMemory (MAX_LENGTH);
	strcpy (tmp, urlName);
	me->urlName = tmp;

     }
   else
     {
	me->outputfile = outputfile;
	me->urlName = urlName;
     }

/***
Change for taking into account the stop button:
The requests will be always asynchronous, however, if mode=AMAYA_SYNC,
we will loop until the document has been received or a stop signal
generated
****/

   HTRequest_setPreemptive (me->request, NO);
   TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_FETCHING),
		 me->urlName);

   me->anchor = (HTParentAnchor *) HTAnchor_findAddress (ref);

   HT_FREE (ref);

   if (mode & AMAYA_FORM_POST)
     {
	HTAnchor_setFormat ((HTParentAnchor *) me->anchor, HTAtom_for ("application/x-www-form-urlencoded"));
	HTAnchor_setLength ((HTParentAnchor *) me->anchor, me->block_size);
	HTRequest_setEntityAnchor (me->request, me->anchor);

	status = HTLoadAbsolute (urlName, me->request);
     }
   else
      status = HTLoadAnchor ((HTAnchor *) me->anchor,
			     me->request);

   if (status == HT_ERROR || me->reqStatus == HT_END
       || me->reqStatus == HT_ERR)
     {

	/* in case of error, free all allocated memory and exit */

	if ((mode & AMAYA_ASYNC) || (mode & AMAYA_IASYNC))
	  {
	     TtaFreeMemory (me->outputfile);
	     TtaFreeMemory (me->urlName);
	  }
	TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_CANNOT_LOAD),
		      me->urlName);

	status = (me->reqStatus == HT_END) ? HT_OK : HT_ERROR;

	AHTReqContext_delete (me);

     }
   else
     {

	/* part of the stop button handler */

	if ((mode & AMAYA_SYNC) || (mode & AMAYA_ISYNC))
	  {
	     status = LoopForStop (me);
	     AHTReqContext_delete (me);
	  }
	else
	  {

	     /* ASYNC MODE */
	     /* if the request went to the pending events queue, then we close
	        ** the file. It'll be open by AddEventLoop upon liberation of the
	        ** queue
	      */

	     /* verify if this request went to the pending request queue */

	     if ((cur = (HTList *) me->request->net->host->pending))
		while ((pending = HTList_nextObject (cur)))
		  {
		     if (me->request->net == (HTNet *) pending)
		       {
			  /* To correct: see if we can fine tune this request */
			  if (me->reqStatus == HT_WAITING)
			     break;
			  me->reqStatus = HT_NEW_PENDING;
			  /* the request went to the pending queue, so we close the fd to
			     **avoid having  many of them open without being used
			   */
			  if (THD_TRACE)
			     fprintf (stderr, "GetObjectWWW: %s is pending. Closing fd %d\n", me->urlName, (int) me->output);
			  fclose (me->output);
			  me->output = NULL;
			  break;
		       }
		  }

	  }
     }
   TtaHandlePendingEvents ();

   return (status);
}

/*----------------------------------------------------------------------
   PutObjectWWW uploads a file into a URL                             
   
   2 upload modes are proposed:                                       
   AMAYA_SYNC : blocking mode                            
   AMAYA_ASYNC : non-blocking mode                       
   

   When the function is called with the SYNC mode, the function will
   return only when the file has been uploaded.
   The ASYNC mode will immediately return after setting up the
   call. Furthermore, at the end of an upload, the ASYNC mode will 
   call back terminate_cbf, handling it the context defined in
   context_tcbf.

   Notes:
   At the end of a succesful request, the urlName string contains the
   name of the actually uploaded URL. As a URL can change over the time,
   (e.g., be redirected elsewhere), it is advised that the function
   caller verifies the value of the urlName variable at the end of
   a request.

   Inputs:
   - docid  Document identifier for the set of objects being
   retrieved.
   - fileName A pointer to the local file to upload
   - urlName The URL to be uploaded (MAX_URL_LENGTH chars length)
   - mode The retrieval mode.

   - terminate_cbf 
   - context_icbf
   Callback and context for a terminate handler

   Outputs:
   - urlName The URL that was uploaded

   Returns:
   HT_ERROR
   HT_OK
 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 PutObjectWWW (int docid, char *fileName, char *urlName, int mode,
				  TTcbf * terminate_cbf, void *context_tcbf)
#else
int                 PutObjectWWW (docid, urlName, fileName, mode,
				  ,terminate_cbf, context_tcbf)
int                 docid;
char               *urlName;
char               *fileName;
int                 mode;
TTcbf              *terminate_cbf;
void               *context_tcbf;

#endif
{
   /*AHTReqContext      *me; */
   int                 status;

#ifdef WWW_XWINDOWS
/*** Temporary patch (I hope)  ****/
   int                 fd;
   struct stat         file_stat;
   char               *mem_ptr;
   unsigned long       block_size;

   if (urlName == NULL || docid == 0 || fileName == NULL ||
       !ThotFile_exist (fileName))
      /* no file to be uploaded */
      return HT_ERROR;

   /* do we support this protocol? */
   if (IsValidProtocol (urlName) == NO)
     {
	/* return error */
	TtaSetStatus (docid, 1, TtaGetMessage (AMAYA, AM_PUT_UNSUPPORTED_PROTOCOL),
		      urlName);
	return HT_ERROR;
     }
   /* read the file into memory */

   if ((fd = open (fileName, O_RDONLY)) == -1)
     {

	/* if we could not open the file, exit */

	/*error msg here */

	return (HT_ERROR);
     }

   fstat (fd, &file_stat);

   if (file_stat.st_size == 0)
     {

	/* file was empty */

	/*errmsg here */

	close (fd);
	return (HT_ERROR);
     }
   block_size = file_stat.st_size;

   if (THD_TRACE)
      fprintf (stderr, "file size == %u\n", (unsigned) block_size);

   mem_ptr = (char *) TtaGetMemory (block_size);

   if (mem_ptr == (char *) NULL)
     {

	/* could not allocate enough memory */

	/*errmsg here */

	close (fd);
	return (HT_ERROR);
     }
   read (fd, mem_ptr, block_size);

   close (fd);

   status = UploadMemWWW (docid, METHOD_PUT, urlName, mem_ptr,
			  block_size, mode, terminate_cbf,
			  context_tcbf, (char *) NULL);

   TtaFreeMemory (mem_ptr);

#endif /*WWW_XWINDOWS */

   return (status);
/*** End of temporary patch */

}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/

#ifdef __STDC__
int                 UploadMemWWW (int docid, HTMethod method,
		     char *urlName, char *mem_ptr, unsigned long block_size,
			int mode, TTcbf * terminate_cbf, void *context_tcbf,
				  char *outputfile)
#else
int                 UploadMemWWW (docid, method, urlName, mem_ptr, block_size, mode,
				  terminate_cbf, context_tcbf, outputfile)
int                 docid;
HTMethod            method;
char               *urlName;
char               *mem_ptr;
usigned long        block_size;
int                 mode;
TTcbf              *terminate_cbf;
void               *context_tcbf;
char               *outputfile;

#endif
{
   AHTReqContext      *me;
   int                 status;

  /****
  int basename_flag;
  ****/

   if (mem_ptr == (char *) NULL ||
       block_size == 0 ||
       docid == 0 ||
       urlName == (char *) NULL)
     {
	/* nothing to be uploaded */
	return HT_ERROR;
     }
   /* if we are doing a post, try to open the destination file */

   /* Initialize the request structure */
   me = AHTReqContext_new (docid);

   if (me == NULL)
     {
	/* need an error message here */
	TtaHandlePendingEvents ();
	return (HT_ERROR);
     }
   me->mode = mode;

   me->incremental_cbf = (TIcbf *) NULL;
   me->context_icbf = (void *) NULL;
   me->terminate_cbf = terminate_cbf;
   me->context_tcbf = context_tcbf;

   me->output = stdout;
   me->outputfile = (char *) NULL;
   me->urlName = urlName;

   HTRequest_setPreemptive (me->request, NO);

   me->method = METHOD_PUT;
   HTRequest_setMethod (me->request, METHOD_PUT);
   me->output = stdout;
   me->outputfile = (char *) NULL;

   me->mem_ptr = mem_ptr;
   me->block_size = block_size;
   HTRequest_setPostCallback (me->request, AHTUpload_callback);

   HTRequest_setOutputStream (me->request,
			      HTFWriter_new (me->request, me->output, YES));

   me->anchor = (HTParentAnchor *) HTAnchor_findAddress (urlName);

   HTAnchor_setFormat ((HTParentAnchor *) me->anchor, HTAtom_for ("text/html"));	/* test */
   HTAnchor_setLength ((HTParentAnchor *) me->anchor, me->block_size);
   HTRequest_setEntityAnchor (me->request, me->anchor);
   /*
      HTRequest_addAfter (me->request, AHTSaveFilter, NULL, NULL, HT_ALL, 
      HT_FILTER_FIRST, NO);
    */
   status = HTLoadAbsolute (urlName, me->request);

   if (status == HT_ERROR || me->reqStatus == HT_END
       || me->reqStatus == HT_ERR || HTError_hasSeverity (HTRequest_error (me->request), ERR_NON_FATAL))
     {
	/* just in case ... :-) <-- because it's the case sometimes */

#ifdef WWW_XWINDOWS
    /***
    if(me->xtinput_id != (XtInputId) NULL) {
      if (THD_TRACE)
	fprintf(stderr, "Query: Removing Xtinput: %lu\n",
		me->xtinput_id);
      XtRemoveInput(me->xtinput_id);
    }
    ***/
#endif /* WWW_XWINDOWS */

	TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_CANNOT_SAVE),
		      me->urlName);

	status = HT_ERROR;
	AHTReqContext_delete (me);

     }
   else
     {

	TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_REMOTE_SAVING),
		      me->urlName);

	/* part of the stop button handler */

	if ((mode & AMAYA_SYNC) || (mode & AMAYA_ISYNC))
	  {
	     status = LoopForStop (me);
	     AHTReqContext_delete (me);
	  }
     }				/* if-else */

   return (status);

}


/*----------------------------------------------------------------------
   Stop Request stops (kills) all active requests associated with a docid 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                StopRequest (int docid)
#else
void                StopRequest (docid)
int                 docid;

#endif
{
   HTList             *cur;
   AHTDocId_Status    *docid_status;
   AHTReqContext      *me;
   int                 open_requests;

   if (Amaya)
     {

	cur = Amaya->reqlist;
	docid_status = (AHTDocId_Status *) GetDocIdStatus (docid,
						       Amaya->docid_status);

	/* verify if there are any requests at all associated with docid */

	if (docid_status == (AHTDocId_Status *) NULL)
	   return;

	open_requests = docid_status->counter;

	while ((me = (AHTReqContext *) HTList_nextObject (cur)))
	  {

	     if (me->docid == docid)
	       {

		  /* kill this request */

		  switch (me->reqStatus)
			{

			   case HT_ABORT:
			      break;

			   case HT_BUSY:
			      me->reqStatus = HT_ABORT;
			      break;
			   case HT_NEW_PENDING:
			   case HT_WAITING:
			   default:
#ifdef WWW_XWINDOWS
			      RequestKillAllXtevents (me);
#endif
			      me->reqStatus = HT_ABORT;
	  /***
	    HTNet_kill(me->request);
	    ***/

			      HTRequest_kill (me->request);

			      if (me->mode == AMAYA_ASYNC ||
				  me->mode == AMAYA_IASYNC)
				{

				   AHTReqContext_delete (me);
				}
			      cur = Amaya->reqlist;

			      open_requests--;

			      break;

			}	/* switch */
	       }		/* if me docid */
	  }			/* while */
     }				/* if amaya open requests */
}				/* StopRequest */


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static int          LoopForStop (AHTReqContext * me)
#else
static int          LoopForStop (AHTReqContext * me)
#endif
{

#ifdef WWW_XWINDOWS
   extern XtAppContext app_cont;
   XEvent              ev;
   XtInputMask         status;

#endif /* WWW_XWINDOWS */
   int                 status_req;

   /* to test the async calls  */
   /* Boucle d'attente des evenements */

   while (me->reqStatus != HT_ABORT &&
	  me->reqStatus != HT_END &&
	  me->reqStatus != HT_ERR)
     {

#ifdef WWW_XWINDOWS
	status = XtAppPending (app_cont);
	if (status & XtIMXEvent)
	  {
	     XtAppNextEvent (app_cont, &ev);
	     TtaHandleOneEvent (&ev);
	  }
	else if (status & (XtIMAll & (~XtIMXEvent)))
	  {
	     XtAppProcessEvent (app_cont,
				(XtIMAll & (~XtIMXEvent)));
	  }
	else
	  {
	     XtAppNextEvent (app_cont, &ev);
	     TtaHandleOneEvent (&ev);
	  }

#endif /* WWW_XWINDOWS */
     }				/* while */

   switch (me->reqStatus)
	 {

	    case HT_ERR:
	    case HT_ABORT:
	       status_req = HT_ERROR;
	       break;

	    case HT_END:
	       status_req = HT_OK;
	       break;

	    default:
	       break;
	 }

   return (status_req);
}
