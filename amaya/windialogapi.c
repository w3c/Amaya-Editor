/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * WIndows Dialogue API routines for Amaya
 *
 * Author: R. Guetari (W3C/INRIA): Windows NT/95 routines
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

#ifdef  APPFILENAMEFILTER
#       undef  APPFILENAMEFILTER
#endif  /* APPFILENAMEFILTER */

#define APPFILENAMEFILTER   "HTML Files (*.html)\0*.html\0HTML Files (*.htm)\0*.htm\0All files (*.*)\0*.*\0"

extern HINSTANCE hInstance;

static char   urlToOpen [256];
static char   message [300];
static char   wndTitle [100];

static int          currentDoc ;
static int          currentView ;
static BOOL	        saveBeforeClose ;
static BOOL         closeDontSave ;
static OPENFILENAME OpenFileName;

#ifdef __STDC__
LRESULT CALLBACK LinkDlgProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK PrintDlgProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK SearchDlgProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK OpenDocDlgProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK CloseDocDlgProc (HWND, UINT, WPARAM, LPARAM);
#else  /* !__STDC__ */
LRESULT CALLBACK LinkDlgProc ();
LRESULT CALLBACK PrintDlgProc ();
LRESULT CALLBACK SearchDlgProc ();
LRESULT CALLBACK OpenDocDlgProc ();
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
	                        /* create an attribute HREF for the Link_Anchor */
	                        attrType.AttrSSchema = TtaGetDocumentSSchema (AttrHREFdocument);
	                        attrType.AttrTypeNum = HTML_ATTR_HREF_;
	                        attrHREF = TtaGetAttribute (AttrHREFelement, attrType);
	                        if (attrHREF == 0) {
	                           /* create an attribute HREF for the element */
	                           attrHREF = TtaNewAttribute (attrType);
	                           TtaAttachAttribute (AttrHREFelement, attrHREF, AttrHREFdocument);
	                        }
	                        TtaSetAttributeText (attrHREF, urlToOpen, AttrHREFelement, AttrHREFdocument);
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
    switch (msg) {
	       case WM_INITDIALOG:
			    SetDlgItemText (hwnDlg, IDC_SEARCHEDIT, "");
			    SetDlgItemText (hwnDlg, IDC_REPLACEDIT, "");

				iMode     = IDC_NOREPLACE ;
				iLocation = IDC_WHOLEDOC;

				CheckRadioButton (hwnDlg, IDC_NOREPLACE, IDC_AUTOMATIC, iMode);
				CheckRadioButton (hwnDlg, IDC_BEFORE, IDC_WHOLEDOC, iLocation);

				break;
		   case WM_COMMAND:
			    switch (LOWORD (wParam)) {
				       case ID_CONFIRM:
				       case ID_NOREPLACE:
				       case ID_DONE:
					        EndDialog (hwnDlg, ID_DONE);
							break;
					   case IDC_NOREPLACE:
					   case IDC_ONREQUEST:
					   case IDC_AUTOMATIC:
						    iMode = LOWORD (wParam) ;
							CheckRadioButton (hwnDlg, IDC_NOREPLACE, IDC_AUTOMATIC, LOWORD (wParam));
							break;
					   case IDC_BEFORE:
					   case IDC_WITHIN:
					   case IDC_AFTER:
					   case IDC_WHOLEDOC:
						    iLocation = LOWORD (wParam);
							CheckRadioButton (hwnDlg, IDC_BEFORE, IDC_WHOLEDOC, LOWORD (wParam));
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
#endif /* _WINDOWS */