/*
 * Interface for the Kaffe Java interpreter.
 *
 * Author: D. Veillard
 *
 */

#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include "StubPreamble.h"
#include "jtypes.h"
#include "native.h"
#include "registry.h"

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
#include "amaya_APIAmayaMsg_stubs.h"
#include "amaya_APIJavaAmaya_stubs.h"

/* DEBUG_KAFFE    will print lot of debug messages                      */
/* DEBUG_SELECT   will print debug messages on Select and Poll use      */
/* DEBUG_TIMING   add fine-tuned timing informations to debug messages  */
/* DEBUG_SELECT_CHANNELS   will do some sanity checking and print debug */
/*                         messages on extra channel uses               */
/* DEBUG_LOCK     will print debug messages on Locks and Releases       */

/*
 * Kaffe runtime accesses not published in native.h
 */
extern void initialiseKaffe(void);
extern int threadedFileDescriptor(int fd);
extern void yieldThread();
extern int blockOnFile(int fd, int op);
extern int (*select_call)(int, fd_set*, fd_set*, fd_set*, struct timeval*);
extern int blockInts;
extern void yieldThread();
extern void reschedule();
extern void sleepThread(jlong time);
static void register_stubs(void);
void register_biss_awt_API_stubs(void);

#ifdef DEBUG_TIMING
/************************************************************************
 *									*
 *			Timing Debug Support				*
 *									*
 ************************************************************************/

static int JavaTimerInitialized = 0;
static struct timeval tv_start;
static struct timeval tv_prev;

static void InitJavaTimer(void)
{
    gettimeofday(&tv_start, NULL);
    gettimeofday(&tv_prev, NULL);
    JavaTimerInitialized = 1;
}

static char *GetJavaTimer(void)
{
    static char JavaGetTimerString[500];
    struct timeval tv_cour;
    unsigned long time;
    unsigned long delta;

    if (!JavaTimerInitialized) InitJavaTimer();
    gettimeofday(&tv_cour, NULL);
    time = tv_cour.tv_sec - tv_start.tv_sec;
    time *= 1000000;
    time += tv_cour.tv_usec;
    time -= tv_start.tv_usec;
    delta = tv_cour.tv_sec - tv_prev.tv_sec;
    delta *= 1000000;
    delta += tv_cour.tv_usec;
    delta -= tv_prev.tv_usec;
    tv_prev.tv_usec = tv_cour.tv_usec;
    tv_prev.tv_sec = tv_cour.tv_sec;
    sprintf(&JavaGetTimerString[0],"%8lu : +%8lu : ", time, delta);
    return(&JavaGetTimerString[0]);
}

#define TIMER fprintf(stderr,GetJavaTimer());
#else /* !DEBUG_TIMING */
#define TIMER
#endif /* DEBUG_TIMING */

/************************************************************************
 *									*
 *	Taking Control of the Scheduling : Kaffe and External I/O	*
 *									*
 ************************************************************************/

#define CLEAR_FD(nb,to) {					\
    register int i, n = (nb) / sizeof(int);			\
    register int *t = (int *) (to);				\
    if (t != NULL) for (i = 0;i <= n; i++) *t++ = 0; }

#define COPY_FD(nb,from,to) {					\
    register int i, n = (nb) / sizeof(int);			\
    register int *f = (int *) (from);				\
    register int *t = (int *) (to);				\
    if ((t != NULL) && (f != NULL))				\
        for (i = 0;i <= n; i++) *t++ = *f++; }

#define OR_FD(nb,from,to) {					\
    register int i, n = (nb) / sizeof(int);			\
    register int *f = (int *) (from);				\
    register int *t = (int *) (to);				\
    if ((t != NULL) && (f != NULL))				\
        for (i = 0;i <= n; i++) *t++ |= *f++; }

#define AND_FD(nb,from,to) {					\
    register int i, n = (nb) / sizeof(int);			\
    register int *f = (int *) (from);				\
    register int *t = (int *) (to);				\
    if ((t != NULL) && (f != NULL))				\
        for (i = 0;i <= n; i++) *t++ &= *f++; }

