#ifdef _WINDOWS

#ifndef _COMPILERS_F_H_
#define _COMPILERS_F_H_

#define DLLEXPORT __declspec(dllexport)

#ifdef __STDC__ 
DLLEXPORT int APPmain (HWND, HWND, int, STRING*, int*);
DLLEXPORT int PRSmain (HWND, HWND, int, STRING*, int*);
DLLEXPORT int STRmain (HWND, HWND, int, STRING*, int*);
DLLEXPORT int TRAmain (HWND, HWND, int, STRING*, int*);
DLLEXPORT int CPPmain (HWND, HWND, int, STRING*, int*);
#else  /* __STDC__ */
DLLEXPORT int APPmain ();
DLLEXPORT int PRSmain ();
DLLEXPORT int STRmain ();
DLLEXPORT int TRAmain ();
DLLEXPORT int CPPmain ();
#endif /* __STDC__ */

#endif /* _COMPILERS_F_H_ */
#endif /* _WINDOWS */

