#ifdef _WINDOWS
#ifndef _THOTPRINTER_F_H
#define _THOTPRINTER_F_H

#define DLLEXPORT __declspec(dllexport)

#ifdef __STDC__
DLLEXPORT void PrintDoc (HWND, int, STRING*, HDC, BOOL, int, STRING, STRING, HINSTANCE, BOOL);
#else  /* !__STDC__ */
DLLEXPORT void PrintDoc ();
#endif /* __STDC__ */

#endif /* _THOTPRINTER_F_H */
#endif /* _WINDOWS */