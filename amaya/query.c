/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * query.c : contains all the functions for requesting iand publishing
 * URLs via libwww. It handles any eventual HTTP error code
 * (redirection, authentication needed, not found, etc.)
 *
 * Author: J. Kahan
 *         R. Guetari (W3C/INRIA) Windows 95/NT routines
 */

#ifndef AMAYA_JAVA

/* Amaya includes  */
#define THOT_EXPORT extern
#include "amaya.h"

#if defined(__svr4__)
#define CATCH_SIG
#endif

/*----------------------------------------------------------------------*/
/* Experimental */

#define AMAYA_LAST_HTTP_ERROR_MSG_SIZE 4096
char AmayaLastHTTPErrorMsg [AMAYA_LAST_HTTP_ERROR_MSG_SIZE];

/*----------------------------------------------------------------------*/

/* local structures coming from libwww and which are
   not found in any .h file
 */

struct _HTStream
  {
     const HTStreamClass *isa;
     FILE               *fp;
     BOOL                leave_open;	/* Close file when TtaFreeMemory? */
     char               *end_command;	/* Command to execute       */
     BOOL                remove_on_close;	/* Remove file?             */
     char               *filename;	/* Name of file             */
     HTRequest          *request;	/* saved for callback       */
     HTRequestCallback  *callback;
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

/*----------------------------------------------------------------------*/

/*** private variables ***/

static HTList      *converters = NULL;	/* List of global converters */
static HTList      *acceptTypes = NULL; /* List of types for the Accept header */
static HTList      *encodings = NULL;
static int          object_counter = 0;	/* loaded objects counter */
static  boolean    AmayaIsAlive;

#include "answer_f.h"
#include "query_f.h"
#include "AHTURLTools_f.h"
#include "AHTBridge_f.h"
#include "AHTMemConv_f.h"
#include "AHTFWrite_f.h"


/*----------------------------------------------------------------------
  GetDocIdStatus
  gets the status associated to a docid                         
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
	  }
     }
   return (AHTDocId_Status *) NULL;

}

/*----------------------------------------------------------------------
  AHTGuessAtom_for
  Converts an Amaya type descriptor into the equivalent MIME type.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static  HTAtom *AHTGuessAtom_for (char *urlName, PicType contentType)
#else
static  HTAtom *AHTGuessAtom_for (urlName, contentType)
char *urlName;
PicType contentType;
#endif
{
 HTAtom           *atom;
 char             *filename;
 HTEncoding        enc;
 HTEncoding        cte;
 HTLanguage        lang;
 double            quality = 1.0;

 switch (contentType)
   {
    case xbm_type:
      atom = HTAtom_for("image/xbm");
      break;
    case eps_type:
      atom = HTAtom_for("application/postscript");
      break;
   case xpm_type:
      atom = HTAtom_for("image/xpm");
     break;
    case gif_type:
      atom = HTAtom_for("image/gif");
      break;
    case jpeg_type:
      atom = HTAtom_for("image/jpeg");
      break;
    case png_type:
      atom = HTAtom_for("image/png");
      break;
   case unknown_type:
   default:
     /* 
     ** Amaya could not detect the type, so 
     ** we try to use the filename's suffix to do so.
     */
     filename = AmayaParseUrl (urlName, "", AMAYA_PARSE_PATH | AMAYA_PARSE_PUNCTUATION);
     HTBind_getFormat (filename, &atom, &enc, &cte, &lang, &quality);
     TtaFreeMemory (filename);
     if (atom ==  WWW_UNKNOWN)
	 /*
	 ** we could not identify the suffix, so we assign it
	 ** a default type
	 */
	 atom = HTAtom_for ("text/html");
     break;
   }

 return atom;
}

/*----------------------------------------------------------------------
  AHTReqContext_new
  create a new Amaya Context Object and update the global Amaya
  request status.
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
   
   /* Initialize the other members of the structure */
   me->reqStatus = HT_NEW; /* initial status of a request */
   me->output = NULL;
#  ifndef _WINDOWS
   me->read_xtinput_id = (XtInputId) NULL;
   me->write_xtinput_id = (XtInputId) NULL;
   me->except_xtinput_id = (XtInputId) NULL;
#  endif
   me->docid = docid;
   HTRequest_setMethod (me->request, METHOD_GET);
   HTRequest_setOutputFormat (me->request, WWW_SOURCE);
   HTRequest_setContext (me->request, me);
   me->read_ops = 0;
   me->write_ops = 0;
   me->except_ops = 0;
   /* experimental */
   me->read_sock = INVSOC;
   me->write_sock = INVSOC;
   me->except_sock = INVSOC;

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
  AHTReqContext_delete
  Delete an Amaya Context Object and update the global Amaya request
  status.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
boolean   AHTReqContext_delete (AHTReqContext * me)
#else
boolean   AHTReqContext_delete (me)
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
	  TtaFreeMemory (me->error_stream);
#ifdef WWW_XWINDOWS	
	if (me->read_xtinput_id || me->write_xtinput_id ||
            me->except_xtinput_id)
          RequestKillAllXtevents(me);
#endif
	TtaFreeMemory ((void *) me);

	Amaya->open_requests--;

	return TRUE;

     }
   return FALSE;
}


/*----------------------------------------------------------------------
  AHTUpload_callback
  callback handler for executing the PUT command
  ----------------------------------------------------------------------*/
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
       return (HT_LOADED);
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
  Thread_deleteAll
  this function deletes the whole list of active threads.           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         Thread_deleteAll (void)
