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

/* defines to include elsewhere
*********************************/

#ifdef _WINDOWS
#include <string.h>
#endif /* !_WINDOWS */
#define AMAYA_WWW_CACHE
#define AMAYA_LOST_UPDATE

#ifdef _WINDOWS
#      define CACHE_DIR_NAME TEXT("\\libwww-cache\\")
#else
#      define CACHE_DIR_NAME TEXT("/libwww-cache/")
#endif

/* libwww default parameters */
#define DEFAULT_CACHE_SIZE 10
#define DEFAULT_MAX_CACHE_ENTRY_SIZE 3
#define DEFAULT_MAX_SOCKET 32
#define DEFAULT_DNS_TIMEOUT 1800L
#define DEFAULT_PERSIST_TIMEOUT 60L
#define DEFAULT_NET_EVENT_TIMEOUT 60000
/* defines the priority for image content negotiation */
#define IMAGE_ACCEPT_NEGOTIATION "*/*;q=0.1,image/*,image/gif,image/jpeg,image/png"

/* Amaya includes  */
#define THOT_EXPORT extern
#include "amaya.h"
#include "init_f.h"
#include <sys/types.h>
#include <fcntl.h>
#include "HTEvtLst.h"
#include "HTAABrow.h"
#include "ustring.h"

#if defined(__svr4__) || defined (_AIX)
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

/* Type definitions and global variables etc. local to this module */

/*----------------------------------------------------------------------*/

/*** private variables ***/

static HTList      *converters = NULL;	/* List of global converters */
static HTList      *acceptTypes = NULL; /* List of types for the Accept header */
static HTList      *acceptLanguages = NULL; /* List of language priorities for the Accept header */
static HTList      *transfer_encodings = NULL;
static HTList      *content_encodings = NULL;
/* List of domains where we can do a safe PUT redirect */
static HTList      *safeput_list = NULL;

static int          object_counter = 0;	/* loaded objects counter */
static  ThotBool    AmayaAlive_flag; /* set to 1 if the application is active;
					0 if we have killed */
static  ThotBool    CanDoStop_flag; /* set to 1 if we can do a stop, 0
				       if we're inside a critical section */
static  ThotBool    UserAborted_flag; /* set to 1 if we're processing a user
					 induced stop request operation */
#ifdef  AMAYA_WWW_CACHE
static int          fd_cachelock; /* open handle to the .lock cache file */
#endif /* AMAYA_WWW_CACHE */

#include "answer_f.h"
#include "query_f.h"
#include "AHTURLTools_f.h"
#include "AHTBridge_f.h"
#include "AHTMemConv_f.h"
#include "AHTFWrite_f.h"

/* prototypes */

#ifdef __STDC__
static void RecCleanCache (CHAR_T* dirname);
#ifdef _WINDOWS
int WIN_Activate_Request (HTRequest* , HTAlertOpcode, int, const char*, void*, HTAlertPar*);
#endif /* _WINDOWS */
static void SafePut_init (void);
static void SafePut_delete (void);
static ThotBool SafePut_query (CHAR_T* url);

#else
static void RecCleanCache (/* char *dirname */);
#ifdef _WINDOWS
int WIN_Activate_Request (/* HTRequest* , HTAlertOpcode, int, const char*, void*, HTAlertPar* */);
#endif /* _WINDOWS */
static void SafePut_init (/* void */);
static void SafePut_delete (/* void */);
static ThotBool SafePut_query (/* CHAR_T* url */);
#endif /* __STDC__ */


#ifdef AMAYA_WWW_CACHE
/***************************************************************
 lock functions, used to avoid concurrent use of the cache
 Mostly based on W. Richard Stevens' APUE book.
 ***************************************************************/
/*----------------------------------------------------------------------
  set_cachelock
  sets a write lock on filename. 
  Returns -1 in case of failure, otherwise, it'll open a handle on
  filename and fd_cachelock will take its value.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int set_cachelock (CHAR_T* filename)
#else        
static int set_cachelock (filename)
CharUniut* filename;
#endif /* __STDC__ */
{
  int status;
#ifdef _WINDOWS

  status = TtaFileExist (filename);
  return ((status) ? 0 : -1);
#else
  struct flock lock;
 
  lock.l_type = F_WRLCK;
  lock.l_start = 0;
  lock.l_whence = SEEK_SET;
  lock.l_len = 0;
  
  fd_cachelock = open (filename, O_WRONLY);
  
  if (fd_cachelock == -1)
    status = -1;
  else
    status = fcntl(fd_cachelock, F_SETLK, &lock);
  
  if (status == -1)
    {
      if (fd_cachelock > 0)
	  close (fd_cachelock);
      fd_cachelock = 0;
    }
  return (status);
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
  clear_cachelock
  remove the write lock set on a filename.
  It'll close the fd handle on filename and reset *fd to 0.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int clear_cachelock (void)
#else        
static int clear_cachelock ()
int *fd;
#endif /* __STDC__ */
{
#ifdef _WINDOWS
  return 0;
#else
  int status;

  if (!fd_cachelock)
    return (-1);
 
  status = close (fd_cachelock);
  fd_cachelock = 0;

  return (status);
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
  test_cachelock
  returns 0 if a fd is not locked by other process, otherwise 
  returns the pid of the process who has the lock
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int test_cachelock (CHAR_T* filename)
#else
static int test_cachelock (filename)
CHAR_T*  filename;
#endif /* __STDC__ */
{
#ifdef _WINDOWS
  /* if the lock is set, we can't unlink the file under Windows */
  if (!TtaFileUnlink (filename))
    return 0;
  else
    return -1;
#else
  struct flock lock;
  int fd, status;

  lock.l_type = F_WRLCK;
  lock.l_start = 0;
  lock.l_whence = SEEK_SET;
  lock.l_len = 0;

  fd = open (filename, O_WRONLY);

  if (fd < 0)
    return (-1);
  /* is this file locked ? */
  status = fcntl (fd,  F_GETLK, &lock);
  close (fd);

  if (status < 0)
    return (-1);

  if (lock.l_type == F_UNLCK)
    return (0); /* false, region is not locked by another proc */
  return (lock.l_pid); /* true, return pid of lock owner */
#endif /* _WINDOWS */
}

#endif /* AMAYA_WWW_CACHE */

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
static  HTAtom *AHTGuessAtom_for (STRING urlName, PicType contentType)
#else
static  HTAtom *AHTGuessAtom_for (urlName, contentType)
STRING  urlName;
PicType contentType;
#endif
{
 HTAtom           *atom;
 CHAR_T*           filename;
 HTEncoding        enc = NULL;
 HTEncoding        cte = NULL;
 HTLanguage        lang = NULL;
 double            quality = 1.0;
 char              FName[MAX_LENGTH];

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
     filename = AmayaParseUrl (urlName, TEXT(""), AMAYA_PARSE_PATH | AMAYA_PARSE_PUNCTUATION);
     wc2iso_strcpy (FName, filename);
     HTBind_getFormat (FName, &atom, &enc, &cte, &lang, &quality);
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
  HTTP_Headers_set
  Copies the headers in which the application is interested, doing
  any in-between conversions as needed.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void HTTP_headers_set (HTRequest * request, HTResponse * response, void *context, int status)
#else
void HTTP_headers_set (request, response, context, status)
HTRequest          *request;
HTResponse         *response;
void               *context;
int                 status;
#endif
{
  AHTReqContext      *me = (AHTReqContext *) HTRequest_context (request);
  HTAtom *tmp_atom;
  char *content_type;
  CHAR_T tmp_wchar[MAX_LENGTH];
  HTParentAnchor *anchor = HTRequest_anchor (request);

  /* @@@ later I'll add a function here to specify which headers we
     want to copy */

  /* we have already forced a content type (what about a charset? */
  if (me->http_headers.content_type)
    return;

   /* copy the content_type */
  tmp_atom = HTAnchor_format (anchor);
  if (tmp_atom)
    content_type = HTAtom_name (tmp_atom);
  else
    content_type = "www/unknown";
  
  if (content_type && content_type [0] != EOS)
    {
      /* libwww gives www/unknown when it gets an error. As this is 
	 an HTML test, we force the type to text/html */
      if (!strcmp (content_type, "www/unknown"))
	me->http_headers.content_type = TtaWCSdup (TEXT("text/html"));
      else 
	{
	  iso2wc_strcpy (tmp_wchar, content_type);
	  me->http_headers.content_type = TtaWCSdup (tmp_wchar);
	}
      
#ifdef DEBUG_LIBWWW
      fprintf (stderr, "content type is: %s\n", me->http_headers.content_type);
#endif /* DEBUG_LIBWWW */
    }
  
  /* copy the charset */
  tmp_atom = HTAnchor_charset (anchor);
  if (tmp_atom)
    {
      iso2wc_strcpy (tmp_wchar, HTAtom_name (tmp_atom));
      me->http_headers.charset = TtaWCSdup (tmp_wchar);
    }
}

/*----------------------------------------------------------------------
  HTTP_headers_delete
  Deletes all the paramaters that were assigned to the response type
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void HTTP_headers_delete (AHTHeaders me)
#else
static void HTTP_headers_delete (me)
AHTHeaders me;
#endif
{
  if (me.content_type)
    TtaFreeMemory (me.content_type);

  if (me.charset)
    TtaFreeMemory (me.charset);
}

/*----------------------------------------------------------------------
  HTTP_headers
  Returns the value of a parameter in the HTTP response structure.
  Returns null if this structure is empty.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
CHAR_T *HTTP_headers (AHTHeaders *me, AHTHeaderName param)
#else
CHAR_T *HTTP_headers (me, param)
AHTHeaders *me;
AHTHeaderName param;
#endif
{
  CHAR_T *result;

  if (!me)
    return NULL;
  
  switch (param)
    {
    case AM_HTTP_CONTENT_TYPE:
      result = me->content_type;
      break;
    case AM_HTTP_CHARSET:
      result = me->charset;
      break;
    default:
      result = NULL;
    }
  
  return result;
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
     outofmem (__FILE__, "AHTReqContext_new");

   /* clear the structure */
   memset ((void *) me, 0, sizeof (AHTReqContext));

   /* Bind the Context object together with the Request Object */
   me->request = HTRequest_new ();
  
   /* clean the associated file structure) */
   HTRequest_setOutputStream (me->request, NULL);
 
   /* Initialize the other members of the structure */
   me->reqStatus = HT_NEW; /* initial status of a request */
   me->docid = docid;
   HTRequest_setMethod (me->request, METHOD_GET);
   HTRequest_setOutputFormat (me->request, WWW_SOURCE);
   HTRequest_setContext (me->request, me);

   /* experimental */
   me->read_sock = INVSOC;
   me->write_sock = INVSOC;
   me->except_sock = INVSOC;

   /* Update the global context */
   HTList_appendObject (Amaya->reqlist, (void *) me);

   docid_status = GetDocIdStatus (docid, Amaya->docid_status);

   if (docid_status == NULL)
     {
	docid_status = (AHTDocId_Status *) 
	  TtaGetMemory (sizeof (AHTDocId_Status));
	docid_status->docid = docid;
	docid_status->counter = 1;
	HTList_addObject (Amaya->docid_status, (void *) docid_status);
     }
   else
      docid_status->counter++;

   Amaya->open_requests++;

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
ThotBool  AHTReqContext_delete (AHTReqContext * me)
#else
ThotBool  AHTReqContext_delete (me)
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

       HTNoFreeStream_delete (HTRequest_outputStream (me->request));
       HTRequest_setOutputStream (me->request, NULL); 
       /* JK: no longer needed in libwww 5.2.3
	  if (me->method != METHOD_PUT && HTRequest_outputStream (me->request))
	  AHTFWriter_FREE (HTRequest_outputStream (me->request));
       */
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
#ifndef _WINDOWS
#ifdef WWW_XWINDOWS	
       if (me->read_xtinput_id || me->write_xtinput_id ||
	   me->except_xtinput_id)
	 RequestKillAllXtevents(me);
#endif /* WWW_XWINDOWS */
#endif /* !_WINDOWS */
       
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
	 }

       /* the real name to which we are publishing */
       if (me->default_put_name)
	 TtaFreeMemory (me->default_put_name);

       /* @@ temp change for the %esc conversion */
       if (me->method == METHOD_PUT && me->urlName)
	 TtaFreeMemory (me->urlName);

       if (me->formdata)
	 HTAssocList_delete (me->formdata);

       /* erase the response headers */
       HTTP_headers_delete (me->http_headers);
       
#ifdef ANNOTATIONS
       if (me->document)
	 TtaFreeMemory (me->document);
#endif /* ANNOTATIONS */

       /* to trace bugs */
       memset ((void *) me, 0, sizeof (AHTReqContext));
       
       TtaFreeMemory ((void *) me);
       
       Amaya->open_requests--;
       
       return TRUE;
     }
   return FALSE;
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
#ifndef _WINDOWS
#ifdef WWW_XWINDOWS 
		  RequestKillAllXtevents (me);
#endif /* WWW_XWINDOWS */
#endif /* !_WINDOWS */
		  if (!HTRequest_kill (me->request))
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
  fprintf(stderr, "AHTOpen_file: start for object : %p\n", me);
#endif /* DEBUG_LIBWWW */

  if (me->reqStatus == HT_ABORT) 
    {
#ifdef DEBUG_LIBWWW
      fprintf(stderr, "AHTOpen_file: caught an abort request, skipping it\n");
#endif /* DEBUG_LIBWWW */

      return HT_OK;
    }

  if (me->method == METHOD_PUT)
    {
      me->reqStatus = HT_WAITING;
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
      (me->output = ufopen (me->outputfile, TEXT("w"))) == NULL)
    {
#else /* !_WINDOWS */
    (me->output = ufopen (me->outputfile, TEXT("wb"))) == NULL)  
    {
#endif /* !_WINDOWS */

      me->outputfile[0] = EOS;	/* file could not be opened */
#ifdef DEBUG_LIBWWW
      fprintf(stderr, "AHTOpen_file: couldn't open output stream for url %s\n", me->urlName);
#endif
      TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_CANNOT_CREATE_FILE), me->outputfile);
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
   CHAR_T*             ref;
   CHAR_T*             escape_src, *dst;
   CHAR_T              urlAdr[MAX_LENGTH];
   char                urlRef[MAX_LENGTH];

   if (!me) /* if the redirect doesn't come from Amaya, we call libwww's standard redirect filter */
      return (HTRedirectFilter (request, response, param, status));

   if (!new_anchor) {
      if (PROT_TRACE)
         HTTrace ("Redirection. No destination\n");
      return HT_OK;
   }

   /*
   ** Only do redirect on GET, HEAD, and authorized domains for PUT
    */
   if ((me->method == METHOD_PUT && !SafePut_query (me->urlName)) || (me->method != METHOD_PUT && !HTMethod_isSafe (method))) {
      /*
      ** If we got a 303 See Other then change the method to GET.
      ** Otherwise ask the user whether we should continue.
      */
      if (status == HT_SEE_OTHER) {
         if (PROT_TRACE)
            HTTrace("Redirection. Changing method from %s to GET\n", HTMethod_name(method));
            HTRequest_setMethod(request, METHOD_GET);
	  } else {
             HTAlertCallback    *prompt = HTAlert_find (HT_A_CONFIRM);
             if (prompt) {
                if ((*prompt) (request, HT_A_CONFIRM, HT_MSG_REDIRECTION, NULL, NULL, NULL) != YES)
		  {
		    /* the user didn't agree on the redirection, so
		       we consider it's an abort */
		    me->reqStatus = HT_ABORT;
		    /* and we return HT_OK so that the terminate_handler
		       will be called */
		    return HT_OK;
		  }
	     } 
	  } 
   } 
   /*
    **  Start new request with the redirect anchor found in the headers.
    **  Note that we reuse the same request object which means that we must
    **  keep this around until the redirected request has terminated. It also           
    **  allows us in an easy way to keep track of the number of redirections
    **  so that we can detect endless loops.
    */

   if (HTRequest_doRetry (request)) {
      /*
      ** Start request with new credentials 
      */
      /* only do a redirect using a network protocol understood by Amaya */
      iso2wc_strcpy (urlAdr, new_anchor->parent->address);
      if (IsValidProtocol (urlAdr)) {
         /* if it's a valid URL, we try to normalize it */
         /* We use the pre-redirection anchor as a base name */
         /* @@ how to obtain this address here? */
         dst = urlAdr;
         escape_src = EscapeURL (me->urlName);
         if (escape_src) {
            ref = AmayaParseUrl (dst, escape_src, AMAYA_PARSE_ALL);
            wc2iso_strcpy (urlRef, ref);
            TtaFreeMemory (escape_src);
		 } else
               ref = NULL;

         if (ref) {
            HTAnchor_setPhysical (HTAnchor_parent (new_anchor), urlRef);
            TtaFreeMemory (ref);
		 } else
               return HT_OK; /* We can't redirect anymore */
	  } else
            return HT_OK; /* We can't redirect anymore */

      /* update the current file name */
      if ((me->mode & AMAYA_ASYNC) || (me->mode & AMAYA_IASYNC) || (me->method == METHOD_PUT)) {
         TtaFreeMemory (me->urlName);
         me->urlName = TtaWCSdup (urlAdr);
	  } else {
            /* it's a SYNC mode, so we should keep the urlName */
            ustrncpy (me->urlName, urlAdr, MAX_LENGTH - 1);
            me->urlName[MAX_LENGTH - 1] = EOS;
	  }
      ChopURL (me->status_urlName, me->urlName);

      /* @@ verify if this is important */
      /* @@@ new libwww doesn't need this free stream while making
         a PUT. Is it the case everywhere or just for PUT? */
      if (me->method != METHOD_PUT && me->request->orig_output_stream != NULL) {
         AHTFWriter_FREE (me->request->orig_output_stream);
         me->request->orig_output_stream = NULL;
         if (me->output != stdout) { /* Are we writing to a file? */
#           ifdef DEBUG_LIBWWW
            fprintf (stderr, "redirection_handler: New URL is  %s, closing FILE %p\n", me->urlName, me->output); 
#           endif 
            fclose (me->output);
            me->output = NULL;
		 }
	  }

      /* tell the user what we're doing */
      TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_RED_FETCHING), me->status_urlName); 
      /*
      ** launch the request
      */
      /* add a link relationship? */
      /* reset the request status */
      me->reqStatus = HT_NEW; 
      /* clear the errors */
      HTError_deleteAll (HTRequest_error (request));
      HTRequest_setError (request, NULL);
      /* clear the authentication credentials, as they get regenerated  */
      HTRequest_deleteCredentialsAll (request);
	
      if (me->method == METHOD_POST || me->method == METHOD_PUT) { /* PUT, POST etc. */
         char url_Name[MAX_LENGTH];
         wc2iso_strcpy (url_Name, me->urlName);
         status = HTLoadAbsolute (url_Name, request);
      } else
           HTLoadAnchor (new_anchor, request);
   } else {
          HTRequest_addError (request, ERR_FATAL, NO, HTERR_MAX_REDIRECT, NULL, 0, "HTRedirectFilter");
          /* so that we can show the error message */
          if (me->error_html)
             DocNetworkStatus[me->docid] |= AMAYA_NET_ERROR;
          me->reqStatus = HT_ERR;
          TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_REDIRECTIONS_LIMIT), NULL);
   }

   /*
   **  By returning HT_ERROR we make sure that this is the last handler to be
   **  called. We do this as we don't want any other filter to delete the 
   **  request object now when we have just started a new one ourselves
   */
   return HT_ERROR;
}

