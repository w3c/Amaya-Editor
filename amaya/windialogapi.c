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

#ifdef THOT_EXPORT
#      undef THOT_EXPORT
#endif /* THOT_EXPORT */

#define THOT_EXPORT extern
#include "amaya.h"
#include "constmenu.h"

#ifdef  APPFILENAMEFILTER
#       undef  APPFILENAMEFILTER
#endif  /* APPFILENAMEFILTER */

#ifndef MAX_PROPOSAL_CHKR
#       define MAX_PROPOSAL_CHKR 10
#endif /* MAX_PROPOSAL_CHKR */

#ifndef MAX_WORD_LEN
#       define MAX_WORD_LEN 30
#endif /* MAX_WORD_LEN */

#define APPFILENAMEFILTER   "HTML Files (*.html)\0*.html\0HTML Files (*.htm)\0*.htm\0Image files (*.gif)\0*.gif\0Image files (*.jpg)\0*.jpg\0Image files (*.png)\0*.png\0Image files (*.bmp)\0*.bmp\0All files (*.*)\0*.*\0"
#define MAX_BUFF 4096
#define IDC_WORDBUTTON    20000
#define IDC_EDITRULE      20001
#define IDC_LANGEDIT      20002
#define ICD_SPELLWORDEDIT 20003

#define MenuMaths          1

extern HINSTANCE hInstance;
extern char*     AttrHREFvalue;
extern char      ChkrCorrection[MAX_PROPOSAL_CHKR+1][MAX_WORD_LEN];

static char   urlToOpen [256];
static char   message [300];
static char   message2 [300];
static char   wndTitle [100];
static char   currentLabel [100];
static char   currentRejectedchars [100];
static char   currentPathName [100];

static int          currentDoc ;
static int          currentView ;
static int          currentRef;
static int          currentParentRef;
static int          SpellingBase; 
static int          ChkrSelectProp; 
static int          ChkrMenuOR; 
static int          ChkrFormCorrect;
static int          ChkrMenuIgnore;
static int          ChkrCaptureNC;
static int          ChkrSpecial;
static int          nbClass;
static int          nbItem;
static int          classForm;
static int          classSelect;
static int          baseDlg;
static int          saveForm;
static int          dirSave;
static int          nameSave;
static int          imgSave;
static int          toggleSave;
static int          confirmSave;
static int          attrHRefForm;
static int          attrHRefTxt;
static char*        classList;
static char*        langList;
static char*        saveList;
static HDC          hDC;
static HDC          hMemDC;
static HFONT        hFont;
static HFONT        hOldFont;
static BOOL	        saveBeforeClose ;
static BOOL         closeDontSave ;
static OPENFILENAME OpenFileName;
static TCHAR        szFilter[] = APPFILENAMEFILTER;
static TCHAR        szFileName[256];
static HWND         currentFrame;
static HWND         currentDlg;


HWND wordButton;
HWND hwnListWords;
HWND hwndCurrentWord;
HWND hwndLanguage;
char currentWord [MAX_WORD_LEN];

#ifdef __STDC__
LRESULT CALLBACK LinkDlgProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK HelpDlgProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK MathDlgProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK PrintDlgProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK Align1DlgProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK Align2DlgProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK SearchDlgProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK SaveAsDlgProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK OpenDocDlgProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK SaveListDlgProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK CloseDocDlgProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK LanguageDlgProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK CharacterDlgProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK CreateRuleDlgProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ApplyClassDlgProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK SpellCheckDlgProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK InitConfirmDlgProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ChangeFormatDlgProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK GreekKeyboardDlgProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK AuthenticationDlgProc (HWND, UINT, WPARAM, LPARAM);
#else  /* !__STDC__ */
LRESULT CALLBACK LinkDlgProc ();
LRESULT CALLBACK HelpDlgProc ();
LRESULT CALLBACK MathDlgProc ();
LRESULT CALLBACK PrintDlgProc ();
LRESULT CALLBACK Align1DlgProc ();
LRESULT CALLBACK Align2DlgProc ();
LRESULT CALLBACK SearchDlgProc ();
LRESULT CALLBACK SaveAsDlgProc ();
LRESULT CALLBACK OpenDocDlgProc ();
LRESULT CALLBACK SaveListDlgProc ();
LRESULT CALLBACK CloseDocDlgProc ();
LRESULT CALLBACK LanguageDlgProc ();
LRESULT CALLBACK CharacterDlgProc ();
LRESULT CALLBACK CreateRuleDlgProc ();
LRESULT CALLBACK ApplyClassDlgProc ();
LRESULT CALLBACK SpellCheckDlgProc ();
LRESULT CALLBACK InitConfirmDlgProc ();
LRESULT CALLBACK ChangeFormatDlgProc ();
LRESULT CALLBACK GreekKeyboardDlgProc ();
LRESULT CALLBACK AuthenticationDlgProc ();
#endif /* __STDC__ */

/*-----------------------------------------------------------------------
 CreateLinkDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void CreateLinkDlgWindow (HWND parent, char* attrHref, int base_dlg, int attr_HREFForm, int attr_HREFText)
#else  /* !__STDC__ */
void CreateLinkDlgWindow (parent, attrHref, base_dlg, attr_HREFForm, attr_HREFText)
HWND      parent;
char*     attrHref;
int       base_dlg; 
int       attr_HREFForm; 
int       attr_HREFText;
#endif /* __STDC__ */
{  
    baseDlg      = base_dlg;
    attrHRefForm = attr_HREFForm;
    attrHRefTxt  = attr_HREFText;
	strcpy (urlToOpen, attrHref);

	DialogBox (hInstance, MAKEINTRESOURCE (LINKDIALOG), parent, (DLGPROC) LinkDlgProc);
}

/*-----------------------------------------------------------------------
 CreateLinkDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void CreateHelpDlgWindow (HWND parent, char* localname, char* msg1, char* msg2)
#else  /* !__STDC__ */
void CreateHelpDlgWindow (parent, localname, msg1, msg2)
HWND      parent;
char*     localname;
char*     msg1;
char*     msg2;
#endif /* __STDC__ */
{  
    sprintf (currentPathName, localname);
    sprintf (message, msg1);
	sprintf (message2, msg2);
	DialogBox (hInstance, MAKEINTRESOURCE (HELPDIALOG), parent, (DLGPROC) HelpDlgProc);
}

/*-----------------------------------------------------------------------
 CreateLinkDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void CreateMathDlgWindow (HWND parent, int mathRef, HWND frame)
#else  /* !__STDC__ */
void CreateMathDlgWindow (parent, mathRef, frame)
HWND      parent;
int       mathRef;
HWND      frame;
#endif /* __STDC__ */
{  
	baseDlg = mathRef;
	currentFrame = frame	;
	DialogBox (hInstance, MAKEINTRESOURCE (MATHDIALOG), NULL, (DLGPROC) MathDlgProc);
}

/*-----------------------------------------------------------------------
 CreatePrintDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void CreatePrintDlgWindow (HWND parent)
#else  /* !__STDC__ */
void CreatePrintDlgWindow (parent)
HWND      parent;
#endif /* __STDC__ */
{  
	DialogBox (hInstance, MAKEINTRESOURCE (PRINTDIALOG), parent, (DLGPROC) PrintDlgProc);
}

/*-----------------------------------------------------------------------
 CreateAlign1DlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void CreateAlign1DlgWindow (HWND parent)
#else  /* !__STDC__ */
void CreateAlign1DlgWindow (parent)
HWND      parent;
#endif /* __STDC__ */
{  
	DialogBox (hInstance, MAKEINTRESOURCE (ALIGN1DIALOG), parent, (DLGPROC) Align1DlgProc);
}

/*-----------------------------------------------------------------------
 CreateAlign2DlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void CreateAlign2DlgWindow (HWND parent)
#else  /* !__STDC__ */
void CreateAlign2DlgWindow (parent)
HWND      parent;
#endif /* __STDC__ */
{  
	DialogBox (hInstance, MAKEINTRESOURCE (ALIGN2DIALOG), parent, (DLGPROC) Align2DlgProc);
}

/*-----------------------------------------------------------------------
 CreateSearchDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void CreateSearchDlgWindow (HWND parent)
#else  /* !__STDC__ */
void CreateSearchDlgWindow (parent)
HWND      parent;
#endif /* __STDC__ */
{  
	DialogBox (hInstance, MAKEINTRESOURCE (SEARCHDIALOG), parent, (DLGPROC) SearchDlgProc);
}

/*-----------------------------------------------------------------------
 CreateSaveAsDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void CreateSaveAsDlgWindow (HWND parent, char* path_name, int base_dlg, int save_form, int dir_save, int name_save, int img_save, int toggle_save)
#else  /* !__STDC__ */
void CreateSaveAsDlgWindow (parent, path_name, base_dlg, save_form, dir_save, name_save, img_save, toggle_save)
HWND  parent;
char* path_name;
int   base_dlg; 
int   save_form; 
int   dir_save; 
int   name_save;
int   img_save;
int   toggle_save;
#endif /* __STDC__ */
{  
	baseDlg          = base_dlg;
	saveForm         = save_form;
	dirSave          = dir_save;
	nameSave         = name_save;
	imgSave          = img_save;
	toggleSave       = toggle_save;
	currentParentRef = baseDlg + saveForm;
	sprintf (currentPathName, path_name);

	DialogBox (hInstance, MAKEINTRESOURCE (SAVEASDIALOG), parent, (DLGPROC) SaveAsDlgProc);
}

/*-----------------------------------------------------------------------
 CreateOPenDocDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void CreateOpenDocDlgWindow (HWND parent, char* doc_to_open)
#else  /* !__STDC__ */
void CreateOpenDocDlgWindow (parent, doc_to_open)
HWND  parent;
char* doc_to_open;
#endif /* __STDC__ */
{  
	DialogBox (hInstance, MAKEINTRESOURCE (OPENDOCDIALOG), parent, (DLGPROC) OpenDocDlgProc);
	strcpy (doc_to_open, urlToOpen);
}

