/*
 * JavaX11Interf.c : this module handle the X-Windows events.
 *                   when kaffe is embedded in Amaya.
 */

/*
 * the X-Window lock is implemented in kaffeine.c
 * the general event registration mecanism is in events.c
 */

#include "thot_sys.h"
#include "interface.h"
#include "registry.h"
#include "JavaX11Interf.h"
#include "events.h"
#include "kaffeine.h"

#include "kaffeine_f.h"
#include "jsyscall.h"         /* for Kaffe_SystemCallInterface */

static int JavaEventLoopInitialized =0;
int x_window_socket;

/* Appeared with Kaffe-0.9.2 */
extern char *realClassPath;

/*
 * If DoJavaSelectPoll is selected, any JavaSelectCall on the
 * x-window socket will fail with value -1 if BreakJavaSelectPoll
 * is non zero. This is needed to do multi filedescriptor polling
 * without breaking the Kaffe threading model.
 */

int DoJavaSelectPoll = 0;
int BreakJavaSelectPoll = 0;


/*
 * The X Application context.
 */

ThotAppContext CurrentAppContext = NULL;

/*
 * JavaHandleOneEvent
 *
 * This routine handle one event fetched from the X-Window socket.
 */
#ifdef __STDC__
void                JavaHandleOneEvent (ThotEvent *ev)
#else
void                JavaHandleOneEvent (ev)
ThotEvent *ev;
#endif
{
    TtaHandleOneEvent(ev);
}

/*
 * JavaFetchEvent
 *
 * This routine poll both the socket used by the Network interface and
 * the X-Windows event queue. As long as no X-Window event is available,
 * it has to handle network traffic. If an X-Window event is available,
 * the routine should fetch it from the queue in the ev argument and return.
 */

#ifdef __STDC__
int                 JavaFetchEvent (ThotEvent *ev)
#else
int                 JavaFetchEvent (ev)
ThotEvent *ev;
#endif
{
  int status;

  JavaThotlibRelease();
  JavaXWindowSocketLock();

#ifdef _WINDOWS
#else  /* !_WINDOWS */
  /*
   * Need to check whether something else has to be scheduled.
   */
  status = XtAppPending (CurrentAppContext);
  if (!status) {
     XFlush(TtaGetCurrentDisplay());

     do {
	 status = blockOnFile(x_window_socket, 0);
     } while (status < 0);
  }
  XtAppNextEvent (CurrentAppContext, ev);
#endif /* _WINDOWS */

  JavaXWindowSocketRelease();
  JavaThotlibLock();
  return(0);
}

#ifdef __STDC__
int                 OldJavaFetchEvent (ThotEvent *ev)
#else
int                 OldJavaFetchEvent (ev)
ThotEvent *ev;
#endif
{
  int status;

  JavaThotlibRelease();
  JavaXWindowSocketLock();

#ifdef _WINDOWS
#else  /* !_WINDOWS */
  /*
   * Need to check whether something else has to be scheduled.
   */
  status = XtAppPending (CurrentAppContext);
  if (!status) {
     XFlush(TtaGetCurrentDisplay());
     status = blockOnFile(x_window_socket, 0);

     if ((DoJavaSelectPoll) && (BreakJavaSelectPoll)) {
	 JavaXWindowSocketRelease();
	 JavaThotlibLock();
         return(-1);
     }
  }
  XtAppNextEvent (CurrentAppContext, ev);
#endif /* _WINDOWS */

  JavaXWindowSocketRelease();
  JavaThotlibLock();
  return(0);
}

/*
 * JavaFetchAvailableEvent
 *
 * This routine look at the socket used by the Network interface and
 * the X-Windows event queue. It first handle changes in the network
 * socket status, and handle them (atomic operations). Once done,
 * if an X-Window event is available, the routine should fetch it from
 * the queue in the ev argument and return TRUE, it returns FALSE otherwise.
 */
#ifdef __STDC__
boolean             JavaFetchAvailableEvent (ThotEvent *ev)
#else
boolean             JavaFetchAvailableEvent (ev)
ThotEvent *ev;
#endif
{
  int status;

#ifdef _WINDOWS
#else  /* !_WINDOWS */
  status = XtAppPending (CurrentAppContext);
  if (status) {
     XtAppNextEvent (CurrentAppContext, ev);
     return(TRUE);
  }
  return(FALSE);
#endif /* _WINDOWS */
}

