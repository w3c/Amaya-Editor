/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2005
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
 
/* Included headerfiles */
#define THOT_EXPORT extern
#include "amaya.h"

#include "MENUconf.h"
#include "init_f.h"
#include "HTMLhistory_f.h"
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
  void               *par;	/* Explanation, e.g. filename  */
  int                 length;	/* For copying by generic routine */
  char               *where;	/* Which function */
};



/*----------------------------------------------------------------------
  AHTProgress 
  displays in the status bar the current state of a request   
  ----------------------------------------------------------------------*/
BOOL AHTProgress (HTRequest *request, HTAlertOpcode op, 
                  int msgnum, const char *dfault,
                  void *input, HTAlertPar *reply)
{
  AHTReqContext      *me = (AHTReqContext *)HTRequest_context (request);
  HTParentAnchor     *anchor;
  char                text[MAX_LENGTH];
  char                tempbuf[MAX_LENGTH];
  char                buf[11];
  long                cl, bytes_rw;
  float               pro;
  int                *raw_rw;

  if (request && HTRequest_internal (request))
    return NO;
  if (!me)
    return NO;
  if (input) 
    strcpy (text, (char *) input);
  else 
    text[0] = EOS;
  switch (op)
    {
    case HT_PROG_TIMEOUT:
      /*
        Message temporarily suppresed as it appears when pressing the Stop button
        TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_REQUEST_TIMEOUT), NULL);
      */
      break;
    case HT_PROG_DNS:
      TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_LOOKING_HOST), text);
      break;
    case HT_PROG_CONNECT:
      TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_CONTACTING_HOST), text);
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
              pro = (float)(bytes_rw * 100.) / (float)cl;
              if (pro > 100.)		/* libwww reports > 100! */
                pro = 100.;
              HTNumToStr ((unsigned long) cl, buf, 10);
              sprintf (tempbuf, "%s (%d%% of %s)", me->status_urlName, (int)pro, buf);
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
            TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_PROG_READ), tempbuf);
          else
            TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_PROG_READ), me->status_urlName);
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
              pro = (float)(bytes_rw * 1001.) / (float)cl;
              HTNumToStr ((unsigned long) cl, buf, 10);
              sprintf (tempbuf, "%s: Writing (%d%% of %s)", me->urlName, (int)pro, buf);
              TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_PROG_WRITE), tempbuf);
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
          TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_PROG_WRITE), me->status_urlName);
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
BOOL AHTConfirm (HTRequest * request, HTAlertOpcode op, int msgnum,
                 const char *dfault,
                 void *input, HTAlertPar *reply)
{
  ThotBool         answer;
  char            *tmp_buf;
  AHTReqContext   *me = (AHTReqContext *)HTRequest_context (request);
  AHTReqStatus     old_reqStatus = (AHTReqStatus)NULL;

  /* for the moment, we only take into account confirmation for
     authentication */
  if (op != HT_A_CONFIRM)
    return TRUE;

  /* protection against having a stop kill this thread */
  if (me)
    {
      old_reqStatus = me->reqStatus;
      me->reqStatus = HT_BUSY;
    }

  switch (msgnum)
    {
    case HT_MSG_RETRY_PROXY_AUTH:
    case HT_MSG_RETRY_AUTHENTICATION:
      if (!TtaTestWaitShowDialogue ())
        // avoid to close the current authentication dialog
        InitConfirm (0, 0, TtaGetMessage (AMAYA, AM_AUTHENTICATION_CONFIRM));
      break;
    case HT_MSG_REDIRECTION:
      if (me)
        {
          TtaSetStatus (0, 1, TtaGetMessage (AMAYA, AM_REDIRECTION_CONFIRM), NULL);
          if (!SafePut_query(me->urlName) && !TtaTestWaitShowDialogue ())
            // no more than one redirect confirm message
            InitConfirm (0, 0, TtaGetMessage (AMAYA, AM_REDIRECTION_CONFIRM));
        }
      break;
    case HT_MSG_FILE_REPLACE:
      tmp_buf = (char *)TtaGetMemory (strlen (me ? me->urlName : "") + strlen (TtaGetMessage (AMAYA, AM_OVERWRITE_CHECK)) + 10); /*a bit more than enough memory */
      sprintf (tmp_buf, TtaGetMessage (AMAYA, AM_OVERWRITE_CHECK), me ? me->urlName : "");
      InitConfirm (0, 0, tmp_buf);
      TtaFreeMemory (tmp_buf);
      break;
    case HT_MSG_RULES:
      tmp_buf = (char *)TtaGetMemory (strlen (me ? me->urlName : "") + strlen (TtaGetMessage (AMAYA, AM_ETAG_CHANGED)) + 10); /*a bit more than enough memory */
      sprintf (tmp_buf, TtaGetMessage (AMAYA, AM_ETAG_CHANGED), me ? me->urlName : "");
      InitConfirm (0, 0, tmp_buf);
      TtaFreeMemory (tmp_buf);
      break;
    default:
      if (msgnum < 0)
        {
          /* @@@@ IV */
          tmp_buf = (char *)TtaGetMemory (strlen (TtaGetMessage (AMAYA, AM_ERROR)) + 20);
          sprintf (tmp_buf, "%s %d", TtaGetMessage (AMAYA, AM_ERROR), msgnum);
          InitConfirm3L (0, 0, me ? me->urlName : (char *)"", tmp_buf, NULL, FALSE);
          TtaFreeMemory (tmp_buf);
        }
      else
        return TRUE;
    }

  if (UserAnswer)
    answer = TRUE;
  else
    answer = FALSE;

  if (me && me->reqStatus != HT_ABORT)
    me->reqStatus = old_reqStatus;

  return (answer);
}

