/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * query.c : contains all the functions for requesting and publishing
 * URLs via libwww. It handles any eventual HTTP error code
 * (redirection, authentication needed, not found, etc.)
 *
 * Author: J. Kahan
 *         J. Kahan/R. Guetari Windows 95/NT routines
 */

#ifndef AMAYA_JAVA

/* Amaya includes  */
#define THOT_EXPORT extern
#include "amaya.h"
#ifdef _WINDOWS 
#include <fcntl.h>
#endif /* _WINDOWS */

#if defined(__svr4__)
#define CATCH_SIG
#endif

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
static  boolean    AmayaAlive;          /* set to 1 if the application is
					   active; 0 if we have killed it */

#include "answer_f.h"
#include "query_f.h"
#include "AHTURLTools_f.h"
#include "AHTBridge_f.h"
#include "AHTMemConv_f.h"
#include "AHTFWrite_f.h"

#ifdef _WINDOWS
#ifdef __STDC__
int WIN_Activate_Request (HTRequest* , HTAlertOpcode, int, const char*, void*, HTAlertPar*);
#else
int WIN_Activate_Request ();
#endif /* __STDC__ */
#endif /* _WINDOWS */

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
  
   /* clean the associated file structure) */
   HTRequest_setOutputStream (me->request, NULL);
 
   /* Initialize the other members of the structure */
   me->reqStatus = HT_NEW; /* initial status of a request */
   me->output = NULL;
   me->content_type = NULL;
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

#ifdef DEBUG_LIBWWW
   fprintf (stderr, "AHTReqContext_new: Created object %p\n", me);
#endif   
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

#ifdef DEBUG_LIBWWW
        fprintf (stderr, "AHTReqContext_delete: Deleting object %p\n", me);
#endif   

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

	if (HTRequest_outputStream (me->request))
	  AHTFWriter_FREE (me->request->output_stream);

	HTRequest_delete (me->request);

	if (me->output && me->output != stdout)
	  {	
#ifdef DEBUG_LIBWWW       
	    fprintf (stderr, "AHTReqContext_delete: URL is  %s, closing "
		     "FILE %p\n", me->urlName, me->output); 
#endif
	    fclose (me->output);
	    me->output = NULL;
	  }
	  
	if (me->error_stream != (char *) NULL)
	  HT_FREE (me->error_stream);
#     ifndef _WINDOWS
#     ifdef WWW_XWINDOWS	
	if (me->read_xtinput_id || me->write_xtinput_id ||
            me->except_xtinput_id)
          RequestKillAllXtevents(me);
#     endif /* WWW_XWINDOWS */
#     endif /* !_WINDOWS */

    if (me->reqStatus == HT_ABORT)
      {
      if (me->outputfile && me->outputfile[0] != EOS)
	{
	  TtaFileUnlink (me->outputfile);
	  me->outputfile[0] = EOS;
	}
      }

	if ((me->mode & AMAYA_ASYNC) || (me->mode & AMAYA_IASYNC))
	  /* for the ASYNC mode, free the memory we allocated in GetObjectWWW
	     or in PutObjectWWW */
	  {
            if (me->urlName)
	       TtaFreeMemory (me->urlName);
            if (me->outputfile)
	       TtaFreeMemory (me->outputfile);
	    if (me->content_type)
	      TtaFreeMemory (me->content_type);
	  }
   
	if (me->mode & AMAYA_FORM_POST)
	  TtaFreeMemory (me->mem_ptr);

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
#ifdef _WINDOWS
	return HT_CONTINUE;
#else
     	return HT_WOULD_BLOCK;
#endif /* _WINDOWS */

	return HT_WOULD_BLOCK;
     }
   else if (status == HT_PAUSE)
     {
	if (PROT_TRACE)
	   HTTrace ("Posting Data Target PAUSED\n");
	/*
	return HT_PAUSE;
	*/
	return HT_CONTINUE;
	
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
#ifdef DEBUG_LIBWWW
      fprintf (stderr, "Thread_deleteAll: Killing %d outstanding "
	               "requests\n", Amaya->open_requests);
#endif   
	{
	  cur = Amaya->reqlist;
	  
	  /* erase the requests */
	  while ((me = (AHTReqContext *) HTList_removeLastObject (cur)))
	    {
	      if (me->request)
		{
#              ifndef _WINDOWS 
		  RequestKillAllXtevents (me);
#              endif /* !_WINDOWS */

		  if (me->request->net)
		    HTRequest_kill (me->request);
#ifndef _WINDOWS
		  AHTReqContext_delete (me);
#endif /* _WINDOWS */
		}
	    }		/* while */
	  
	  /* erase the docid_status entities */
	  while ((docid_status = (AHTDocId_Status *) HTList_removeLastObject ((void *) Amaya->docid_status)))
	    TtaFreeMemory ((void *) docid_status);
	  
	}			/* if */
	
    }
}

