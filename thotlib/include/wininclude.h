#ifndef _WIN_FCT_
#define _WIN_FCT_
#include <windows.h>

#include "fileaccess.h"



#ifdef __STDC__
extern void       DropFile (Document, View, STRING);
extern HMENU      WIN_GetMenu (int);
extern void       WinErrorBox (HWND, STRING);
extern void       WinLoadGC (HDC, int, int);
extern HFONT      WinLoadFont (HDC, ptrfont);
extern int        WIN_InitSystemColors ();
extern void       WIN_GetDeviceContext (int);
extern void       WIN_ReleaseDeviceContext ();
extern BOOL       WIN_TtaInitDialogue (CHAR_T*);
extern void       WIN_TtaSetPulldownOff (int, ThotMenu, HWND);
extern void       WIN_TtaSetPulldownOn (int, ThotMenu, HWND);
extern void       WIN_TtaSetToggleMenu (int, int, ThotBool, HWND);
extern void       WIN_APP_TextCallback (HWND, int);
extern void       WIN_ThotCallBack (HWND, WPARAM, LPARAM);
extern void       WIN_CharTranslation (HWND, int, UINT, WPARAM, LPARAM);
extern void       WIN_InitDialogueFonts (HDC, CHAR_T*);
extern void       APP_ButtonCallback (ThotButton, int, caddr_t);
extern void       CleanFrameCatList (int);
extern int        GetMainFrameNumber (ThotWindow);
extern LRESULT    ToolBarNotify (int, HWND, WPARAM, LPARAM);
extern int        GetFrameNumber (ThotWindow);
extern void       TtaHandleOneWindowEvent (MSG*);
extern void       CreateOpenImgDlgWindow (HWND, STRING, int, int, int, int, int, int);
extern void       CreateTableDlgWindow (int, int, int, int, int, int, int, int);
extern void       WIN_ListSaveDirectory (int, STRING, STRING);
extern void       ThotCallback (int, int, STRING data);
extern void       WIN_TtaSwitchButton (Document, View, int, int, int, BOOL);
extern void       SwitchIconMath (Document, View, ThotBool);
extern void       AddMathButton (Document, View);
extern int        TtaGetMessageTable (CONST CHAR_T*, int msgNumber);
extern int        GetWindowNumber (Document, View view);
extern void       TtaError (int);
extern void       CreateMatrixDlgWindow (int, int, int, int, int, int);
extern HDC        PASCAL GetPrinterDC (void);
extern void       WinInitPrinterColors (void);
extern void       CreateAltDlgWindow (int, int, int, int);
extern void       CreateCSSDlgWindow (HWND, int, int, int, STRING, int, STRING, STRING);
extern void       CreateLinkDlgWindow (HWND, STRING, int, int, int);
extern void       CreateHelpDlgWindow (HWND, STRING, STRING, STRING);
extern void       CreateMathDlgWindow (HWND, int, HWND);
extern void       CreatePrintDlgWindow (HWND, STRING, int, int, int, int, int, int);
extern void       CreateAlign1DlgWindow (HWND, int);
extern void       CreateAlign2DlgWindow (HWND, int);
extern void       CreateAlign3DlgWindow (HWND, int);
extern void       CreateSearchDlgWindow (HWND, BOOL);
extern void       CreateSaveAsDlgWindow (HWND, STRING, int, int, int, int, int, int);
extern void       CreateOpenDocDlgWindow (HWND, STRING, STRING, STRING, int, int, int, int, int, int);
extern void       CreateGraphicsDlgWindow (int, int, int, HWND);
extern void       CreateSaveListDlgWindow (HWND, int, STRING, int, int);
extern void       CreateCloseDocDlgWindow (HWND, STRING, STRING, int, BOOL*, BOOL*);
extern void       CreateLanguageDlgWindow (HWND, STRING, STRING, int, STRING, STRING, int, STRING);
extern void       CreateCharacterDlgWindow (HWND, int, int, int, int, int);
extern void       CreateAttributeDlgWindow (STRING, int, int);
extern void       CreateCreateRuleDlgWindow (HWND, int, int, int, int, STRING);
extern void       CreateApplyClassDlgWindow (HWND, int, int, int, int, STRING);
extern void       CreateSpellCheckDlgWindow (HWND, STRING, STRING, int, int, int, int, int, int, int);
extern void       CreateMathAttribDlgWindow (int);
extern void       CreateInitConfirmDlgWindow (HWND, int, STRING, STRING);
extern void       CreateChangeFormatDlgWindow (int, int, int, int, int, int, int, int);
extern void       CreateGreekKeyboardDlgWindow (HWND);
extern void       CreateAuthenticationDlgWindow (HWND, STRING, STRING);
extern void       CreateBackgroundImageDlgWindow (HWND, int, int, int, int, int, int, int, STRING);
extern void       TtaHandleOneEvent (ThotEvent*);
extern ThotWindow TtaGetThotWindow (int);
extern BinFile    TtaReadOpen (CONST CHAR_T*);
extern HWND       GetCurrentWindow ();
extern void       WIN_DisplayWords (void);
extern void       KbdCallbackHandler (ThotWidget, int, caddr_t);
extern void       CheckTtCmap ();

