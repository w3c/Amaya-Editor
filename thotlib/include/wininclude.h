#ifndef _WIN_FCT_
#define _WIN_FCT_
#include <windows.h>
#include "fileaccess.h"
#define APPFILENAMEFILTER "HTML files (*.*htm[l])\0*.*htm*\0XML files (*.xml)\0*.xml\0MathML Files (*.mml)\0*.mml\0SVG files (*.svg)\0*.svg\0CSS files (*.css)\0Text files (*.txt)\0*.css\0Javascript files (*.js)\0*.js\0All files (*.*)\0*.*\0"
#define APPCSSNAMEFILTER "CSS files (*.css)\0*.css\0All files (*.*)\0*.*\0"
#define APPSVGNAMEFILTER "SVG files (*.svg)\0*.svg\0All files (*.*)\0*.*\0"
#define APPMATHNAMEFILTER "MathML files (*.mml)\0*.mml\0All files (*.*)\0*.*\0"
#define APPHTMLNAMEFILTER "HTML files (*.*htm[l])\0*.*htm*\0All files (*.*)\0*.*\0"
#define APPTXTNAMEFILTER "Text files (*.txt)\0*.txt\0All files (*.*)\0*.*\0"
#define APPIMAGENAMEFILTER "All files (*.*)\0*.*\0Image files (*.png)\0*.png\0Image files (*.jpg)\0*.jpg\0Image files (*.gif)\0*.gif\0"
#define APPLIBRARYNAMEFILTER "Library files (*.lhtml)\0*.lhtml\0All files (*.*)\0*.*\0"
#define APPJAVASCRIPTNAMEFILTER _T("Javascript files (*.js)\0*.js\0All files (*.*)\0*.*\0"
#define APPALLFILESFILTER "All files (*.*)\0*.*\0"

extern ThotBool RegisterWin95 (CONST WNDCLASS *lpwc);
extern void   DropFile (Document doc, View view, char *s);
extern HMENU  WIN_GetMenu (int ref);
extern void   WinErrorBox (ThotWindow w, char *s);
extern HFONT  WinLoadFont (HDC w, ThotFont f);
extern int    WIN_InitSystemColors ();
extern void   WIN_GetDeviceContext (int frame);
extern void   WIN_ReleaseDeviceContext ();
extern void   WIN_TtaSetPulldownOff (int, ThotMenu, ThotWindow);
extern void   WIN_TtaSetPulldownOn (int, ThotMenu, ThotWindow);
extern void   WIN_TtaSetToggleMenu (int, int, ThotBool, ThotWindow);
extern void   WIN_ThotCallBack (ThotWindow, WPARAM, LPARAM);
extern ThotBool WIN_CharTranslation (HWND hWnd, int frame, UINT msg, WPARAM wParam,
				     LPARAM lParam, ThotBool isSpecial);
extern void   WIN_HandleExpose (ThotWindow w, int frame, WPARAM wParam, LPARAM lParam);
extern void  WIN_SetDialogfont (ThotWindow wnd);
extern unsigned char GetSystemColorIndex (unsigned short red, unsigned short green,
					  unsigned short blue);
extern void   APP_ButtonCallback (ThotButton, int, caddr_t);
extern void   CleanFrameCatList (int frame, int ref);
extern int    GetMainFrameNumber (ThotWindow);
extern LRESULT  ToolBarNotify (int, ThotWindow, WPARAM, LPARAM);
extern LRESULT CALLBACK TextZoneProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern int    GetFrameNumber (ThotWindow);
extern void   TtaHandleOneWindowEvent (MSG*);
extern void   CreateOpenImgDlgWindow (ThotWindow parent, char *imgName, int doc_select,
				      int dir_select, int doc_type, ThotBool getAlt);
extern void   WIN_ListSaveDirectory (ThotWindow hwnDlg, char *s, char *url);
extern void   ThotCallback (int, int, char *data);
extern void   SwitchIconMath (Document, View, ThotBool);
extern void   AddMathButton (Document, View);
extern int    TtaGetMessageTable (CONST char *, int msgNumber);
extern int    GetWindowNumber (Document, View view);
extern void   TtaError (int);
extern void   CreateMCHARDlgWindow (ThotWindow parent, char *math_entity_name);
extern void   CreateMathDlgWindow (ThotWindow parent);
extern void   CreateTableDlgWindow (int num_cols, int num_rows, int t_border);
extern void   CreateMatrixDlgWindow (int num_cols, int num_rows);
extern ThotBool TtaGetPrinterDC (ThotBool reuse, int *orientation, int *paper);
extern void   ReusePrinterDC (void);
extern void   WinInitPrinterColors (void);
extern void   CreateAltDlgWindow ();
extern void   CreateXMLDlgWindow (ThotWindow parent, int nb_item, char *buffer,
				  ThotBool withText);
extern void   CreateCSSDlgWindow (ThotWindow parent, int nb_item, char *buffer,
				  char *title, char * msg_text);
extern void   CreateTitleDlgWindow (ThotWindow parent, char *title);
extern void   CreateHRefDlgWindow (ThotWindow parent, char *docName, int doc_select,
				   int dir_select, int doc_type);