/*----------------------------------------------------------------------
  AHTOpen_file
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 AHTOpen_file (HTRequest * request)
#else
int                 AHTOpen_file (request)
HTRequest           *request;

#endif /* __STDC__ */
{
  AHTReqContext      *me;      /* current request */

  me = HTRequest_context (request);


  if (!me)
      return HT_ERROR;

#ifdef DEBUG_LIBWWW
  fprintf(stderr, "AHTOpen_file: start\n");
#endif /* DEBUG_LIBWWW */

  if (me->reqStatus == HT_ABORT) 
    {
#ifdef DEBUG_LIBWWW
      fprintf(stderr, "AHTOpen_file: caught an abort request, skipping it\n");
#endif /* DEBUG_LIBWWW */

      return HT_OK;
    }

  if (HTRequest_outputStream (me->request)) 
    {

#ifdef DEBUG_LIBWWW
      fprintf(stderr, "AHTOpen_file: output stream already existed for url %s\n", me->urlName);
#endif /* DEBUG_LIBWWW */      
      return HT_OK;
    }

#ifdef DEBUG_LIBWWW
      fprintf(stderr, "AHTOpen_file: opening output stream for url %s\n", me->urlName);
#endif /* DEBUG_LIBWWW */      

  if (!(me->output) && 
      (me->output != stdout) && 
#ifndef _WINDOWS
      (me->output = fopen (me->outputfile, "w")) == NULL)
    {
#else
    (me->output = fopen (me->outputfile, "wb")) == NULL) 
    {
#endif /* !_WINDOWS */

      me->outputfile[0] = '\0';	/* file could not be opened */
#ifdef DEBUG_LIBWWW
      fprintf(stderr, "AHTOpen_file: couldn't open output stream for url %s\n", me->urlName);
#endif
      TtaSetStatus (me->docid, 1, 
		    TtaGetMessage (AMAYA, AM_CANNOT_CREATE_FILE),
		    me->outputfile);
      me->reqStatus = HT_ERR;
      return (HT_ERROR);
    }
	  
  HTRequest_setOutputStream (me->request,
			     AHTFWriter_new (me->request, 
					     me->output, YES));
  me->reqStatus = HT_WAITING;
  
  return HT_OK;
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

   HTAnchor           *new_anchor = HTResponse_redirection (response);
   AHTReqContext      *me = HTRequest_context (request);
   HTMethod            method = HTRequest_method (request);
   char               *ref;
   char               *tmp;

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
	/* do we need to normalize the URL? */
	if (strncmp (new_anchor->parent->address, "http:", 5))
	  {
	     /* Yes, so we use the pre-redirection anchor as a base name */
	     ref = AmayaParseUrl (new_anchor->parent->address, 
				  me->urlName, AMAYA_PARSE_ALL);
	     if (ref)
	       {
		 HT_FREE (new_anchor->parent->address);
		 tmp = NULL;
		 HTSACopy (&tmp, ref);
		 new_anchor->parent->address = tmp;
		 TtaFreeMemory (ref);
	       }
	     else
	       return HT_ERROR; /* We can't redirect anymore */
	  }

	/* update the current file name */
	if (strlen (new_anchor->parent->address) > (MAX_LENGTH - 2))
	  {
	     strncpy (me->urlName, new_anchor->parent->address, MAX_LENGTH - 1);
	     me->urlName[MAX_LENGTH - 1] = EOS;
	  }
	else
	  strcpy (me->urlName, new_anchor->parent->address);

	ChopURL (me->status_urlName, me->urlName);
	TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_RED_FETCHING),
		      me->status_urlName);

	/* Start request with new credentials */

	if (HTRequest_outputStream (me->request) != NULL) {
	  AHTFWriter_FREE (request->output_stream);
	  if (me->output != stdout) { /* Are we writing to a file? */
#ifdef DEBUG_LIBWWW
	  fprintf (stderr, "redirection_handler: New URL is  %s, closing "
		             "FILE %p\n", me->urlName, me->output); 
#endif 
	    fclose (me->output);
	    me->output = NULL;
	  }
	}

	/* reset the status */
	me->reqStatus = HT_NEW;
	/* clear the errors */
	HTError_deleteAll( HTRequest_error (request));
	HTRequest_setError (request, NULL);
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
	 DocNetworkStatus[me->docid] |= AMAYA_NET_ERROR; 
       /* so that we can show the error message */
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
   char               *content_type;
   boolean             error_flag;

   if (!me)
     return HT_OK;		/* not an Amaya request */
   
   /* if Amaya was killed, treat with this request as if it were
      issued by a Stop button event */
   if (!AmayaAlive)           
      me->reqStatus = HT_ABORT; 
   
   if (status == HT_LOADED || 
       status == HT_CREATED || 
       status == HT_NO_DATA ||
       me->reqStatus == HT_ABORT)
     error_flag = FALSE;
   else
     error_flag = TRUE;
   
   /* output any errors from the server */
   
   /*
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
   
#ifdef DEBUG_LIBWWW
   fprintf (stderr, "terminate_handler: URL is "
	    "%s, closing FILE %p status is %d\n", me->urlName, me->output, 
	    status); 
#endif
   
   if (me->output && me->output != stdout)
     {
       /* we are writing to a file */
       if (me->reqStatus != HT_ABORT)
	 {			/* if the request was not aborted and */
	   if (error_flag &&
	       me->error_html == TRUE)
	       /* there were some errors and we want to print them */
	     {		
	       if (me->error_stream_size == 0)/* and the stream is empty */
		 AHTError_MemPrint (request); /* copy errors from 
						  **the error stack 
						  ** into the error stream */
	       if (me->error_stream)
		 {	/* if the stream is non-empty */
		   fprintf (me->output, me->error_stream);/* output the errors */
		   /* Clear the error context, so that we can deal with
		      this answer as if it were a normal reply */
		    HTError_deleteAll( HTRequest_error (request));
		    HTRequest_setError (request, NULL);
		    error_flag = 0;
		 }
	     }		        /* if error_stack */
	 }

       /* if != HT_ABORT */
       
