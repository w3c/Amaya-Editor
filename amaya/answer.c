/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * answer.c:  contains all the functions for requesting user input for
 * libwww and for displaying request status.
 *
 * Author: J. Kahan
 *
 */

#ifndef AMAYA_JAVA

/* Included headerfiles */
#define THOT_EXPORT extern
#include "amaya.h"

#include "init_f.h"
#include "query_f.h"
#include "AHTURLTools_f.h"

/* Local definitions */

/* Maximum number of chars of a url that can be displayed on the 
** status window, before we need to truncate the url.
*/

/* libwww's interface needs this */
struct _HTError
  {
     HTErrorElement      element;	/* Index number into HTError */
     HTSeverity          severity;	/* A la VMS */
     BOOL                ignore;	/* YES if msg should not go to user */
     void               *par;	        /* Explanation, e.g. filename  */
     int                 length;	/* For copying by generic routine */
     char               *where;	        /* Which function */
  };



/*----------------------------------------------------------------------
  AHTProgress 
  displays in the status bar the current state of a request   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
 BOOL         AHTProgress (HTRequest *request, HTAlertOpcode op, 
			   int msgnum, const char *dfault,
			   void *input, HTAlertPar *reply)
#else  /* __STDC__ */
 BOOL         AHTProgress (request, op, msgnum, dfault, input, reply)
HTRequest          *request;
HTAlertOpcode       op;
int                 msgnum;
const char         *dfault;
void               *input;
HTAlertPar         *reply;

#endif
{
   AHTReqContext      *me = HTRequest_context (request);
   char                tempbuf[MAX_LENGTH];
   char                buf[11];
   long                cl, bytes_rw;
   int                 pro;
   int                *raw_rw;
   HTParentAnchor     *anchor;
   STRING              Uinput = ISO2WideChar ((char*)input);

   if (request && HTRequest_internal (request))
      return NO;

   if (!me)
      return NO;

   switch (op)
	 {
	    case HT_PROG_TIMEOUT:
	      /* Message temporarily suppresed as it appears when pressing the Stop button */
	      /*
		TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_REQUEST_TIMEOUT), NULL);
		*/
	      break;
	    case HT_PROG_DNS:
	       TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_LOOKING_HOST), Uinput);
	       break;
	    case HT_PROG_CONNECT:
	       TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_CONTACTING_HOST), Uinput);
	       break;
	    case HT_PROG_ACCEPT:
	       TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_WAITING_FOR_CONNECTION), NULL);
	       break;
	    case HT_PROG_DONE:
	       /* a message is displayed elsewhere */
	      break;
	    case HT_PROG_READ:
	       if ((me->method != METHOD_PUT) && (me->method != METHOD_POST))
		 {
		   cl = HTAnchor_length (HTRequest_anchor (request));
		   if (cl > 0)
		     {
		       bytes_rw = HTRequest_bodyRead (request);
		       pro = (int) ((bytes_rw * 100l) / cl);
		       
		       if (pro > 100)		/* libwww reports > 100! */
			 pro = 100;
		       HTNumToStr ((unsigned long) cl, buf, 10);
		       sprintf (tempbuf, "%s (%d%% of %s)\n", me->status_urlName, (int) pro, buf);
		     }
		   else 
		     {
		       bytes_rw = HTRequest_bytesRead(request);
		       raw_rw = input ? (int *) input : NULL;
		       if (bytes_rw > 0)
			 {
			   HTNumToStr(bytes_rw, buf, 10);
			   sprintf (tempbuf, "%s bytes", buf);
			 } 
		       else if (raw_rw && *raw_rw>0) 
			 {
			   HTNumToStr(*raw_rw, buf, 10);
			   sprintf (tempbuf, "%s bytes", buf);
			 } 
		       else 
			 buf[0] = EOS;
		     }
		   /* update the message on the status bar */
		   if (buf[0]) 
		     TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_PROG_READ), ISO2WideChar (tempbuf));
		   else
		     TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_PROG_READ), ISO2WideChar (me->status_urlName));
		 }
	       break;

	    case HT_PROG_WRITE:
	      if ((me->method == METHOD_PUT) || (me->method == METHOD_POST))
		{
		  anchor = HTRequest_anchor (HTRequest_source (request));
		  cl = HTAnchor_length (anchor);
		  if (cl > 0)
		    {
		      bytes_rw = HTRequest_bodyWritten (request);
			pro = (int) ((bytes_rw * 100l) / cl);
			HTNumToStr ((unsigned long) cl, buf, 10);
			sprintf (tempbuf, "%s: Writing (%d%% of %s)\n", me->urlName, pro, buf);
			TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_PROG_WRITE), ISO2WideChar (tempbuf));
		    }
		  else 
		    {
		      bytes_rw = HTRequest_bytesWritten(request);
		      raw_rw = input ? (int *) input : NULL;
		      if (bytes_rw > 0)
			{
			  HTNumToStr(bytes_rw, buf, 10);
			  sprintf (tempbuf, "%s bytes ", buf);
			} 
		      else if (raw_rw && *raw_rw >0) 
			{
			  HTNumToStr(*raw_rw, buf, 10);
			  sprintf (tempbuf, "%s bytes ", buf);
			} 
		      else 
			buf[0] = EOS;
		    }
		  /* update the message on the status bar */
		  /****
		  if (buf[0]) 
		    TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_PROG_READ), tempbuf);
		  else
		  ***/
		    TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_PROG_WRITE), ISO2WideChar (me->status_urlName));
		}
	       break;
		 
	     default:
	       TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_UNKNOWN_STATUS), NULL);
	       break;
	 }

   return YES;
}