#define EVENT_READ	1
#define EVENT_WRITE	2
#define EVENT_EXCEPT	4

typedef void (* SelectCallback) (int fd, int event);

static int max_extra_fd = 0;
static fd_set extra_readfds;
static fd_set extra_writefds;
static fd_set extra_exceptfds;

static SelectCallback JavaSelectCallback = NULL;

static int JavaEventLoopInitialized =0;
static int x_window_socket;

/*
 * If DoJavaSelectPoll is selected, any JavaSelectCall on the
 * x-window socket will fail with value -1 if BreakJavaSelectPoll
 * is non zero. This is needed to do multi filedescriptor polling
 * without breaking the Kaffe threading model.
 */

static int DoJavaSelectPoll = 0;
static int BreakJavaSelectPoll = 0;

/*----------------------------------------------------------------------
  InitJavaSelect

  Initialize all the static data for JavaSelect call.
  ----------------------------------------------------------------------*/

static int NbJavaSelect = 0;
static int JavaSelectInitialized = 0;

#ifdef __STDC__
void               InitJavaSelect(void)
#else
void               InitJavaSelect()
#endif
{
    NbJavaSelect = 0;
    max_extra_fd = 0;
    DoJavaSelectPoll = 0;
    BreakJavaSelectPoll = 0;
    FD_ZERO(&extra_readfds);
    FD_ZERO(&extra_writefds);
    FD_ZERO(&extra_exceptfds);
#ifdef DEBUG_SELECT
    TIMER
    fprintf(stderr,"InitJavaSelect\n");
#endif
    JavaSelectInitialized = 1;
}

/*----------------------------------------------------------------------
  JavaSetSelectCallback

  Set SelectCallback, ugly !
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void               JavaSetSelectCallback (void *val)
#else
void               JavaSetSelectCallback (val)
void *val;
#endif
{
    if (!JavaSelectInitialized) InitJavaSelect();
    JavaSelectCallback = (SelectCallback) val;
}

/*----------------------------------------------------------------------
  JavaFdSetState

  This routine register an I/O channel, either for Read, Write or
  Exceptions.  From that point all the Select call will catch the
  corresponding situations and call the adequate handler.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void               JavaFdSetState (int fd, int io)
#else
void               JavaFdSetState (fd, io)
int fd;
int io;
#endif
{
    if (!JavaSelectInitialized) InitJavaSelect();
    if (DoJavaSelectPoll) BreakJavaSelectPoll++;
    if ((fd < 0) || (fd > sizeof(fd_set) * 8)) return;
    if (fd >= max_extra_fd) max_extra_fd = fd;
#ifdef DEBUG_SELECT_CHANNELS
    TIMER
    if (io & 1) fprintf(stderr, "adding channel %d for read\n", fd);
    if (io & 2) fprintf(stderr, "adding channel %d for write\n", fd);
    if (io & 4) fprintf(stderr, "adding channel %d for exceptions\n", fd);
#endif
    if (io & 1) FD_SET(fd, &extra_readfds);
    if (io & 2) FD_SET(fd, &extra_writefds);
    if (io & 4) FD_SET(fd, &extra_exceptfds);
    threadedFileDescriptor(fd);
}

/*----------------------------------------------------------------------
  JavaFdResetState

  This routine unregister an I/O channel, either for Read, Write or
  Exceptions.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void               JavaFdResetState (int fd, int io)
#else
void               JavaFdResetState (fd, io)
int fd;
int io;
#endif
{
    int i;
    int max;
    if (!JavaSelectInitialized) InitJavaSelect();
    if (DoJavaSelectPoll) BreakJavaSelectPoll++;
    if ((fd < 0) || (fd > sizeof(fd_set) * 8)) return;
#ifdef DEBUG_SELECT_CHANNELS
    TIMER
    if (io & 1) fprintf(stderr, "removing channel %d for read\n", fd);
    if (io & 2) fprintf(stderr, "removing channel %d for write\n", fd);
    if (io & 4) fprintf(stderr, "removing channel %d for exceptions\n", fd);
#endif
    if (io & 1) FD_CLR(fd, &extra_readfds);
    if (io & 2) FD_CLR(fd, &extra_writefds);
    if (io & 4) FD_CLR(fd, &extra_exceptfds);
    for (i = 0,max = 0;i <= fd;i++) {
        if ((FD_ISSET(i, &extra_readfds)) || (FD_ISSET(i, &extra_writefds)) ||
	    (FD_ISSET(i, &extra_exceptfds)))
	    max = i;
    }
    max_extra_fd = max;
}

/*----------------------------------------------------------------------
  JavaSelect

  This routine provide a shared select() syscall needed to multiplex
  the various packages (libWWW, Java, X-Windows ...) needing I/O in
  the Java program.
  ----------------------------------------------------------------------*/

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
    int fd;
    fd_set full_readfds;
    fd_set full_writefds;
    fd_set full_exceptfds;
    fd_set lextra_readfds;
    fd_set lextra_writefds;
    fd_set lextra_exceptfds;
    struct timeval tm;
    int nb;
    int res;
    static int InJavaSelect = 0;

    if (!JavaSelectInitialized) InitJavaSelect();

    /*
     * Check for reentrancy, would be a Very Bad Thing (c)
     */
    if ((InJavaSelect) &&
        ((timeout == NULL) || (timeout->tv_usec != 0) ||
	 (timeout->tv_sec != 0))){
        /* char *p = NULL; */
	TIMER
        fprintf(stderr, "JavaSelect reentrancy !\n");
	/* call debugger or dump core
	*p = 0; ! */
    }
    InJavaSelect = 1;

    if ((DoJavaSelectPoll) && (BreakJavaSelectPoll) &&
        (n >= x_window_socket) && (readfds != NULL) &&
	(FD_ISSET(x_window_socket, readfds)))  {
	/*
	 * We must interrupt the poll on the X-Window socket.
	 */
#ifdef DEBUG_SELECT
	TIMER
	fprintf(stderr,"JavaSelect : Poll break !\n");
#endif
	FD_ZERO(readfds);
	FD_SET(x_window_socket, readfds);
	if (writefds) FD_ZERO(writefds);
	if (exceptfds) FD_ZERO(exceptfds);
	return(1);
    }