#ifdef AMAYA_LOST_UPDATE
/*----------------------------------------------------------------------
  precondition_handler
  412 "Precondition failed" handler
  ----------------------------------------------------------------------*/
#if __STDC__
static int          precondition_handler (HTRequest * request, HTResponse * response, void *context, int status)
#else
static int          precondition_handler (request, response, context, status)
HTRequest          *request;
HTResponse         *response;
void               *context;
int                 status;
#endif /* __STDC__ */
{
  AHTReqContext  *me = (AHTReqContext *) HTRequest_context (request);
  HTAlertCallback    *prompt = HTAlert_find (HT_A_CONFIRM);
  ThotBool force_put;

  if (!me)
    return HT_OK;		/* not an Amaya request */

  if (prompt)
       force_put = (*prompt) (request, HT_A_CONFIRM,  HT_MSG_RULES,
			      NULL, NULL, NULL);
  else
    force_put = NO;
  
  if (force_put)
    {
      /* start a new PUT request without preconditions */
      /* @@ do we need to kill the request? */
      if (me->output && me->output != stdout)
	{
	  fclose (me->output);
	  me->output = NULL;
	}
      /*
      ** reset the Amaya and libwww request status 
      */

      /* start with a new, clean request structure */
      HTRequest_delete (me->request);
      me->request = HTRequest_new ();
      /* clean the associated file structure) */
      HTRequest_setOutputStream (me->request, NULL);
      /* Initialize the other members of the structure */
      HTRequest_setMethod (me->request, METHOD_GET);
      HTRequest_setOutputFormat (me->request, WWW_SOURCE);
      HTRequest_setContext (me->request, me);
      HTRequest_setPreemptive (me->request, NO);
      /*
      ** Make sure that the first request is flushed immediately and not
      ** buffered in the output buffer
      */
      if (me->mode & AMAYA_FLUSH_REQUEST)
	HTRequest_setFlush(me->request, YES);

      /* turn off preconditions */
      HTRequest_setPreconditions(me->request, HT_NO_MATCH);
      me->reqStatus = HT_NEW; 
      /* make the request */      
      status = HTPutDocumentAnchor (HTAnchor_parent (me->source), 
				    me->dest, me->request);
      /* stop here */
      return HT_ERROR;
    }
  else
    {
      /* abort the request */
      /* @@ should we call StopRequest here? */
      me->reqStatus = HT_ABORT;
      /* stop here */
      return HT_ERROR; 
    }
}

/*----------------------------------------------------------------------
  check_handler
  Request HEAD checker filter
  ----------------------------------------------------------------------*/