/*----------------------------------------------------------------------
  AHTConfirm 
  opens a form to request user confirmation on an action.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
 BOOL         AHTConfirm (HTRequest * request, HTAlertOpcode op, int msgnum,
			  const char * dfault,
			  void *input, HTAlertPar * reply)
#else  /* __STDC__ */
 BOOL         AHTConfirm (request, op, msgnum, dfault, input, reply)
HTRequest          *request;
HTAlertOpcode       op;
int                 msgnum;
const char         *dfault;
void               *input;
HTAlertPar         *reply;

#endif /* __STDC__ */
{
  ThotBool         answer;
  STRING          tmp_buf;
  AHTReqContext   *me = HTRequest_context (request);

    /* for the moment, we only take into account confirmation for
       authentication */
  if (op != HT_A_CONFIRM)
    return TRUE;

  switch (msgnum)
    {
    case HT_MSG_RETRY_PROXY_AUTH:
    case HT_MSG_RETRY_AUTHENTICATION:
      InitConfirm (0, 0, TtaGetMessage (AMAYA, AM_AUTHENTICATION_CONFIRM));
      break;
    case HT_MSG_REDIRECTION:
      InitConfirm (0, 0, TtaGetMessage (AMAYA, AM_REDIRECTION_CONFIRM));
      break;
    case HT_MSG_FILE_REPLACE:
      tmp_buf = TtaAllocString (strlen (me->urlName)
		  		     + ustrlen (TtaGetMessage (AMAYA, AM_OVERWRITE_CHECK))
				     + 10); /*a bit more than enough memory */
      usprintf (tmp_buf, TtaGetMessage (AMAYA, AM_OVERWRITE_CHECK), me->urlName);
      InitConfirm (0, 0, tmp_buf);
      TtaFreeMemory (tmp_buf);
      break;
    case HT_MSG_RULES:
      tmp_buf = TtaAllocString (strlen (me->urlName)
					+ ustrlen (TtaGetMessage (AMAYA, AM_ETAG_CHANGED))
					+ 10); /*a bit more than enough memory */
      usprintf (tmp_buf, TtaGetMessage (AMAYA, AM_ETAG_CHANGED), me->urlName);
      InitConfirm (0, 0, tmp_buf);
      TtaFreeMemory (tmp_buf);
      break;
    default:
      return TRUE;
    }

  if (UserAnswer)
    answer = TRUE;
  else
    answer = FALSE;

  return (answer);
}

