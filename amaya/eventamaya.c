/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * eventamaya.c : front end for the network requests from Amaya.
 *      Request are document based, i.e. concerns a full URL
 *      transfert. This involves queuing incoming request, 
 *      especially the associated callback, moving request to
 *      active request lists, removing them once completed and
 *      executing the associated callback.
 *      Before queuing the request, the front-end function checks for
 *      any possible errors, and load any object needed in memory.
 *      Once everything is ready, the Availability of a request
 *      is signified to the Event manager (eventloop.c) and the request
 *      can return.
 */

#define EXPORT
#include "amaya.h"
#include "netamaya.h"
#include "eventamaya.h"

#include "AHTURLTools_f.h"

static AmayaNetRequestPtr NetRequestList = NULL; /* linked list of Request */

static int          object_counter = 0;	/* loaded objects counter */

#define DEBUG_NET_REQUEST */ /* switch on verbose debugging and checking */

#ifdef DEBUG_NET_REQUEST

/*----------------------------------------------------------------------
  CheckNetRequest
  check a network request.
  ----------------------------------------------------------------------*/
 
#ifdef __STDC__
void                CheckNetRequest (AmayaNetRequestPtr cur)
#else
void                CheckNetRequest (cur)
AmayaNetRequestPtr  cur;
#endif
{
    if (cur == NULL) return;
    switch (cur->state) {
        case AMAYA_NET_STATE_NONE:
	    fprintf(stderr,"CheckNetRequest : state is NONE\n");
	    return;
	case AMAYA_NET_STATE_STARTED:
	case AMAYA_NET_STATE_FAILED:
	    break;
	case AMAYA_NET_STATE_QUEUED:
	    if ((cur->prev == NULL) || (cur->next == NULL)) {
		fprintf(stderr,"CheckNetRequest : linked request : NULL links\n");
	        return;
	    }
	    break;
	default : 
	    fprintf(stderr,"CheckNetRequest : invalid state\n");
	    return;
    }
    if ((cur->prev != NULL) || (cur->next != NULL)) {
        if ((cur->prev == NULL) || (cur->next == NULL)) {
	    fprintf(stderr,"CheckNetRequest : only one link set\n");
	    return;
	}
        if (cur->prev->next != cur) {
	    fprintf(stderr,"CheckNetRequest : backward link loop corrupted\n");
	    return;
	}
        if (cur->next->prev != cur) {
	    fprintf(stderr,"CheckNetRequest : backward link loop corrupted\n");
	    return;
	}
    }
    switch (cur->type) {
        case AMAYA_NET_TYPE_NONE:
	    fprintf(stderr,"CheckNetRequest : type is NONE\n");
	    return;
	case AMAYA_NET_TYPE_GET:
	case AMAYA_NET_TYPE_PUT:
	case AMAYA_NET_TYPE_FORM_POST:
	case AMAYA_NET_TYPE_FORM_GET:
	    break;
	default : 
	    fprintf(stderr,"CheckNetRequest : invalid type\n");
	    return;
    }
    switch (cur->mode) {
        case AMAYA_NET_MODE_NONE:
	    fprintf(stderr,"CheckNetRequest : mode is NONE\n");
	    return;
	case AMAYA_NET_MODE_ASYNC:
	case AMAYA_NET_MODE_IASYNC:
	default : 
	    fprintf(stderr,"CheckNetRequest : invalid mode\n");
	    return;
    }
}				/* CheckNetRequest */

#define CHECK_NET_REQUEST(cur) CheckNetRequest(cur);

#else /* !DEBUG_NET_REQUEST */

#define CHECK_NET_REQUEST(cur)

#endif /* DEBUG_NET_REQUEST */


/*----------------------------------------------------------------------
  NewNetRequest
  Allocate and initialize a new request.
  ----------------------------------------------------------------------*/
 
#ifdef __STDC__
AmayaNetRequestPtr  NewNetRequest (void)
#else
AmayaNetRequestPtr  NewNetRequest ()
#endif
{
    AmayaNetRequestPtr cur = TtaGetMemory(sizeof(AmayaNetRequest));

    if (cur == NULL) return(NULL);

    cur->prev = cur->next = NULL;
    cur->state = AMAYA_NET_STATE_NONE;
    cur->type = AMAYA_NET_TYPE_NONE;
    cur->mode = AMAYA_NET_MODE_NONE;
    cur->callback = NULL;
    cur->blk.doc = 0;
    cur->blk.outputfile = NULL;
    cur->blk.data = NULL;
    cur->blk.size = -1;
    cur->blk.url[0] = '\0';
    return(cur);
}

