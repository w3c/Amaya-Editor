#ifndef _WIN_FCT_
#define _WIN_FCT_
#include <windows.h>
#include "fileaccess.h"
#define APPFILENAMEFILTER "HTML files (*.htm[l])\0*.htm*\0XML files (*.xml)\0*.xml\0MathML Files (*.mml)\0*.mml\0SVG files (*.svg)\0*.svg\0All files (*.*)\0*.*\0"
#define APPIMAGENAMEFILTER "Image files (*.gif)\0*.gif\0Image files (*.jpg)\0*.jpg\0Image files (*.png)\0*.png\0Image files (*.bmp)\0*.bmp\0All files (*.*)\0*.*\0"
#define APPALLFILESFILTER "All files (*.*)\0*.*\0"

extern ThotBool   RegisterWin95 (CONST WNDCLASS *lpwc);
extern void       DropFile (Document doc, View view, char *s);
extern HMENU      WIN_GetMenu (int ref);
extern void       WinErrorBox (ThotWindow w, char *s);
extern HFONT      WinLoadFont (HDC w, ptrfont f);
extern int        WIN_InitSystemColors ();
extern void       WIN_GetDeviceContext (int frame);
extern void       WIN_ReleaseDeviceContext ();
extern ThotBool   WIN_TtaInitDialogue (char *s);
extern void       WIN_TtaSetPulldownOff (int, ThotMenu, ThotWindow);
extern void       WIN_TtaSetPulldownOn (int, ThotMenu, ThotWindow);
extern void       WIN_TtaSetToggleMenu (int, int, ThotBool, ThotWindow);
extern void       WIN_ThotCallBack (ThotWindow, WPARAM, LPARAM);
extern int        WIN_TtaHandleMultiKeyEvent (UINT msg, WPARAM wParam, LPARAM lParam, int *k);
extern void       WIN_CharTranslation (HWND hWnd, int frame, UINT msg, WPARAM wParam, LPARAM lParam, ThotBool isSpecial);
extern void       WIN_InitDialogueFonts (HDC, char *);
extern void       WIN_HandleExpose (ThotWindow w, int frame, WPARAM wParam, LPARAM lParam);
extern unsigned char GetSystemColorIndex (unsigned short red, unsigned short green, unsigned short blue);
extern void       APP_ButtonCallback (ThotButton, int, caddr_t);
extern void       CleanFrameCatList (int frame, int ref);
extern int        GetMainFrameNumber (ThotWindow);
extern LRESULT    ToolBarNotify (int, ThotWindow, WPARAM, LPARAM);
extern LRESULT CALLBACK TextZoneProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern int        GetFrameNumber (ThotWindow);
extern void       TtaHandleOneWindowEvent (MSG*);
extern void       CreateOpenImgDlgWindow (ThotWindow parent, char *imgName, int doc_select, int dir_select, int doc_type);
extern void       WIN_ListSaveDirectory (int ref, char *s, char *url);
extern void       ThotCallback (int, int, char *data);
extern void       WIN_TtaSwitchButton (Document, View, int, int, int, BOOL);
extern void       SwitchIconMath (Document, View, ThotBool);
extern void       AddMathButton (Document, View);
extern int        TtaGetMessageTable (CONST char *, int msgNumber);
extern int        GetWindowNumber (Document, View view);
extern void       TtaError (int);
extern void       CreateMCHARDlgWindow (ThotWindow parent, char *math_entity_name);
extern void       CreateMathDlgWindow (ThotWindow parent);
extern void       CreateTableDlgWindow (int num_cols, int num_rows, int t_border);
extern void       CreateMatrixDlgWindow (int num_cols, int num_rows);
extern ThotBool   TtaGetPrinterDC (ThotBool reuse, int *orientation, int *paper);
extern void       ReusePrinterDC (void);
extern void       WinInitPrinterColors (void);
extern void       CreateAltDlgWindow ();
extern void       CreateCSSDlgWindow (ThotWindow parent, int nb_item, char *buffer, char *title, char * msg_text);
extern void       CreateTitleDlgWindow (ThotWindow parent, char *title);
extern void       CreateHRefDlgWindow (ThotWindow parent, char *docName, int doc_select, int dir_select, int doc_type);
extern void       CreateHelpDlgWindow (ThotWindow, char *, char *, char *);
extern void       CreatePrintDlgWindow (ThotWindow parent, char *ps_dir);
extern void       CreateAlign1DlgWindow (ThotWindow, int);
extern void       CreateAlign2DlgWindow (ThotWindow, int);
extern void       CreateAlign3DlgWindow (ThotWindow, int);
extern void       CreateSearchDlgWindow (ThotWindow parent);
extern void       CreateSaveAsDlgWindow (ThotWindow parent, char *path_name);
extern void       CreateOpenDocDlgWindow (ThotWindow parent, char *title, char *docName, int doc_select, int dir_select, int doc_type);
extern void       CreateGraphicsDlgWindow (ThotWindow frame);
extern void       CreateSaveListDlgWindow (ThotWindow parent, int nb_item, char *save_list);
extern void       CreateCloseDocDlgWindow (ThotWindow parent, char *msg, ThotBool* save_befor, ThotBool* close_dont_save);
extern void       CreateLanguageDlgWindow (ThotWindow, char *, char *, int, char *, char *, int, char *);
extern void       CreateCharacterDlgWindow (ThotWindow, int, int, int, int, int);
extern void       CreateAttributeDlgWindow (char *, int, int);
extern void       CreateRuleDlgWindow (ThotWindow parent, int nb_class, char *class_list);
extern void       CreateApplyClassDlgWindow (ThotWindow parent, int nb_class, char *class_list);
extern void       CreateSpellCheckDlgWindow (ThotWindow, char *, char *, int, int, int, int, int, int, int);
extern void       CreateMathAttribDlgWindow (int);
extern void       CreateInitConfirmDlgWindow (ThotWindow parent, int ref, char *title, char *msg);
extern void       CreateInitConfirm3LDlgWindow (ThotWindow parent, int ref, char *title, char *msg, char *msg2, char *msg3, ThotBool withCancel);
extern void       CreateChangeFormatDlgWindow (int num_zone_recess, int num_zone_line_spacing, int align_num, int indent_value, int indent_num, int old_lineSp, int line_spacingNum);
extern void       CreateGreekKeyboardDlgWindow (ThotWindow parent);
extern void       CreateAuthenticationDlgWindow (ThotWindow parent, const char *realm, char *server);
extern void       CreateBackgroundImageDlgWindow (ThotWindow parent, char *image_location);
extern void       CreateDocumentInfoDlgWindow (ThotWindow parent, const Document doc);
extern void       CreateMakeIDDlgWindow (ThotWindow parent);
extern void       TtaHandleOneEvent (ThotEvent *ev);
extern ThotWindow TtaGetThotWindow (int f);
extern BinFile    TtaReadOpen (CONST char *f);
extern ThotWindow GetCurrentWindow ();
extern void       KbdCallbackHandler (ThotWidget, int, caddr_t);
extern void       CheckTtCmap ();

#endif /* _WIN_FCT_ */