/*
 * JavaHandleAvailableEvents
 *
 * This routine check wether some XtEvent are to be fetched.
 * If yes, get it and handle it, and return once all have been consumed.
 */
#ifdef __STDC__
void             JavaHandleAvailableEvents (void)
#else
void             JavaHandleAvailableEvents ()
#endif
{
  ThotEvent ev;
  int status;

  if (ThotlibLockValue != 0) {
#ifdef DEBUG_SELECT
      fprintf(stderr,
         "JavaHandleAvailableEvents called while ThotlibLockValue != 0\n");
#endif
      return;
  }
  if (XWindowSocketLockValue != 0) {
#ifdef DEBUG_SELECT
      fprintf(stderr,
         "JavaHandleAvailableEvents called while XWindowSocketLockValue != 0\n");
#endif
      return;
  }

#ifdef _WINDOWS
#else  /* !_WINDOWS */
  do {
      status = XtAppPending (CurrentAppContext);
      if (status) {
	 XtAppNextEvent (CurrentAppContext, &ev);
	 JavaThotlibLock();
	 JavaHandleOneEvent (&ev);
	 JavaThotlibRelease();
      }
  } while (status);
#endif /* _WINDOWS */
}

/*
 * InitJavaEventLoop
 *
 * Initialize the JavaEventLoop environment, including the network
 * interface, Java, etc...
 */
#ifdef __STDC__
void                InitJavaEventLoop (ThotAppContext app_ctx)
#else
void                InitJavaEventLoop (app_ctx)
ThotAppContext app_ctx;
#endif
{
    char *env_value;
    char  new_env[1024];

    CurrentAppContext = app_ctx;
    if (JavaEventLoopInitialized) return;

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

    /* setup the CLASSPATH value for Kaffe. */
    new_env[0] = '\0';
    env_value = getenv("CLASSPATH");
    if (env_value)
       strcat(new_env, env_value);
    realClassPath = TtaStrdup(new_env);

    /*
     * Register the X-Window socket as an input channel
     */
    x_window_socket = ConnectionNumber(TtaGetCurrentDisplay());
    threadedFileDescriptor(x_window_socket);

    /*
     * set up our own select call.
     */
    Kaffe_SystemCallInterface._select = JavaSelect;

    /*
     * Startup the Java environment. We should never return
     * from this call, but InitJava will call TtaMainLoop again
     * on the Application thread.
     */
    InitJava();

}

/*
 * JavaStopPoll
 *
 * Stop the poll loop (below).
 */
#ifdef __STDC__
int                 JavaStopPoll ()
#else
int                 JavaStopPoll ()
#endif
{
   if (DoJavaSelectPoll)
       BreakJavaSelectPoll++;
   return(0);
}

/*
 * JavaPollLoop
 *
 * This is the equivalent of the basic event loop except that it will
 * return after any interraction on the extra file descriptors.
 */
#ifdef __STDC__
int                 JavaPollLoop ()
#else
int                 JavaPollLoop ()
#endif
{
   int status;
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
      return(-1);

   /*
    * We want to jump off the loop if transfers did occurs
    * on the extra descriptors.
    */
   DoJavaSelectPoll = 1;
   BreakJavaSelectPoll = 0;

#ifdef DEBUG_SELECT
   TIMER
   fprintf(stderr,"JavaPollLoop entered\n");
#endif

   /*
    * Release the ThotLib lock...
    */
   JavaThotlibRelease();

   /* Loop waiting for the events */
   while (1)
     {
#ifdef _WINDOWS
#else /* ! _WINDOWS */
        /*
	 * Block looking for events available on the X-Window socket.
	 * The select will return an exception condition if someting
	 * occured on the other file descriptors.
	 */
        status = blockOnFile(x_window_socket, 0);

        if (status < 0) {
	    JavaThotlibLock();
	    DoJavaSelectPoll = 0;
	    BreakJavaSelectPoll = 0;
#ifdef DEBUG_SELECT
	    TIMER
	    fprintf(stderr,"JavaPollLoop stopped\n");
#endif
	    return(-1);
	}

	/*
	 * If there is another thread waiting for the X-Windows socket
	 * reads, sleep to let it fetch the event ...
	 */
        if ((XWindowSocketWaitValue > 0) || (XWindowSocketLockValue > 0)) {
            sleepThread(5);
	    if (BreakJavaSelectPoll) {
		JavaThotlibLock();
		DoJavaSelectPoll = 0;
		BreakJavaSelectPoll = 0;
#ifdef DEBUG_SELECT
		TIMER
		fprintf(stderr,"JavaPollLoop stopped\n");
#endif
		return(-1);
	    }
	    continue;
	}

	/*
	 * Noone is waiting for this event, take back the ThotLib lock
	 * and handle this event.
	 */
	JavaThotlibLock();
	if (OldJavaFetchEvent(&ev) < 0) {
	    DoJavaSelectPoll = 0;
	    BreakJavaSelectPoll = 0;
#ifdef DEBUG_SELECT
	    TIMER
	    fprintf(stderr,"JavaPollLoop stopped\n");
#endif
	    return(-1);
	}
        JavaHandleOneEvent (&ev);
	JavaThotlibRelease();
#endif /* _WINDOWS */
     }
   /*ENOTREACHED*/
   JavaThotlibLock();
   return(0);
}

