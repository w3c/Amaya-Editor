#ifdef _WINDOWS
#include <windows.h>
#include "resource.h"

extern HINSTANCE hInstance;

static char   urlToOpen [256];
static char   message [300];
static char   wndTitle [100];

static int  currentDoc ;
static int  currentView ;
static BOOL	saveBeforeClose ;
static BOOL closeDontSave ;

#ifdef __STDC__
LRESULT CALLBACK LinkDlgProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK PrintDlgProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK SearchDlgProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK CloseDocDlgProc (HWND, UINT, WPARAM, LPARAM);
#else  /* !__STDC__ */
LRESULT CALLBACK LinkDlgProc ();
LRESULT CALLBACK PrintDlgProc ();
LRESULT CALLBACK SearchDlgProc ();
LRESULT CALLBACK CloseDocDlgProc ();
#endif /* __STDC__ */

/*-----------------------------------------------------------------------
 CreateLinkDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void CreateLinkDlgWindow (HWND parent)
#else  /* !__STDC__ */
void CreateLinkDlgWindow (parent)
HWND      parent;
#endif /* __STDC__ */
{  
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
			    SetDlgItemText (hwnDlg, IDC_URLEDIT, "");
			    break;
           case WM_COMMAND:
	            switch (LOWORD (wParam)) {
				       case ID_CONFIRM:
						    GetDlgItemText (hwnDlg, IDC_URLEDIT, urlToOpen, sizeof (urlToOpen) - 1);
					        EndDialog (hwnDlg, ID_CONFIRM);
			                MessageBox (hwnDlg, urlToOpen, "Open URL", MB_OK);
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
    switch (msg) {
	       case WM_INITDIALOG:
			    SetDlgItemText (hwnDlg, IDC_SEARCHEDIT, "");
			    SetDlgItemText (hwnDlg, IDC_REPLACEDIT, "");
				break;
		   case WM_COMMAND:
			    switch (LOWORD (wParam)) {
				       case ID_CONFIRM:
				       case ID_NOREPLACE:
				       case ID_DONE:
					        EndDialog (hwnDlg, ID_DONE);
							break;
				}
				break;
				default: return FALSE;
	}
	return TRUE ;
}

/*-----------------------------------------------------------------------
 CallbackCloseDocMenu
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
					                       15, 15, 200, 15, hwnDlg, (HMENU) 99, hInstance, NULL);
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
#endif /* _WINDOWS */