static int check_handler (HTRequest * request, HTResponse * response,
                           void * param, int status)
{
  AHTReqContext  *me = (AHTReqContext *) HTRequest_context (request);
  HTAlertCallback    *prompt = HTAlert_find (HT_A_CONFIRM);
  ThotBool force_put;

  if (!me)
    return HT_OK;		/* not an Amaya request */

  HTRequest_deleteAfter(me->request, check_handler);
  /*
  ** If head request showed that the document doesn't exist
  ** then just go ahead and PUT it. Otherwise ask for help
  */
  if (status == HT_ERROR || status == HT_INTERRUPTED || status == HT_TIMEOUT)
    {
      /* we'd need to call terminate_handler, to free the resources */
      /* abort the request */
      /* @@ should we call StopRequest here? */
      me->reqStatus = HT_ABORT;
      /* stop here */
      return HT_ERROR; 
    } 
  else if (status == HT_NO_ACCESS || status == HT_NO_PROXY_ACCESS) 
    {
      /* we'd need to call terminate_handler, to free the resources */
      /* abort the request */
      /* @@ should we call StopRequest here? */
      me->reqStatus = HT_ABORT;
      /* stop here */
      return HT_ERROR; 
    }
  else if ( (abs(status)/100) != 2) 
    {
      /* start with a new, clean request structure */
      HTRequest_delete (me->request);
      me->request = HTRequest_new ();
      /* add the default PUT name */
      HTRequest_setDefaultPutName (me->request, me->default_put_name);
      /* clean the associated file structure) */
      HTRequest_setOutputStream (me->request, NULL);
      /* Initialize the other members of the structure */
      HTRequest_setMethod (me->request, METHOD_GET);
      HTRequest_setOutputFormat (me->request, WWW_SOURCE);
      HTRequest_setContext (me->request, me);
      HTRequest_setPreemptive (me->request, NO);
      /*
      ** Make sure that the first request is flushed immediately and not
      ** buffered in the output buffer
      */
      if (me->mode & AMAYA_FLUSH_REQUEST)
	HTRequest_setFlush(me->request, YES);

      /* turn on the special preconditions, to avoid having this
	 ressource appear before we do the PUT */
      HTRequest_setPreconditions(me->request, HT_DONT_MATCH_ANY);
      me->reqStatus = HT_NEW;
      status = HTPutDocumentAnchor (HTAnchor_parent (me->source), me->dest, me->request);
      return HT_ERROR; /* stop here */
    }
  else 
    {
      if (prompt)
	  force_put = (*prompt) (request, HT_A_CONFIRM, HT_MSG_FILE_REPLACE,
				 NULL, NULL, NULL);
      else
	force_put = FALSE;

      if (force_put)
	{
	  /* Start a new PUT request without preconditions */

	  /* get a new, clean request structure */
	  HTRequest_delete (me->request);
	  me->request = HTRequest_new ();
	  /* add the default PUT name */
	  HTRequest_setDefaultPutName (me->request, me->default_put_name);
	  /* clean the associated file structure) */
	  HTRequest_setOutputStream (me->request, NULL);
	  /* Initialize the other members of the structure */
	  HTRequest_setMethod (me->request, METHOD_GET);
	  HTRequest_setOutputFormat (me->request, WWW_SOURCE);
	  HTRequest_setContext (me->request, me);
	  HTRequest_setPreemptive (me->request, NO);
	  /*
	  ** Make sure that the first request is flushed immediately and not
	  ** buffered in the output buffer
	  */
	  if (me->mode & AMAYA_FLUSH_REQUEST)
	    HTRequest_setFlush(me->request, YES);
	  /* remove the preconditions */
	  HTRequest_setPreconditions(me->request, HT_NO_MATCH);
	  me->reqStatus = HT_NEW; 
	  status = HTPutDocumentAnchor (HTAnchor_parent (me->source), 
					me->dest, me->request);
	  return HT_ERROR; /* stop here */
	} 
      else
	{
	  /* we'd need to call terminate_handler, to free the resources */
	  /* abort the request */
	  /* @@ should we call StopRequest here? */
	  me->reqStatus = HT_ABORT;
	  /* stop here */
	  return HT_ERROR; 
	}
    }
  return HT_ERROR;
}
#endif /* AMAYA_LOST_UPDATE */

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
#endif /* __STDC__ */
{
  AHTReqContext      *me = (AHTReqContext *) HTRequest_context (request);
  ThotBool            error_flag;

  if (!me)
     return HT_ERROR;		/* not an Amaya request */

  if (me->reqStatus == HT_END)
    /* we have already processed this request! */
    return HT_OK;

  /* if Amaya was killed, treat with this request as if it were
     issued by a Stop button event */

   if (!AmayaIsAlive ())           
      me->reqStatus = HT_ABORT; 

   /* trying to protect against this problem... hard to place
      the detection elsewhere :-/ */
   /* @@ JK: Is this still needed? */
   if (IsHTTP09Error (request))
     status = -1;

   if (status == HT_LOADED
       || status == HT_CREATED
       || status == HT_NO_DATA
       /* kludge for work around libwww problem */
       || (status == HT_INTERRUPTED && me->method == METHOD_PUT)
#ifdef AMAYA_WWW_CACHE
       /* what status to use to know we're downloading from a cache? */
       || status ==  HT_NOT_MODIFIED
       || status == HT_PARTIAL_CONTENT
#endif /* AMAYA_WWW_CACHE */
       || me->reqStatus == HT_ABORT)
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
       /* we are writing to a local file */
       if (me->reqStatus != HT_ABORT)
	 {			/* if the request was not aborted and */
	   if (error_flag &&
	       me->error_html == TRUE)
	       /* there were some errors and we want to print them */
	     {		
	       if (me->error_stream_size == 0)/* and the stream is empty */
		 {
		   /* if the transfer was interrupted, the file may not be
		      empty. So, we erase it */
		   fflush (me->output);
		   rewind (me->output);
		   AHTError_MemPrint (request); /* copy errors from 
						**the error stack 
						** into the error stream */
		 }
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

   /* a very special handling so that we can put a message box
      to tell the user WHAT he has just done... */
   if (UserAborted_flag && me->method == METHOD_PUT)
     {
       HTRequest_addError (request, ERR_FATAL, NO, HTERR_INTERRUPTED,
			   "Operation aborted by user", 0, NULL);
     }

   if (error_flag)
     me->reqStatus = HT_ERR;
   else 
     if (me->reqStatus != HT_ABORT)
       me->reqStatus = HT_END;

   /* copy the headers in which the application is interested */
   HTTP_headers_set (request, response, context, status);

   /* to avoid a hangup while downloading css files */
   if (AmayaAlive_flag && (me->mode & AMAYA_LOAD_CSS))
     TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_ELEMENT_LOADED), me->status_urlName);
   
  /* don't remove or Xt will hang up during the PUT */
   if (AmayaIsAlive ()  && ((me->method == METHOD_POST) ||
			    (me->method == METHOD_PUT)))
     {
       PrintTerminateStatus (me, status);
     } 

   ProcessTerminateRequest (request, response, context, status);
   
   /* stop here */
   return HT_ERROR;
}

/*----------------------------------------------------------------------
  AHTLoadTerminate_handler
  this is an application "AFTER" Callback. It's called by the library
  when a request has ended, so that we can setup the correct status.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
int          AHTLoadTerminate_handler (HTRequest * request, HTResponse * response, void *param, int status)
#else
int          AHTLoadTerminate_handler (request, response, param, status)
HTRequest          *request;
HTResponse         *response;
void               *param;
int                 status;

#endif
{

  /** @@@@ use this with printstatus ?? */

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
           HTTrace ("Load End.... ERROR: Can't access `%s\'\n", me->status_urlName ? me->status_urlName :"<UNKNOWN>"); 
       TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_CANNOT_LOAD), me->status_urlName ? me->status_urlName : TEXT("<UNKNOWN>"));
       break;
     default:
       if (PROT_TRACE)
	 HTTrace ("Load End.... UNKNOWN RETURN CODE %d\n", status);
       break;
     }
   
   return HT_OK;
}

#ifdef DEBUG_LIBWWW
static  int LineTrace (const char * fmt, va_list pArgs)
{
    return (vfprintf(stderr, fmt, pArgs));
}
#endif DEBUG_LIBWWW

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

   HTConversion_add (c, "image/png",  "www/present", HTThroughLine, 1.0, 0.0, 0.0);
   HTConversion_add (c, "image/jpeg", "www/present", HTThroughLine, 1.0, 0.0, 0.0);
   HTConversion_add (c, "image/gif",  "www/present", HTThroughLine, 1.0, 0.0, 0.0);
   HTConversion_add (c, "image/xbm",  "www/present", HTThroughLine, 1.0, 0.0, 0.0);
   HTConversion_add (c, "image/xpm",  "www/present", HTThroughLine, 1.0, 0.0, 0.0);

   /* Define here the equivalences between MIME types and file extensions for
      the types that Amaya can display */

   /* Initialize suffix bindings for local files */
   HTBind_init();

   /* Register the default set of file suffix bindings */
   HTFileInit ();

   /* Don't do any case distinction */
   HTBind_caseSensitive (FALSE);
}

/*----------------------------------------------------------------------
  AHTAcceptLanguagesInit
  This function prepares the Accept header used by Amaya during
  the HTTP content negotiation phase
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         AHTAcceptLanguagesInit (HTList *c)
#else  /* __STDC__ */
static void         AHTAcceptLanguagesInit (c)
HTList             *c;
#endif /* __STDC__ */
{
  CHAR_T*         ptr;
  CHAR_T*         lang_list;
  char              s[3];
  int               count, lg;
  double            quality;
  ThotBool          still;

  if (c == (HTList *) NULL) 
      return;
  
  lang_list = TtaGetEnvString ("ACCEPT_LANGUAGES");
  s[2] = EOS;
  if (lang_list && *lang_list != WC_EOS)
    {
      /* add the default language first  */
      HTLanguage_add (c, "*", -1.0);
      /* how many languages do we have? */
      ptr = lang_list;
      count = 0;
      while (*ptr != WC_EOS) {
            while (*ptr != WC_EOS && (*ptr < TEXT('A') || (*ptr > TEXT('Z') && *ptr < TEXT('a')) || *ptr > TEXT('z')))
                  /* skip the whole separator */
                  ptr++;
            lg = 0;
            while ((*ptr >= TEXT('A') && *ptr <= TEXT('Z')) || (*ptr >= TEXT('a') && *ptr <= TEXT('z')) || *ptr == TEXT('-')) {
                  /* it's a new language */
                  ptr++;
                  lg++;
			}
            if (lg >= 2)
               count++;
            if (*ptr != WC_EOS)
               ptr++;
	  }

      if (count > 0)
         quality = 1.1 - (double) count/10.0;
      else
          quality = 1.0;

     /* Read the languages from the registry, then inject them one, by one.
        The first one is the one with the highest priority.
        The libwww ask for the lowest priority first.
      */
      ptr--;
      still = TRUE;
      while (count) {
            while (still && (*ptr < TEXT('A') || (*ptr > TEXT('Z') && *ptr < TEXT('a')) || *ptr > TEXT('z')))
                  /* skip the whole separator */
                  if (ptr > lang_list)
                     ptr--;
                  else
                      still = FALSE;
            lg = 0;
            while (still && ((*ptr >= TEXT('A') && *ptr <= TEXT('Z')) || (*ptr >= TEXT('a') && *ptr <= TEXT('z')) || *ptr == TEXT('-'))) {
                  /* it's a new language */
                  if (ptr > lang_list)
                     ptr--;
                  else
                      still = FALSE;
                  lg++;
			}
            if (lg >= 2) {
               if (still)
                  wc2iso_strncpy  (s, &ptr[1], 2);
               else
                   wc2iso_strncpy (s, lang_list, 2);
               count--;
               HTLanguage_add (c, s, quality);
               quality += 0.1;
			}
            ptr--;
	  }
  }
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
   HTConversion_add (c, "message/x-rfc822-foot", "*/*", HTMIMEFooter, 1.0, 0.0, 0.0);
   HTConversion_add (c, "message/x-rfc822-head", "*/*", HTMIMEHeader, 1.0, 0.0, 0.0);
   HTConversion_add(c,"message/x-rfc822-cont",	"*/*", HTMIMEContinue, 1.0, 0.0, 0.0);
   HTConversion_add(c,"message/x-rfc822-partial","*/*",	HTMIMEPartial, 1.0, 0.0, 0.0);
   HTConversion_add (c, "multipart/*", "*/*", HTBoundary, 1.0, 0.0, 0.0);
   HTConversion_add (c, "text/plain", "text/html", HTPlainToHTML, 1.0, 0.0, 0.0);

   /*
   ** The following conversions are converting ASCII output from various
   ** protocols to HTML objects.
   */
   HTConversion_add (c, "text/x-http", "*/*", HTTPStatus_new, 1.0, 0.0, 0.0);

   /*
   ** We also register a special content type guess stream that can figure out
   ** the content type by reading the first bytes of the stream
   */
   HTConversion_add (c, "www/unknown", "*/*", HTGuess_new, 1.0, 0.0, 0.0);

#ifdef AMAYA_WWW_CACHE
   /*
   ** Register a persistent cache stream which can save an object to local
   ** file
   */
   HTConversion_add (c, "www/cache", "*/*", HTCacheWriter, 1.0, 0.0, 0.0);
   HTConversion_add(c,"www/cache-append", "*/*", HTCacheAppend, 1.0, 0.0, 0.0);
#endif AMAYA_WWW_CACHE

   /*
   ** This dumps all other formats to local disk without any further
   ** action taken
   */
   HTConversion_add (c, "*/*", "www/present", HTSaveLocally, 0.3, 0.0, 0.0);  
}


/*----------------------------------------------------------------------
  AHTProtocolInit
  Registers all amaya supported protocols.
  ----------------------------------------------------------------------*/
static void         AHTProtocolInit (void)
{
  CHAR_T* strptr;

  /* 
     NB. Preemptive == YES means Blocking requests
     Non-preemptive == NO means Non-blocking requests
     */
  HTTransport_add("tcp", HT_TP_SINGLE, HTReader_new, HTWriter_new);
  HTTransport_add("buffered_tcp", HT_TP_SINGLE, HTReader_new, 
		  HTBufferWriter_new);
  HTProtocol_add ("http", "buffered_tcp", HTTP_PORT, NO, HTLoadHTTP, NULL);
#ifdef _WINDOWS
  HTProtocol_add ("file", "local", 0, YES, HTLoadFile, NULL);
#else
  HTProtocol_add ("file", "local", 0, NO, HTLoadFile, NULL);
#endif /* _WINDOWS */
#ifdef AMAYA_WWW_CACHE
   HTProtocol_add("cache",  "local", 0, YES, HTLoadCache, NULL);
#endif /* AMAYA_WWW_CACHE */
#if 0 /* experimental code */
   HTProtocol_add ("ftp", "tcp", FTP_PORT, NO, HTLoadFTP, NULL);
#endif

   /* initialize pipelining */
  strptr = TtaGetEnvString ("ENABLE_PIPELINING");
  if (strptr && *strptr && ustrcasecmp (strptr, TEXT("yes")))
    HTTP_setConnectionMode (HTTP_11_NO_PIPELINING);
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

  HTNet_addBefore (HTCredentialsFilter, "http://*", NULL, HT_FILTER_LATE);
  HTNet_addBefore (HTProxyFilter, NULL, NULL, HT_FILTER_LATE);
  HTHost_setActivateRequestCallback (AHTOpen_file);

/*      register AFTER filters
**      The AFTER filters handle error messages, logging, redirection,
**      authentication etc.
**      The filters are called in the order by which the are registered
**      Not done automaticly - may be done by application!
*/

  HTNet_addAfter (HTAuthFilter, "http://*", NULL, HT_NO_ACCESS, HT_FILTER_MIDDLE);
  HTNet_addAfter (HTAuthFilter, "http://*", NULL, HT_REAUTH, HT_FILTER_MIDDLE);
  HTNet_addAfter (redirection_handler, "http://*", NULL, HT_PERM_REDIRECT, HT_FILTER_MIDDLE);
  HTNet_addAfter (redirection_handler, "http://*", NULL, HT_FOUND, HT_FILTER_MIDDLE);
  HTNet_addAfter (redirection_handler, "http://*", NULL, HT_SEE_OTHER, HT_FILTER_MIDDLE);
  HTNet_addAfter (redirection_handler, "http://*", NULL, HT_TEMP_REDIRECT, HT_FILTER_MIDDLE);
  HTNet_addAfter (HTAuthInfoFilter, 	"http://*", NULL, HT_ALL, HT_FILTER_MIDDLE);
  HTNet_addAfter (HTUseProxyFilter, "http://*", NULL, HT_USE_PROXY, HT_FILTER_MIDDLE);
#ifdef AMAYA_LOST_UPDATE
  HTNet_addAfter (precondition_handler, NULL, NULL, HT_PRECONDITION_FAILED, HT_FILTER_MIDDLE);
#endif /* AMAYA_LOST_UPDATE */
#ifndef _WINDOWS
  HTNet_addAfter (AHTLoadTerminate_handler, NULL, NULL, HT_ALL, HT_FILTER_LAST);	
#endif /* !_WINDOWS */
   /**** for later ?? ****/
   /*  HTNet_addAfter(HTInfoFilter, 	NULL,		NULL, HT_ALL,		HT_FILTER_LATE); */
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
#ifdef __WINDOWS
   HTAlert_add ((HTAlertCallback *) WIN_Activate_Request, HT_PROG_CONNECT);
#endif /* _WINDOWS */
   HTAlert_add (AHTError_print, HT_A_MESSAGE);
   HTError_setShow (~((unsigned int) 0 ) & ~((unsigned int) HT_ERR_SHOW_DEBUG));	/* process all messages except debug ones*/
   HTAlert_add (AHTConfirm, HT_A_CONFIRM);
   HTAlert_add (AHTPrompt, HT_A_PROMPT);
   HTAlert_add (AHTPromptUsernameAndPassword, HT_A_USER_PW);
}

