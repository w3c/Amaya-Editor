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
#include "thotlib_APIExtra_stubs.h"
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
    DoJavaSelectPoll = 0;
    BreakJavaSelectPoll = 0;
#ifdef DEBUG_SELECT
    TIMER
    fprintf(stderr,"InitJavaSelect\n");
#endif
    JavaSelectInitialized = 1;
}

/*----------------------------------------------------------------------
  JavaSelect

  This routine provide a shared select() syscall needed to multiplex
  the various packages (Java, X-Windows ...) needing I/O in
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
    struct timeval tm;
    int res;
    static int InJavaSelect = 0;

    if (!JavaSelectInitialized) InitJavaSelect();

    /*
     * Check for reentrancy, would be a Very Bad Thing (c)
    if ((InJavaSelect) &&
        ((timeout == NULL) || (timeout->tv_usec != 0) ||
	 (timeout->tv_sec != 0))){
	TIMER
        fprintf(stderr, "JavaSelect reentrancy !\n");
    }
     */

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

    /*
     * Do not block if there is a Poll Break requested.
     */
    if ((DoJavaSelectPoll) && (BreakJavaSelectPoll)) {
       tm.tv_usec = 0;
       tm.tv_sec = 0;
    } else if (timeout != NULL)
       memcpy(&tm, timeout, sizeof(tm));

    /*
     * Do the select on the merged channels descriptors.
     */
    NbJavaSelect++;
    if (((DoJavaSelectPoll) && (BreakJavaSelectPoll)) ||
        (timeout != NULL))
       res = select(n, readfds, writefds, exceptfds, &tm);
    else
       res = select(n, readfds, writefds, exceptfds, NULL);

#ifdef DEBUG_SELECT_CHANNELS
    /*
     * shows the channel state.
     */
    fprintf(stderr,"res:%d nb:%d rd:",res,n);
    for (fd = 0;fd < n;fd++)
        if (FD_ISSET(fd, readfds)) fprintf(stderr,"r");
        else fprintf(stderr,"-");
    fprintf(stderr," wr:");
    for (fd = 0;fd < n;fd++)
        if (FD_ISSET(fd, writefds)) fprintf(stderr,"w");
        else fprintf(stderr,"-");
    fprintf(stderr,"\n");
#endif

    /*
     * Error !
     */
    if (res < 0) {
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
	InJavaSelect = 0;
#ifdef DEBUG_SELECT
	TIMER
        fprintf(stderr,">");
#endif
        return(0);
    }

    /*
     * If there was a Poll in progress register that we need to interrupt.
     */

    if (DoJavaSelectPoll) {
#ifdef DEBUG_SELECT
	TIMER
        fprintf(stderr,"JavaSelect : Register Poll break\n");
#endif
        BreakJavaSelectPoll++;
    }

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
 *	Thotlib Access and one for the DNS server.			*
 *									*
 ************************************************************************/

static int ThotlibLockValue = 0;
static int XWindowSocketLockValue = 0;
static int XWindowSocketWaitValue = 0;
#ifndef SYNC_DNS
static int DNSLockValue = 0;
#endif


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
            fprintf(stderr,"JavaThotlibLock(%d,%d) : block\n",
	            ThotlibLockValue, XWindowSocketLockValue);
#endif
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
            fprintf(stderr,"JavaXWindowSocketLock(%d,%d) : block\n",
	            ThotlibLockValue, XWindowSocketLockValue);
#endif
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

#ifndef SYNC_DNS
void DNSserverLock()
{
    while (1) {
        /*
	 * block on the entry.
	 */
        while (DNSLockValue > 0) {
#ifdef DEBUG_LOCK
	    TIMER
            fprintf(stderr,"DNSserverLock(%d) : block\n", DNSLockValue);
#endif
	    sleepThread(5);
        }

        /*
	 * try.
	 */
        DNSLockValue++;

	/*
	 * if not alone back ..
	 */
	if (DNSLockValue > 1) {
	   DNSLockValue--;
	   continue;
	}
#ifdef DEBUG_LOCK
	TIMER
        fprintf(stderr,"JavaXWindowSocketLock(%d,%d) : Ok\n",
                DNSLockValue);
#endif
	break;
    }
}

void DNSserverRelease()
{
    DNSLockValue--;
#ifdef DEBUG_LOCK
    TIMER
    fprintf(stderr,"DNSserverRelease(%d)\n", DNSLockValue);
#endif
}
#endif /* !SYNC_DNS */


#ifndef SYNC_DNS
/************************************************************************
 *									*
 *			Non-blocking DNS lookups.			*
 *									*
 ************************************************************************/

int dns_daemonRequestChannel[2];
int dns_daemonResultChannel[2];
FILE *dns_daemonRequest;
FILE *dns_daemonResult;
int dns_daemon_pid;
int JavaDnsInitialized = 0;
extern char BinariesDirectory[];

