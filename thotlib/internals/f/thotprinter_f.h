#ifdef _WINDOWS
#ifndef _THOTPRINTER_F_H
#define _THOTPRINTER_F_H

#define DLLEXPORT __declspec(dllexport)

#ifdef __STDC__
DLLEXPORT void PrintDoc (int, char**, HDC, BOOL, int, char*, char*, HINSTANCE);
#else  /* !__STDC__ */
DLLEXPORT void PrintDoc ();
#endif /* __STDC__ */

#endif /* _THOTPRINTER_F_H */
#endif /* _WINDOWS */