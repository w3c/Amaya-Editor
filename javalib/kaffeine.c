/*
 * kaffeine.c : Interface for the Kaffe Java interpreter.
 *
 * Daniel Veillard 1997
 */

#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include "kaffeine.h"
#include "registry.h"
#include "events.h"
#include "JavaX11Interf.h"

#include "w3c_thotlib_APIApplication_stubs.h"
#include "w3c_thotlib_APIDocument_stubs.h"
#include "w3c_thotlib_APITree_stubs.h"
#include "w3c_thotlib_APIContent_stubs.h"
#include "w3c_thotlib_APIAttribute_stubs.h"
#include "w3c_thotlib_APIReference_stubs.h"
#include "w3c_thotlib_APILanguage_stubs.h"
#include "w3c_thotlib_APIPresentation_stubs.h"
#include "w3c_thotlib_APIView_stubs.h"
#include "w3c_thotlib_APISelection_stubs.h"
#include "w3c_thotlib_APIInterface_stubs.h"
#include "w3c_thotlib_APIRegistry_stubs.h"
#include "w3c_thotlib_APIDialog_stubs.h"
#include "w3c_thotlib_APIExtra_stubs.h"
#include "w3c_amaya_APIAmayaMsg_stubs.h"
#include "w3c_amaya_APIJavaAmaya_stubs.h"
#include "debug_stubs.h"

#include "debug_stubs_f.h"

/*****
#define DEBUG_SELECT
#define DEBUG_SELECT_CHANNELS
#define DEBUG_TIMING
 *****/

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

#ifdef __STDC__
void             JavaHandleAvailableEvents (void);
#else
void             JavaHandleAvailableEvents ();
#endif

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
int                JavaSelect (int  nb,  fd_set  *readfds,  fd_set  *writefds,
       fd_set *exceptfds, struct timeval *timeout)
#else
int                JavaSelect (nb, readfds, writefds, exceptfds, timeout)
int  nb;
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
#ifdef DEBUG_SELECT_CHANNELS
    int fd;
#endif
    static int check_for_xt_events = 0;
    int use_extra_timer = 0;
    struct timeval *extra_timer;
    int n;

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

rerun_select:
    n = nb;

    if (check_for_xt_events) {
        check_for_xt_events = 0;
	JavaHandleAvailableEvents();
    }

#ifdef DEBUG_SELECT
    fprintf(stderr,"\n");
    TIMER
#endif

    /*
     * Do not block if there is a Poll Break requested.
     * Otherwise check for Kaffe and User timers.
     */
    if ((DoJavaSelectPoll) && (BreakJavaSelectPoll)) {
        tm.tv_usec = 0;
        tm.tv_sec = 0;
	use_extra_timer = 0;
    } else {
        /*
	 * Check for any extra timer defined .
	 */
	extra_timer = nextTimer();
	
        if (timeout == NULL) {
	    /*
	     * Kaffe didn't provide a time-out.
	     */
	    if (extra_timer != NULL) {
		/* User's delay ... */
		tm.tv_usec = extra_timer->tv_usec;
		tm.tv_sec = extra_timer->tv_sec;
		use_extra_timer = 1;
	    } else {
	        /*
		 * No time-out at all enforce 25 msec...
		 */
		tm.tv_sec = 0;
		tm.tv_usec = 25000;
		use_extra_timer = 0;
	    }
	} else {
	    /* normalize the timeout (we never know ...) */
	    timeout->tv_sec += timeout->tv_usec / 1000000;
	    timeout->tv_usec %= 1000000;

	    if (extra_timer == NULL) {
	        /*
		 * Kaffe provided a time limit, use it !
		 */
		tm.tv_usec = timeout->tv_usec;
		tm.tv_sec = timeout->tv_sec;
		use_extra_timer = 0;
	    } else {
	        /*
		 * Both Kaffe and User provided time limits, use the
		 * shortest delay.
		 */
		if ((extra_timer->tv_sec > timeout->tv_sec) ||
		    ((extra_timer->tv_sec == timeout->tv_sec) &&
		     (extra_timer->tv_usec > timeout->tv_usec)))  {
		    /* Kaffe delay ... */
		    tm.tv_usec = timeout->tv_usec;
		    tm.tv_sec = timeout->tv_sec;
		    use_extra_timer = 0;
		} else {
		    /* User's delay ... */
		    tm.tv_usec = extra_timer->tv_usec;
		    tm.tv_sec = extra_timer->tv_sec;
		    use_extra_timer = 1;
		}
	    }
	}
    }

    /*
     * Merge the file descriptors with clients one, if any.
     */
    createChannelMasks(&n, readfds, writefds);


#ifdef DEBUG_SELECT_CHANNELS
    /*
     * shows the channel state.
     */
    fprintf(stderr,"nb:%02d rd:",n);
    if (readfds != NULL)
      for (fd = 0;fd < n;fd++)
        if (FD_ISSET(fd, readfds)) fprintf(stderr,"r");
        else fprintf(stderr,"-");
    fprintf(stderr," wr:");
    if (writefds != NULL)
      for (fd = 0;fd < n;fd++)
        if (FD_ISSET(fd, writefds)) fprintf(stderr,"w");
        else fprintf(stderr,"-");
    fprintf(stderr,"\n");