#ifdef DEBUG_LIBWWW       
       fprintf (stderr, "terminate_handler: URL is  %s, closing "
		"FILE %p\n", me->urlName, me->output); 
#endif
       fclose (me->output);
       me->output = NULL;
     }

   
   if (error_flag)
     me->reqStatus = HT_ERR;
   else if (me->reqStatus != HT_ABORT)
     me->reqStatus = HT_END;

   /* copy the content_type */  
   content_type = request->anchor->content_type->name; 
   if (content_type && content_type [0] != EOS)  
     {
        /* libwww gives www/unknown when it gets an error. As this is 
           an HTML test, we force the type to text/html */
	if (!strcmp (content_type, "www/unknown"))
          {
	    if (me->content_type == NULL)
	      me->content_type = TtaStrdup ("text/html");
	    else
	      strcpy (me->content_type, "text/html");
          }
        else 
          {
	    if (me->content_type == NULL)
	      me->content_type = TtaStrdup (content_type);
	    else
	      {
		strncpy (me->content_type, content_type, NAME_LENGTH -1);
		me->content_type [NAME_LENGTH-1] = '\0';
	      }
           
           /* Content-Type can be specified by a server's admin. To be on
              the safe side, we normalize its case */
           ConvertToLowerCase (me->content_type);
          } 
#ifdef DEBUG_LIBWWW
        fprintf (stderr, "content type is: %s\n", me->content_type);
#endif /* DEBUG_LIBWWW */
     }
 
  /* don't remove or Xt will hang up during the PUT */
   if (AmayaAlive  && ((me->method == METHOD_POST) ||
			 (me->method == METHOD_PUT)))
     {
       PrintTerminateStatus (me, status);
     } 

  ProcessTerminateRequest (me);
  return HT_OK;
}

/*----------------------------------------------------------------------
  AHTLoadTerminate_handler
  this is an application "AFTER" Callback. It's called by the library
  when a request has ended, so that we can setup the correct status.
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
		  TtaSetStatus (me->docid, 1, 
				TtaGetMessage (AMAYA, AM_ELEMENT_LOADED),
				me->status_urlName);
	       break;

	    case HT_NO_DATA:
	       if (PROT_TRACE)
		  HTTrace ("Load End.... OK BUT NO DATA: `%s\'\n", 
			   me->status_urlName);
	       TtaSetStatus (me->docid, 1, 
			     TtaGetMessage (AMAYA, AM_LOADED_NO_DATA),
			     me->status_urlName);
	       break;

	    case HT_INTERRUPTED:
	       if (PROT_TRACE)
		  HTTrace ("Load End.... INTERRUPTED: `%s\'\n", 
			   me->status_urlName);
	       TtaSetStatus (me->docid, 1, 
			     TtaGetMessage (AMAYA, AM_LOAD_ABORT), 
			     NULL);
	       break;

	    case HT_RETRY:
	       if (PROT_TRACE)
		  HTTrace ("Load End.... NOT AVAILABLE, RETRY AT %ld\n",
			   HTResponse_retryTime (response));
	       TtaSetStatus (me->docid, 1, 
			     TtaGetMessage (AMAYA, AM_NOT_AVAILABLE_RETRY),
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
			   me->status_urlName ? me->status_urlName :"<UNKNOWN>");
	       TtaSetStatus (me->docid, 1,
			     TtaGetMessage (AMAYA, AM_CANNOT_LOAD),
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
  This function prepares the Accept header used by Amaya during
  the HTTP content negotiation phase
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

      HTConversion_add (c, "image/png",  "www/present", 
			HTThroughLine, 1.0, 0.0, 0.0);
      HTConversion_add (c, "image/jpeg", "www/present", 
			HTThroughLine, 1.0, 0.0, 0.0);
      HTConversion_add (c, "image/gif",  "www/present", 
			HTThroughLine, 1.0, 0.0, 0.0);
      HTConversion_add (c, "image/xbm",  "www/present", 
			HTThroughLine, 1.0, 0.0, 0.0);
      HTConversion_add (c, "image/xpm",  "www/present", 
			HTThroughLine, 1.0, 0.0, 0.0);
      HTConversion_add (c, "application/postscript",  
			"www/present", HTThroughLine, 1.0, 0.0, 0.0);

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
#if 0
   HTConversion_add (c, "www/cache", "*/*", HTCacheWriter,
		     1.0, 0.0, 0.0);
#endif

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
      NB. Preemptive == YES means Blocking requests
      Non-preemptive == NO means Non-blocking requests
   */

   HTProtocol_add ("http", "buffered_tcp", NO, HTLoadHTTP, NULL);
   /*   HTProtocol_add ("http", "tcp", NO, HTLoadHTTP, NULL); */
   HTProtocol_add ("file", "local", NO, HTLoadFile, NULL);
#if 0 /* experimental code */
   HTProtocol_add ("cache", "local", NO, HTLoadCache, NULL);
   HTProtocol_add ("ftp", "tcp", NO, HTLoadFTP, NULL);
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
  

  HTNet_addBefore (HTCredentialsFilter, "http://*", NULL, 6);
  HTNet_addBefore (HTProxyFilter, NULL, NULL, 10);
  /*  HTNet_addBefore (AHTOpen_file, NULL, NULL, 11); */
  HTHost_setActivateRequestCallback (AHTOpen_file);

