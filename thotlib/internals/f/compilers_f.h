#ifdef _WINDOWS

#ifndef _COMPILERS_F_H_
#define _COMPILERS_F_H_

#define DLLEXPORT __declspec(dllexport)

#ifdef __STDC__ 
DLLEXPORT int APPmain (HWND, int, char**, int*);
DLLEXPORT int PRSmain (HWND, int, char**, int*);
DLLEXPORT int STRmain (HWND, int, char**, int*);
DLLEXPORT int TRAmain (HWND, int, char**, int*);
DLLEXPORT int CPPmain (HWND, int, char**, int*);
#else  /* __STDC__ */
DLLEXPORT int APPmain ();
DLLEXPORT int PRSmain ();
DLLEXPORT int STRmain ();
DLLEXPORT int TRAmain ();
DLLEXPORT int CPPmain ();
#endif /* __STDC__ */

#endif /* _COMPILERS_F_H_ */
#endif /* _WINDOWS */