#else
static void         Thread_deleteAll ()
#endif
{
  HTList             *cur;
  AHTReqContext      *me;
  AHTDocId_Status    *docid_status;

   if (Amaya && Amaya->reqlist)
     {
	if (Amaya->open_requests > 0)
	  {
	    cur = Amaya->reqlist;

	     HTNet_killAll ();
	     /* erase the requests */
	     while ((me = (AHTReqContext *) HTList_removeLastObject (cur)))
	       {
		  if (me->request)
		    {
#                      ifndef _WINDOWS
		       RequestKillAllXtevents (me);
#                      endif /* !_WINDOWS */
		       AHTReqContext_delete (me);
		    }
	       }		/* while */

	     /* erase the docid_status entities */
	     while ((docid_status = (AHTDocId_Status *) HTList_removeLastObject ((void *) Amaya->docid_status)))
		TtaFreeMemory ((void *) docid_status);

	  }			/* if */
     }
}

/*----------------------------------------------------------------------
  redirection_handler
  this function is registered to handle permanent and temporary
  redirections.
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
	     ref = AmayaParseUrl (new_anchor->parent->address, me->urlName, AMAYA_PARSE_ALL);
	     if (ref)
	       {
		  TtaFreeMemory (new_anchor->parent->address);
		  new_anchor->parent->address = ref;
	       }
	  }

	/* update the current file name */
	if (strlen (new_anchor->parent->address) > (MAX_LENGTH - 2))
	  {
	     /*
	        ** copy MAX_LENGTH cars. The error will be detected later on and shown on the
	        ** screen. This code will be fixed up later on
	      */
	     strncpy (me->urlName, new_anchor->parent->address, MAX_LENGTH - 1);
	     me->urlName[MAX_LENGTH - 1] = EOS;
	  }
	else
	   strcpy (me->urlName, new_anchor->parent->address);

	ChopURL (me->status_urlName, me->urlName);

	TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_RED_FETCHING),
		      me->status_urlName);

	/* Start request with new credentials */
	me->reqStatus = HT_NEW; /* reset the status */
	if (me->method == METHOD_PUT || me->method == METHOD_POST)	/* PUT, POST etc. */
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
	   DocNetworkStatus[me->docid] |= AMAYA_NET_ERROR; /* so we can show the error message */
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
  this function is registered to handle the result of the request
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
   boolean             error_flag;
   char                msg_status[10];
   HTError             *error;
   HTErrorElement      errorElement;
   HTList              *cur;

   if (!me)
      return HT_OK;		/* not an Amaya request */

   if (!AmayaIsAlive)
     me->reqStatus = HT_ABORT;

   if (status == HT_LOADED || status == HT_CREATED || status == HT_NO_DATA)
     error_flag = FALSE;
   else
     error_flag = TRUE;

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
	    if (error_flag)
	      {		/* there were some errors */
		if (me->error_html == TRUE)
		  {		/* and we want to print errors */
		    if (me->error_stream_size == 0)	/* and the stream is empty */
			  AHTError_MemPrint (request);	/* copy errors from the error stack 
							   ** into a data structure */
		       if (me->error_stream)
			 {	/* if the stream is non-empty */
			    fprintf (me->output, me->error_stream);	/* output the errors */
			    error_flag = FALSE;		/* show it in the HTML window */
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
	  }			/* if != HT_ABORT */
	fclose (me->output);
     }
   else
     {
	/* We must be doing a PUT. Verify if there was an error */
	if (error_flag)
	  me->reqStatus = HT_ERR;
     }				/* if me-output */

   /* Second Step: choose a correct treatment in function of the request's
      being associated with an error, with an interruption, or with a
      succesful completion */

   if (!error_flag  && me->reqStatus != HT_ERR
       && me->reqStatus != HT_ABORT)
     {
	me->reqStatus = HT_END;	/* no errors */
	if (me->terminate_cbf)
	   (*me->terminate_cbf) ((AHTReqContext *) me,
				 HT_LOADED);
     }
   else if (me->reqStatus == HT_ABORT)
     /* either the application ended or the user pressed the stop 
	button. We erase the incoming file, if it exists */
     {
	if (me->outputfile && me->outputfile[0] != EOS)
	  {
	     TtaFileUnlink (me->outputfile);
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
	     TtaFileUnlink (me->outputfile);
	     me->outputfile[0] = EOS;
	  }
     }
   else if (error_flag && 
	    (me->reqStatus == HT_BUSY || me->reqStatus == HT_WAITING))
     {
	/* there was an error */
	if (me->terminate_cbf)
	   (*me->terminate_cbf) ((AHTReqContext *) me,
				 HT_ERROR);

	if (me->outputfile && me->outputfile[0] != EOS)
	  {
	     TtaFileUnlink (me->outputfile);
	     me->outputfile[0] = EOS;
	     me->reqStatus = HT_ERR;
	  }
     }				/* if-else HT_END, HT_ABORT, HT_ERROR */

   if ((me->mode & AMAYA_ASYNC) || (me->mode & AMAYA_IASYNC))
     /* for the ASYNC mode, free the memory we allocated in GetObjectWWW
	or in PutObjectWWW */
     {
	TtaFreeMemory (me->urlName);
	me->urlName = NULL;
	TtaFreeMemory (me->outputfile);
	me->outputfile = NULL;
     }

   /* don't remove or Xt will hang up during the PUT */

   if (AmayaIsAlive  && ((me->method == METHOD_POST) ||
			 (me->method == METHOD_PUT)))
     {
       /* output the status of the request */
       if (status == 200)
	 TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_REQUEST_SUCCEEDED), me->status_urlName);
       else if (status == 201)
	 TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_CREATED_NEW_REMOTE_RESSOURCE), me->status_urlName);
       else if (status == 204 && me->method == METHOD_PUT)
	 TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_UPDATED_REMOTE_RESSOURCE), me->status_urlName);
       else if (status == 204 && me->method == METHOD_PUT)
	 TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_NO_DATA), (char *) NULL);
       else if (status == -400 || status == 505)
	 {
	 TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_SERVER_DID_NOT_UNDERSTAND_REQ_SYNTAX), (char *) NULL);
	   sprintf (AmayaLastHTTPErrorMsg, TtaGetMessage (AMAYA, AM_SERVER_DID_NOT_UNDERSTAND_REQ_SYNTAX));
	 }
       else if (status == -401) 
	 {
	   TtaSetStatus (me->docid, 1,
			 TtaGetMessage (AMAYA, AM_AUTHENTICATION_FAILURE), me->status_urlName);
	   sprintf (AmayaLastHTTPErrorMsg, TtaGetMessage (AMAYA, AM_AUTHENTICATION_FAILURE), me->status_urlName);
	 }
       else if (status == -403)
	 {
	   TtaSetStatus (me->docid, 1,
			 TtaGetMessage (AMAYA, AM_FORBIDDEN_ACCESS), me->status_urlName);
	   sprintf (AmayaLastHTTPErrorMsg, TtaGetMessage (AMAYA, AM_FORBIDDEN_ACCESS), me->urlName);
	 }
       else if (status == -405)
	 {
	 TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_METHOD_NOT_ALLOWED), (char *) NULL);
	 sprintf(AmayaLastHTTPErrorMsg, TtaGetMessage (AMAYA, AM_METHOD_NOT_ALLOWED));
	 }
       else if (status == -1)
	 {
	   /*
	   ** Here we deal with errors for which libwww does not
	   ** return a correct status code 
	   */
	   cur = HTRequest_error (request);
	   error = (HTError *) HTList_nextObject (cur);
	   if (error == (HTError *) NULL)
	     /* there's no error context */
	     {
	       sprintf (msg_status, "%d", status); 
	       TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_UNKNOWN_XXX_STATUS), msg_status);
	       sprintf (AmayaLastHTTPErrorMsg, TtaGetMessage (AMAYA, AM_UNKNOWN_XXX_STATUS), msg_status);
	       return (HT_OK);
	     }
	     errorElement = error->element;

	   if (errorElement == HTERR_NOT_IMPLEMENTED)
	     {
	       TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_SERVER_NOT_IMPLEMENTED_501_ERROR), (char *) NULL);
	       sprintf (AmayaLastHTTPErrorMsg, TtaGetMessage (AMAYA, AM_SERVER_NOT_IMPLEMENTED_501_ERROR));
	       status = -501;
	     }
	   else if (errorElement == HTERR_INTERNAL)
	     {
	       if ((error->length > 0) && (error->length <= 25) &&
		   (error->par) && (((char *) error->par)[0] != EOS)) 
		 {
		 TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_SERVER_INTERNAL_ERROR_500_CAUSE), (char *) (error->par));
		 sprintf (AmayaLastHTTPErrorMsg, TtaGetMessage (AMAYA, AM_SERVER_INTERNAL_ERROR_500_CAUSE), (char *) (error->par));
		 }
	       else
		 {
		 TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_SERVER_INTERNAL_ERROR_500_NO_CAUSE), (char *) NULL);
		 sprintf (AmayaLastHTTPErrorMsg, TtaGetMessage (AMAYA, AM_SERVER_INTERNAL_ERROR_500_NO_CAUSE));
		 }
	       status = -500; 
	     }
	   else
	     {
	       sprintf (msg_status, "%d", status); 
	       TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_UNKNOWN_XXX_STATUS), msg_status);
		 sprintf (AmayaLastHTTPErrorMsg, TtaGetMessage (AMAYA, AM_UNKNOWN_XXX_STATUS), msg_status);
	     }
	 }
     }
   return HT_OK;
}

