/*				    				    AHTEvntrg.c
**	EVENT MANAGER
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	Updated HTEvent module 
**	This new module combines the functions of the old HTEvent module and 
**	the HTThread module. We retain the old HTThread module, but it
**	consists of calls to the HTEvent interfaces
**
** Authors:
**	HFN	Henrik Frystyk <frystyk@w3.org>
**	CLB    	Charlie Brooks <cbrooks@osf.org>
** Bugs
**
*/

/*   WSAAsyncSelect and windows app stuff need the following definitions:
 *   WWW_WIN_ASYNC - enable WSAAsyncSelect instead of select
 *   _WIN23 - win32 libararies - may be window or console app
 *   _WINSOCKAPI_ - using WINSOCK.DLL - not necessarily the async routines.
 *   _CONSOLE - the console app for NT
 *
 * first pass: EGP - 10/26/95
 */

#include <assert.h>			/* @@@ Should be in sysdep.h @@@ */

/* Implementation dependent include files */
#include "sysdep.h"
#include "HTEvntrg.h"					 /* Implemented here */

#ifdef WWW_WIN_ASYNC
#define TIMEOUT	1 /* WM_TIMER id */
static HWND HTSocketWin;
static unsigned long HTwinMsg;
#endif

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

typedef struct action_t  { 
    HTRequest * rq ;    				/* request structure */
    SockOps ops ;         			     /* requested operations */
    HTEventCallback *cbf;      		     /* callback function to execute */
    HTPriority p;         	     /* priority associated with this socket */
} ACTION ;

struct rq_t { 
    RQ * next ;	  
    SOCKET s ;		 	/* our socket */
    BOOL unregister;		/* notify app when completely unregistered */
    ACTION actions[1];
};

#ifndef WWW_MSWINDOWS 
typedef void * HANDLE ;
#endif

/* ------------------------------------------------------------------------- */

#ifdef WWW_WIN_ASYNC
/*	HTEventrg_get/setWinHandle
**	--------------------------
**	Managing the windows handle on Windows
*/
PUBLIC BOOL HTEventrg_setWinHandle (HWND window, unsigned long message)
{
    HTSocketWin = window;
    HTwinMsg = message;
    return YES;
}

PUBLIC HWND HTEventrg_getWinHandle (unsigned long * pMessage)
{
    if (pMessage)
        *pMessage = HTwinMsg;
    return (HTSocketWin);
}
#else
#ifdef WWW_WIN_DLL
/*
**	By having these dummy definitions we can keep the same def file
*/
PUBLIC BOOL HTEventrg_setWinHandle (HWND window, unsigned long message)
{
    return YES;
}

PUBLIC HWND HTEventrg_getWinHandle (unsigned long * pMessage)
{
    return (HWND) 0;
}
#endif /* WWW_WIN_DLL */
#endif /* WWW_WIN_ASYNC */


/*  HTEventrg_loop
**  ------------
**  event loop: that is, we wait for activity from one of our registered 
**  channels, and dispatch on that.
**
**  There are now two versions of the event loop. The first is if you want
**  to use async I/O on windows, and the other is if you want to use normal
**  Unix setup with sockets
*/
#ifdef WWW_WIN_ASYNC
/* only responsible for WM_TIMER and WSA_AsyncSelect */    	
LRESULT CALLBACK AmayaAsyncWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    WORD event;
    SOCKET sock;

    /* timeout stuff */
    if (uMsg == WM_TIMER) {
	if (seltime.tcbf && (seltime.always || HTNet_isIdle())) {
	    if (THD_TRACE)
		HTTrace("Event Loop.. calling timeout cbf\n");
	    (*(seltime.tcbf))(seltime.request);
	}
	return (0);
    }

    if (uMsg != HTwinMsg)	/* not our async message */
    	return (DefWindowProc(hwnd, uMsg, wParam, lParam));

    event = LOWORD(lParam);
    sock = (SOCKET)wParam;
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
    if (event & FD_CLOSE) /* delete this event */ 
      WSAAsyncSelect( (int)sock, HTSocketWin, HTwinMsg, 0);

    return (0);
}

#endif

bool AHTEventInit (void)
{
#ifdef WWW_WIN_ASYNC
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
 	char space[50];
       	HTTrace("HTLibInit.. Can't CreateWindow \"WWW_WIN_ASYNC\" - error:");
	sprintf(space, "%ld\n", GetLastError());
	HTTrace(space);
    	return NO;
    }
    HTwinMsg = WM_USER;  /* use first available message since app uses none */
#endif /* WWW_WIN_ASYNC */

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

    HTEvent_setRegisterCallback(HTEventrg_register);
    HTEvent_setUnregisterCallback(HTEventrg_unregister);
    return YES;
}

PUBLIC BOOL AHTEventTerminate (void)
{
#ifdef _WINSOCKAPI_
    WSACleanup();
#endif

#ifdef WWW_WIN_ASYNC
    DestroyWindow(HTSocketWin);
#endif
    return YES;
}