/*----------------------------------------------------------------------
  FreeNetRequest
  Free an old request.
  ----------------------------------------------------------------------*/
 
#ifdef __STDC__
void  FreeNetRequest (AmayaNetRequestPtr cur)
#else
void  FreeNetRequest (cur)
AmayaNetRequestPtr cur;
#endif
{
    if ((cur->prev != NULL) || (cur->next != NULL)) {
        fprintf(stderr,"FreeNetRequest : linked request\n");
	return;
    }
    CHECK_NET_REQUEST(cur)
    if (cur->blk.data != NULL) TtaFreeMemory(cur->blk.data);
    TtaFreeMemory(cur);
}

/*----------------------------------------------------------------------
  AddNetRequest
  Add a request to the queue of pending request for Net accesses.
  ----------------------------------------------------------------------*/
 
#ifdef __STDC__
void                AddNetRequest (AmayaNetRequestPtr cur)
#else
void                AddNetRequest (cur)
AmayaNetRequestPtr  cur;
#endif
{
    if (cur == NULL) return;
    if ((cur->prev != NULL) || (cur->next != NULL)) {
        fprintf(stderr,"AddNetRequest : linked request\n");
	return;
    }

    CHECK_NET_REQUEST(cur)
    CHECK_NET_REQUEST(NetRequestList)

    cur->state = AMAYA_NET_STATE_QUEUED;
    if (NetRequestList == NULL) {
        NetRequestList = cur;
	cur->next = cur->prev = cur;
    } else {
        cur->prev = NetRequestList->prev;
	cur->next = NetRequestList;
	cur->prev->next = cur;
	cur->next->prev = cur;
    }
}				/* AddRequest */

/*----------------------------------------------------------------------
  RemoveNetRequest
  Remove a request from the queue of pending request for Net accesses.
  ----------------------------------------------------------------------*/
 
#ifdef __STDC__
void                RemoveNetRequest (AmayaNetRequestPtr cur)
#else
void                RemoveNetRequest (cur)
AmayaNetRequestPtr  cur;
#endif
{
    if (cur == NULL) return;
    CHECK_NET_REQUEST(cur)
    if ((cur->prev == NULL) || (cur->next == NULL)) {
        fprintf(stderr,"RemoveNetRequest : not linked request\n");
	return;
    }
    if (NetRequestList == NULL) {
        fprintf(stderr,"RemoveNetRequest : NetRequestList empty\n");
	return;
    }

    if ((NetRequestList == cur) && (cur->next == cur)) {
        NetRequestList = NULL;
    } else {
        if (NetRequestList == cur) NetRequestList = cur->next;
        cur->prev = NetRequestList->prev;
	cur->next = NetRequestList;
	cur->prev->next = cur;
	cur->next->prev = cur;
    }
    cur->next = cur->prev = NULL;
    CHECK_NET_REQUEST(NetRequestList)
}				/* RemoveNetRequest */

/*----------------------------------------------------------------------
  NextNetRequest
  Get the next request in the queue of pending requests for Net accesses.
  ----------------------------------------------------------------------*/
 
#ifdef __STDC__
AmayaNetRequestPtr  NextNetRequest (void)
#else
AmayaNetRequestPtr  NextNetRequest ()
#endif
{
    AmayaNetRequestPtr  cur = NetRequestList;

    CHECK_NET_REQUEST(cur)

    if (cur == NULL) return(NULL);
    RemoveNetRequest (cur);
    cur->state = AMAYA_NET_STATE_STARTED;
    return(cur);
}				/* NextNetRequest */