#ifdef DEBUG_SELECT
    TIMER
    fprintf(stderr,"<");
#endif

restart_select:
    /*
     * Do a local copy of everything needed later.
     */
    nb = (n > (max_extra_fd + 1)? n : (max_extra_fd + 1));
    COPY_FD(nb, &extra_readfds, &lextra_readfds);
    COPY_FD(nb, &extra_writefds, &lextra_writefds);
    COPY_FD(nb, &extra_exceptfds, &lextra_exceptfds);

    /*
     * Do not block if there is a Poll Break requested.
     */
    if ((DoJavaSelectPoll) && (BreakJavaSelectPoll)) {
       tm.tv_usec = 0;
       tm.tv_sec = 0;
    } else if (timeout != NULL)
       memcpy(&tm, timeout, sizeof(tm));

#ifdef DEBUG_SELECT_CHANNELS
    /*
     * Check that Kaffe and External descriptor sets don't overlap.
     */

    FD_ZERO(&full_readfds); /* CLEAR_FD(nb, &full_readfds); */
    OR_FD(nb, readfds, &full_readfds);
    OR_FD(nb, writefds, &full_readfds);
    OR_FD(nb, exceptfds, &full_readfds);

    FD_ZERO(&full_writefds); /* CLEAR_FD(nb, &full_writefds); */
    OR_FD(max_extra_fd, &extra_readfds, &full_writefds);
    OR_FD(max_extra_fd, &extra_writefds, &full_writefds);
    OR_FD(max_extra_fd, &extra_exceptfds, &full_writefds);
    
    AND_FD(nb, &full_writefds, &full_readfds);

    for (fd = 0;fd < nb; fd++) {
        if (FD_ISSET(fd, &full_readfds))
	    fprintf(stderr, 
	      "Kaffe and external conflict on channel %d\n", fd);
    }
