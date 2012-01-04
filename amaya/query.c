/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2009
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
 *         L. Carcone SSL support
 */

/* defines to include elsewhere
*********************************/
#ifdef _WX
#include "wx/utils.h"
#include "wx/dir.h"
#include "wxAmayaSocketEvent.h"
#include "wxAmayaSocketEventLoop.h"
#endif /* _WX */

#define AMAYA_WWW_CACHE
#define AMAYA_LOST_UPDATE
/* Amaya includes  */
#define THOT_EXPORT extern
#include "amaya.h"
#include "init_f.h"
#include <sys/types.h>
#ifndef _WINDOWS
#include <unistd.h>
#endif /* #ifndef _WINDOWS */

#include <fcntl.h>
#include "HTEvtLst.h"
#include "HTAABrow.h"
#include "string.h"
#include "interface.h"
#include "message_wx.h"

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

/* libwww default parameters */
#ifdef _WINDOWS
#define CACHE_DIR_NAME "\\libwww-cache\\"
#endif /* _WINDOWS */

#if defined(_UNIX)
#define CACHE_DIR_NAME "/libwww-cache/"
#endif /* #if defined(_UNIX)*/

#define DEFAULT_CACHE_SIZE 10
#define DEFAULT_MAX_CACHE_ENTRY_SIZE 3
#define DEFAULT_MAX_SOCKET 32
#define DEFAULT_DNS_TIMEOUT 1800L
#define DEFAULT_PERSIST_TIMEOUT 60L
#define DEFAULT_NET_EVENT_TIMEOUT 60000
/* defines the priority for general content negotiation */
#define GENERAL_ACCEPT_NEGOTIATION "*/*;q=0.1,"AM_SVG_MIME_TYPE","AM_MATHML_MIME_TYPE","AM_XHTML_MIME_TYPE
/* defines the priority for image content negotiation */
#define IMAGE_ACCEPT_NEGOTIATION "*/*;q=0.1,image/*,image/gif,image/jpeg,image/png,image/svg+xml,"AM_SVG_MIME_TYPE

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

static  ThotBool    FTPURL_flag; /* if set to true, we can browse FTP URLs */

static  FILE        *trace_fp = NULL;   /* file pointer to the trace logs */

#include "answer_f.h"
#include "query_f.h"
#include "AHTURLTools_f.h"
#include "AHTBridge_f.h"
#include "AHTMemConv_f.h"
#include "AHTFWrite_f.h"

#ifdef DAV
#define WEBDAV_EXPORT extern
#include "davlib.h"
#include "davlib_f.h"
#include "davlibCommon_f.h"
#endif /* DAV */

/* prototypes */

#ifdef _WINDOWS
int WIN_Activate_Request (HTRequest* , HTAlertOpcode, int, const char*, void*, HTAlertPar*);
#endif /* _WINDOWS */

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
static int set_cachelock (char *filename)
{
  int status = 0;
#ifdef _WINDOWS

  status = TtaFileExist (filename);
  return ((status) ? 0 : -1);
#endif /* _WINDOWS */
  
#if defined(_UNIX)
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
#endif /* #if defined(_UNIX) */

}

/*----------------------------------------------------------------------
  clear_cachelock
  remove the write lock set on a filename.
  It'll close the fd handle on filename and reset *fd to 0.
  ----------------------------------------------------------------------*/
static int clear_cachelock (void)
{
  int status = 0;  

#if defined(_UNIX)
  if (!fd_cachelock)
    return (-1);
 
  status = close (fd_cachelock);
  fd_cachelock = 0;
#endif /* #if defined(_UNIX) */

  return (status);
}

/*----------------------------------------------------------------------
  test_cachelock
  returns 0 if a fd is not locked by other process, otherwise 
  returns the pid of the process who has the lock
  ----------------------------------------------------------------------*/
static int test_cachelock (char *filename)
{
#ifdef _WINDOWS
  /* if the lock is set, we can't unlink the file under Windows */
  if (TtaFileUnlink(filename))
    return 0;
  else
    return -1;
#endif
  
#if defined(_UNIX)
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
#endif /* #if defined(_UNIX) */
}

#endif /* AMAYA_WWW_CACHE */

/*----------------------------------------------------------------------
  GetDocIdStatus
  gets the status associated to a docid                         
  ----------------------------------------------------------------------*/
AHTDocId_Status    *GetDocIdStatus (int docid, HTList * documents)
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
static  HTAtom *AHTGuessAtom_for (char *urlName, const char *contentType)
{
  HTAtom           *atom;
  char *          filename;
  HTEncoding        enc = NULL;
  HTEncoding        cte = NULL;
  HTLanguage        lang = NULL;
  double            quality = 1.0;

  /* we already have a MIME type, we jsut return the atom */
  if (contentType && *contentType)
    atom = HTAtom_for (contentType);
  else
    {
      /* we don't have the MIME type, so
         we try to use the filename's suffix to infer it */
      filename = AmayaParseUrl (urlName, "", 
                                AMAYA_PARSE_PATH | AMAYA_PARSE_PUNCTUATION);
      HTBind_getFormat (filename, &atom, &enc, &cte, &lang, &quality);
      TtaFreeMemory (filename);
      if (atom ==  WWW_UNKNOWN)
        {
          /*
          ** we could not identify the suffix, so we give it a default type.
          ** (we should ask the user, but we're not ready for that yet).
          */
          atom = HTAtom_for ("text/html");
        }
    }
  return atom;
}

/*----------------------------------------------------------------------
  HTTP_Headers_set
  Copies the headers in which the application is interested, doing
  any in-between conversions as needed.
  ----------------------------------------------------------------------*/
void HTTP_headers_set (HTRequest *request, HTResponse *response,
                       void *context, int status)
{
  AHTReqContext  *me;
  HTAtom         *tmp_atom = NULL;
  char           *tmp_char;
  char           *tmp_char2;
  char           tmp_string[20];
  HTParentAnchor *anchor;
  ThotBool        use_anchor;
  HTError        *pres;
  HTList         *cur;
  int             index;

  me =  (AHTReqContext *) HTRequest_context (request);

  anchor = HTRequest_anchor (request);

  /* if we get a cached answer, we should get all the header information
     from the anchor, instead of from the response object */
  tmp_char = HTAnchor_physical (anchor);
  if (tmp_char && !strncmp (tmp_char, "cache:", sizeof ("cache:") - 1))
    use_anchor = TRUE;
  else
    use_anchor = FALSE;

  /* @@@ JK: we need a function here to specify which headers we
     want to copy */

  /* copy the content_type if we didn't force it before */
  if (me->http_headers.content_type == NULL)
    {
      /* trying to use the info in the anchor, rather than in the response.
         Seems it's more recent */

      /* @@ JK: trying to use the content type stored in the response object */
      if (!use_anchor && response)
        tmp_atom = HTResponse_format (response);
      if (!tmp_atom)
        tmp_atom = HTAnchor_format (anchor);

      if (tmp_atom)
        tmp_char = HTAtom_name (tmp_atom);
      else
        tmp_char = (char*)"www/unknown";
      
      if (tmp_char && tmp_char[0] != EOS)
        {
          /* libwww gives www/unknown when it gets an error. As this is 
             an HTML test, we force the type to text/html */
          if (!strcmp (tmp_char, "www/unknown"))
            {
              /* if it's not a file downloaded from FTP, initialize the
                 content type to text/html by default */
              me->http_headers.content_type = NULL;
            }
          else 
            me->http_headers.content_type = TtaStrdup (tmp_char);
	  
#ifdef DEBUG_LIBWWW
          fprintf (stderr, "Set content type to: %s\n", 
                   me->http_headers.content_type);
#endif /* DEBUG_LIBWWW */
        }
    }

  /* copy the charset */

  /* @@ JK I think there was a problem when using info directly from
     the anchor... but what it was? The response object doesn't have
     it anyway */
  tmp_atom = HTAnchor_charset (anchor);
#if 0
  if (use_anchor)
    tmp_atom = HTAnchor_charset (anchor);
  else
    tmp_atom = HTResponse_charset (response);
#endif
  
  if (tmp_atom)
    {
      tmp_char = HTAtom_name (tmp_atom);
      if (tmp_char)
        me->http_headers.charset = TtaStrdup (tmp_char);
    }

  /* copy the content length */
  snprintf (tmp_string, sizeof (tmp_string), "%ld", HTAnchor_length (anchor));
  me->http_headers.content_length = TtaStrdup (tmp_string);

  /* copy the reason */
  if (status != HT_INTERRUPTED)
    {
      tmp_char = HTResponse_reason (response);
      if (tmp_char)
        me->http_headers.reason = TtaStrdup (tmp_char);
    }

  /* copy the content-location */
  tmp_char = HTAnchor_location (anchor);
  if (tmp_char && *tmp_char)
    {
      /* make a copy of the full location_header, for computing the base url */
      if (HTURL_isAbsolute (tmp_char))
        me->http_headers.full_content_location = TtaStrdup (tmp_char);
      else
        me->http_headers.full_content_location = AmayaParseUrl (tmp_char, me->urlName,
                                                                AMAYA_PARSE_ALL);
      tmp_char = me->http_headers.full_content_location;
      /* only include the filename. We suppose we have either a 
         relative or an absolute URL and that everything after the last
         slash is the doc name + ext */
      if (HTURL_isAbsolute (tmp_char))
        {
          tmp_char2 = tmp_char + strlen (tmp_char) - 1;
          while (*tmp_char2 != URL_SEP && tmp_char2 != tmp_char)
            tmp_char2--;
          if (tmp_char2 != tmp_char && *(tmp_char2 + 1))
            tmp_char = tmp_char2 + 1;
        }
      me->http_headers.content_location = TtaStrdup (tmp_char);
    }

  /* copy the status */
  me->http_headers.status = 0;

  cur = request->error_stack;
  while (me->http_headers.reason == NULL
         && (pres = (HTError *) HTList_nextObject (cur)))
    {
      index = HTError_index (pres);
      switch (index)
        {
        case HTERR_NO_REMOTE_HOST:
        case HTERR_SYSTEM:
        case HTERR_INTERNAL:
        case HTERR_TIME_OUT:
        case HTERR_CSO_SERVER:
          me->http_headers.status = index;
          if (me->http_headers.reason)
            TtaFreeMemory (me->http_headers.reason);
          me->http_headers.reason = TtaStrdup ("Cannot contact server");
          break;
        case HTERR_INTERRUPTED:
          me->http_headers.status = index;
          if (me->http_headers.reason)
            TtaFreeMemory (me->http_headers.reason);
          me->http_headers.reason = TtaStrdup ("Interrupted");
          break;
        }
    }
}

/*----------------------------------------------------------------------
  HTTP_headers_delete
  Deletes all the paramaters that were assigned to the response type
  ----------------------------------------------------------------------*/
