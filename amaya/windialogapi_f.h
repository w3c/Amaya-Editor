/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * WIndows Dialogue API routines for Amaya
 *
 * Author: R. Guetari (W3C/INRIA)
 *
 */
#ifdef _WINDOWS
#include <windows.h>
#include "resource.h"

extern HDC PASCAL GetPrinterDC (void);
extern void WinInitPrinterColors (void);
extern void CreateLinkDlgWindow (HWND, char*, int, int, int);
extern void CreateHelpDlgWindow (HWND, char*, char*, char*);
extern void CreateMathDlgWindow (HWND, int, HWND);
extern void CreatePrintDlgWindow (HWND, char*, int, int, int, int, int, int);
extern void CreateAlign1DlgWindow (HWND, int);
extern void CreateAlign2DlgWindow (HWND, int);
extern void CreateSearchDlgWindow (HWND);
extern void CreateSaveAsDlgWindow (HWND, char*, int, int, int, int, int, int);
extern void CreateOpenDocDlgWindow (HWND, char*, int, int, int, int, int);
extern void CreateSaveListDlgWindow (HWND, int, char*, int, int);
extern void CreateCloseDocDlgWindow (HWND, char*, char*, BOOL*, BOOL*);
extern void CreateLanguageDlgWindow (HWND, char*, char*, int, char*, char*, int, int, int, char*);
extern void CreateCharacterDlgWindow (HWND, int, int, int, int);
extern void CreateCreateRuleDlgWindow (HWND, int, int, int, int, char*);
extern void CreateApplyClassDlgWindow (HWND, int, int, int, int, char*);
extern void CreateSpellCheckDlgWindow (HWND, char*, char*, int, int, int, int, int, int, int);
extern void CreateInitConfirmDlgWindow (HWND, int, char*, char*);
extern void CreateChangeFormatDlgWindow (HWND);
extern void CreateGreekKeyboardDlgWindow (HWND);
extern void CreateAuthenticationDlgWindow (HWND);
extern void CreateBackgroundImageDlgWindow (HWND, int, int, int, int, int, int, int, char*);
#endif /* _WINDOWS */