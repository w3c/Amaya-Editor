/*
 * Interface for the Kaffe Java interpreter.
 *    NOTE : This must NOT include the W-Windows stuff since
 *           Object and ObjectClass identifiers clashes
 *
 *  Daniel Veillard
 */

#include <stdio.h>
#include <string.h>
#include "StubPreamble.h"
#include "jtypes.h"
#include "native.h"

#include "thotlib_APIApplication_stubs.h"
#include "thotlib_APIDocument_stubs.h"
#include "thotlib_APITree_stubs.h"
#include "thotlib_APIContent_stubs.h"
#include "thotlib_APIAttribute_stubs.h"
#include "thotlib_APIReference_stubs.h"
#include "thotlib_APILanguage_stubs.h"
#include "thotlib_APIPresentation_stubs.h"
#include "thotlib_APIView_stubs.h"
#include "thotlib_APISelection_stubs.h"
#include "thotlib_APIInterface_stubs.h"
#include "thotlib_APIRegistry_stubs.h"
#include "thotlib_APIDialog_stubs.h"

#define BISS_AWT "-Dawt.toolkit=biss.awt.kernel.Toolkit"

/*
 * Kaffe runtime accesses not published in native.h
 */
extern void initialiseKaffe(void);
extern int threadedFileDescriptor(int fd);
extern void yieldThread();
extern int blockOnFile(int fd, int op);

static void register_stubs(void);

static int JavaEventLoopInitialized =0;
static int x_window_socket;


/*----------------------------------------------------------------------
   InitJava

   Initialize the Java Interpreter.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InitJava (void)
#else
void                InitJava ()
#endif
{
    object* args;
    stringClass** str;
    char initClass[256];

    char *app_name = TtaGetEnvString ("appname");

    fprintf(stderr, "Initialize Java Runtime\n");

    /* Initialise */
    initialiseKaffe();

    /* Register Thotlib stubs */
    register_stubs();

    fprintf(stderr, "Java Runtime Initialized\n");

    /* Build the init class name */
    strcpy(initClass, app_name);
    strcat(initClass, "Init");

    /* Build an array of strings as the arguments */
    args = AllocObjectArray(1, "Ljava/lang/String;");

    /* Build each string and put into the array */
    str = (stringClass**)(args + 1);
    str[0] = makeJavaString(app_name, strlen(app_name));

    /* lauch the init class for the application */
    do_execute_java_class_method(initClass, "main",
                   "([Ljava/lang/String;)V", args);

    /* Start the application loop of events */
    do_execute_java_class_method("thotlib.Interface", "main",
                   "([Ljava/lang/String;)V", args);
}

/*----------------------------------------------------------------------
  InitJavaEventLoop

  Initialize the JavaEventLoop environment, including the network
  interface, Java, etc...
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InitJavaEventLoop (void)
#else
void                InitJavaEventLoop ()
#endif
{
    char *env_value;
    char  new_env[1024];

    /*
     * Everything is initialized BEFORE starting the
     * Java Runtime ...
     */
    JavaEventLoopInitialized = 1;

    /*
     * set up the environment
     */
    strcpy(new_env,"CLASSPATH=");
    env_value  = TtaGetEnvString("CLASSPATH");
    if (env_value)
       strcat(new_env, env_value);
    env_value = getenv("CLASSPATH");
    if (env_value) {
       strcat(new_env,":");
       strcat(new_env,env_value);
    }
    putenv(TtaStrdup(new_env));
    strcpy(new_env,"KAFFEHOME=");
    env_value  = TtaGetEnvString("KAFFEHOME");
    if (env_value)
       strcat(new_env, env_value);
    putenv(TtaStrdup(new_env));

    /*
     * Register the X-Window socket as an input channel
     */
    x_window_socket = ConnectionNumber(TtaGetCurrentDisplay());
    threadedFileDescriptor(x_window_socket);

    /*
     * Startup the Java environment. We should never return
     * from this call, but InitJava will call TtaMainLoop again
     * on the Application thread.
     */
    InitJava();

}

/*----------------------------------------------------------------------
  JavaSelect

  This routine provide a shared select() syscall needed to multiplex
  the various packages (libWWW, Java, X-Windows ...) needing I/O in
  the Java program.
  ----------------------------------------------------------------------*/

static int NbJavaSelect = 0;

#ifdef __STDC__
int                JavaSelect (int  n,  fd_set  *readfds,  fd_set  *writefds,
       fd_set *exceptfds, struct timeval *timeout)
#else
int                JavaSelect (n, readfds, writefds, exceptfds, timeout)
int  n;
fd_set  *readfds;
fd_set  *writefds;
fd_set *exceptfds;
struct timeval *timeout;
ThotEvent *ev;
#endif
{
    int res;

    NbJavaSelect++;

    /* Just a test for now ... */
    if (n <= x_window_socket) n = x_window_socket + 1;
    res = select(n, readfds, writefds, exceptfds, timeout);

    return(res);
}

