/*				    				    AHTEvntrg.c
**	Amaya EVENT MANAGER
**
*/

/* Amaya includes  */
#define THOT_EXPORT extern

/* Implementation dependent include files */
#include "amaya.h"
#include "AHTBridge_f.h"
#include <assert.h>			/* @@@ Should be in sysdep.h @@@ */
#include <fcntl.h>

static HWND HTSocketWin;
static unsigned long HTwinMsg;

/* Type definitions and global variables etc. local to this module */

PRIVATE int HTEndLoop = 0;		       /* If !0 then exit event loop */
typedef unsigned long DWORD;

/*
** our internal structure to hold a socket, it's request 
** and our action. For version 1, we allow one action per socket
*/

typedef struct rq_t RQ;

/*
** an action consists of a request, a set of requested operations 
** a HTEventCallback function, and a priority (priority is not used in this
** version)
*/

#ifndef WWW_MSWINDOWS 
typedef void * HANDLE ;
#endif


/* ------------------------------------------------------------------------- */

/*  HTEventrg_loop
**  ------------
**  event loop: that is, we wait for activity from one of our registered 
**  channels, and dispatch on that.
**
**  There are now two versions of the event loop. The first is if you want
**  to use async I/O on windows, and the other is if you want to use normal
**  Unix setup with sockets
*/

/* only responsible for WM_TIMER and WSA_AsyncSelect */    	
PUBLIC LRESULT CALLBACK AmayaAsyncWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    WORD event;
    SOCKET sock;
    HTEventCallback    *cbf;
	HTRequest *rqp;

    if (uMsg != HTwinMsg)	/* not our async message */
    	return (DefWindowProc(hwnd, uMsg, wParam, lParam));

    event = LOWORD(lParam);
    sock = (SOCKET)wParam;

    cbf = (HTEventCallback *) __RetrieveCBF (sock, FD_WRITE, &rqp);

    if (event & FD_CLOSE) {
		/* close the socket and unregister it from the Windows environment */
    	if (HTEventrg_dispatch((int)sock, FD_READ) != HT_OK)
	        HTEndLoop = -1;
	    WSAAsyncSelect(sock, HTSocketWin, 0, 0);
		return 0;
	}
    if (event & (FD_READ | FD_ACCEPT))
    	if (HTEventrg_dispatch((int)sock, FD_READ) != HT_OK) {
	    HTEndLoop = -1;
	    return 0;
	}
    if (event & (FD_WRITE | FD_CONNECT))
    	if (HTEventrg_dispatch((int)sock, FD_WRITE) != HT_OK) {
	    HTEndLoop = -1;
	    return 0;
	}
    if (event & FD_OOB)
    	if (HTEventrg_dispatch((int)sock, FD_OOB) != HT_OK) {
	    HTEndLoop = -1;
	    return 0;
	}
	  	
    return (0);
}



ThotBool AHTEventInit (void)
{
    /*
    **	We are here starting a hidden window to take care of events from
    **  the async select() call in the async version of the event loop in
    **	the Internal event manager (HTEvntrg.c)
    */
    static char className[] = "AsyncWindowClass";
    WNDCLASS wc;
    OSVERSIONINFO osInfo;
    
    wc.style=0;
    wc.lpfnWndProc=(WNDPROC)AmayaAsyncWindowProc;
    wc.cbClsExtra=0;
    wc.cbWndExtra=0;
    wc.hIcon=0;
    wc.hCursor=0;
    wc.hbrBackground=0;
    wc.lpszMenuName=(LPSTR)0;
    wc.lpszClassName=className;

    osInfo.dwOSVersionInfoSize = sizeof(osInfo);
    GetVersionEx(&osInfo);
    if (osInfo.dwPlatformId == VER_PLATFORM_WIN32s || osInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
	wc.hInstance=GetModuleHandle(NULL); /* 95 and non threaded platforms */
    else
	wc.hInstance=GetCurrentProcess(); /* NT and hopefully everything following */
    if (!RegisterClass(&wc)) {
    	HTTrace("HTLibInit.. Can't RegisterClass \"%s\"\n", className);
	    return NO;
    }
    if (!(HTSocketWin = CreateWindow(className, "WWW_WIN_ASYNC", WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT, 
                                     CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, wc.hInstance,0))) {
       char *space = TtaGetMemory (50);
       HTTrace("HTLibInit.. Can't CreateWindow \"WWW_WIN_ASYNC\" - error:");
       sprintf(space, "%ld\n", GetLastError());
       HTTrace(space);
	   TtaFreeMemory (space);
       return NO;
    }
    HTwinMsg = WM_USER;  /* use first available message since app uses none */
    HTEventrg_setWinHandle  (HTSocketWin, HTwinMsg);
#ifdef _WINSOCKAPI_
    /*
    ** Initialise WinSock DLL. This must also be shut down! PMH
    */
    {
        WSADATA            wsadata;
	if (WSAStartup(DESIRED_WINSOCK_VERSION, &wsadata)) {
	    if (WWWTRACE)
		HTTrace("HTEventInit. Can't initialize WinSoc\n");
            WSACleanup();
            return NO;
        }
        if (wsadata.wVersion < MINIMUM_WINSOCK_VERSION) {
            if (WWWTRACE)
		HTTrace("HTEventInit. Bad version of WinSoc\n");
            WSACleanup();
            return NO;
        }
	if (APP_TRACE)
	    HTTrace("HTEventInit. Using WinSoc version \"%s\".\n", 
		    wsadata.szDescription);
    }
#endif /* _WINSOCKAPI_ */

    HTEvent_setRegisterCallback(AHTEvent_register);
	HTEvent_setUnregisterCallback (AHTEvent_unregister);
    return YES;
}

PUBLIC BOOL AHTEventTerminate (void)
{
#ifdef _WINSOCKAPI_
    WSACleanup();
#endif

    DestroyWindow(HTSocketWin);
    return YES;
}