/*
 *  JavaEventLoop
 *
 *  The point where events arriving from the X-Windows socket as well as
 *  the network sockets are fetched and dispatched to the correct handlers.
 *  The loop may be called recursively (introduced for ILU), and the current
 *  level must break as soon as *stop is set to 0.
 *
 *  The stack frame mechanism (and pieces of code) are borowed from Xerox ILU.
 *
 */

typedef struct stack_s {
    int *stop;
    struct stack_s *next;
} StackFrame;

static StackFrame *run_stack = NULL;

#ifdef __STDC__
void                JavaEventLoop (int *stop)
#else
void                JavaEventLoop (stop)
int *stop;
#endif
{
   int status;
   StackFrame ours;
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
      return;

   /*
    * push our "frame" onto the "stack"
    */
   ours.next = run_stack;
   ours.stop = stop;
   run_stack = &ours;
   *stop = 1;

   /*
    * We don't want to jump off the loop if transfers did occurs
    * on the extra descriptors.
    */
   DoJavaSelectPoll = 0;
   BreakJavaSelectPoll = 0;
   JavaThotlibRelease();

   /* Loop waiting for the events */
   while (*stop != 0) {
        while (XWindowSocketWaitValue > 0) {
	    /*
	     * Don't block appplication thread reading events.
	     */
            sleepThread(30);
	    continue;
	}
        status = blockOnFile(x_window_socket, 0);
	do {
	    status = blockOnFile(x_window_socket, 0);
	} while (status < 0);
        status = XtAppPending (CurrentAppContext);
        if (!status) {
	    XFlush(TtaGetCurrentDisplay());
	    continue;
        }
	JavaXWindowSocketLock();
        XtAppNextEvent (CurrentAppContext, &ev);
	JavaXWindowSocketRelease();
	JavaThotlibLock();
        JavaHandleOneEvent (&ev);
        JavaThotlibRelease();
    }

    /*
     * pop our "frame" from the stack
     */
    run_stack = ours.next;


}

/*
 * JavaExitEventLoop
 *
 * stop one level of cascaded event loops.
 */

void JavaExitEventLoop(int *stop)
{
    StackFrame *f;

    for (f = run_stack; f != NULL; f = f->next) {
        if (f->stop == stop) *stop = 0;
	return;
    }
    fprintf(stderr, "JavaExitEventLoop : stop pointer 0x%X not found\n",
            (unsigned) stop);
}

/*
 * JavaStartEventLoop
 *
 * the lowest Event loop (May be cascading them).
 */

int JavaEventLoopStop;

#ifdef __STDC__
void                JavaStartEventLoop ()
#else
void                JavaStartEventLoop ()
#endif
{
    while (1) {
	JavaEventLoop(&JavaEventLoopStop);
    }
}

/*
 * JavaLoadResources 
 *
 * link in the Java stuff and initialize it.
 */
void                JavaLoadResources ()
{
   TtaSetMainLoop (InitJavaEventLoop, JavaStartEventLoop,
		   JavaFetchEvent, JavaFetchAvailableEvent);
}