/*----------------------------------------------------------------------
  JavaHandleOneEvent

  This routine handle one event fetched from the X-Window socket.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                JavaHandleOneEvent (ThotEvent *ev)
#else
void                JavaHandleOneEvent (ev)
ThotEvent *ev;
#endif
{
    TtaHandleOneEvent(ev);
}

/*----------------------------------------------------------------------
  JavaFetchEvent

  This routine poll both the socket used by the Network interface and
  the X-Windows event queue. As long as no X-Window event is available,
  it has to handle network traffic. If an X-Window event is available,
  the routine should fetch it from the queue in the ev argument and return.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                JavaFetchEvent (ThotAppContext app_ctxt, ThotEvent *ev)
#else
void                JavaFetchEvent (app_ctxt, ev)
ThotAppContext app_ctxt;
ThotEvent *ev;
#endif
{
  int status;

#ifdef WWW_XWINDOWS
  status = XtAppPending (app_ctxt);
  while (status & XtIMXEvent) {
     XtAppProcessEvent (app_ctxt, XtIMXEvent);
     status = XtAppPending (app_ctxt);
  }

  /*
   * Need to check whether something else has to be scheduled.
   */
  blockOnFile(x_window_socket, 0);
  XtAppNextEvent (app_ctxt, ev);

#else  /* WWW_XWINDOWS */
#endif /* !WWW_XWINDOWS */
}

/*----------------------------------------------------------------------
  JavaFetchAvailableEvent

  This routine look at the socket used by the Network interface and
  the X-Windows event queue. It first handle changes in the network
  socket status, and handle them (atomic operations). Once done,
  if an X-Window event is available, the routine should fetch it from
  the queue in the ev argument and return TRUE, it returns FALSE otherwise.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             JavaFetchAvailableEvent (ThotAppContext app_ctxt, ThotEvent *ev)
#else
boolean             JavaFetchAvailableEvent (app_ctxt, ev)
ThotAppContext app_ctxt;
ThotEvent *ev;
#endif
{
  int status;

#ifdef WWW_XWINDOWS
  status = XtAppPending (app_ctxt);
  while (status & XtIMXEvent) {
     XtAppProcessEvent (app_ctxt, XtIMXEvent);
     status = XtAppPending (app_ctxt);
  }
  if (status) {
     XtAppNextEvent (app_ctxt, ev);
     return(TRUE);
  }
  return(FALSE);
#else  /* WWW_XWINDOWS */
#endif /* !WWW_XWINDOWS */
}

/*----------------------------------------------------------------------
  JavaEventLoop

  The point where events arriving from the X-Windows socket as well as
  the network sockets are fetched and dispatched to the correct handlers.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                JavaEventLoop (ThotAppContext app_ctxt)
#else
void                JavaEventLoop (app_ctxt)
ThotAppContext app_ctxt;
#endif
{
#ifndef _WINDOWS
   ThotEvent           ev;
#endif /* _WINDOWS */
#ifdef _WINDOWS
   MSG                 msg;
#endif

   /*
    * initialize the whole context if needed.
    */
   if (!JavaEventLoopInitialized) 
      InitJavaEventLoop();

   /* Loop waiting for the events */
   while (1)
     {
#ifdef WWW_XWINDOWS
        JavaFetchEvent (app_ctxt, &ev);
        JavaHandleOneEvent (&ev);
#else  /* WWW_XWINDOWS */
	GetMessage (&msg, NULL, 0, 0);
	TranslateMessage (&msg);
	TtaHandleOneWindowEvent (&msg);
#endif /* !WWW_XWINDOWS */
     }
}

/*----------------------------------------------------------------------
   JavaLoadResources 

   link in the Java stuff and initialize it.
  ----------------------------------------------------------------------*/
void                JavaLoadResources ()
{
   TtaSetMainLoop (JavaEventLoop, JavaFetchEvent, JavaFetchAvailableEvent);
}

/*
 * This method is needed by the Kaffe interpreter.
 * What's happening when the memory is too low ?
 */

void
throwOutOfMemory ()
{
/*************************
        if (OutOfMemoryError != NULL)
                throwException(OutOfMemoryError);
 *************************/
        fprintf (stderr, "(Insufficient memory)\n");
        exit (-1);
}

/*
 * Register the thotlib stuff.
 */
static void register_stubs(void)
{
   register_thotlib_APIApplication_stubs();
   register_thotlib_APIDocument_stubs();
   register_thotlib_APITree_stubs();
   register_thotlib_APIContent_stubs();
   register_thotlib_APIAttribute_stubs();
   register_thotlib_APIReference_stubs();
   register_thotlib_APILanguage_stubs();
   register_thotlib_APIPresentation_stubs();
   register_thotlib_APIView_stubs();
   register_thotlib_APISelection_stubs();
   register_thotlib_APIInterface_stubs();
   register_thotlib_APIRegistry_stubs();
}

