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

#define APPFILENAMEFILTER   "HTML Files (*.html)\0*.html\0HTML Files (*.htm)\0*.htm\0Image files (*.gif)\0*.gif\0Image files (*.jpg)\0*.jpg\0Image files (*.png)\0*.png\0Image files (*.bmp)\0*.bmp\0All files (*.*)\0*.*\0"

extern HINSTANCE hInstance;
extern char*     AttrHREFvalue;

static char   urlToOpen [256];
static char   message [300];
static char   wndTitle [100];

static int          currentDoc ;
static int          currentView ;
static int          currentRef;
static BOOL	        saveBeforeClose ;
static BOOL         closeDontSave ;
static OPENFILENAME OpenFileName;

#ifdef __STDC__
LRESULT CALLBACK LinkDlgProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK PrintDlgProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK SearchDlgProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK OpenDocDlgProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK CloseDocDlgProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK CharacterDlgProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK InitConfirmDlgProc (HWND, UINT, WPARAM, LPARAM);
#else  /* !__STDC__ */
LRESULT CALLBACK LinkDlgProc ();
LRESULT CALLBACK PrintDlgProc ();
LRESULT CALLBACK SearchDlgProc ();
LRESULT CALLBACK OpenDocDlgProc ();
LRESULT CALLBACK CloseDocDlgProc ();
LRESULT CALLBACK CharacterDlgProc ();
LRESULT CALLBACK InitConfirmDlgProc ();
#endif /* __STDC__ */

/*-----------------------------------------------------------------------
 CreateLinkDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void CreateLinkDlgWindow (HWND parent, int ref)
#else  /* !__STDC__ */
void CreateLinkDlgWindow (parent, ref)
HWND      parent;
int       ref;
#endif /* __STDC__ */
{  
	currentRef = ref;
	DialogBox (hInstance, MAKEINTRESOURCE (LINKDIALOG), parent, (DLGPROC) LinkDlgProc);
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
 CreateOPenDocDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void CreateOPenDocDlgWindow (HWND parent, char* doc_to_open)
#else  /* !__STDC__ */
void CreateOPenDocDlgWindow (parent, doc_to_open)
HWND  parent;
char* doc_to_open;
#endif /* __STDC__ */
{  
	DialogBox (hInstance, MAKEINTRESOURCE (OPENDOCDIALOG), parent, (DLGPROC) OpenDocDlgProc);
	strcpy (doc_to_open, urlToOpen);
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
 CreateInitConfirmDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void CreateInitConfirmDlgWindow (HWND parent, int ref, char* title, char* msg)
#else  /* !__STDC__ */
void CreateInitConfirmDlgWindow (parent, title, msg)
HWND  parent;
char* msg;
#endif /* __STDC__ */
{  
	sprintf (message, msg);
	sprintf (wndTitle, title);
	currentRef = ref;
	DialogBox (hInstance, MAKEINTRESOURCE (INITCONFIRMDIALOG), parent, (DLGPROC) InitConfirmDlgProc);
}

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
    AttributeType attrType;
    Attribute     attrHREF;
    switch (msg) {
           case WM_INITDIALOG:
			    SetDlgItemText (hwnDlg, IDC_URLEDIT, "");
			    break;

           case WM_COMMAND:
	            switch (LOWORD (wParam)) {
				       case ID_CONFIRM:
						    GetDlgItemText (hwnDlg, IDC_URLEDIT, urlToOpen, sizeof (urlToOpen) - 1);
							AttrHREFvalue = (char*) TtaGetMemory (strlen (urlToOpen) + 1);
							strcpy (AttrHREFvalue, urlToOpen);
							CallbackDialogue (currentRef, INTEGER_DATA, (char*) 1);
							EndDialog (hwnDlg, ID_CONFIRM);
					        break;

				       case ID_DONE:
					        EndDialog (hwnDlg, ID_DONE);
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
							WIN_ListOpenDirectory (hwnDlg, urlToOpen);
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
					                       15, 15, 400, 60, hwnDlg, (HMENU) 99, hInstance, NULL);
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
 CloseDocDlgProc
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
					                       15, 15, 303, 60, hwnDlg, (HMENU) 99, hInstance, NULL);
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
#endif /* _WINDOWS */