/*----------------------------------------------------------------------
  AHTPrompt
  prompts for a text answer and returns this answer.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
 BOOL         AHTPrompt (HTRequest * request, HTAlertOpcode op, int msgnum, const char *dfault,
			       void *input, HTAlertPar * reply)
#else  /* __STDC__ */
 BOOL         AHTPrompt (request, op, msgnum, dfault, input, reply)
HTRequest          *request;
HTAlertOpcode       op;
int                 msgnum;
const char         *dfault;
void               *input;
HTAlertPar         *reply;

#endif
{
  /* JK: removed temporarily this prompt as it should be rewritten for
     use by the FTP module to save a file */
#if 1
  return NO;
#else
   AHTReqContext      *me = HTRequest_context (request);
   char               *result = NULL;

#  ifndef _WINDOWS
   TtaNewForm (BaseDialog + FormAnswer, TtaGetViewFrame (me->docid, 1), 
      TtaGetMessage (AMAYA, AM_GET_AUTHENTICATION), TRUE, 1, 'L', D_CANCEL);

   if (input)
      TtaNewLabel (BaseDialog + Label1, BaseDialog + FormAnswer, (char *) input);

   else if (dfault)
      TtaNewLabel (BaseDialog + Label1, BaseDialog + FormAnswer, (char *) dfault);
   else
     TtaNewLabel (BaseDialog + Label1, BaseDialog + FormAnswer, TtaGetMessage (AMAYA, AM_GET_USER_NAME + msgnum));
   TtaNewTextForm (BaseDialog + AnswerText, BaseDialog + FormAnswer,
		   TtaGetMessage (AMAYA, AM_NAME), NAME_LENGTH, 1, FALSE);
   
   if (reply && msgnum >= 0)
     {
       TtaSetTextForm (BaseDialog + AnswerText, "");
       TtaShowDialogue (BaseDialog + FormAnswer, FALSE);
       TtaWaitShowDialogue ();

	/* give back the reply to the libwww */
	if (*Answer_name)
	  {
	     StrAllocCopy (result, Answer_name);
	     HTAlert_setReplyMessage (reply, result);
	     return YES;
	  }
     }
#  else /* _WINDOWS */
   /* it could be a problem to use this form */
   CreateAuthenticationDlgWindow (TtaGetViewFrame (me->docid, 1));
#  endif /* _WINDOWS */
   return NO;
#endif
}

/*----------------------------------------------------------------------
  AHTPromptUsernameAndPassword
  prompts for  both a username and a password. 
  msgnum is the prompting message.  
  *username and *password	are char pointers; they are	
  changed to point to result strings.			
  If *username is not NULL, it is taken to point to a	
  default value.						
  Initial value of *password is completely discarded.	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
 BOOL         AHTPromptUsernameAndPassword (HTRequest *request, HTAlertOpcode op, int msgnum,
					    const char *dfault, 
					    void *input, HTAlertPar * reply)
#else
BOOL         AHTPromptUsernameAndPassword (request, op, msgnum, dfault, input reply)
HTRequest          *request;
HTAlertOpcode       op;
int                 msgnum;
const char         *dfault;
void               *input;
HTAlertPar         *reply;

#endif /* __STDC */
{
   AHTReqContext      *me = HTRequest_context (request);
   const STRING       realm = ISO2WideChar (HTRequest_realm (request));
   STRING             server;

   if (reply && msgnum >= 0)
     {
       /* initialise */
       Answer_name[0] = EOS;
       Lg_password = 0;
       Answer_password[0] = EOS;

       /* prepare the authentication realm message */
       server = AmayaParseUrl (ISO2WideChar (me->urlName), _EMPTYSTR_, AMAYA_PARSE_HOST);
       /* show the popup */
       InitFormAnswer (me->docid, 1, realm, server);
       /* free allocated memory */
       if (server)
	 TtaFreeMemory (server);

       /* handle the user's answers back to the library */
       if (Answer_name[0] != EOS)
	 {
	   HTAlert_setReplyMessage (reply, WideChar2ISO (Answer_name));
	   if (Answer_password[0] != EOS)
	     {
	       /* give password back to the request */
	       HTAlert_setReplySecret (reply, WideChar2ISO (Answer_password));
	       return YES;
	     }
	 }
     }
   return NO;
}

