#ifndef _WIN_FCT_
#define _WIN_FCT_
#include <windows.h>
#include "fileaccess.h"
#define APPFILENAMEFILTER "HTML files (*.htm[l])\0*.htm*\0XML files (*.xml)\0*.xml\0MathML Files (*.mml)\0*.mml\0SVG files (*.svg)\0*.svg\0All files (*.*)\0*.*\0"
#define APPIMAGENAMEFILTER "Image files (*.gif)\0*.gif\0Image files (*.jpg)\0*.jpg\0Image files (*.png)\0*.png\0Image files (*.bmp)\0*.bmp\0All files (*.*)\0*.*\0"
#define APPALLFILESFILTER "All files (*.*)\0*.*\0"

extern BOOL       RegisterWin95 (CONST WNDCLASS* lpwc);
extern void       DropFile (Document, View, STRING);
extern HMENU      WIN_GetMenu (int);
extern void       WinErrorBox (ThotWindow, STRING);
extern HFONT      WinLoadFont (HDC, ptrfont);
extern int        WIN_InitSystemColors ();
extern void       WIN_GetDeviceContext (int);
extern void       WIN_ReleaseDeviceContext ();
extern BOOL       WIN_TtaInitDialogue (CHAR_T*);
extern void       WIN_TtaSetPulldownOff (int, ThotMenu, ThotWindow);
extern void       WIN_TtaSetPulldownOn (int, ThotMenu, ThotWindow);
extern void       WIN_TtaSetToggleMenu (int, int, ThotBool, ThotWindow);
extern void       WIN_ThotCallBack (ThotWindow, WPARAM, LPARAM);
extern void       WIN_CharTranslation (HWND hWnd, int frame, UINT msg, WPARAM wParam, LPARAM lParam, ThotBool isSpecial);
extern void       WIN_InitDialogueFonts (HDC, CHAR_T*);
extern void       WIN_HandleExpose (ThotWindow w, int frame, WPARAM wParam, LPARAM lParam);
extern void       APP_ButtonCallback (ThotButton, int, caddr_t);
extern void       CleanFrameCatList (int frame, int ref);
extern int        GetMainFrameNumber (ThotWindow);
extern LRESULT    ToolBarNotify (int, ThotWindow, WPARAM, LPARAM);
extern LRESULT CALLBACK TextZoneProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern int        GetFrameNumber (ThotWindow);
extern void       TtaHandleOneWindowEvent (MSG*);
extern void       CreateOpenImgDlgWindow (ThotWindow parent, STRING imgName, int doc_select, int dir_select, int doc_type);
extern void       WIN_ListSaveDirectory (int, STRING, STRING);
extern void       ThotCallback (int, int, STRING data);
extern void       WIN_TtaSwitchButton (Document, View, int, int, int, BOOL);
extern void       SwitchIconMath (Document, View, ThotBool);
extern void       AddMathButton (Document, View);
extern int        TtaGetMessageTable (CONST CHAR_T*, int msgNumber);
extern int        GetWindowNumber (Document, View view);
extern void       TtaError (int);
extern void       CreateMCHARDlgWindow (ThotWindow parent, STRING math_entity_name);
extern void       CreateMathDlgWindow (ThotWindow parent);
extern void       CreateTableDlgWindow (int num_cols, int num_rows, int t_border);
extern void       CreateMatrixDlgWindow (int num_cols, int num_rows);
extern ThotBool   TtaGetPrinterDC (ThotBool reuse, int *orientation, int *paper);
extern void       ReusePrinterDC (void);
extern void       WinInitPrinterColors (void);
extern void       CreateAltDlgWindow ();
extern void       CreateCSSDlgWindow (ThotWindow parent, int nb_item, STRING buffer, STRING title, CHAR_T* msg_text);
extern void       CreateTitleDlgWindow (ThotWindow parent, STRING title);
extern void       CreateHRefDlgWindow (ThotWindow parent, STRING docName, int doc_select, int dir_select, int doc_type);
extern void       CreateHelpDlgWindow (ThotWindow, STRING, STRING, STRING);
extern void       CreatePrintDlgWindow (ThotWindow parent, STRING ps_dir);
extern void       CreateAlign1DlgWindow (ThotWindow, int);
extern void       CreateAlign2DlgWindow (ThotWindow, int);
extern void       CreateAlign3DlgWindow (ThotWindow, int);
extern void       CreateSearchDlgWindow (ThotWindow parent);
extern void       CreateSaveAsDlgWindow (ThotWindow parent, STRING path_name);
extern void       CreateOpenDocDlgWindow (ThotWindow parent, STRING title, STRING docName, int doc_select, int dir_select, int doc_type);
extern void       CreateGraphicsDlgWindow (ThotWindow frame);
extern void       CreateSaveListDlgWindow (ThotWindow parent, int nb_item, STRING save_list);
extern void       CreateCloseDocDlgWindow (ThotWindow parent, STRING msg, ThotBool* save_befor, ThotBool* close_dont_save);
extern void       CreateLanguageDlgWindow (ThotWindow, STRING, STRING, int, STRING, STRING, int, STRING);
extern void       CreateCharacterDlgWindow (ThotWindow, int, int, int, int, int);
extern void       CreateAttributeDlgWindow (STRING, int, int);
extern void       CreateRuleDlgWindow (ThotWindow parent, int nb_class, STRING class_list);
extern void       CreateApplyClassDlgWindow (ThotWindow parent, int nb_class, STRING class_list);
extern void       CreateSpellCheckDlgWindow (ThotWindow, STRING, STRING, int, int, int, int, int, int, int);
extern void       CreateMathAttribDlgWindow (int);
extern void       CreateInitConfirmDlgWindow (ThotWindow, int, STRING, STRING);
extern void       CreateInitConfirm3LDlgWindow (ThotWindow, int, STRING, STRING, STRING, STRING, ThotBool withCancel);
extern void       CreateChangeFormatDlgWindow (int, int, int, int, int, int, int);
extern void       CreateGreekKeyboardDlgWindow (ThotWindow);
extern void       CreateAuthenticationDlgWindow (ThotWindow, const STRING, STRING);
extern void       CreateBackgroundImageDlgWindow (ThotWindow parent, STRING image_location);
extern void       CreateDocumentInfoDlgWindow (ThotWindow parent, const Document doc);
extern void       CreateMakeIDDlgWindow (ThotWindow parent);
extern void       TtaHandleOneEvent (ThotEvent*);
extern ThotWindow TtaGetThotWindow (int);
extern BinFile    TtaReadOpen (CONST CHAR_T*);
extern ThotWindow GetCurrentWindow ();
extern void       KbdCallbackHandler (ThotWidget, int, caddr_t);
extern void       CheckTtCmap ();

#endif /* _WIN_FCT_ */