#endif /* DEBUG_SELECT */

    /*
     * Create a full descriptor set merging both Kaffe ones and External ones.
     */
    FD_ZERO(&full_readfds); /* CLEAR_FD(nb, &full_readfds); */
    FD_ZERO(&full_writefds); /* CLEAR_FD(nb, &full_writefds); */
    FD_ZERO(&full_exceptfds); /* CLEAR_FD(nb, &full_exceptfds); */

    COPY_FD(nb, readfds, &full_readfds);
    COPY_FD(nb, writefds, &full_writefds);
    COPY_FD(nb, exceptfds, &full_exceptfds);

    OR_FD(nb, &extra_readfds, &full_readfds);
    OR_FD(nb, &extra_writefds, &full_writefds);
    OR_FD(nb, &extra_exceptfds, &full_exceptfds);

    /*
     * Do the select on the merged channels descriptors.
     */
    NbJavaSelect++;
    if (((DoJavaSelectPoll) && (BreakJavaSelectPoll)) ||
        (timeout != NULL))
       res = select(nb, &full_readfds, &full_writefds, &full_exceptfds, &tm);
    else
       res = select(nb, &full_readfds, &full_writefds, &full_exceptfds, NULL);

#ifdef DEBUG_SELECT_CHANNELS
    /*
     * shows the channel state.
     */
    fprintf(stderr,"res:%d nb:%d rd:",res,nb);
    for (fd = 0;fd < nb;fd++)
        if (FD_ISSET(fd, &full_readfds)) fprintf(stderr,"r");
        else fprintf(stderr,"-");
    fprintf(stderr," wr:");
    for (fd = 0;fd < nb;fd++)
        if (FD_ISSET(fd, &full_writefds)) fprintf(stderr,"w");
        else fprintf(stderr,"-");
    fprintf(stderr,"\n");
#endif

    /*
     * Error !
     */
    if (res < 0) {
        AND_FD(nb, &full_readfds, readfds);
        AND_FD(nb, &full_writefds, writefds);
        AND_FD(nb, &full_exceptfds, exceptfds);
	InJavaSelect = 0;
#ifdef DEBUG_SELECT
	TIMER
        fprintf(stderr,"X");
#endif
        return(res);
    }

    /*
     * Timeout, give control back to Kaffe.
     */
    if (res == 0) {
        AND_FD(nb, &full_readfds, readfds);
        AND_FD(nb, &full_writefds, writefds);
        AND_FD(nb, &full_exceptfds, exceptfds);
	InJavaSelect = 0;
#ifdef DEBUG_SELECT
	TIMER
        fprintf(stderr,">");
#endif
        return(0);
    }

    /*
     * the tricky part : analyze which external channels need attention
     * decrement res accordingly.
     */

    if (DoJavaSelectPoll) {
#ifdef DEBUG_SELECT
	TIMER
        fprintf(stderr,"JavaSelect : Register Poll break\n");
#endif
        BreakJavaSelectPoll++;
    }

    for (fd = 0;(fd < nb) && (res > 0); fd++) {
        if (FD_ISSET(fd, &lextra_readfds) && FD_ISSET(fd, &full_readfds)) {
#ifdef DEBUG_SELECT_CHANNELS
            fprintf(stderr,"reading on channel %d\n", fd);
#endif
            JavaSelectCallback(fd, EVENT_READ);
	    res--;
        }
        if (FD_ISSET(fd, &lextra_writefds) && FD_ISSET(fd, &full_writefds)) {
#ifdef DEBUG_SELECT_CHANNELS
            fprintf(stderr,"writing on channel %d\n", fd);
#endif
            JavaSelectCallback(fd, EVENT_WRITE);
	    res--;
        }
        if (FD_ISSET(fd, &lextra_exceptfds) && FD_ISSET(fd, &full_exceptfds)) {
#ifdef DEBUG_SELECT_CHANNELS
            fprintf(stderr,"exception on channel %d\n", fd);
#endif
	    res--;
        }
    }

    /*
     * If no Kaffe file descriptor need attention, loop on select
     * after updating tm.
     * !!!!!! update tm
     */
    if (res <= 0) {
#ifdef DEBUG_SELECT
	TIMER
        fprintf(stderr,"|");
#endif
        goto restart_select;
    }

    /*
     * Update Kaffe file descriptor sets.
     */
    AND_FD(nb, &full_readfds, readfds);
    AND_FD(nb, &full_writefds, writefds);
    AND_FD(nb, &full_exceptfds, exceptfds);
    /* !!! Do or do not update timeout ??? */
    InJavaSelect = 0;