/*----------------------------------------------------------------------
  libwww_CleanCache
  frontend to the recursive cache cleaning function
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void libwww_CleanCache (void)
#else
void libwww_CleanCache ()
Document doc;
View view;
#endif /* __STDC__ */
{
#ifdef AMAYA_WWW_CACHE
  CHAR_T*  real_dir;
  char*    cache_dir;
  char*    tmp;
  int      cache_size;
  int      cache_expire;
  int      cache_disconnect;
  ThotBool error;
  CHAR_T*  ptr;

  if (!HTCacheMode_enabled ())
    /* don't do anything if we're not using a cache */
    return;
  /* temporarily close down the cache, purge it, then restart */
  tmp = HTCacheMode_getRoot ();
  /* don't do anything if we don't have a valid cache dir */
  if (!tmp || *tmp == EOS)
	  return;
  cache_dir = TtaStrdup (tmp);
  HT_FREE (tmp);
  cache_size = HTCacheMode_maxSize ();
  cache_expire = HTCacheMode_expires ();
  cache_disconnect = HTCacheMode_disconnected ();

  /* get something we can work on :) */
  tmp = HTWWWToLocal (cache_dir, "file:", NULL);
  real_dir = TtaAllocString (strlen (tmp) + 20);
  iso2wc_strcpy (real_dir, tmp);
  HT_FREE (tmp);

  /* safeguard... abort the operation if cache_dir doesn't end with
     CACHE_DIR_NAME */
  error = TRUE;
  ptr = ustrstr (real_dir, CACHE_DIR_NAME);  
    if (ptr && *ptr && !ustrcasecmp (ptr, CACHE_DIR_NAME))
      error = FALSE;
  if (error)
    return;  
  
  /* remove the concurrent cache lock */
#ifdef DEBUG_LIBWWW
  fprintf (stderr, "Clearing the cache lock\n");
#endif /* DEBUG_LIBWWW */
  clear_cachelock ();
  HTCacheTerminate ();
  HTCacheMode_setEnabled (FALSE);
  
  RecCleanCache (real_dir);

  HTCacheMode_setExpires (cache_expire);
  HTCacheMode_setDisconnected (cache_disconnect);
  HTCacheInit (cache_dir, cache_size);
  /* set a new concurrent cache lock */
  ustrcat (real_dir, TEXT(".lock"));
  if (set_cachelock (real_dir) == -1)
    /* couldn't open the .lock file, so, we close the cache to
       be in the safe side */
    {
#ifdef DEBUG_LIBWWW
      fprintf (stderr, "couldnt set the cache lock\n");
#endif /* DEBUG_LIBWWW */
      HTCacheTerminate ();
      HTCacheMode_setEnabled (FALSE);
    }
#ifdef DEBUG_LIBWWW
  fprintf (stderr, "set a cache lock\n");
#endif /* DEBUG_LIBWWW */
  TtaFreeMemory (real_dir);
  TtaFreeMemory (cache_dir);
#endif /* AMAYA_WWW_CACHE */
}

#ifdef AMAYA_WWW_CACHE
/*----------------------------------------------------------------------
  RecCleanCache
  Clears an existing cache directory
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void RecCleanCache (CHAR_T* dirname)
#else
static void RecCleanCache (dirname)
CHAR_T*   dirname;
#endif /* __STDC__ */

#ifdef _WINDOWS
{
  HANDLE          hFindFile;
  ThotBool        status;
  WIN32_FIND_DATA ffd;
  CHAR_T        t_dir [MAX_LENGTH];
  CHAR_T*       ptr;

  /* create a t_dir name to start searching for files */
  if ((ustrlen (dirname) + 10) > MAX_LENGTH)
    /* ERROR: directory name is too big */
    return;

  ustrcpy (t_dir, dirname);
  /* save the end of the dirname. We'll use it to make
     a complete pathname when erasing files */
  ptr = &t_dir[ustrlen (t_dir)];
  ustrcat (t_dir, TEXT("*"));

  hFindFile = FindFirstFile (t_dir, &ffd);
    
  if (hFindFile == INVALID_HANDLE_VALUE)
    /* nothing to erase? */
    return;

  status = TRUE;
  while (status) 
    {
      if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
	  /* it's a directory, erase it recursively */
	  if (ustrcmp (ffd.cFileName, TEXT("..")) && ustrcmp (ffd.cFileName, TEXT(".")))
	    {
	      ustrcpy (ptr, ffd.cFileName);
	      ustrcat (ptr, WC_DIR_STR);
	      RecCleanCache (t_dir);
	      urmdir (t_dir);
	    }
	}
	else
	  {
	    /* it's a file, erase it */
	    ustrcpy (ptr, ffd.cFileName);
	    TtaFileUnlink (t_dir);
	  }
      status = FindNextFile (hFindFile, &ffd);
    }
  FindClose (hFindFile);
}

#else /* _WINDOWS */
{
  DIR *dp;
  struct stat st;
#ifdef HAVE_DIRENT_H
  struct dirent *d;
#else
  struct direct *d;
#endif /* HAVE_DIRENT_H */
  char filename[BUFSIZ+1];

  if ((dp = opendir (dirname)) == NULL) 
    {
      /* @@@ we couldn't open the directory ... we need some msg */
      perror (dirname);
      return;
    }
  
  while ((d = readdir (dp)) != NULL)
    {
      /* skip the UNIX . and .. links */
      if (!strcmp (d->d_name, "..")
	  || !strcmp (d->d_name, "."))
	continue;

      sprintf (filename, "%s%c%s", dirname, DIR_SEP, d->d_name);
      if  (lstat (filename, &st) < 0 ) 
	{
	  /* @@2 need some error message */
	  perror (filename);
	  continue;
	}
      
      switch (st.st_mode & S_IFMT)
	{
	case S_IFDIR:
	  /* if we find a directory, we erase it, recursively */
	  strcat (filename, DIR_STR);
	  RecCleanCache (filename);
	  rmdir (filename);
	  break;
	case S_IFLNK:
	  /* skip any links we find */
	  continue;
	  break;
	default:
	  /* erase the filename */
	  TtaFileUnlink (filename);
	  break;
	}
    }
  closedir (dp);
}
#endif /* _WINDOWS */
#endif /* AMAYA_WWW_CACHE */

/*----------------------------------------------------------------------
  CacheInit
  Reads the cache settings from the thot.ini file.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void CacheInit (void)
#else
static void Cacheinit ()
#endif

{
#ifndef AMAYA_WWW_CACHE
   HTCacheMode_setEnabled (NO);

#else /* AMAYA_WWW_CACHE */
  CHAR_T* strptr;
  CHAR_T* real_dir = NULL;
  CHAR_T* cache_lockfile;
  char      www_realDir[MAX_LENGTH];
  char*     cache_dir = NULL;
  int       cache_size;
  int       cache_entry_size;
  ThotBool  cache_enabled;
  ThotBool  cache_locked;
  ThotBool  tmp_bool;

int i;

  /* activate cache? */
  strptr = TtaGetEnvString ("ENABLE_CACHE");
  if (strptr && *strptr && ustrcasecmp (strptr, TEXT("yes")))
    cache_enabled = NO;
  else
    cache_enabled = YES;

  /* cache protected documents? */
  strptr = TtaGetEnvString ("CACHE_PROTECTED_DOCS");
  if (strptr && *strptr && !ustrcasecmp (strptr, TEXT("yes")))
    HTCacheMode_setProtected (YES);
  else
    HTCacheMode_setProtected (NO);

  /* get the cache dir (or use a default one) */
  strptr = TtaGetEnvString ("CACHE_DIR");
  if (strptr && *strptr) 
    {
      real_dir = TtaAllocString (ustrlen (strptr) + ustrlen (CACHE_DIR_NAME) + 20);
      ustrcpy (real_dir, strptr);
	  if (*(real_dir + ustrlen (real_dir) - 1) != WC_DIR_SEP)
	    ustrcat (real_dir, WC_DIR_STR);
    }
  else
    {
      real_dir = TtaAllocString (ustrlen (TempFileDirectory) + ustrlen (CACHE_DIR_NAME) + 20);
      usprintf (real_dir, TEXT("%s%s"), TempFileDirectory, CACHE_DIR_NAME);
    }

  /* compatiblity with previous versions of Amaya: does real_dir
     include CACHE_DIR_NAME? If not, add it */
  strptr = ustrstr (real_dir, CACHE_DIR_NAME);
  if (!strptr)
    ustrcat (real_dir, CACHE_DIR_NAME);
  else
    {
      i = ustrlen (CACHE_DIR_NAME);
	  if (strptr[i] != WC_EOS)
          ustrcat (real_dir, CACHE_DIR_NAME);
    }

  /* convert the local cache dir into a file URL, as expected by
     libwww */
 
  wc2iso_strcpy (www_realDir, real_dir);

  cache_dir = HTLocalToWWW (www_realDir, "file:");

  /* get the cache size (or use a default one) */
  strptr = TtaGetEnvString ("CACHE_SIZE");
  if (strptr && *strptr) 
    cache_size = wctoi (strptr);
  else
    cache_size = DEFAULT_CACHE_SIZE;

  /* get the max cached file size (or use a default one) */
  if (!TtaGetEnvInt ("MAX_CACHE_ENTRY_SIZE", &cache_entry_size))
    cache_entry_size = DEFAULT_MAX_CACHE_ENTRY_SIZE;

  if (cache_enabled) 
    {
      /* how to remove the lock? force remove it? */
      cache_lockfile = TtaAllocString (ustrlen (real_dir) + 20);
      ustrcpy (cache_lockfile, real_dir);
      ustrcat (cache_lockfile, TEXT(".lock"));
      cache_locked = FALSE;
      if (TtaFileExist (cache_lockfile) && !(cache_locked = test_cachelock (cache_lockfile)))
	{
#ifdef DEBUG_LIBWWW
	  fprintf (stderr, "found a stale cache, removing it\n");
#endif /* DEBUG_LIBWWW */
	  /* remove the lock and clean the cache (the clean cache 
	     will remove all, making the following call unnecessary */
	  /* little trick to win some memory */
	  strptr = ustrrchr (cache_lockfile, TEXT('.'));
	  *strptr = WC_EOS;
	  RecCleanCache (cache_lockfile);
	  *strptr = TEXT('.');
	}

      if (!cache_locked) 
	{
	  /* initialize the cache if there's no other amaya
	     instance running */
	  HTCacheMode_setMaxCacheEntrySize (cache_entry_size);
	  if (TtaGetEnvBoolean ("CACHE_EXPIRE_IGNORE", &tmp_bool) && tmp_bool)
	    HTCacheMode_setExpires (HT_EXPIRES_IGNORE);
	  else
	    HTCacheMode_setExpires (HT_EXPIRES_AUTO);
	  TtaGetEnvBoolean ("CACHE_DISCONNECTED_MODE", &tmp_bool);
	  if (tmp_bool)
	    HTCacheMode_setDisconnected (HT_DISCONNECT_NORMAL);
	  else
	    HTCacheMode_setDisconnected (HT_DISCONNECT_NONE);
	  if (HTCacheInit (cache_dir, cache_size))
	    {
	      if (set_cachelock (cache_lockfile) == -1)
		/* couldn't open the .lock file, so, we close the cache to
		   be in the safe side */
		{
		  HTCacheTerminate ();
		  HTCacheMode_setEnabled (FALSE);
#ifdef DEBUG_LIBWWW
		  fprintf (stderr, "couldnt set the cache lock\n");
#endif /* DEBUG_LIBWWW */
		}
#ifdef DEBUG_LIBWWW
	      else
		fprintf (stderr, "created the cache lock\n");
#endif /* DEBUG_LIBWWW */
	    }
	  else
	    {
#ifdef DEBUG_LIBWWW
	      fprintf (stderr, "couldn't create the cache\n");
#endif /* DEBUG_LIBWWW */	      
	      HTCacheTerminate ();
	    }
	}
      else 
	{
	  HTCacheMode_setEnabled (FALSE);
#ifdef DEBUG_LIBWWW
	  fprintf (stderr, "lock detected, starting libwww without a cache/n");
#endif /* DEBUG_LIBWWW */
	}
      TtaFreeMemory (cache_lockfile);
    }
  else
    {
      HTCacheMode_setEnabled (FALSE);
    }
  if (cache_dir)
    HT_FREE (cache_dir);
  if (real_dir)
    TtaFreeMemory (real_dir);
  /* warn the user if the cache isn't active */
  if (cache_enabled && !HTCacheMode_enabled ())
      InitInfo (TEXT("Cache"), TtaGetMessage (AMAYA, AM_CANT_CREATE_CACHE));
