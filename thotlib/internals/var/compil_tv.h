/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * Variables used by compilers grm, str, prs, and app
 */

/* message origin */
THOT_EXPORT int COMPIL;
THOT_EXPORT int STR;
THOT_EXPORT int PRS;
THOT_EXPORT int TRA;
THOT_EXPORT int GRM;
THOT_EXPORT int APP;

/* a line from the source file being compiled */
THOT_EXPORT lineBuffer 	inputLine;

/* an error has been detected */
THOT_EXPORT ThotBool 	error;

#ifdef _WINDOWS
THOT_EXPORT HDC compilersDC;
THOT_EXPORT HWND COMPWnd;
THOT_EXPORT int    _CY_;
#endif /* _WINDOWS */
