#ifndef _WIN_FCT_
#define _WIN_FCT_
#include <windows.h>

#ifdef __STDC__
extern void    WinErrorBox (HWND);
extern void    WinLoadGC (HDC, int, int);
extern HFONT   WinLoadFont (HDC, ptrfont);
extern int     WIN_InitSystemColors ();
extern void    WIN_GetDeviceContext (int);
extern void    WIN_ReleaseDeviceContext ();
extern BOOL    WIN_TtaInitDialogue (char*, char*, char*, char*);
extern void    WIN_TtaSetPulldownOff (int, ThotWidget, HWND);
extern void    WIN_TtaSetPulldownOn (int, ThotWidget, HWND);
extern void    WIN_TtaSetToggleMenu (int, int, boolean, HWND);
extern void    WIN_APP_TextCallback (HWND, int);
extern void    WIN_ThotCallBack (HWND, WPARAM, LPARAM);
extern void    WIN_CharTranslation (HWND, int, UINT, WPARAM, LPARAM);
extern void    WIN_InitDialogueFonts (HDC, char*);
extern void    APP_ButtonCallback (ThotWidget, int, caddr_t);
extern void    CleanFrameCatList (int);
extern int     GetMainFrameNumber (ThotWindow);
extern LRESULT ToolBarNotify (int, HWND, WPARAM, LPARAM);
extern int     GetFrameNumber (ThotWindow);
extern void    TtaHandleOneWindowEvent (MSG*);
#else  /* __STDC__ */
extern void    WinErrorBox ();
extern void    WinLoadGC ();
extern HFONT   WinLoadFont ();
extern int     WIN_InitSystemColors ();
extern void    WIN_GetDeviceContext ();
extern void    WIN_ReleaseDeviceContext ();
extern BOOL    WIN_TtaInitDialogue ();
extern void    WIN_TtaSetPulldownOff ();
extern void    WIN_TtaSetPulldownOn ();
extern void    WIN_TtaSetToggleMenu ();
extern void    WIN_APP_TextCallback ();
extern void    WIN_ThotCallBack ();
extern void    WIN_CharTranslation ();
extern void    WIN_InitDialogueFonts ();
extern void    APP_ButtonCallback ();
extern void    CleanFrameCatList ();
extern int     GetMainFrameNumber ();
extern LRESULT ToolBarNotify ();
extern int     GetFrameNumber ();
extern void    TtaHandleOneWindowEvent ();
#endif /* __STDC__ */

#endif _WIN_FCT_

