#ifndef _WIN_FCT_
#define _WIN_FCT_

#ifdef __STDC__
extern void WinErrorBox (HWND);
extern int  WIN_InitSystemColors ();
extern void WIN_GetDeviceContext (int);
extern void WIN_ReleaseDeviceContext ();
#else  /* __STDC__ */
extern void WinErrorBox ();
extern int  WIN_InitSystemColors ();
extern void WIN_GetDeviceContext ();
extern void WIN_ReleaseDeviceContext ();
#endif /* __STDC__ */

#endif _WIN_FCT_