static void HTTP_headers_delete (AHTHeaders me)
{
  if (me.content_type)
    TtaFreeMemory (me.content_type);

  if (me.charset)
    TtaFreeMemory (me.charset);

  if (me.content_length)
    TtaFreeMemory (me.content_length);
  
  if (me.reason)
    TtaFreeMemory (me.reason);

  if (me.content_location)
    TtaFreeMemory (me.content_location);

  if (me.full_content_location)
    TtaFreeMemory (me.full_content_location);
}

/*----------------------------------------------------------------------
  HTTP_headers
  Returns the value of a parameter in the HTTP response structure.
  Returns null if this structure is empty.
  ----------------------------------------------------------------------*/
char   *HTTP_headers (AHTHeaders *me, AHTHeaderName param)
{
  char   *result;

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
    case AM_HTTP_CONTENT_LENGTH:
      result = me->content_length;
      break;
    case AM_HTTP_REASON:
      result = me->reason;
      break;
    case AM_HTTP_CONTENT_LOCATION:
      result = me->content_location;
      break;
    case AM_HTTP_FULL_CONTENT_LOCATION:
      result = me->full_content_location;
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
  
  ----------------------------------------------------------------------
  MKP: this function is now used in davlib.c file for the WebDAV requests 
  ---------------------------------------------------------------------- */
AHTReqContext *AHTReqContext_new (int docid)
{
  AHTReqContext      *me;
  AHTDocId_Status    *docid_status;

  if ((me = (AHTReqContext *) TtaGetMemory (sizeof (AHTReqContext))) == NULL)
    return NULL;

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

#ifdef DAV                /* clean the DAV request context */
  me->dav_context = NULL; /* it should be create only when doing a DAV request */
#endif /* DAV */  

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
ThotBool  AHTReqContext_delete (AHTReqContext * me)
{
  AHTDocId_Status    *docid_status;

  if (me)
    {
#ifdef DEBUG_LIBWWW
      fprintf (stderr, "AHTReqContext_delete: Deleting object %p\n", me);
#endif   

#ifdef DAV /* if there is a DAV context object, delete it */
      if (me->dav_context)
        AHTDAVContext_delete ((AHTDAVContext*)(me->dav_context));
      me->dav_context = NULL;
#endif /* DAV */

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
              docid_status = NULL;
            }
        }

      HTNoFreeStream_delete (HTRequest_outputStream (me->request));
      HTRequest_setOutputStream (me->request, NULL); 
      /* JK: no longer needed in libwww 5.2.3
         if (me->method != METHOD_PUT && HTRequest_outputStream (me->request))
         AHTFWriter_FREE (HTRequest_outputStream (me->request));
      */
      HTRequest_delete (me->request);
      me->request = NULL;
      if (me->output && me->output != stdout)
        {	
#ifdef DEBUG_LIBWWW       
          fprintf (stderr, "AHTReqContext_delete: URL is  %s, closing "
                   "FILE %p\n", me->urlName, me->output); 
#endif
          TtaReadClose (me->output);
          me->output = NULL;
        }
	  
      if (me->error_stream != (char *) NULL)
        HT_FREE (me->error_stream);
      me->error_stream = NULL;
      
      if (me->reqStatus == HT_ABORT)
        {
          if (me->outputfile && me->outputfile[0] != EOS)
            {
              TtaFileUnlink ((const char *) &(me->outputfile[0]));
              me->outputfile[0] = EOS;
            }
        }
       
      if (me->urlName)
        TtaFreeMemory (me->urlName);
      me->urlName = NULL;
      if ((me->mode & AMAYA_ASYNC) || (me->mode & AMAYA_IASYNC))
        /* for the ASYNC mode, free the memory we allocated in GetObjectWWW
           or in PutObjectWWW */
        {
          if (me->outputfile)
            TtaFreeMemory (me->outputfile);
          me->outputfile = NULL;
        }

      /* the real name to which we are publishing */
      if (me->default_put_name)
        TtaFreeMemory (me->default_put_name);
      me->default_put_name = NULL;
      /* @@ temp change for the %esc conversion */
      /*if (me->method == METHOD_PUT && me->urlName)
        {
          TtaFreeMemory (me->urlName);
          me->urlName = NULL;
          }*/
      if (me->formdata)
        HTAssocList_delete (me->formdata);
      me->formdata = NULL;
      /* erase the response headers */
      HTTP_headers_delete (me->http_headers);
       
#ifdef ANNOTATIONS
      if (me->document)
        TtaFreeMemory (me->document);
      me->document = NULL;
#endif /* ANNOTATIONS */

      if (me->refdocUrl)
        TtaFreeMemory (me->refdocUrl);
      me->refdocUrl = NULL;
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
static void Thread_deleteAll (void)
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
#if defined(_GTK) || defined(_WX)
#ifdef WWW_XWINDOWS 
                RequestKillAllXtevents (me);
#endif /* WWW_XWINDOWS */
#endif /* defined(_GTK) || defined(_WX) */
                if (!HTRequest_kill (me->request))
                  AHTReqContext_delete (me);
              }
          }
	  
        /* erase the docid_status entities */
        while ((docid_status = (AHTDocId_Status *) HTList_removeLastObject ((HTList *) Amaya->docid_status)))
          TtaFreeMemory ((void *) docid_status);
      }
    }
}
 
/*----------------------------------------------------------------------
  AHTOpen_file
  ----------------------------------------------------------------------*/
int AHTOpen_file (HTRequest * request)
{
  AHTReqContext      *me;      /* current request */

  me = (AHTReqContext *)HTRequest_context (request);
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

  /* me->outputfile is only set when we want to store whatever result the server
     sends back */
  if (me->method == METHOD_PUT && !me->outputfile)
    {
      me->reqStatus = HT_WAITING;
      return HT_OK;
    }

  if (HTRequest_outputStream (me->request)) 
    {
#ifdef DEBUG_LIBWWW
      fprintf(stderr, "AHTOpen_file: output stream already existed for url %s\n",
              me->urlName);
#endif /* DEBUG_LIBWWW */      
      return HT_OK;
    }

#ifdef DEBUG_LIBWWW
  fprintf(stderr, "AHTOpen_file: opening output stream for url %s\n", me->urlName);
#endif /* DEBUG_LIBWWW */      

  if (!(me->output) && 
      (me->output != stdout) && 
      (me->output = TtaWriteOpen (me->outputfile)) == NULL)
    {
      me->outputfile[0] = EOS;	/* file could not be opened */
#ifdef DEBUG_LIBWWW
      fprintf(stderr, "AHTOpen_file: couldn't open output stream for url %s\n",
              me->urlName);
#endif
      TtaSetStatus (me->docid, 1,
                    TtaGetMessage (AMAYA, AM_CANNOT_CREATE_FILE),
                    me->outputfile);
      me->reqStatus = HT_ERR;
      return (HT_ERROR);
    }
	  
  HTRequest_setOutputStream (me->request,
                             AHTFWriter_new (me->request, me->output, YES));
  me->reqStatus = HT_WAITING;

  return HT_OK;
}

/*----------------------------------------------------------------------
  SafePut_init
  Sets up the domains which are authorized to make a redirect on 
  a PUT.
  ----------------------------------------------------------------------*/