/*----------------------------------------------------------------------
  AHTPrompt
  prompts for a text answer and returns this answer.
  ----------------------------------------------------------------------*/
BOOL AHTPrompt (HTRequest * request, HTAlertOpcode op, int msgnum, const char *dfault,
                void *input, HTAlertPar * reply)
{
  /* JK: removed temporarily this prompt as it should be rewritten for
     use by the FTP module to save a file */
#if 1
  return NO;
#else
  AHTReqContext      *me = HTRequest_context (request);
  char               *result = NULL;

#ifndef _WINGUI
  TtaNewForm (BaseDialog + FormAnswer, TtaGetViewFrame (me->docid, 1), 
              TtaGetMessage (AMAYA, AM_GET_AUTHENTICATION), TRUE, 1, 'L', D_CANCEL);

  if (input)
    TtaNewLabel (BaseDialog + Label1, BaseDialog + FormAnswer, (char *) input);

  else if (dfault)
    TtaNewLabel (BaseDialog + Label1, BaseDialog + FormAnswer, (char *) dfault);
  else
    TtaNewLabel (BaseDialog + Label1, BaseDialog + FormAnswer, 
                 TtaGetMessage (AMAYA, AM_GET_USER_NAME + msgnum));
  TtaNewTextForm (BaseDialog + AnswerText, BaseDialog + FormAnswer,
                  TtaGetMessage (AMAYA, AM_NAME), NAME_LENGTH, 1, FALSE);
   
  if (reply && msgnum >= 0)
    {
      TtaSetTextForm (BaseDialog + AnswerText, "");
      TtaShowDialogue (BaseDialog + FormAnswer, FALSE, TRUE);
      TtaWaitShowDialogue ();

      /* give back the reply to the libwww */
      if (*Answer_name)
        {
          StrAllocCopy (result, Answer_name);
          HTAlert_setReplyMessage (reply, result);
          return YES;
        }
    }
#else /* _WINGUI */
  /* it could be a problem to use this form */
  CreateAuthenticationDlgWindow (TtaGetViewFrame (me->docid, 1));
#endif /* _WINGUI */
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
BOOL AHTPromptUsernameAndPassword (HTRequest *request, HTAlertOpcode op,
				   int msgnum, const char *dfault,
				   void *input, HTAlertPar * reply)
{
  AHTReqContext      *me = (AHTReqContext *)HTRequest_context (request);
  const char         *realm = HTRequest_realm (request);
  char               *server;
  AHTReqStatus        old_reqStatus;
  int                 i_auth = 0;

  if (reply && msgnum >= 0) 
    {
      /* initialise */
      Answer_name[0] = EOS;
      Lg_password = 0;
      Answer_password[0] = EOS;

      /* prepare the authentication realm message */
      server = AmayaParseUrl (me->urlName, "", AMAYA_PARSE_HOST);
      /* protection against having a stop kill this thread */
      old_reqStatus = me->reqStatus;
      me->reqStatus = HT_BUSY;
      /* ask the password manager */
      i_auth = SearchPasswordTable (realm, server);
      /* show the popup */
      InitFormAnswer (me->docid, 1, realm, server, i_auth);
      if (me->reqStatus != HT_ABORT)
        me->reqStatus = old_reqStatus;

      /* handle the user's answers back to the library */
      if (UserAnswer && !ExtraChoice)
        {
          /* set the user name */
          HTAlert_setReplyMessage (reply, Answer_name);
          /* set the password */
          HTAlert_setReplySecret (reply, Answer_password);
          /* Add the new password in the password table if asked */
          if (Answer_save_password && Answer_name[0] != EOS)
            NewPasswordTable ((char *)realm, server, Answer_name,
                              Answer_password, i_auth, TRUE);
          if (server)
            TtaFreeMemory (server);
          return YES;
        }
      else
        {
          if (server)
            TtaFreeMemory (server);
          return NO;
        }
    }
  return NO;
}

/*----------------------------------------------------------------------
  IsHTTP09Error
  Returns TRUE if libwww forced a downgrade to 0.9 while fulfilling
  the request. FALSE, otherwise.
  ----------------------------------------------------------------------*/
ThotBool IsHTTP09Error (HTRequest *request)
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
             before this error. */
          HTRequest_addError (request, ERR_FATAL, NO, HTERR_BAD_REPLY,
                              (void *)"Error: Server sent an unexpected reply.", 0,
                              NULL);
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
BOOL AHTError_print (HTRequest *request, HTAlertOpcode op, int msgnum,
                     const char *dfault, void *input, HTAlertPar *reply)
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
                        TtaGetMessage (AMAYA, AM_AUTHENTICATION_FAILURE), me->urlName);
          break;
        case HTERR_FORBIDDEN:
          TtaSetStatus (me->docid, 1,
                        TtaGetMessage (AMAYA, AM_FORBIDDEN_ACCESS), me->urlName);
          break;
        case HTERR_SYSTEM:
          if (!strcmp ("connect",  HTError_location (pres)))
            TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_CANT_CONNECT_TO_HOST), NULL);
          else
            TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_UNKNOWN_SAVE_ERROR), me->urlName);
          break;
        case HTERR_NO_REMOTE_HOST:
          TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_CANT_CONNECT_TO_HOST), NULL);
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
void AHTError_MemPrint (HTRequest *request)
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
  if (!me->http_headers.content_type)
    me->http_headers.content_type = TtaStrdup ("text/html");
  while ((pres = (HTError *) HTList_nextObject (cur)))
    {
      index = HTError_index (pres);
      {			/* Error number */
        switch (index)
          {
          case HTERR_NO_REMOTE_HOST:
          case HTERR_NO_HOST:
          case HTERR_SYSTEM:
          case HTERR_INTERNAL:
          case HTERR_CSO_SERVER:
            if (pres->par != NULL)
              {
                if (me->method == METHOD_PUT) 
                  {
                    sprintf (buffer, "Error: Server is unavailable or doesn't exist");
                    StrAllocCat (me->error_stream, buffer);
                  }
              }
            return;
            break;
          case HTERR_TIME_OUT:
            if (me->method == METHOD_PUT)
              {
                sprintf (buffer, "Error: Server is unavailable or doesn't exist");
                StrAllocCat (me->error_stream, buffer);
              }
            break;	   
          default:
            if (pres->par)
              StrAllocCat (me->error_stream, (char *)pres->par);
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
void AHTPrintPendingRequestStatus (Document docid, BOOL last_seconds_of_life)
{
  int                 waiting_count = 0;
  AHTDocId_Status    *docid_status;
  char              buffer[120];

  /* verify if there are any requests at all associated with docid */

  docid_status = (AHTDocId_Status *) GetDocIdStatus (docid,
                                                     Amaya->docid_status);

  if (docid_status == (AHTDocId_Status *) NULL)
    return;

  waiting_count = (last_seconds_of_life) ? (docid_status->counter - 1) : docid_status->counter;

  if (waiting_count > 0)
    {
      sprintf (buffer, TtaGetMessage (AMAYA, AM_WAITING_REQUESTS), waiting_count);
      TtaSetStatus (docid, 1, buffer, NULL);
    }
}

/*----------------------------------------------------------------------
  PrintTerminateStatus 
  displays a message on the status bar indicating the result of the
  request.
  ----------------------------------------------------------------------*/
void PrintTerminateStatus (AHTReqContext *me, int status) 
{

  HTError           *error = (HTError *) NULL;
  HTError           *next_error;
  HTErrorElement     errorElement;
  HTList            *cur;
  char               msg_status[20];
  char              *server_status = NULL;
  char              *wc_tmp;

  if (status == 200)
    TtaSetStatus (me->docid, 1,  
                  TtaGetMessage (AMAYA, AM_REQUEST_SUCCEEDED),
                  me->status_urlName);
  else if (status == 201)
    TtaSetStatus (me->docid, 1, 
                  TtaGetMessage (AMAYA, AM_CREATED_NEW_REMOTE_RESSOURCE),
                  me->status_urlName);
  else if (status == 204 && me->method == METHOD_PUT)
    TtaSetStatus (me->docid, 1,
                  TtaGetMessage (AMAYA, AM_UPDATED_REMOTE_RESSOURCE),
                  me->status_urlName);
  else if (status == 204 && me->method == METHOD_PUT)
    TtaSetStatus (me->docid, 1, 
                  TtaGetMessage (AMAYA, AM_NO_DATA), 
                  NULL);
  else if (status == -400 || status == 505)
    {
      strcpy (AmayaLastHTTPErrorMsg, 
              TtaGetMessage (AMAYA, AM_SERVER_DID_NOT_UNDERSTAND_REQ_SYNTAX));
      TtaSetStatus (me->docid, 1, AmayaLastHTTPErrorMsg, NULL);
    }
  else if (status == -401)
    {
      sprintf (AmayaLastHTTPErrorMsg, 
               TtaGetMessage (AMAYA, AM_AUTHENTICATION_FAILURE),
               me->status_urlName);
      TtaSetStatus (me->docid, 1, AmayaLastHTTPErrorMsg, NULL);
    }
  else if (status == -403)
    {
      sprintf (AmayaLastHTTPErrorMsg, 
               TtaGetMessage (AMAYA, AM_FORBIDDEN_ACCESS), 
               me->urlName);
      TtaSetStatus (me->docid, 1, AmayaLastHTTPErrorMsg, NULL);
    }
  else if (status == -405)
    {
      strcpy (AmayaLastHTTPErrorMsg, 
              TtaGetMessage (AMAYA, AM_METHOD_NOT_ALLOWED));
      TtaSetStatus (me->docid, 1, AmayaLastHTTPErrorMsg, NULL);
    }
  else if (status == -409)
    {
      strcpy (AmayaLastHTTPErrorMsg, TtaGetMessage (AMAYA, AM_CONFLICT));
      TtaSetStatus (me->docid, 1, AmayaLastHTTPErrorMsg, NULL);
    }
  else if (status == -902)
    {
      strcpy (AmayaLastHTTPErrorMsg, TtaGetMessage (AMAYA, AM_LOAD_ABORT));
      TtaSetStatus (me->docid, 1, AmayaLastHTTPErrorMsg, NULL);
    }
#ifdef DAV
  else if (status == -423 || status == 207 || status == -424 ) 
    {
      /* WebDAV status - there are filters that handle this 
       * status codes. We don't need to deal with this here. 
       * change status value to prevent any message to the user.
       */	  
#ifdef DEBUG_DAV
      fprintf (stderr, "PrintTerminateStatus.... WebDAV code %d\n",status); 
#endif	 
      status =  200;
    }
#endif  /* DAV */ 
  else if (status < 0)
    {
      /*
      ** Here we deal with errors for which libwww does not
      ** return a correct status code 
      */

#ifdef DEBUG_DAV
      fprintf (stderr, "PrintTerminateStatus.... unknown code %d\n",status); 
#endif	  
       
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
          if (error->par && error->length < MAX_LENGTH/2 && ((char *) error->par)[0] != EOS)
            server_status = (char *) error->par;
        }

      if (error == (HTError *) NULL)
        /* there's no error context */
        {
          sprintf (msg_status, "%d", status);
          if (status == -905)
            strcpy (AmayaLastHTTPErrorMsg, TtaGetMessage (AMAYA, AM_CANT_CONNECT_TO_HOST));
          else if (status == -906)
            strcpy (AmayaLastHTTPErrorMsg, TtaGetMessage (AMAYA, AM_NO_HOST));
          else if (status == -904)
            strcpy (AmayaLastHTTPErrorMsg, "Recover pipe line");
          else
            sprintf (AmayaLastHTTPErrorMsg, TtaGetMessage (AMAYA, AM_UNKNOWN_XXX_STATUS), msg_status);
            
          TtaSetStatus (me->docid, 1, AmayaLastHTTPErrorMsg, NULL);
          return;
        }
      else
        errorElement = error->element;	

      if (errorElement == HTERR_INTERRUPTED)
        {
          strcpy (AmayaLastHTTPErrorMsg, TtaGetMessage (AMAYA, AM_LOAD_ABORT));
          TtaSetStatus (me->docid, 1, AmayaLastHTTPErrorMsg, NULL);
        }
      else if (errorElement == HTERR_PRECON_FAILED) 
        {
          strcpy (AmayaLastHTTPErrorMsg, "Document has changed (412)");
          TtaSetStatus (me->docid, 1, AmayaLastHTTPErrorMsg, NULL);
          status = -412;
        }
      else if (errorElement == HTERR_TIME_OUT || errorElement == HTERR_TIMEOUT)
        {
          strcpy (AmayaLastHTTPErrorMsg, TtaGetMessage (AMAYA, AM_REQUEST_TIMEOUT));
          TtaSetStatus (me->docid, 1, AmayaLastHTTPErrorMsg, NULL);
        }
      else if (errorElement == HTERR_NOT_IMPLEMENTED)
        {
          strcpy (AmayaLastHTTPErrorMsg, 
                  TtaGetMessage (AMAYA, AM_SERVER_NOT_IMPLEMENTED_501_ERROR));
          TtaSetStatus (me->docid, 1, AmayaLastHTTPErrorMsg, NULL);
          status = -501;
        }
      else if (errorElement == HTERR_NO_REMOTE_HOST)
        {
          sprintf (AmayaLastHTTPErrorMsg,  "%s: %s",
                   TtaGetMessage (AMAYA, AM_NO_HOST), (char *) error->par);
          TtaSetStatus (me->docid, 1, AmayaLastHTTPErrorMsg, NULL);
        }
      else if (errorElement == HTERR_INTERNAL)
        {
          if ((error->length > 0) && (error->length <= 25) &&
              (error->par) && (((char *) error->par)[0] != EOS)) 
            {
              sprintf (AmayaLastHTTPErrorMsg, 
                       TtaGetMessage (AMAYA, AM_SERVER_INTERNAL_ERROR_500_CAUSE), 
                       (char *) error->par);
              TtaSetStatus (me->docid, 1, AmayaLastHTTPErrorMsg, NULL);
            }
          else
            {
               sprintf (AmayaLastHTTPErrorMsg, 
                       TtaGetMessage (AMAYA, AM_SERVER_INTERNAL_ERROR_500_CAUSE), "?");
               TtaSetStatus (me->docid, 1, AmayaLastHTTPErrorMsg, NULL);
            }
          status = -500; 
        }
      else if (server_status)
        {
          wc_tmp = TtaStrdup (server_status);
          /* let's output whatever the server or libwww reports as an error */
          TtaSetStatus (me->docid, 1, wc_tmp, NULL);
          strcpy (AmayaLastHTTPErrorMsg, wc_tmp);
          TtaFreeMemory (wc_tmp);
        }
      else
        {
          /* we don't have anything else, except for the status code */
          sprintf (msg_status, "%d", status);
          if (status == -905)
            strcpy (AmayaLastHTTPErrorMsg, TtaGetMessage (AMAYA, AM_CANT_CONNECT_TO_HOST));
          else if (status == -906)
            strcpy (AmayaLastHTTPErrorMsg, TtaGetMessage (AMAYA, AM_NO_HOST));
          else if (status == -904)
            strcpy (AmayaLastHTTPErrorMsg, "Recover pipe line");
          else
            sprintf (AmayaLastHTTPErrorMsg, TtaGetMessage (AMAYA, AM_UNKNOWN_XXX_STATUS),
                     msg_status);
            
          TtaSetStatus (me->docid, 1, AmayaLastHTTPErrorMsg, NULL);
        }
    }
#ifdef IV
  /* set the reason string */
  if (status < 0 && status != -902 )
    {
  HTResponse        *response;
      if (me->http_headers.reason && *me->http_headers.reason)
        sprintf (AmayaLastHTTPErrorMsgR, TtaGetMessage (AMAYA,AM_SERVER_REASON), 
                 me->http_headers.reason);
      else
        {
          response = HTRequest_response (me->request);
          if (response)
            {
              server_status = HTResponse_reason (response);
              if (server_status && *server_status)
                {
                  wc_tmp = TtaStrdup (server_status);
                  sprintf (AmayaLastHTTPErrorMsgR, TtaGetMessage (AMAYA,AM_SERVER_REASON), 
                           wc_tmp);
                  TtaFreeMemory (wc_tmp);
                }
            }
        }
    }
#endif

#ifdef LC
#ifdef SSL
  HTUnSetSocketBufSize ();
#endif /* SSL */
#endif /* LC */

}