#ifdef DEBUG_SELECT
    TIMER
    fprintf(stderr,">");
#endif
    return(res);
}

/************************************************************************
 *									*
 *	Locks Handling : One for the X-Windows Events, One for the	*
 *	Thotlib Access.							*
 *									*
 ************************************************************************/

static int ThotlibLockValue = 0;
static int XWindowSocketLockValue = 0;
static int XWindowSocketWaitValue = 0;


void JavaThotlibLock()
{
    while (1) {
        /*
	 * block on the entry.
	 */
        while (ThotlibLockValue > 0) {
	    /****
	    yieldThread();
	    blockInts++;
	    reschedule();
	    blockInts--;
	     ****/
#ifdef DEBUG_LOCK
	    TIMER
#endif
            fprintf(stderr,"JavaThotlibLock(%d,%d) : block\n",
	            ThotlibLockValue, XWindowSocketLockValue);
	    sleepThread(5);
        }

        /*
	 * try.
	 */
        ThotlibLockValue++;

	/*
	 * if not alone back ..
	 */
	if (ThotlibLockValue > 1) {
	   ThotlibLockValue--;
	   continue;
	}
#ifdef DEBUG_LOCK
	TIMER
        fprintf(stderr,"JavaThotlibLock(%d,%d) : Ok\n",
	        ThotlibLockValue, XWindowSocketLockValue);
#endif
	break;
    }
}

void JavaThotlibRelease()
{
    ThotlibLockValue--;
#ifdef DEBUG_LOCK
	TIMER
        fprintf(stderr,"JavaThotlibRelease(%d,%d)\n",
	        ThotlibLockValue, XWindowSocketLockValue);
#endif
}

void JavaXWindowSocketLock()
{
    while (1) {
        /*
	 * block on the entry.
	 */
        while (XWindowSocketLockValue > 0) {
#ifdef DEBUG_LOCK
	    TIMER
#endif
            fprintf(stderr,"JavaXWindowSocketLock(%d,%d) : block\n",
	            ThotlibLockValue, XWindowSocketLockValue);
	    XWindowSocketWaitValue++;
	    sleepThread(30);
	    XWindowSocketWaitValue--;
        }

        /*
	 * try.
	 */
        XWindowSocketLockValue++;

	/*
	 * if not alone back ..
	 */
	if (XWindowSocketLockValue > 1) {
	   XWindowSocketLockValue--;
	   continue;
	}
#ifdef DEBUG_LOCK
	TIMER
        fprintf(stderr,"JavaXWindowSocketLock(%d,%d) : Ok\n",
                ThotlibLockValue, XWindowSocketLockValue);
#endif
	break;
    }
}

void JavaXWindowSocketRelease()
{
    XWindowSocketLockValue--;
#ifdef DEBUG_LOCK
    TIMER
    fprintf(stderr,"JavaXWindowSocketRelease(%d,%d)\n",
            ThotlibLockValue, XWindowSocketLockValue);
#endif
}

/************************************************************************
 *									*
 *	Event Handling : Loops for events, fetching and handling	*
 *									*
 ************************************************************************/

ThotAppContext CurrentAppContext = NULL;

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

/*----------------------------------------------------------------------
  JavaFetchAvailableEvent

  This routine look at the socket used by the Network interface and
  the X-Windows event queue. It first handle changes in the network
  socket status, and handle them (atomic operations). Once done,
  if an X-Window event is available, the routine should fetch it from
  the queue in the ev argument and return TRUE, it returns FALSE otherwise.
  ----------------------------------------------------------------------*/
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
    KaffeObject* args;
    stringClass** str;
    char initClass[MAX_PATH];

    char *app_name = TtaGetEnvString ("appname");

    /* fprintf(stderr, "Initialize Java Runtime\n"); */

    /* Initialise */
    initialiseKaffe();
    /* biss_awt_kernel_NativeLib_initialize(); */

    /* Register Thotlib stubs */
    register_stubs();

    /* fprintf(stderr, "Java Runtime Initialized\n"); */

    /* Build the init class name */
    sprintf(initClass, "%s/%sInit", app_name, app_name);

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
   CloseJava

   Stops cleanly all the Java stuff.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CloseJava (void)
