#ifdef _WINDOWS

#ifndef _COMPILERS_F_H_
#define _COMPILERS_F_H_

#define DLLEXPORT __declspec(dllexport)
DLLEXPORT int APPmain (HWND hwnd, HWND statusBar, int argc, char **argv, int *Y);
DLLEXPORT int PRSmain (HWND hwnd, HWND statusBar, int argc, char **argv, int *Y);
DLLEXPORT int STRmain (HWND hwnd, HWND statusBar, int argc, char **argv, int *Y);
DLLEXPORT int TRAmain (HWND hwnd, HWND statusBar, int argc, char **argv, int *Y);
DLLEXPORT int CPPmain (HWND hwnd, int argc, char **argv, int *Y);

#endif /* _COMPILERS_F_H_ */
#endif /* _WINDOWS */