/*-----------------------------------------------------------------------
 CreateSaveListDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void CreateSaveListDlgWindow (HWND parent, int nb_item, char* save_list, int base_dialog, int confirm_save)
#else  /* !__STDC__ */
void CreateSaveListDlgWindow (parent, nb_item, doc_to_open, base_dialog, confirm_save)
HWND  parent;
int   nb_item;
char* save_list;
int   base_dialog; 
int   confirm_save;
#endif /* __STDC__ */
{  
    nbItem      = nb_item;
    saveList    = save_list;
	baseDlg     = base_dialog;
	confirmSave = confirm_save;
	DialogBox (hInstance, MAKEINTRESOURCE (SAVELISTDIALOG), parent, (DLGPROC) SaveListDlgProc);
}

/*-----------------------------------------------------------------------
 CreateCloseDocDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void CreateCloseDocDlgWindow (HWND parent, char* title, char* msg, BOOL* save_befor, BOOL* close_dont_save)
#else  /* !__STDC__ */
void CreateCloseDocDlgWindow (parent, title, msg, save_befor, close_dont_save)
HWND  parent;
char* title;
char* msg;
BOOL* save_befor;
BOOL* close_dont_save;
#endif /* __STDC__ */
{  
	sprintf (message, msg);
	sprintf (wndTitle, title);

	DialogBox (hInstance, MAKEINTRESOURCE (CLOSEDOCDIALOG), parent, (DLGPROC) CloseDocDlgProc);
	*save_befor = saveBeforeClose;
	*close_dont_save = closeDontSave;
}

/*-----------------------------------------------------------------------
 CreateLanguageDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void CreateLanguageDlgWindow (HWND parent, char* title, char* msg1, int nb_item, char* lang_list, char* msg2)
#else  /* !__STDC__ */
void CreateLanguageDlgWindow (parent, title, msg1, nb_item, lang_list, msg2)
HWND  parent;
char* title;
char* msg1;
int   nb_item;
char* lang_list;
char* msg2;
#endif /* __STDC__ */
{  
	sprintf (wndTitle, title);
	sprintf (message, msg1);
	sprintf (message2, msg2);
	langList = lang_list;
	nbItem   = nb_item;

	DialogBox (hInstance, MAKEINTRESOURCE (LANGUAGEDIALOG), parent, (DLGPROC) LanguageDlgProc);
}

/*-----------------------------------------------------------------------
 CreateCharacterDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void CreateCharacterDlgWindow (HWND parent)
#else  /* !__STDC__ */
void CreateCharacterDlgWindow (parent)
HWND  parent;
#endif /* __STDC__ */
{  
	DialogBox (hInstance, MAKEINTRESOURCE (CHARACTERSDIALOG), parent, (DLGPROC) CharacterDlgProc);
}

/*-----------------------------------------------------------------------
 CreateCreateRuleDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void CreateCreateRuleDlgWindow (HWND parent, int base_dlg, int class_form, int class_select, int nb_class, char* class_list)
#else  /* !__STDC__ */
void CreateCreateRuleDlgWindow (parent, nb_class, class_list)
HWND  parent;
int   nb_class;
char* class_list;
#endif /* __STDC__ */
{  
	nbClass     = nb_class;
	classList   = class_list;
	baseDlg     = base_dlg;
	classForm   = class_form;
	classSelect = class_select;

	DialogBox (hInstance, MAKEINTRESOURCE (CREATERULEDIALOG), parent, (DLGPROC) CreateRuleDlgProc);
}

/*-----------------------------------------------------------------------
 CreateApplyClassDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void CreateApplyClassDlgWindow (HWND parent, int base_dlg, int class_form, int class_select, int nb_class, char* class_list)
#else  /* !__STDC__ */
void CreateApplyClassDlgWindow (parent, nb_class, class_list)
HWND  parent;
int   nb_class;
char* class_list;
#endif /* __STDC__ */
{  
	nbClass     = nb_class;
	classList   = class_list;
	baseDlg     = base_dlg;
	classForm   = class_form;
	classSelect = class_select;

	DialogBox (hInstance, MAKEINTRESOURCE (APPLYCLASSDIALOG), parent, (DLGPROC) ApplyClassDlgProc);
}

/*-----------------------------------------------------------------------
 CreateSpellCheckDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void CreateSpellCheckDlgWindow (HWND parent, char* label, char* rejectedChars,
								int spellingBase, int chkrSelectProp, int chkrMenuOR, 
							    int chkrFormCorrect, int chkrMenuIgnore, int chkrCaptureNC, int chkrSpecial)
#else  /* !__STDC__ */
void CreateSpellCheckDlgWindow (parent, label, rejectedChars, spellingBase, 
								chkrSelectProp, chkrMenuOR, chkrFormCorrect, chkrMenuIgnore, chkrCaptureNC, chkrSpecial)
HWND  parent;
char* label;
char* rejectedChars;
int   spellingBase; 
int   chkrSelectProp; 
int   chkrMenuOR; 
int   chkrFormCorrect;
int   chkrMenuIgnore;
int   chkrCaptureNC;
int   chkrSpecial;
#endif /* __STDC__ */
{  
	SpellingBase    = spellingBase; 
	ChkrSelectProp  = chkrSelectProp;
	ChkrMenuOR      = chkrMenuOR ;
	ChkrFormCorrect = chkrFormCorrect;
	ChkrMenuIgnore  = chkrMenuIgnore;
	ChkrCaptureNC   = chkrCaptureNC;

	sprintf (currentLabel, label);
	sprintf (currentRejectedchars, rejectedChars);

	DialogBox (hInstance, MAKEINTRESOURCE (SPELLCHECKDIALOG), parent, (DLGPROC) SpellCheckDlgProc);
}

/*-----------------------------------------------------------------------
 CreateInitConfirmDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void CreateInitConfirmDlgWindow (HWND parent, int ref, char* title, char* msg)
#else  /* !__STDC__ */
void CreateInitConfirmDlgWindow (parent, ref, title, msg)
HWND  parent;
int   ref;
char* title;
char* msg;
#endif /* __STDC__ */
{  
	sprintf (message, msg);
	sprintf (wndTitle, title);
	currentRef = ref;

	DialogBox (hInstance, MAKEINTRESOURCE (INITCONFIRMDIALOG), parent, (DLGPROC) InitConfirmDlgProc);
}

/*-----------------------------------------------------------------------
 CreateChangeFormatDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void CreateChangeFormatDlgWindow (HWND parent)
#else  /* !__STDC__ */
void CreateChangeFormatDlgWindow (parent)
HWND  parent;
#endif /* __STDC__ */
{  
	DialogBox (hInstance, MAKEINTRESOURCE (FORMATDIALOG), parent, (DLGPROC) ChangeFormatDlgProc);
}

/*-----------------------------------------------------------------------
 CreateGreekKeyboardDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void CreateGreekKeyboardDlgWindow (HWND parent)
#else  /* !__STDC__ */
void CreateGreekKeyboardDlgWindow (parent)
HWND  parent;
#endif /* __STDC__ */
{  
	DialogBox (hInstance, MAKEINTRESOURCE (GALPHABETDIALOG), NULL, (DLGPROC) GreekKeyboardDlgProc);
}

/*-----------------------------------------------------------------------
 CreateAuthentificationDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void CreateAuthenticationDlgWindow (HWND parent)
#else  /* !__STDC__ */
void CreateAuthenticationDlgWindow (parent)
HWND  parent;
#endif /* __STDC__ */
{  
	DialogBox (hInstance, MAKEINTRESOURCE (AUTHENTIFICATIONDIALOG), parent, (DLGPROC) AuthenticationDlgProc);
}


        /*********************************************************
         *                                                       *
         *                   C A L L B A C K S                   *
         *                                                       *
         *********************************************************/


/*-----------------------------------------------------------------------
 LinkDlgProc
 ------------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK LinkDlgProc (HWND hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK LinkDlgProc (hwnDlg, msg, wParam, lParam)
HWND   hwndParent; 
UINT   msg; 
WPARAM wParam; 
LPARAM lParam;
#endif /* __STDC__ */
{
    switch (msg) {
           case WM_INITDIALOG:
			    SetDlgItemText (hwnDlg, IDC_URLEDIT, urlToOpen);
			    break;

           case WM_COMMAND:
	            switch (LOWORD (wParam)) {
				       case ID_CONFIRM:
						    GetDlgItemText (hwnDlg, IDC_URLEDIT, urlToOpen, sizeof (urlToOpen) - 1);
							AttrHREFvalue = (char*) TtaGetMemory (strlen (urlToOpen) + 1);
							strcpy (AttrHREFvalue, urlToOpen);
							ThotCallback (baseDlg + attrHRefTxt, STRING_DATA, urlToOpen);
							ThotCallback (baseDlg + attrHRefForm, INTEGER_DATA, (char*) 1);
							/* CallbackDialogue (currentRef, INTEGER_DATA, (char*) 1); */
							EndDialog (hwnDlg, ID_CONFIRM);
					        break;

				       case ID_DONE:
							ThotCallback (baseDlg + attrHRefForm, INTEGER_DATA, (char*) 0);
					        EndDialog (hwnDlg, ID_DONE);
					        break;
				}
				break;

           default: return (FALSE) ;
    }
	return TRUE;
}
	