/*----------------------------------------------------------------------
   IsHTTP09Error
   Returns TRUE if libwww forced a downgrade to 0.9 while fulfilling
   the request. FALSE, otherwise.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool IsHTTP09Error (HTRequest * request)
#else
ThotBool IsHTTP09Error (request)
HTRequest * request;
#endif
{
   HTList             *cur;
   HTError            *pres;
   ThotBool           found = FALSE;
   int                 index;

   cur = HTRequest_error (request);

   while ((pres = (HTError *) HTList_nextObject (cur)))
     {
       index = HTError_index (pres);
       if (index == HTERR_HTTP09) 
	 {
	   found = TRUE;
	   if (WWWTRACE)
	     HTTrace ("IsHTTP09Error..... Detected 0.9 backward compatibility\n");
	   /* we set up a more explicit message, as I don't know what may be
	      before this error */
	   HTRequest_addError (request, ERR_FATAL, NO, HTERR_HTTP09,
			       "Error: Server or network forced libwww to downgrade to HTTP/0.9 for this host.\nPlease quite Amaya.", 0, NULL);
	   /* how do we switch back to 1.x? */
	   /* HTNet_setHost ??? */
	   break;
	 }
     }
   return (found);
}


/*----------------------------------------------------------------------
   AHTError_print (hacked from HTError_print)
   default function that creates an error message using
   HTAlert() to put out the contents of the error_stack messages.
   Furthermore, the error_info structure contains a name of a 
   help file that might be put up as a link. This file can then be
   multi-linguistic.                            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
BOOL                AHTError_print (HTRequest * request, HTAlertOpcode op, int msgnum, const char *dfault, void *input, HTAlertPar * reply)
#else  /* __STDC__ */
BOOL                AHTError_print (request, op, msgnum, dfault, input, reply)
HTRequest          *request;
HTAlertOpcode       op;
int                 msgnum;
const char         *dfault;
void               *input;
HTAlertPar         *reply;

#endif /* __STDC__ */
{
   HTList             *cur = (HTList *) input;
   HTError            *pres;
   AHTReqContext      *me = (AHTReqContext *) HTRequest_context (request);
   int                 index;

   if (WWWTRACE)
      HTTrace ("HTError..... Generating message\n");
   if (!request || !cur)
      return NO;
   while ((pres = (HTError *) HTList_nextObject (cur)))
     {
       index = HTError_index (pres);
       switch (index)
	 {
	 case HTERR_UNAUTHORIZED:
	   TtaSetStatus (me->docid, 1,
			 TtaGetMessage (AMAYA, AM_AUTHENTICATION_FAILURE), ISO2WideChar (me->urlName));
	   break;
	 case HTERR_FORBIDDEN:
	   TtaSetStatus (me->docid, 1,
			 TtaGetMessage (AMAYA, AM_FORBIDDEN_ACCESS), ISO2WideChar (me->urlName));
	   break;
	 case HTERR_SYSTEM:
	   if (!ustrcmp (TEXT("connect"),  ISO2WideChar (HTError_location (pres))))
	     TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_CANT_CONNECT_TO_HOST), (STRING) NULL);
	   else
	     TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_UNKNOWN_SAVE_ERROR), ISO2WideChar (me->urlName));
	   break;
	 case HTERR_NO_REMOTE_HOST:
	   TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_CANT_CONNECT_TO_HOST), (STRING) NULL);
	     break;
	 default:
	   break;
	 }
     }

   return YES;
}

