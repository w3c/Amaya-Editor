/*
 * JavaWWWInterf.c : rewrite of a Java based WWW access interface for Amaya.
 */

#include "thot_sys.h"
#include "content.h"
#include "JavaTypes.h"
#include "amaya_HTTPRequest.h"

#define AMAYA_SYNC      1
#define AMAYA_ASYNC     4
#define AMAYA_FORM_POST 16
#define AMAYA_FORM_GET  32
#define AMAYA_NOCACHE	64
#define AMAYA_NOREDIR	128

extern char TempFileDirectory[];

/************************************************************************
 *									*
 *   HTTP Requests Allocator. Since freeing Java allocated class	*
 *       instances is not a good thing, keep them in a pool		*
 *									*
 ************************************************************************/

/*
 * CreateHTTPRequest : Create an CreateHTTPRequest instance.
 */

#ifdef __STDC__
struct Hamaya_HTTPRequest* CreateHTTPRequest ()
#else
struct Hamaya_HTTPRequest* CreateHTTPRequest ()
#endif
{
    Hjava_lang_Object *obj;
    struct Hamaya_HTTPRequest* request;

    obj = AllocObject("amaya/HTTPRequest");
    request = (struct Hamaya_HTTPRequest*) obj;

    return(request);
}

/*
 * AllocHTTPRequest : Get an CreateHTTPRequest instance and preinitialize it.
 */

#ifdef __STDC__
struct Hamaya_HTTPRequest*
AllocHTTPRequest (int doc, char *url, char *postString, char *outputfile)
#else
struct Hamaya_HTTPRequest*
AllocHTTPRequest (doc, url, postString, outputfile)
int                 doc;
char               *urlName;
char               *postString;
char               *outputfile;
#endif
{
    static int req_nr = 0;
    struct Hamaya_HTTPRequest* request;
    struct Hjava_lang_String* urlName = NULL;
    struct Hjava_lang_String* postCmd = NULL;
    struct Hjava_lang_String* filename = NULL;

    if (outputfile[0] == '\0') {
        sprintf(outputfile,"%s/amaya_req%d",&TempFileDirectory[0],req_nr++);
    }

    if (url != NULL) {
        urlName = makeJavaString(url, strlen(url));
    }
    if (postString != NULL) {
        postCmd = makeJavaString(postString, strlen(postString));
    }
    if (outputfile != NULL) {
        filename = makeJavaString(outputfile, strlen(outputfile));
    }

    request = CreateHTTPRequest();

    if (doc == 0) doc = 1;
    unhand(request)->doc = doc;
    unhand(request)->urlName = urlName;
    unhand(request)->filename = filename;
    unhand(request)->postCmd = postCmd;
    unhand(request)->callback = 0;
    unhand(request)->callback_f = 0;
    unhand(request)->callback_arg = 0;

    return(request);
}

/*
 * FreeHTTPRequest : Take it back to the pool.
 */

#ifdef __STDC__
void FreeHTTPRequest (struct Hamaya_HTTPRequest* request)
#else
void FreeHTTPRequest (struct Hamaya_HTTPRequest* request)
#endif
{
}

/*----------------------------------------------------------------------
   GetObjectWWWCallback
   this function is called when an asynchronous Get is finished.

   It's just a gate to the actual C callback. It has for only argument
   the handle to the HTTPRequest instance handle.
  ----------------------------------------------------------------------*/

typedef void (*GetObjectWWWCCallback) (int doc, int status,
              char *urlName, char *outputfile, void *context);

#ifdef __STDC__
int GetObjectWWWCallback (void *arg)
#else
int GetObjectWWWCallback (arg)
void *arg;
#endif
{
    GetObjectWWWCCallback callback = NULL;
    int doc;
    int status;
    void *context = NULL;
    char urlName[1024];
    char outputfile[1024];

    struct Hamaya_HTTPRequest* request = (struct Hamaya_HTTPRequest*) arg;

    callback = (GetObjectWWWCCallback) ((void *)
                                        unhand(request)->callback_f);
    doc = unhand(request)->doc;
    status = unhand(request)->status;
    context = (void *) unhand(request)->callback_arg;
    javaString2CString(unhand(request)->urlName, urlName, sizeof(urlName));
    javaString2CString(unhand(request)->filename, outputfile, sizeof(outputfile));

/* fprintf(stderr,"GetObjectWWWCallback : %s : %d\n", urlName, status); */

    if (callback != NULL)
        callback(doc, status, &urlName[0], &outputfile[0], context);
    FreeHTTPRequest(request);
    return(0);
}