/*-----------------------------------------------------------------------
 LinkDlgProc
 ------------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK HelpDlgProc (HWND hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK HelpDlgProc (hwnDlg, msg, wParam, lParam)
HWND   hwndParent; 
UINT   msg; 
WPARAM wParam; 
LPARAM lParam;
#endif /* __STDC__ */
{
	HWND messageWnd1;
	HWND messageWnd2;
	HWND messageWnd3;

    switch (msg) {
           case WM_INITDIALOG:
				messageWnd1 = CreateWindow ("STATIC", currentPathName, WS_CHILD | WS_VISIBLE | SS_LEFT,
					                       15, 70, 400, 20, hwnDlg, (HMENU) 9, 
										   (HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);
				messageWnd2 = CreateWindow ("STATIC", message, WS_CHILD | WS_VISIBLE | SS_LEFT,
					                       15, 90, 400, 60, hwnDlg, (HMENU) 99, 
										   (HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);
				messageWnd3 = CreateWindow ("STATIC", message2, WS_CHILD | WS_VISIBLE | SS_LEFT,
					                       15, 110, 400, 60, hwnDlg, (HMENU) 999, 
										   (HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);
			    break;

           case WM_COMMAND:
	            switch (LOWORD (wParam)) {
				       case ID_CONFIRM:
							EndDialog (hwnDlg, ID_CONFIRM);
					        break;
				}
				break;

           default: return (FALSE) ;
    }
	return TRUE;
}
	
/*-----------------------------------------------------------------------
 LinkDlgProc
 ------------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK MathDlgProc (HWND hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK MathDlgProc (hwnDlg, msg, wParam, lParam)
HWND   hwndParent; 
UINT   msg; 
WPARAM wParam; 
LPARAM lParam;
#endif /* __STDC__ */
{
	HWND messageWnd1;
	HWND messageWnd2;
	HWND messageWnd3;

    switch (msg) {
           case WM_COMMAND:
                SetFocus (currentFrame);
	            switch (LOWORD (wParam)) {
				       case ID_DONE:
							EndDialog (hwnDlg, ID_DONE);
					        break;

                       case IDC_MATH:
						    ThotCallback (baseDlg + MenuMaths, INTEGER_DATA, (char*)0);
                            break;

                       case IDC_MATHD:
						    ThotCallback (baseDlg + MenuMaths, INTEGER_DATA, (char*)1);
                            break;

                       case IDC_ROOT:
						    ThotCallback (baseDlg + MenuMaths, INTEGER_DATA, (char*)2);
                            break;

                       case IDC_SROOT:
						    ThotCallback (baseDlg + MenuMaths, INTEGER_DATA, (char*)3);
                            break;

                       case IDC_DIV:
						    ThotCallback (baseDlg + MenuMaths, INTEGER_DATA, (char*)4);
                            break;

                       case IDC_POWIND:
						    ThotCallback (baseDlg + MenuMaths, INTEGER_DATA, (char*)5);
                            break;

                       case IDC_POW:
						    ThotCallback (baseDlg + MenuMaths, INTEGER_DATA, (char*)6);
                            break;

                       case IDC_IND:
						    ThotCallback (baseDlg + MenuMaths, INTEGER_DATA, (char*)7);
                            break;

                       case IDC_UPDN:
						    ThotCallback (baseDlg + MenuMaths, INTEGER_DATA, (char*)8);
                            break;

                       case IDC_UP:
						    ThotCallback (baseDlg + MenuMaths, INTEGER_DATA, (char*)9);
                            break;

                       case IDC_DOWN:
						    ThotCallback (baseDlg + MenuMaths, INTEGER_DATA, (char*)10);
                            break;

                       case IDC_PAREXP:
						    ThotCallback (baseDlg + MenuMaths, INTEGER_DATA, (char*)11);
                            break;

                       case IDC_UDLR:
						    ThotCallback (baseDlg + MenuMaths, INTEGER_DATA, (char*)12);
                            break;

                       case IDC_SYM:
						    ThotCallback (baseDlg + MenuMaths, INTEGER_DATA, (char*)13);
                            break;
				}
				break;

           default: return (FALSE) ;
    }
	return TRUE;
}
	
/*-----------------------------------------------------------------------
 PrintDlgProc
 ------------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK PrintDlgProc (HWND hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK PrintDlgProc (hwnDlg, msg, wParam, lParam)
HWND   hwndParent; 
UINT   msg; 
WPARAM wParam; 
LPARAM lParam;
#endif /* __STDC__ */
{
    switch (msg) {
	       case WM_INITDIALOG:
			    SetDlgItemText (hwnDlg, IDC_PRINTEREDIT, "lpr");
				break;
		   case WM_COMMAND:
			    switch (LOWORD (wParam)) {
				       case ID_PRINT:
					        EndDialog (hwnDlg, ID_PRINT);
			                MessageBox (hwnDlg, "Not yet supported", "Warning", MB_OK);
							break;
				       case IDCANCEL:
					 	    EndDialog (hwnDlg, IDCANCEL);
							break;
				}
				break;
				default: return FALSE;
	}
	return TRUE ;
}

/*-----------------------------------------------------------------------
 Align1DlgProc
 ------------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK Align1DlgProc (HWND hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK Align1DlgProc (hwnDlg, msg, wParam, lParam)
HWND   hwndParent; 
UINT   msg; 
WPARAM wParam; 
LPARAM lParam;
#endif /* __STDC__ */
{
	static int iLocation;
    switch (msg) {
		   case WM_COMMAND:
			    switch (LOWORD (wParam)) {
					   case IDC_LEFT:
						    iLocation = 0;
							break;

					   case IDC_CENTER:
						    iLocation = 1;
							break;

					   case IDC_RIGHT:
						    iLocation = 2;
							break;

				       case ID_APPLY:
						    ThotCallback (NumMenuAttrEnum, INTEGER_DATA, (char*) iLocation);
							ThotCallback (NumMenuAttr, INTEGER_DATA, (char*) 1);
							break;

					   case ID_DELETE:
						    ThotCallback (NumMenuAttrEnum, INTEGER_DATA, (char*) iLocation);
							ThotCallback (NumMenuAttr, INTEGER_DATA, (char*) 2);
							break;

					   case ID_DONE:
							ThotCallback (NumMenuAttr, INTEGER_DATA, (char*) 0);
					 	    EndDialog (hwnDlg, IDCANCEL);
							break;
				}
				break;

				default: return FALSE;
	}
	return TRUE ;
}

/*-----------------------------------------------------------------------
 Align2DlgProc
 ------------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK Align2DlgProc (HWND hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK Align2DlgProc (hwnDlg, msg, wParam, lParam)
HWND   hwndParent; 
UINT   msg; 
WPARAM wParam; 
LPARAM lParam;
#endif /* __STDC__ */
{
	static int iLocation;
    switch (msg) {
		   case WM_COMMAND:
			    switch (LOWORD (wParam)) {
				       case IDC_TOP:
						    iLocation = 0;
							break;

					   case IDC_MIDDLE:
						    iLocation = 1;
							break;

					   case IDC_BOTTOM:
						    iLocation = 2;
							break;

					   case IDC_LEFT:
						    iLocation = 3;
							break;

					   case IDC_RIGHT:
						    iLocation = 4;
							break;

				       case ID_APPLY:
						    ThotCallback (NumMenuAttrEnum, INTEGER_DATA, (char*) iLocation);
							ThotCallback (NumMenuAttr, INTEGER_DATA, (char*) 1);
							break;

					   case ID_DELETE:
						    ThotCallback (NumMenuAttrEnum, INTEGER_DATA, (char*) iLocation);
							ThotCallback (NumMenuAttr, INTEGER_DATA, (char*) 2);
							break;

					   case ID_DONE:
							ThotCallback (NumMenuAttr, INTEGER_DATA, (char*) 0);
					 	    EndDialog (hwnDlg, IDCANCEL);
							break;
				}
				break;

				default: return FALSE;
	}
	return TRUE ;
}

/*-----------------------------------------------------------------------
 SearchDlgProc
 ------------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK SearchDlgProc (HWND hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK SearchDlgProc (hwnDlg, msg, wParam, lParam)
HWND   hwndParent; 
UINT   msg; 
WPARAM wParam; 
LPARAM lParam;
#endif /* __STDC__ */
{
	static int iMode, iLocation;
	static char textToSearch [255];
	static char newText [255];
	static BOOL upper_lower = FALSE ;
    switch (msg) {
	       case WM_INITDIALOG:
			    SetDlgItemText (hwnDlg, IDC_SEARCHEDIT, "");
			    SetDlgItemText (hwnDlg, IDC_REPLACEDIT, "");

				iMode     = 0 ;
				iLocation = 3 ;

				CheckRadioButton (hwnDlg, IDC_NOREPLACE, IDC_AUTOMATIC, IDC_NOREPLACE);
				CheckRadioButton (hwnDlg, IDC_BEFORE, IDC_WHOLEDOC, IDC_WHOLEDOC);

				break;
		   case WM_COMMAND:
			    switch (LOWORD (wParam)) {
				       case ID_CONFIRM:
						    GetDlgItemText (hwnDlg, IDC_SEARCHEDIT, textToSearch, sizeof (textToSearch) - 1);
						    GetDlgItemText (hwnDlg, IDC_REPLACEDIT, newText, sizeof (newText) - 1);
							if (newText && newText[0] != '\0' && iMode == 0) {
							   iMode = 1;
				               CheckRadioButton (hwnDlg, IDC_NOREPLACE, IDC_AUTOMATIC, IDC_ONREQUEST);
							}

						    if (iMode == 1 || iMode == 2) 
							   ThotCallback (NumZoneTextReplace, STRING_DATA, newText);
							
						    ThotCallback (NumZoneTextSearch, STRING_DATA, textToSearch);
						    ThotCallback (NumMenuReplaceMode, INTEGER_DATA, (char*) iMode);
						    ThotCallback (NumMenuOrSearchText, INTEGER_DATA, (char*) iLocation);
						    ThotCallback (NumFormSearchText, INTEGER_DATA, (char*) 1);
							if (iLocation == 3) {
				               CheckRadioButton (hwnDlg, IDC_BEFORE, IDC_WHOLEDOC, IDC_AFTER);
							   iLocation = 2;
							}
							break;

					   case ID_NOREPLACE:
						    ThotCallback (NumZoneTextSearch, STRING_DATA, textToSearch);
						    ThotCallback (NumMenuReplaceMode, INTEGER_DATA, (char*) 0);
						    ThotCallback (NumMenuOrSearchText, INTEGER_DATA, (char*) iLocation);
						    ThotCallback (NumFormSearchText, INTEGER_DATA, (char*) 1);
							if (iLocation == 3) {
				               CheckRadioButton (hwnDlg, IDC_BEFORE, IDC_WHOLEDOC, IDC_AFTER);
							   iLocation = 2;
							}
						    break;

				       case ID_DONE:
						    ThotCallback (120, 1, NULL);
					        EndDialog (hwnDlg, ID_DONE);
							break;

					   case IDC_NOREPLACE:
						    iMode = 0 ;
							CheckRadioButton (hwnDlg, IDC_NOREPLACE, IDC_AUTOMATIC, LOWORD (wParam));
							break;

					   case IDC_ONREQUEST:
						    iMode = 1 ;
							CheckRadioButton (hwnDlg, IDC_NOREPLACE, IDC_AUTOMATIC, LOWORD (wParam));
							break;

					   case IDC_AUTOMATIC:
						    iMode = 2 ;
							CheckRadioButton (hwnDlg, IDC_NOREPLACE, IDC_AUTOMATIC, LOWORD (wParam));
							break;

					   case IDC_BEFORE:
						    iLocation = 0;
							CheckRadioButton (hwnDlg, IDC_BEFORE, IDC_WHOLEDOC, LOWORD (wParam));
							break;

					   case IDC_WITHIN:
						    iLocation = 1;
							CheckRadioButton (hwnDlg, IDC_BEFORE, IDC_WHOLEDOC, LOWORD (wParam));
							break;

					   case IDC_AFTER:
						    iLocation = 2;
							CheckRadioButton (hwnDlg, IDC_BEFORE, IDC_WHOLEDOC, LOWORD (wParam));
							break;

					   case IDC_WHOLEDOC:
						    iLocation = 3;
							CheckRadioButton (hwnDlg, IDC_BEFORE, IDC_WHOLEDOC, LOWORD (wParam));
							break;

					   case	IDC_UPPERLOWER:
						    ThotCallback (NumToggleUpperEqualLower, INTEGER_DATA, (char*) 0);
							break;
				}
				break;
				default: return FALSE;
	}
	return TRUE ;
}

/*-----------------------------------------------------------------------
 SaveAsDlgProc
 ------------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK SaveAsDlgProc (HWND hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK SaveAsDlgProc (hwnDlg, msg, wParam, lParam)
HWND   hwndParent; 
UINT   msg; 
WPARAM wParam; 
LPARAM lParam;
#endif /* __STDC__ */
{
	static char txt [500];
    switch (msg) {
	       case WM_INITDIALOG:
			    currentDlg = hwnDlg;
			    SetDlgItemText (hwnDlg, IDC_EDITDOCSAVE, currentPathName);
				if (SaveAsHTML)
				   CheckRadioButton (hwnDlg, IDC_HTML, IDC_TEXT, IDC_HTML);

				if (SaveAsText)
				   CheckRadioButton (hwnDlg, IDC_HTML, IDC_TEXT, IDC_TEXT);

				if (CopyImages)
				   CheckRadioButton (hwnDlg, IDC_COPYIMG, IDC_COPYIMG, IDC_COPYIMG);

				if (UpdateURLs)
				   CheckRadioButton (hwnDlg, IDC_TRANSFORMURL, IDC_TRANSFORMURL, IDC_TRANSFORMURL);
				break;
           case WM_DESTROY:
                currentDlg = (HWND) 0;
                EndDialog (hwnDlg, IDCANCEL);
				break;

		   case WM_COMMAND:
			    if (HIWORD (wParam) == EN_UPDATE) {
				   if (LOWORD (wParam) == IDC_EDITDOCSAVE) {
					  GetDlgItemText (hwnDlg, IDC_EDITDOCSAVE, urlToOpen, sizeof (urlToOpen) - 1);
					  ThotCallback (baseDlg + nameSave, STRING_DATA, urlToOpen);
				   } else if (LOWORD (wParam) == IDC_EDITIMGSAVE) {
					      GetDlgItemText (hwnDlg, IDC_EDITIMGSAVE, urlToOpen, sizeof (urlToOpen) - 1);
					      ThotCallback (baseDlg + imgSave, STRING_DATA, urlToOpen);
				   }
				}
			    switch (LOWORD (wParam)) {
				       case IDC_HTML:
						    ThotCallback (baseDlg + toggleSave, INTEGER_DATA, (char*) 0);
						    break;

					   case IDC_TEXT:
						    ThotCallback (baseDlg + toggleSave, INTEGER_DATA, (char*) 1);
						    break;

					   case IDC_COPYIMG:
						    ThotCallback (baseDlg + toggleSave, INTEGER_DATA, (char*) 3);
						    break;

					   case IDC_TRANSFORMURL:
						    ThotCallback (baseDlg + toggleSave, INTEGER_DATA, (char*) 4);
						    break;

				       case ID_CLEAR:
						    SetDlgItemText (hwnDlg, IDC_EDITDOCSAVE, "");
							SetDlgItemText (hwnDlg, IDC_EDITIMGSAVE, "");
							ThotCallback (baseDlg + saveForm, INTEGER_DATA, (char*) 2);
							break;

					   case IDC_BROWSE:
						    WIN_ListSaveDirectory (currentParentRef, urlToOpen);
							SetDlgItemText (hwnDlg, IDC_EDITDOCSAVE, urlToOpen);
							ThotCallback (baseDlg + nameSave, STRING_DATA, urlToOpen);
							break;

					   case IDCANCEL:
						    ThotCallback (baseDlg + saveForm, INTEGER_DATA, (char*) 0);
                            currentDlg = (HWND) 0;
 					        EndDialog (hwnDlg, IDCANCEL);
							break;

				       case ID_CONFIRM:
						    ThotCallback (baseDlg + saveForm, INTEGER_DATA, (char*) 1);
                            currentDlg = (HWND) 0;
					        EndDialog (hwnDlg, ID_CONFIRM);
							break;
				}
				break;

				default: return FALSE;
	}
	return TRUE ;
}

/*-----------------------------------------------------------------------
 CloseDocDlgProc
 ------------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK OpenDocDlgProc (HWND hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK OpenDocDlgProc (hwnDlg, msg, wParam, lParam)
HWND   hwndParent; 
UINT   msg; 
WPARAM wParam; 
LPARAM lParam;
#endif /* __STDC__ */
{
    switch (msg) {
	       case WM_INITDIALOG:
			    SetDlgItemText (hwnDlg, IDC_GETURL, "");
				break;
		   case WM_COMMAND:
			    switch (LOWORD (wParam)) {
				       case ID_CONFIRM:
						    GetDlgItemText (hwnDlg, IDC_GETURL, urlToOpen, sizeof (urlToOpen) - 1);
					        EndDialog (hwnDlg, ID_CONFIRM);
							break;

				       case IDC_BROWSE:
							/* WIN_ListOpenDirectory (hwnDlg, urlToOpen); */
                            OpenFileName.lStructSize       = sizeof (OPENFILENAME); 
                            OpenFileName.hwndOwner         = hwnDlg; 
                            OpenFileName.hInstance         = hInstance ; 
                            OpenFileName.lpstrFilter       = (LPSTR) szFilter; 
                            OpenFileName.lpstrCustomFilter = (LPTSTR) NULL; 
                            OpenFileName.nMaxCustFilter    = 0L; 
                            OpenFileName.nFilterIndex      = 1L; 
                            OpenFileName.lpstrFile         = (LPSTR) szFileName; 
                            OpenFileName.nMaxFile          = 256; 
                            OpenFileName.lpstrInitialDir   = NULL; 
                            OpenFileName.lpstrTitle        = TEXT ("Open a File"); 
                            OpenFileName.nFileOffset       = 0; 
                            OpenFileName.nFileExtension    = 0; 
                            OpenFileName.lpstrDefExt       = TEXT ("*.html"); 
                            OpenFileName.lCustData         = 0; 
                            OpenFileName.Flags             = OFN_SHOWHELP | OFN_HIDEREADONLY; 
 
                            if (GetOpenFileName (&OpenFileName)) {
	                           strcpy (urlToOpen, OpenFileName.lpstrFile);
	                        }

					        EndDialog (hwnDlg, IDC_BROWSE);
							break;

				       case IDCANCEL:
					        EndDialog (hwnDlg, IDCANCEL);
							break;
				}
				break;
				default: return FALSE;
	}
	return TRUE ;
}

/*-----------------------------------------------------------------------
 CloseDocDlgProc
 ------------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK SaveListDlgProc (HWND hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK SaveListDlgProc (hwnDlg, msg, wParam, lParam)
HWND   hwndParent; 
UINT   msg; 
WPARAM wParam; 
LPARAM lParam;
#endif /* __STDC__ */
{
    HWND  wndSaveList;
	int   index = 0;
	UINT  i = 0;

	static int itemIndex; 
	static char szBuffer [MAX_BUFF];

    switch (msg) {
	       case WM_INITDIALOG:
				wndSaveList = CreateWindow ("listbox", NULL, WS_CHILD | WS_VISIBLE | LBS_STANDARD,
					                         10, 30, 260, 180, hwnDlg, (HMENU) 1, 
											 (HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);

	            SendMessage (wndSaveList, LB_RESETCONTENT, 0, 0);

	            while (i < nbItem && saveList[index] != '\0') {
	                  SendMessage (wndSaveList, LB_INSERTSTRING, i, (LPARAM) &saveList[index]);  
	                  index += strlen (&saveList[index]) + 1;	/* Longueur de l'intitule */
					  i++;
				}
                break;
		   case WM_COMMAND:
				if (LOWORD (wParam) == 1 && HIWORD (wParam) == LBN_SELCHANGE) {
				   itemIndex = SendMessage (wndSaveList, LB_GETCURSEL, 0, 0);
				   itemIndex = SendMessage (wndSaveList, LB_GETTEXT, itemIndex, (LPARAM) szBuffer);
			       SetDlgItemText (hwnDlg, IDC_LANGEDIT, szBuffer);
				}
                switch (LOWORD (wParam)) {
                       case ID_CONFIRM:
                            ThotCallback (baseDlg + confirmSave ,INTEGER_DATA, (char*)1);
                            EndDialog (hwnDlg, ID_CONFIRM);
						    SendMessage (currentDlg, WM_DESTROY, 0, 0);
							break;
                            
				       case IDCANCEL:
                            ThotCallback (baseDlg + confirmSave ,INTEGER_DATA, (char*)0);
					        EndDialog (hwnDlg, IDCANCEL);
							break;
                }
				break;     
				default: return FALSE;
	}
	return TRUE ;

}

/*-----------------------------------------------------------------------
 CloseDocDlgProc
 ------------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK CloseDocDlgProc (HWND hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK CloseDocDlgProc (hwnDlg, msg, wParam, lParam)
HWND   hwndParent; 
UINT   msg; 
WPARAM wParam; 
LPARAM lParam;
#endif /* __STDC__ */
{
	HWND messageWnd;
    switch (msg) {
	       case WM_INITDIALOG:
			    SetWindowText (hwnDlg, wndTitle);
				messageWnd = CreateWindow ("STATIC", message, WS_CHILD | WS_VISIBLE | SS_LEFT,
					                       15, 15, 400, 60, hwnDlg, (HMENU) 99, 
										   (HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);
				break;
		   case WM_COMMAND:
			    switch (LOWORD (wParam)) {
				       case ID_SAVEDOC:
						    /* SaveDocument (currentDoc, currentView); */
			                closeDontSave   = FALSE; 
	                        saveBeforeClose = TRUE;
							EndDialog (hwnDlg, ID_SAVEDOC);
							break;

				       case IDC_DONTSAVE:
			                closeDontSave   = FALSE; 
                            saveBeforeClose = FALSE;
					        EndDialog (hwnDlg, IDC_DONTSAVE);
							break;

				       case IDCANCEL:
			                closeDontSave = TRUE;
                            saveBeforeClose = FALSE;
					        EndDialog (hwnDlg, IDCANCEL);
							break;
				}
				break;
				default: return FALSE;
	}
	return TRUE ;
}

/*-----------------------------------------------------------------------
 LanguageDlgProc
 ------------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK LanguageDlgProc (HWND hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK LanguageDlgProc (hwnDlg, msg, wParam, lParam)
HWND   hwndParent; 
UINT   msg; 
WPARAM wParam; 
LPARAM lParam;
#endif /* __STDC__ */
{
	int  index = 0;
	UINT  i = 0;

	HWND wndLangEdit;
	HWND wndMessage1;
	HWND wndMessage2;

	static HWND wndLangList;
	static int itemIndex; 
	static char szBuffer [MAX_BUFF];

    switch (msg) {
	       case WM_INITDIALOG:
			    SetWindowText (hwnDlg, wndTitle);
				wndMessage1 = CreateWindow ("STATIC", message, WS_CHILD | WS_VISIBLE | SS_LEFT,
					                        10, 10, 200, 20, hwnDlg, (HMENU) 99, 
											(HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);

				wndLangList = CreateWindow ("listbox", NULL, WS_CHILD | WS_VISIBLE | LBS_STANDARD,
					                         10, 40, 240, 200, hwnDlg, (HMENU) 1, 
											 (HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);

	            SendMessage (wndLangList, LB_RESETCONTENT, 0, 0);
	            while (i < nbItem && langList[index] != '\0') {
	                  SendMessage (wndLangList, LB_INSERTSTRING, i, (LPARAM) &langList[index]);  
	                  index += strlen (&langList[index]) + 1;	/* Longueur de l'intitule */
					  i++;
				}

				wndLangEdit	= CreateWindow ("EDIT", NULL, WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER,
					                         10, 250, 240, 30, hwnDlg, (HMENU) IDC_LANGEDIT, 
											 (HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);

				wndMessage2 = CreateWindow ("STATIC", message2, WS_CHILD | WS_VISIBLE | SS_LEFT,
					                        10, 280, 200, 20, hwnDlg, (HMENU) 99, 
											(HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);
				break;

		   case WM_COMMAND:
				if (LOWORD (wParam) == 1 && HIWORD (wParam) == LBN_SELCHANGE) {
				   itemIndex = SendMessage (wndLangList, LB_GETCURSEL, 0, 0);
				   itemIndex = SendMessage (wndLangList, LB_GETTEXT, itemIndex, (LPARAM) szBuffer);
			       SetDlgItemText (hwnDlg, IDC_LANGEDIT, szBuffer);
				}

			    switch (LOWORD (wParam)) {
				       case ID_APPLY:
						    ThotCallback (NumSelectLanguage, STRING_DATA, szBuffer);
							ThotCallback (NumFormLanguage, INTEGER_DATA, (char*) 1);
							EndDialog (hwnDlg, ID_APPLY);
							break;

				       case ID_DELETE:
						    ThotCallback (NumSelectLanguage, STRING_DATA, szBuffer);
							ThotCallback (NumFormLanguage, INTEGER_DATA, (char*) 2);
					        EndDialog (hwnDlg, ID_DELETE);
							break;

				       case ID_DONE:
							ThotCallback (NumFormLanguage, INTEGER_DATA, (char*) 0);
					        EndDialog (hwnDlg, ID_DONE);
							break;
				}
				break;
				default: return FALSE;
	}
	return TRUE ;
}

/*-----------------------------------------------------------------------
 CharacterDlgProc
 ------------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK CharacterDlgProc (HWND hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK CharacterDlgProc (hwnDlg, msg, wParam, lParam)
HWND   hwndParent; 
UINT   msg; 
WPARAM wParam; 
LPARAM lParam;
#endif /* __STDC__ */
{
    switch (msg) {
	       case WM_INITDIALOG:
				CheckRadioButton (hwnDlg, IDC_TIMES, IDC_DEFAULTFAMILY, IDC_TIMES);
				CheckRadioButton (hwnDlg, IDC_ROMAN, IDC_DEFAULTSTYLE, IDC_ROMAN);
				CheckRadioButton (hwnDlg, IDC_NORMAL, IDC_DEFAULTUNDERLINE, IDC_NORMAL);
				CheckRadioButton (hwnDlg, IDC_06PT, IDC_DEFAULTSIZE, IDC_12PT);

				break;
		   case WM_COMMAND:
			    switch (LOWORD (wParam)) {
				       case ID_DONE:
						    ThotCallback (NumFormPresChar, INTEGER_DATA, (char*) 0);
						    EndDialog (hwnDlg, ID_DONE);
							break;

					   case ID_APPLY:
						    ThotCallback (NumFormPresChar, INTEGER_DATA, (char*) 1);
							break;

					   case IDC_TIMES:
						    ThotCallback (NumMenuCharFamily, INTEGER_DATA, (char*) 0);
							break;

					   case IDC_HELVETICA:
						    ThotCallback (NumMenuCharFamily, INTEGER_DATA, (char*) 1);
							break;

					   case IDC_COURIER:
						    ThotCallback (NumMenuCharFamily, INTEGER_DATA, (char*) 2);
							break;

					   case IDC_DEFAULTFAMILY:
						    ThotCallback (NumMenuCharFamily, INTEGER_DATA, (char*) 3);
							break;

					   case IDC_ROMAN:
						    ThotCallback (NumMenuStyleChar, INTEGER_DATA, (char*) 0);
							break;

					   case IDC_BOLD:
						    ThotCallback (NumMenuStyleChar, INTEGER_DATA, (char*) 1);
							break;

					   case IDC_ITALIC:
						    ThotCallback (NumMenuStyleChar, INTEGER_DATA, (char*) 2);
							break;

					   case IDC_OBLIQUE:
						    ThotCallback (NumMenuStyleChar, INTEGER_DATA, (char*) 3);
							break;

					   case IDC_BOLDITALIC:
						    ThotCallback (NumMenuStyleChar, INTEGER_DATA, (char*) 4);
							break;

					   case IDC_BOLDOBLIQUE:
						    ThotCallback (NumMenuStyleChar, INTEGER_DATA, (char*) 5);
							break;

					   case IDC_DEFAULTSTYLE:
						    ThotCallback (NumMenuStyleChar, INTEGER_DATA, (char*) 6);
							break;

					   case IDC_NORMAL:
						    ThotCallback (NumMenuUnderlineType, INTEGER_DATA, (char*) 0);
							break;

					   case IDC_UNDERLINE:
						    ThotCallback (NumMenuUnderlineType, INTEGER_DATA, (char*) 1);
							break;

					   case IDC_CROSSOUT:
						    ThotCallback (NumMenuUnderlineType, INTEGER_DATA, (char*) 2);
							break;

					   case IDC_DEFAULTUNDERLINE:
						    ThotCallback (NumMenuUnderlineType, INTEGER_DATA, (char*) 3);
							break;

					   case IDC_06PT:
						    ThotCallback (NumMenuCharFontSize, INTEGER_DATA, (char*) 0);
							break;

					   case IDC_08PT:
						    ThotCallback (NumMenuCharFontSize, INTEGER_DATA, (char*) 1);
							break;

					   case IDC_10PT:
						    ThotCallback (NumMenuCharFontSize, INTEGER_DATA, (char*) 2);
							break;

					   case IDC_12PT:
						    ThotCallback (NumMenuCharFontSize, INTEGER_DATA, (char*) 3);
							break;

					   case IDC_14PT:
						    ThotCallback (NumMenuCharFontSize, INTEGER_DATA, (char*) 4);
							break;

					   case IDC_16PT:
						    ThotCallback (NumMenuCharFontSize, INTEGER_DATA, (char*) 5);
							break;

					   case IDC_20PT:
						    ThotCallback (NumMenuCharFontSize, INTEGER_DATA, (char*) 6);
							break;

					   case IDC_24PT:
						    ThotCallback (NumMenuCharFontSize, INTEGER_DATA, (char*) 7);
							break;

					   case IDC_30PT:
						    ThotCallback (NumMenuCharFontSize, INTEGER_DATA, (char*) 8);
							break;

					   case IDC_40PT:
						    ThotCallback (NumMenuCharFontSize, INTEGER_DATA, (char*) 9);
							break;

					   case IDC_60PT:
						    ThotCallback (NumMenuCharFontSize, INTEGER_DATA, (char*) 10);
							break;

					   case IDC_DEFAULTSIZE:
						    ThotCallback (NumMenuCharFontSize, INTEGER_DATA, (char*) 11);
							break;
				}
				break;

		   default:	return FALSE;
	}
	return TRUE;
}

/*-----------------------------------------------------------------------
 CreateRuleDlgProc
 ------------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK CreateRuleDlgProc (HWND hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK CreateRuleDlgProc (hwnDlg, msg, wParam, lParam)
HWND   hwndParent; 
UINT   msg; 
WPARAM wParam; 
LPARAM lParam;
#endif /* __STDC__ */
{
	int  index = 0;
	int  i = 0;

	static HWND wndListRule;
	static HWND wndEditRule;
	static int  itemIndex;
	static char szBuffer [MAX_BUFF];

    switch (msg) {
	       case WM_INITDIALOG:
				wndListRule = CreateWindow ("listbox", NULL, WS_CHILD | WS_VISIBLE | LBS_STANDARD,
					                         10, 10, 200, 130, hwnDlg, (HMENU) 1, 
											 (HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);

	            SendMessage (wndListRule, LB_RESETCONTENT, 0, 0);
	            while (i < nbClass && classList[index] != '\0') {
	                  SendMessage (wndListRule, LB_INSERTSTRING, i, (LPARAM) &classList[index]);  
	                  index += strlen (&classList[index]) + 1;	/* Longueur de l'intitule */
					  i++;
				}

				wndEditRule	= CreateWindow ("EDIT", NULL, WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER,
					                        10, 150, 200, 30, hwnDlg, (HMENU) IDC_EDITRULE, 
											(HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);

			    SetDlgItemText (hwnDlg, IDC_EDITRULE, classList);
				break;

		   case WM_COMMAND:
				if (LOWORD (wParam) == 1 && HIWORD (wParam) == LBN_SELCHANGE) {
				   itemIndex = SendMessage (wndListRule, LB_GETCURSEL, 0, 0);
				   itemIndex = SendMessage (wndListRule, LB_GETTEXT, itemIndex, (LPARAM) szBuffer);
			       SetDlgItemText (hwnDlg, IDC_EDITRULE, szBuffer);
				   ThotCallback (baseDlg + classSelect, STRING_DATA, szBuffer);
				} else if (HIWORD (wParam) == EN_UPDATE) {
					   char text [100];
                       GetDlgItemText (hwnDlg, IDC_EDITRULE, text, sizeof (text) - 1);
					   ThotCallback (baseDlg + classForm, STRING_DATA, text);
				}

			    switch (LOWORD (wParam)) {
				       case ID_CONFIRM:
						    ThotCallback (baseDlg + classForm, INTEGER_DATA, (char*) 1);
						    EndDialog (hwnDlg, ID_CONFIRM);
							break;

				       case ID_DONE:
						    ThotCallback (baseDlg + classForm, INTEGER_DATA, (char*) 0);
						    EndDialog (hwnDlg, ID_DONE);
							break;

				}
				break;
				default: return FALSE;
	}
	return TRUE ;
}

/*-----------------------------------------------------------------------
 ApplyClassDlgProc
 ------------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK ApplyClassDlgProc (HWND hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK ApplyClassDlgProc (hwnDlg, msg, wParam, lParam)
HWND   hwndParent; 
UINT   msg; 
WPARAM wParam; 
LPARAM lParam;
#endif /* __STDC__ */
{
	int  index = 0;
	UINT  i = 0;

	static HWND wndListRule;
	static int  itemIndex;
	static char szBuffer [MAX_BUFF];

    switch (msg) {
	       case WM_INITDIALOG:
				wndListRule = CreateWindow ("listbox", NULL, WS_CHILD | WS_VISIBLE | LBS_STANDARD,
					                         10, 10, 200, 130, hwnDlg, (HMENU) 1, 
											 (HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);

	            SendMessage (wndListRule, LB_RESETCONTENT, 0, 0);
	            while (i < nbClass && classList[index] != '\0') {
	                  SendMessage (wndListRule, LB_INSERTSTRING, i, (LPARAM) &classList[index]);  
	                  index += strlen (&classList[index]) + 1;	/* Longueur de l'intitule */
					  i++;
				}
				break;

		   case WM_COMMAND:
				if (LOWORD (wParam) == 1 && HIWORD (wParam) == LBN_SELCHANGE) {
				   itemIndex = SendMessage (wndListRule, LB_GETCURSEL, 0, 0);
				   itemIndex = SendMessage (wndListRule, LB_GETTEXT, itemIndex, (LPARAM) szBuffer);
			       SetDlgItemText (hwnDlg, IDC_EDITRULE, szBuffer);
				   ThotCallback (baseDlg + classSelect, STRING_DATA, szBuffer);
				}

			    switch (LOWORD (wParam)) {
				       case ID_CONFIRM:
						    ThotCallback (baseDlg + classForm, INTEGER_DATA, (char*) 1);
						    EndDialog (hwnDlg, ID_CONFIRM);
							break;

				       case ID_DONE:
						    ThotCallback (baseDlg + classForm, INTEGER_DATA, (char*) 0);
						    EndDialog (hwnDlg, ID_DONE);
							break;

				}
				break;
				default: return FALSE;
	}
	return TRUE ;
}

/*-----------------------------------------------------------------------
 SpellCheckDlgProc
 ------------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK SpellCheckDlgProc (HWND hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK SpellCheckDlgProc (hwnDlg, msg, wParam, lParam)
HWND   hwndParent; 
UINT   msg; 
WPARAM wParam; 
LPARAM lParam;
#endif /* __STDC__ */
{
	HWND wndLabel;
	BOOL ok;	  
	int  val;

	static int iLocation;
	static int iIgnore;
	static int itemIndex; 

    switch (msg) {
	       case WM_INITDIALOG:
                hwndLanguage = CreateWindow ("STATIC", NULL, WS_CHILD | WS_VISIBLE | SS_LEFT,
                                             13, 10, 150, 16, hwnDlg, (HMENU) 2,  
                                             (HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);

			    wndLabel = CreateWindow ("STATIC", currentLabel, WS_CHILD | WS_VISIBLE | SS_LEFT,
					                     13, 29, 150, 16, hwnDlg, (HMENU) 99, 
										 (HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);

			    wordButton = CreateWindow ("BUTTON", NULL, WS_CHILD | BS_DEFPUSHBUTTON | WS_VISIBLE,
                                            13, 48, 150, 20, hwnDlg, IDC_WORDBUTTON, 
											(HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);

				hwnListWords = CreateWindow ("listbox", NULL, WS_CHILD | WS_VISIBLE | LBS_STANDARD,
					                         13, 72, 150, 70, hwnDlg, (HMENU) 1, 
											 (HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);

				hwndCurrentWord = CreateWindow ("EDIT", NULL, WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER,
					                            13, 146, 150, 20, hwnDlg, (HMENU) IDC_LANGEDIT, 
											 (HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);
 

				CheckRadioButton (hwnDlg, IDC_BEFORE, IDC_WHOLEDOC, IDC_AFTER);
			    SetDlgItemInt (hwnDlg, IDC_EDITPROPOSALS, 3, FALSE);
				SetDlgItemText (hwnDlg, IDC_EDITIGNORE, currentRejectedchars); 
				iLocation = 2;
                SetWindowText (hwndCurrentWord, "");
				WIN_DisplayWords ();
			    break;

		   case WM_COMMAND:
				if (LOWORD (wParam) == 1 && HIWORD (wParam) == LBN_SELCHANGE) {
				   itemIndex = SendMessage (hwnListWords, LB_GETCURSEL, 0, 0);
				   itemIndex = SendMessage (hwnListWords, LB_GETTEXT, itemIndex, (LPARAM) currentWord);
			       SetDlgItemText (hwnDlg, IDC_LANGEDIT, currentWord);
				}

                if (HIWORD (wParam) == EN_UPDATE) {
				   if (LOWORD (wParam) == IDC_EDITPROPOSALS) {
					  val = GetDlgItemInt (hwnDlg, IDC_EDITPROPOSALS, &ok, TRUE);
                      if (ok)
                         ThotCallback (SpellingBase + ChkrCaptureNC, INTEGER_DATA, (char*) val);
				   } else if (LOWORD (wParam) == IDC_EDITIGNORE) {
                          GetDlgItemText (hwnDlg, IDC_EDITIGNORE, currentRejectedchars, sizeof (currentRejectedchars) + 1);
                          ThotCallback (SpellingBase + ChkrSpecial, STRING_DATA, currentRejectedchars);
                   } else if (LOWORD (wParam) == IDC_LANGEDIT) 
                          GetDlgItemText (hwnDlg, IDC_LANGEDIT, currentWord, sizeof (currentWord) + 1);
                }

			    switch (LOWORD (wParam)) {
				       case IDC_BEFORE:
						    iLocation = 0;
						    break;
					   case IDC_WITHIN:
						    iLocation = 1;
						    break;
					   case IDC_AFTER:
						    iLocation = 2;
						    break;
					   case IDC_WHOLEDOC:
						    iLocation = 3;
						    break;
					   case IDC_IGNORE1:
                            ThotCallback (SpellingBase + ChkrMenuIgnore, INTEGER_DATA, (char*) 0);
						    break;
					   case IDC_IGNORE2:
                            ThotCallback (SpellingBase + ChkrMenuIgnore, INTEGER_DATA, (char*) 1);
						    break;
					   case IDC_IGNORE3: 
                            ThotCallback (SpellingBase + ChkrMenuIgnore, INTEGER_DATA, (char*) 2);
						    break;
					   case IDC_IGNORE4:
                            ThotCallback (SpellingBase + ChkrMenuIgnore, INTEGER_DATA, (char*) 3);
						    break;
					   case ID_SKIPNEXT:
                            /*
						    if (strcmp (ChkrCorrection[1], "$") != 0)
						       ThotCallback (SpellingBase + ChkrSelectProp, STRING_DATA, ChkrCorrection[1]);
							else 
						       ThotCallback (SpellingBase + ChkrSelectProp, STRING_DATA, "");
                            */
                            ThotCallback (SpellingBase + ChkrSelectProp, STRING_DATA, currentWord);
							ThotCallback (SpellingBase + ChkrMenuOR, INTEGER_DATA, (char*) iLocation);
							ThotCallback (SpellingBase + ChkrFormCorrect, INTEGER_DATA, (char*) 1);
							if (iLocation == 3) {
				               CheckRadioButton (hwnDlg, IDC_BEFORE, IDC_WHOLEDOC, IDC_AFTER);
							   iLocation = 2;
							}
						    break;
					   case ID_SKIPDIC:
                            ThotCallback (SpellingBase + ChkrSelectProp, STRING_DATA, currentWord);
							ThotCallback (SpellingBase + ChkrMenuOR, INTEGER_DATA, (char*) iLocation);
							ThotCallback (SpellingBase + ChkrFormCorrect, INTEGER_DATA, (char*) 2);
							if (iLocation == 3) {
				               CheckRadioButton (hwnDlg, IDC_BEFORE, IDC_WHOLEDOC, IDC_AFTER);
							   iLocation = 2;
							}
						    break;
					   case ID_REPLACENEXT:
                            ThotCallback (SpellingBase + ChkrSelectProp, STRING_DATA, currentWord);
							ThotCallback (SpellingBase + ChkrMenuOR, INTEGER_DATA, (char*) iLocation);
							ThotCallback (SpellingBase + ChkrFormCorrect, INTEGER_DATA, (char*) 3);
							if (iLocation == 3) {
				               CheckRadioButton (hwnDlg, IDC_BEFORE, IDC_WHOLEDOC, IDC_AFTER);
							   iLocation = 2;
							}
						    break;
					   case ID_REPLACEDIC:
                            ThotCallback (SpellingBase + ChkrSelectProp, STRING_DATA, currentWord);
							ThotCallback (SpellingBase + ChkrMenuOR, INTEGER_DATA, (char*) iLocation);
							ThotCallback (SpellingBase + ChkrFormCorrect, INTEGER_DATA, (char*) 4);
							if (iLocation == 3) {
				               CheckRadioButton (hwnDlg, IDC_BEFORE, IDC_WHOLEDOC, IDC_AFTER);
							   iLocation = 2;
							}
						    break;
					   case IDC_WORDBUTTON:
                            GetWindowText (wordButton, currentWord, MAX_WORD_LEN) ;
                            SetWindowText (hwndCurrentWord, currentWord);
						    break;
					   case ID_DONE:
						    EndDialog (hwnDlg, ID_DONE);
							break;
				}
				break;
		   default: return FALSE;
	}
	return TRUE;
}

/*-----------------------------------------------------------------------
 CloseDocDlgProc
 ------------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK InitConfirmDlgProc (HWND hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK InitConfirmDlgProc (hwnDlg, msg, wParam, lParam)
HWND   hwndParent; 
UINT   msg; 
WPARAM wParam; 
LPARAM lParam;
#endif /* __STDC__ */
{
	HWND messageWnd;
    switch (msg) {
	       case WM_INITDIALOG:
			    SetWindowText (hwnDlg, wndTitle);
				messageWnd = CreateWindow ("STATIC", message, WS_CHILD | WS_VISIBLE | SS_LEFT,
					                       15, 15, 303, 60, hwnDlg, (HMENU) 99, 
										   (HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);
				break;

		   case WM_COMMAND:
			    switch (LOWORD (wParam)) {
		               case ID_CONFIRM:
			                EndDialog (hwnDlg, ID_CONFIRM);
				            ThotCallback (currentRef, INTEGER_DATA, (char*) 1);
			                break;

		               case IDCANCEL:
			                EndDialog (hwnDlg, IDCANCEL);
				            break;
				}
				break;

				default: return FALSE;
	}
	return TRUE;
}

/*-----------------------------------------------------------------------
 ChangeFormatDlgProc
 ------------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK ChangeFormatDlgProc (HWND hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK ChangeFormatDlgProc (hwnDlg, msg, wParam, lParam)
HWND   hwndParent; 
UINT   msg; 
WPARAM wParam; 
LPARAM lParam;
#endif /* __STDC__ */
{
    switch (msg) {
	       case WM_INITDIALOG:
			    break;

		   case WM_COMMAND:
			    switch (LOWORD (wParam)) {
					   /* Alignement menu */
				       case ID_APPLY:
						    ThotCallback (NumFormPresFormat, INTEGER_DATA, (char*) 1);
							break;

					   case ID_DONE:
						    ThotCallback (NumFormPresFormat, INTEGER_DATA, (char*) 0);
						    EndDialog (hwnDlg, ID_DONE);
							break;

					   case IDC_LEFT:
						    ThotCallback (NumMenuAlignment, INTEGER_DATA, (char*) 0);
							break;

					   case IDC_RIGHT:
						    ThotCallback (NumMenuAlignment, INTEGER_DATA, (char*) 1);
							break;

					   case IDC_CENTER:
						    ThotCallback (NumMenuAlignment, INTEGER_DATA, (char*) 2);
							break;

					   case IDC_DEFAULTALIGN:
						    ThotCallback (NumMenuAlignment, INTEGER_DATA, (char*) 3);
							break;

					   /* Jusitification menu */
					   case IDC_JUSTIFYES:
						    ThotCallback (NumMenuJustification, INTEGER_DATA, (char*) 0);
							break;

					   case IDC_JUSTIFNO:
						    ThotCallback (NumMenuJustification, INTEGER_DATA, (char*) 1);
							break;

					   case IDC_JUSTIFDEFAULT:
						    ThotCallback (NumMenuJustification, INTEGER_DATA, (char*) 2);
							break;

					   /* Indent Menu */ 
					   case IDC_INDENT1:
						    ThotCallback (NumMenuRecessSense, INTEGER_DATA, (char*) 0);
							break;

					   case IDC_INDENT2:
						    ThotCallback (NumMenuRecessSense, INTEGER_DATA, (char*) 1);
							break;

					   case IDC_INDENTDEFAULT:
						    ThotCallback (NumMenuRecessSense, INTEGER_DATA, (char*) 2);
							break;

					   /* Line spacing menu */
					   case IDC_SSMALL:
						    ThotCallback (NumMenuLineSpacing, INTEGER_DATA, (char*) 0);
							break;

					   case IDC_SMEDIUM:
						    ThotCallback (NumMenuLineSpacing, INTEGER_DATA, (char*) 1);
							break;

					   case IDC_SLARGE:
						    ThotCallback (NumMenuLineSpacing, INTEGER_DATA, (char*) 2);
							break;

					   case IDC_SPACINGDEFAULT:
						    ThotCallback (NumMenuLineSpacing, INTEGER_DATA, (char*) 3);
							break;
				}
				break;

				default: return FALSE;
	}
	return TRUE;
}

/*-----------------------------------------------------------------------
 AuthenticationDlgProc
 ------------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK GreekKeyboardDlgProc (HWND hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK GreekKeyboardDlgProc (hwnDlg, msg, wParam, lParam)
HWND   hwndParent; 
UINT   msg; 
WPARAM wParam; 
LPARAM lParam;
#endif /* __STDC__ */
{
	int    car;
    switch (msg) {
		   case WM_COMMAND:
			    switch (LOWORD (wParam)) {
		               case IDCANCEL:
			                EndDialog (hwnDlg, IDCANCEL);
				            break;

		               case IDC_GKEY001:
                            car = 800;
				            break;

		               case IDC_GKEY002:
                            car = 801;
				            break;

		               case IDC_GKEY003:
                            car = 802;
				            break;

		               case IDC_GKEY004:
                            car = 803;
				            break;

		               case IDC_GKEY005:
                            car = 804;
				            break;

		               case IDC_GKEY006:
                            car = 805;
				            break;

		               case IDC_GKEY007:
                            car = 806;
				            break;

		               case IDC_GKEY008:
                            car = 807;
				            break;

		               case IDC_GKEY009:
                            car = 808;
				            break;

		               case IDC_GKEY010:
                            car = 809;
				            break;

		               case IDC_GKEY011:
                            car = 810;
				            break;

		               case IDC_GKEY012:
                            car = 811;
				            break;

		               case IDC_GKEY013:
                            car = 812;
				            break;

		               case IDC_GKEY014:
                            car = 813;
				            break;

		               case IDC_GKEY015:
                            car = 814;
				            break;

		               case IDC_GKEY016:
                            car = 815;
				            break;

		               case IDC_GKEY017:
                            car = 816;
				            break;

		               case IDC_GKEY018:
                            car = 817;
				            break;

		               case IDC_GKEY019:
                            car = 818;
				            break;

		               case IDC_GKEY020:
                            car = 819;
				            break;

		               case IDC_GKEY021:
                            car = 820;
				            break;

		               case IDC_GKEY022:
                            car = 821;
				            break;

		               case IDC_GKEY023:
                            car = 822;
				            break;

		               case IDC_GKEY024:
                            car = 823;
				            break;

		               case IDC_GKEY025:
                            car = 824;
				            break;

		               case IDC_GKEY026:
                            car = 825;
				            break;

		               case IDC_GKEY027:
                            car = 826;
				            break;

		               case IDC_GKEY028:
                            car = 827;
				            break;

		               case IDC_GKEY029:
                            car = 828;
				            break;

		               case IDC_GKEY030:
                            car = 829;
				            break;

		               case IDC_GKEY031:
                            car = 830;
				            break;

		               case IDC_GKEY032:
                            car = 831;
				            break;

		               case IDC_GKEY033:
                            car = 832;
				            break;

		               case IDC_GKEY034:
                            car = 833;
				            break;

		               case IDC_GKEY035:
                            car = 834;
				            break;

		               case IDC_GKEY036:
                            car = 835;
				            break;

		               case IDC_GKEY037:
                            car = 836;
				            break;

		               case IDC_GKEY038:
                            car = 837;
				            break;

		               case IDC_GKEY039:
                            car = 838;
				            break;

		               case IDC_GKEY040:
                            car = 839;
				            break;

		               case IDC_GKEY041:
                            car = 840;
				            break;

		               case IDC_GKEY042:
                            car = 841;
				            break;

		               case IDC_GKEY043:
                            car = 842;
				            break;

		               case IDC_GKEY044:
                            car = 843;
				            break;

		               case IDC_GKEY045:
                            car = 844;
				            break;

		               case IDC_GKEY046:
                            car = 845;
				            break;

		               case IDC_GKEY047:
                            car = 846;
				            break;

		               case IDC_GKEY048:
                            car = 847;
				            break;

		               case IDC_GKEY049:
                            car = 848;
				            break;

		               case IDC_GKEY050:
                            car = 849;
				            break;

		               case IDC_GKEY051:
                            car = 850;
				            break;

		               case IDC_GKEY052:
                            car = 851;
				            break;

		               case IDC_GKEY053:
                            car = 852;
				            break;

		               case IDC_GKEY054:
                            car = 853;
				            break;

		               case IDC_GKEY055:
                            car = 854;
				            break;

		               case IDC_GKEY056:
                            car = 855;
				            break;

		               case IDC_GKEY057:
                            car = 856;
				            break;

		               case IDC_GKEY058:
                            car = 857;
				            break;

		               case IDC_GKEY059:
                            car = 858;
				            break;

		               case IDC_GKEY060:
                            car = 859;
				            break;

		               case IDC_GKEY061:
                            car = 860;
				            break;

		               case IDC_GKEY062:
                            car = 861;
				            break;

		               case IDC_GKEY063:
                            car = 862;
				            break;

		               case IDC_GKEY064:
                            car = 863;
				            break;

		               case IDC_GKEY065:
                            car = 865;
				            break;

		               case IDC_GKEY066:
                            car = 866;
				            break;

		               case IDC_GKEY067:
                            car = 867;
				            break;

		               case IDC_GKEY068:
                            car = 868;
				            break;

		               case IDC_GKEY069:
                            car = 869;
				            break;

		               case IDC_GKEY070:
                            car = 870;
				            break;

		               case IDC_GKEY071:
                            car = 871;
				            break;

		               case IDC_GKEY072:
                            car = 872;
				            break;

		               case IDC_GKEY073:
                            car = 873;
				            break;

		               case IDC_GKEY074:
                            car = 874;
				            break;

		               case IDC_GKEY075:
                            car = 875;
				            break;

		               case IDC_GKEY076:
                            car = 876;
				            break;

		               case IDC_GKEY077:
                            car = 877;
				            break;

		               case IDC_GKEY078:
                            car = 878;
				            break;

		               case IDC_GKEY079:
                            car = 879;
				            break;

		               case IDC_GKEY080:
                            car = 880;
				            break;

		               case IDC_GKEY081:
                            car = 881;
				            break;

		               case IDC_GKEY082:
                            car = 882;
				            break;

		               case IDC_GKEY083:
                            car = 883;
				            break;

		               case IDC_GKEY084:
                            car = 884;
				            break;

		               case IDC_GKEY085:
                            car = 885;
				            break;

		               case IDC_GKEY086:
                            car = 886;
				            break;

		               case IDC_GKEY087:
                            car = 887;
				            break;

		               case IDC_GKEY088:
                            car = 888;
				            break;

		               case IDC_GKEY089:
                            car = 889;
				            break;

		               case IDC_GKEY090:
                            car = 890;
				            break;

		               case IDC_GKEY091:
                            car = 891;
				            break;

		               case IDC_GKEY092:
                            car = 892;
				            break;

		               case IDC_GKEY093:
                            car = 893;
				            break;

		               case IDC_GKEY094:
                            car = 894;
				            break;

		               case IDC_GKEY095:
                            car = 929;
				            break;

		               case IDC_GKEY096:
                            car = 930;
				            break;

		               case IDC_GKEY097:
                            car = 931;
				            break;

		               case IDC_GKEY098:
                            car = 932;
				            break;

		               case IDC_GKEY099:
                            car = 933;
				            break;

		               case IDC_GKEY100:
                            car = 934;
				            break;

		               case IDC_GKEY101:
                            car = 935;
				            break;

		               case IDC_GKEY102:
                            car = 936;
				            break;

		               case IDC_GKEY103:
                            car = 937;
				            break;

		               case IDC_GKEY104:
                            car = 938;
				            break;

		               case IDC_GKEY105:
                            car = 939;
				            break;

		               case IDC_GKEY106:
                            car = 940;
				            break;

		               case IDC_GKEY107:
                            car = 941;
				            break;

		               case IDC_GKEY108:
                            car = 942;
				            break;

		               case IDC_GKEY109:
                            car = 943;
				            break;

		               case IDC_GKEY110:
                            car = 944;
				            break;

		               case IDC_GKEY111:
                            car = 945;
				            break;

		               case IDC_GKEY112:
                            car = 946;
				            break;

		               case IDC_GKEY113:
                            car = 947;
				            break;

		               case IDC_GKEY114:
                            car = 948;
				            break;

		               case IDC_GKEY115:
                            car = 949;
				            break;

		               case IDC_GKEY116:
                            car = 950;
				            break;

		               case IDC_GKEY117:
                            car = 951;
				            break;

		               case IDC_GKEY118:
                            car = 952;
				            break;

		               case IDC_GKEY119:
                            car = 953;
				            break;

		               case IDC_GKEY120:
                            car = 954;
				            break;

		               case IDC_GKEY121:
                            car = 955;
				            break;

		               case IDC_GKEY122:
                            car = 956;
				            break;

		               case IDC_GKEY123:
                            car = 957;
				            break;

		               case IDC_GKEY124:
                            car = 958;
				            break;

		               case IDC_GKEY125:
                            car = 959;
				            break;

		               case IDC_GKEY126:
                            car = 960;
				            break;

		               case IDC_GKEY127:
                            car = 961;
				            break;

		               case IDC_GKEY128:
                            car = 962;
				            break;

		               case IDC_GKEY129:
                            car = 963;
				            break;

		               case IDC_GKEY130:
                            car = 964;
				            break;

		               case IDC_GKEY131:
                            car = 965;
				            break;

		               case IDC_GKEY132:
                            car = 966;
				            break;

		               case IDC_GKEY133:
                            car = 967;
				            break;

		               case IDC_GKEY134:
                            car = 968;
				            break;

		               case IDC_GKEY135:
                            car = 969;
				            break;

		               case IDC_GKEY136:
                            car = 970;
				            break;

		               case IDC_GKEY137:
                            car = 971;
				            break;

		               case IDC_GKEY138:
                            car = 972;
				            break;

		               case IDC_GKEY139:
                            car = 973;
				            break;

		               case IDC_GKEY140:
                            car = 974;
				            break;

		               case IDC_GKEY141:
                            car = 975;
				            break;

		               case IDC_GKEY142:
                            car = 976;
				            break;

		               case IDC_GKEY143:
                            car = 977;
				            break;

		               case IDC_GKEY144:
                            car = 980;
				            break;

		               case IDC_GKEY145:
                            car = 981;
				            break;

		               case IDC_GKEY146:
                            car = 982;
				            break;

		               case IDC_GKEY147:
                            car = 983;
				            break;

		               case IDC_GKEY148:
                            car = 984;
				            break;

		               case IDC_GKEY149:
                            car = 985;
				            break;

		               case IDC_GKEY150:
                            car = 986;
				            break;

		               case IDC_GKEY151:
                            car = 987;
				            break;

		               case IDC_GKEY152:
                            car = 988;
				            break;

		               case IDC_GKEY153:
                            car = 989;
				            break;

		               case IDC_GKEY154:
                            car = 990;
				            break;

		               case IDC_GKEY155:
                            car = 991;
				            break;

		               case IDC_GKEY156:
                            car = 992;
				            break;

		               case IDC_GKEY157:
                            car = 993;
				            break;

		               case IDC_GKEY158:
                            car = 997;
				            break;

		               case IDC_GKEY159:
                            car = 1009;
				            break;

		               case IDC_GKEY160:
                            car = 1010;
				            break;
				}
				break;

				default: return FALSE;
	}
	KbdCallbackHandler (hwnDlg, car, "\n");
	return TRUE;
}

/*-----------------------------------------------------------------------
 AuthenticationDlgProc
 ------------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK AuthenticationDlgProc (HWND hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK AuthenticationDlgProc (hwnDlg, msg, wParam, lParam)
HWND   hwndParent; 
UINT   msg; 
WPARAM wParam; 
LPARAM lParam;
#endif /* __STDC__ */
{
    switch (msg) {
	       case WM_INITDIALOG:
			    SetDlgItemText (hwnDlg, IDC_NAMEEDIT, "");
			    SetDlgItemText (hwnDlg, IDC_PASSWDEDIT, "");
				break;

		   case WM_COMMAND:
			    switch (LOWORD (wParam)) {
		               case ID_CONFIRM:
						    GetDlgItemText (hwnDlg, IDC_NAMEEDIT, Answer_name, sizeof (Answer_name) + 1);
						    GetDlgItemText (hwnDlg, IDC_PASSWDEDIT, Answer_password, sizeof (Answer_password) + 1);
			                EndDialog (hwnDlg, ID_CONFIRM);
			                break;

		               case IDCANCEL:
			                EndDialog (hwnDlg, IDCANCEL);
				            break;
				}
				break;

				default: return FALSE;
	}
	return TRUE;
}

#endif /* _WINDOWS */