/*----------------------------------------------------------------------
  AHTLoadTerminate_handler
  this is an application "AFTER" Callback. It uses all the functionaly
  that the app part of the Library gives for handling AFTER a request. 
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
   AHTReqContext      *me = HTRequest_context (request);
   HTAlertCallback    *cbf;
   AHTDocId_Status    *docid_status;

   switch (status)
	 {
	    case HT_LOADED:
	       if (PROT_TRACE)
		  HTTrace ("Load End.... OK: `%s\' has been accessed\n",
			   me->status_urlName);

	       docid_status = GetDocIdStatus (me->docid,
					      Amaya->docid_status);

	       if (docid_status != NULL && docid_status->counter > 1)
		  TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA,
						   AM_ELEMENT_LOADED), me->status_urlName);

	       break;

	    case HT_NO_DATA:
	       if (PROT_TRACE)
		  HTTrace ("Load End.... OK BUT NO DATA: `%s\'\n", me->status_urlName);
	       TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_LOADED_NO_DATA),
			     me->status_urlName);
	       break;

	    case HT_INTERRUPTED:
	       if (PROT_TRACE)
		  HTTrace ("Load End.... INTERRUPTED: `%s\'\n", me->status_urlName);
	       TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_LOAD_ABORT), NULL);

	       break;

	    case HT_RETRY:
	       if (PROT_TRACE)
		  HTTrace ("Load End.... NOT AVAILABLE, RETRY AT %ld\n",
			   HTResponse_retryTime (response));
	       TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_NOT_AVAILABLE_RETRY),
			     me->status_urlName);
	       break;

	    case HT_ERROR:

	       cbf = HTAlert_find (HT_A_MESSAGE);
	       if (cbf)
		  (*cbf) (request, HT_A_MESSAGE, HT_MSG_NULL, NULL,
			  HTRequest_error (request), NULL);
	       break;

	       if (PROT_TRACE)
		  HTTrace ("Load End.... ERROR: Can't access `%s\'\n",
			   me->status_urlName ? me->status_urlName : "<UNKNOWN>");
	       TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_CANNOT_LOAD),
			     me->status_urlName ? me->status_urlName : "<UNKNOWN>");
	       break;
	    default:
	       if (PROT_TRACE)
		  HTTrace ("Load End.... UNKNOWN RETURN CODE %d\n", status);
	       break;
	 }

   return HT_OK;
}

/*----------------------------------------------------------------------
  AHTAcceptTypesInit
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void           AHTAcceptTypesInit (HTList *c)
#else  /* __STDC__ */
static void           AHTAcceptTypesInit (c)
HTList             *c;
#endif /* __STDC__ */
{
  if (c == (HTList *) NULL) 
      return;

      /* define here all the mime types that Amaya can accept */

      HTConversion_add (c, "image/png",  "www/present", HTThroughLine, 1.0, 0.0, 0.0);
      HTConversion_add (c, "image/jpeg", "www/present", HTThroughLine, 1.0, 0.0, 0.0);
      HTConversion_add (c, "image/gif",  "www/present", HTThroughLine, 1.0, 0.0, 0.0);
      HTConversion_add (c, "image/xbm",  "www/present", HTThroughLine, 1.0, 0.0, 0.0);
      HTConversion_add (c, "image/xpm",  "www/present", HTThroughLine, 1.0, 0.0, 0.0);
      HTConversion_add (c, "application/postscript",  "www/present", HTThroughLine, 1.0, 0.0, 0.0);

   /* Define here the equivalences between MIME types and file extensions for
    the types that Amaya can display */

   /* Register the default set of file suffix bindings */
   HTFileInit ();

   /* Don't do any case distinction */
   HTBind_caseSensitive (FALSE);
}