#endif /* AMAYA_WWW_CACHE */
}

/*----------------------------------------------------------------------
  ProxyInit
  Reads any proxies settings which may be declared as environmental
  variables or in the thot.ini file. The former overrides the latter.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void ProxyInit (void)
#else
static void ProxyInit ()
#endif /* __STDC__ */
{
  CHAR_T*   strptr;
  char*     name;
  ThotBool  proxy_is_onlyproxy;
  char*     tmp = NULL;
  char*     strptrA;

  /* get the proxy settings from the thot.ini file */
  strptr = TtaGetEnvString ("HTTP_PROXY");
  if (strptr && *strptr)
    {
      tmp = (char*) TtaGetMemory (ustrlen (strptr) + 1);
      wc2iso_strcpy (tmp, strptr);

      /* does the proxy env string has an "http://" prefix? */
      if (!ustrncasecmp (strptr, TEXT("http://"), 7)) 
        HTProxy_add ("http", tmp);
      else 
	{
	  strptrA = (char*) TtaGetMemory (ustrlen (strptr) + 9);
	  strcpy (strptrA, "http://");
	  strcat (strptrA, tmp);
	  HTProxy_add ("http", strptrA);
	  TtaFreeMemory (strptrA);
	}
      TtaFreeMemory (tmp);
    }

  /* get the no_proxy settings from the thot.ini file */
  strptr = TtaGetEnvString ("PROXYDOMAIN");
  if (strptr && *strptr) 
    {
      strptrA = (char*) TtaGetMemory (ustrlen (strptr) + 1);
      wc2iso_strcpy (strptrA, strptr);
      /* as HTNextField changes the ptr we pass as an argument, we'll
	 work with another variable, so that we can free the strptrA
	 block later on */
      tmp = strptrA;
      while ((name = HTNextField (&tmp)) != NULL) 
	{
	  char* portstr = strchr (name, ':');
	  unsigned port=0;
	  if (portstr) { 
	    *portstr++ = EOS;
	    if (*portstr) 
	      port = (unsigned) atoi (portstr);
	  }
	  /* Register it for all access methods */
	  HTNoProxy_add (name, NULL, port);
	}
      TtaFreeMemory (strptrA);
    }
  
  /* how should we interpret the proxy domain list? */
  TtaGetEnvBoolean ("PROXYDOMAIN_IS_ONLYPROXY", &proxy_is_onlyproxy);
  HTProxy_setNoProxyIsOnlyProxy (proxy_is_onlyproxy);

  /* use libwww's routine to get all proxy settings from the environment */
   HTProxy_getEnvVar ();
}

/*----------------------------------------------------------------------
  SafePut_init
  Sets up the domains which are authorized to make a redirect on 
  a PUT.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void SafePut_init (void)
#else
static void SafePut_init ()
#endif /* __STDC__ */
{
  CHAR_T* strptr;
  CHAR_T* str = NULL;
  char*     ptr, *strptrA, *ptr2;
  char*     domain;

  /* get the proxy settings from the thot.ini file */
  strptr = TtaGetEnvString ("SAFE_PUT_REDIRECT");
  if (strptr && *strptr)
    {
      /* Get copy we can mutilate */
      strptrA = (char*) TtaGetMemory (ustrlen (strptr) + 1);
      wc2iso_strcpy (strptrA, strptr);
      ptr2 = strptrA;
      /* convert to lowercase */
      ptr = strptrA;
      while (*ptr) 
	{
	  *ptr = tolower (*ptr);
	  ptr++;
	}
      
      /* create the list container */
      safeput_list = HTList_new ();   
      /* store the domain list */

      ptr = strptrA;
      while ((domain = HTNextField (&strptrA)) != NULL)
	  HTList_addObject (safeput_list, TtaStrdup (domain)); 

      TtaFreeMemory (str);
      TtaFreeMemory (ptr2);
    }
}

/*----------------------------------------------------------------------
  SafePut_delete
  Deletes the safeput_list variable
  ----------------------------------------------------------------------*/
static void SafePut_delete (void)
{
  HTList *cur = safeput_list;
  char *domain;

  if (!safeput_list) 
    return;

  while ((domain = (char *) HTList_nextObject (cur))) 
      TtaFreeMemory (domain);
   HTList_delete (safeput_list);
   safeput_list = NULL;
}

/*----------------------------------------------------------------------
  SafePut_query
  returns true if the domain to which belongs the URL accepts an automatic
  PUT redirect.
  ----------------------------------------------------------------------*/
static ThotBool SafePut_query (CHAR_T* url)
{
  HTList*  cur;
  char*    me;
  ThotBool found;
  CHAR_T   tmp[MAX_LENGTH];

  /* extract the domain path of the url and normalize it */
  /* domain = url; */
  cur = safeput_list;
  found = FALSE;

  while ((me = (char*) HTList_nextObject (cur))) {
        iso2wc_strcpy (tmp, me);
        if (ustrstr (url, tmp)) {
           found = TRUE;
           break;
		} 
  } 

  return (found);
}

/*----------------------------------------------------------------------
  AHTProfile_newAmaya
  creates the Amaya client profile for libwww.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         AHTProfile_newAmaya (CHAR_T* AppName, CHAR_T* AppVersion)
#else  /* __STDC__ */
static void         AHTProfile_newAmaya (AppName, AppVersion)
char* AppName;
char* AppVersion;
#endif /* __STDC__ */
{
   CHAR_T* strptr;
#  ifdef _I18N_
   unsigned char mbAppName[MAX_LENGTH], mbAppVersion[MAX_LENGTH];
#  else  /* !_I18N_ */
   char* mbAppName    = AppName;
   char* mbAppVersion = AppVersion;
#  endif /* !_I18N_ */

   /* If the Library is not already initialized then do it */
   if (!HTLib_isInitialized ()) {
#     ifdef _I18N_
      /* Here we suppose that libwww works with multibyte character string (MBCS).
         AppName and AppVersion are wide character strings (WCS). The following 
		 code transform each of AppName and AppVersion (WCSs) int mbAppName and
         mbAppName (MBCSs).
         If the libwww will support WCSs, than you have to remove the code related 
         to _I18N_ (rounded by #ifdef _I18N_ #endif) and pass to HTLibInit
         AppName instead of mbAppName and AppVersion instead of mbAppVersion */
      wcstombs (mbAppName, AppName, MAX_LENGTH);
      wcstombs (mbAppVersion, AppVersion, MAX_LENGTH);
#     endif /* _I18N_ */
      HTLibInit (mbAppName, mbAppVersion);
   } 

   if (!converters)
      converters = HTList_new ();
   if (!acceptTypes)
      acceptTypes = HTList_new ();
   if (!acceptLanguages)
      acceptLanguages = HTList_new ();
   if (!transfer_encodings)
      transfer_encodings = HTList_new ();
   if (!content_encodings)
      content_encodings = HTList_new ();

   /* inhibits libwww's automatic file_suffix_binding */
   HTFile_doFileSuffixBinding (FALSE);

   /* Register the default set of transport protocols */
   HTTransportInit ();

   /* Register the default set of application protocol modules */
   AHTProtocolInit ();

   /* Register the default set of messages and dialog functions */
   AHTAlertInit ();
   HTAlert_setInteractive (YES);

#ifdef AMAYA_WWW_CACHE
   /* Enable the persistent cache  */
   CacheInit ();
#else
   HTCacheMode_setEnabled (NO);
#endif /* AMAYA_WWW_CACHE */

   /* Register the default set of BEFORE and AFTER filters */
   AHTNetInit ();

   /* Set up the default set of Authentication schemes */
   HTAA_newModule ("basic", HTBasic_generate, HTBasic_parse, NULL, HTBasic_delete);
   /* activate MDA by defaul */
   strptr = TtaGetEnvString ("ENABLE_MDA");
   if (!strptr || (strptr && *strptr && ustrcasecmp (strptr, TEXT("no"))))
     HTAA_newModule ("digest", HTDigest_generate, HTDigest_parse, HTDigest_updateInfo, HTDigest_delete);

   /* Get any proxy settings */
   ProxyInit ();

   /* Set up the domains where we accept a redirect on PUT */
   SafePut_init ();

   /* Register the default set of converters */
   AHTConverterInit (converters);
   HTFormat_setConversion (converters);
   AHTAcceptTypesInit (acceptTypes);
   AHTAcceptLanguagesInit (acceptLanguages);

   /* Register the default set of transfer encoders and decoders */
   HTTransferEncoderInit (transfer_encodings);
   HTFormat_setTransferCoding (transfer_encodings);
   /* Register the default set of content encoders and decoders */
   HTContentEncoderInit (content_encodings);
   /* ignore all other encoding formats (or libwww will send them 
      thru a blackhole otherwise */
   HTCoding_add (content_encodings, "*", NULL, HTIdentityCoding, 1.0);
   if (HTList_count(content_encodings) > 0)
     HTFormat_setContentCoding(content_encodings);
   else 
     {
       HTList_delete(content_encodings);
       content_encodings = NULL;
     }

   /* Register the default set of MIME header parsers */
   HTMIMEInit ();   /* must be called again for language selector */

   /* Register the default set of Icons for directory listings */
   /*HTIconInit(NULL); *//* experimental */
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

  /* Clean up all the registred converters */
  HTFormat_deleteAll ();
  if (acceptTypes)
    HTConversion_deleteAll (acceptTypes);
  if (acceptLanguages)
    HTLanguage_deleteAll (acceptLanguages);

  StopAllRequests (1);
  HTList_delete (Amaya->docid_status);
  HTList_delete (Amaya->reqlist);
  TtaFreeMemory (Amaya);

#ifdef _WINDOWS
  if (HTLib_isInitialized ())      
    HTEventTerminate ();
#endif /* _WINDOWS; */		
    
  /* Clean up the persistent cache (if any) */
#ifdef AMAYA_WWW_CACHE
  clear_cachelock ();
  HTCacheTerminate ();
#endif /* AMAYA_WWW_CACHE */
  
  /* call remove functions that are not called automatically by libwww */
  HTNetCall_deleteBeforeAll (HTNet_before ());
  HTNetCall_deleteAfterAll (HTNet_after ());
  HTAA_deleteAllModules ();
  HTAlertCall_deleteAll (HTAlert_global () );
  HTAlert_setGlobal ((HTList *) NULL);
  /* these two functions are broken, so we can't call them right now 
  HTHeader_deleteAll ();
  HTTimer_deleteAll ();
  */
  HTTransport_deleteAll ();
  /* remove bindings between suffixes, media types*/
  HTBind_deleteAll ();
  /* Terminate libwww */
  HTLibTerminate ();
}