#else  /* __STDC__ */
extern void       DropFile ();
extern HMENU      WIN_GetMenu ();
extern void       WinErrorBox ();
extern void       WinLoadGC ();
extern HFONT      WinLoadFont ();
extern int        WIN_InitSystemColors ();
extern void       WIN_GetDeviceContext ();
extern void       WIN_ReleaseDeviceContext ();
extern BOOL       WIN_TtaInitDialogue ();
extern void       WIN_TtaSetPulldownOff ();
extern void       WIN_TtaSetPulldownOn ();
extern void       WIN_TtaSetToggleMenu ();
extern void       WIN_APP_TextCallback ();
extern void       WIN_ThotCallBack ();
extern void       WIN_CharTranslation ();
extern void       WIN_InitDialogueFonts ();
extern void       APP_ButtonCallback ();
extern void       CleanFrameCatList ();
extern int        GetMainFrameNumber ();
extern LRESULT    ToolBarNotify ();
extern int        GetFrameNumber ();
extern void       TtaHandleOneWindowEvent ();
extern void       CreateOpenImgDlgWindow ();
extern void       CreateTableDlgWindow ();
extern void       WIN_ListSaveDirectory ();
extern void       ThotCallback ();
extern void       WIN_TtaSwitchButton ();
extern void       SwitchIconMath ();
extern void       AddMathButton ();
extern int        TtaGetMessageTable ();
extern int        GetWindowNumber (Document, View);
extern void       TtaError ();
extern void       CreateMatrixDlgWindow ();
extern HDC        PASCAL GetPrinterDC ();
extern void       WinInitPrinterColors ();
extern void       CreateAltDlgWindow ();
extern void       CreateCSSDlgWindow ();
extern void       CreateLinkDlgWindow ();
extern void       CreateHelpDlgWindow ();
extern void       CreatePrintDlgWindow ();
extern void       CreateAlign1DlgWindow ();
extern void       CreateAlign2DlgWindow ();
extern void       CreateAlign3DlgWindow ();
extern void       CreateSearchDlgWindow ();
extern void       CreateSaveAsDlgWindow ();
extern void       CreateOpenDocDlgWindow ();
extern void       CreateGraphicsDlgWindow ();
extern void       CreateSaveListDlgWindow ();
extern void       CreateCloseDocDlgWindow ();
extern void       CreateLanguageDlgWindow ();
extern void       CreateCharacterDlgWindow ();
extern void       CreateAttributeDlgWindow ();
extern void       CreateCreateRuleDlgWindow ();
extern void       CreateApplyClassDlgWindow ();
extern void       CreateSpellCheckDlgWindow ();
extern void       CreateMathAttribDlgWindow ();
extern void       CreateInitConfirmDlgWindow ();
extern void       CreateChangeFormatDlgWindow ();
extern void       CreateGreekKeyboardDlgWindow ();
extern void       CreateAuthenticationDlgWindow ();
extern void       CreateBackgroundImageDlgWindow ();
extern void       TtaHandleOneEvent ();
extern ThotWindow TtaGetThotWindow ();
extern BinFile    TtaReadOpen ();
extern HWND       GetCurrentWindow ();
extern void       WIN_DisplayWords ();
extern void       KbdCallbackHandler ();
extern void       CheckTtCmap ();
#endif /* __STDC__ */

#endif /* _WIN_FCT_ */