/*----------------------------------------------------------------------
   AHTError_MemPrint (hacked from HTError_print)      
   takes an error message from a request, writes it to a memory pointer,
   then uses it as the actual server response (useful for displaying
   errors as HTML code). 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                AHTError_MemPrint (HTRequest * request)
#else  /* __STDC__ */
void                AHTERROR_MemPrint (request)
HTRequest          *request;

#endif /* __STDC__ */
{
   HTError            *pres;
   HTList             *cur = request->error_stack;
   AHTReqContext      *me = (AHTReqContext *) HTRequest_context (request);
   int                 index;
   char                buffer[1024];


   if (WWWTRACE)	   
      HTTrace ("HTError..... Generating message\n");
   if (!request || !cur)
      return;

   /* force the error type (we're generating it anyway) */
   if (!me->content_type)
     me->content_type = WideChar2ISO (TtaStrdup (TEXT("text/html")));

   while ((pres = (HTError *) HTList_nextObject (cur)))
     {
	index = HTError_index (pres);
	{			/* Error number */
	  switch (index)
	    {
	    case HTERR_NO_REMOTE_HOST:
	    case HTERR_SYSTEM:
	    case HTERR_INTERNAL:
	      if (pres->par != NULL)
		{
		  if (me->method != METHOD_PUT) 
		    {
		      sprintf (buffer, 
			       WideChar2ISO (TtaGetMessage (AMAYA, AM_SYS_ERROR_TMPL)), 
			       me->urlName, me->urlName, 
			       pres->element, pres->par);
		      StrAllocCat (me->error_stream, buffer);
		    }
		  else
		    {
		      sprintf (buffer, "Error: Server is unavailable or doesn't exist");
		      StrAllocCat (me->error_stream, buffer);
		    }
		}
	      return;
	      break;
	    case HTERR_TIME_OUT:
	      if (me->method != METHOD_PUT)
		{
		  sprintf (buffer, WideChar2ISO (TtaGetMessage (AMAYA, AM_SYS_ERROR_TMPL)),
			   me->urlName, me->urlName,
			   pres->element, "connection timeout");
		  StrAllocCat (me->error_stream, buffer);
		}
	      else
		{
		  sprintf (buffer, "Error: Server is unavailable or doesn't exist");
		  StrAllocCat (me->error_stream, buffer);
		}
	      break;	   
	    default:
	      if (pres->par != NULL)
		  StrAllocCat (me->error_stream, pres->par);
	      return;
	      break;
	    }
	}
     }
}

/*----------------------------------------------------------------------
  AHTPrintPendingRequestStatus
  displays a message on the status bar that states the number of
  pending requests.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                AHTPrintPendingRequestStatus (Document docid, BOOL last_seconds_of_life)
#else
void                AHTPrintPendingRequestStatus (doc, last_seconds_of_life)
Document            doc;
BOOL             last_seconds_of_life;

#endif

{
   int                 waiting_count = 0;
   AHTDocId_Status    *docid_status;
   CHAR_T              buffer[120];

   /* verify if there are any requests at all associated with docid */

   docid_status = (AHTDocId_Status *) GetDocIdStatus (docid,
						      Amaya->docid_status);

   if (docid_status == (AHTDocId_Status *) NULL)
      return;

   waiting_count = (last_seconds_of_life) ? (docid_status->counter - 1) : docid_status->counter;

   if (waiting_count > 0)
     {
	usprintf (buffer, TtaGetMessage (AMAYA, AM_WAITING_REQUESTS), waiting_count);
	TtaSetStatus (docid, 1, buffer, NULL);

     }
}

