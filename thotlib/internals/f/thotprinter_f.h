#ifdef _WINGUI
#ifndef _THOTPRINTER_F_H
#define _THOTPRINTER_F_H

#define DLLEXPORT __declspec(dllexport)

DLLEXPORT void PrintDoc (HWND hWnd, int argc, char **argv, HDC PrinterDC,
			 ThotBool isTrueColors, int depth, char *tmpDocName,
			 char *tmpDir, HINSTANCE hInst, ThotBool buttonCmd);
#endif /* _THOTPRINTER_F_H */
#endif /* _WINGUI */