/*----------------------------------------------------------------------
  Stop Request
  remove all queued and active requests associated with a doc 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                StopRequest (int doc)
#else
void                StopRequest (doc)
int                 doc;
#endif
{
    AmayaNetRequestPtr  cur = NetRequestList;
    AmayaNetRequestPtr  prev;

    if (cur != NULL) {
       cur = cur->prev;
       do {
	   CHECK_NET_REQUEST(cur)

	   prev = cur->prev;
	   if (cur->blk.doc == doc) {
	       RemoveNetRequest(cur);
	       cur->callback(&cur->blk, AMAYA_NET_STATUS_FAILED);
	       FreeNetRequest(cur);
	   }
	   cur = prev;
       } while ((NetRequestList != NULL) && (cur != NetRequestList));
    }

    /*
     * Stop all Active requests.
     */
    StopActiveNetRequests(doc);
}				/* StopRequest */


/*----------------------------------------------------------------------
   GetObjectWWW
   this function requests a resource designated by a URLname into a
   temporary filename. The download can come from a simple GET operation,
   or can come from POSTING/GETTING a form. In the latter
   case, the function receives a query string to send to the server.

   2  file retrieval modes are proposed:                              
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

   The function will immediately return after setting up the
   call.

   Notes:
   At the end of a succesful request, the urlName string contains the
   name of the actually retrieved URL. As an URL can change over the time,
   (e.g., be redirected elsewhere), it is advised that the function
   caller verify the value of the urlName variable at the end of
   a request.

   Inputs:
   - doc  Document identifier for the set of objects being
   retrieved.
   - urlName The URL to be retrieved (MAX_LENGTH chars length)
   - outputfile A pointer to an empty string of MAX_LENGTH.
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
AmayaNetStatus   GetObjectWWW (int doc, char *urlName, char *postString,
  char *outputfile, int mode, TIcbf * incremental_cbf, void *context_icbf,
  TTcbf * terminate_cbf, void *context_tcbf, boolean  error_html)
#else
AmayaNetStatus   GetObjectWWW (doc, urlName, postString, outputfile, mode,
		 incremental_cbf, context_icbf, terminate_cbf, context_tcbf,
				  error_html)
int                 doc;
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
   AmayaNetRequestPtr  request;

   FILE               *tmp_fp;
   char               *tmp_dir;
   char               *ref;
   int                 status;
   HTList             *cur, *pending;

   /*
    * check parameters.
    */
   if (urlName == NULL || doc <= 0 || outputfile == NULL)
     {
	TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_BAD_URL), urlName);

	if (error_html)
	   FilesLoading[doc] = 2;	/* so we can show the error message */
	return(AMAYA_NET_STATUS_FAILED);

     }

   /*
    * do we support this protocol?
    */
   if (IsValidProtocol (urlName) == NO)
     {
	outputfile[0] = EOS;	/* file could not be opened */
	TtaSetStatus (doc, 1,
	              TtaGetMessage (AMAYA, AM_GET_UNSUPPORTED_PROTOCOL),
		      urlName);

	if (error_html)
	   FilesLoading[doc] = 2;	/* so we can show the error message */
	return(AMAYA_NET_STATUS_FAILED);
     }

   /*
    * verify if a directory exists for storing temporary files
    */
   tmp_dir = TtaGetMemory (strlen (TempFileDirectory) + 5 + 1);
   sprintf (tmp_dir, "%s/%d", TempFileDirectory, doc);

   /* !!!!!!!  fopen sur un repertoire !!!!!! */
   tmp_fp = fopen (tmp_dir, "r");
   if (tmp_fp == 0)
     {
	/*directory did not exist */
	if (mkdir (tmp_dir, S_IRWXU) == -1)
	  {
	     /*error */
	     outputfile[0] = EOS;
	     TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_CACHE_ERROR),
			   urlName);

	     if (error_html)
		FilesLoading[doc] = 2;	/* so we can show the error message */

	     return(AMAYA_NET_STATUS_FAILED);
	  }
     }
   else
      fclose (tmp_fp);

   /*
    * create a temporary filename
    */
   sprintf (outputfile, "%s/%04dAM", tmp_dir, object_counter++);
   TtaFreeMemory (tmp_dir);

   /*
    * normalize the URL
    */
   ref = HTParse (urlName, "", PARSE_ALL);

   /*
    * should we abort the request if we could not normalize the url?
    */
   if (ref == (char *) NULL || ref[0] == EOS)
     {
	/*error */
	outputfile[0] = EOS;
	TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_BAD_URL),
		      urlName);

	if (error_html)
	   FilesLoading[doc] = 2;	/* so we can show the error message */

	return(AMAYA_NET_STATUS_FAILED);
     }

   /*
    * remove any old file.
    */
   if (TtaFileExist (outputfile))
     {
	TtaFileUnlink (outputfile);
     }

   /*
    * try to open the outputfile
    */
   if ((tmp_fp = fopen (outputfile, "w")) == NULL)
     {
	outputfile[0] = EOS;	/* file could not be opened */
	TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_CANNOT_CREATE_FILE),
		      outputfile);
	HT_FREE (ref);

	if (error_html)
	   FilesLoading[doc] = 2;	/* so we can show the error message */
	return (AMAYA_NET_STATUS_FAILED);
     }

   /*
    * Initialize the request structure
    */
   request =  NewNetRequest();
   if (request == NULL) {
      HT_FREE (ref);
      return (AMAYA_NET_STATUS_FAILED);
   }
   request->blk.outputfile = TtaStrdup(outputfile);
   strcpy(request->blk.url, urlName);

   /* Specific initializations for POST and GET */
   if (mode & AMAYA_FORM_POST)
     {
	request->type = AMAYA_NET_TYPE_FORM_POST;
	if (postString)
	  {
	    request->blk.data = TtaStrdup(postString);
	    request->blk.size = strlen (postString);
	  }
     }
   else
     {
	request->type = AMAYA_NET_TYPE_FORM_GET;
     }


   /*
    * queue the request and advise the Event Core that there
    * is a transfert to look at.
    */
   AddNetRequest (request);
   HandleQueuedNetRequests ();

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
   - doc  Document identifier for the set of objects being
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
int                 PutObjectWWW (int doc, char *fileName, char *urlName, int mode,
				  TTcbf * terminate_cbf, void *context_tcbf)