/*----------------------------------------------------------------------
  AHTConverterInit
  Bindings between a source media type and a destination media type
  (conversion).
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         AHTConverterInit (HTList *c)
#else  /* __STDC__ */
static void         AHTConverterInit (c)
HTList             *c;
#endif /* __STDC__ */
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

}


/*----------------------------------------------------------------------
  AHTProtocolInit
  Registers all amaya supported protocols.
  ----------------------------------------------------------------------*/
static void         AHTProtocolInit (void)
{

   /* 
      NB. Preemptive == YES = Blocking request
      Non-preemptive == NO = Non-blocking request
   */

   HTProtocol_add ("http", "buffered_tcp", NO, HTLoadHTTP, NULL);
   /*   HTProtocol_add ("http", "tcp", NO, HTLoadHTTP, NULL); */
   HTProtocol_add ("file", "local", NO, HTLoadFile, NULL);
   HTProtocol_add ("cache", "local", NO, HTLoadCache, NULL);
   HTProtocol_add ("ftp", "tcp", NO, HTLoadFTP, NULL);
#if 0 /* experimental code */
   HTProtocol_add ("telnet", "", YES, HTLoadTelnet, NULL);
   HTProtocol_add ("tn3270", "", YES, HTLoadTelnet, NULL);
   HTProtocol_add ("rlogin", "", YES, HTLoadTelnet, NULL);
   HTProtocol_add ("nntp", "tcp", NO, HTLoadNews, NULL);
   HTProtocol_add ("news", "tcp", NO, HTLoadNews, NULL);
#endif
}

/*----------------------------------------------------------------------
  AHTNetInit
  Reegisters "before" and "after" request filters.
  ----------------------------------------------------------------------*/
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
   HTNet_addAfter (HTAuthFilter, "http://*", NULL, HT_NO_ACCESS, 5);
   HTNet_addAfter (redirection_handler, "http://*", NULL, HT_TEMP_REDIRECT, 5);
   HTNet_addAfter (redirection_handler, "http://*", NULL, HT_PERM_REDIRECT, 5);
   HTNet_addAfter (HTUseProxyFilter, "http://*", NULL, HT_USE_PROXY, 5);
   HTNet_addAfter (AHTLoadTerminate_handler, NULL, NULL, HT_ALL, HT_FILTER_LAST);	
   /* handles all errors */
   HTNet_addAfter (terminate_handler, NULL, NULL, HT_ALL, HT_FILTER_LAST);
#endif
}

/*----------------------------------------------------------------------
  AHTAlertInit
  Register alert messages and their callbacks.
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
   HTError_setShow (~((unsigned int) 0 ) & ~((unsigned int) HT_ERR_SHOW_DEBUG));	/* process all messages except debug ones*/
   HTAlert_add (AHTConfirm, HT_A_CONFIRM);
   HTAlert_add (AHTPrompt, HT_A_PROMPT);
   HTAlert_add (AHTPromptPassword, HT_A_SECRET);
   HTAlert_add (AHTPromptUsernameAndPassword, HT_A_USER_PW);
}