/*----------------------------------------------------------------------
  AmayacontextInit
  initializes an internal Amaya context for our libwww interface 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void                AmayaContextInit ()
#else
static void                AmayaContextInit ()
#endif

{
  AmayaAlive_flag = TRUE;
  /* Initialization of the global context */
  Amaya = (AmayaContext *) TtaGetMemory (sizeof (AmayaContext));
  Amaya->reqlist = HTList_new ();
  Amaya->docid_status = HTList_new ();
  Amaya->open_requests = 0;
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
  CHAR_T* strptr;
  int tmp_i;
  long tmp_l;

   AmayaContextInit ();
   AHTProfile_newAmaya (HTAppName, HTAppVersion);
   CanDoStop_set (TRUE);
   UserAborted_flag = FALSE;

#ifdef _WINDOWS
   HTEventInit ();
#endif /* _WINDOWS */

#ifndef _WINDOWS
   HTEvent_setRegisterCallback ((void *) AHTEvent_register);
   HTEvent_setUnregisterCallback ((void *) AHTEvent_unregister);
#ifndef _GTK
   HTTimer_registerSetTimerCallback ((void *) AMAYA_SetTimer);
   HTTimer_registerDeleteTimerCallback ((void *) AMAYA_DeleteTimer);
#endif /* _GTK */
#endif /* !_WINDOWS */

#ifdef HTDEBUG
   /* an undocumented option for being able to generate an HTTP protocol trace */
   if (TtaGetEnvInt ("ENABLE_LIBWWW_DEBUG", &tmp_i))
     WWW_TraceFlag = tmp_i;
   else
     WWW_TraceFlag = 0;
#endif /* HTDEBUG */

#ifdef DEBUG_LIBWWW
  /* forwards error messages to our own function */
   WWW_TraceFlag = THD_TRACE;
   HTTrace_setCallback(LineTrace);
   /* Trace activation (for debugging) */
   /***
    WWW_TraceFlag = SHOW_CORE_TRACE | SHOW_THREAD_TRACE | SHOW_PROTOCOL_TRACE;
    WWW_TraceFlag |= 0xFFFFFFFFl;
    ***/
#endif

   /* Setting up different network parameters */

   /* Maximum number of simultaneous open sockets */
   strptr = TtaGetEnvString ("MAX_SOCKET");
   if (strptr && *strptr) 
     tmp_i = wctoi (strptr);
   else
     tmp_i = DEFAULT_MAX_SOCKET;
   HTNet_setMaxSocket (tmp_i);

   /* different network services timeouts */
   /* dns timeout */
   strptr = TtaGetEnvString ("DNS_TIMEOUT");
   if (strptr && *strptr) 
     tmp_i = wctoi (strptr);
   else
     tmp_i = DEFAULT_DNS_TIMEOUT;
   HTDNS_setTimeout (tmp_i);

   /* persistent connections timeout */
   strptr = TtaGetEnvString ("PERSIST_CX_TIMEOUT");
   if (strptr && *strptr) 
     tmp_l = uatol (strptr); 
   else
     tmp_l = DEFAULT_PERSIST_TIMEOUT;
   HTHost_setPersistTimeout (tmp_l);

   /* default timeout in ms */
   strptr = TtaGetEnvString ("NET_EVENT_TIMEOUT");
   if (strptr && *strptr) 
     tmp_i = wctoi (strptr);
   else
     tmp_i = DEFAULT_NET_EVENT_TIMEOUT;
   HTHost_setEventTimeout (tmp_i);

   HTRequest_setMaxRetry (8);
#ifdef CATCH_SIG
   signal (SIGPIPE, SIG_IGN);
#endif
}

/*----------------------------------------------------------------------
  LoopForStop
  a copy of the Thop event loop so we can handle the stop button in Unix
  and preemptive requests under Windows
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          LoopForStop (AHTReqContext * me)
#else
static int          LoopForStop (AHTReqContext * me)
#endif
{
#ifdef _WINDOWS
  MSG msg;
  unsigned long libwww_msg;
  HWND old_active_window, libwww_window;
  int  status_req = HT_OK;

  old_active_window = GetActiveWindow ();
  libwww_window = HTEventList_getWinHandle (&libwww_msg);
 
  while (me->reqStatus != HT_END && me->reqStatus != HT_ERR
	     && me->reqStatus != HT_ABORT && AmayaIsAlive () &&
	     GetMessage (&msg, NULL, 0, 0))
		{
         if (msg.message != WM_QUIT)
			{
		      TranslateMessage (&msg);
		      DispatchMessage (&msg);
			}
         else
	        break;      
		}
  if (!AmayaIsAlive ())
    /* Amaya was killed by one of the callback handlers */
    exit (0);
#else /* _WINDOWS */
   extern ThotAppContext app_cont;
   XEvent                ev;
   XtInputMask           status;
   int                 status_req = HT_OK;

   /* to test the async calls  */
   /* Loop while waiting for new events, exists when the request is over */
   while (me->reqStatus != HT_ABORT &&
	  me->reqStatus != HT_END &&
	  me->reqStatus != HT_ERR) {
	 if (!AmayaIsAlive ())
	    /* Amaya was killed by one of the callback handlers */
	    exit (0);

	 status = XtAppPending (app_cont);
	 if (status & XtIMXEvent)
	   {
	     XtAppNextEvent (app_cont, &ev);
	     TtaHandleOneEvent (&ev);
	   } 
	 else if (status != 0) 
	   XtAppProcessEvent (app_cont, XtIMAll);
   }