static void JavaInitDns(void) {
    /*
     * create a pipe to exchange data with the dns_daemon.
     */
    if (pipe(dns_daemonRequestChannel) < 0) {
	perror("JavaInitDns : pipe(dns_daemonRequestChannel) failed");
	exit(1);
    }
    if (pipe(dns_daemonResultChannel) < 0) {
	perror("JavaInitDns : pipe(dns_daemonResultChannel) failed");
	exit(1);
    }

    /*
     * fork : the subprocess will launch dns_daemon after having
     *        resets its stdin and stdout channels.
     */
    if ((dns_daemon_pid = fork()) == 0) {
        char path[1024];

        /*
	 * reaffect the stdin to the read part of the RequestChannel pipe.
	 */
        close(0);
	dup(dns_daemonRequestChannel[0]);
	close(dns_daemonRequestChannel[0]);
	close(dns_daemonRequestChannel[1]);

        /*
	 * reaffect the stdout to the write part of the ResultChannel pipe.
	 */
        close(1);
	dup(dns_daemonResultChannel[1]);
	close(dns_daemonResultChannel[0]);
	close(dns_daemonResultChannel[1]);

	/*
	 * Now exec the dns_daemon.
	 */
	sprintf(path, "%s/dns_daemon", BinariesDirectory);
	execlp(path, path, NULL);
	perror("JavaInitDns : cannot execve dns_daemon");
	fprintf(stderr,"Check that %s binary is in place\n", path);
	exit(1);
    }
    if (dns_daemon_pid < 0) {
	perror("JavaInitDns : fork() failed");
	exit(1);
    }
    /*
     * Close the two file descriptor not used on the client side.
     */
    close(dns_daemonRequestChannel[0]);
    close(dns_daemonResultChannel[1]);

    /*
     * and register the two others as Input/Output channels.
     */
    threadedFileDescriptor(dns_daemonRequestChannel[1]);
    threadedFileDescriptor(dns_daemonResultChannel[0]);

    /*
     * and register the two others as Input/Output channels.
     */
    dns_daemonRequest = fdopen(dns_daemonRequestChannel[1], "w");
    dns_daemonResult = fdopen(dns_daemonResultChannel[0], "r");
    JavaDnsInitialized++;
}

static struct hostent gethostbyname_result;
static char hostname_result[256];
static char *ip_addr_list[2];
static unsigned char ip_addr_result[20];

struct hostent *gethostbyname(const char *name) {
    char msg[1000];
    char host[256];
    char address[50];
    int addrtype;
    int addrlength;
    int res;
    int retries = 0;
    int ip[4];

    if (!JavaDnsInitialized) JavaInitDns();

retry:
    /*
     * Send request to the DNS server.
     */
    if (name == NULL) name = "";
    if (fprintf(dns_daemonRequest, "%s\n", name) < 0) {
	perror("JavaDns : gethostbyname() write to RequestChannel failed");
	goto io_failed;
    }
    fflush(dns_daemonRequest); /* needed !!! */

    /*
     * The key point of all this DNS thing : replace a blocking system
     * call to a polled one !!!
     */
    if (JavaSelectInitialized)
	do {
	    res = blockOnFile(dns_daemonResultChannel[0], 0);
	} while (res < 0);

    /*
     * Process the result from the server.
     */
    if (!fgets(msg, sizeof(msg), dns_daemonResult)) {
	perror("JavaDns : gethostbyname() read from ResultChannel failed");
	goto io_failed;
    }
    res = sscanf(msg, "%s %d %d %s", host, &addrtype, &addrlength, address);
    if (res < 4) goto dns_failed;

    if ((addrtype == 0) || (addrlength == 0)) goto dns_failed;

    /* set up h_name in answer */
    strncpy(hostname_result, name, sizeof(hostname_result));
    gethostbyname_result.h_name = hostname_result;
    
    /* ignore h_aliases in answer */
    gethostbyname_result.h_aliases = NULL;

    /* set up h_addrtype in answer */
    gethostbyname_result.h_addrtype = addrtype;
    
    /* set up h_length in answer */
    gethostbyname_result.h_length = addrlength;

    /* parse the IP addresses and return the first one in the list */
    sscanf(address, "%d.%d.%d.%d", &ip[0],&ip[1],&ip[2],&ip[3]);
    ip_addr_result[0] = (unsigned char) ip[0];
    ip_addr_result[1] = (unsigned char) ip[1];
    ip_addr_result[2] = (unsigned char) ip[2];
    ip_addr_result[3] = (unsigned char) ip[3];
    ip_addr_list[0] = &ip_addr_result[0];
    ip_addr_list[1] = NULL;
    gethostbyname_result.h_addr_list = &ip_addr_list[0];
    return(&gethostbyname_result);

dns_failed:
    
    return(NULL);

io_failed:
    if (retries < 3) {
	retries++;
	fprintf(stderr,"restarting dns_daemon\n");
	fclose(dns_daemonRequest);
	fclose(dns_daemonResult);
	JavaInitDns();
	goto retry;
    }
    return(NULL);
}

#endif /* !SYNC_DNS */

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
    struct Hjava_lang_String* str;
    char initClass[MAX_PATH];

    char *app_name = TtaGetEnvString ("appname");

#ifndef SYNC_DNS
    JavaInitDns();
#endif

    /* fprintf(stderr, "Initialize Java Runtime\n"); */

    /* Initialise */
    initialiseKaffe();
    /* biss_awt_kernel_NativeLib_initialize(); */

    /* Register Thotlib stubs */
    register_stubs();

    /* fprintf(stderr, "Java Runtime Initialized\n"); */

    /* Build the init class name */
    sprintf(initClass, "%s/%sInit", app_name, app_name);

    /* Build each string and put into the array */
    str = makeJavaString(app_name, strlen(app_name));

    /* lauch the init class for the application */
    do_execute_java_class_method(initClass, "main",
                   "(Ljava/lang/String;)V", str);

    /* Start the application loop of events */
    do_execute_java_class_method("thotlib.Interface", "main",
                   "(Ljava/lang/String;)V", str);
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
  JavaStopPoll

  Stop the poll loop (below).
  ----------------------------------------------------------------------*/
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
      return;

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