/*----------------------------------------------------------------------
  AHTProfile_newAmaya
  creates the Amaya client profile for libwww.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         AHTProfile_newAmaya (char *AppName, char *AppVersion)
#else  /* __STDC__ */
static void         AHTProfile_newAmaya (AppName, AppVersion)
char               *AppName;
char               *AppVersion;
#endif /* __STDC__ */
{
   /* If the Library is not already initialized then do it */
   if (!HTLib_isInitialized ())
      HTLibInit (AppName, AppVersion);

   if (!converters)
      converters = HTList_new ();
   if (!acceptTypes)
      acceptTypes = HTList_new ();
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
   AHTAcceptTypesInit (acceptTypes);
   HTFormat_setConversion (converters);

   /* Register the default set of transfer encoders and decoders */
   HTEncoderInit (encodings);	/* chunks ??? */
   HTFormat_setTransferCoding (encodings);

   /* Register the default set of MIME header parsers */
   HTMIMEInit ();		/* must be called again for language selector */

   /* Register the default set of Icons for directory listings */
   /*HTIconInit(NULL); *//* experimental */

   /* Register the default set of messages and dialog functions */
   AHTAlertInit ();
   HTAlert_setInteractive (YES);
}

/*----------------------------------------------------------------------
  AHTProfile_delete
  deletes the Amaya client profile.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         AHTProfile_delete (void)
#else
static void         AHTProfile_delete ()
#endif				/* __STDC__ */
{
 
  /* free the Amaya global context */
   if (!converters)
      HTConversion_deleteAll (converters);
   if (!acceptTypes)
      HTConversion_deleteAll (acceptTypes);
   if (!encodings)
      HTCoding_deleteAll (encodings);

   HTList_delete (Amaya->docid_status);
   HTList_delete (Amaya->reqlist);
   TtaFreeMemory (Amaya);
  
  if (HTLib_isInitialized ())
    {
      
      /* Clean up the persistent cache (if any) */
      HTCacheTerminate ();

      /* Clean up all the global preferences */
      HTFormat_deleteAll ();

      /* Terminate libwww */
      HTLibTerminate ();
    }
}

/*----------------------------------------------------------------------
  QueryInit
  initializes the libwww interface 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                QueryInit ()
#else
void                QueryInit ()
#endif
{

   AmayaIsAlive = TRUE;
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

   /* Setting up other user interfaces */
   /* needs a little bit more work */

   /* Setting up handlers */
#ifndef HACK_WWW
/*    HTNetCall_addBefore(HTLoadStart, NULL, 0); */
#endif

   /* Setting up different network parameters */
   /* Maximum number of simultaneous open sockets */
   HTNet_setMaxSocket (8);
   HTDNS_setTimeout (3600);
   /* Cache is disabled in this version */
   HTCacheMode_setEnabled (0);

   /* Initialization of the global context */
   Amaya = (AmayaContext *) TtaGetMemory (sizeof (AmayaContext));
   Amaya->reqlist = HTList_new ();
   Amaya->docid_status = HTList_new ();
   Amaya->open_requests = 0;

#ifdef CATCH_SIG
   signal (SIGPIPE, SIG_IGN);
#endif
}


/*----------------------------------------------------------------------
  LoopForStop
  a copy of the Thop event loop so we can handle the stop button.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          LoopForStop (AHTReqContext * me)
#else
static int          LoopForStop (AHTReqContext * me)
#endif
{

#  ifndef _WINDOWS
   extern ThotAppContext app_cont;
   XEvent                ev;
   XtInputMask           status;
#  else  /* _WINDOWS */
   MSG ev;
#  endif /* _WINDOWS */
   int                 status_req = HT_OK;

   /* to test the async calls  */
   /* Loop while waiting for new events, exists when the request is over */
   while (me->reqStatus != HT_ABORT &&
	  me->reqStatus != HT_END &&
	  me->reqStatus != HT_ERR)
     {
	if (!AmayaIsAlive)
	  /* Amaya was killed by one of the callback handlers */
	  exit (0);

#       ifndef _WINDOWS
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
#       else  /* _WINDOWS */
	if (GetMessage (&ev, NULL, 0, 0))
	   TtaHandleOneWindowEvent (&ev);
#       endif /* !_WINDOWS */
     }

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


/*----------------------------------------------------------------------
  QueryClose
  closes all existing threads, frees all non-automatically deallocated
  memory and then ends libwww.
  ----------------------------------------------------------------------*/