/*      register AFTER filters
**      The AFTER filters handle error messages, logging, redirection,
**      authentication etc.
**      The filters are called in the order by which the are registered
**      Not done automaticly - may be done by application!
*/

   HTNet_addAfter (HTAuthFilter, "http://*", NULL, HT_NO_ACCESS, HT_FILTER_MIDDLE);
   HTNet_addAfter (redirection_handler, "http://*", NULL, HT_TEMP_REDIRECT, HT_FILTER_MIDDLE);
   HTNet_addAfter (redirection_handler, "http://*", NULL, HT_PERM_REDIRECT, HT_FILTER_MIDDLE);
   HTNet_addAfter (HTUseProxyFilter, "http://*", NULL, HT_USE_PROXY, HT_FILTER_MIDDLE);
   HTNet_addAfter (AHTLoadTerminate_handler, NULL, NULL, HT_ALL, HT_FILTER_LAST);	
   /* handles all errors */
   HTNet_addAfter (terminate_handler, NULL, NULL, HT_ALL, HT_FILTER_LAST);
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
#  ifndef _WINDOWS
   /*   HTAlert_add ((HTAlertCallback *) Add_NewSocket_to_Loop, HT_PROG_CONNECT); */
#  else  /* _WINDOWS */
   HTAlert_add ((HTAlertCallback *) WIN_Activate_Request, HT_PROG_CONNECT);
#  endif /* _WINDOWS */
   HTAlert_add (AHTError_print, HT_A_MESSAGE);
   HTError_setShow (~((unsigned int) 0 ) & ~((unsigned int) HT_ERR_SHOW_DEBUG));	/* process all messages except debug ones*/
   HTAlert_add (AHTConfirm, HT_A_CONFIRM);
   HTAlert_add (AHTPrompt, HT_A_PROMPT);
   HTAlert_add (AHTPromptPassword, HT_A_SECRET);
   HTAlert_add (AHTPromptUsernameAndPassword, HT_A_USER_PW);
}

/*----------------------------------------------------------------------
  ProxyInit
  Reads any proxies settings which may be declared as environmental
  variables or in the thot.ini file. The former overrides the latter.
  ----------------------------------------------------------------------*/