extern void   CreateHelpDlgWindow (ThotWindow, char *, char *, char *);
extern void   CreatePrintDlgWindow (ThotWindow parent, char *ps_dir);
extern void   CreateAlign1DlgWindow (ThotWindow, int);
extern void   CreateAlign2DlgWindow (ThotWindow, int);
extern void   CreateAlign3DlgWindow (ThotWindow, int);
extern void   CreateSearchDlgWindow (ThotWindow parent);
extern void   CreateCharsetDlgWindow (ThotWindow parent);
extern void   CreateMimeTypeDlgWindow (ThotWindow parent, int nb_item,
					 char *mimetype_list);
extern void   MimeTypeDlgStatus (char *msg);
extern void   SaveAsDlgStatus (char *msg);
extern void   CreateSaveAsDlgWindow (ThotWindow parent, char *path_name);
extern void   CreateOpenDocDlgWindow (ThotWindow parent, char *title, char *url,
					  char *docName, int doc_select, int dir_select,
					  int doc_type);
extern void   CreateGraphicsDlgWindow (ThotWindow frame);
extern void   CreateSaveListDlgWindow (ThotWindow parent, int nb_item,
					 char *save_list);
extern void   CreateCloseDocDlgWindow (ThotWindow parent, char *msg);
extern void   CreateLanguageDlgWindow (ThotWindow parent, int nb_item,
					 char *lang_list, char *msg, int lang_value);
extern void   CreateCharacterDlgWindow (ThotWindow, int, int, int, int, int);
extern void   CreateAttributeDlgWindow (char *title, int curr_val, int nb_items,
					  char *item_list, ThotBool required);
extern void   CreateRuleDlgWindow (ThotWindow parent, int nb_class,
				   char *class_list);
extern void   CreateApplyClassDlgWindow (ThotWindow parent, int nb_class,
					   char *class_list);
extern void   CreateSpellCheckDlgWindow (ThotWindow, char *, char *, int, int, int,
					 int, int, int, int);
extern void   CreateMathAttribDlgWindow (int);
extern void   CreateInitConfirmDlgWindow (ThotWindow parent, char *extrabutton,
					  char *confirmbutton, char *msg);
extern void   CreateInitConfirm3LDlgWindow (ThotWindow parent, char *title,
						char *msg, char *msg2, char *msg3,
						ThotBool withCancel);
extern void CreateChangeFormatDlgWindow (int num_zone_recess, int num_zone_line_spacing,
					 int align_num, int indent_value, int indent_num,
					 int old_lineSp, int line_spacingNum);
extern void   CreateGreekKeyboardDlgWindow (ThotWindow parent);
extern void   CreateAuthenticationDlgWindow (ThotWindow parent, const char *realm,
						 char *server);
extern void   CreateBackgroundImageDlgWindow (ThotWindow parent, char *image_location);
extern void   CreateDocumentInfoDlgWindow (ThotWindow parent, const Document doc);
extern void   CreateMakeIDDlgWindow (ThotWindow parent);
extern ThotWindow TtaGetThotWindow (int f);
extern BinFile  TtaReadOpen (CONST char *f);
extern ThotWindow GetCurrentWindow ();
extern void   KbdCallbackHandler (ThotWidget w, unsigned int param, caddr_t call_d);
extern void   CheckTtCmap ();

/* Override some functions for unicode support */
#ifndef _WIN9X
#undef  GetMessage
#define GetMessage  GetMessageW
#endif /* _WIN9X */

/* The following macros override some other windows functions */
/* Uncomment it to enable unicod but take care of side effect */
/* Look into WINUSER.H for other windows unicode functions */
/*
#undef  DispatchMessage
#define DispatchMessage					DispatchMessageW
#undef  PeekMessage
#define PeekMessage						PeekMessageW
#undef  SendMessage
#define SendMessage						SendMessageW
#undef  PostMessage
#define PostMessage						PostMessageW
#undef  DefWindowProc
#define DefWindowProc					DefWindowProcW
#undef  CallWindowProc
#define CallWindowProc					CallWindowProcW
#undef  CreateDialogParam
#define CreateDialogParam				CreateDialogParamW
#undef  CreateDialogIndirectParam
#define CreateDialogIndirectParam		CreateDialogIndirectParamW
#undef  CreateDialog
#define CreateDialog					CreateDialogW
#undef  CreateDialogIndirect
#define CreateDialogIndirect			CreateDialogIndirectW
#undef  DialogBoxParam
#define DialogBoxParam					DialogBoxParamW
#undef  DialogBoxIndirectParam
#define DialogBoxIndirectParam			DialogBoxIndirectParamW
#undef  DialogBox
#define DialogBox						DialogBoxW
#undef  DialogBoxIndirect
#define DialogBoxIndirect				DialogBoxIndirectW
#undef  CreateWindowEx
#define CreateWindowEx					CreateWindowExW
#undef  CreateWindow
#define CreateWindow					CreateWindowW
#undef  DrawText
#define DrawText						DrawTextW
#undef  MessageBox
#define MessageBox						MessageBoxW
*/

#endif /* _WIN_FCT_ */