/*----------------------------------------------------------------------
   GetObjectWWW
   this function requests a resource designated by a URLname into a
   temporary filename. The download can come from a simple GET operation,
   or can come from POSTING/GETTING a form. In the latter
   case, the function receives a query string to send to the server.

   Various file retrieval modes are proposed:
   AMAYA_SYNC : blocking mode
   AMAYA_ISYNC : incremental, blocking mode
   AMAYA_ASYNC : non-blocking mode
   AMAYA_IASYNC : incremental, non-blocking mode

   In addition
   AMAYA_NOCACHE : no cache
   AMAYA_NOREDIR : no redirection

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
   - doc  Document
   - url The URL to be retrieved (MAX_URL_LENGTH chars length)
   - outputfile A pointer to an empty string of MAX_URL_LENGTH.
   - mode The retrieval mode.
   - incremental (unused)
   - context (unused)
   Callback and context for the incremental modes
   - terminate (unused)
   - tcontext (unused)
   Callback and context for a terminate handler
   -error_html if TRUE, then display any server error message as an
   HTML document.

   Outputs:
   - url The URL that was retrieved
   - outputfile The name of the temporary file which holds the
   retrieved data. (Only in case of success)
   Returns:
   The HTTP error code or zero in case of success.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
int GetObjectWWW (int doc, char *url, char *postString,
                  char *outputfile, int mode, void *incremental,
                  void *context, void *terminate,
                  void *tcontext, boolean error_html)
#else
int GetObjectWWW (doc, url, postString, outputfile, mode, incremental,
                  context, terminate, tcontext, error_html)
int                 doc;
char               *urlName;
char               *postString;
char               *outputfile;
int                 mode;
void               *incremental; /* unused */
void               *context;     /* unused */
void               *terminate;   /* unused */
void               *tcontext;    /* unused */
boolean             error_html;
#endif
{
    struct Hamaya_HTTPRequest* request;
    int result;
    int flag = 0;

    if (mode & AMAYA_NOCACHE) {
        mode -= AMAYA_NOCACHE;
	flag += AMAYA_NOCACHE;
    }
    if (mode & AMAYA_NOREDIR) {
        mode -= AMAYA_NOREDIR;
	flag += AMAYA_NOREDIR;
    }

    /*
     * Allocate and fill in a new HTTP Request instance.
     */
    outputfile[0] = '\0';
    request = AllocHTTPRequest (doc, url, postString, outputfile);

    /*
     * Call the Java WWW access implementation.
     * Release the Thot library lock in the interval.
     */
    JavaThotlibRelease();
    switch (mode) {
        case AMAYA_SYNC:
	    unhand(request)->callback = (jlong) 0;
	    do_execute_java_method(0, (void *) request, "Get", "(I)I",
	                           0, 0, flag);
	    break;
        case AMAYA_ASYNC:
	    unhand(request)->callback = (jlong) GetObjectWWWCallback;
	    unhand(request)->callback_f = (jlong) terminate;
	    unhand(request)->callback_arg = (jlong) tcontext;
	    do_execute_java_method(0, (void *) request, "AsyncGet", "(I)I",
	                           0, 0, flag);
	    break;
	case AMAYA_FORM_POST | AMAYA_SYNC:
	    do_execute_java_method(0, (void *) request, "Post", "(I)I",
	                           0, 0, flag);
	    break;
	default:
	    fprintf(stderr,"GetObjectWWW : unsupported mode %d\n", mode);
	    exit(1);
    }
    JavaThotlibLock();

    /*
     * Check the result.
     */
    result = unhand(request)->status;
    switch (result) {
        case 0:
	    /* not finished */
	    break;
        case 200:
        case 201:
        case 202:
        case 203:
        case 204:
	    /* Success */
            javaString2CString(unhand(request)->urlName, url, MAX_PATH);
            javaString2CString(unhand(request)->filename, outputfile, MAX_PATH);
	    break;
	case 404:
	    /* Not found error */
            javaString2CString(unhand(request)->urlName, url, MAX_PATH);
            javaString2CString(unhand(request)->filename, outputfile, MAX_PATH);
	    break;
	default:
	    /* Some kind of error or strange behaviour occured */
	    FreeHTTPRequest(request);
	    fprintf(stderr,"GetObjectWWW : Protocol error %d\n", result);
	    return(-1);
    }
    switch (mode) {
        case AMAYA_SYNC:
	    FreeHTTPRequest(request);
	    break;
    }

    return(0);
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
   - terminate_cbf (unused)
   - context_icbf (unused)
   Callback and context for a terminate handler

   Outputs:
   - urlName The URL that was uploaded

   Returns:
   HT_ERROR
   HT_OK
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int PutObjectWWW (int doc, char *fileName, char *url, int mode,
                  PicType contentType, void * terminate_cbf,
                  void *context_tcbf)
#else
int PutObjectWWW (doc, fileName, urlName, mode, contentType, terminate, context)
int                 doc;
char               *fileName;
char               *url;
int                 mode;
PicType             contentType;
void               *terminate_cbf;
void               *context_tcbf;

#endif
{
    struct Hamaya_HTTPRequest* request;
    int result;
    int flag = 0;
    char *mimeType;

    if (mode & AMAYA_NOCACHE) {
        mode -= AMAYA_NOCACHE;
	flag += AMAYA_NOCACHE;
    }
    if (mode & AMAYA_NOREDIR) {
        mode -= AMAYA_NOREDIR;
	flag += AMAYA_NOREDIR;
    }

    /*
     * Allocate and fill in a new HTTP Request instance.
     * Set the ContentType ...
     */
    request = AllocHTTPRequest (doc, url, NULL, fileName);
    switch (contentType) {
        case xbm_type:
	    mimeType = "image/x-xbitmap"; break;
	case eps_type:
	    mimeType = "application/postscript"; break;
	case xpm_type:
	    mimeType = "image/x-xpixmap"; break;
	case gif_type:
	    mimeType = "image/gif"; break;
	case jpeg_type:
	    mimeType = "image/jpeg"; break;
	case png_type:
	    mimeType = "image/png"; break;
	case unknown_type:
	    mimeType = "text/html"; break;
	default:
	    mimeType = "text/html"; break;
    }
    unhand(request)->mimeType = makeJavaString(mimeType, strlen(mimeType));

    /*
     * Call the Java WWW access implementation.
     * Release the Thot library lock in the interval.
     */
    JavaThotlibRelease();
    switch (mode) {
        case AMAYA_SYNC:
	    do_execute_java_method(0, (void *) request, "Put", "(I)I",
	                           0, 0, flag);
	    break;
	default:
	    fprintf(stderr,"PutObjectWWW : unsupported mode %d\n", mode);
	    exit(1);
    }
    JavaThotlibLock();

    /*
     * Check the result.
     */
    result = unhand(request)->status;
    switch (result) {
        case 200:
        case 201:
        case 202:
        case 203:
        case 204:
	    /* Success */
            javaString2CString(unhand(request)->urlName, url, MAX_PATH);
	    break;
	default:
	    /* Some kind of error or strange behaviour occured */
	    FreeHTTPRequest(request);
	    fprintf(stderr,"PutObjectWWW : Protocol error %d\n", result);
	    return(-1);
    }
    FreeHTTPRequest(request);

    return(0);
}

/*----------------------------------------------------------------------
  Stop Request
  stops (kills) all active requests associated with a document
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                StopRequest (int doc)
#else
void                StopRequest (doc)
int                 doc;
#endif
{
    JavaThotlibRelease();
    do_execute_java_class_method("amaya/HTTPRequest", "Stop", "(I)V",
                                 (jint) doc);
    JavaThotlibLock();
}