static void ProxyInit (void)
{
  char *strptr;
  char *str = NULL;
  char *name;

  /* get the proxy settings from the thot.ini file */
  strptr = (char *) TtaGetEnvString ("HTTP_PROXY");
  if (strptr && *strptr)
    HTProxy_add ("http", strptr);
  /* get the no_proxy settings from the thot.ini file */
  strptr = (char *) TtaGetEnvString ("NO_PROXY");
  if (strptr && *strptr) 
    {
      str = TtaStrdup (strptr);          /* Get copy we can mutilate */
      strptr = str;
      while ((name = HTNextField (&strptr)) != NULL) {
	char *portstr = strchr (name, ':');
	unsigned port=0;
	if (portstr) {
	  *portstr++ = '\0';
	  if (*portstr) port = (unsigned) atoi(portstr);
	}
	/* Register it for all access methods */
	HTNoProxy_add (name, NULL, port);
      }
      TtaFreeMemory (str);
    }
  
  /* use libw3's routine to get all proxy settings from the environment */
   HTProxy_getEnvVar ();

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
   AHTProtocolInit ();

   /* Enable the persistent cache */
   /*   HTCacheInit (NULL, 20); */

   /* Register the default set of BEFORE and AFTER filters */
   AHTNetInit ();

   /* Set up the default set of Authentication schemes */
   HTAAInit ();

   /* Get any proxy settings */
   ProxyInit ();

   /* Register the default set of converters */
   AHTConverterInit (converters);
   AHTAcceptTypesInit (acceptTypes);
   HTFormat_setConversion (converters);

   /* Register the default set of transfer encoders and decoders */
   HTEncoderInit (encodings);	/* chunks ??? */
   HTFormat_setTransferCoding (encodings);

   /* Register the default set of MIME header parsers */
   HTMIMEInit ();   /* must be called again for language selector */

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
  {

    if (HTLib_isInitialized ())
      
#  ifdef _WINDOWS
      HTEventTerminate ();
#  endif _WINDOWS;		
    
    /* Clean up the persistent cache (if any) */
  /*  HTCacheTerminate (); */
    
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

   AmayaAlive = TRUE;
   AHTProfile_newAmaya (HTAppName, HTAppVersion);

   /* New AHTBridge stuff */

#  ifdef _WINDOWS
   AHTEventInit ();
#  endif _WINDOWS;

   HTEvent_setRegisterCallback (AHTEvent_register);
   /*** a effacer ***/
   HTEvent_setUnregisterCallback (AHTEvent_unregister);
	/***  ***/

#  ifndef _WINDOWS
   HTEvent_setUnregisterCallback (AHTEvent_unregister);
#  endif /* _WINDOWS */

#ifdef DEBUG_LIBWWW
  WWW_TraceFlag = SHOW_CORE_TRACE | SHOW_THREAD_TRACE | PROT_TRACE;
#endif

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

   /* Setting up different network parameters */
   /* Maximum number of simultaneous open sockets */
   HTNet_setMaxSocket (8);
   /* different network services timeouts */
   HTDNS_setTimeout (60);
#ifdef _WINDOWS
   /* under windows, the libwww persistent socket handling has
   ** some bugs. The following line inhibits idle socket reusal.
   ** this is a bit slower, but avoids crashes and gives us time
   ** to distribute Amaya before having to patch up libwww.
   */
   HTHost_setPersistTimeout (-1L);
#else
   HTHost_setPersistTimeout (60L);
#endif /* _WINDOWS */

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

#ifndef _WINDOWS
/*----------------------------------------------------------------------
  LoopForStop
  a copy of the Thop event loop so we can handle the stop button.
  Not useful for windows code (Ramzi).
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          LoopForStop (AHTReqContext * me)
#else
static int          LoopForStop (AHTReqContext * me)
#endif
{

   extern ThotAppContext app_cont;
   XEvent                ev;
   XtInputMask           status;
   int                 status_req = HT_OK;

   /* to test the async calls  */
   /* Loop while waiting for new events, exists when the request is over */
   while (me->reqStatus != HT_ABORT &&
	  me->reqStatus != HT_END &&
	  me->reqStatus != HT_ERR) {
	 if (!AmayaAlive)
	    /* Amaya was killed by one of the callback handlers */
	    exit (0);

	 status = XtAppPending (app_cont);
	 if (status & XtIMXEvent) {
            XtAppNextEvent (app_cont, &ev);
	    TtaHandleOneEvent (&ev);
	 } else if (status & (XtIMAll & (~XtIMXEvent))) {
                XtAppProcessEvent (app_cont, (XtIMAll & (~XtIMXEvent)));
	 } else {
               XtAppNextEvent (app_cont, &ev);
	       TtaHandleOneEvent (&ev);
	 }
   }

   switch (me->reqStatus) {
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
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
  QueryClose
  closes all existing threads, frees all non-automatically deallocated
  memory and then ends libwww.
  ----------------------------------------------------------------------*/
void                QueryClose ()
{

   AmayaAlive = FALSE;

   /* remove all the handlers and callbacks that may output a message to
      a non-existent Amaya window */

   HTNet_deleteAfter (AHTLoadTerminate_handler);
   HTNet_deleteAfter (redirection_handler);
   HTAlertCall_deleteAll (HTAlert_global () );
   HTAlert_setGlobal ((HTList *) NULL);
   HTEvent_setRegisterCallback ((HTEvent_registerCallback *) NULL);
   HTEvent_setUnregisterCallback ((HTEvent_unregisterCallback *) NULL);
    HTHost_setActivateRequestCallback (NULL);
   Thread_deleteAll ();
 
   HTProxy_deleteAll ();
   HTNoProxy_deleteAll ();
   HTGateway_deleteAll ();
   AHTProfile_delete ();
}


/*----------------------------------------------------------------------
  InvokeGetObjectWWW_callback
  A simple function to invoke a callback function whenever there's an error
  in GetObjectWWW
  ---------------------------------------------------------------------*/

#ifdef _STDC
void      InvokeGetObjectWWW_callback (int docid, char *urlName, char *outputfile, TTcbf *terminate_cbf, void *context_cbf)
#else
void      InvokeGetObjectWWW_callback (docid, urlName, outputfile, terminate_cbf, context_tcbf)
int docid;
char *urlName;
char *outputfile;
TTcbf *terminate_cbf;
void *context_tcbf;
#endif /* _STDC */
{
  if (!terminate_cbf)
    return;
  
  (*terminate_cbf) (docid, -1, urlName, outputfile,
		    NULL, context_tcbf);  
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
   - content_type a string
 
   Outputs:
   - urlName The URL that was retrieved
   - outputfile The name of the temporary file which holds the
   retrieved data. (Only in case of success)
   - if content_type wasn't NULL, it will contain a copy of the parameter
     sent in the HTTP answer
   Returns:
   HT_ERROR
   HT_OK
 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int GetObjectWWW (int docid, char* urlName, char* postString, char* outputfile, int mode,
		  TIcbf* incremental_cbf, void* context_icbf, TTcbf* terminate_cbf, 
		  void* context_tcbf, boolean error_html, char *content_type)
#else
int GetObjectWWW (docid, urlName, postString, outputfile, mode, incremental_cbf, context_icbf, 
		  terminate_cbf, context_tcbf, error_html, content_type)
int           docid;
char         *urlName;
char         *postString;
char         *outputfile;
int           mode;
TIcbf        *incremental_cbf;
void         *context_icbf;
TTcbf        *terminate_cbf;
void         *context_tcbf;
boolean       error_html;
char 	     *content_type;
#endif
{
   AHTReqContext      *me;
   char               *ref;
   int                 status, l;

   if (urlName == NULL || docid == 0 || outputfile == NULL) {
      /* no file to be loaded */
      TtaSetStatus (docid, 1, TtaGetMessage (AMAYA, AM_BAD_URL), urlName);
       
      if (error_html)
	 /* so we can show the error message */
	 DocNetworkStatus[docid] |= AMAYA_NET_ERROR;
      InvokeGetObjectWWW_callback (docid, urlName, outputfile, terminate_cbf,
				   context_tcbf);
      return HT_ERROR;
   }

   /* do we support this protocol? */
   if (IsValidProtocol (urlName) == NO) {
      /* return error */
      outputfile[0] = EOS;	/* file could not be opened */
      TtaSetStatus (docid, 1, TtaGetMessage (AMAYA, AM_GET_UNSUPPORTED_PROTOCOL), urlName);
      
      if (error_html)
	 /* so we can show the error message */
	 DocNetworkStatus[docid] |= AMAYA_NET_ERROR;
      InvokeGetObjectWWW_callback (docid, urlName, outputfile, terminate_cbf,
				   context_tcbf);
      return HT_ERROR;
   }

   /*create a tempfilename */
   sprintf (outputfile, "%s%c%d%c%04dAM", TempFileDirectory, DIR_SEP, docid, DIR_SEP, object_counter);

   /* update the object_counter */
   object_counter++;
   /* normalize the URL */
   ref = AmayaParseUrl (urlName, "", AMAYA_PARSE_ALL);
   /* should we abort the request if we could not normalize the url? */
   if (ref == (char*) NULL || ref[0] == EOS) {
      /*error */
      outputfile[0] = EOS;
      TtaSetStatus (docid, 1, TtaGetMessage (AMAYA, AM_BAD_URL), urlName);
      
      if (error_html)
	 /* so we can show the error message */
	 DocNetworkStatus[docid] |= AMAYA_NET_ERROR;
      InvokeGetObjectWWW_callback (docid, urlName, outputfile, terminate_cbf,
				   context_tcbf);
      return HT_ERROR;
   }
   /* verify if that file name existed */
   if (TtaFileExist (outputfile))
     TtaFileUnlink (outputfile);

   /* Initialize the request structure */
   me = AHTReqContext_new (docid);
   if (me == NULL) {
     outputfile[0] = EOS;
     /* need an error message here */
     TtaFreeMemory (ref);
     InvokeGetObjectWWW_callback (docid, urlName, outputfile, terminate_cbf,
				   context_tcbf);
     return HT_ERROR;
   }
   
   /* Specific initializations for POST and GET */
   if (mode & AMAYA_FORM_POST) {
     me->method = METHOD_POST;
     if (postString) {
       me->block_size = strlen (postString);
       me->mem_ptr = TtaStrdup (postString);
     } else {
       me->mem_ptr = "";
       me->block_size = 0;
     }
     HTRequest_setMethod (me->request, METHOD_POST);
      HTRequest_setPostCallback (me->request, AHTUpload_callback);
   } else {
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

   /* for the async. request modes, we need to have our
      own copy of outputfile and urlname
    */

   if ((mode & AMAYA_ASYNC) || (mode & AMAYA_IASYNC)) {
      l = strlen (outputfile);
      if (l > MAX_LENGTH)
	me->outputfile = TtaGetMemory (l + 2);
      else
	me->outputfile = TtaGetMemory (MAX_LENGTH + 2);
      strcpy (me->outputfile, outputfile);
      l = strlen (urlName);
      if (l > MAX_LENGTH)
	me->urlName = TtaGetMemory (l + 2);
      else
	me->urlName = TtaGetMemory (MAX_LENGTH + 2);
      strcpy (me->urlName, urlName);
#  ifdef _WINDOWS
      HTRequest_setPreemptive (me->request, NO);
   } else {
     me->outputfile = outputfile;
     me->urlName = urlName;
     HTRequest_setPreemptive (me->request, YES);
   }
#  else /* !_WINDOWS */
   } else {
     me->outputfile = outputfile;
     me->urlName = urlName;
     me->content_type = content_type;
   }
     /***
     Change for taking into account the stop button:
     The requests will be always asynchronous, however, if mode=AMAYA_SYNC,
     we will loop until the document has been received or a stop signal
     generated
     ****/
     HTRequest_setPreemptive (me->request, NO);
#  endif /* _WINDOWS */

   /* prepare the URLname that will be displayed in teh status bar */
   ChopURL (me->status_urlName, me->urlName);

   TtaSetStatus (me->docid, 1, 
		 TtaGetMessage (AMAYA, AM_FETCHING),
		 me->status_urlName);

   me->anchor = (HTParentAnchor *) HTAnchor_findAddress (ref);
   TtaFreeMemory (ref);

   if (mode & AMAYA_FORM_POST) {
      HTAnchor_setFormat ((HTParentAnchor *) me->anchor, HTAtom_for ("application/x-www-form-urlencoded"));
      HTAnchor_setLength ((HTParentAnchor *) me->anchor, me->block_size);
      HTRequest_setEntityAnchor (me->request, me->anchor);
      
      status = HTLoadAbsolute (urlName, me->request);
   } else
     status = HTLoadAnchor ((HTAnchor *) me->anchor, me->request);
   
   if (status == HT_ERROR && me->reqStatus == HT_NEW)
     {
       InvokeGetObjectWWW_callback (docid, urlName, outputfile, terminate_cbf,
    			            context_tcbf);
     }

#ifndef _WINDOWS
   if (status == HT_ERROR ||
    me->reqStatus == HT_END ||
    me->reqStatus == HT_ERR)
   {
     /* in case of error, free all allocated memory and exit */
     if (me->output && me->output != stdout) {
#ifdef DEBUG_LIBWWW      
       fprintf (stderr, "GetObjectWWW:: URL is  %s, closing "
		"FILE %p\n", me->urlName, me->output); 
#endif
       fclose (me->output);
       me->output = NULL;
     }
     
     if (me->reqStatus == HT_ERR) {
	status = HT_ERROR;
	/* show an error message on the status bar */
	DocNetworkStatus[me->docid] |= AMAYA_NET_ERROR;
	TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_CANNOT_LOAD), me->
		      status_urlName);
      } else
	status = HT_OK;

      AHTReqContext_delete (me);
 } else {
         /* part of the stop button handler */
         if ((mode & AMAYA_SYNC) || (mode & AMAYA_ISYNC)) {
            status = LoopForStop (me);
	    AHTReqContext_delete (me);
         }
   }
#else  /* !_WINDOWS */

   if (status == HT_ERROR ||
	   me->reqStatus == HT_ERROR)
     {
	   status = HT_ERROR;

       /* in case of error, close any open files, free all allocated
 	      memory and exit */
       if (me->output && me->output != stdout)
	     {
#ifdef DEBUG_LIBWWW      
           fprintf (stderr, "GetObjectWWW: URL is  %s, closing "
	 	    "FILE %p\n", me->urlName, me->output); 
#endif
           fclose (me->output);
           me->output = NULL;
         }
     
       if (me->reqStatus == HT_ERR) 
	     {
	      /* show an error message on the status bar */
	      DocNetworkStatus[me->docid] |= AMAYA_NET_ERROR;
	      TtaSetStatus (me->docid, 1, 
		  TtaGetMessage (AMAYA, AM_CANNOT_LOAD),
		  me->status_urlName);
         }
     } 
       else 
	  {
	    status = HT_OK;
      }
 
   /* part of the stop button handler */
     if ((mode & AMAYA_SYNC) || (mode & AMAYA_ISYNC))
       {
        if (HTRequest_net (me->request))
           HTRequest_kill (me->request);
        else
           AHTReqContext_delete (me);
       }

#endif /* !_WINDOWS */

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
	TtaSetStatus (docid, 1, 
                      TtaGetMessage (AMAYA, AM_PUT_UNSUPPORTED_PROTOCOL),
		      urlName);
	return HT_ERROR;
     }
   /* read the file into memory */
#  ifndef _WINDOWS
   if ((fd = open (fileName, O_RDONLY)) == -1)
#  else /* _WINDOWS */
   if ((fd = open (fileName, _O_RDONLY | _O_BINARY)) == -1)
#  endif /* _WINDOWS */
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
   return (status);
}

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

#ifdef _WINDOWS
   HTRequest_setPreemptive (me->request, YES);
#else
   HTRequest_setPreemptive (me->request, NO);
#endif /* _WINDOWS */

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

   me->anchor = (HTParentAnchor *) HTAnchor_findAddress (urlName);

   /* Set the Content-Type of the file we are uploading */
   HTAnchor_setFormat ((HTParentAnchor *) me->anchor,
		       AHTGuessAtom_for (me->urlName, contentType));

   HTAnchor_setLength ((HTParentAnchor *) me->anchor, me->block_size);
   HTRequest_setEntityAnchor (me->request, me->anchor);
   /* prepare the URLname that will be displayed in teh status bar */
   ChopURL (me->status_urlName, me->urlName);
   TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_REMOTE_SAVING),
		     me->status_urlName);
   status = HTLoadAbsolute (urlName, me->request);

