/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * WIndows Dialogue API routines for Amaya
 *
 * Author: I. Vatton (W3C/INRIA)
 *
 */
#ifdef _WINGUI

#ifdef __STDC__
extern HDC  PASCAL GetPrinterDC (void);
extern void WinInitPrinterColors (void);
extern void CreateCSSDlgWindow (HWND, int, int, int, char*, int, char*, char*, CHAR_T*);
extern void CreateLinkDlgWindow (HWND, char*, int, int, int, CHAR_T*, CHAR_T*);
extern void CreateHelpDlgWindow (HWND, char*, char*, char*);
extern void CreateMathDlgWindow (HWND, int, HWND, CHAR_T*);
extern void CreatePrintDlgWindow (HWND, char*, int, int, int, int, int, int);
extern void CreateAlign1DlgWindow (HWND, int);
extern void CreateAlign2DlgWindow (HWND, int);
extern void CreateAlign3DlgWindow (HWND, int);
extern void CreateSearchDlgWindow (HWND, BOOL, CHAR_T*);
extern void CreateSaveAsDlgWindow (HWND, char*, int, int, int, int, int, int);
extern void CreateOpenDocDlgWindow (HWND, char*, char*, char*, int, int, int, int, int, int);
extern void CreateGraphicsDlgWindow (int, int, int, HWND);
extern void CreateSaveListDlgWindow (HWND, int, char*, int, int);
extern void CreateCloseDocDlgWindow (HWND, char*, char*, int, BOOL*, BOOL*);
extern void CreateLanguageDlgWindow (HWND, char*, char*, int, char*, char*, int, int, int, char*);
extern void CreateCharacterDlgWindow (HWND, int, int, int, int);
extern void CreateAttributeDlgWindow (char*, int, int);
extern void CreateCreateRuleDlgWindow (HWND, int, int, int, int, char*);
extern void CreateApplyClassDlgWindow (HWND, int, int, int, int, char*);
extern void CreateSpellCheckDlgWindow (HWND, char*, char*, int, int, int, int, int, int, int);
extern void CreateMathAttribDlgWindow (int);
extern void CreateInitConfirmDlgWindow (HWND, int, char*, char*);
extern void CreateChangeFormatDlgWindow (HWND);
extern void CreateGreekKeyboardDlgWindow (HWND);
extern void CreateAuthenticationDlgWindow (HWND);
extern void CreateBackgroundImageDlgWindow (HWND, int, int, int, int, int, int, int, char*);
#else  /* !__STDC__ */
extern HDC  PASCAL GetPrinterDC ();
extern void WinInitPrinterColors ();
extern void CreateCSSDlgWindow ();
extern void CreateLinkDlgWindow ();
extern void CreateHelpDlgWindow ();
extern void CreateMathDlgWindow ();
extern void CreatePrintDlgWindow ();
extern void CreateAlign1DlgWindow ();
extern void CreateAlign2DlgWindow ();
extern void CreateAlign3DlgWindow ();
extern void CreateSearchDlgWindow ();
extern void CreateSaveAsDlgWindow ();
extern void CreateOpenDocDlgWindow ();
extern void CreateGraphicsDlgWindow ();
extern void CreateSaveListDlgWindow ();
extern void CreateCloseDocDlgWindow ();
extern void CreateLanguageDlgWindow ();
extern void CreateCharacterDlgWindow ();
extern void CreateAttributeDlgWindow ();
extern void CreateCreateRuleDlgWindow ();
extern void CreateApplyClassDlgWindow ();
extern void CreateSpellCheckDlgWindow ();
extern void CreateMathAttribDlgWindow ();
extern void CreateInitConfirmDlgWindow ();
extern void CreateChangeFormatDlgWindow ();
extern void CreateGreekKeyboardDlgWindow ();
extern void CreateAuthenticationDlgWindow ();
extern void CreateBackgroundImageDlgWindow ();
#endif /* __STDC__ */

#endif /* _WINGUI */