#endif /* _WINDOWS */
   switch (me->reqStatus) {
	  case HT_ERR:
          case HT_ABORT:
	       status_req = NO;
	       break;

	  case HT_END:
	       status_req = YES;
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
void QueryClose ()
{

  AmayaAlive_flag = FALSE;

  /* remove all the handlers and callbacks that may output a message to
     a non-existent Amaya window */
  HTEvent_setRegisterCallback ((HTEvent_registerCallback *) NULL);
  HTEvent_setUnregisterCallback ((HTEvent_unregisterCallback *) NULL);
#ifndef _WINDOWS
  /** need to erase all existing timers too **/
   HTTimer_registerSetTimerCallback (NULL);
   HTTimer_registerDeleteTimerCallback (NULL);
#endif /* !_WINDOWS */
  HTHost_setActivateRequestCallback (NULL);
  Thread_deleteAll ();
 
  HTProxy_deleteAll ();
  HTNoProxy_deleteAll ();
  SafePut_delete ();
  HTGateway_deleteAll ();
  AHTProfile_delete ();
}

/*----------------------------------------------------------------------
  NextNameValue
  ---------------------------------------------------------------------*/
#ifdef __STDC__
static char * NextNameValue (char ** pstr, char **name, char **value)
#else
static char * NextNameValue (pstr, name, value);
char ** pstr;
char **name;
char **value;
#endif /* __STDC__ */
{
  char * p = *pstr;
  char * start = NULL;
  if (!pstr || !*pstr) return NULL;
  
    if (!*p) {
      *pstr = p;
      *name = NULL;
      *value = NULL;
      return NULL;				   	 /* No field */
    }
    
    /* Now search for the next '&' and ';' delimitators */
    start = p;
    while (*p && *p != '&' && *p != ';') p++;
    if (*p) 
      *p++ = '\0';
    *pstr = p;

    /* Search for the name and value */
    *name = start;
    p = start;
    
    while(*p && *p != '=') 
      p++;
    if (*p) 
      *p++ = '\0';
    *value = p;

    return start;
}

/*----------------------------------------------------------------------
  PrepareFormdata
  ---------------------------------------------------------------------*/
#ifdef __STDC__
static   HTAssocList * PrepareFormdata (CHAR_T* string)
#else
static   HTAssocList * PrepareFormdata (string)
CHAR_T*  string;
#endif /* __STDC__ */
{
  char*        tmp_string, *tmp_string_ptr;
  char*        name;
  char*        value;
  HTAssocList* formdata;

  if (!string)
    return NULL;

  /* store the ptr in another variable, as the original address will
     change
     */
  
  tmp_string_ptr = TtaGetMemory (ustrlen (string) + 1);
  tmp_string = tmp_string_ptr;
  wc2iso_strcpy (tmp_string_ptr, string);
  formdata = HTAssocList_new();
  
  while (*tmp_string)
    {
      NextNameValue (&tmp_string, &name, &value);
      HTAssocList_addObject(formdata,
			    name, value);
    }

  TtaFreeMemory (tmp_string_ptr);
  return formdata;
}

/*----------------------------------------------------------------------
  ---------------------------------------------------------------------*/
#ifdef __STDC__
void AHTRequest_setCustomAcceptHeader (HTRequest *request, char *value)
#else
void AHTRequest_setCustomAcceptHeader (request, value)
HTRequest *request;
char *value;
#endif /* __STDC__ */
{				
  HTRqHd rqhd = HTRequest_rqHd (request);
  rqhd = rqhd & (~HT_C_ACCEPT_TYPE);
  HTRequest_setRqHd (request, rqhd);
  HTRequest_addExtraHeader (request, "Accept", value);
}

/*----------------------------------------------------------------------
  QGetFileSize
  Returns 0 and the filesize in the 2nd parameter.
  Otherwise, returns -1.
  ---------------------------------------------------------------------*/

#ifdef __STDC__
static ThotBool QGetFileSize (CHAR_T* fileName, unsigned long *file_size)
#else
static ThotBool QGetFileSize (fileName, file_size)
CHAR_T*        fileName;
unsigned long* file_size;
#endif /* __STDC__ */
{
  int fd;
  struct stat file_stat;

  if (!TtaFileExist (fileName))
    return -1;

  /* verify the file's size */
#ifndef _WINDOWS
  if ((fd = uopen (fileName, O_RDONLY)) == -1)
#else 
    if ((fd = uopen (fileName, _O_RDONLY | _O_BINARY)) == -1)
#endif /* _WINDOWS */
      {
	/* if we could not open the file, exit */
	/*error msg here */
	return (-1);
      }
  
  fstat (fd, &file_stat);

#ifdef _WINDOWS
  _close (fd);
#else /* _WINDOWS */
  close (fd);
#endif /* _WINDOWS */
  
  *file_size = (unsigned long) file_stat.st_size;

   return 0;
}

/*----------------------------------------------------------------------
  InvokeGetObjectWWW_callback
  A simple function to invoke a callback function whenever there's an error
  in GetObjectWWW
  ---------------------------------------------------------------------*/

#ifdef __STDC__
void      InvokeGetObjectWWW_callback (int docid, STRING urlName, STRING outputfile, TTcbf *terminate_cbf, void *context_tcbf, int status)
#else
void      InvokeGetObjectWWW_callback (docid, urlName, outputfile, terminate_cbf, context_tcbf, status)
int docid;
STRING urlName;
STRING outputfile;
TTcbf *terminate_cbf;
void *context_tcbf;
#endif /* __STDC__ */
{
  if (!terminate_cbf)
    return;
  
  (*terminate_cbf) (docid, status, urlName, outputfile,
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
int GetObjectWWW (int docid, STRING urlName, STRING formdata,
		  STRING outputfile, int mode, TIcbf* incremental_cbf, 
		  void* context_icbf, TTcbf* terminate_cbf, 
		  void* context_tcbf, ThotBool error_html, STRING content_type)
#else
int GetObjectWWW (docid, urlName, formdata, outputfile, mode, 
		  incremental_cbf, context_icbf, 
		  terminate_cbf, context_tcbf, error_html, content_type)
int           docid;
CHAR_T*       urlName;
CHAR_T*       formdata;
CHAR_T*       outputfile;
int           mode;
TIcbf        *incremental_cbf;
void         *context_icbf;
TTcbf        *terminate_cbf;
void         *context_tcbf;
ThotBool      error_html;
CHAR_T*       content_type;
#endif
{
   AHTReqContext      *me;
   CHAR_T*             ref;
   CHAR_T*             esc_url;
   char                urlRef[MAX_LENGTH];
   int                 status, l;
   int                 tempsubdir;
   ThotBool            bool_tmp;

   if (urlName == NULL || docid == 0 || outputfile == NULL) 
     {
       /* no file to be loaded */
       TtaSetStatus (docid, 1, TtaGetMessage (AMAYA, AM_BAD_URL), urlName);
       
       if (error_html)
	 /* so we can show the error message */
	 DocNetworkStatus[docid] |= AMAYA_NET_ERROR;
       InvokeGetObjectWWW_callback (docid, urlName, outputfile, terminate_cbf,
				    context_tcbf, HT_ERROR);
       return HT_ERROR;
     }

   /* if it's a 'docImage', we have already downloaded it */
   if (!ustrncmp (TEXT("internal:"), urlName, 9)) 
     {
       ustrcpy (outputfile, urlName);
       InvokeGetObjectWWW_callback (docid, urlName, outputfile,
				    terminate_cbf, context_tcbf, HT_OK);
       return HT_OK;
     }

   /* do we support this protocol? */
   if (IsValidProtocol (urlName) == NO) 
     {
       /* return error */
       outputfile[0] = EOS;	/* file could not be opened */
       TtaSetStatus (docid, 1, 
		     TtaGetMessage (AMAYA, AM_GET_UNSUPPORTED_PROTOCOL),
		     urlName);

       if (error_html)
	 /* so we can show the error message */
	 DocNetworkStatus[docid] |= AMAYA_NET_ERROR;
       InvokeGetObjectWWW_callback (docid, urlName, outputfile, terminate_cbf,
				    context_tcbf, HT_ERROR);
       return HT_ERROR;
     }

   /* we store CSS in subdir named 0; all the other files go to a subidr
      named after their own docid */
   
   tempsubdir = (mode & AMAYA_LOAD_CSS) ? 0 : docid;

   /* create a tempfilename */
   usprintf (outputfile, TEXT("%s%c%d%c%04dAM"), TempFileDirectory, WC_DIR_SEP, tempsubdir, WC_DIR_SEP, object_counter);
   /* update the object_counter (used for the tempfilename) */
   object_counter++;
   
   /* normalize the URL */
   esc_url = EscapeURL (urlName);
   if (esc_url) 
     {
       ref = AmayaParseUrl (esc_url, TEXT(""), AMAYA_PARSE_ALL);
       TtaFreeMemory (esc_url);
     }
   else
     ref = NULL;

   /* should we abort the request if we could not normalize the url? */
   if (ref == NULL || ref[0] == EOS) {
      /*error */
      outputfile[0] = EOS;
      TtaSetStatus (docid, 1, TtaGetMessage (AMAYA, AM_BAD_URL), urlName);
      if (ref)
	TtaFreeMemory (ref); 
      if (error_html)
	/* so we can show the error message */
	DocNetworkStatus[docid] |= AMAYA_NET_ERROR;
      InvokeGetObjectWWW_callback (docid, urlName, outputfile, terminate_cbf,
				   context_tcbf, HT_ERROR);
      return HT_ERROR;
   }

   /* verify if that file name existed */
   if (TtaFileExist (outputfile))
     TtaFileUnlink (outputfile);
   
   /* Initialize the request structure */
   me = AHTReqContext_new (docid);
   if (me == NULL) 
     {
       outputfile[0] = EOS;
       /* need an error message here */
       TtaFreeMemory (ref);
       InvokeGetObjectWWW_callback (docid, urlName, outputfile, terminate_cbf,
				   context_tcbf, HT_ERROR);
       return HT_ERROR;
     }

   /* Specific initializations for POST and GET */
   if (mode & AMAYA_FORM_POST
       || mode & AMAYA_FILE_POST)
     {
       me->method = METHOD_POST;
       HTRequest_setMethod (me->request, METHOD_POST);
     }
   else 
     {
       me->method = METHOD_GET;
       if (!HasKnownFileSuffix (ref))
	 {
	   /* try to adjust the Accept header in an netwise economical way */
	   if (mode & AMAYA_LOAD_IMAGE)
	     AHTRequest_setCustomAcceptHeader (me->request, IMAGE_ACCEPT_NEGOTIATION);
	   else if (mode & AMAYA_LOAD_CSS)
	     AHTRequest_setCustomAcceptHeader (me->request, "*/*;q=0.1,css/*");
	   /*
	   HTRequest_setConversion(me->request, acceptTypes, TRUE);
	   */
	 }
	   HTRequest_setLanguage (me->request, acceptLanguages, TRUE);
     }

   /* Common initialization for all HTML methods */
   me->mode = mode;
   me->error_html = error_html;
   me->incremental_cbf = incremental_cbf;
   me->context_icbf = context_icbf;
   me->terminate_cbf = terminate_cbf;
   me->context_tcbf = context_tcbf;

   /* for the async. request modes, we need to have our
      own copy of outputfile and urlname
      */

   if ((mode & AMAYA_ASYNC) || (mode & AMAYA_IASYNC)) 
     {
       l = ustrlen (outputfile);
       if (l > MAX_LENGTH)
	 me->outputfile = TtaAllocString (l + 2);
       else
	 me->outputfile = TtaAllocString (MAX_LENGTH + 2);
       ustrcpy (me->outputfile, outputfile);
       l = ustrlen (urlName);
       if (l > MAX_LENGTH)
	 me->urlName = TtaAllocString (l + 2);
       else
	 me->urlName = TtaAllocString (MAX_LENGTH + 2);
       ustrcpy (me->urlName, urlName);
#ifdef _WINDOWS
     /* force windows ASYNC requests to always be non preemptive */
     HTRequest_setPreemptive (me->request, NO);
#endif /*_WINDOWS */
     } /* AMAYA_ASYNC mode */ 
   else 
#ifdef _WINDOWS
     {
       me->outputfile = outputfile;
       me->urlName = urlName;
       /* force windows SYNC requests to always be non preemptive */
       HTRequest_setPreemptive (me->request, YES);
     }
#else /* !_WINDOWS */
     {
       me->outputfile = outputfile;
       me->urlName = urlName;
     }
   /***
     In order to take into account the stop button, 
     the requests will be always asynchronous, however, if mode=AMAYA_SYNC,
     we will loop until the document has been received or a stop signal
     generated
     ****/
   HTRequest_setPreemptive (me->request, NO);
#endif /* _WINDOWS */

   /*
   ** Make sure that the first request is flushed immediately and not
   ** buffered in the output buffer
   */
   if (mode & AMAYA_FLUSH_REQUEST)
     HTRequest_setFlush(me->request, YES);
   HTRequest_setFlush(me->request, YES);

   /* prepare the URLname that will be displayed in teh status bar */
   ChopURL (me->status_urlName, me->urlName);
   TtaSetStatus (me->docid, 1, 
		 TtaGetMessage (AMAYA, AM_FETCHING),
		 me->status_urlName);

   wc2iso_strcpy (urlRef, ref);
   me->anchor = (HTParentAnchor *) HTAnchor_findAddress (urlRef);
   TtaFreeMemory (ref);
   
   TtaGetEnvBoolean ("CACHE_DISCONNECTED_MODE", &bool_tmp);
   if (!bool_tmp && (mode & AMAYA_NOCACHE))
      HTRequest_setReloadMode (me->request, HT_CACHE_FLUSH);

   /* prepare the query string and format for POST */
   if (mode & AMAYA_FORM_POST)
     {
       HTAnchor_setFormat ((HTParentAnchor *) me->anchor, 
			   HTAtom_for ("application/x-www-form-urlencoded"));
       HTAnchor_setLength ((HTParentAnchor *) me->anchor, me->block_size);
       HTRequest_setEntityAnchor (me->request, me->anchor);
     } 

   /* create the formdata element for libwww */
   if (formdata && ! (mode & AMAYA_FILE_POST))
      me->formdata = PrepareFormdata (formdata);

   /* do the request */
   if (mode & AMAYA_FORM_POST)
     {
       /* this call doesn't give back a ThotBool */
       HTParentAnchor * posted = NULL;

       posted = HTPostFormAnchor (me->formdata, (HTAnchor *) me->anchor, 
				    me->request);
       status = posted ? YES : NO; 
     }
#ifdef ANNOTATIONS
   if (mode & AMAYA_FILE_POST)
     {
       unsigned long filesize;
       char *fileURL;

       /* @@@ a very ugly patch :)))
	I'm copying here some of the functionality I use in the PUT
       I need to put the common parts in another module */
       QGetFileSize (formdata, &filesize);
       filesize = filesize + strlen ("w3c_annotate=");
       me->block_size = filesize;

       fileURL = HTParse (formdata, "file:/", PARSE_ALL);
       me->source = HTAnchor_findAddress (fileURL);
       HT_FREE (fileURL);

       /* hardcoded ... */
       AHTRequest_setCustomAcceptHeader (me->request, "application/xml");
       HTAnchor_setFormat (HTAnchor_parent (me->source),
			   HTAtom_for ("application/xml"));
       HTAnchor_setFormat (me->anchor,
			   HTAtom_for ("application/xml"));
       HTAnchor_setLength ((HTParentAnchor *) me->source, me->block_size);
       /* @@ here I need to actually read the file and put it in document,
	  then, when I kill the request, I need to kill it */
       {
	 FILE *fp;
	 int i;
	 char c;

	 me->document = TtaGetMemory (me->block_size 
				      + strlen ("w3c_annotate=") + 1);
	 fp = fopen (formdata, "r");
	 i = 0;
	 strcpy (me->document, "w3c_annotate=");
	 c = getc (fp);
	 i = strlen(me->document);
	 while (!feof (fp))
	   {
	     me->document[i++] = c;
	     c = getc (fp);
	   }
	 me->document[i] = '\0';
	 fclose (fp);
       }
       HTAnchor_setDocument ( (HTParentAnchor *) me->source,
			      (void * ) me->document);
       HTRequest_setEntityAnchor (me->request, HTAnchor_parent (me->source));
 
       status = HTPostAnchor (HTAnchor_parent (me->source), 
			      (HTAnchor *) me->anchor, 
			      me->request);
     }
#endif /* ANNOTATIONS */
   else if (formdata)
     status = HTGetFormAnchor(me->formdata, (HTAnchor *) me->anchor,
			      me->request);
   else
     status = HTLoadAnchor ((HTAnchor *) me->anchor, me->request);

   /* @@@ may need some special windows error msg here */
   /* control the errors */

    if (status == NO)
     /* the request invocation failed */
     {
       /* show an error message on the status bar */
       DocNetworkStatus[docid] |= AMAYA_NET_ERROR;
       TtaSetStatus (docid, 1, 
		     TtaGetMessage (AMAYA, AM_CANNOT_LOAD),
		     urlName);
       if (me->reqStatus == HT_NEW)
	 /* manually invoke the last processing that usually gets done
	    in a succesful request */
	 InvokeGetObjectWWW_callback (docid, urlName, outputfile, 
				      terminate_cbf, context_tcbf, HT_ERROR);
       /* terminate_handler wasn't called */
       AHTReqContext_delete (me);
     }
   else
     /* end treatment for SYNC requests */
     if ((mode & AMAYA_SYNC) || (mode & AMAYA_ISYNC))
       {
	 /* wait here untilt the asynchronous request finishes */
	 status = LoopForStop (me);
	 /* if status returns HT_ERROR, should we invoke the callback? */
	 if (!HTRequest_kill (me->request))
	   AHTReqContext_delete (me);
       }

    /* an interface problem!!! */
    return (status == YES ? 0 : -1);
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
int                 PutObjectWWW (int docid, STRING fileName, STRING urlName, int mode, PicType contentType,
				  TTcbf * terminate_cbf, void *context_tcbf)
#else
int                 PutObjectWWW (docid, urlName, fileName, mode, contentType,
				  ,terminate_cbf, context_tcbf)
int                 docid;
STRING              urlName;
STRING              fileName;
int                 mode;
PicType             contentType;
TTcbf              *terminate_cbf;
void               *context_tcbf;

#endif /* __STDC__ */
{
   AHTReqContext      *me;
   int                 status;
   unsigned long      file_size;
   char               *fileURL;
   char               *etag = NULL;
   HTParentAnchor     *dest_anc_parent;
   CHAR_T*             tmp;
   CHAR_T*             esc_url;
   int                 UsePreconditions;
   ThotBool            lost_update_check = TRUE;
   char                url_name[MAX_LENGTH];
   char*               tmp2;
#ifdef _WINDOWS
   char                file_name[MAX_LENGTH];
#endif /* _WINDOWS */

   /* should we protect the PUT against lost updates? */
   tmp = TtaGetEnvString ("ENABLE_LOST_UPDATE_CHECK");
   if (tmp && *tmp && ustrcasecmp (tmp, TEXT("yes")))
     lost_update_check = FALSE;

   UsePreconditions = mode & AMAYA_USE_PRECONDITIONS;

   AmayaLastHTTPErrorMsg [0] = EOS;
   
   if (urlName == NULL || docid == 0 || fileName == NULL 
       || !TtaFileExist (fileName))
      /* no file to be uploaded */
      return HT_ERROR;

   /* do we support this protocol? */
   if (IsValidProtocol (urlName) == NO)
     {
	/* return error */
	TtaSetStatus (docid, 1, TtaGetMessage (AMAYA, AM_PUT_UNSUPPORTED_PROTOCOL), urlName);
	return HT_ERROR;
     }

   /* get the size of the file */
   if (QGetFileSize (fileName, &file_size) || file_size == 0)
     {
	/* file was empty */
	/*errmsg here */
	return (HT_ERROR);
     }

   /* prepare the request context */
   if (THD_TRACE)
      fprintf (stderr, "file size == %u\n", (unsigned) file_size);

   me = AHTReqContext_new (docid);
   if (me == NULL)
     {
       /* @@ need an error message here */
	TtaHandlePendingEvents (); 
	return (HT_ERROR);
     }

   /*
   ** Set up the original URL name
   */
   if (DocumentMeta[docid]->put_default_name)
     {
       CHAR_T *ptr1, *ptr2;
       ptr1 = TtaGetEnvString ("DEFAULTNAME");
       if (ptr1 && *ptr1) 
	 {
	   ptr2 = ustrstr (urlName, ptr1);
	   if (ptr2) 
	     {
           wc2iso_strcpy (url_name, urlName);
	       me->default_put_name = TtaStrdup (url_name);
	       me->default_put_name[strlen (me->default_put_name) - ustrlen (ptr1)] = EOS;
	       HTRequest_setDefaultPutName (me->request, me->default_put_name);
	     }
	 }
     }

   me->mode = mode;
   me->incremental_cbf = (TIcbf *) NULL;
   me->context_icbf = (void *) NULL;
   me->terminate_cbf = terminate_cbf;
   me->context_tcbf = context_tcbf;
   esc_url = EscapeURL (urlName);
   me->urlName = TtaWCSdup (esc_url);
   TtaFreeMemory (esc_url);
   me->block_size =  file_size;
   /* select the parameters that distinguish a PUT from a GET/POST */
   me->method = METHOD_PUT;
   me->output = stdout;
   /* we are not expecting to receive any input from the server */
   me->outputfile = (CHAR_T*) NULL; 

#ifdef _WINDOWS
   /* libwww's HTParse function doesn't take into account the drive name;
      so we sidestep it */

   fileURL = NULL;
   StrAllocCopy (fileURL, "file:");
   wc2iso_strcpy (file_name, fileName);
   StrAllocCat (fileURL, file_name);
#else
   fileURL = HTParse (fileName, "file:/", PARSE_ALL);
#endif /* _WINDOWS */
   me->source = HTAnchor_findAddress (fileURL);
   HT_FREE (fileURL);
   wc2iso_strcpy (url_name, me->urlName);
   me->dest = HTAnchor_findAddress (url_name);
   /* we memorize the anchor's parent @ as we use it a number of times
      in the following lines */
   dest_anc_parent = HTAnchor_parent (me->dest);

   /*
   **  Set the Content-Type of the file we are uploading 
   */
   /* we try to use any content-type previosuly associated
      with the parent. If it doesn't exist, we try to guess it
      from the URL */
   tmp2 = HTAtom_name (HTAnchor_format (dest_anc_parent));
   if (!tmp2 || !strcmp (tmp2, "www/unknown"))
     {
       HTAnchor_setFormat (dest_anc_parent, AHTGuessAtom_for (me->urlName, contentType));
       tmp2 = HTAtom_name (HTAnchor_format (dest_anc_parent));
     }
   /* .. and we give the same type to the source anchor */
   /* we go thru setOutputFormat, rather than change the parent's
      anchor, as that's the place that libwww expects it to be */
   HTAnchor_setFormat (HTAnchor_parent (me->source), HTAtom_for (tmp2));

   HTRequest_setOutputFormat (me->request, HTAtom_for (tmp2));

#if 0 /* JK: code ready, but we're not going to use it yet */
   /*
   **  Set the Charset of the file we are uploading 
   */
   /* we set the charset as indicated in the document's metadata
      structure (and only if it exists) */
   tmp = DocumentMeta[docid]->charset;
   if (tmp && *tmp != WC_EOS)
     {
       tmp2 = TtaWC2ISOdup (tmp);
       HTAnchor_setCharset (dest_anc_parent, HTAtom_for (tmp2));
       TtaFreeMemory (tmp2);
       tmp2 = HTAtom_name (HTAnchor_charset (dest_anc_parent));
       /* .. and we give the same charset to the source anchor */
       /* we go thru setCharSet, rather than change the parent's
	  anchor, as that's the place that libwww expects it to be */
       HTAnchor_setCharset (HTAnchor_parent (me->source),
			    HTAtom_for (tmp2));
     }
#endif /* CHARSET */

   /*
   ** define other request characteristics
   */
#ifdef _WINDOWS
   HTRequest_setPreemptive (me->request, NO);
#else
   HTRequest_setPreemptive (me->request, NO);
#endif /* _WINDOWS */

   /*
   ** Make sure that the first request is flushed immediately and not
   ** buffered in the output buffer
   */
   if (mode & AMAYA_FLUSH_REQUEST)
     HTRequest_setFlush(me->request, YES);
   
   /* Should we use preconditions? */
   if (lost_update_check)
     {
       if (UsePreconditions) 
	 etag = HTAnchor_etag (HTAnchor_parent (me->dest));
       
       if (etag) 
	 {
	   HTRequest_setPreconditions(me->request, HT_MATCH_THIS);
	 }
       else
	 {
	   HTRequest_setPreconditions(me->request, HT_NO_MATCH);
	   HTRequest_addAfter(me->request, check_handler, NULL, NULL, HT_ALL,
			      HT_FILTER_MIDDLE, YES);
	   HTRequest_addAfter (me->request, HTAuthFilter, "http://*", NULL, 
			       HT_NO_ACCESS, HT_FILTER_MIDDLE, YES);
	   HTRequest_addAfter (me->request, HTAuthFilter, "http://*", NULL,
			       HT_REAUTH, HT_FILTER_MIDDLE, YES);
	   HTRequest_addAfter (me->request, HTAuthInfoFilter, "http://*", NULL,
			       HT_ALL, HT_FILTER_MIDDLE, YES);
	   HTRequest_addAfter (me->request, HTUseProxyFilter, "http://*", NULL,
			       HT_USE_PROXY, HT_FILTER_MIDDLE, YES);
	 }
     }
   else
     {
       /* don't use preconditions */
       HTRequest_setPreconditions(me->request, HT_NO_MATCH);
     }
   
   /* don't use the cache while saving a document */
   HTRequest_setReloadMode (me->request, HT_CACHE_FLUSH);

   /* Throw away any reponse body */
   /*
   HTRequest_setOutputStream (me->request, HTBlackHole());        
   */

   /* prepare the URLname that will be displayed in the status bar */
   ChopURL (me->status_urlName, me->urlName);
   TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_REMOTE_SAVING), me->status_urlName);

   /* make the request */
   if (lost_update_check && (!UsePreconditions || !etag))
     status = HTHeadAnchor (me->dest, me->request);
   else
     status = HTPutDocumentAnchor (HTAnchor_parent (me->source), me->dest, me->request);

   if (status == YES && me->reqStatus != HT_ERR)
     {
       /* part of the stop button handler */
       if ((mode & AMAYA_SYNC) || (mode & AMAYA_ISYNC))
	 status = LoopForStop (me);
     }
   if (!HTRequest_kill (me->request))
     AHTReqContext_delete (me);
   
   TtaHandlePendingEvents ();

   return (status == YES ? 0 : -1);
}

/*----------------------------------------------------------------------
  StopRequest
  stops (kills) all active requests associated with a docid 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                StopRequest (int docid)
#else
void                StopRequest (docid)
int                 docid;
#endif
{
   if (Amaya && CanDoStop ())
     { 
#if 0 /* for later */
       AHTDocId_Status    *docid_status;
        /* verify if there are any requests at all associated with docid */
       docid_status = (AHTDocId_Status *) GetDocIdStatus (docid,
							  Amaya->docid_status);
       if (docid_status == (AHTDocId_Status *) NULL)
	 return;
#endif /* 0 */
       /* temporary call to stop all requests, as libwww changed its API */
       StopAllRequests (docid);
     }
}