#ifndef _WINDOWS
   if (status == HT_ERROR || 
       me->reqStatus == HT_END || 
       me->reqStatus == HT_ERR || 
       HTError_hasSeverity (HTRequest_error (me->request), ERR_INFO))
     {
       status = HT_ERROR;
     }     
   else
     {
	/* part of the stop button handler */

	if ((mode & AMAYA_SYNC) || (mode & AMAYA_ISYNC))
	  {
	     status = LoopForStop (me);
	  }
     }
#else /* _WINDOWS */
  if (status == HT_ERROR || 
	  me->reqStatus == HT_ERR) /* || Error_hasSeverity (HTRequest_error (me->request), ERR_INFO)) */
	  status = HT_ERROR;
  else
	  status = HT_OK;
#endif /* _WINDOWS */

    if (HTRequest_net (me->request))
	{
	 HTRequest_kill (me->request);
	}
    else 
        AHTReqContext_delete (me);
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
   HTNet              *reqNet;
   HTHost             *reqHost;
   HTChannel          *reqChannel;
   int                 reqSock;
#ifdef DEBUG_LIBWWW
   int                 open_requests;
#endif /* DEBUG_LIBWWW */
  
   if (Amaya)
     {
#ifdef DEBUG_LIBWWW
       fprintf (stderr, "StopRequest: number of Amaya requests : %d\n", Amaya->open_requests);
#endif /* DEBUG_LIBWWW */

	docid_status = (AHTDocId_Status *) GetDocIdStatus (docid,
						       Amaya->docid_status);
	/* verify if there are any requests at all associated with docid */

	if (docid_status == (AHTDocId_Status *) NULL)
	   return;

#ifdef DEBUG_LIBWWW
	open_requests = docid_status->counter;
#endif /* DEBUG_LIBWWW */

	/* First, kill all pending requests */
	/* We first inhibit the activation of pending requests */
	HTHost_disable_PendingReqLaunch ();
	cur = Amaya->reqlist;
	while ((me = (AHTReqContext *) HTList_nextObject (cur))) 
	  {
	     if (me->docid == docid && me->reqStatus == HT_NEW)
	       {
		 reqNet = HTRequest_net (me->request);
		 reqSock = HTNet_socket (reqNet);
		 reqChannel = HTChannel_find(reqSock);
		 reqHost = HTChannel_host (reqChannel);

		 /* If we have an ASYNC request, we kill it.
		 ** If it's a SYNC request, we just mark it as aborted
		 */
		 me->reqStatus = HT_ABORT;
		 if (((me->mode & AMAYA_IASYNC)
		     ||	 (me->mode & AMAYA_ASYNC))
		     && !(me->mode & AMAYA_ASYNC_SAFE_STOP))
		   {
		     if (HTRequest_net (me->request))
		       /* delete the libwww request context */
		       HTRequest_kill (me->request);
#ifndef _WINDOWS
		     /* delete the Amaya request context */
		     AHTReqContext_delete (me);
#endif /* !_WINDOWS */
		     cur = Amaya->reqlist;
#ifdef DEBUG_LIBWWW
		     /* update the number of open requests */
		     open_requests--;		   
#endif /* DEBUG_LIBWWW */
		   }

		 if (HTHost_isIdle (reqHost) ) {
#ifdef DEBUG_LIBWWW
		   fprintf (stderr, "Host is idle, killing socket %d\n",
			    reqSock);
#endif /* DEBUG_LIBWWW */

		   HTEvent_unregister (reqSock, FD_ALL);
		   HTEvent_register(reqSock, NULL, (SockOps) FD_READ,
				    HTHost_catchClose,  HT_PRIORITY_MAX);
		   NETCLOSE (reqSock);
		   /*	
   		   if (reqChannel && reqHost)
		   HTHost_clearChannel(reqHost, HT_OK);
		   HTHost_catchClose (reqSock, NULL, FD_CLOSE);
		   */
		 }
	       }
	  }
	/* enable the activation of pending requests */
	HTHost_enable_PendingReqLaunch ();

	cur = Amaya->reqlist;
	while ((me = (AHTReqContext *) HTList_nextObject (cur)))
	  {
	     if (me->docid == docid)
	       {
		 /* kill this request */

		 switch (me->reqStatus)
		   {
		   case HT_ABORT:
#ifdef DEBUG_LIBWWW
fprintf (stderr, "Stop: url %s says abort", me->urlName);
#endif /* DEBUG_LIBWWW */
		     break;
		  
		   case HT_END:
#ifdef DEBUG_LIBWWW
fprintf (stderr, "Stop: url %s says end", me->urlName);
#endif /* DEBUG_LIBWWW */
		     break;

		   case HT_BUSY:
		     me->reqStatus = HT_ABORT;
#ifdef DEBUG_LIBWWW
fprintf (stderr, "Stop: url %s going from busy to abort\n", me->urlName);
#endif /* DEBUG_LIBWWW */
		     break;

		   case HT_NEW_PENDING:
		   case HT_WAITING:
		   default:
#ifdef DEBUG_LIBWWW
fprintf (stderr, "Stop: url %s says NEW_PENDING, WAITING", me->urlName);
#endif /* DEBUG_LIBWWW */
		     me->reqStatus = HT_ABORT;
		     
#                 ifndef _WINDOWS
		     RequestKillAllXtevents (me);
#                 endif _WINDOWS

		     reqNet = HTRequest_net (me->request);
		     reqSock = HTNet_socket (reqNet);
		     reqChannel = HTChannel_find(reqSock);
		     reqHost = HTChannel_host (reqChannel);

		 if (((me->mode & AMAYA_IASYNC)
		     ||	 (me->mode & AMAYA_ASYNC))
		     && !(me->mode & AMAYA_ASYNC_SAFE_STOP))
		   {
		     if (HTRequest_net (me->request))
		       /* delete the libwww request context */
		       HTRequest_kill (me->request);
#ifndef _WINDOWS
		     /* delete the Amaya request context */
		     AHTReqContext_delete (me);
#endif /* !_WINDOWS */
		     cur = Amaya->reqlist;
#ifdef DEBUG_LIBWWW
		     /* update the number of open requests */
		     open_requests--;		   
#endif /* DEBUG_LIBWWW */
		   }

		     /* if there are no more requests, then close
			the connection */
		     
		     if (HTHost_isIdle (reqHost) ) {
#ifdef                        DEBUG_LIBWWW
		       fprintf (stderr, "Host is idle, "
				"killing socket %d\n",
				reqSock);
#endif                        /* DEBUG_LIBWWW */
		       HTEvent_unregister (reqSock, FD_ALL);
		       HTEvent_register(reqSock,
					NULL,
					(SockOps) FD_READ,
					HTHost_catchClose,
					HT_PRIORITY_MAX);
		       NETCLOSE (reqSock);				
		       HTHost_clearChannel (reqHost, HT_ERROR);
		       /*
			 if (reqChannel && reqHost)
			 HTHost_clearChannel(reqHost, HT_OK);
			 HTHost_catchClose (reqSock, NULL, FD_CLOSE);
			 */
#ifdef                          DEBUG_LIBWWW				
		       fprintf (stderr, "After killing, "
				"HTHost_isIdle gives %d\n",
				HTHost_isIdle (reqHost));
#endif                          /* DEBUG_LIBWWW */
		     }
#ifdef DEBUG_LIBWWW		     
		     open_requests--;
#endif /* DEBUG_LIBWWW */		     
		     break;
		     
		   }	/* switch */
	       }		/* if me docid */
	  }			/* while */

#ifdef DEBUG_LIBWWW
	fprintf (stderr, "StopRequest: number of Amaya requests : %d\n", Amaya->open_requests);
#endif /* DEBUG_LIBWWW */

     }				/* if amaya open requests */
   
} /* StopRequest */

/*
  end of Module query.c
*/

/*----------------------------------------------------------------------
  AmayaIsAlive
  returns the status of the AmayaAlive flag
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean AmayaIsAlive (void)
#else
boolean AmayaIsAlive ()
#endif /* _STDC_ */
{
  return AmayaAlive;
}
#endif /* AMAYA_JAVA */




