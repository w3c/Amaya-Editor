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
extern void CreateLinkDlgWindow (HWND, STRING, int, int, int);
extern void CreateHelpDlgWindow (HWND, STRING, STRING, STRING);
extern void CreateMathDlgWindow (HWND, int, HWND);
extern void CreatePrintDlgWindow (HWND, STRING, int, int, int, int, int, int);
extern void CreateAlign1DlgWindow (HWND, int);
extern void CreateAlign2DlgWindow (HWND, int);
extern void CreateSearchDlgWindow (HWND);
extern void CreateSaveAsDlgWindow (HWND, STRING, STRING, int, int, int, int, int, int);
extern void CreateOpenDocDlgWindow (HWND, STRING, STRING, STRING, int, int, int, int, int, int);
extern void CreateSaveListDlgWindow (HWND, int, STRING, int, int);
extern void CreateCloseDocDlgWindow (HWND, STRING, STRING, BOOL*, BOOL*);
extern void CreateLanguageDlgWindow (HWND, STRING, STRING, int, STRING, STRING, int, int, int, STRING);
extern void CreateCharacterDlgWindow (HWND, int, int, int, int, int);
extern void CreateCreateRuleDlgWindow (HWND, int, int, int, int, STRING);
extern void CreateApplyClassDlgWindow (HWND, int, int, int, int, STRING);
extern void CreateSpellCheckDlgWindow (HWND, STRING, STRING, int, int, int, int, int, int, int);
extern void CreateInitConfirmDlgWindow (HWND, int, STRING, STRING);
extern void CreateChangeFormatDlgWindow (HWND);
extern void CreateGreekKeyboardDlgWindow (HWND);
extern void CreateAuthenticationDlgWindow (HWND, STRING, STRING);
extern void CreateBackgroundImageDlgWindow (HWND, int, int, int, int, int, int, int, STRING);
#endif /* _WINDOWS */