#else
int                 PutObjectWWW (doc, urlName, fileName, mode,
				  ,terminate_cbf, context_tcbf)
int                 doc;
char               *urlName;
char               *fileName;
int                 mode;
TTcbf              *terminate_cbf;
void               *context_tcbf;

#endif
{
   AmayaNetRequestPtr  request;
   int                 status;

   int                 fd;
   struct stat         file_stat;
   char               *mem_ptr;
   unsigned long       block_size;

   /*
    * check parameters.
    */
   if ((urlName == NULL) || (doc <= 0) || (outputfile == NULL) ||
       (!TtaFileExist (fileName)))
     {
	TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_BAD_URL), urlName);

	if (error_html)
	   FilesLoading[doc] = 2;	/* so we can show the error message */
	return(AMAYA_NET_STATUS_FAILED);
     }

   /*
    * do we support this protocol?
    */
   if (IsValidProtocol (urlName) == NO)
     {
	/* return error */
	TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_PUT_UNSUPPORTED_PROTOCOL),
		      urlName);
	return(AMAYA_NET_STATUS_FAILED);
     }

   /*
    * Initialize the request structure
    */
   request =  NewNetRequest();
   if (request == NULL) {
      return (AMAYA_NET_STATUS_FAILED);
   }
   request->blk.outputfile = TtaStrdup(outputfile);
   strcpy(request->blk.url, urlName);

   /*
    * read the file into memory
    */
   if ((fd = open (fileName, O_RDONLY)) == -1)
     {
	/* if we could not open the file, exit */
	/*error msg here */
	return (HT_ERROR);
     }

   fstat (fd, &file_stat);

   if (file_stat.st_size == 0) {
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

   status = UploadMemWWW (doc, METHOD_PUT, urlName, mem_ptr,
			  block_size, mode, terminate_cbf,
			  context_tcbf, (char *) NULL);
   /* Specific initializations for POST and GET */
   if (mode & AMAYA_FORM_POST)
     {
	request->type = AMAYA_NET_TYPE_FORM_POST;
	if (postString)
	  {
	    request->blk.data = TtaStrdup(postString);
	    request->blk.size = strlen (postString);
	  }
     }
   else
     {
	request->type = AMAYA_NET_TYPE_FORM_GET;
     }


   /*
    * queue the request and advise the Event Core that there
    * is a transfert to look at.
    */
   AddNetRequest (request);
   HandleQueuedNetRequests ();

   return (status);
}
}