#else
void                CloseJava ()
#endif
{
    char initClass[MAX_PATH];

    char *app_name = TtaGetEnvString ("appname");

    /* fprintf(stderr, "Stop Java Runtime\n"); */

    /* Build the init class name */
    sprintf(initClass, "%s/%sInit", app_name, app_name);

    /* lauch the stop class for the application */
    do_execute_java_class_method(initClass, "Stop", "()V");

}

/*----------------------------------------------------------------------
  InitJavaEventLoop

  Initialize the JavaEventLoop environment, including the network
  interface, Java, etc...
  ----------------------------------------------------------------------*/
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

    /*
     * Register the X-Window socket as an input channel
     */
    x_window_socket = ConnectionNumber(TtaGetCurrentDisplay());
    threadedFileDescriptor(x_window_socket);

    /*
     * set up our own select call.
     */
    select_call = JavaSelect;

    /*
     * Startup the Java environment. We should never return
     * from this call, but InitJava will call TtaMainLoop again
     * on the Application thread.
     */
    InitJava();

}

/*----------------------------------------------------------------------
  JavaPollLoop

  This is the equivalent of the basic event loop except that it will
  return after any interraction on the extra file descriptors.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 JavaPollLoop ()
#else
int                 JavaPollLoop ()
#endif
{
#ifndef _WINDOWS
   ThotEvent           ev;
   int res;
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

   /* Loop waiting for the events */
   while (1)
     {
        while ((XWindowSocketWaitValue > 0) || (XWindowSocketLockValue > 0)) {
	    /*
	     * Don't block appplication thread reading events.
	     */
	    JavaThotlibRelease();
            sleepThread(5);
	    JavaThotlibLock();
	    if ((DoJavaSelectPoll) && (BreakJavaSelectPoll)) {
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
        while (JavaFetchEvent (&ev) < 0) {
	    DoJavaSelectPoll = 0;
	    BreakJavaSelectPoll = 0;
#ifdef DEBUG_SELECT
	    TIMER
            fprintf(stderr,"JavaPollLoop stopped\n");
#endif
	    return(res);
	}
        JavaHandleOneEvent (&ev);
     }
   /*ENOTREACHED*/
   return(0);
}

/*----------------------------------------------------------------------
  JavaEventLoop

  The point where events arriving from the X-Windows socket as well as
  the network sockets are fetched and dispatched to the correct handlers.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                JavaEventLoop ()
#else
void                JavaEventLoop ()
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
      return(-1);

   /*
    * We don't want to jump off the loop if transfers did occurs
    * on the extra descriptors.
    */
   DoJavaSelectPoll = 0;
   BreakJavaSelectPoll = 0;

   /* Loop waiting for the events */
   while (1)
     {
        while (XWindowSocketWaitValue > 0) {
	    /*
	     * Don't block appplication thread reading events.
	     */
	    JavaThotlibRelease();
            sleepThread(30);
	    JavaThotlibLock();
	    continue;
	}
        while (JavaFetchEvent (&ev) < 0) {
	    DoJavaSelectPoll = 0;
	    BreakJavaSelectPoll = 0;
	}
        JavaHandleOneEvent (&ev);
     }
}

/*----------------------------------------------------------------------
   JavaLoadResources 

   link in the Java stuff and initialize it.
  ----------------------------------------------------------------------*/
void                JavaLoadResources ()
{
   TtaSetMainLoop (InitJavaEventLoop, JavaEventLoop,
		   JavaFetchEvent, JavaFetchAvailableEvent);
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
   register_biss_awt_API_stubs();
   register_thotlib_Extra_stubs();
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
   register_amaya_APIAmayaMsg_stubs();
   register_amaya_APIJavaAmaya_stubs();
}