/* @@@ the docid parameter isn't used... clean it up */
/*----------------------------------------------------------------------
  StopAllRequests
  stops (kills) all active requests. We use the docid 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                StopAllRequests (int docid)
#else
void                StopAllRequests (docid)
int                 docid;
#endif
{
   HTList             *cur;
   AHTReqContext      *me;
   static ThotBool     lock_stop = 0;
   ThotBool            async_flag;
   AHTReqStatus        old_reqStatus;

   /* only do the stop if we're not being called while processing a 
      request, and if we're not already dealing with a stop */
   if (Amaya && CanDoStop () && !lock_stop)
     {
#ifdef DEBUG_LIBWWW
       fprintf (stderr, "StopRequest: number of Amaya requests "
		"before kill: %d\n", Amaya->open_requests);
#endif /* DEBUG_LIBWWW */
       /* enter the critical section */
       lock_stop = TRUE; 
       /* set a module global variable so that we can do special
	  processing easier */
       UserAborted_flag = TRUE;
       /* abort all outstanding libwww UI dialogues */
       CallbackDialogue (BaseDialog + FormAnswer,  STRING_DATA, (CHAR_T*) 0);
       CallbackDialogue (BaseDialog + ConfirmForm, INTEGER_DATA, (CHAR_T*) 0);
       /* expire all outstanding timers */
       HTTimer_expireAll ();
       /* HTNet_killAll (); */
       cur = Amaya->reqlist;
       while ((me = (AHTReqContext *) HTList_nextObject (cur))) 
	 {
	   if (AmayaIsAlive ())
	     {
#ifdef DEBUG_LIBWWW
	       fprintf (stderr,"StopRequest: killing req %p, url %s, status %d\n", me, me->urlName, me->reqStatus);
#endif /* DEBUG_LIBWWW */

	       if (me->reqStatus != HT_END && me->reqStatus != HT_ABORT)
		 {
		   if ((me->mode & AMAYA_ASYNC)
		       || (me->mode & AMAYA_IASYNC))
		     async_flag = TRUE;
		   else
		     async_flag = FALSE;

		   /* change the status to say that the request aborted */
		   /* if the request was "busy", we just change a flag to say so and
		    let the handler finish the processing itself */
		   old_reqStatus = me->reqStatus;
		   me->reqStatus = HT_ABORT;
		   if (old_reqStatus == HT_BUSY)
		     continue;

		   /* kill the request, using the appropriate function */
		   if (me->request->net)
		       HTNet_killPipe (me->request->net);
		   else
		     {
		       if (me->terminate_cbf)
			 (*me->terminate_cbf) (me->docid, -1, me->urlName,
					       me->outputfile,
					       NULL,
					       me->context_tcbf);

		       if (async_flag) 
			 /* explicitly free the request context for async
			  requests. The sync requests context is freed by LoopForStop */
			   AHTReqContext_delete (me);
		     }
		   cur = Amaya->reqlist;
		 }
#ifndef _WINDOWS
#ifdef WWW_XWINDOWS
	   /* to be on the safe side, remove all outstanding X events */
		   else 
		     RequestKillAllXtevents (me);
#endif /* WWW_XWINDOWS */
#endif /* !_WINDOWS */
	     }
	 }
       /* Delete remaining channels */
       HTChannel_safeDeleteAll ();
       /* reset the stop status */
       UserAborted_flag = FALSE;
       /* exit the critical section */
       lock_stop = FALSE; 
#ifdef DEBUG_LIBWWW
       fprintf (stderr, "StopRequest: number of Amaya requests "
		"after kill: %d\n", Amaya->open_requests);
#endif /* DEBUG_LIBWWW */
     }
} /* StopAllRequests */


/*----------------------------------------------------------------------
  AmayaIsAlive
  returns the value of the AmayaAlive_flag
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool AmayaIsAlive (void)
#else
ThotBool AmayaIsAlive ()
#endif /* _STDC_ */
{
  return AmayaAlive_flag;
}

/*----------------------------------------------------------------------
  CanDoStop
  returns the value of the CanDoStop flag
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool CanDoStop (void)
#else
ThotBool CanDoStop ()
#endif /* _STDC_ */
{
  return CanDoStop_flag;
}

/*----------------------------------------------------------------------
  CanDoStop_set
  sets the value of the CanDoStop flag
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void CanDoStop_set (ThotBool value)
#else
void CanDoStop (value)
ThotBool value;
#endif /* _STDC_ */
{
  CanDoStop_flag = value;
}

#ifdef __STDC__
void libwww_updateNetworkConf (int status)
#else
void libwww_updateNetworkConf (status)
int status;
#endif /*__STDC__*/
{
  /* @@@ the docid parameter isn't used... clean it up */
  int docid = 1;

  /* first, stop all current requests, as the network
   may make some changes */
  StopAllRequests (docid);

  if (status & AMAYA_SAFEPUT_RESTART)
    { 
      SafePut_delete ();
      SafePut_init ();
    }

  if (status & AMAYA_PROXY_RESTART)
    {
      HTProxy_deleteAll ();
      ProxyInit ();
    }

  if (status & AMAYA_CACHE_RESTART)
    {
      clear_cachelock ();
      HTCacheTerminate ();
      HTCacheMode_setEnabled (NO);
      CacheInit ();
    }

  if (status & AMAYA_LANNEG_RESTART)
    {
      /* clear the current values */
      if (acceptLanguages)
	HTLanguage_deleteAll (acceptLanguages);
      /* read in the new ones */
      acceptLanguages = HTList_new ();
      AHTAcceptLanguagesInit (acceptLanguages);
    }
}
