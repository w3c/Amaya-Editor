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
#ifdef __STDC__
 BOOL         AHTProgress (HTRequest * request, HTAlertOpcode op, int msgnum, const char *dfault,
				 void *input, HTAlertPar * reply)
#else  /* __STDC__ */
 BOOL         AHTProgress (request, op, msgnum, dfault, input, reply)
HTRequest          *request;
HTAlertOpcode       op;
int                 msgnum;
const char         *dfault;
void               *input, HTAlertPar * reply;

#endif
{
   AHTReqContext      *me = HTRequest_context (request);
   char                tempbuf[MAX_LENGTH];
   char                buf[11];
   long                cl, bytes_rw;
   int                 pro;

   if (request && HTRequest_internal (request))
      return NO;

   if (!me)
      return NO;

   switch (op)
	 {
	    case HT_PROG_DNS:
	       TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_LOOKING_HOST), (char *) input);
	       break;
	    case HT_PROG_CONNECT:
	       TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_CONTACTING_HOST), (char *) input);
	    case HT_PROG_ACCEPT:
	       TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_WAITING_FOR_CONNECTION), NULL);
	       break;
	    case HT_PROG_READ:
	       if ((me->method != METHOD_PUT) && (me->method != METHOD_POST))
		 {
		   cl = HTAnchor_length (HTRequest_anchor (request));
		   
		   if (cl > 0)
		     {
		       bytes_rw = HTRequest_bytesRead (request);
		       pro = (int) ((bytes_rw * 100l) / cl);
		       
		       if (pro > 100)		/* libwww reports > 100! */
			 pro = 100;
		       /*
		       ** prepare the string that will be displayed on
		       ** the status bar
		       */
		       
		       HTNumToStr ((unsigned long) cl, buf, 10);
		       sprintf (tempbuf, "%s (%d%% of %s)\n", me->status_urlName, (int) pro, buf);
		       TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_PROG_READ), tempbuf);
		     }
		   else
		     TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_PROG_READ), me->status_urlName);
		 }
	       break;

	    case HT_PROG_WRITE:
	       if ((me->method == METHOD_PUT) || (me->method == METHOD_POST))
		 {
		   /*experimental code to display the write progress */
		   /*it still does not work (libwww bug) */
#if 0
		    HTParentAnchor     *anchor = HTRequest_anchor (request);
		    cl = HTAnchor_length (anchor);

		    if (cl > 0)
		      {
			 bytes_rw = HTRequest_bytesWritten (request);
			 pro = (int) ((bytes_rw * 100l) / cl);

			 HTNumToStr ((unsigned long) cl, buf, 10);
			 sprintf (tempbuf, "%s: Writing (%d%% of %s)\n", me->urlName, pro, buf);
			 TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_PROG_WRITE), tempbuf);
		      }
		    else
#endif
		       TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_PROG_WRITE), me->status_urlName);
		 }
	       break;

	    case HT_PROG_DONE:
	       /* a message is displayed elsewhere */
	       break;
	    case HT_PROG_WAIT:
	       TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_WAITING_FOR_SOCKET), NULL);
	       break;
	    case HT_PROG_GC:
	       TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_CACHE_GC), NULL);
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
 BOOL         AHTConfirm (HTRequest * request, HTAlertOpcode op, int msgnum, const char *dfault,
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

   /* Wait an answer */
   UserAnswer = TRUE;
   TtaNewLabel (BaseDialog + ConfirmText, BaseDialog + ConfirmForm, TtaGetMessage (AMAYA, AM_GET_USER_NAME + msgnum));
   TtaShowDialogue (BaseDialog + ConfirmForm, FALSE);
   TtaWaitShowDialogue ();
   if (UserAnswer)
     {
	UserAnswer = FALSE;
	return (TRUE);
     }
   else
      return (FALSE);
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
   char               *result = NULL;

   TtaNewLabel (BaseDialog + Label1, BaseDialog + FormAnswer, TtaGetMessage (AMAYA, AM_GET_USER_NAME + msgnum));

   if (input)
      TtaNewLabel (BaseDialog + Label1, BaseDialog + FormAnswer, (char *) input);

   if (dfault)
      TtaNewLabel (BaseDialog + Label1, BaseDialog + FormAnswer, (char *) dfault);

   if (reply && msgnum >= 0)
     {
	TtaDetachForm (BaseDialog + NameText);
	TtaDetachForm (BaseDialog + PasswordText);
	TtaSetTextForm (BaseDialog + AnswerText, "");
	TtaShowDialogue (BaseDialog + FormAnswer, FALSE);
	TtaWaitShowDialogue ();
	/* come back from dialogue */
	TtaAttachForm (BaseDialog + NameText);
	TtaAttachForm (BaseDialog + PasswordText);

	/* give back the reply to the libwww */
	if (*Answer_name)
	  {
	     StrAllocCopy (result, Answer_name);
	     HTAlert_setReplyMessage (reply, result);
	     return YES;
	  }
     }
   return NO;
}