void                QueryClose ()
{

   AmayaIsAlive = FALSE;

   /* remove all the handlers and callbacks that may output a message to
      a non-existent Amaya window */

   HTNet_deleteAfter (AHTLoadTerminate_handler);
   HTNet_deleteAfter (redirection_handler);
   HTAlertCall_deleteAll (HTAlert_global () );
   HTAlert_setGlobal ((HTList *) NULL);
   HTEvent_setRegisterCallback ((HTEvent_registerCallback *) NULL);
   HTEvent_setUnregisterCallback ((HTEvent_unregisterCallback *) NULL);

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
   GetObjectWWW
   this function requests a resource designated by a URLname into a
   temporary filename. The download can come from a simple GET operation,
   or can come from POSTING/GETTING a form. In the latter
   case, the function receives a query string to send to the server.

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
   caller verify the value of the urlName variable at the end of
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
   -error_html if TRUE, then display any server error message as an
   HTML document.

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
		 TTcbf * terminate_cbf, void *context_tcbf, boolean  error_html)
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
boolean             error_html;
#endif
{
   AHTReqContext      *me;

   FILE               *tmp_fp;
   char               *tmp_dir;
   char               *ref;
   int                 status;
   HTList             *cur, *pending;
#  ifdef _WINDOWS
   DWORD               attribs;
#  endif /* _WINDOWS */

   if (urlName == NULL || docid == 0 || outputfile == NULL)
     {
	/* no file to be loaded */
	TtaSetStatus (docid, 1, TtaGetMessage (AMAYA, AM_BAD_URL),
		      urlName);

	if (error_html)
	  DocNetworkStatus[docid] |= AMAYA_NET_ERROR; /* so we can show the error message */
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
	  DocNetworkStatus[docid] |= AMAYA_NET_ERROR; /* so we can show the error message */
	return HT_ERROR;
     }

   /* verify if a docid directory exists */

   tmp_dir = TtaGetMemory (strlen (TempFileDirectory) + 5 + 1);
#  ifndef _WINDOWS
   sprintf (tmp_dir, "%s/%d", TempFileDirectory, docid);
#  else  /* _WINDOWS */
   sprintf (tmp_dir, "C:\\TEMP\\AMAYA\\%d", docid);
#  endif /* _WINDOWS */

#  ifndef _WINDOWS
   tmp_fp = fopen (tmp_dir, "r");
   if (tmp_fp == 0)
#  else /* _WINDOWS */
   attribs = GetFileAttributes (tmp_dir) ;

   if (attribs == 0xFFFFFFFF)
#  endif /* _WINDOWS */
     {
	/*directory did not exist */
	if (mkdir (tmp_dir, S_IRWXU) == -1) {
	   /*error */
	   outputfile[0] = EOS;
	   TtaSetStatus (docid, 1, TtaGetMessage (AMAYA, AM_CACHE_ERROR), urlName);
	   
	   if (error_html)
	      DocNetworkStatus[docid] |= AMAYA_NET_ERROR; /* so we can show the error message */
	   return HT_ERROR;
	}
     }
#  ifndef _WINDOWS
   else
      fclose (tmp_fp);
#  endif /* !_WINDOWS */

   /*create a tempfilename */

#  ifndef _WINDOWS
   sprintf (outputfile, "%s/%04dAM", tmp_dir, object_counter);
#  else  /* _WINDOWS */
   sprintf (outputfile, "%s\\%04dAM", tmp_dir, object_counter);
#  endif /* _WINDOWS */

   TtaFreeMemory (tmp_dir);

   /* update the object_counter */
   object_counter++;

   /* normalize the URL */
   ref = AmayaParseUrl (urlName, "", AMAYA_PARSE_ALL);

   /* should we abort the request if we could not normalize the url? */

   if (ref == (char*) NULL || ref[0] == EOS)
     {
	/*error */
	outputfile[0] = EOS;
	TtaSetStatus (docid, 1, TtaGetMessage (AMAYA, AM_BAD_URL),
		      urlName);

	if (error_html)
	  DocNetworkStatus[docid] |= AMAYA_NET_ERROR; /* so we can show the error message */

	return HT_ERROR;
     }
   /* verify if that file name existed */
   if (TtaFileExist (outputfile))
     {
	TtaFileUnlink (outputfile);
     }

   /* try to open the outputfile */

   if ((tmp_fp = fopen (outputfile, "w")) == NULL)
     {
	outputfile[0] = EOS;	/* file could not be opened */
	TtaSetStatus (docid, 1, TtaGetMessage (AMAYA, AM_CANNOT_CREATE_FILE),
		      outputfile);
	TtaFreeMemory (ref);

	if (error_html)
	  DocNetworkStatus[docid] |= AMAYA_NET_ERROR; /* so we can show the error message */
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
	TtaFreeMemory (ref);
	return (HT_ERROR);
     }


   /* Specific initializations for POST and GET */
   if (mode & AMAYA_FORM_POST)
     {
	me->method = METHOD_POST;
	if (postString)
	  {
	    me->mem_ptr = postString;
	    me->block_size = strlen (postString);
	  }
	else
	  {
	    me->mem_ptr = "";
	    me->block_size = 0;
	  }
	HTRequest_setMethod (me->request, METHOD_POST);
	HTRequest_setPostCallback (me->request, AHTUpload_callback);
     }
   else
     {
	me->method = METHOD_GET;
	me->dest = (HTParentAnchor *) NULL;	/*useful only for PUT and POST methods */
	if (!HasKnownFileSuffix (ref))
	  HTRequest_setConversion(me->request, acceptTypes, TRUE);
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
	char* tmp;

	tmp = TtaGetMemory (strlen (outputfile) + 1);
	strcpy (tmp, outputfile);
	me->outputfile = tmp;

	tmp = TtaGetMemory (MAX_LENGTH + 1);
        strncpy (tmp, urlName, MAX_LENGTH);
        tmp[MAX_LENGTH] = EOS;
	me->urlName = tmp;
     }
   else
     {
	me->outputfile = outputfile;
	me->urlName = urlName;
     }
   
   /* prepare the URLname that will be displayed in teh status bar */
   ChopURL (me->status_urlName, me->urlName);

/***
Change for taking into account the stop button:
The requests will be always asynchronous, however, if mode=AMAYA_SYNC,
we will loop until the document has been received or a stop signal
generated
****/

   HTRequest_setPreemptive (me->request, NO);
   TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_FETCHING),
		 me->status_urlName);

   me->anchor = (HTParentAnchor *) HTAnchor_findAddress (ref);

   TtaFreeMemory (ref);

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

	if (me->output)
	    fclose (me->output);

	if ((mode & AMAYA_ASYNC) || (mode & AMAYA_IASYNC))
	  {
	    if(me->outputfile)
	      TtaFreeMemory (me->outputfile);
	    if(me->urlName)
	      TtaFreeMemory (me->urlName);
	  }

	if (me->reqStatus == HT_ERR)
	  {
	    status = HT_ERROR;
	    /* show an error message on the status bar */
	  DocNetworkStatus[me->docid] |= AMAYA_NET_ERROR;
	    TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_CANNOT_LOAD),
			  me->status_urlName);
	  }
	else
	  status = HT_OK;

	AHTReqContext_delete (me);
     }

   else
     {
	/* part of the stop button handler */

	if ((mode & AMAYA_SYNC) || (mode & AMAYA_ISYNC))
	  {
#            ifndef _WINDOWS
	     status = LoopForStop (me);
#            endif /* _WINDOWS */ 
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
			  /* free the allocated stream object */
			  AHTFWriter_FREE (HTRequest_outputStream(me->request));
			  HTRequest_setOutputStream (me->request, (HTStream *) NULL);
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
   PutObjectWWW
   frontend for uploading a resource to a URL. This function downloads
   a file to be uploaded into memory, it then calls UploadMemWWW to
   finish the job.

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
int                 PutObjectWWW (int docid, char *fileName, char *urlName, int mode, PicType contentType,
				  TTcbf * terminate_cbf, void *context_tcbf)
#else
int                 PutObjectWWW (docid, urlName, fileName, mode, contentType,
				  ,terminate_cbf, context_tcbf)
int                 docid;
char               *urlName;
char               *fileName;
int                 mode;
PicType             contentType;
TTcbf              *terminate_cbf;
void               *context_tcbf;

#endif
{
   /*AHTReqContext      *me; */
   int                 status;

#ifndef _WINDOWS
   int                 fd;
   struct stat         file_stat;
   char               *mem_ptr;
   unsigned long       block_size;

   AmayaLastHTTPErrorMsg [0] = EOS;
   
   if (urlName == NULL || docid == 0 || fileName == NULL ||
       !TtaFileExist (fileName))
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

   status = UploadMemWWW (docid, METHOD_PUT, urlName, contentType, mem_ptr,
			  block_size, mode, terminate_cbf,
			  context_tcbf, (char *) NULL);

   TtaFreeMemory (mem_ptr);
   TtaHandlePendingEvents ();

#endif /*!_WINDOWS */

   return (status);
}

#ifdef _WINDOWS
/*----------------------------------------------------------------------
  WIN_GetObjectWWW
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int WIN_GetObjectWWW (int doc_id, char* URL_name, char* output_file, boolean error_HTML)
#else /* __STDC__ */
int WIN_GetObjectWWW (int doc_id, char* URL_name, char* output_file, boolean error_HTML)
int     doc_id;
char*   URL_name;
char*   output_file;
boolean error_HTML;
#endif /* __STDC__ */
{
   HTRequest* WIN_request    = HTRequest_new ();
   HTList*    WIN_converters = HTList_new ();		/* List of converters */
   HTList*    WIN_encodings  = HTList_new ();		/* List of encoders */
   HTChunk*   WIN_chunk      = NULL;

   FILE*      WIN_tmp_fp;
   char*      WIN_tmp_dir;
   char*      WIN_ref;
   DWORD      WIN_attribs;

   if (URL_name == NULL || doc_id == 0 || output_file == NULL) {
      /* no file to be loaded */
      TtaSetStatus (doc_id, 1, TtaGetMessage (AMAYA, AM_BAD_URL), URL_name);

      if (error_HTML)
	 DocNetworkStatus[doc_id] |= AMAYA_NET_ERROR; /* so we can show the error message */
      return HT_ERROR;
   }

   /* do we support this protocol? */
   if (IsValidProtocol (URL_name) == NO) {
      /* return error */
      output_file[0] = EOS;	/* file could not be opened */
      TtaSetStatus (doc_id, 1, TtaGetMessage (AMAYA, AM_GET_UNSUPPORTED_PROTOCOL), URL_name);
   
      if (error_HTML)
	 DocNetworkStatus[doc_id] |= AMAYA_NET_ERROR; /* so we can show the error message */
      return HT_ERROR;
   }

   /* verify if a docid directory exists */
   WIN_tmp_dir = TtaGetMemory (strlen (TempFileDirectory) + 5 + 1);
   sprintf (WIN_tmp_dir, "C:\\TEMP\\AMAYA\\%d", doc_id);

   WIN_attribs = GetFileAttributes (WIN_tmp_dir) ;

   if (WIN_attribs == 0xFFFFFFFF) {
      /*directory did not exist */
      if (mkdir (WIN_tmp_dir, S_IRWXU) == -1) {
	 /*error */
	  output_file[0] = EOS;
	  TtaSetStatus (doc_id, 1, TtaGetMessage (AMAYA, AM_CACHE_ERROR), URL_name);
	   
	  if (error_HTML)
	     DocNetworkStatus[doc_id] |= AMAYA_NET_ERROR; /* so we can show the error message */
	  return HT_ERROR;
      }
   }

   /*create a tempfilename */
   sprintf (output_file, "%s\\%04dAM", WIN_tmp_dir, object_counter);
   TtaFreeMemory (WIN_tmp_dir);

   /* update the object_counter */
   object_counter++;

   /* normalize the URL */
   WIN_ref = AmayaParseUrl (URL_name, "", AMAYA_PARSE_ALL);

   /* should we abort the request if we could not normalize the url? */

   if (WIN_ref == (char*) NULL || WIN_ref[0] == EOS) {
      /*error */
      output_file[0] = EOS;
      TtaSetStatus (doc_id, 1, TtaGetMessage (AMAYA, AM_BAD_URL), URL_name);

      if (error_HTML)
	 DocNetworkStatus[doc_id] |= AMAYA_NET_ERROR; /* so we can show the error message */
      return HT_ERROR;
   }
   /* verify if that file name existed */
   if (TtaFileExist (output_file))
      TtaFileUnlink (output_file);

   /* try to open the outputfile */
   if ((WIN_tmp_fp = fopen (output_file, "wb")) == NULL) {
      output_file[0] = EOS;	/* file could not be opened */
      TtaSetStatus (doc_id, 1, TtaGetMessage (AMAYA, AM_CANNOT_CREATE_FILE), output_file);
      TtaFreeMemory (WIN_ref);
    
      if (error_HTML)
	 DocNetworkStatus[doc_id] |= AMAYA_NET_ERROR; /* so we can show the error message */
      return (HT_ERROR);
   }

   HTLibInit ("Amaya for Windows", "1.0a");

   /* Register the default set of transport protocols */
   HTTransportInit ();
   
   /* Register the default set of protocol modules */
   HTProtocolInit ();
   
   /* Register the default set of BEFORE and AFTER callback functions */
   HTNetInit ();
   
   /* Register the default set of converters */
   HTConverterInit (WIN_converters);
   HTFormat_setConversion (WIN_converters);
   
   /* Register the default set of transfer encoders and decoders */
   HTEncoderInit (WIN_encodings);
   HTFormat_setTransferCoding (WIN_encodings);
   
   /* Register the default set of MIME header parsers */
   HTMIMEInit ();
   
   /* Set up the request and pass it to the Library */
   HTRequest_setOutputFormat (WIN_request, WWW_SOURCE);
   HTRequest_setPreemptive (WIN_request, YES);
   if (WIN_ref) {
      char*     WIN_cwd          = HTGetCurrentDirectoryURL ();
      char*     WIN_absolute_url = HTParse (WIN_ref, WIN_cwd, PARSE_ALL);
      HTAnchor* WIN_anchor       = HTAnchor_findAddress (WIN_absolute_url);
      WIN_chunk                  = HTLoadAnchorToChunk (WIN_anchor, WIN_request);
      HT_FREE (WIN_absolute_url);
      HT_FREE (WIN_cwd);
      
       /* If chunk != NULL then we have the data */
      if (WIN_chunk) {
	 char* WIN_string = HTChunk_toCString (WIN_chunk);
	 if (WIN_string)
	    fprintf (WIN_tmp_fp, "%s", WIN_string);
	 HT_FREE (WIN_string);
      }
   }
   
   /* Clean up the request */
   HTRequest_delete(WIN_request);
   HTFormat_deleteAll();
   
   /* Terminate the Library */
   HTLibTerminate();
   fclose (WIN_tmp_fp);
   return 0;
}
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
  UploadMemWWW
  low level interface function to libwww for uploading a block of
  memory to a URL.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 UploadMemWWW (int docid, HTMethod method,
		     char *urlName, PicType contentType, char *mem_ptr, unsigned long block_size,
			int mode, TTcbf * terminate_cbf, void *context_tcbf,
				  char *outputfile)
#else
int                 UploadMemWWW (docid, method, urlName, contentType, mem_ptr, block_size, mode,
				  terminate_cbf, context_tcbf, outputfile)
int                 docid;
HTMethod            method;
char               *urlName;
PicType             contentType;
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

   if (mem_ptr == (char *) NULL ||
       block_size == 0 ||
       docid == 0 ||
       urlName == (char *) NULL)
     {
	/* nothing to be uploaded */
	return HT_ERROR;
     }

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

   /* select the parameters that distinguish a PUT from a GET/POST */
   me->method = METHOD_PUT;
   HTRequest_setMethod (me->request, METHOD_PUT);
   me->output = stdout;
   /* we are not expecting to receive any input from the server */
   me->outputfile = (char *) NULL; 

   me->mem_ptr = mem_ptr;
   me->block_size = block_size;

   /* set the callback which will actually copy data into the
      output stream */

   HTRequest_setPostCallback (me->request, AHTUpload_callback);

   HTRequest_setOutputStream (me->request,
			      HTFWriter_new (me->request, me->output, YES));

   me->anchor = (HTParentAnchor *) HTAnchor_findAddress (urlName);

   /* Set the Content-Type of the file we are uploading */
   HTAnchor_setFormat ((HTParentAnchor *) me->anchor, AHTGuessAtom_for (me->urlName, contentType));

   HTAnchor_setLength ((HTParentAnchor *) me->anchor, me->block_size);
   HTRequest_setEntityAnchor (me->request, me->anchor);
   status = HTLoadAbsolute (urlName, me->request);

   if (status == HT_ERROR || me->reqStatus == HT_END || me->reqStatus == HT_ERR || HTError_hasSeverity (HTRequest_error (me->request), ERR_INFO))
     {
       status = HT_ERROR;
       AHTReqContext_delete (me);
     }     
   else
     {
       /* prepare the URLname that will be displayed in teh status bar */
       ChopURL (me->status_urlName, me->urlName);
       TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_REMOTE_SAVING),
		     me->status_urlName);

	/* part of the stop button handler */

	if ((mode & AMAYA_SYNC) || (mode & AMAYA_ISYNC))
	  {
#            ifndef _WINDOWS
	     status = LoopForStop (me);
#            endif /* _WINDOWS */ 
	     AHTReqContext_delete (me);
	  }
     }
   return (status);
}



/*----------------------------------------------------------------------
  Stop Request
  stops (kills) all active requests associated with a docid 
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
#                             ifndef _WINDOWS
			      RequestKillAllXtevents (me);
#                             endif
			      me->reqStatus = HT_ABORT;
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

/*
  end of Module query.c
*/

#endif /* AMAYA_JAVA */