#endif

    /*
     * Do the select on the merged channels descriptors and timeouts.
     */
    NbJavaSelect++;
    if (((DoJavaSelectPoll) && (BreakJavaSelectPoll)) ||
        (timeout != NULL))
       res = select(n, readfds, writefds, exceptfds, &tm);
    else {
       res = select(n, readfds, writefds, exceptfds, NULL);
    }

    /*
     * Update the timeout return, if any ...
     */
    if (timeout != NULL)
	memcpy(timeout, &tm, sizeof(tm));

#ifdef DEBUG_SELECT_CHANNELS
    /*
     * shows the channel state.
     */
    TIMER
    fprintf(stderr,"res:%02d nb:%02d rd:",res,n);
    if (readfds != NULL)
      for (fd = 0;fd < n;fd++)
        if (FD_ISSET(fd, readfds)) fprintf(stderr,"r");
        else fprintf(stderr,"-");
    fprintf(stderr," wr:");
    if (writefds != NULL)
      for (fd = 0;fd < n;fd++)
        if (FD_ISSET(fd, writefds)) fprintf(stderr,"w");
        else fprintf(stderr,"-");
    fprintf(stderr,"\n");
#endif

    /*
     * Error or get interrupted.
     */
    if (res < 0) {
	InJavaSelect = 0;
	check_for_xt_events = 1;
#ifdef DEBUG_SELECT
        perror("select");
#endif
        return(res);
    }

    /*
     * Timeout, depending on wether it's a user-timeout or
     * Kaffe one, give control back to Kaffe or run the
     * user's timeout checks.
     */
    if (res == 0) {
        if (use_extra_timer) {
	    checkTimers();
	    /* Decrement any Kaffe timer if any !!! */
	    goto rerun_select;
	} else {
	    InJavaSelect = 0;
	    check_for_xt_events = 1;
#ifdef DEBUG_SELECT
	    fprintf(stderr,">\n");
#endif
	    return(0);
	}
    }

    /*
     * Handle User I/O for registered channels.
     * If there is no I/O left for Kaffe threads, redo the select.
     */
    checkChannelMasks(n, readfds, writefds, &res);
    if (res <= 0) goto rerun_select;

    /*
     * If there was a Poll in progress register that we need to interrupt.
     */

    if (DoJavaSelectPoll) {
#ifdef DEBUG_SELECT
        fprintf(stderr,"JavaSelect : Register Poll break\n");
#endif
        BreakJavaSelectPoll++;
    }

    InJavaSelect = 0;
#ifdef DEBUG_SELECT
    TIMER
    fprintf(stderr,">\n");
#endif
    return(res);
}


/************************************************************************
 *									*
 *	Locks Handling : One for the X-Windows Events, One for the	*
 *	Thotlib Access and one for the DNS server.			*
 *									*
 ************************************************************************/

int ThotlibLockValue = 0;
int XWindowSocketLockValue = 0;
int XWindowSocketWaitValue = 0;

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
        fprintf(stderr,"DNSserverLock(%d) : Ok\n",
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

    /* Initialize the type conversion unit */
    initJavaTypes();

    /* fprintf(stderr, "Java Runtime Initialized\n"); */

    /* Build the init class name */
    sprintf(initClass, "w3c.%s.%sInit", app_name, app_name);

    /* Build each string and put into the array */
    str = makeJavaString(app_name, strlen(app_name));

    /* lauch the init class for the application */
    do_execute_java_class_method(initClass, "main",
                   "(Ljava/lang/String;)V", str);

    /* Start the application loop of events */
    do_execute_java_class_method("w3c.thotlib.Interface", "main",
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
    sprintf(initClass, "w3c.%s.%sInit", app_name, app_name);

    /* lauch the stop class for the application */
    do_execute_java_class_method(initClass, "Stop", "()V");

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
   register_w3c_thotlib_Extra_stubs();
   register_w3c_thotlib_APIApplication_stubs();
   register_w3c_thotlib_APIDocument_stubs();
   register_w3c_thotlib_APITree_stubs();
   register_w3c_thotlib_APIContent_stubs();
   register_w3c_thotlib_APIAttribute_stubs();
   register_w3c_thotlib_APIReference_stubs();
   register_w3c_thotlib_APILanguage_stubs();
   register_w3c_thotlib_APIPresentation_stubs();
   register_w3c_thotlib_APIView_stubs();
   register_w3c_thotlib_APISelection_stubs();
   register_w3c_thotlib_APIInterface_stubs();
   register_w3c_thotlib_APIRegistry_stubs();
   register_w3c_amaya_APIAmayaMsg_stubs();
   register_w3c_amaya_APIJavaAmaya_stubs();
   register_debug_stubs();
}