/*----------------------------------------------------------------------
  PrintTerminateStatus 
  displays a message on the status bar indicating the result of the
  request.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void PrintTerminateStatus (AHTReqContext *me, int status) 
#else
void PrintTerminateStatus (me, status)
AHTReqContext *me;
int status;
#endif

{

  HTError             *error = (HTError *) NULL;
  HTError             *next_error;
  HTErrorElement      errorElement;
  HTList              *cur;
  CHAR_T              msg_status[10];
  STRING              server_status = (STRING) NULL;

  if (status == 200)
    TtaSetStatus (me->docid, 1,  
		  TtaGetMessage (AMAYA, AM_REQUEST_SUCCEEDED),
		  ISO2WideChar (me->status_urlName));
  else if (status == 201)
    TtaSetStatus (me->docid, 1, 
		  TtaGetMessage (AMAYA, AM_CREATED_NEW_REMOTE_RESSOURCE),
		  ISO2WideChar (me->status_urlName));
  else if (status == 204 && me->method == METHOD_PUT)
    TtaSetStatus (me->docid, 1,
		  TtaGetMessage (AMAYA, AM_UPDATED_REMOTE_RESSOURCE),
		  ISO2WideChar (me->status_urlName));
  else if (status == 204 && me->method == METHOD_PUT)
    TtaSetStatus (me->docid, 1, 
		  TtaGetMessage (AMAYA, AM_NO_DATA), 
		  (STRING) NULL);
  else if (status == -400 || status == 505)
    {
      TtaSetStatus (me->docid, 1, 
		    TtaGetMessage (AMAYA, 
				   AM_SERVER_DID_NOT_UNDERSTAND_REQ_SYNTAX),
		    (STRING) NULL);
      usprintf (AmayaLastHTTPErrorMsg, 
	       TtaGetMessage (AMAYA,
			      AM_SERVER_DID_NOT_UNDERSTAND_REQ_SYNTAX));
    }
  else if (status == -401) 
    {
      TtaSetStatus (me->docid, 1,
		    TtaGetMessage (AMAYA, 
				   AM_AUTHENTICATION_FAILURE), 
		    ISO2WideChar (me->status_urlName));
      usprintf (AmayaLastHTTPErrorMsg, 
	       TtaGetMessage (AMAYA, AM_AUTHENTICATION_FAILURE),
	       me->status_urlName);
    }
  else if (status == -403)
    {
      TtaSetStatus (me->docid, 1,
		    TtaGetMessage (AMAYA, AM_FORBIDDEN_ACCESS),
		    ISO2WideChar (me->status_urlName));
      usprintf (AmayaLastHTTPErrorMsg, 
	       TtaGetMessage (AMAYA, AM_FORBIDDEN_ACCESS), 
	       me->urlName);
    }
  else if (status == -405)
    {
      TtaSetStatus (me->docid, 1, 
		    TtaGetMessage (AMAYA, AM_METHOD_NOT_ALLOWED),
		    (STRING) NULL);
      usprintf(AmayaLastHTTPErrorMsg, 
	      TtaGetMessage (AMAYA, AM_METHOD_NOT_ALLOWED));
    }
  else if (status == -409)
    {
      TtaSetStatus (me->docid, 1,
		    TEXT("Conflict with the current state of the resource"), NULL);
      usprintf(AmayaLastHTTPErrorMsg, TEXT("409: Conflict with the current state of the resource"));
    }
  else if (status <0)
    {
      /*
      ** Here we deal with errors for which libwww does not
      ** return a correct status code 
      */

      /* get a pointer to the server's status message */
      cur = HTRequest_error (me->request);
      error = (HTError *) HTList_nextObject (cur);
      /* copy the reference to the server status message if it
         exists and if it has an appropriate length */
      if (error)
        {
          /* if there's no error reason (for example a timeout),
             we try to see if the next element has one and then
             we use it */
          if (error->par == NULL && cur)
            {
              next_error = (HTError *) HTList_nextObject (cur);
              if (next_error && next_error->par)
                error = next_error;
            }
          errorElement = error->element;
          if (error->par && error->length < MAX_LENGTH/2
              && ((char *) error->par)[0] != EOS)
            server_status = ISO2WideChar ((char *) error->par);
        }

      if (error == (HTError *) NULL)
	/* there's no error context */
	{
	  usprintf (msg_status, TEXT("%d"), status); 
	  TtaSetStatus (me->docid, 1, 
			TtaGetMessage (AMAYA, AM_UNKNOWN_XXX_STATUS), 
			msg_status);
	  usprintf (AmayaLastHTTPErrorMsg, 
		   TtaGetMessage (AMAYA, AM_UNKNOWN_XXX_STATUS),
		   msg_status);
	  return;
	}

      if (errorElement == HTERR_INTERRUPTED)
	{
	  TtaSetStatus (me->docid, 1,
			TEXT("Transfer interrupted by user"), (STRING) NULL);
	  usprintf (AmayaLastHTTPErrorMsg, "%s",
		    TEXT("Transfer interrupted by user"));
	}
      else if (errorElement == HTERR_PRECON_FAILED) 
	{
	  TtaSetStatus (me->docid, 1,
			TEXT("Document has changed (412)"), (STRING) NULL);
	  usprintf (AmayaLastHTTPErrorMsg, "%s",
		    TEXT("Document has changed (412)"));
	  status = -412;
	}
      else if (errorElement == HTERR_TIME_OUT || errorElement == HTERR_TIMEOUT)
	{
	  TtaSetStatus (me->docid, 1,
			TEXT("Connection Timeout"), (STRING) NULL);
	  usprintf (AmayaLastHTTPErrorMsg,
		    TEXT("Connection Timeout"));
	}
      else if (errorElement == HTERR_NOT_IMPLEMENTED)
	{
	  TtaSetStatus (me->docid, 1,
			TtaGetMessage (AMAYA, AM_SERVER_NOT_IMPLEMENTED_501_ERROR), 
			(STRING) NULL);
	  usprintf (AmayaLastHTTPErrorMsg, 
		    TtaGetMessage (AMAYA, AM_SERVER_NOT_IMPLEMENTED_501_ERROR));
	  status = -501;
	}
      else if (errorElement == HTERR_INTERNAL)
	{
	  if ((error->length > 0) && (error->length <= 25) &&
	      (error->par) && (((char *) error->par)[0] != EOS)) 
	    {
	      TtaSetStatus (me->docid, 1, 
			    TtaGetMessage (AMAYA, AM_SERVER_INTERNAL_ERROR_500_CAUSE), 
			    ISO2WideChar (error->par));
	      usprintf (AmayaLastHTTPErrorMsg, 
			TtaGetMessage (AMAYA, AM_SERVER_INTERNAL_ERROR_500_CAUSE), 
			ISO2WideChar (error->par));
	    }
	  else
	    {
	      TtaSetStatus (me->docid, 1, 
			    TtaGetMessage (AMAYA, AM_SERVER_INTERNAL_ERROR_500_NO_CAUSE), 
			    (STRING) NULL);
	      usprintf (AmayaLastHTTPErrorMsg, 
			TtaGetMessage (AMAYA, AM_SERVER_INTERNAL_ERROR_500_NO_CAUSE));
	    }
	  status = -500; 
	}
      else if (errorElement == HTERR_HTTP09)
	{
	  TtaSetStatus (me->docid, 1,
			TEXT("Server or network forced libwww to downgrade to HTTP/0.9."),
			(STRING) NULL);
	  usprintf (AmayaLastHTTPErrorMsg,
		    TEXT("Server or network forced libwww to downgrade to HTTP/0.9 for this host. Please quit."));
	}
      else
	{
	  usprintf (msg_status, TEXT("%d"), status); 
	  TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_UNKNOWN_XXX_STATUS), msg_status);
	  usprintf (AmayaLastHTTPErrorMsg, TtaGetMessage (AMAYA, AM_UNKNOWN_XXX_STATUS), 
		    msg_status);
	}
    }
}
   


/*
  end of Module answer.c
*/

#endif