static void SafePut_init (void)
{
  char     *strptr;
  char     *ptr, *ptr2;
  char     *domain;

  /* get the proxy settings from the thot.ini file */
  strptr = TtaGetEnvString ("SAFE_PUT_REDIRECT");
  if (strptr && *strptr)
    {
      /* Get copy we can mutilate */
      ptr2 = TtaStrdup (strptr);
      /* convert to lowercase */
      ptr = ptr2;
      while (*ptr) 
        {
          *ptr = tolower (*ptr);
          ptr++;
        }
      
      /* create the list container */
      safeput_list = HTList_new ();
 
      /* store the domain list */
      ptr = ptr2;
      while ((domain = HTNextField (&ptr)) != NULL)
        HTList_addObject (safeput_list, TtaStrdup (domain)); 

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
  returns true if the domain to which the URL belongs accepts an automatic
  PUT redirect.
  ----------------------------------------------------------------------*/
ThotBool SafePut_query (char *url)
{
  HTList   *cur;
  char     *me;
  ThotBool  found;
  char      tmp[MAX_LENGTH];

  /* extract the domain path of the url and normalize it */
  /* domain = url; */
  cur = safeput_list;
  found = FALSE;

  while ((me = (char *) HTList_nextObject (cur)))
    {
      strcpy (tmp, me);
      if (strstr (url, tmp))
        {
          found = TRUE;
          break;
        } 
    }

  return (found);
}

/*----------------------------------------------------------------------
  redirection_handler
  this function is registered to handle permanent and temporary
  redirections.
  ----------------------------------------------------------------------*/
static int redirection_handler (HTRequest *request, HTResponse *response,
                                void *param, int status)
{
  HTAnchor           *new_anchor = HTResponse_redirection (response);
  AHTReqContext      *me = (AHTReqContext *)HTRequest_context (request);
  HTMethod            method = HTRequest_method (request);
  char               *ref;
  char               *escape_src, *dst;
  char                urlAdr[MAX_LENGTH];

  if (!me)
    /* if the redirect doesn't come from Amaya, we call libwww's standard redirect filter */
    return (HTRedirectFilter (request, response, param, status));
   
  if (!new_anchor)
    {
      if (PROT_TRACE)
        HTTrace ("Redirection. No destination\n");
      return HT_OK;
    }

  /*
  ** Only do redirect on GET, HEAD, and authorized domains for PUT
  */
  if ((me->method == METHOD_PUT && !SafePut_query (me->urlName)) ||
      (me->method != METHOD_PUT && !HTMethod_isSafe (method)))
    {
      /*
      ** If we got a 303 See Other then change the method to GET.
      ** Otherwise ask the user whether we should continue.
      */
      if (status == HT_SEE_OTHER)
        {
          if (PROT_TRACE)
            HTTrace("Redirection. Changing method from %s to GET\n", HTMethod_name(method));
          HTRequest_setMethod(request, METHOD_GET);
        }
      else
        {
          HTAlertCallback    *prompt = HTAlert_find (HT_A_CONFIRM);
          if (prompt)
            {
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
  if (HTRequest_doRetry (request))
    {
      /*
      ** Start request with new credentials 
      */
      /* only do a redirect using a network protocol understood by Amaya */
      strncpy (urlAdr, new_anchor->parent->address, MAX_LENGTH - 1);
      urlAdr[MAX_LENGTH - 1] = EOS;
      if (me->urlName && !strcmp (me->urlName, urlAdr))
        {
          /* redirect to itself */
          me->reqStatus = HT_ABORT;
          /* and we return HT_OK so that the terminate_handler
             will be called */
          return HT_OK;
        }
      if (IsValidProtocol (urlAdr))
        {
          /* if it's a valid URL, we try to normalize it */
          /* We use the pre-redirection anchor as a base name */
          /* @@ how to obtain this address here? */
          dst = urlAdr;
          escape_src = EscapeURL (me->urlName);
          if (escape_src)
            {
              ref = AmayaParseUrl (dst, escape_src, AMAYA_PARSE_ALL);
              if (me->method != METHOD_PUT)
                AHTRequest_setRefererHeader (me);
              TtaFreeMemory (escape_src);
              escape_src = NULL;
            }
          else
            ref = NULL;
	   
          if (ref)
            {
              HTAnchor_setPhysical (HTAnchor_parent (new_anchor), ref);
              TtaFreeMemory (ref);
              ref = NULL;
            }
          else
            return HT_OK; /* We can't redirect anymore */
        }
      else
        return HT_OK; /* We can't redirect anymore */
       
      /* update the current file name */
      if ((me->mode & AMAYA_ASYNC) || (me->mode & AMAYA_IASYNC) ||
          (me->method == METHOD_PUT))
        {
          TtaFreeMemory (me->urlName);
          me->urlName = TtaStrdup (urlAdr);
        }
      else
        {
          /* it's a SYNC mode, so we should keep the urlName */
          TtaFreeMemory (me->urlName);
          me->urlName = TtaStrdup (urlAdr);
        }
      ChopURL (me->status_urlName, me->urlName);

      /* @@ verify if this is important */
      /* @@@ new libwww doesn't need this free stream while making
         a PUT. Is it the case everywhere or just for PUT? */
      if (me->method != METHOD_PUT &&
          me->request->orig_output_stream != NULL)
        {
          AHTFWriter_FREE (me->request->orig_output_stream);
          me->request->orig_output_stream = NULL;
          if (me->output != stdout)
            {
              /* Are we writing to a file? */
#ifdef DEBUG_LIBWWW
              fprintf (stderr, "redirection_handler: New URL is  %s, closing FILE %p\n",
                       me->urlName, me->output); 
#endif 
              TtaReadClose (me->output);
              me->output = NULL;
            }
        }
       
      /* tell the user what we're doing */
      TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_RED_FETCHING),
                    me->status_urlName);
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

#ifdef DAV
      /* remove the old header "If" (WebDAV lock information) */
      DAVRemoveIfHeader (me);
       
      /* search lock information for the new url */
      if (me->method == METHOD_POST || me->method == METHOD_PUT) 
        DAVAddIfHeader (me, me->urlName);
#endif /* DAV */      
       
      if (me->method == METHOD_POST || me->method == METHOD_PUT)
        {
          /* PUT, POST etc. */
          /* for PUT, we memorize there was a redirection, so that we 
             can clear the original cache entry in the terminate_handler */
          if (me->method == METHOD_PUT)
            me->put_redirection = TRUE;
          status = HTLoadAbsolute (me->urlName, request);
        }
      else
        HTLoadAnchor (new_anchor, request);
    }
  else
    {
      HTRequest_addError (request, ERR_FATAL, NO, HTERR_MAX_REDIRECT, NULL, 0, (char*)"HTRedirectFilter");
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
static int precondition_handler (HTRequest *request, HTResponse *response,
                                 void *context, int status)
{
  AHTReqContext      *me = (AHTReqContext *) HTRequest_context (request);
  HTAlertCallback    *prompt = HTAlert_find (HT_A_CONFIRM);
  ThotBool            force_put;

  if (!me)
    return HT_OK;		/* not an Amaya request */

  if (prompt)
    {
      if (me->method == METHOD_GET)
        {
          /* @@@@ IV */
          (*prompt) (request, HT_A_CONFIRM, status, NULL, NULL, NULL);
          force_put = NO;
        }
      else
	      force_put = ((*prompt)(request, HT_A_CONFIRM, HT_MSG_RULES, NULL, NULL, NULL) != 0);
    }
  else
    force_put = NO;
  
  if (force_put)
    {
#ifdef DAV
      BOOL noIf = NO;

      /* MKP: if the old request has preconditions, *
       * then, we supose that these preconditions   *
       * caused the 412 Precondition Failed status  *
       * code, otherwise we supose that the cause   *
       * was an eventual If header.                 */
      if (HTRequest_preconditions(me->request) != HT_NO_MATCH)
        noIf = NO;
      else
        noIf = YES;      
#endif /* DAV */
      
      
      /* start a new PUT request without preconditions */
      /* @@ do we need to kill the request? */
      if (me->output && me->output != stdout)
        {
          TtaReadClose (me->output);
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

      
#ifdef DAV
      /* MKP: add an If header (lock information) only 
       * if there wasn't preconditions */
      if (noIf != YES)
        DAVAddIfHeader (me,HTAnchor_address(me->dest));
#endif /* DAV */
      
      
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
   
#ifdef DAV
      /* MKP: try to add an "If" header (lock information) 
       *      Such header will be added only if there is a
       *      lock information in the local base. */
      DAVAddIfHeader (me, me->urlName);   
#endif /* DAV */


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
	      force_put = ((*prompt)(request, HT_A_CONFIRM, HT_MSG_FILE_REPLACE, NULL, NULL, NULL) != 0);
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
   
#ifdef DAV
          /* MKP: try to add an "If" header (lock information) 
           *      Such header will be added only if there is a
           *      lock information in the local base. */ 
          DAVAddIfHeader (me,HTAnchor_address(me->dest));   
#endif /* DAV */

	  
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
/*--------------------------- -------------------------------------------
  terminate_handler
  this function is registered to handle the result of the request
  ----------------------------------------------------------------------*/
static int terminate_handler (HTRequest *request, HTResponse *response,
                              void *context, int status)
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
      /* JK: removed it as the kludge doesn't seem useful anymore */
      /* kludge for work around libwww problem */
      /* || (status == HT_INTERRUPTED && me->method == METHOD_PUT) */
#ifdef AMAYA_WWW_CACHE
      /* what status to use to know we're downloading from a cache? */
      || status ==  HT_NOT_MODIFIED
      || status == HT_PARTIAL_CONTENT
#endif /* AMAYA_WWW_CACHE */
      || me->reqStatus == HT_ABORT)
    error_flag = FALSE;
#ifdef DAV
  else if (status == HT_LOCKED 
           || status ==  HT_FAILED_DEPENDENCY 
           || status == HT_MULTI_STATUS) 
    {
      /* WebDAV return codes - they are handled in
       * specific filters. We don't need to deal
       * with them anymore
       */
      error_flag = FALSE;  	   
    }
#endif   /* DAV */
  else
    error_flag = TRUE;

  /* If we did a PUT that was redirected, clean the original
     cache entry */
  if (status == 204 && me->method == METHOD_PUT && me->put_redirection)
    {
      HTCache * cache;
      HTParentAnchor * anchor = HTAnchor_parent (me->dest);
      cache = HTCache_find (anchor, NULL);
      if (cache)
        {
          /* what a problem... we update the cache with the wrong data
             from the response... after the redirection */
          HTCache_resetMeta (cache, request, response);
        }
      else
        /* If entry doesn't already exist then create a new entry */
        HTCache_touch(request, response, HTAnchor_parent (me->dest));
    }

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
        {			
          /* if the request was not aborted and
             there were some errors and we want to print them */

          /* JK: 30/04/2004. The 201 reponse from the annotation server
             is being stored inside the error flag! */
          if ((status == 201 && me->mode & AMAYA_FILE_POST && me->error_stream)
              || (error_flag && me->error_html == TRUE))
            {		
              /* if the transfer was interrupted, the file may not be
                 empty. So, we erase it */
              fflush (me->output);
              rewind (me->output);

              if (me->error_stream)
                {	/* if the stream is non-empty */
                  fprintf (me->output, "%s", me->error_stream);/* output the errors */
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
      TtaWriteClose (me->output);
      me->output = NULL;
    }

  /* a very special handling so that we can put a message box
     to tell the user WHAT he has just done... */
  if (UserAborted_flag && me->method == METHOD_PUT)
    {
      HTRequest_addError (request, ERR_FATAL, NO, HTERR_INTERRUPTED,
                          (void *)"Operation aborted by user", 0, NULL);
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
    TtaSetStatus (me->docid, 1,
                  TtaGetMessage (AMAYA, AM_ELEMENT_LOADED),
                  me->status_urlName);
   
  /* don't remove or Xt will hang up during the PUT */
  if (AmayaIsAlive ()  &&
      (me->method == METHOD_POST || me->method == METHOD_PUT))
    PrintTerminateStatus (me, status);
  ProcessTerminateRequest (request, response, context, status);
  /* stop here */
  return HT_ERROR;
}

/*----------------------------------------------------------------------
  AHTLoadTerminate_handler
  this is an application "AFTER" Callback. It's called by the library
  when a request has ended, so that we can setup the correct status.
  ----------------------------------------------------------------------*/
int AHTLoadTerminate_handler (HTRequest *request, HTResponse *response,
                              void *param, int status)
{
  /** @@@@ use this with printstatus ?? */
  AHTReqContext      *me = (AHTReqContext *)HTRequest_context (request);
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

      if (docid_status && docid_status->counter > 1)
        TtaSetStatus (me->docid, 1, 
                      TtaGetMessage (AMAYA, AM_ELEMENT_LOADED),
                      me->status_urlName);
      break;
	       
    case HT_NO_DATA:
      if (PROT_TRACE)
        HTTrace ("Load End.... OK BUT NO DATA: `%s\'\n", 
                 me->status_urlName);
#ifdef DAV      
      if (me->method==METHOD_UNLOCK) 
        {
          /* MKP: set an appropriate status message */
          TtaSetStatus (me->docid, 1, 
                        TtaGetMessage (AMAYA, AM_UNLOCK_SUCCEED),
                        NULL);
        }
      else
#endif /* DAV */
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

    case HT_NOT_MODIFIED:
      if (PROT_TRACE)
        HTTrace ("Load End.... NOT MODIFIED (%s)",
                 me->status_urlName ? me->status_urlName :" <UNKNOWN>");
      break;
       
    case HT_ERROR:
      cbf = HTAlert_find (HT_A_MESSAGE);
      if (cbf)
        (*cbf) (request, HT_A_MESSAGE, HT_MSG_NULL, NULL,
                HTRequest_error (request), NULL);
      if (PROT_TRACE)
        HTTrace ("Load End.... ERROR: Can't access `%s\'\n",
                 me->status_urlName ? me->status_urlName :"<UNKNOWN>"); 
      TtaSetStatus (me->docid, 1,
                    TtaGetMessage (AMAYA, AM_CANNOT_LOAD),
                    me->status_urlName ? me->status_urlName : (char *)"<UNKNOWN>");
      break;

    case HTERR_TIMEOUT:
      cbf = HTAlert_find (HT_A_MESSAGE);
      if (cbf)
        (*cbf) (request, HT_A_MESSAGE, HT_MSG_NULL, NULL,
                HTRequest_error (request), NULL);
      if (PROT_TRACE)
        HTTrace ("Load End.... REQUEST TIMEOUT: Can't access `%s\'\n",
                 me->status_urlName ? me->status_urlName :"<UNKNOWN>"); 
      TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_CANNOT_LOAD),
                    me->status_urlName ? me->status_urlName : (char *)"<UNKNOWN>");
      break;

    default:
      if (PROT_TRACE)
        HTTrace ("Load End.... UNKNOWN RETURN CODE %d\n", status);
      break;
    }
   
  return HT_OK;
}

/*----------------------------------------------------------------------
  LineTrace
  Makes a file copy of libwww's traces
  ----------------------------------------------------------------------*/
static  int LineTrace (const char * fmt, va_list pArgs)
{
  vfprintf (trace_fp, fmt, pArgs);
  return (fflush (trace_fp));
}

/*----------------------------------------------------------------------
  AHTAcceptTypesInit
  This function prepares the Accept header used by Amaya during
  the HTTP content negotiation phase
  ----------------------------------------------------------------------*/
static void           AHTAcceptTypesInit (HTList *c)
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

  /* Register additional bindings */
  HTBind_add("htm", "text/html",  NULL, "8bit", NULL, 1.0);
  HTBind_add("tgz", "application/gnutar",  NULL, "binary", NULL, 1.0);
  HTBind_add("mml", AM_MATHML_MIME_TYPE,  NULL, "8bit", NULL, 1.0);
  HTBind_add("svg", AM_SVG_MIME_TYPE,  NULL, "8bit", NULL, 1.0);
  HTBind_add("xsl", "text/xml",  NULL, "8bit", NULL, 1.0);
  /* Don't do any case distinction */
  HTBind_caseSensitive (FALSE);
}

/*----------------------------------------------------------------------
  AHTAcceptLanguagesInit
  This function prepares the Accept header used by Amaya during
  the HTTP content negotiation phase
  ----------------------------------------------------------------------*/
static void         AHTAcceptLanguagesInit (HTList *c)
{
  char             *ptr;
  char             *lang_list;
  char              s[3];
  int               count, lg;
  double            quality;
  ThotBool          still;

  if (c == (HTList *) NULL) 
    return;
  
  lang_list = TtaGetEnvString ("ACCEPT_LANGUAGES");
  s[2] = EOS;

  if (lang_list == NULL)
    lang_list = TtaGetLanguageCode(TtaGetDefaultLanguage());

  if (lang_list && *lang_list != EOS)
    {
      /* add the default language first  */
      HTLanguage_add (c, "*", -1.0);
      /* how many languages do we have? */
      ptr = lang_list;
      count = 0;
      while (*ptr != EOS)
        {
          while (*ptr != EOS &&
                 (*ptr < 'A' || (*ptr > 'Z' && *ptr < 'a') || *ptr > 'z'))
            /* skip the whole separator */
            ptr++;
          lg = 0;
          while ((*ptr >= 'A' &&
                  *ptr <= 'Z') || (*ptr >= 'a' && *ptr <= 'z') || *ptr == '-')
            {
              /* it's a new language */
              ptr++;
              lg++;
            }
          if (lg >= 2)
            count++;
          if (*ptr != EOS)
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
      while (count)
        {
          while (still && (*ptr < 'A' || (*ptr > 'Z' && *ptr < 'a') || *ptr > 'z'))
            /* skip the whole separator */
            if (ptr > lang_list)
              ptr--;
            else
              still = FALSE;
          lg = 0;
          while (still &&
                 ((*ptr >= 'A' &&
                   *ptr <= 'Z') || (*ptr >= 'a' && *ptr <= 'z') || *ptr == '-'))
            {
              /* it's a new language */
              if (ptr > lang_list)
                ptr--;
              else
                still = FALSE;
              lg++;
            }
          if (lg >= 2)
            {
              if (still)
                strncpy  (s, &ptr[1], 2);
              else
                strncpy (s, lang_list, 2);
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
static void  AHTConverterInit (HTList *c)
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
#endif /*AMAYA_WWW_CACHE*/

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
  char *strptr;

  /* 
     NB. Preemptive == YES means Blocking requests
     Non-preemptive == NO means Non-blocking requests
  */
  HTTransport_add("tcp", HT_TP_SINGLE, HTReader_new, HTWriter_new);
  HTTransport_add("buffered_tcp", HT_TP_SINGLE, HTReader_new, 
                  HTBufferWriter_new);
  HTProtocol_add ("http", "buffered_tcp", HTTP_PORT, NO, HTLoadHTTP, NULL);
#ifdef _WINDOWS
  /* TODO: verifier que le param YES est adapte pour WX */
  HTProtocol_add ("file", "local", 0, YES, HTLoadFile, NULL);
#endif /* _WINDOWS */
#if defined(_UNIX)
  HTProtocol_add ("file", "local", 0, NO, HTLoadFile, NULL);
#endif /* #if defined(_UNIX) */

#ifdef AMAYA_WWW_CACHE
  HTProtocol_add("cache",  "local", 0, YES, HTLoadCache, NULL);
#endif /* AMAYA_WWW_CACHE */
  HTProtocol_add ("ftp", "tcp", FTP_PORT, NO, HTLoadFTP, NULL);

  /* initialize pipelining */
  strptr = TtaGetEnvString ("ENABLE_PIPELINING");
  if (strptr && *strptr && strcasecmp (strptr, "yes"))
    HTTP_setConnectionMode (HTTP_11_NO_PIPELINING);
}

#ifdef SSL
/*----------------------------------------------------------------------
  AHTHTTPSInit
  SSL initialization
  ----------------------------------------------------------------------*/
static void        AHTHTTPSInit (void)
{
  /* Set the SSL protocol method. By default, it is the highest available
     protocol. Setting it up to SSL_V23 allows the client to negotiate
     with the server and set up either TSLv1, SSLv3 or SSLv2  */
  HTSSL_protMethod_set (HTSSL_V23);
  
  /* Set the certificate verification depth to 2 in order to be able to
     validate self signed certificates */
  HTSSL_verifyDepth_set (2);
  
  /* Register SSL stuff for handling ssl access */
  // HTSSLhttps_init(YES);
  HTSSLhttps_init(NO);
}
#endif /* SSL */

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
#ifdef SSL
  HTNet_addBefore (HTPEP_beforeFilter,	"http://*", NULL, HT_FILTER_LATE);
#endif /* SSL */
  HTHost_setActivateRequestCallback (AHTOpen_file);

  /*      register AFTER filters
  **      The AFTER filters handle error messages, logging, redirection,
  **      authentication etc.
  **      The filters are called in the order by which the are registered
  **      Not done automaticly - may be done by application!
  */

  HTNet_addAfter (HTAuthFilter, "http://*", NULL, HT_NO_ACCESS, HT_FILTER_MIDDLE);
  HTNet_addAfter (HTAuthFilter, "http://*", NULL, HT_REAUTH, HT_FILTER_MIDDLE);
#ifdef SSL
  HTNet_addAfter (HTPEP_afterFilter,	"http://*", NULL, HT_ALL, HT_FILTER_MIDDLE);
#endif /* SSL */
  HTNet_addAfter (redirection_handler, "http://*", NULL, HT_PERM_REDIRECT, HT_FILTER_MIDDLE);
  HTNet_addAfter (redirection_handler, "http://*", NULL, HT_FOUND, HT_FILTER_MIDDLE);
  HTNet_addAfter (redirection_handler, "http://*", NULL, HT_SEE_OTHER, HT_FILTER_MIDDLE);
  HTNet_addAfter (redirection_handler, "http://*", NULL, HT_TEMP_REDIRECT, HT_FILTER_MIDDLE);
  HTNet_addAfter (HTAuthInfoFilter, 	"http://*", NULL, HT_ALL, HT_FILTER_MIDDLE);
  HTNet_addAfter (HTUseProxyFilter, "http://*", NULL, HT_USE_PROXY, HT_FILTER_MIDDLE);
#ifdef AMAYA_LOST_UPDATE
  HTNet_addAfter (precondition_handler, NULL, NULL, HT_PRECONDITION_FAILED, HT_FILTER_MIDDLE);
#endif /* AMAYA_LOST_UPDATE */

#ifdef SSL
  /* A rajouter ?? */
  //HTNet_addAfter (HTAuthInfoFilter, 	"https://*", NULL, HT_ALL, HT_FILTER_MIDDLE);
#endif /* SSL */

#if defined(_GTK) || defined(_WX)
  HTNet_addAfter (AHTLoadTerminate_handler, NULL, NULL, HT_ALL, HT_FILTER_LAST);
#endif /* defined(_GTK) || defined(_WX) */
  
  /**** for later ?? ****/
  /*  HTNet_addAfter(HTInfoFilter,NULL, NULL, HT_ALL, HT_FILTER_LATE); */
  /* @@ JK: Filters for doing ftp authentication */
  HTNet_addAfter (HTAuthFilter, "ftp://*", NULL, HT_NO_ACCESS, HT_FILTER_MIDDLE);
  HTNet_addAfter (HTAuthFilter, "ftp://*", NULL, HT_REAUTH, HT_FILTER_MIDDLE);
  /* handles all errors */
  HTNet_addAfter (terminate_handler, NULL, NULL, HT_ALL, HT_FILTER_LAST);
}

/*----------------------------------------------------------------------
  AHTAlertInit
  Register alert messages and their callbacks.
  ----------------------------------------------------------------------*/
static void         AHTAlertInit (void)
{
  HTAlert_add (AHTProgress, HT_A_PROGRESS);

#ifdef _WINDOWS
  HTAlert_add ((HTAlertCallback *) WIN_Activate_Request, HT_PROG_CONNECT);
#endif /* _WINDOWS */
  HTAlert_add (AHTPrompt, HT_A_PROMPT);
  HTAlert_add (AHTPromptUsernameAndPassword, HT_A_USER_PW);
   
  HTAlert_add (AHTError_print, HT_A_MESSAGE);
  HTError_setShow ( (HTErrorShow)(~((unsigned int) 0 ) & ~((unsigned int) HT_ERR_SHOW_DEBUG)) );	/* process all messages except debug ones*/
  HTAlert_add (AHTConfirm, HT_A_CONFIRM);
}

#ifdef AMAYA_WWW_CACHE
/*----------------------------------------------------------------------
  RecCleanCache
  Clears an existing cache directory
  ----------------------------------------------------------------------*/
static ThotBool RecCleanCache (char *dirname)
{
  char     buf[MAX_LENGTH];
  wxString name, path;
  wxString wx_dir_name = wxString(dirname, *wxConvCurrent);
  bool     cont;

  /* try to delete the current directory */
  if (TtaDirectoryUnlink (dirname))
    return TRUE;

  /* try to delete the files & directorys inside */
  wxDir wx_dir(wx_dir_name);
  cont = wx_dir.GetFirst (&name);
  while (cont)
    {
      path = wx_dir_name+name;
      //printf ("%s\n", (const char *)path.mb_str(*wxConvCurrent));
      // get the next name
      cont = wx_dir.GetNext (&name);
      if (wxDirExists (path))
	{
	  /* it's a sub-directory */
	  strcpy (buf, (const char *)path.mb_str(*wxConvCurrent));
	  strcat (buf, DIR_STR);
	  /* delete it recursively */
	  RecCleanCache(buf);
	}
      else
	/* it's a file */
	wxRemoveFile(path);
    }
  /* try to delete the current directory */
  if (TtaDirectoryUnlink (dirname))
    return TRUE;
  return FALSE;
}
#endif /* AMAYA_WWW_CACHE */

/*----------------------------------------------------------------------
  libwww_CleanCache
  frontend to the recursive cache cleaning function
  ----------------------------------------------------------------------*/
void libwww_CleanCache (void)
{
#ifdef AMAYA_WWW_CACHE
  char    *real_dir, *cache_dir, *tmp, *ptr;
  int      cache_size, cache_expire, cache_disconnect;
  int      retry = 20;

  if (!HTCacheMode_enabled ())
    /* don't do anything if we're not using a cache */
    return;
  /* temporarily close down the cache, purge it, then restart */
  tmp = HTCacheMode_getRoot ();
  /* don't do anything if we don't have a valid cache dir */
  if (!tmp || *tmp == EOS)
	  return;
  cache_dir = TtaStrdup (tmp);
  TtaFreeMemory (tmp);
  cache_size = HTCacheMode_maxSize ();
  cache_expire = HTCacheMode_expires ();
  cache_disconnect = HTCacheMode_disconnected ();

  /* get something we can work on :) */
  tmp = HTWWWToLocal (cache_dir, "file:", NULL);
  real_dir = (char *)TtaGetMemory (strlen (tmp) + 20);
  strcpy (real_dir, tmp);
  TtaFreeMemory (tmp);

  /* safeguard... abort the operation if cache_dir doesn't end with
     CACHE_DIR_NAME */
  ptr = strstr (real_dir, CACHE_DIR_NAME);  
  if (ptr == NULL || *ptr == EOS || strcasecmp (ptr, CACHE_DIR_NAME))
    return;
  
  /* remove the concurrent cache lock */
#ifdef DEBUG_LIBWWW
  fprintf (stderr, "Clearing the cache lock\n");
#endif /* DEBUG_LIBWWW */
  clear_cachelock ();
  HTCacheTerminate ();
  HTCacheMode_setEnabled (FALSE);
  // On Mac OS X it's necessary to relaunch the clean cache
  while (retry > 0 && !RecCleanCache (real_dir))
    retry --;
  HTCacheMode_setExpires ((HTExpiresMode)cache_expire);
  HTCacheMode_setDisconnected ((HTDisconnectedMode)cache_disconnect);
  HTCacheInit (cache_dir, cache_size);
  /* set a new concurrent cache lock */
  strcat (real_dir, ".lock");
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

/*----------------------------------------------------------------------
  CacheInit
  Reads the cache settings from the thot.ini file.
  ----------------------------------------------------------------------*/
static void CacheInit (void)
{
#ifndef AMAYA_WWW_CACHE
  HTCacheMode_setEnabled (NO);
#else /* AMAYA_WWW_CACHE */
  char     *ptr = NULL;
  char     *real_dir = NULL;
  char     *cache_lockfile;
  char     *cache_dir = NULL;
  int       cache_size, i;
  int       cache_entry_size;
  ThotBool  cache_enabled;
  ThotBool  cache_locked;
  ThotBool  tmp_bool;

  /* activate cache? */
  ptr = TtaGetEnvString ("ENABLE_CACHE");
  if (ptr && *ptr && strcasecmp (ptr, "yes"))
    cache_enabled = NO;
  else
    cache_enabled = YES;

  /* cache protected documents? */
  ptr = TtaGetEnvString ("CACHE_PROTECTED_DOCS");
  if (ptr && *ptr && !strcasecmp (ptr, "yes"))
    HTCacheMode_setProtected (YES);
  else
    HTCacheMode_setProtected (NO);

  /* get the cache dir (or use a default one) */
  ptr = TtaGetRealFileName(TtaGetEnvString ("CACHE_DIR"));
  if (ptr && *ptr) 
    {
      real_dir = (char *)TtaGetMemory (strlen (ptr) + strlen (CACHE_DIR_NAME) + 20);
      strcpy (real_dir, ptr);
      if (*(real_dir + strlen (real_dir) - 1) != DIR_SEP)
        strcat (real_dir, DIR_STR);
      TtaFreeMemory(ptr);
      ptr = NULL;
    }
  else
    {
      real_dir = (char *)TtaGetMemory (strlen (TempFileDirectory) + strlen (CACHE_DIR_NAME) + 20);
      sprintf (real_dir, "%s%s", TempFileDirectory, CACHE_DIR_NAME);
    }

  /* compatiblity with previous versions of Amaya: does real_dir
     include CACHE_DIR_NAME? If not, add it */
  ptr = strstr (real_dir, CACHE_DIR_NAME);
  if (!ptr)
    strcat (real_dir, CACHE_DIR_NAME);
  else
    {
      i = strlen (CACHE_DIR_NAME);
      if (ptr[i] != EOS)
        strcat (real_dir, CACHE_DIR_NAME);
    }

  /* convert the local cache dir into a file URL, as expected by
     libwww */
  cache_dir = HTLocalToWWW (real_dir, "file:");
  /* get the cache size (or use a default one) */
  ptr = TtaGetEnvString ("CACHE_SIZE");
  if (ptr && *ptr)
    cache_size = atoi (ptr);
  else
    cache_size = DEFAULT_CACHE_SIZE;
  /* get the max cached file size (or use a default one) */
  if (!TtaGetEnvInt ("MAX_CACHE_ENTRY_SIZE", &cache_entry_size))
    cache_entry_size = DEFAULT_MAX_CACHE_ENTRY_SIZE;
  if (cache_enabled) 
    {
      /* how to remove the lock? force remove it? */
      cache_lockfile = (char *)TtaGetMemory (strlen (real_dir) + 20);
      strcpy (cache_lockfile, real_dir);
      strcat (cache_lockfile, ".lock");
      cache_locked = FALSE;
      if (TtaFileExist(cache_lockfile))
	{
	  cache_locked = (test_cachelock(cache_lockfile) != 0);
	  if (!cache_locked)
	    {
#ifdef DEBUG_LIBWWW
	      fprintf (stderr, "found a stale cache, removing it\n");
#endif /* DEBUG_LIBWWW */
	      /* remove the lock and clean the cache (the clean cache 
		 will remove all, making the following call unnecessary */
	      /* little trick to win some memory */
	      ptr = strrchr (cache_lockfile, '.');
	      *ptr = EOS;
	      RecCleanCache (cache_lockfile);
	      *ptr = '.';
	    }
	}

      if (!cache_locked) 
        {
          /* initialize the cache if there's no other amaya
             instance running */
          HTCacheMode_setMaxCacheEntrySize (cache_entry_size);
	  TtaGetEnvBoolean ("CACHE_EXPIRE_IGNORE", &tmp_bool);
          if (tmp_bool)
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

  TtaFreeMemory (cache_dir);
  TtaFreeMemory (real_dir);
  /* warn the user if the cache isn't active */
  if (cache_enabled && !HTCacheMode_enabled ())
    {
      ThotBool warn_user;

      TtaGetEnvBoolean ("WARN_NO_CACHE", &warn_user);
      if (warn_user)
        InitInfo ("Cache", TtaGetMessage (AMAYA, AM_CANT_CREATE_CACHE));
    }
#endif /* AMAYA_WWW_CACHE */
}

/*----------------------------------------------------------------------
  ProxyInit
  Reads any proxies settings which may be declared as environmental
  variables or in the thot.ini file. The former overrides the latter.
  ----------------------------------------------------------------------*/
static void ProxyInit (void)
{
  char     *ptr;
  char     *name;
  char     *tmp = NULL;
  char     *ptrA;
  ThotBool  proxy_is_onlyproxy;

  /* get the proxy settings from the thot.ini file */
  ptr = TtaGetEnvString ("HTTP_PROXY");
  if (ptr && *ptr)
    {
      tmp = (char *) TtaGetMemory (strlen (ptr) + 1);
      strcpy (tmp, ptr);

      /* does the proxy env string has an "http://" prefix? */
      if (!strncasecmp (ptr, "http://", 7)) 
        HTProxy_add ("http", tmp);
      else 
        {
          ptrA = (char *) TtaGetMemory (strlen (ptr) + 9);
          strcpy (ptrA, "http://");
          strcat (ptrA, tmp);
          HTProxy_add ("http", ptrA);
          TtaFreeMemory (ptrA);
        }
      TtaFreeMemory (tmp);
    }

  /* get the no_proxy settings from the thot.ini file */
  ptr = TtaGetEnvString ("PROXYDOMAIN");
  if (ptr && *ptr) 
    {
      ptrA = (char *) TtaGetMemory (strlen (ptr) + 1);
      tmp = ptrA;
      strcpy (tmp, ptr);
      /* as HTNextField changes the ptr we pass as an argument, we'll
         work with another variable, so that we can free the tmp
         block later on */
      while ((name = HTNextField (&tmp)) != NULL) 
        {
          char* portstr = strchr (name, ':');
          unsigned port=0;
          if (portstr)
            { 
              *portstr++ = EOS;
              if (*portstr) 
                port = (unsigned) atoi (portstr);
            }
          /* Register it for all access methods */
          HTNoProxy_add (name, NULL, port);
        }
      TtaFreeMemory (ptrA);
    }
  
  /* how should we interpret the proxy domain list? */

  TtaGetEnvBoolean ("PROXYDOMAIN_IS_ONLYPROXY", &proxy_is_onlyproxy);
  HTProxy_setNoProxyIsOnlyProxy (proxy_is_onlyproxy);

  /* use libwww's routine to get all proxy settings from the environment */
  HTProxy_getEnvVar ();
}

/*----------------------------------------------------------------------
  AHTProfile_newAmaya
  creates the Amaya client profile for libwww.
  ----------------------------------------------------------------------*/
static void AHTProfile_newAmaya (const char *AppName, const char *AppVersion)
{
  char *ptr;

  /* If the Library is not already initialized then do it */
  if (!HTLib_isInitialized ()) 
    HTLibInit (AppName, AppVersion);

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
  ptr = TtaGetEnvString ("ENABLE_MDA");
  if (!ptr || (ptr && *ptr && strcasecmp (ptr, "no")))
    HTAA_newModule ("digest", HTDigest_generate, HTDigest_parse, HTDigest_updateInfo, HTDigest_delete);

#ifdef SSL
  /* SSL initialization */
  AHTHTTPSInit ();
#endif /* SSL */
   
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
  /* commented because we don't know yet how to handle deflate in the content enconding */
  /* HTContentEncoderInit (content_encodings); */
  HTCoding_add (content_encodings, "gzip", NULL, HTIdentityCoding, 1.0);
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
static void         AHTProfile_delete (void)
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
  HTTimer_deleteAll ();
  HTEventList_unregisterAll ();
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
static void AmayaContextInit ()
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
void QueryInit ()
{
  char   *ptr;
  int     tmp_i;
  long    tmp_l;

  AmayaContextInit ();
  AHTProfile_newAmaya (TtaGetAppName(), TtaGetAppVersion());
  CanDoStop_set (TRUE);
  UserAborted_flag = FALSE;
#ifdef _WX
  wxAmayaSocketEventLoop::InitSocketLib();
  HTEvent_setRegisterCallback ( AHTEvent_register);
  HTEvent_setUnregisterCallback (AHTEvent_unregister);
  HTTimer_registerSetTimerCallback ((BOOL (*)(HTTimer*)) AMAYA_SetTimer);
  HTTimer_registerDeleteTimerCallback ((BOOL (*)(HTTimer*))AMAYA_DeleteTimer);
#endif /* _WX */
#ifdef HTDEBUG
  /* an undocumented option for being able to generate an HTTP protocol
     trace.  The flag can take values from 1-10, which are interpreted as
     different kinds of debug traces. Value 99 means a complete trace.
     0 or other values means No Debug.1 No Debug (default), up to Full debug, respectively. */
  if (TtaGetEnvInt ("ENABLE_LIBWWW_DEBUG", &tmp_i))
    {
      switch (tmp_i)
        {
        case 1:
          WWW_TraceFlag = SHOW_URI_TRACE;
          break;
        case 2:
          WWW_TraceFlag = SHOW_BIND_TRACE;
          break;
        case 3:
          WWW_TraceFlag = SHOW_THREAD_TRACE;
          break;
        case 4:
          WWW_TraceFlag = SHOW_STREAM_TRACE;
          break;
        case 5:
          WWW_TraceFlag = SHOW_PROTOCOL_TRACE;
          break;
        case 6:
          WWW_TraceFlag = SHOW_MEM_TRACE;
          break;
        case 7:
          WWW_TraceFlag = SHOW_URI_TRACE;
          break;
        case 8:
          WWW_TraceFlag = SHOW_AUTH_TRACE;
          break;
        case 9:
          WWW_TraceFlag = SHOW_ANCHOR_TRACE;
          break;
        case 10 :
          WWW_TraceFlag = SHOW_CORE_TRACE; 
          break;
        case 11 :
          WWW_TraceFlag = SHOW_CORE_TRACE | SHOW_ANCHOR_TRACE | SHOW_AUTH_TRACE | SHOW_URI_TRACE | SHOW_THREAD_TRACE | SHOW_STREAM_TRACE;
          break;
        case 99 :
          WWW_TraceFlag = SHOW_ALL_TRACE;
          break;
        default:
          WWW_TraceFlag = 0;
          break;
        }
      if (WWW_TraceFlag)
        {
          /* Trace activation (for debugging) */
          char   *s, *tmp;
          s = TtaGetEnvString ("APP_TMPDIR");
          tmp = (char *)TtaGetMemory (strlen (s) + sizeof ("/libwww.log") + 1);
          strcpy (tmp, s);
          strcat (tmp, "/libwww.log");
          trace_fp = fopen (tmp, "ab");
          TtaFreeMemory (tmp);
          if (trace_fp)
            HTTrace_setCallback(LineTrace);
        }
    }
  else
    WWW_TraceFlag = 0;
#endif /* HTDEBUG */

#ifdef DEBUG_LIBWWW
  /* forwards error messages to our own function */
  WWW_TraceFlag = THD_TRACE;
  HTTrace_setCallback(LineTrace);

  /***
      WWW_TraceFlag = SHOW_CORE_TRACE | SHOW_THREAD_TRACE | SHOW_PROTOCOL_TRACE;
  ***/
#endif

  //TtaGetEnvBoolean ("ENABLE_FTP", &FTPURL_flag);
  FTPURL_flag = TRUE;
  /* Setting up different network parameters */

  /* Maximum number of simultaneous open sockets */
  ptr = TtaGetEnvString ("MAX_SOCKET");
  if (ptr && *ptr) 
    tmp_i = atoi (ptr);
  else
    tmp_i = DEFAULT_MAX_SOCKET;
  HTNet_setMaxSocket (tmp_i);

  /* different network services timeouts */
  /* dns timeout */
  ptr = TtaGetEnvString ("DNS_TIMEOUT");
  if (ptr && *ptr) 
    tmp_i = atoi (ptr);
  else
    tmp_i = DEFAULT_DNS_TIMEOUT;
  HTDNS_setTimeout (tmp_i);

  /* persistent connections timeout */
  ptr = TtaGetEnvString ("PERSIST_CX_TIMEOUT");
  if (ptr && *ptr) 
    tmp_l = atol (ptr); 
  else
    tmp_l = DEFAULT_PERSIST_TIMEOUT;
  HTHost_setPersistTimeout (tmp_l);

  /* default timeout in ms */
  ptr = TtaGetEnvString ("NET_EVENT_TIMEOUT");
  if (ptr && *ptr) 
    tmp_i = atoi (ptr);
  else
    tmp_i = DEFAULT_NET_EVENT_TIMEOUT;
  HTHost_setEventTimeout (tmp_i);

  HTRequest_setMaxRetry (8);
#ifdef CATCH_SIG
  signal (SIGPIPE, SIG_IGN);
#endif

#ifdef SSL
  ptr = TtaGetEnvString ("BUF_SIZE_SOCKET");
  if (ptr && *ptr) {
    int bufSize;
    bufSize = atoi (ptr);
    TtaSetEnvInt ("BUF_SIZE_SOCKET", bufSize, TRUE);
    bufSize = bufSize * 1024;
    HTSetSocketBufSize (bufSize);
  }
  else {
    HTUnSetSocketBufSize ();
  }
#endif /* SSL */
}

static AHTReqContext *LoopRequest= NULL;
/*----------------------------------------------------------------------
  LoopForStop
  a copy of the Thop event loop so we can handle the stop button in Unix
  and preemptive requests under Windows
  ----------------------------------------------------------------------*/
static int LoopForStop (AHTReqContext *me)
{
  int  status_req = YES;
  int  count = 0;
  
#ifdef _WX
  ThotEvent                ev;

  // register the current request
  LoopRequest = me;
  /* to test the async calls  */
  /* Loop while waiting for new events, exists when the request is over */
  while (me->reqStatus != HT_ABORT &&
         me->reqStatus != HT_END &&
         me->reqStatus != HT_ERR)
    {
      if (!AmayaIsAlive ())
        /* Amaya was killed by one of the callback handlers */
        exit (0);
      if (TtaFetchOneAvailableEvent (&ev))
        TtaHandleOneEvent (&ev);
      if (me->method == METHOD_GET)
        {
          if (count < 1300)
            count ++; // no more retries
          else
            me->reqStatus = HT_ABORT;
        }
      /* this is necessary for synchronous request*/
      /* check the socket stats */
      if (me->reqStatus != HT_ABORT)
        // the request is not aborted
        wxAmayaSocketEvent::CheckSocketStatus( 500 );
    }
#endif /* _WX */

  switch (me->reqStatus)
    {
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
  // Clean up the current request
  LoopRequest = NULL;
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
#if defined(_GTK) || defined(_WX)
  /** need to erase all existing timers too **/
  HTTimer_registerSetTimerCallback (NULL);
  HTTimer_registerDeleteTimerCallback (NULL);
#endif /* defined(_GTK) || defined(_WX) */
  HTHost_setActivateRequestCallback (NULL);
  Thread_deleteAll ();
 
  HTProxy_deleteAll ();
  HTNoProxy_deleteAll ();
  SafePut_delete ();
  HTGateway_deleteAll ();
  
#ifdef SSL
  /* Close down SSL */
  HTSSLhttps_terminate();
#endif /* SSL */

  AHTProfile_delete ();

  /* close the trace file (if it exists) */
  if (trace_fp)
    fclose (trace_fp);

}

/*----------------------------------------------------------------------
  NextNameValue
  ---------------------------------------------------------------------*/
static char * NextNameValue (char **pstr, char **name, char **value)
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
static   HTAssocList * PrepareFormdata (const char *string)
{
  char          *tmp_string, *tmp_string_ptr;
  char          *name;
  char          *value;
  HTAssocList   *formdata;

  if (!string)
    return NULL;

  /* store the ptr in another variable, as the original address will
     change
  */
  
  tmp_string_ptr = (char *)TtaGetMemory (strlen (string) + 1);
  tmp_string = tmp_string_ptr;
  strcpy (tmp_string_ptr, string);
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
void AHTRequest_setRefererHeader (AHTReqContext  *me)
{				
  HTRequest *request;
  HTRqHd     rqhd;
  ThotBool   referer;

  TtaGetEnvBoolean ("SEND_REFERER", &referer);
  if (referer && me->refdocUrl && me->refdocUrl[0] != EOS
      && IsHTTPPath (me->refdocUrl))
    {
      request = me->request;
      rqhd = HTRequest_rqHd (request);
      rqhd = (HTRqHd)((int)rqhd & (~HT_C_REFERER));      

      HTRequest_setRqHd (request, rqhd);
      HTRequest_addExtraHeader (request, (char*)"Referer", (char*)me->refdocUrl);
    }
}

/*----------------------------------------------------------------------
  ---------------------------------------------------------------------*/
void AHTRequest_setCustomAcceptHeader (HTRequest *request, const char *value)
{				
  HTRqHd rqhd = HTRequest_rqHd (request);
  rqhd = (HTRqHd)((int)rqhd & (~HT_C_ACCEPT_TYPE));
  HTRequest_setRqHd (request, rqhd);
  HTRequest_addExtraHeader (request, (char*)"Accept", (char*)value);
}

/*----------------------------------------------------------------------
  GetOutputFileName
  Generates a temporary filename (based on a counter) for storing
  the information that a server sends back.
  ----------------------------------------------------------------------*/
static void GetOutputFileName (char *outputfile, int tempsubdir)
{
  sprintf (outputfile, "%s%c%d%c%04dAM", TempFileDirectory, DIR_SEP, tempsubdir, DIR_SEP,
           object_counter);
  /* update the object_counter (used for the tempfilename) */
  object_counter++;
}

/*----------------------------------------------------------------------
  InvokeGetObjectWWW_callback
  A simple function to invoke a callback function whenever there's an error
  in GetObjectWWW
  ---------------------------------------------------------------------*/
void InvokeGetObjectWWW_callback (int docid, char *urlName,
                                  char *outputfile, TTcbf *terminate_cbf,
                                  void *context_tcbf, int status)
{
  if (!terminate_cbf)
    return;
  
  (*terminate_cbf) (docid, status, urlName, outputfile, NULL, NULL, context_tcbf);  
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
  - refdoc Document identifier to refer.
  - urlName The utf8-coded URL to be retrieved (MAX_URL_LENGTH chars length)
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
int GetObjectWWW (int docid, int refdoc, char *urlName, const char *formdata,
                  char *outputfile, int mode, TIcbf *incremental_cbf, 
                  void *context_icbf, TTcbf *terminate_cbf, 
                  void *context_tcbf, ThotBool error_html, const char *content_type)
{
  AHTReqContext      *me;
  char               *ref;
  char               *esc_url;
  int                 status, l;
  int                 tempsubdir;
  ThotBool            bool_tmp, referer;

  if (urlName == NULL || outputfile == NULL) 
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
  if (!strncmp ("internal:", urlName, 9)) 
    {
      strcpy (outputfile, urlName);
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

  /* we store CSS in subdir named 0; all the other files go to a subdir
     named after their own docid */
  tempsubdir = (mode & AMAYA_LOAD_CSS) ? 0 : docid;
  /* create a tempfilename */
  GetOutputFileName (outputfile, tempsubdir);

  /* normalize the URL */
  esc_url = EscapeURL (urlName);
  if (esc_url) 
    {
      ref = AmayaParseUrl (esc_url, "", AMAYA_PARSE_ALL);
      TtaFreeMemory (esc_url);
    }
  else
    ref = NULL;

  /* should we abort the request if we could not normalize the url? */
  if (ref == NULL || ref[0] == EOS)
    {
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
#ifdef ANNOTATIONS
      /* we support the DELETE method for deleting annotations.
         the rest of the request is similar to the GET */
      if (mode & AMAYA_DELETE)
        {
          me->method = METHOD_GET;
          HTRequest_setMethod (me->request, METHOD_DELETE);
        }
      else
#endif /* ANNOTATIONS */
        me->method = METHOD_GET;
#ifdef ANNOTATIONS
      /* use the custom sent content_type */
      if (content_type && content_type[0] != EOS)
        AHTRequest_setCustomAcceptHeader (me->request, content_type);
      else
#endif /* ANNOTATIONS */
        if (!HasKnownFileSuffix (ref))
          {
            /* try to adjust the Accept header in an netwise economical way */
            if (mode & AMAYA_LOAD_IMAGE)
              AHTRequest_setCustomAcceptHeader (me->request, IMAGE_ACCEPT_NEGOTIATION);
            else if (mode & AMAYA_LOAD_CSS)
              AHTRequest_setCustomAcceptHeader (me->request, "*/*;q=0.1,css/*");
            else
              AHTRequest_setCustomAcceptHeader (me->request, GENERAL_ACCEPT_NEGOTIATION);
            /*
              HTRequest_setConversion(me->request, acceptTypes, TRUE);
            */
          }
        else
          AHTRequest_setCustomAcceptHeader (me->request, GENERAL_ACCEPT_NEGOTIATION);
      /* IV 13/08/2003 */
      TtaGetEnvBoolean ("SEND_REFERER", &referer);
      if (referer && refdoc && DocumentURLs[refdoc] &&
          IsHTTPPath (DocumentURLs[refdoc]))
        {
          me->refdocUrl = TtaStrdup (DocumentURLs[refdoc]);
          AHTRequest_setRefererHeader (me);
        }
      /* language negotiation */
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
      l = strlen (outputfile);
      if (l > MAX_LENGTH)
        me->outputfile = (char *)TtaGetMemory (l + 2);
      else
        me->outputfile = (char *)TtaGetMemory (MAX_LENGTH + 2);
      strcpy (me->outputfile, outputfile);
      l = strlen (urlName);
      if (l > MAX_LENGTH)
        me->urlName = (char *)TtaGetMemory (l + 2);
      else
        me->urlName = (char *)TtaGetMemory (MAX_LENGTH + 2);
      strcpy (me->urlName, urlName);
      HTRequest_setPreemptive (me->request, NO);
    } /* AMAYA_ASYNC mode */ 
  else 
    {
      me->outputfile = outputfile;
      me->urlName = TtaStrdup (urlName);
#ifdef _WINDOWS
      /* force windows SYNC requests to always be non preemptive */
      HTRequest_setPreemptive (me->request, YES);
#else /* _WINDOWS */
      /***
          In order to take into account the stop button, 
          the requests will be always asynchronous, however, if mode=AMAYA_SYNC,
          we will loop until the document has been received or a stop signal
          generated
      ****/
      HTRequest_setPreemptive (me->request, NO);
#endif /* _WINDOWS */
    }

  /*
  ** Make sure that the first request is flushed immediately and not
  ** buffered in the output buffer
  */
  if (mode & AMAYA_FLUSH_REQUEST)
    HTRequest_setFlush(me->request, YES);
  HTRequest_setFlush(me->request, YES);

  /* prepare the URLname that will be displayed in the status bar */
  ChopURL (me->status_urlName, me->urlName);
  TtaSetStatus (me->docid, 1, 
                TtaGetMessage (AMAYA, AM_FETCHING),
                me->status_urlName);

  me->anchor = (HTParentAnchor *) HTAnchor_findAddress (ref);
  TtaFreeMemory (ref);
  ref = NULL;
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

       
#ifdef DAV 
  /* try to add an "If" header (lock information) also for POST 
   * requests and for GET forms */
  if (HTRequest_method(me->request) == METHOD_POST ||
      (HTRequest_method(me->request) == METHOD_GET && me->formdata))	   
    DAVAddIfHeader (me,HTAnchor_address((HTAnchor*)me->anchor));
#endif /* DAV */
       
   
  /* do the request */
  if (mode & AMAYA_FORM_POST)
    {
      /* this call doesn't give back a ThotBool */
      HTParentAnchor * posted = NULL;
      posted = HTPostFormAnchor (me->formdata, (HTAnchor *) me->anchor, 
                                 me->request);
      status = posted ? YES : NO; 
    }
  else if (mode & AMAYA_FILE_POST)
    {
      unsigned long  filesize;
      char          *fileURL;

      /* @@@ a very ugly patch :)))
         I'm copying here some of the functionality I use in the PUT
         I need to put the common parts in another module */
      AM_GetFileSize (formdata, &filesize);
      me->block_size = filesize;
      fileURL = HTParse (formdata, "file:/", PARSE_ALL);
      me->source = HTAnchor_findAddress (fileURL);
      HT_FREE (fileURL);
      AHTRequest_setCustomAcceptHeader (me->request, content_type);
      HTAnchor_setFormat (HTAnchor_parent (me->source),
                          HTAtom_for (content_type));
      HTAnchor_setFormat (me->anchor,
                          HTAtom_for (content_type));
      HTAnchor_setLength ((HTParentAnchor *) me->source, me->block_size);
      /* @@ here I need to actually read the file and put it in document,
         then, when I kill the request, I need to kill it */
      {
        FILE *fp;
        int i;
        char c;

        me->document = (char *)TtaGetMemory (me->block_size + 1);
        fp = TtaReadOpen (formdata);
        i = 0; 
        c = getc (fp);
        while (!feof (fp))
          {
            me->document[i++] = c;
            c = getc (fp);
          }
        me->document[i] = EOS;
        TtaReadClose (fp);
      }
      HTAnchor_setDocument ( (HTParentAnchor *) me->source,
                             (void * ) me->document);
      HTRequest_setEntityAnchor (me->request, HTAnchor_parent (me->source));
 
      status = HTPostAnchor (HTAnchor_parent (me->source), 
                             (HTAnchor *) me->anchor, 
                             me->request);
    }
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
        /* wait here until the asynchronous request finishes */
        SetStopButton (docid);
        status = LoopForStop (me);
        FilesLoading[docid]++;
        ResetStop(docid);
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
int PutObjectWWW (int docid, char *fileName, char *urlName, 
                  const char *contentType, char *outputfile, int mode,
                  TTcbf *terminate_cbf, void *context_tcbf)
{
  AHTReqContext      *me;
  CHARSET             charset;
  HTParentAnchor     *dest_anc_parent;
  int                 status;
  unsigned long       file_size = 0;
  char               *fileURL;
  char               *etag = NULL;
  char               *esc_url;
  int                 UsePreconditions;
  char                url_name[MAX_LENGTH];
  char               *resource_name, *localfilename;
  char               *tmp2;
  char                file_name[MAX_LENGTH];
  ThotBool            lost_update_check = FALSE;

  if (mode & AMAYA_SIMPLE_PUT)
    {
      UsePreconditions = FALSE;
      if (!outputfile)
        return HT_ERROR;
    }
  else
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
  if (!AM_GetFileSize (fileName, &file_size) || file_size == 0L)
    /* file was empty */
    return (HT_ERROR);

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

  /* Set up the original URL name */
  /* are we using content-location? */
  if (DocumentMeta[docid]->content_location)
    resource_name = DocumentMeta[docid]->content_location;
  else
    resource_name = NULL;

  /* prepare the target URL */
  if (resource_name)
    {
      char *tmp = strstr (urlName, resource_name);     
      if (!tmp)
        {
          /* urlName does not include the resource name */
          me->default_put_name = (char *)TtaGetMemory (strlen (urlName)
                                                       + strlen (resource_name)
                                                       + sizeof (URL_SEP)
                                                       + 1);
          strcpy (me->default_put_name, urlName);
          tmp = strrchr (me->default_put_name, URL_SEP);
          if (tmp)
            {
              /* it is a URL finishing in /. Only add the resource name */
              tmp++;
              strcpy (tmp, resource_name);
            }
          else
            {
              strcat (me->default_put_name, URL_STR);
              strcat (me->default_put_name, resource_name);
            }
        }
    }
   
  if (!me->default_put_name)
    me->default_put_name = TtaStrdup (urlName);
   
  HTRequest_setDefaultPutName (me->request, me->default_put_name);

  me->mode = mode;
  me->incremental_cbf = (TIcbf *) NULL;
  me->context_icbf = (void *) NULL;
  me->terminate_cbf = terminate_cbf;
  me->context_tcbf = context_tcbf;

  /* normalize the URL */
  esc_url = EscapeURL (urlName);
  me->urlName = TtaStrdup (esc_url);
  TtaFreeMemory (esc_url);

  me->block_size =  file_size;

#ifdef LC
#ifdef SSL
  HTSetSocketBufSize (file_size);
#endif /* SSL */
#endif /* LC */

  /* select the parameters that distinguish a PUT from a GET/POST */
  me->method = METHOD_PUT;
  if (mode & AMAYA_SIMPLE_PUT)
    {
      me->output = NULL;
      GetOutputFileName (outputfile, docid);
      me->outputfile = outputfile;
    }
  else
    {
      me->output = stdout;
      /* we are not expecting to receive any input from the server */
      me->outputfile = (char  *) NULL; 
    }

  localfilename = TtaGetRealFileName (fileName);
  /* @@IV 18/08/2004 eencode spaces in the local filename */
  fileURL = EscapeURL (localfilename);
  TtaFreeMemory(localfilename);
  if (fileURL)
    {
      strcpy (file_name, fileURL);
      TtaFreeMemory (fileURL);
    }
  else
    strcpy (file_name, fileName);
  fileURL = NULL;

#ifdef _WINDOWS
  /* libwww's HTParse function doesn't take into account the drive name;
     so we sidestep it */
  StrAllocCopy (fileURL, "file:");
  StrAllocCat (fileURL, file_name);
#endif /* _WINDOWS */
#ifdef _UNIX
  fileURL = HTParse (file_name, "file:/", PARSE_ALL);
#endif /* _UNIX */

  me->source = HTAnchor_findAddress (fileURL);
  HT_FREE (fileURL);
  strcpy (url_name, me->urlName);
  me->dest = HTAnchor_findAddress (url_name);
  /* we memorize the anchor's parent @ as we use it a number of times
     in the following lines */
  dest_anc_parent = HTAnchor_parent (me->dest);

  /* Set the Content-Type of the file we are uploading  */
  /* we try to use any content-type previosuly associated
     with the parent. If it doesn't exist, we try to guess it
     from the URL */
  /* @@ JK: trying to use the content type we stored */
  tmp2 = NULL;
  if (!tmp2 || !strcmp (tmp2, "www/unknown"))
    {
      HTAtom *tmp_atom;
      char   *s = NULL;
       
      tmp_atom = AHTGuessAtom_for (me->urlName, contentType);
      if (tmp_atom)
        s = HTAtom_name (tmp_atom);
      if (!tmp_atom || (s && !strcmp (s, "www/unknown")))
        {
          /* ask the user for a MIME type */
        }
      HTAnchor_setFormat (dest_anc_parent, tmp_atom);
      tmp2 = HTAtom_name (HTAnchor_format (dest_anc_parent));
    }
  /* .. and we give the same type to the source anchor */
  /* we go thru setOutputFormat, rather than change the parent's
     anchor, as that's the place that libwww expects it to be */
  HTAnchor_setFormat (HTAnchor_parent (me->source), HTAtom_for (tmp2));
  HTRequest_setOutputFormat (me->request, HTAtom_for (tmp2));

  /*  Set the Charset of the file we are uploading  */
  /* we set the charset as indicated in the document's metadata
     structure (and only if it exists) */
  charset = TtaGetDocumentCharset (docid);
  if (charset != UNDEFINED_CHARSET)
    {
      const char *tmp =  TtaGetCharsetName (charset);
      if (tmp && *tmp != EOS)
        {
          tmp2 = TtaStrdup (tmp);
          HTAnchor_setCharset (dest_anc_parent, HTAtom_for (tmp2));
          TtaFreeMemory (tmp2);
          tmp2 = HTAtom_name (HTAnchor_charset (dest_anc_parent));
          /* .. and we give the same charset to the source anchor */
          /* we go thru setCharSet, rather than change the parent's
             anchor, as that's the place that libwww expects it to be */
          HTAnchor_setCharset (HTAnchor_parent (me->source),
                               HTAtom_for (tmp2));
        }
    }

  /* define other request characteristics  */
  HTRequest_setPreemptive (me->request, NO);

  /* Make sure that the first request is flushed immediately and not
     buffered in the output buffer */
  if (mode & AMAYA_FLUSH_REQUEST)
    HTRequest_setFlush(me->request, YES);
   
  /* Should we use preconditions? */
  if (UsePreconditions)
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
  else
    {
      //TtaGetEnvBoolean ("ENABLE_LOST_UPDATE_CHECK", &lost_update_check);
      if (lost_update_check)
        etag = HTAnchor_etag (HTAnchor_parent (me->dest));
      if (etag)
        HTRequest_setPreconditions(me->request, HT_MATCH_THIS);
      else
        /* don't use preconditions */
        HTRequest_setPreconditions(me->request, HT_NO_MATCH);
    }
   
  /* don't use the cache while saving a document */
  HTRequest_setReloadMode (me->request, HT_CACHE_FLUSH);

#if 0
  /* Throw away any reponse body */
  HTRequest_setOutputStream (me->request, NULL);
#endif

  /* prepare the URLname that will be displayed in the status bar */
  ChopURL (me->status_urlName, me->urlName);
  TtaSetStatus (me->docid, 1, TtaGetMessage (AMAYA, AM_REMOTE_SAVING),
                me->status_urlName);

#ifdef DAV
  /* MKP: for a PUT request, try to add an "If" header (lock information)
   * for a HEAD request, leave this for check_handler */
  //if (!UsePreconditions)
  DAVAddIfHeader (me,HTAnchor_address(me->dest));   
#endif /* DAV */

   
  /* make the request */
  if (UsePreconditions)
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
void StopRequest (int docid)
{
  if (Amaya && CanDoStop ())
    { 
       /* temporary call to stop all requests, as libwww changed its API */
      StopAllRequests (docid);
    }
}

/* @@@ the docid parameter isn't used... clean it up */
/*----------------------------------------------------------------------
  StopAllRequests
  stops (kills) all active requests. We use the docid 
  ----------------------------------------------------------------------*/
void StopAllRequests (int docid)
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
      // is there a current LoopForStop?
      if (LoopRequest)
        LoopRequest->reqStatus = HT_ABORT;
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
      CallbackDialogue (BaseDialog + FormAnswer,  STRING_DATA, NULL);
      CallbackDialogue (BaseDialog + ConfirmForm, INTEGER_DATA, NULL);
      /* expire all outstanding timers */
        HTTimer_expireAll ();
      /* HTNet_killAll (); */
      if (Amaya->open_requests)
        {
          cur = Amaya->reqlist;
          while ((me = (AHTReqContext *) HTList_nextObject (cur))) 
            {
              if (AmayaIsAlive ())
                {
#ifdef DEBUG_LIBWWW
                  fprintf (stderr,"StopRequest: killing req %p, url %s, status %d\n", me,
                           me->urlName, me->reqStatus);
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
                            (*me->terminate_cbf) (me->docid, -1, &(me->urlName[0]),
                                                  &(me->outputfile[0]),
                                                  NULL, NULL,
                                                  me->context_tcbf);
			   
                          if (async_flag) 
                            /* explicitly free the request context for async
                               requests. The sync requests context is freed
                               by LoopForStop */
                            AHTReqContext_delete (me);
                        }
                      cur = Amaya->reqlist;
                    }
#if defined(_GTK) || defined(_WX)
#ifdef WWW_XWINDOWS
                  /* to be on the safe side, remove all outstanding 
                     X events */
                  else 
                    RequestKillAllXtevents (me);
#endif /* WWW_XWINDOWS */
#endif /* defined(_GTK) || defined(_WX) */
                }
            }
          /* Delete remaining channels */
          HTChannel_safeDeleteAll ();
        }
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
ThotBool AmayaIsAlive (void)
{
  return AmayaAlive_flag;
}

/*----------------------------------------------------------------------
  CanDoStop
  returns the value of the CanDoStop flag
  ----------------------------------------------------------------------*/
ThotBool CanDoStop (void)
{
  return CanDoStop_flag;
}

/*----------------------------------------------------------------------
  CanDoStop_set
  sets the value of the CanDoStop flag
  ----------------------------------------------------------------------*/
void CanDoStop_set (ThotBool value)
{
  CanDoStop_flag = value;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void libwww_updateNetworkConf (int status)
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

/****************************************************************************
 ** The following two functions allow to set and read the value of          *
 ** the  FTPURL_flag. If this flag is true, it means we can browse FTP URLs *
 ***************************************************************************/

/*----------------------------------------------------------------------
  AHT_FTPURL_flag_set
  ----------------------------------------------------------------------*/
void AHTFTPURL_flag_set (ThotBool value)
{
  FTPURL_flag = value;
}

/*----------------------------------------------------------------------
  AHT_FTPURL_flag
  ----------------------------------------------------------------------*/
ThotBool AHTFTPURL_flag (void)
{
  return (FTPURL_flag);
}

/*----------------------------------------------------------------------
  FreeAmayaCache 
  ----------------------------------------------------------------------*/
void FreeAmayaCache (void)
{
#ifdef _UNIX
  char str[MAX_LENGTH];
  pid_t pid;

  /* unregister this instance of Amaya */
  pid = getpid ();
  sprintf (str, "%s/pid/%d", TempFileDirectory, pid);
  if (TtaFileExist (str))
    TtaFileUnlink (str);
#endif /* _UNIX */
}

/*----------------------------------------------------------------------
  InitAmayaCache 
  Create the temporary sub-directories for storing the HTML and
  other downloaded files.
  ----------------------------------------------------------------------*/
void InitAmayaCache (void)
{
  ThotBool can_erase;
  int i;
  char str[MAX_LENGTH];
  char *ptr;
#ifdef _UNIX
  pid_t pid;
  int fd_pid;

  /* create the temp dir for the Amaya pid */
  sprintf (str, "%s%cpid", TempFileDirectory, DIR_SEP);
  TtaMakeDirectory (str);
#endif /* _UNIX */

  /* protection against dir names that have . in them to avoid
     erasing everything  by accident */
  ptr = TempFileDirectory;
  can_erase = TRUE;
#ifdef _UNIX
  while (*ptr && can_erase)
    {
      if (*ptr == '.')
        {
          if (*(ptr + 1) == '.')
            can_erase = FALSE;
          else
            ptr++;
          continue;
        }
      else if (*ptr == '~')
        can_erase = FALSE;
      else if (!isalnum (*ptr) && *ptr != SPACE && *ptr != DIR_SEP && *ptr != ':')
        can_erase = FALSE;
      ptr++;
    }
#endif /* _UNIX */

  /* Detect if it's a unique instance */
  AmayaUniqueInstance = TRUE;
  if (can_erase && AmayaUniqueInstance)
    {
      /* Erase the previous directories */
      for (i = 0; i < DocumentTableLength; i++)
        {      
          sprintf (str, "%s%c%d%c", TempFileDirectory, DIR_SEP, i, DIR_SEP);
          RecCleanCache (str);
        }
    }

  /* create the temporary cache directories if they don't exit*/
  for (i = 0; i < DocumentTableLength; i++)
    {      
      sprintf (str, "%s%c%d", TempFileDirectory, DIR_SEP, i);
      TtaMakeDirectory (str);
    }

#ifdef _UNIX
  /* register this instance of Amaya */
  pid = getpid ();
  sprintf (str, "%s/pid/%d", TempFileDirectory, pid);
  fd_pid = open (str, O_CREAT, S_IRUSR | S_IWUSR );
  if (fd_pid != -1)
    close (fd_pid);
  else
    printf ("Couldn't create fd_pid %s\n", str);
#endif /* _UNIX */
}

/*-----------------------------------------------------------------------
  ClearCacheEntry
  Clears the libwww cache entry for the corresponding URL, if it exists.
  -----------------------------------------------------------------------*/
void ClearCacheEntry (char *url)
{
  HTCache * cache;
  HTAnchor * anchor;
  HTParentAnchor * panchor;

  if (!url || !*url)
    return;
  
  anchor = HTAnchor_findAddress (url);
  panchor = HTAnchor_parent (anchor);
  cache = HTCache_find (panchor, NULL);
  if (cache)
    HTCache_remove (cache);
}