/*----------------------------------------------------------------------
  AHTPromptPassword
  prompts for password without echoing the reply.			
  ----------------------------------------------------------------------*/
#ifdef __STDC__
 BOOL         AHTPromptPassword (HTRequest * request, HTAlertOpcode op, int msgnum, const char *dfault,
				       void *input, HTAlertPar * reply)
#else  /* __STDC__ */
 BOOL         AHTPromptPassword (request, op, msgnum, dfault, input, reply)
HTRequest          *request;
HTAlertOpcode       op;
int                 msgnum;
const char         *dfault;
void               *input;
HTAlertPar         *reply;

#endif
{
   char               *result = NULL;

   if (reply && msgnum >= 0)
     {
	/* initialise */
	Lg_password = 0;
	Answer_password[0] = EOS;
	Display_password[0] = EOS;
	TtaNewLabel (BaseDialog + Label1, BaseDialog + FormAnswer, TtaGetMessage (AMAYA, AM_GET_USER_NAME + msgnum));
	TtaDetachForm (BaseDialog + AnswerText);
	TtaDetachForm (BaseDialog + NameText);
	TtaSetTextForm (BaseDialog + PasswordText, "");
	TtaShowDialogue (BaseDialog + FormAnswer, FALSE);
	TtaWaitShowDialogue ();
	/* come back from dialogue */
	TtaAttachForm (BaseDialog + AnswerText);
	TtaAttachForm (BaseDialog + NameText);
	if (*Answer_password)
	  {
	     StrAllocCopy (result, Answer_password);
	     HTAlert_setReplySecret (reply, result);
	     return YES;
	  }
     }
   return NO;
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
 BOOL         AHTPromptUsernameAndPassword (HTRequest * request, HTAlertOpcode op, int msgnum,
			const char *dfault, void *input, HTAlertPar * reply)
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
   char               *username = NULL;
   char               *password = NULL;
   AHTReqContext      *me = HTRequest_context (request);
   
   /* Update the status bar */
     TtaSetStatus (me->docid, 1,
		   TtaGetMessage (AMAYA, AM_PLEASE_AUTHENTICATE), me->urlName);

   /* initialise */
   Answer_name[0] = EOS;
   Lg_password = 0;
   Answer_password[0] = EOS;

   InitFormAnswer (me->docid, 1);

   /* handle the user's answers back to the library */
   if (Answer_name[0] != EOS)
     {
	StrAllocCopy (username, Answer_name);
	HTAlert_setReplyMessage (reply, username);

	if (Answer_password[0] != EOS)
	  {
	     /* give password back to the request */
	     StrAllocCopy (password, Answer_password);
	     HTAlert_setReplySecret (reply, password);
	     return YES;
	  }
     }
   return NO;
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
BOOL                AHTError_print (HTRequest * request, HTAlertOpcode op, int msgnum, const char *dfault,
				    void *input, HTAlertPar * reply)
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
			 TtaGetMessage (AMAYA, AM_AUTHENTICATION_FAILURE), me->urlName);
	   break;
	 case HTERR_FORBIDDEN:
	   TtaSetStatus (me->docid, 1,
			 TtaGetMessage (AMAYA, AM_FORBIDDEN_ACCESS), me->urlName);
	   break;
	 case HTERR_SYSTEM:
	   if (!strcmp ("connect",  HTError_location (pres)))
	     TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_CANT_CONNECT_TO_HOST), (char *) NULL);
	   else
	     TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_UNKNOWN_SAVE_ERROR), me->urlName);
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

   while ((pres = (HTError *) HTList_nextObject (cur)))
     {
	index = HTError_index (pres);
	/*	if (HTError_doShow (pres)) */
	  {			/* Error number */
	     switch (index)
		   {
		      case HTERR_SYSTEM:
		      case HTERR_INTERNAL:
			 if (pres->par != NULL)
			   {
			     if (me->method != METHOD_PUT) 
			       {
				 sprintf (buffer, TtaGetMessage (AMAYA, AM_SYS_ERROR_TMPL), me->urlName, me->urlName, pres->element, (char *) pres->par);
				 StrAllocCat (me->error_stream, buffer);
			       }
			     else
			       {
				 sprintf (buffer, "Error: Server is unavaiable or doesn't exist");
				 StrAllocCat (me->error_stream, buffer);
			       }
			   }
			 return;
			 break;
		      default:
			if (pres->par != NULL)
			   {
			     StrAllocCat (me->error_stream, pres->par);
			   }
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
   char                buffer[120];

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


/*
  end of Module answer.c
*/

#endif
