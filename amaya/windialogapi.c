/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Windows Dialogue API routines for Amaya
 *
 * Authors:  I. Vatton (INRIA), R. Guetari (W3C/INRIA), J. Kahan (W3C/INRIA)
 *
 */
#ifdef _WINDOWS
#include <windows.h>
#include "resource.h"
#include "constmedia.h"
#include "corrmsg.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "constmenu.h"
#include "wininclude.h"

extern ThotWindow  FrRef[MAX_FRAME + 2];
extern int         currentFrame;
extern ThotBool    searchEnd;
#ifdef  APPFILENAMEFILTER
#undef  APPFILENAMEFILTER
#endif  /* APPFILENAMEFILTER */

#ifndef MAX_PROPOSAL_CHKR
#define MAX_PROPOSAL_CHKR 10
#endif /* MAX_PROPOSAL_CHKR */

#ifndef MAX_WORD_LEN
#define MAX_WORD_LEN 30
#endif /* MAX_WORD_LEN */

#define APPFILENAMEFILTER    TEXT("HTML Files (*.htm[l])\0*.htm*\0XML Files (*.xml)\0*.xml\0All files (*.*)\0*.*\0")
#define APPIMAGENAMEFILTER   TEXT("Image files (*.gif)\0*.gif\0Image files (*.jpg)\0*.jpg\0Image files (*.png)\0*.png\0Image files (*.bmp)\0*.bmp\0All files (*.*)\0*.*\0")
#define APPALLFILESFILTER    TEXT("All files (*.*)\0*.*\0")

#define MAX_BUFF 4096
#define IDC_WORDBUTTON    20000
#define IDC_EDITRULE      20001
#define IDC_LANGEDIT      20002
#define ICD_SPELLWORDEDIT 20003
#define IDC_CSSEDIT       20004

#define OPT1               1110
#define OPT2               1111
#define OPT3               1112
#define OPT4               1113
#define OPT5               1114
#define OPT6               1115
#define OPT7               1116
#define OPT8               1117
#define OPT9               1118

#define REPEAT                0
#define REPEAT_X              1
#define REPEAT_Y              2
#define NO_REPEAT             3

#define IMG_FILE              1
#define TEXT_FILE             2

int                WIN_IndentValue;
int                WIN_OldLineSp;
int                WIN_NormalLineSpacing;

extern HINSTANCE    hInstance;
extern HDC          TtPrinterDC;
extern CHAR_T       DocToOpen [MAX_LENGTH];
extern CHAR_T       WIN_buffMenu [MAX_TXT_LEN];
extern CHAR_T       ChkrCorrection[MAX_PROPOSAL_CHKR+1][MAX_WORD_LEN];
extern int          ClickX, ClickY;
extern int          CORR;
extern ThotBool     TtIsPrinterTrueColor;
extern ThotBool     bUserAbort;
extern ThotBool     WithToC;
extern ThotBool     NumberLinks;
extern ThotBool     PrintURL;
extern ThotBool     IgnoreCSS;

static CHAR_T       urlToOpen [MAX_LENGTH];
static CHAR_T       HrefUrl [MAX_LENGTH];
static CHAR_T       tmpDocName [MAX_LENGTH];
static CHAR_T       altText [MAX_LENGTH];
static CHAR_T       message [300];
static CHAR_T       message2 [300];
static CHAR_T       message3 [300];
static CHAR_T       wndTitle [100];
static CHAR_T       currentLabel [100];
static CHAR_T       currentRejectedchars [100];
static CHAR_T       currentPathName [100];
static CHAR_T       winCurLang [100];
static CHAR_T       currentFileToPrint [MAX_PATH];
static CHAR_T       attDlgTitle [100];
static CHAR_T       mathEntityName[MAX_TXT_LEN];
static CHAR_T       szBuffer[MAX_BUFF];
static CHAR_T*      classList;
static CHAR_T*      langList;
static CHAR_T*      saveList;
static CHAR_T*      cssList;
static CHAR_T       textToSearch [255];
static CHAR_T       newText [255];
static int          currentDoc;
static int          currentView;
static int          currentRef;
static int          SpellingBase;
static int          ChkrSelectProp;
static int          ChkrMenuOR;
static int          ChkrFormCorrect;
static int          ChkrMenuIgnore;
static int          ChkrCaptureNC;
static int          ChkrSpecial;
static int          repeatMode;
static int          fontNum;
static int          fontStyle;
static int          fontWeight;
static int          fontUnderline;
static int          fontSize;
static int          docSelect;
static int          dirSelect;
static int          currAttrVal;
static int          LangValue;
static int          Num_zoneRecess;
static int          Num_zoneLineSpacing;
static int          Align_num;
static int          Indent_value;
static int          Indent_num;
static int          Justification_num;
static int          Old_lineSp;
static int          Line_spacingNum;
static int          attDlgNbItems;
static int          numCols;
static int          numRows;
static int          tBorder;
static int          formAlt;
static int          iLocation;
static int          cxChar;
static int          cyChar;
static int          iMode;

static HDC          hDC;
static HDC          hMemDC;

static HFONT        hFont;
static HFONT        hOldFont;

static ThotBool	    saveBeforeClose;
static ThotBool     closeDontSave;
static ThotBool     selectionFound;
static ThotBool     isHref;
static ThotBool     WithEdit;
static ThotBool     WithCancel;
static ThotBool     WithBorder;
static ThotBool     upper_lower = FALSE;

static OPENFILENAME OpenFileName;
static CHAR_T*      szFilter;
static CHAR_T       szFileName[256];
static CHAR_T       szBuffer [MAX_BUFF];

static ThotWindow         currentDlg;
static ThotWindow         wndCSSList;
static ThotWindow         wndLangList;
static ThotWindow         wndListRule;
static ThotWindow         wndEditRule;
static ThotWindow         EditURLWnd;
static ThotWindow         transURLWnd;
static ThotWindow         copyImgWnd;
static ThotWindow         WndSearchEdit;
static ThotWindow         GraphPal = NULL;
static ThotWindow         MathPal = NULL;
static ThotWindow         GreekPal = NULL;

static UINT         itemIndex;
static UINT         nbClass;
static UINT         nbItem;


static CHAR_T*      string_par1;
static CHAR_T*      string_par2;

static ThotBool     ReleaseFocus;
static ThotBool     initialized = 0;
static PRINTDLG     Pdlg;
static char         text[1024];


ThotWindow          wordButton;
ThotWindow          hwnListWords;
ThotWindow          hwndCurrentWord;
ThotWindow          hwndLanguage;
ThotWindow          ghwndAbort;
ThotWindow          ghwndMain;
ThotWindow          MakeIDHwnd;
CHAR_T              currentWord [MAX_WORD_LEN];
ThotBool            gbAbort;


/* ------------------------------------------------------------------------ *
   GetPrinterDC()
   Call the Windows print dialogue and returns a handle to the DC if 
   successful or NULL otherwise.
  ------------------------------------------------------------------------ */
HDC PASCAL GetPrinterDC ()
{
  LPDEVMODE   lpDevMode;

  /* Display the PRINT dialog box. */
  if (!initialized)
  {
    /* initialize the pinter context */
    memset(&Pdlg, 0, sizeof(PRINTDLG));
    Pdlg.lStructSize = sizeof(PRINTDLG);
	Pdlg.nCopies = 1;
    Pdlg.Flags       = PD_RETURNDC;
    Pdlg.hInstance   = (HANDLE) NULL;
	initialized = TRUE;
  }

  Pdlg.hwndOwner   = GetCurrentWindow ();
  if (PrintDlg (&Pdlg))
  {
    if (Pdlg.hDevMode)
	{
      lpDevMode = (LPDEVMODE) GlobalLock (Pdlg.hDevMode);
	  if (lpDevMode->dmOrientation == DMORIENT_LANDSCAPE)
	    /* landscape */
	    ThotCallback (BasePrint + PaperOrientation, INTEGER_DATA, (CHAR_T*) 1);
	  else
		/* portrait */
	    ThotCallback (BasePrint + PaperOrientation, INTEGER_DATA, (CHAR_T*) 0);
	  if (lpDevMode->dmPaperSize == DMPAPER_A4)
		/* A4 */
	    ThotCallback (BasePrint + PaperFormat, INTEGER_DATA, (CHAR_T*) 0);
	  else
		/* US */
	    ThotCallback (BasePrint + PaperFormat, INTEGER_DATA, (CHAR_T*) 1);
	  GlobalUnlock (Pdlg.hDevMode);
	}
    return (Pdlg.hDC);
  }
  else
    return NULL;
}


/* ------------------------------------------------------------------------ *
   ReusePrinterDC()
   Call the Windows print dialogue or reuse the previous context.
  ------------------------------------------------------------------------ */
void ReusePrinterDC ()
{
  LPDEVNAMES  lpDevNames;
  LPDEVMODE   lpDevMode;
  LPSTR       lpDriverName, lpDeviceName, lpPortName;

  /* Display the PRINT dialog box. */
  if (!initialized)
    TtPrinterDC = GetPrinterDC();
  else if (Pdlg.hDevNames)
  {
    lpDevNames = (LPDEVNAMES) GlobalLock (Pdlg.hDevNames);
	lpDriverName = (LPSTR) lpDevNames + lpDevNames->wDriverOffset;
	lpDeviceName = (LPSTR) lpDevNames + lpDevNames->wDeviceOffset;
	lpPortName = (LPSTR) lpDevNames + lpDevNames->wOutputOffset;
	GlobalUnlock (Pdlg.hDevNames);
    if (Pdlg.hDevMode)
	{
      lpDevMode = (LPDEVMODE) GlobalLock (Pdlg.hDevMode);
	  TtPrinterDC = CreateDC (lpDriverName, lpDeviceName, lpPortName, lpDevMode);
	  GlobalUnlock (Pdlg.hDevMode);
	}
  }
  
  if (TtPrinterDC)
  {
	WinInitPrinterColors ();    
	EnableWindow (ghwndMain, FALSE);
	ThotCallback (BasePrint + PPrinterName, STRING_DATA, currentFileToPrint);
	ThotCallback (BasePrint + FormPrint, INTEGER_DATA, (CHAR_T*)1);
	if (TtPrinterDC)
	{
	  DeleteDC (TtPrinterDC);
	  TtPrinterDC = NULL;
	}
  }
}

/* ----------------------------------------------------------------------*/
/* ----------------------------------------------------------------------*/
#ifdef __STDC__
void WinInitPrinterColors (void)
#else /* __STDC__ */
void WinInitPrinterColors ()
#endif /* __STDC__ */
{
   int        palSize;

   if (!initialized)
   {
     palSize = GetDeviceCaps (TtPrinterDC, SIZEPALETTE);
     if (palSize == 0)
       TtIsPrinterTrueColor = TRUE;
     else  
       TtIsPrinterTrueColor = FALSE;
     initialized = TRUE;
   }
}


        /*********************************************************
         *                                                       *
         *                   C A L L B A C K S                   *
         *                                                       *
         *********************************************************/


/*-----------------------------------------------------------------------
 AltDlgProc
 ------------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK AltDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK AltDlgProc (hwnDlg, msg, wParam, lParam)
ThotWindow   hwndParent;
UINT   msg;
WPARAM wParam;
LPARAM lParam;
#endif /* __STDC__ */
{
  switch (msg)
    {
    case WM_INITDIALOG:
      SetWindowText (hwnDlg, TtaGetMessage (AMAYA, AM_ALT));
      SetWindowText (GetDlgItem (hwnDlg, IDC_ALTTEXT), TtaGetMessage (AMAYA, AM_ALT));
      SetWindowText (GetDlgItem (hwnDlg, ID_CONFIRM), TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
      SetWindowText (GetDlgItem (hwnDlg, ID_DONE), TtaGetMessage (LIB, TMSG_DONE));
      SetDlgItemText (hwnDlg, IDC_GETALT, TEXT(""));
      break;
      
    case WM_COMMAND:
#ifdef IV
      if (HIWORD (wParam) == EN_UPDATE)
	{
	  if (LOWORD (wParam) == IDC_GETALT)
	    {
	      GetDlgItemText (hwnDlg, IDC_GETALT, altText, sizeof (altText) - 1);
	      ThotCallback (BaseImage + ImageAlt, STRING_DATA, altText);
	    }
	}
#endif /* IV */
      switch (LOWORD (wParam))
	{
	case ID_CONFIRM:
	case ID_DONE:
	  GetDlgItemText (hwnDlg, IDC_GETALT, altText, sizeof (altText) - 1);
	  ThotCallback (BaseImage + ImageAlt, STRING_DATA, altText);
	  if (!altText || altText [0] == 0)
	    MessageBox (hwnDlg, TtaGetMessage (AMAYA, AM_ALT_MISSING),
			TtaGetMessage (AMAYA, AM_ALT), MB_OK | MB_ICONERROR);
	  else 
	    EndDialog (hwnDlg, ID_CONFIRM);
	  ThotCallback (BaseImage + FormAlt, STRING_DATA, (CHAR_T *)1);
	  break;
	}
      break;
    default:
      return FALSE;
    }
  return TRUE;
}

/*-----------------------------------------------------------------------
 CSSDlgProc
 ------------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK CSSDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK CSSDlgProc (hwnDlg, msg, wParam, lParam)
ThotWindow   hwndParent;
UINT   msg;
WPARAM wParam;
LPARAM lParam;
#endif /* __STDC__ */
{
  int     index = 0;
  UINT    i = 0;

  switch (msg)
    {
    case WM_INITDIALOG:
      SetWindowText (hwnDlg, wndTitle);
      SetWindowText (GetDlgItem (hwnDlg, ID_CONFIRM), TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
      SetWindowText (GetDlgItem (hwnDlg, ID_DONE), TtaGetMessage (LIB, TMSG_DONE));
      SetWindowText (GetDlgItem (hwnDlg, IDC_CSSFILES), TtaGetMessage (AMAYA, AM_CSS_FILE));
      wndCSSList = CreateWindow (TEXT("listbox"), NULL,
				 WS_CHILD | WS_VISIBLE | LBS_STANDARD,
				 10, 35, 400, 120, hwnDlg, (HMENU) 1, 
				 (HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE),
				 NULL);

      SendMessage (wndCSSList, LB_RESETCONTENT, 0, 0);
      while (i < nbItem && cssList[index] != EOS)
	{
	  SendMessage (wndCSSList, LB_INSERTSTRING, i, (LPARAM) &cssList[index]); 
	  index += ustrlen (&cssList[index]) + 1;	/* entry length */
	  i++;
	}

      itemIndex = SendMessage (wndCSSList, LB_SETCURSEL, (WPARAM)0, (LPARAM)0);
      SetDlgItemText (hwnDlg, IDC_CSSEDIT, szBuffer);
      break;

    case WM_COMMAND:
      if (LOWORD (wParam) == 1 && HIWORD (wParam) == LBN_SELCHANGE) {
	itemIndex = SendMessage (wndCSSList, LB_GETCURSEL, 0, 0);
	itemIndex = SendMessage (wndCSSList, LB_GETTEXT, itemIndex, (LPARAM) szBuffer);
	SetDlgItemText (hwnDlg, IDC_CSSEDIT, szBuffer);
	/* ThotCallback (NumSelectLanguage, STRING_DATA, szBuffer);*/
      }

      switch (LOWORD (wParam))
	{
	case ID_CONFIRM:
	  ThotCallback (BaseCSS + CSSForm, INTEGER_DATA, (CHAR_T*) 1);
	  EndDialog (hwnDlg, ID_CONFIRM);
	  break;

	case ID_DONE:
	  ThotCallback (BaseCSS + CSSForm, INTEGER_DATA, (CHAR_T*) 0);
	  EndDialog (hwnDlg, ID_DONE);
	  break;

	case WM_CLOSE:
	case WM_DESTROY:
	  EndDialog (hwnDlg, ID_DONE);
	  break;
	}
      break;
    default:
      return FALSE;
    }
  return TRUE;
}

/*-----------------------------------------------------------------------
 TextDlgProc
 ------------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK TextDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK TextDlgProc (hwnDlg, msg, wParam, lParam)
ThotWindow   hwndParent;
UINT   msg;
WPARAM wParam;
LPARAM lParam;
#endif /* __STDC__ */
{
  switch (msg)
    {
    case WM_INITDIALOG:
      if (isHref)
	{
	  SetWindowText (hwnDlg, TtaGetMessage (AMAYA, AM_ATTRIBUTE));
	  SetWindowText (GetDlgItem (hwnDlg, IDC_URL_TEXT), TtaGetMessage (AMAYA, AM_HREF_VALUE));
	}
      else
	{
	  SetWindowText (hwnDlg, TtaGetMessage (1, BTitle));
	  SetWindowText (GetDlgItem (hwnDlg, IDC_URL_TEXT), TtaGetMessage (AMAYA, AM_TITLE));
	}
      SetWindowText (GetDlgItem (hwnDlg, ID_CONFIRM), TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
      SetWindowText (GetDlgItem (hwnDlg, ID_DONE), TtaGetMessage (LIB, TMSG_CANCEL));
      SetDlgItemText (hwnDlg, IDC_URLEDIT, urlToOpen);      
      SetFocus (GetDlgItem (hwnDlg, IDC_URLEDIT));
      break;
      
    case WM_COMMAND:
      switch (LOWORD (wParam))
	{
	case ID_CONFIRM:
	  GetDlgItemText (hwnDlg, IDC_URLEDIT, urlToOpen, sizeof (urlToOpen) - 1);
	  if (isHref)
	    {
	      AttrHREFvalue = TtaAllocString (ustrlen (urlToOpen) + 1);
	      ustrcpy (AttrHREFvalue, urlToOpen);
	      ThotCallback (BaseDialog + AttrHREFForm, INTEGER_DATA, (CHAR_T*) 1);
	    }
	  else
	    {
	      ThotCallback (BaseDialog + TitleText, STRING_DATA, urlToOpen);
	      ThotCallback (BaseDialog + TitleForm, INTEGER_DATA, (CHAR_T*) 1);
	    }
	  EndDialog (hwnDlg, ID_CONFIRM);
	  break;
      
	case ID_DONE:
	  if (isHref)
	    ThotCallback (BaseDialog + AttrHREFForm, INTEGER_DATA, (CHAR_T*) 0);
	  else
	    ThotCallback (BaseDialog + TitleForm, INTEGER_DATA, (CHAR_T*) 0);
	  EndDialog (hwnDlg, ID_DONE);
	  break;
      
	case WM_CLOSE:
	case WM_DESTROY:
	  EndDialog (hwnDlg, ID_DONE);
	  break;
	}
      break;
      
    default:
      break;
    }
  return FALSE;
}

/*-----------------------------------------------------------------------
 HRefDlgProc
 ------------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK HRefDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK HRefDlgProc (hwnDlg, msg, wParam, lParam)
ThotWindow   hwndParent;
UINT   msg;
WPARAM wParam;
LPARAM lParam;
#endif /* __STDC__ */
{
  switch (msg)
    {
    case WM_INITDIALOG:
      SetWindowText (hwnDlg, TtaGetMessage (AMAYA, AM_ATTRIBUTE));
      SetWindowText (GetDlgItem (hwnDlg, IDC_URLMESSAGE), TtaGetMessage (AMAYA, AM_LOCATION));
      SetWindowText (GetDlgItem (hwnDlg, ID_CONFIRM), TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
      SetWindowText (GetDlgItem (hwnDlg, IDC_BROWSE), TEXT("Browse"));
      SetWindowText (GetDlgItem (hwnDlg, IDCANCEL), TtaGetMessage (LIB, TMSG_CANCEL));

      EditURLWnd = GetDlgItem (hwnDlg, IDC_GETURL);
      SetDlgItemText (hwnDlg, IDC_GETURL, tmpDocName);
      HrefUrl [0] = 0;
      break;

    case WM_COMMAND:
      if (HIWORD (wParam) == EN_UPDATE)
	{
	  if (LOWORD (wParam) == IDC_GETURL)
	    {
	      GetDlgItemText (hwnDlg, IDC_GETURL, HrefUrl, sizeof (HrefUrl) - 1);
	      if (HrefUrl[0] != 0)
		ThotCallback (BaseDialog + AttrHREFText, STRING_DATA, HrefUrl);
	    }
	}

    switch (LOWORD (wParam))
      {
      case ID_CONFIRM:
	ThotCallback (BaseDialog + AttrHREFForm, INTEGER_DATA, (CHAR_T*)1);
	EndDialog (hwnDlg, ID_CONFIRM);
	break;

      case IDC_BROWSE:
	OpenFileName.lStructSize       = sizeof (OPENFILENAME);
	OpenFileName.hwndOwner         = hwnDlg;
	OpenFileName.hInstance         = hInstance;
	OpenFileName.lpstrFilter       = (LPTSTR) szFilter;
	OpenFileName.lpstrCustomFilter = (LPTSTR) NULL;
	OpenFileName.nMaxCustFilter    = 0L;
	OpenFileName.nFilterIndex      = 1L;
	OpenFileName.lpstrFile         = (LPTSTR) szFileName;
	OpenFileName.nMaxFile          = 256;
	OpenFileName.lpstrInitialDir   = NULL;
	OpenFileName.lpstrTitle        = TEXT ("Select");
	OpenFileName.nFileOffset       = 0;
	OpenFileName.nFileExtension    = 0;
	OpenFileName.lpstrDefExt       = TEXT ("html");
	OpenFileName.lCustData         = 0;
	OpenFileName.Flags             = OFN_SHOWHELP | OFN_HIDEREADONLY;
	
	if (GetOpenFileName (&OpenFileName))
	  ustrcpy (HrefUrl, OpenFileName.lpstrFile);
      
	SetDlgItemText (hwnDlg, IDC_GETURL, HrefUrl);
	if (HrefUrl[0] != 0)
	  {
	    ThotCallback (BaseDialog + AttrHREFText, STRING_DATA, HrefUrl);
	    EndDialog (hwnDlg, ID_CONFIRM);
	    ThotCallback (BaseDialog + AttrHREFForm, INTEGER_DATA, (CHAR_T*) 1);
	  }
	break;
      
      case IDCANCEL:
	ThotCallback (BaseDialog + AttrHREFForm, INTEGER_DATA, (CHAR_T*) 0);
	HrefUrl [0] = 0;
	EndDialog (hwnDlg, IDCANCEL);
	break;      
      }
    break;
    default:
      return FALSE;
    } 
  return TRUE;
}
		
/*-----------------------------------------------------------------------
 HelpDlgProc
 ------------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK HelpDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK HelpDlgProc (hwnDlg, msg, wParam, lParam)
ThotWindow   hwndParent;
UINT   msg;
WPARAM wParam;
LPARAM lParam;
#endif /* __STDC__ */
{
  switch (msg)
    {
    case WM_INITDIALOG:
      SetWindowText (GetDlgItem (hwnDlg, IDC_VERSION), currentPathName);
      SetWindowText (GetDlgItem (hwnDlg, IDC_ABOUT1), message);
      SetWindowText (GetDlgItem (hwnDlg, IDC_ABOUT2), message2);
      SetWindowText (GetDlgItem (hwnDlg, ID_CONFIRM), TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
      break;
      
    case WM_COMMAND:
      switch (LOWORD (wParam))
	{
	case ID_CONFIRM:
	  EndDialog (hwnDlg, ID_CONFIRM);
	  break;

	case WM_CLOSE:
	case WM_DESTROY:
	  EndDialog (hwnDlg, ID_CONFIRM);
	  break;
	}
      break;

    default:
      return (FALSE);
    }
  return TRUE;
}
	
/*-----------------------------------------------------------------------
 MathDlgProc
 ------------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK MathDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK MathDlgProc (hwnDlg, msg, wParam, lParam)
ThotWindow   hwndParent;
UINT   msg;
WPARAM wParam;
LPARAM lParam;
#endif /* __STDC__ */
{
  switch (msg)
    {
    case WM_INITDIALOG:
      MathPal = hwnDlg;
      SetWindowText (hwnDlg, TtaGetMessage (AMAYA, AM_BUTTON_MATH));
      SetWindowText (GetDlgItem (hwnDlg, ID_DONE), TtaGetMessage (LIB, TMSG_DONE));
      break;

	case WM_CLOSE:
	case WM_DESTROY:
	  MathPal = NULL;
	  EndDialog (hwnDlg, ID_DONE);
	  break;

    case WM_COMMAND:
      switch (LOWORD (wParam))
	{
	case ID_DONE:
	  MathPal = NULL;
	  EndDialog (hwnDlg, ID_DONE);
	  break;
	case IDC_MATH:
	  ThotCallback (MathsDialogue + MenuMaths, INTEGER_DATA, (CHAR_T*)0);
	  break;
	case IDC_ROOT:
	  ThotCallback (MathsDialogue + MenuMaths, INTEGER_DATA, (CHAR_T*)1);
	  break;
	case IDC_SROOT:
	  ThotCallback (MathsDialogue + MenuMaths, INTEGER_DATA, (CHAR_T*)2);
	  break;
	case IDC_DIV:
	  ThotCallback (MathsDialogue + MenuMaths, INTEGER_DATA, (CHAR_T*)3);
	  break;
	case IDC_POWIND:
	  ThotCallback (MathsDialogue + MenuMaths, INTEGER_DATA, (CHAR_T*)4);
	  break;
	case IDC_IND:
	  ThotCallback (MathsDialogue + MenuMaths, INTEGER_DATA, (CHAR_T*)5);
	  break;
	case IDC_POW:
	  ThotCallback (MathsDialogue + MenuMaths, INTEGER_DATA, (CHAR_T*)6);
	  break;
	case IDC_UPDN:
	  ThotCallback (MathsDialogue + MenuMaths, INTEGER_DATA, (CHAR_T*)7);
	  break;
	case IDC_UP:
	  ThotCallback (MathsDialogue + MenuMaths, INTEGER_DATA, (CHAR_T*)8);
	  break;
	case IDC_DOWN:
	  ThotCallback (MathsDialogue + MenuMaths, INTEGER_DATA, (CHAR_T*)9);
	  break;
	case IDC_PAREXP:
	  ThotCallback (MathsDialogue + MenuMaths, INTEGER_DATA, (CHAR_T*)10);
	  break;
	case IDC_UDLR:
	  ThotCallback (MathsDialogue + MenuMaths, INTEGER_DATA, (CHAR_T*)11);
	  break;
	case IDC_MATRIX:
	  ThotCallback (MathsDialogue + MenuMaths, INTEGER_DATA, (CHAR_T*)12);
	  break;
	case IDC_SYM:
	  ThotCallback (MathsDialogue + MenuMaths, INTEGER_DATA, (CHAR_T*)13);
	  break;
	}
      SetFocus (FrRef[currentFrame]);
      break;
      
    default:
      return (FALSE);
    }
  return TRUE;
}
	
/*-----------------------------------------------------------------------
 PrintDlgProc
 ------------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK PrintDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK PrintDlgProc (hwnDlg, msg, wParam, lParam)
ThotWindow   hwndParent;
UINT   msg;
WPARAM wParam;
LPARAM lParam;
#endif /* __STDC__ */
{
  switch (msg)
    {
    case WM_INITDIALOG:
      SetWindowText (hwnDlg, TtaGetMessage (LIB, TMSG_LIB_PRINT));
      SetWindowText (GetDlgItem (hwnDlg, ID_PRINT), TtaGetMessage (AMAYA, AM_BUTTON_PRINT));
      SetWindowText (GetDlgItem (hwnDlg, IDCANCEL), TtaGetMessage (LIB, TMSG_CANCEL));
      SetWindowText (GetDlgItem (hwnDlg, IDC_OPTIONS), TtaGetMessage (LIB, TMSG_OPTIONS));
      SetWindowText (GetDlgItem (hwnDlg, IDC_TABOFCONTENTS), TtaGetMessage (AMAYA, AM_PRINT_TOC));
      SetWindowText (GetDlgItem (hwnDlg, IDC_LINKS), TtaGetMessage (AMAYA, AM_NUMBERED_LINKS));
      SetWindowText (GetDlgItem (hwnDlg, IDC_PRINTURL), TtaGetMessage (AMAYA, AM_PRINT_URL));
      SetWindowText (GetDlgItem (hwnDlg, IDC_IGNORE_CSS),  TtaGetMessage (AMAYA, AM_WITH_CSS));
      CheckDlgButton (hwnDlg, IDC_PRINTURL, PrintURL);
      CheckDlgButton (hwnDlg, IDC_IGNORE_CSS, IgnoreCSS);
      CheckDlgButton (hwnDlg, IDC_TABOFCONTENTS, WithToC);
      CheckDlgButton (hwnDlg, IDC_LINKS, NumberLinks);
      break;
    
    case WM_COMMAND:
      switch (LOWORD (wParam))
	{
	case IDC_TABOFCONTENTS:
	  ThotCallback (BasePrint + PrintOptions, INTEGER_DATA, (CHAR_T*) 1);
	  break;
	case IDC_LINKS:
	  ThotCallback (BasePrint + PrintOptions, INTEGER_DATA, (CHAR_T*) 2);
	  break;
	case IDC_PRINTURL:
	  ThotCallback (BasePrint + PrintOptions, INTEGER_DATA, (CHAR_T*) 3);
	  break;
	case IDC_IGNORE_CSS:
	  ThotCallback (BasePrint + PrintOptions, INTEGER_DATA, (CHAR_T*) 4);
	  break;
	case ID_PRINT:
	  ThotCallback (BasePrint + PrintSupport, INTEGER_DATA, (CHAR_T*) 0);
	  EndDialog (hwnDlg, ID_PRINT);
	  if (TtPrinterDC)
	    DeleteDC (TtPrinterDC);
	  TtPrinterDC = GetPrinterDC ();
	  if (TtPrinterDC)
	    {
	      WinInitPrinterColors ();
	      
	      EnableWindow (ghwndMain, FALSE);
	      ThotCallback (BasePrint + PPrinterName, STRING_DATA, currentFileToPrint);
	      ThotCallback (BasePrint + FormPrint, INTEGER_DATA, (CHAR_T*)1);
	      if (TtPrinterDC)
		  {
		     DeleteDC (TtPrinterDC);
		     TtPrinterDC = NULL;
	      }
	    }
	  break;
	case IDCANCEL:
	  if (TtPrinterDC)
	    {
	      DeleteDC (TtPrinterDC);
	      TtPrinterDC = NULL;
	    }
	  ThotCallback (BasePrint + FormPrint, INTEGER_DATA, (CHAR_T*)0);
	  EndDialog (hwnDlg, IDCANCEL);
	  break;
	}
      break;
    default:
      return FALSE;
    } 
  return TRUE;
}

/*-----------------------------------------------------------------------
 TableDlgProc
 ------------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK TableDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK TableDlgProc (hwnDlg, msg, wParam, lParam)
ThotWindow    hwndParent;
UINT    msg;
WPARAM  wParam;
LPARAM  lParam;
#endif /* __STDC__ */
{
  ThotBool ok;
  int      val;

  switch (msg)
    {
    case WM_INITDIALOG:
      SetWindowText (hwnDlg, TtaGetMessage (1, BTable));
      SetWindowText (GetDlgItem (hwnDlg, IDC_NUMCOL), TtaGetMessage (AMAYA, AM_COLS));
      SetWindowText (GetDlgItem (hwnDlg, IDC_NUMROWS), TtaGetMessage (AMAYA, AM_ROWS));
      if (WithBorder)
	SetWindowText (GetDlgItem (hwnDlg, IDC_BORDER), TtaGetMessage (AMAYA, AM_BORDER));
      SetWindowText (GetDlgItem (hwnDlg, ID_CONFIRM), TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
      SetWindowText (GetDlgItem (hwnDlg, IDCANCEL), TtaGetMessage (LIB, TMSG_CANCEL));
	
      SetDlgItemInt (hwnDlg, IDC_NUMCOLEDIT, numCols, FALSE);
      SetDlgItemInt (hwnDlg, IDC_NUMROWSEDIT, numRows, FALSE);
      if (WithBorder)
	SetDlgItemInt (hwnDlg, IDC_BORDEREDIT, tBorder, FALSE);
      break;

    case WM_COMMAND:
      if (HIWORD (wParam) == EN_UPDATE)
	{
	  if (LOWORD (wParam) == IDC_NUMCOLEDIT)
	    {
	      val = GetDlgItemInt (hwnDlg, IDC_NUMCOLEDIT, &ok, TRUE);
	      if (ok)
		ThotCallback (BaseDialog + TableCols, INTEGER_DATA, (CHAR_T*) val);
	    }
	  else if (LOWORD (wParam) == IDC_NUMROWSEDIT)
	    {
	      val = GetDlgItemInt (hwnDlg, IDC_NUMROWSEDIT, &ok, TRUE);
	      if (ok)
		ThotCallback (BaseDialog + TableRows, INTEGER_DATA, (CHAR_T*) val);
	    }
	  else if (LOWORD (wParam) == IDC_BORDEREDIT)
	    {
	      val = GetDlgItemInt (hwnDlg, IDC_BORDEREDIT, &ok, TRUE);
	      if (ok)
		ThotCallback (BaseDialog + TableBorder, INTEGER_DATA, (CHAR_T*) val);
	    }
	}
      
      switch (LOWORD (wParam))
	{
	case ID_CONFIRM:
	  ThotCallback (BaseDialog + TableForm, INTEGER_DATA, (CHAR_T*) 1);
	  EndDialog (hwnDlg, ID_CONFIRM);
	  break;
	case IDCANCEL:
	  EndDialog (hwnDlg, IDCANCEL);
	  ThotCallback (BaseDialog + TableForm, INTEGER_DATA, (CHAR_T*) 0);
	  break;
	}
      break;
    default:
      return FALSE;
    }
  return TRUE;
}

/*-----------------------------------------------------------------------
  AttrItemsDlgProc
 ------------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK AttrItemsDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK AttrItemsDlgProc (hwnDlg, msg, wParam, lParam)
ThotWindow   hwndParent;
UINT   msg;
WPARAM wParam;
LPARAM lParam;
#endif /* __STDC__ */
{
  int        ndx = 0;
  int        i   = 0;
  HDC        hDC;
  RECT       rect;
  ThotWindow radio1;
  ThotWindow radio2;
  ThotWindow radio3;
  ThotWindow radio4;
  ThotWindow radio5;
  ThotWindow radio6;
  ThotWindow radio7;
  ThotWindow radio8;
  ThotWindow radio9;
  ThotWindow groupBx;
  TEXTMETRIC tm;

  switch (msg)
    {
    case WM_INITDIALOG:
      GetClientRect (hwnDlg, &rect);
      hDC = GetDC (hwnDlg);
      SelectObject (hDC, GetStockObject (SYSTEM_FIXED_FONT));
      GetTextMetrics (hDC, &tm);
      cxChar = tm.tmAveCharWidth;
      cyChar = tm.tmHeight + tm.tmExternalLeading;
      ReleaseDC (hwnDlg, hDC);
      
      SetWindowText (hwnDlg, TtaGetMessage (LIB, TMSG_ATTR));
      SetWindowText (GetDlgItem (hwnDlg, ID_APPLY), TtaGetMessage (LIB, TMSG_APPLY));
      SetWindowText (GetDlgItem (hwnDlg, ID_DELETE), TtaGetMessage (LIB, TMSG_DEL_ATTR));
      SetWindowText (GetDlgItem (hwnDlg, ID_DONE), TtaGetMessage (LIB, TMSG_DONE));
      
      radio1 = CreateWindow (TEXT("BUTTON"), &WIN_buffMenu [ndx],
		  WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 2 * cxChar,
		  cyChar * (1 + 2 * i), 20 * cxChar, 7 * cyChar / 4, hwnDlg,
		  (HMENU) OPT1, (HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);
      ndx += ustrlen (&WIN_buffMenu [ndx]) + 1;
      i++;
      radio2 = CreateWindow (TEXT("BUTTON"), &WIN_buffMenu [ndx],
		  WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 2 * cxChar,
		  cyChar * (1 + 2 * i), 20 * cxChar, 7 * cyChar / 4, hwnDlg,
		  (HMENU) OPT2, (HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);
      ndx += ustrlen (&WIN_buffMenu [ndx]) + 1;
      i++;
      if (attDlgNbItems > 2)
	{
	  radio3 = CreateWindow (TEXT("BUTTON"), &WIN_buffMenu [ndx],
		  WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 2 * cxChar,
		  cyChar * (1 + 2 * i), 20 * cxChar, 7 * cyChar / 4, hwnDlg,
		  (HMENU) OPT3, (HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);
	  ndx += ustrlen (&WIN_buffMenu [ndx]) + 1;
	  i++;
	  if (attDlgNbItems > 3)
	    {	  
	      radio4 = CreateWindow (TEXT("BUTTON"), &WIN_buffMenu [ndx],
			  WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 2 * cxChar,
			  cyChar * (1 + 2 * i), 20 * cxChar, 7 * cyChar / 4, hwnDlg,
			  (HMENU) OPT4, (HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);
	      ndx += ustrlen (&WIN_buffMenu [ndx]) + 1;
	      i++;
	      if (attDlgNbItems > 4)
		{
		  radio5 = CreateWindow (TEXT("BUTTON"), &WIN_buffMenu [ndx],
			  WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 2 * cxChar,
			  cyChar * (1 + 2 * i), 20 * cxChar, 7 * cyChar / 4, hwnDlg,
			  (HMENU) OPT5, (HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);
	      ndx += ustrlen (&WIN_buffMenu [ndx]) + 1;
		  i++;
          if (attDlgNbItems > 5)
		  {
	         radio6 = CreateWindow (TEXT("BUTTON"), &WIN_buffMenu [ndx],
				 WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 2 * cxChar,
				 cyChar * (1 + 2 * i), 20 * cxChar, 7 * cyChar / 4, hwnDlg,
				 (HMENU) OPT6, (HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);
	         ndx += ustrlen (&WIN_buffMenu [ndx]) + 1;
	         i++;
	         if (attDlgNbItems > 6)
			 {	  
	            radio7 = CreateWindow (TEXT("BUTTON"), &WIN_buffMenu [ndx],
					WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 2 * cxChar,
					cyChar * (1 + 2 * i), 20 * cxChar, 7 * cyChar / 4, hwnDlg,
					(HMENU) OPT7, (HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);
	            ndx += ustrlen (&WIN_buffMenu [ndx]) + 1;
	            i++;
	            if (attDlgNbItems > 7)
				{
		          radio8 = CreateWindow (TEXT("BUTTON"), &WIN_buffMenu [ndx],
					  WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 2 * cxChar,
					  cyChar * (1 + 2 * i), 20 * cxChar, 7 * cyChar / 4, hwnDlg,
					  (HMENU) OPT8, (HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);
	              ndx += ustrlen (&WIN_buffMenu [ndx]) + 1;
		          i++;
				  if (attDlgNbItems > 8)
				  {
		            radio9 = CreateWindow (TEXT("BUTTON"), &WIN_buffMenu [ndx],
					  WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 2 * cxChar,
					  cyChar * (1 + 2 * i), 20 * cxChar, 7 * cyChar / 4, hwnDlg,
					  (HMENU) OPT9, (HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);
	                ndx += ustrlen (&WIN_buffMenu [ndx]) + 1;
		            i++;
				  }
				}
			 }
			 }
		}
	   }
	}
      groupBx = CreateWindow (TEXT("BUTTON"), attDlgTitle,
		  WS_CHILD | WS_VISIBLE | BS_GROUPBOX, cxChar, 0,
		  rect.right - (2 * cxChar), i * (2 * cyChar) + cyChar, hwnDlg,
		  (HMENU) 1, (HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);
      switch (currAttrVal)
	{
	case 1: CheckRadioButton (hwnDlg, OPT1, OPT9, OPT1);
	  break;
	case 2: CheckRadioButton (hwnDlg, OPT1, OPT9, OPT2);
	  break;
	case 3: CheckRadioButton (hwnDlg, OPT1, OPT9, OPT3);
	  break;
	case 4: CheckRadioButton (hwnDlg, OPT1, OPT9, OPT4);
	  break;
	case 5: CheckRadioButton (hwnDlg, OPT1, OPT9, OPT5);
	  break;
	case 6: CheckRadioButton (hwnDlg, OPT1, OPT9, OPT6);
	  break;
	case 7: CheckRadioButton (hwnDlg, OPT1, OPT9, OPT7);
	  break;
	case 8: CheckRadioButton (hwnDlg, OPT1, OPT9, OPT8);
	  break;
	case 9: CheckRadioButton (hwnDlg, OPT1, OPT9, OPT9);
	  break;	  
	default:
	  break;
	}
      break;
      
    case WM_COMMAND:
      switch (LOWORD (wParam)) {
      case OPT1:
	iLocation = 0;
	ThotCallback (NumMenuAttrEnum, INTEGER_DATA, (CHAR_T*) iLocation);
	break;
      case OPT2:
	iLocation = 1;
	ThotCallback (NumMenuAttrEnum, INTEGER_DATA, (CHAR_T*) iLocation);
	break;
      case OPT3:
	iLocation = 2;
	ThotCallback (NumMenuAttrEnum, INTEGER_DATA, (CHAR_T*) iLocation);
	break;
      case OPT4:
	iLocation = 3;
	ThotCallback (NumMenuAttrEnum, INTEGER_DATA, (CHAR_T*) iLocation);
	break;
      case OPT5:
	iLocation = 4;
	ThotCallback (NumMenuAttrEnum, INTEGER_DATA, (CHAR_T*) iLocation);
	break;
      case OPT6:
	iLocation = 5;
	ThotCallback (NumMenuAttrEnum, INTEGER_DATA, (CHAR_T*) iLocation);
	break;
      case OPT7:
	iLocation = 6;
	ThotCallback (NumMenuAttrEnum, INTEGER_DATA, (CHAR_T*) iLocation);
	break;
      case OPT8:
	iLocation = 7;
	ThotCallback (NumMenuAttrEnum, INTEGER_DATA, (CHAR_T*) iLocation);
	break;
      case OPT9:
	iLocation = 8;
	ThotCallback (NumMenuAttrEnum, INTEGER_DATA, (CHAR_T*) iLocation);
	break;
       case ID_APPLY:
	ThotCallback (NumMenuAttr, INTEGER_DATA, (CHAR_T*) 1);
	break;
	
      case ID_DELETE:
	ThotCallback (NumMenuAttrEnum, INTEGER_DATA, (CHAR_T*) iLocation);
	ThotCallback (NumMenuAttr, INTEGER_DATA, (CHAR_T*) 2);
	EndDialog (hwnDlg, ID_DELETE);
	break;
	
      case ID_DONE:
	ThotCallback (NumMenuAttr, INTEGER_DATA, (CHAR_T*) 0);
	EndDialog (hwnDlg, IDCANCEL);
	break;
	
      case WM_CLOSE:
      case WM_DESTROY:
	EndDialog (hwnDlg, IDCANCEL);
	break;
      }
      break;
      
    default:
      return FALSE;
    }
  return TRUE;
}

/*-----------------------------------------------------------------------
 SearchDlgProc
 ------------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK SearchDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK SearchDlgProc (hwnDlg, msg, wParam, lParam)
ThotWindow   hwndParent;
UINT   msg;
WPARAM wParam;
LPARAM lParam;
#endif /* __STDC__ */
{
    switch (msg)
      {
      case WM_INITDIALOG:
	SetWindowText (hwnDlg, wndTitle);
	SetWindowText (GetDlgItem (hwnDlg, IDC_SEARCHFOR), TtaGetMessage (LIB, TMSG_SEARCH_FOR));
	SetWindowText (GetDlgItem (hwnDlg, IDC_UPPERLOWER), TtaGetMessage (LIB, TMSG_UPPERCASE_EQ_LOWERCASE));
	SetWindowText (GetDlgItem (hwnDlg, IDC_REPLACEGROUP), TtaGetMessage (LIB, TMSG_REPLACE));
	SetWindowText (GetDlgItem (hwnDlg, IDC_REPLACEDBY), TtaGetMessage (LIB, TMSG_REPLACE_BY));
	SetWindowText (GetDlgItem (hwnDlg, IDC_NOREPLACE), TtaGetMessage (LIB, TMSG_NO_REPLACE));
	SetWindowText (GetDlgItem (hwnDlg, IDC_ONREQUEST), TtaGetMessage (LIB, TMSG_REPLACE_ON_REQU));
	SetWindowText (GetDlgItem (hwnDlg, IDC_AUTOMATIC), TtaGetMessage (LIB, TMSG_AUTO_REPLACE));
	SetWindowText (GetDlgItem (hwnDlg, IDC_WHEREGROUP), TtaGetMessage (LIB, TMSG_SEARCH_WHERE));
	SetWindowText (GetDlgItem (hwnDlg, IDC_BEFORE), TtaGetMessage (LIB, TMSG_BEFORE_SEL));
	SetWindowText (GetDlgItem (hwnDlg, IDC_WITHIN), TtaGetMessage (LIB, TMSG_WITHIN_SEL));
	SetWindowText (GetDlgItem (hwnDlg, IDC_AFTER), TtaGetMessage (LIB, TMSG_AFTER_SEL));
	SetWindowText (GetDlgItem (hwnDlg, IDC_WHOLEDOC), TtaGetMessage (LIB, TMSG_IN_WHOLE_DOC));
	SetWindowText (GetDlgItem (hwnDlg, ID_CONFIRM), TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
	SetWindowText (GetDlgItem (hwnDlg, ID_NOREPLACE), TtaGetMessage (LIB, TMSG_DO_NOT_REPLACE));
	SetWindowText (GetDlgItem (hwnDlg, ID_DONE), TtaGetMessage (LIB, TMSG_DONE));
	SetDlgItemText (hwnDlg, IDC_SEARCHEDIT, TEXT(""));
	SetDlgItemText (hwnDlg, IDC_REPLACEDIT, TEXT(""));	
	iMode     = 0;
	
	CheckRadioButton (hwnDlg, IDC_NOREPLACE, IDC_AUTOMATIC, IDC_NOREPLACE);
	if (selectionFound)
	  {
	    CheckRadioButton (hwnDlg, IDC_BEFORE, IDC_WHOLEDOC, IDC_AFTER);
	    iLocation = 2;
	  }
	else
	  {
	    CheckRadioButton (hwnDlg, IDC_BEFORE, IDC_WHOLEDOC, IDC_WHOLEDOC);
	    iLocation = 3;
	  }
	SetFocus (GetDlgItem (hwnDlg, IDC_SEARCHEDIT));
	break;

      case WM_COMMAND:
	switch (LOWORD (wParam))
	  {
	  case ID_CONFIRM:
	    searchEnd = FALSE;
	    GetDlgItemText (hwnDlg, IDC_SEARCHEDIT, textToSearch, sizeof (textToSearch) - 1);
	    GetDlgItemText (hwnDlg, IDC_REPLACEDIT, newText, sizeof (newText) - 1);
	    if (newText && newText[0] != '\0' && iMode == 0)
	      {
		iMode = 1;
		CheckRadioButton (hwnDlg, IDC_NOREPLACE, IDC_AUTOMATIC, IDC_ONREQUEST);
	      }

	    if (iMode == 1 || iMode == 2) 
	      ThotCallback (NumZoneTextReplace, STRING_DATA, newText);
							
	    ThotCallback (NumZoneTextSearch, STRING_DATA, textToSearch);
	    ThotCallback (NumMenuReplaceMode, INTEGER_DATA, (CHAR_T*) iMode);
	    ThotCallback (NumMenuOrSearchText, INTEGER_DATA, (CHAR_T*) iLocation);
	    ThotCallback (NumFormSearchText, INTEGER_DATA, (CHAR_T*) 1);
	    if (!searchEnd && iLocation == 3)
	      {
		CheckRadioButton (hwnDlg, IDC_BEFORE, IDC_WHOLEDOC, IDC_AFTER);
		iLocation = 2;
	      }
	    break;

	  case ID_NOREPLACE:
	    ThotCallback (NumZoneTextSearch, STRING_DATA, textToSearch);
	    ThotCallback (NumMenuReplaceMode, INTEGER_DATA, (CHAR_T*) 0);
	    ThotCallback (NumMenuOrSearchText, INTEGER_DATA, (CHAR_T*) iLocation);
	    ThotCallback (NumFormSearchText, INTEGER_DATA, (CHAR_T*) 1);
	    if (iLocation == 3)
	      {
		CheckRadioButton (hwnDlg, IDC_BEFORE, IDC_WHOLEDOC, IDC_AFTER);
		iLocation = 2;
	      }
	    break;
	    
	  case WM_CLOSE:
	  case WM_DESTROY:
	    EndDialog (hwnDlg, ID_DONE);
	    break;
	    
	  case ID_DONE:
	    ThotCallback (120, 1, NULL);
	    EndDialog (hwnDlg, ID_DONE);
	    break;
	    
	  case IDC_NOREPLACE:
	    iMode = 0;
	    CheckRadioButton (hwnDlg, IDC_NOREPLACE, IDC_AUTOMATIC, LOWORD (wParam));
	    break;
	    
	  case IDC_ONREQUEST:
	    iMode = 1;
	    CheckRadioButton (hwnDlg, IDC_NOREPLACE, IDC_AUTOMATIC, LOWORD (wParam));
	    break;
	    
	  case IDC_AUTOMATIC:
	    iMode = 2;
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
	    ThotCallback (NumToggleUpperEqualLower, INTEGER_DATA, (CHAR_T*) 0);
	    break;
	  }
	break;
      default:
	return FALSE;
      }
    return TRUE;
}

/*-----------------------------------------------------------------------
 SaveAsDlgProc
 ------------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK SaveAsDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK SaveAsDlgProc (hwnDlg, msg, wParam, lParam)
ThotWindow   hwndParent;
UINT   msg;
WPARAM wParam;
LPARAM lParam;
#endif /* __STDC__ */
{
  CHAR_T      txt [500];

  txt [0] = 0;
  switch (msg)
    {
    case WM_INITDIALOG:
      currentDlg = hwnDlg;
      SetWindowText (hwnDlg, TtaGetMessage (AMAYA, AM_SAVE_AS));
      SetWindowText (GetDlgItem (hwnDlg, IDC_DOCLOCATION), TtaGetMessage (AMAYA, AM_DOC_LOCATION));
      SetWindowText (GetDlgItem (hwnDlg, ID_CONFIRM), TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
      SetWindowText (GetDlgItem (hwnDlg, ID_CLEAR), TtaGetMessage (AMAYA, AM_CLEAR));
      SetWindowText (GetDlgItem (hwnDlg, IDC_BROWSE), TEXT("Browse"));
      SetWindowText (GetDlgItem (hwnDlg, IDCANCEL), TtaGetMessage (LIB, TMSG_CANCEL));
      SetDlgItemText (hwnDlg, IDC_EDITDOCSAVE, currentPathName);
      
      if (!TextFormat &&
	  DocumentTypes[SavingDocument] != docMath &&
	  DocumentTypes[SavingDocument] != docSVG)
	{
	  SetWindowText (GetDlgItem (hwnDlg, IDC_OUTPUTGROUP), TtaGetMessage (LIB, TMSG_DOCUMENT_FORMAT));
	  SetWindowText (GetDlgItem (hwnDlg, IDC_HTML), TEXT("HTML"));
	  SetWindowText (GetDlgItem (hwnDlg, IDC_XML), TEXT("XML"));
	  SetWindowText (GetDlgItem (hwnDlg, IDC_TEXT), TEXT("Text"));
	  SetWindowText (GetDlgItem (hwnDlg, IDC_COPYIMG), TtaGetMessage (AMAYA, AM_BCOPY_IMAGES));
	  SetWindowText (GetDlgItem (hwnDlg, IDC_TRANSFORMURL), TtaGetMessage (AMAYA, AM_BTRANSFORM_URL));
	  SetWindowText (GetDlgItem (hwnDlg, IDC_IMGLOCATION), TtaGetMessage (AMAYA, AM_IMAGES_LOCATION));

	  transURLWnd = GetDlgItem (hwnDlg, IDC_COPYIMG);
	  copyImgWnd = GetDlgItem (hwnDlg, IDC_TRANSFORMURL);
	  if (SaveAsHTML)
	    {
	      CheckRadioButton (hwnDlg, IDC_HTML, IDC_XML, IDC_HTML);
	      EnableWindow (transURLWnd, TRUE);
	      EnableWindow (copyImgWnd, TRUE);
	    }
	  else if (SaveAsXML)
	    {
	      CheckRadioButton (hwnDlg, IDC_HTML, IDC_XML, IDC_XML);
	      EnableWindow (transURLWnd, TRUE);
	      EnableWindow (copyImgWnd, TRUE);
	    }
	  else if (SaveAsText)
	    {
	      CheckRadioButton (hwnDlg, IDC_HTML, IDC_XML, IDC_TEXT);
	      EnableWindow (transURLWnd, FALSE);
	      EnableWindow (copyImgWnd, FALSE);
	    }
    
	  if (CopyImages)
	    CheckRadioButton (hwnDlg, IDC_COPYIMG, IDC_COPYIMG, IDC_COPYIMG);
	  
	  if (UpdateURLs)
	    CheckRadioButton (hwnDlg, IDC_TRANSFORMURL, IDC_TRANSFORMURL, IDC_TRANSFORMURL);
	}
      break;
    
    case WM_COMMAND:
      if (HIWORD (wParam) == EN_UPDATE)
	{
	  if (LOWORD (wParam) == IDC_EDITDOCSAVE)
	    {
	      GetDlgItemText (hwnDlg, IDC_EDITDOCSAVE, urlToOpen, sizeof (urlToOpen) - 1);
	      ThotCallback (BaseDialog + NameSave, STRING_DATA, urlToOpen);
	    }
	  else if (LOWORD (wParam) == IDC_EDITIMGSAVE)
	    {
	      GetDlgItemText (hwnDlg, IDC_EDITIMGSAVE, urlToOpen, sizeof (urlToOpen) - 1);
	      ThotCallback (BaseDialog + ImgDirSave, STRING_DATA, urlToOpen);
	    }
	}

      switch (LOWORD (wParam))
	{
	case IDC_HTML:
	  EnableWindow (transURLWnd, TRUE);
	  EnableWindow (copyImgWnd, TRUE);
	  ThotCallback (BaseDialog + ToggleSave, INTEGER_DATA, (CHAR_T*) 0);
	  SetDlgItemText (hwnDlg, IDC_EDITDOCSAVE, DocToOpen);
	  return 0;
	  
	case IDC_XML:
	  EnableWindow (transURLWnd, TRUE);
	  EnableWindow (copyImgWnd, TRUE);
	  ThotCallback (BaseDialog + ToggleSave, INTEGER_DATA, (CHAR_T*) 1);
	  SetDlgItemText (hwnDlg, IDC_EDITDOCSAVE, DocToOpen);
	  return 0;
	  
	case IDC_TEXT:
	  EnableWindow (transURLWnd, FALSE);
	  EnableWindow (copyImgWnd, FALSE);
	  
	  ThotCallback (BaseDialog + ToggleSave, INTEGER_DATA, (CHAR_T*) 2);
	  SetDlgItemText (hwnDlg, IDC_EDITDOCSAVE, DocToOpen);
	  return 0;
	  
	case IDC_COPYIMG:
	  ThotCallback (BaseDialog + ToggleSave, INTEGER_DATA, (CHAR_T*) 4);
	  break;
	  
	case IDC_TRANSFORMURL:
	  ThotCallback (BaseDialog + ToggleSave, INTEGER_DATA, (CHAR_T*) 5);
	  break;
	  
	case ID_CLEAR:
	  SetDlgItemText (hwnDlg, IDC_EDITDOCSAVE, TEXT(""));
	  SetDlgItemText (hwnDlg, IDC_EDITIMGSAVE, TEXT(""));
	  ThotCallback (BaseDialog + SaveForm, INTEGER_DATA, (CHAR_T*) 2);
	  break;
	  
	case IDC_BROWSE:
	  WIN_ListSaveDirectory (BaseDialog + SaveForm, TtaGetMessage (AMAYA, AM_SAVE_AS), urlToOpen);
	  SetDlgItemText (hwnDlg, IDC_EDITDOCSAVE, urlToOpen);
	  ThotCallback (BaseDialog + NameSave, STRING_DATA, urlToOpen);
	  break;
	  
	case IDCANCEL:
	  EndDialog (hwnDlg, IDCANCEL);
	  ThotCallback (BaseDialog + SaveForm, INTEGER_DATA, (CHAR_T*) 0);
	  currentDlg = NULL;
	  break;
	  
	case ID_CONFIRM:
	  EndDialog (hwnDlg, ID_CONFIRM);
	  ThotCallback (BaseDialog + SaveForm, INTEGER_DATA, (CHAR_T*) 1);
	  currentDlg = NULL;
	  break;
	}
      break;
      
    default:
      return FALSE;
    }
  return TRUE;
}

/*-----------------------------------------------------------------------
 OpenDocDlgProc
 ------------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK OpenDocDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK OpenDocDlgProc (hwnDlg, msg, wParam, lParam)
ThotWindow   hwndParent;
UINT   msg;
WPARAM wParam;
LPARAM lParam;
#endif /* __STDC__ */
{
  switch (msg)
    {
    case WM_INITDIALOG:
      SetWindowText (hwnDlg, TtaGetMessage (AMAYA, AM_OPEN_URL));
      SetWindowText (GetDlgItem (hwnDlg, IDC_URLMESSAGE), TEXT("Type the URI or push the button Browse"));
      SetWindowText (GetDlgItem (hwnDlg, ID_CONFIRM), TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
      SetWindowText (GetDlgItem (hwnDlg, IDC_BROWSE), TEXT("Browse"));
      SetWindowText (GetDlgItem (hwnDlg, IDCANCEL), TtaGetMessage (LIB, TMSG_CANCEL));

      EditURLWnd = GetDlgItem (hwnDlg, IDC_GETURL);
      SetWindowText (hwnDlg, wndTitle);
      SetDlgItemText (hwnDlg, IDC_GETURL, tmpDocName);
      urlToOpen [0] = 0;
      break;

    case WM_COMMAND:
      if (HIWORD (wParam) == EN_UPDATE)
	{
	  if (LOWORD (wParam) == IDC_GETURL)
	    {
	      GetDlgItemText (hwnDlg, IDC_GETURL, urlToOpen, sizeof (urlToOpen) - 1);
	      if (urlToOpen[0] != 0)
		ThotCallback (BaseDialog + URLName, STRING_DATA, urlToOpen);
	    }
	}

    switch (LOWORD (wParam))
      {
      case ID_CONFIRM:
	ThotCallback (BaseDialog + OpenForm, INTEGER_DATA, (CHAR_T*)1);
	EndDialog (hwnDlg, ID_CONFIRM);
	break;

      case IDC_BROWSE:
	OpenFileName.lStructSize       = sizeof (OPENFILENAME);
	OpenFileName.hwndOwner         = hwnDlg;
	OpenFileName.hInstance         = hInstance;
	OpenFileName.lpstrFilter       = (LPTSTR) szFilter;
	OpenFileName.lpstrCustomFilter = (LPTSTR) NULL;
	OpenFileName.nMaxCustFilter    = 0L;
	OpenFileName.nFilterIndex      = 1L;
	OpenFileName.lpstrFile         = (LPTSTR) szFileName;
	OpenFileName.nMaxFile          = 256;
	OpenFileName.lpstrInitialDir   = NULL;
	OpenFileName.lpstrTitle        = TEXT ("Select");
	OpenFileName.nFileOffset       = 0;
	OpenFileName.nFileExtension    = 0;
	OpenFileName.lpstrDefExt       = TEXT ("html");
	OpenFileName.lCustData         = 0;
	OpenFileName.Flags             = OFN_SHOWHELP | OFN_HIDEREADONLY;
	
	if (GetOpenFileName (&OpenFileName))
	  ustrcpy (urlToOpen, OpenFileName.lpstrFile);
      
	SetDlgItemText (hwnDlg, IDC_GETURL, urlToOpen);
	if (urlToOpen[0] != 0)
	  {
	    ThotCallback (BaseDialog + URLName, STRING_DATA, urlToOpen);
	    EndDialog (hwnDlg, ID_CONFIRM);
	    ThotCallback (BaseDialog + OpenForm, INTEGER_DATA, (CHAR_T*) 1);
	  }
	break;
      
      case IDCANCEL:
	ThotCallback (BaseDialog + OpenForm, INTEGER_DATA, (CHAR_T*) 0);
	urlToOpen [0] = 0;
	EndDialog (hwnDlg, IDCANCEL);
	break;      
      }
    break;
    default:
      return FALSE;
    } 
  return TRUE;
}

/*-----------------------------------------------------------------------
 OpenImgDlgProc
 ------------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK OpenImgDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK OpenImgDlgProc (hwnDlg, msg, wParam, lParam)
ThotWindow   hwndParent;
UINT   msg;
WPARAM wParam;
LPARAM lParam;
#endif /* __STDC__ */
{
    switch (msg)
      {
      case WM_INITDIALOG:
	SetWindowText (hwnDlg, TtaGetMessage (AMAYA, AM_IMAGES_LOCATION));
	SetWindowText (GetDlgItem (hwnDlg, IDC_URLMESSAGE), TtaGetMessage (AMAYA, AM_BUTTON_IMG));
	SetWindowText (GetDlgItem (hwnDlg, IDC_ALTMESSAGE), TtaGetMessage (AMAYA, AM_ALT));
	SetWindowText (GetDlgItem (hwnDlg, ID_CONFIRM), TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
	SetWindowText (GetDlgItem (hwnDlg, IDC_BROWSE), TEXT("Browse"));
	SetWindowText (GetDlgItem (hwnDlg, IDCANCEL), TtaGetMessage (LIB, TMSG_CANCEL));
	
	SetDlgItemText (hwnDlg, IDC_GETURL, TEXT(""));
	urlToOpen [0] = 0;
	SetDlgItemText (hwnDlg, IDC_GETALT, TEXT(""));
	altText [0] = 0;
	break;
	
      case WM_COMMAND:
	if (HIWORD (wParam) == EN_UPDATE)
	  if (LOWORD (wParam) == IDC_GETALT)
	    {
	      GetDlgItemText (hwnDlg, IDC_GETALT, altText, sizeof (altText) - 1);
	      ThotCallback (BaseImage + ImageAlt, STRING_DATA, altText);
	    }
	switch (LOWORD (wParam))
	  {
	  case ID_CONFIRM:
	    GetDlgItemText (hwnDlg, IDC_GETURL, urlToOpen, sizeof (urlToOpen) - 1);
	    GetDlgItemText (hwnDlg, IDC_GETALT, altText, sizeof (altText) - 1);
	    if (!altText || altText [0] == 0)
	      MessageBox (hwnDlg, TtaGetMessage (AMAYA, AM_ALT_MISSING),
			  TtaGetMessage (AMAYA, AM_BUTTON_IMG),
			  MB_OK | MB_ICONERROR);
	    else 
	      {
		ThotCallback (BaseImage + ImageAlt, STRING_DATA, altText);
		ThotCallback (BaseImage + ImageURL, STRING_DATA, urlToOpen);
		ThotCallback (BaseImage + FormImage, INTEGER_DATA, (CHAR_T*) 1);
		EndDialog (hwnDlg, ID_CONFIRM);
	      }
	    break;
	    
	  case IDC_BROWSE:
	    OpenFileName.lStructSize       = sizeof (OPENFILENAME);
	    OpenFileName.hwndOwner         = hwnDlg;
	    OpenFileName.hInstance         = hInstance;
	    OpenFileName.lpstrFilter       = (LPTSTR) szFilter;
	    OpenFileName.lpstrCustomFilter = (LPTSTR) NULL;
	    OpenFileName.nMaxCustFilter    = 0L;
	    OpenFileName.nFilterIndex      = 1L;
	    OpenFileName.lpstrFile         = (LPTSTR) szFileName;
	    OpenFileName.nMaxFile          = 256;
	    OpenFileName.lpstrInitialDir   = NULL;
	    OpenFileName.lpstrTitle        = TtaGetMessage (AMAYA, AM_FILES);
	    OpenFileName.nFileOffset       = 0;
	    OpenFileName.nFileExtension    = 0;
	    OpenFileName.lpstrDefExt       = ImgFilter;
	    OpenFileName.lCustData         = 0;
	    OpenFileName.Flags             = OFN_SHOWHELP | OFN_HIDEREADONLY;
	    
	    if (GetOpenFileName (&OpenFileName))
	      ustrcpy (urlToOpen, OpenFileName.lpstrFile);
	    
	    SetDlgItemText (hwnDlg, IDC_GETURL, urlToOpen);
	    if (altText [0] != 0)
	      EndDialog (hwnDlg, ID_CONFIRM);
	    break;

	  case IDCANCEL:
	    ThotCallback (BaseImage + FormImage, INTEGER_DATA, (CHAR_T*) 0);
	    urlToOpen [0] = 0;
	    EndDialog (hwnDlg, IDCANCEL);
	    break;
	  }
	break;
      default:
	return FALSE;
      }
    return TRUE;
}

/*-----------------------------------------------------------------------
 GraphicsDlgProc
 ------------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK GraphicsDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK GraphicsDlgProc (hwnDlg, msg, wParam, lParam)
ThotWindow   hwndParent;
UINT   msg;
WPARAM wParam;
LPARAM lParam;
#endif /* __STDC__ */
{
  switch (msg)
    {
    case WM_INITDIALOG:
      GraphPal = hwnDlg;
      SetWindowText (hwnDlg, TtaGetMessage (AMAYA, AM_BUTTON_GRAPHICS));
      SetWindowText (GetDlgItem (hwnDlg, ID_DONE), TtaGetMessage (LIB, TMSG_DONE));
      break;

	case WM_CLOSE:
	case WM_DESTROY:
	  GraphPal = NULL;
	  EndDialog (hwnDlg, ID_DONE);
	  break;

    case WM_COMMAND:
      SetFocus (FrRef[currentFrame]);
      switch (LOWORD (wParam))
	{
	case ID_DONE:
	  GraphPal = NULL;
	  EndDialog (hwnDlg, ID_DONE);
	  break;	 
	  
	case IDC_GLINE:
	  ThotCallback (GraphDialogue + MenuGraph, INTEGER_DATA, (CHAR_T*)0);
	  break;
	  
	case IDC_GRECT:
	  ThotCallback (GraphDialogue + MenuGraph, INTEGER_DATA, (CHAR_T*)1);
	  break;
	  
	case IDC_GRRECT:
	  ThotCallback (GraphDialogue + MenuGraph, INTEGER_DATA, (CHAR_T*)2);
	  break;

	case IDC_GCIRCLE:
	  ThotCallback (GraphDialogue + MenuGraph, INTEGER_DATA, (CHAR_T*)3);
	  break;
	  
	case IDC_GELLIPSE:
	  ThotCallback (GraphDialogue + MenuGraph, INTEGER_DATA, (CHAR_T*)4);
	  break;
	  
	case IDC_GPOLYLINE:
	  ThotCallback (GraphDialogue + MenuGraph, INTEGER_DATA, (CHAR_T*)5);
	  break;
	  
	case IDC_GCPOLYLINE:
	  ThotCallback (GraphDialogue + MenuGraph, INTEGER_DATA, (CHAR_T*)6);
	  break;
	  
	case IDC_GCURVE:
	  ThotCallback (GraphDialogue + MenuGraph, INTEGER_DATA, (CHAR_T*)7);
	  break;
	  
	case IDC_GCCURVE:
	  ThotCallback (GraphDialogue + MenuGraph, INTEGER_DATA, (CHAR_T*)8);
	  break;
	  
	case IDC_GALPHA1:
	  ThotCallback (GraphDialogue + MenuGraph, INTEGER_DATA, (CHAR_T*)9);
	  break;
	  
	case IDC_GALPHA2:
	  ThotCallback (GraphDialogue + MenuGraph, INTEGER_DATA, (CHAR_T*)10);
	  break;
	  
	case IDC_GGROUP:
	  ThotCallback (GraphDialogue + MenuGraph, INTEGER_DATA, (CHAR_T*)11);
	  break;
	}
      break;

    default: return (FALSE);
    }
  return TRUE;
}

/*-----------------------------------------------------------------------
  SaveListDlgProc
 ------------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK SaveListDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK SaveListDlgProc (hwnDlg, msg, wParam, lParam)
ThotWindow   hwndParent;
UINT   msg;
WPARAM wParam;
LPARAM lParam;
#endif /* __STDC__ */
{
  static ThotWindow  wndSaveList;
  int          index = 0;
  UINT         i = 0;

  switch (msg)
    {
    case WM_INITDIALOG:
      SetWindowText (hwnDlg, TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
      SetWindowText (GetDlgItem (hwnDlg, IDC_MSG), TtaGetMessage (AMAYA, AM_WARNING_SAVE_OVERWRITE));
      SetWindowText (GetDlgItem (hwnDlg, ID_CONFIRM), TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
      SetWindowText (GetDlgItem (hwnDlg, IDCANCEL), TtaGetMessage (LIB, TMSG_CANCEL));
      
      wndSaveList = CreateWindow (TEXT("listbox"), NULL, WS_CHILD | WS_VISIBLE | LBS_STANDARD,
				  10, 30, 260, 180, hwnDlg, (HMENU) 1, 
				  (HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);
      
      SendMessage (wndSaveList, LB_RESETCONTENT, 0, 0);
      while (i < nbItem && saveList[index] != EOS)
	{
	  SendMessage (wndSaveList, LB_INSERTSTRING, i, (LPARAM) &saveList[index]); 
	  index += ustrlen (&saveList[index]) + 1;	/* Longueur de l'intitule */
	  i++;
	}
      break;
      
    case WM_COMMAND:
      if (LOWORD (wParam) == 1 && HIWORD (wParam) == LBN_SELCHANGE)
	{
	  itemIndex = SendMessage (wndSaveList, LB_GETCURSEL, 0, 0);
	  itemIndex = SendMessage (wndSaveList, LB_GETTEXT, itemIndex, (LPARAM) szBuffer);
	  SetDlgItemText (hwnDlg, IDC_LANGEDIT, szBuffer);
	}
      switch (LOWORD (wParam))
	{
	case ID_CONFIRM:
	  ThotCallback (BaseDialog + ConfirmSaveList, INTEGER_DATA, (CHAR_T*)1);
	  EndDialog (hwnDlg, ID_CONFIRM);
	  SendMessage (currentDlg, WM_DESTROY, 0, 0);
	  break;
	  
	case IDCANCEL:
	  ThotCallback (BaseDialog + ConfirmSaveList, INTEGER_DATA, (CHAR_T*)0);
	  EndDialog (hwnDlg, IDCANCEL);
	  break;
	}
      break;    
    default:
      return FALSE;
    }
  return TRUE;
  
}

/*-----------------------------------------------------------------------
 CloseDocDlgProc
 ------------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK CloseDocDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK CloseDocDlgProc (hwnDlg, msg, wParam, lParam)
ThotWindow   hwndParent;
UINT   msg;
WPARAM wParam;
LPARAM lParam;
#endif /* __STDC__ */
{
  switch (msg)
    {
    case WM_INITDIALOG:
      SetWindowText (hwnDlg, TtaGetMessage (LIB, TMSG_CLOSE_DOC));
      SetWindowText (GetDlgItem (hwnDlg, IDC_CLOSEMSG), message);
      SetWindowText (GetDlgItem (hwnDlg, ID_SAVEDOC), TtaGetMessage (LIB, TMSG_SAVE_DOC));
      SetWindowText (GetDlgItem (hwnDlg, IDC_DONTSAVE), TtaGetMessage (LIB, TMSG_CLOSE_DON_T_SAVE));
      SetWindowText (GetDlgItem (hwnDlg, IDCANCEL), TtaGetMessage (LIB, TMSG_CANCEL));
      break;
      
    case WM_COMMAND:
      switch (LOWORD (wParam))
	{
	case IDCANCEL:
	  ThotCallback (NumFormClose, INTEGER_DATA, (CHAR_T*)0);
	  closeDontSave = TRUE;
	  saveBeforeClose = FALSE;
	  EndDialog (hwnDlg, IDCANCEL);
	  break;
	  
	case ID_SAVEDOC:
	  ThotCallback (NumFormClose, INTEGER_DATA, (CHAR_T*)1);
	  closeDontSave   = FALSE;
	  saveBeforeClose = TRUE;
	  EndDialog (hwnDlg, ID_SAVEDOC);
	  break;
	  
	case IDC_DONTSAVE:
	  ThotCallback (NumFormClose, INTEGER_DATA, (CHAR_T*)2);
	  closeDontSave   = FALSE;
	  saveBeforeClose = FALSE;
	  EndDialog (hwnDlg, IDC_DONTSAVE);
	  break;
	}
      break;
    default:
      return FALSE;
    }
  return TRUE;
}

/*-----------------------------------------------------------------------
 LanguageDlgProc
 ------------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK LanguageDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK LanguageDlgProc (hwnDlg, msg, wParam, lParam)
ThotWindow   hwndParent;
UINT   msg;
WPARAM wParam;
LPARAM lParam;
#endif /* __STDC__ */
{
  int  index = 0;
  UINT  i = 0; 
    
  switch (msg)
    {
    case WM_INITDIALOG:
      SetWindowText (hwnDlg, wndTitle);
	  SetWindowText (GetDlgItem (hwnDlg, ID_APPLY), TtaGetMessage (LIB, TMSG_APPLY));
	  SetWindowText (GetDlgItem (hwnDlg, ID_DELETE), TtaGetMessage (LIB, TMSG_DEL_ATTR));
	  SetWindowText (GetDlgItem (hwnDlg, ID_DONE), TtaGetMessage (LIB, TMSG_DONE));
	  SetWindowText (GetDlgItem (hwnDlg, IDC_LANGELEM), message);
	  SetWindowText (GetDlgItem (hwnDlg, IDC_INHERITEDLANG), message2);
      
      wndLangList = CreateWindow (TEXT("listbox"), NULL, WS_CHILD | WS_VISIBLE | LBS_STANDARD,
				  10, 40, 310, 200, hwnDlg, (HMENU) 1, 
				  (HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);
      
      SendMessage (wndLangList, LB_RESETCONTENT, 0, 0);
      while (i < nbItem && langList[index] != '\0')
	{
	  SendMessage (wndLangList, LB_INSERTSTRING, i, (LPARAM) &langList[index]); 
	  index += ustrlen (&langList[index]) + 1;/* Longueur de l'intitule */
	  i++;
      }
      SetWindowText (GetDlgItem (hwnDlg, IDC_LNGEDIT), winCurLang);
      break;
      
    case WM_COMMAND:
      if (LOWORD (wParam) == 1)
	{
	  if (HIWORD (wParam) == LBN_SELCHANGE)
	    {
	      itemIndex = SendMessage (wndLangList, LB_GETCURSEL, 0, 0);
	      itemIndex = SendMessage (wndLangList, LB_GETTEXT, itemIndex, (LPARAM) szBuffer);
	    }
	  else if (HIWORD (wParam) == LBN_DBLCLK)
	    {
	      if (LB_ERR == (itemIndex = SendMessage (wndLangList, LB_GETCURSEL, 0, 0L)))
		break;
	      itemIndex = SendMessage (wndLangList, LB_GETTEXT, itemIndex, (LPARAM) szBuffer);
	    }
	  SetDlgItemText (hwnDlg, IDC_LANGEDIT, szBuffer);
	  ThotCallback (NumSelectLanguage, STRING_DATA, szBuffer);
	  if (HIWORD (wParam) == LBN_DBLCLK)
	    {
	      ThotCallback (NumFormLanguage, INTEGER_DATA, (CHAR_T*) 1);
	      EndDialog (hwnDlg, ID_APPLY);
	      return 0;
	    }
	}
      
      switch (LOWORD (wParam))
	{
	case ID_APPLY:
	  ThotCallback (NumFormLanguage, INTEGER_DATA, (CHAR_T*) 1);
	  EndDialog (hwnDlg, ID_APPLY);
	  break;
	  
	case ID_DELETE:
	  ThotCallback (NumFormLanguage, INTEGER_DATA, (CHAR_T*) 2);
	  EndDialog (hwnDlg, ID_DELETE);
	  break;
	  
	case ID_DONE:
	  ThotCallback (NumFormLanguage, INTEGER_DATA, (CHAR_T*) 0);
	  EndDialog (hwnDlg, ID_DONE);
	  break;
	  
	case WM_CLOSE:
	case WM_DESTROY:
	  EndDialog (hwnDlg, ID_DONE);
	  break;
	}
      break;
    default: return FALSE;
    }
  return TRUE;
}

/*-----------------------------------------------------------------------
 CharacterDlgProc
 ------------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK CharacterDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK CharacterDlgProc (hwnDlg, msg, wParam, lParam)
ThotWindow   hwndParent;
UINT   msg;
WPARAM wParam;
LPARAM lParam;
#endif /* __STDC__ */
{
  switch (msg)
    {
    case WM_INITDIALOG:
      SetWindowText (hwnDlg, TtaGetMessage (LIB, TMSG_CHAR));
      SetWindowText (GetDlgItem (hwnDlg, IDC_FONTFAMILYGROUP), TtaGetMessage (LIB, TMSG_FONT_FAMILY));
      SetWindowText (GetDlgItem (hwnDlg, IDC_TIMES), TEXT("Times"));
      SetWindowText (GetDlgItem (hwnDlg, IDC_HELVETICA), TEXT("Helvetica"));
      SetWindowText (GetDlgItem (hwnDlg, IDC_COURIER), TEXT("Courrier"));
      SetWindowText (GetDlgItem (hwnDlg, IDC_DEFAULTFAMILY), TtaGetMessage (LIB, TMSG_UNCHANGED));
      
      SetWindowText (GetDlgItem (hwnDlg, IDC_CHARSTYLEGROUP), TtaGetMessage (LIB, TMSG_STYLE));
      SetWindowText (GetDlgItem (hwnDlg, IDC_ROMAN), TtaGetMessage (LIB, TMSG_ROMAN));
      SetWindowText (GetDlgItem (hwnDlg, IDC_ITALIC), TtaGetMessage (LIB, TMSG_ITALIC));
      SetWindowText (GetDlgItem (hwnDlg, IDC_OBLIQUE), TtaGetMessage (LIB, TMSG_OBLIQUE));
      SetWindowText (GetDlgItem (hwnDlg, IDC_DEFAULTSTYLE), TtaGetMessage (LIB, TMSG_UNCHANGED));
      
      SetWindowText (GetDlgItem (hwnDlg, IDC_UNDERLINEGROUP), TtaGetMessage (LIB, TMSG_LINE));
      SetWindowText (GetDlgItem (hwnDlg, IDC_NORMAL), TtaGetMessage (LIB, TMSG_NORMAL));
      SetWindowText (GetDlgItem (hwnDlg, IDC_UNDERLINE), TtaGetMessage (LIB, TMSG_UNDERLINE));
      SetWindowText (GetDlgItem (hwnDlg, IDC_OVERLINE), TtaGetMessage (LIB, TMSG_OVERLINE));
      SetWindowText (GetDlgItem (hwnDlg, IDC_CROSSOUT), TtaGetMessage (LIB, TMSG_CROSS_OUT));
      SetWindowText (GetDlgItem (hwnDlg, IDC_DEFAULTUNDERLINE), TtaGetMessage (LIB, TMSG_UNCHANGED));
      
      SetWindowText (GetDlgItem (hwnDlg, IDC_BOLDGROUP), TtaGetMessage (LIB, TMSG_BOLDNESS));
      SetWindowText (GetDlgItem (hwnDlg, IDC_NORMALBOLD), TtaGetMessage (LIB, TMSG_NOT_BOLD));
      SetWindowText (GetDlgItem (hwnDlg, IDC_BOLD), TtaGetMessage (LIB, TMSG_BOLD));
      SetWindowText (GetDlgItem (hwnDlg, IDC_DEFAULTBOLD), TtaGetMessage (LIB, TMSG_UNCHANGED));
      
      SetWindowText (GetDlgItem (hwnDlg, IDC_BODYSIZEGROUP), TtaGetMessage (LIB, TMSG_BODY_SIZE_PTS));
      SetWindowText (GetDlgItem (hwnDlg, IDC_06PT), TEXT(" 6 pt"));
      SetWindowText (GetDlgItem (hwnDlg, IDC_08PT), TEXT(" 8 pt"));
      SetWindowText (GetDlgItem (hwnDlg, IDC_10PT), TEXT("10 pt"));
      SetWindowText (GetDlgItem (hwnDlg, IDC_12PT), TEXT("12 pt"));
      SetWindowText (GetDlgItem (hwnDlg, IDC_14PT), TEXT("14 pt"));
      SetWindowText (GetDlgItem (hwnDlg, IDC_16PT), TEXT("16 pt"));
      SetWindowText (GetDlgItem (hwnDlg, IDC_20PT), TEXT("20 pt"));
      SetWindowText (GetDlgItem (hwnDlg, IDC_24PT), TEXT("24 pt"));
      SetWindowText (GetDlgItem (hwnDlg, IDC_30PT), TEXT("30 pt"));
      SetWindowText (GetDlgItem (hwnDlg, IDC_40PT), TEXT("40 pt"));
      SetWindowText (GetDlgItem (hwnDlg, IDC_60PT), TEXT("60 pt"));
      SetWindowText (GetDlgItem (hwnDlg, IDC_DEFAULTSIZE), TtaGetMessage (LIB, TMSG_UNCHANGED));
      
      SetWindowText (GetDlgItem (hwnDlg, ID_APPLY), TtaGetMessage (LIB, TMSG_APPLY));
      SetWindowText (GetDlgItem (hwnDlg, ID_DONE), TtaGetMessage (LIB, TMSG_DONE));
      
      switch (fontNum) {
      case 1:  CheckRadioButton (hwnDlg, IDC_TIMES, IDC_DEFAULTFAMILY, IDC_TIMES);
	break;
      case 2:  CheckRadioButton (hwnDlg, IDC_TIMES, IDC_DEFAULTFAMILY, IDC_HELVETICA);
	break;
      case 3:  CheckRadioButton (hwnDlg, IDC_TIMES, IDC_DEFAULTFAMILY, IDC_COURIER);
	break;
      default: CheckRadioButton (hwnDlg, IDC_TIMES, IDC_DEFAULTFAMILY, IDC_DEFAULTFAMILY);
      }
      /****  separer ce menu en deux, en utilisant la varable fontWeight *****/
      switch (fontStyle) {
      case 0:  CheckRadioButton (hwnDlg, IDC_ROMAN, IDC_DEFAULTSTYLE, IDC_ROMAN);
	break;
      case 1:  CheckRadioButton (hwnDlg, IDC_ROMAN, IDC_DEFAULTSTYLE, IDC_ITALIC);
	break;
      case 2:  CheckRadioButton (hwnDlg, IDC_ROMAN, IDC_DEFAULTSTYLE, IDC_OBLIQUE);
	break;
      default: CheckRadioButton (hwnDlg, IDC_ROMAN, IDC_DEFAULTSTYLE, IDC_DEFAULTSTYLE);
	break;
      }
      
      switch (fontWeight) {
      case 0:  CheckRadioButton (hwnDlg, IDC_ROMAN, IDC_DEFAULTSTYLE, IDC_NORMALBOLD);
	break;
      case 1:  CheckRadioButton (hwnDlg, IDC_ROMAN, IDC_DEFAULTSTYLE, IDC_BOLD);
	break;
      default: CheckRadioButton (hwnDlg, IDC_ROMAN, IDC_DEFAULTSTYLE, IDC_DEFAULTSTYLE);
	break;
      }
      
      switch (fontUnderline) {
      case 0:  CheckRadioButton (hwnDlg, IDC_NORMAL, IDC_DEFAULTUNDERLINE, IDC_NORMAL);
	break;
      case 1:  CheckRadioButton (hwnDlg, IDC_NORMAL, IDC_DEFAULTUNDERLINE, IDC_UNDERLINE);
	break;
      case 2:  CheckRadioButton (hwnDlg, IDC_NORMAL, IDC_DEFAULTUNDERLINE, IDC_OVERLINE);
	break;
      case 3:  CheckRadioButton (hwnDlg, IDC_NORMAL, IDC_DEFAULTUNDERLINE, IDC_CROSSOUT);
	break;
      default: CheckRadioButton (hwnDlg, IDC_NORMAL, IDC_DEFAULTUNDERLINE, IDC_DEFAULTUNDERLINE);
	break;
      }
      
      switch (fontSize) {
      case  6:  CheckRadioButton (hwnDlg, IDC_06PT, IDC_DEFAULTSIZE, IDC_06PT);
	break;
      case  8:  CheckRadioButton (hwnDlg, IDC_06PT, IDC_DEFAULTSIZE, IDC_08PT);
	break;
      case 10:  CheckRadioButton (hwnDlg, IDC_06PT, IDC_DEFAULTSIZE, IDC_10PT);
	break;
      case 12:  CheckRadioButton (hwnDlg, IDC_06PT, IDC_DEFAULTSIZE, IDC_12PT);
	break;
      case 14:  CheckRadioButton (hwnDlg, IDC_06PT, IDC_DEFAULTSIZE, IDC_14PT);
	break;
      case 16:  CheckRadioButton (hwnDlg, IDC_06PT, IDC_DEFAULTSIZE, IDC_16PT);
	break;
      case 20:  CheckRadioButton (hwnDlg, IDC_06PT, IDC_DEFAULTSIZE, IDC_20PT);
	break;
      case 24:  CheckRadioButton (hwnDlg, IDC_06PT, IDC_DEFAULTSIZE, IDC_24PT);
	break;
      case 30:  CheckRadioButton (hwnDlg, IDC_06PT, IDC_DEFAULTSIZE, IDC_30PT);
	break;
      case 40:  CheckRadioButton (hwnDlg, IDC_06PT, IDC_DEFAULTSIZE, IDC_40PT);
	break;
      case 60:  CheckRadioButton (hwnDlg, IDC_06PT, IDC_DEFAULTSIZE, IDC_60PT);
	break;
      default:  CheckRadioButton (hwnDlg, IDC_06PT, IDC_DEFAULTSIZE, IDC_DEFAULTSIZE);
	break;
      }
      break;

    case WM_COMMAND:
      switch (LOWORD (wParam)) {
      case ID_DONE:
	ThotCallback (NumFormPresChar, INTEGER_DATA, (CHAR_T*) 0);
	EndDialog (hwnDlg, ID_DONE);
	break;
      case WM_CLOSE:
      case WM_DESTROY:
	EndDialog (hwnDlg, ID_DONE);
	break;
      case ID_APPLY:
	ThotCallback (NumFormPresChar, INTEGER_DATA, (CHAR_T*) 1);
	break;
      case IDC_TIMES:
	ThotCallback (NumMenuCharFamily, INTEGER_DATA, (CHAR_T*) 0);
	break;
      case IDC_HELVETICA:
	ThotCallback (NumMenuCharFamily, INTEGER_DATA, (CHAR_T*) 1);
	break;
      case IDC_COURIER:
	ThotCallback (NumMenuCharFamily, INTEGER_DATA, (CHAR_T*) 2);
	break;
      case IDC_DEFAULTFAMILY:
	ThotCallback (NumMenuCharFamily, INTEGER_DATA, (CHAR_T*) 3);
	break;
      case IDC_ROMAN:
	ThotCallback (NumMenuCharFontStyle, INTEGER_DATA, (CHAR_T*) 0);
	break;
      case IDC_ITALIC:
	ThotCallback (NumMenuCharFontStyle, INTEGER_DATA, (CHAR_T*) 1);
	break;
      case IDC_OBLIQUE:
	ThotCallback (NumMenuCharFontStyle, INTEGER_DATA, (CHAR_T*) 2);
	break;
      case IDC_DEFAULTSTYLE:
	ThotCallback (NumMenuCharFontStyle, INTEGER_DATA, (CHAR_T*) 3);
	break;
      case IDC_NORMALBOLD:
	ThotCallback (NumMenuCharFontWeight, INTEGER_DATA, (CHAR_T*) 0);
	break;
      case IDC_BOLD:
	ThotCallback (NumMenuCharFontWeight, INTEGER_DATA, (CHAR_T*) 1);
	break;
      case IDC_DEFAULTBOLD:
	ThotCallback (NumMenuCharFontWeight, INTEGER_DATA, (CHAR_T*) 2);
	break;
      case IDC_NORMAL:
	ThotCallback (NumMenuUnderlineType, INTEGER_DATA, (CHAR_T*) 0);
	break;
      case IDC_UNDERLINE:
	ThotCallback (NumMenuUnderlineType, INTEGER_DATA, (CHAR_T*) 1);
	break;
	ThotCallback (NumMenuUnderlineType, INTEGER_DATA, (CHAR_T*) 2);
	break;
      case IDC_CROSSOUT:
	ThotCallback (NumMenuUnderlineType, INTEGER_DATA, (CHAR_T*) 3);
	break;
      case IDC_DEFAULTUNDERLINE:
	ThotCallback (NumMenuUnderlineType, INTEGER_DATA, (CHAR_T*) 4);
	break;
      case IDC_06PT:
	ThotCallback (NumMenuCharFontSize, INTEGER_DATA, (CHAR_T*) 0);
	break;
      case IDC_08PT:
	ThotCallback (NumMenuCharFontSize, INTEGER_DATA, (CHAR_T*) 1);
	break;
      case IDC_10PT:
	ThotCallback (NumMenuCharFontSize, INTEGER_DATA, (CHAR_T*) 2);
	break;
      case IDC_12PT:
	ThotCallback (NumMenuCharFontSize, INTEGER_DATA, (CHAR_T*) 3);
	break;
      case IDC_14PT:
	ThotCallback (NumMenuCharFontSize, INTEGER_DATA, (CHAR_T*) 4);
	break;
      case IDC_16PT:
	ThotCallback (NumMenuCharFontSize, INTEGER_DATA, (CHAR_T*) 5);
	break;
      case IDC_20PT:
	ThotCallback (NumMenuCharFontSize, INTEGER_DATA, (CHAR_T*) 6);
	break;
      case IDC_24PT:
	ThotCallback (NumMenuCharFontSize, INTEGER_DATA, (CHAR_T*) 7);
	break;
      case IDC_30PT:
	ThotCallback (NumMenuCharFontSize, INTEGER_DATA, (CHAR_T*) 8);
	break;
      case IDC_40PT:
	ThotCallback (NumMenuCharFontSize, INTEGER_DATA, (CHAR_T*) 9);
	break;
      case IDC_60PT:
	ThotCallback (NumMenuCharFontSize, INTEGER_DATA, (CHAR_T*) 10);
	break;
      case IDC_DEFAULTSIZE:
	ThotCallback (NumMenuCharFontSize, INTEGER_DATA, (CHAR_T*) 11);
	break;
      }
      break;
      
    default:
      return FALSE;
    }
  return TRUE;
}


/*-----------------------------------------------------------------------
 ------------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK MathEntityDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK MathEntityDlgProc (hwnDlg, msg, wParam, lParam)
ThotWindow   hwndParent;
UINT   msg;
WPARAM wParam;
LPARAM lParam;
#endif /* !__STDC__ */
{
  switch (msg)
    {
    case WM_INITDIALOG:
      SetWindowText (hwnDlg, TtaGetMessage (1, BMCharacter));
      SetWindowText (GetDlgItem (hwnDlg, IDC_ENTITY_NAME), TtaGetMessage (AMAYA, AM_MATH_ENTITY_NAME));
      SetWindowText (GetDlgItem (hwnDlg, ID_CONFIRM), TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
      SetWindowText (GetDlgItem (hwnDlg, IDCANCEL), TtaGetMessage (LIB, TMSG_CANCEL));
      break;
      
    case WM_COMMAND:
      switch (LOWORD (wParam))
	{
	case ID_CONFIRM:
	  GetDlgItemText (hwnDlg, IDC_EDIT_NAME, mathEntityName, sizeof (mathEntityName) - 1);
	  ThotCallback (BaseDialog + MathEntityText, STRING_DATA, mathEntityName);
	  ThotCallback (BaseDialog + MathEntityForm, INTEGER_DATA, (CHAR_T*) 1);
	  EndDialog (hwnDlg, ID_CONFIRM);
	  break;
	  
	case IDCANCEL:
	  EndDialog (hwnDlg, IDCANCEL);
	  ThotCallback (BaseDialog + MathEntityForm, INTEGER_DATA, (CHAR_T*) 0);
	}
    default:
      return FALSE; 
    }
  return TRUE;
}

/*-----------------------------------------------------------------------
 ApplyClassDlgProc
 ------------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK ApplyClassDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK ApplyClassDlgProc (hwnDlg, msg, wParam, lParam)
ThotWindow   hwndParent;
UINT   msg;
WPARAM wParam;
LPARAM lParam;
#endif /* __STDC__ */
{
  int   index = 0;
  UINT  i = 0;

  switch (msg)
    {
    case WM_INITDIALOG:
      if (WithEdit)
	  {
        SetWindowText (hwnDlg, TtaGetMessage (AMAYA, AM_DEF_CLASS));
	    SetWindowText (GetDlgItem (hwnDlg, ID_CONFIRM), TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
	  }
      else
	  {
        SetWindowText (hwnDlg, TtaGetMessage (AMAYA, AM_APPLY_CLASS));
	    SetWindowText (GetDlgItem (hwnDlg, ID_CONFIRM), TtaGetMessage (LIB, TMSG_APPLY));
	  }
      SetWindowText (GetDlgItem (hwnDlg, ID_DONE), TtaGetMessage (LIB, TMSG_DONE));
      
      wndListRule = CreateWindow (TEXT("listbox"), NULL, WS_CHILD | WS_VISIBLE | LBS_STANDARD,
				  10, 10, 200, 120, hwnDlg, (HMENU) 1, 
				  (HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);
      
      SendMessage (wndListRule, LB_RESETCONTENT, 0, 0);
      while (i < nbClass && classList[index] != EOS)
	{
	  SendMessage (wndListRule, LB_INSERTSTRING, i, (LPARAM) &classList[index]); 
	  index += ustrlen (&classList[index]) + 1;	/* entry length */
	  i++;
	}
      if (WithEdit)
	{
	  wndEditRule	= CreateWindow (TEXT("EDIT"), NULL, WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER,
					10, 130, 200, 30, hwnDlg, (HMENU) IDC_EDITRULE, 
					(HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);
	  
	  SetDlgItemText (hwnDlg, IDC_EDITRULE, classList);
	}
      break;

    case WM_COMMAND:
      if (LOWORD (wParam) == 1 && HIWORD (wParam) == LBN_SELCHANGE)
	{
	  itemIndex = SendMessage (wndListRule, LB_GETCURSEL, 0, 0);
	  itemIndex = SendMessage (wndListRule, LB_GETTEXT, itemIndex, (LPARAM) szBuffer);
	  SetDlgItemText (hwnDlg, IDC_EDITRULE, szBuffer);
	  if (WithEdit)
	  ThotCallback (BaseDialog + ClassSelect, STRING_DATA, szBuffer);
	  else
	  ThotCallback (BaseDialog + AClassSelect, STRING_DATA, szBuffer);
	}
      else if (LOWORD (wParam) == 1 && HIWORD (wParam) == LBN_DBLCLK)
	{
	  if (LB_ERR == (itemIndex = SendMessage (wndListRule, LB_GETCURSEL, 0, 0L)))
	    break;
	  itemIndex = SendMessage (wndListRule, LB_GETTEXT, itemIndex, (LPARAM) szBuffer);
	  SetDlgItemText (hwnDlg, IDC_EDITRULE, szBuffer);
	  if (WithEdit)
	  {
	    ThotCallback (BaseDialog + ClassSelect, STRING_DATA, szBuffer);
	    ThotCallback (BaseDialog + ClassForm, INTEGER_DATA, (CHAR_T*) 1);
	  }
	  else
	  {
	    ThotCallback (BaseDialog + AClassSelect, STRING_DATA, szBuffer);
	    ThotCallback (BaseDialog + AClassForm, INTEGER_DATA, (CHAR_T*) 1);
	  }
	  EndDialog (hwnDlg, ID_CONFIRM);
	  return FALSE;
	}
      else if (WithEdit && HIWORD (wParam) == EN_UPDATE)
	{
	  GetDlgItemText (hwnDlg, IDC_EDITRULE, szBuffer, sizeof (szBuffer) - 1);
	  ThotCallback (BaseDialog + ClassSelect, STRING_DATA, szBuffer);
	}

      switch (LOWORD (wParam))
	{
	case ID_CONFIRM:
	  if (WithEdit)
	  {
	    ThotCallback (BaseDialog + ClassForm, INTEGER_DATA, (CHAR_T*) 1);
	    EndDialog (hwnDlg, ID_CONFIRM);
	  }
	  else
	  {
	    ThotCallback (BaseDialog + AClassForm, INTEGER_DATA, (CHAR_T*) 1);
      return FALSE;
	  }
	  break;
	  
	case ID_DONE:
	  if (WithEdit)
	    ThotCallback (BaseDialog + ClassForm, INTEGER_DATA, (CHAR_T*) 0);
	  else
	    ThotCallback (BaseDialog + AClassForm, INTEGER_DATA, (CHAR_T*) 0);
	  EndDialog (hwnDlg, ID_DONE);
	  break;
	  
	case WM_CLOSE:
	case WM_DESTROY:
	  EndDialog (hwnDlg, ID_DONE);
	  break;
	}
      break;
    default:
      return FALSE;
    }
  return TRUE;
}

/*-----------------------------------------------------------------------
 SpellCheckDlgProc
 ------------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK SpellCheckDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK SpellCheckDlgProc (hwnDlg, msg, wParam, lParam)
ThotWindow   hwndParent;
UINT   msg;
WPARAM wParam;
LPARAM lParam;
#endif /* __STDC__ */
{
  ThotBool ok;	  
  int  val;

  static int  iLocation;
  static int  iIgnore;

  switch (msg)
    {
    case WM_INITDIALOG:
      hwndLanguage = GetDlgItem (hwnDlg, IDC_LANG);
      wordButton = GetDlgItem (hwnDlg, IDC_CURWORD);
      SetWindowText (GetDlgItem (hwnDlg, IDC_LABEL), currentLabel);
      SetWindowText (GetDlgItem (hwnDlg, IDC_BEFORE), TtaGetMessage (LIB, TMSG_BEFORE_SEL));
      SetWindowText (GetDlgItem (hwnDlg, IDC_WITHIN), TtaGetMessage (LIB, TMSG_WITHIN_SEL));
      SetWindowText (GetDlgItem (hwnDlg, IDC_AFTER), TtaGetMessage (LIB, TMSG_AFTER_SEL));
      SetWindowText (GetDlgItem (hwnDlg, IDC_WHOLEDOC), TtaGetMessage (LIB, TMSG_IN_WHOLE_DOC));
      SetWindowText (GetDlgItem (hwnDlg, IDC_NBPROPOSALS), TtaGetMessage (CORR, Number_Propositions));
      
      SetWindowText (GetDlgItem (hwnDlg, ID_SKIPNEXT), TtaGetMessage (CORR, Pass_Without));
      SetWindowText (GetDlgItem (hwnDlg, ID_SKIPDIC), TtaGetMessage (CORR, Pass_With));
      SetWindowText (GetDlgItem (hwnDlg, ID_REPLACENEXT), TtaGetMessage (CORR, Replace_Without));
      SetWindowText (GetDlgItem (hwnDlg, ID_REPLACEDIC), TtaGetMessage(CORR, Replace_With));
      
      SetWindowText (GetDlgItem (hwnDlg, IDC_IGNORE1), TtaGetMessage (CORR, Capitals));
      SetWindowText (GetDlgItem (hwnDlg, IDC_IGNORE2), TtaGetMessage (CORR, Arabics));
      SetWindowText (GetDlgItem (hwnDlg, IDC_IGNORE3), TtaGetMessage (CORR, Romans));
      SetWindowText (GetDlgItem (hwnDlg, IDC_IGNORE4), TtaGetMessage (CORR, Specials));
      
      SetWindowText (GetDlgItem (hwnDlg, IDC_CHECKGROUP), TtaGetMessage (CORR, What));
      
      SetWindowText (GetDlgItem (hwnDlg, IDC_IGNOREGROUP), TtaGetMessage (CORR, Ignore));
      
      SetWindowText (GetDlgItem (hwnDlg, ID_DONE), TtaGetMessage (LIB, TMSG_DONE));
      
      hwnListWords = CreateWindow (TEXT("listbox"), NULL, WS_CHILD | WS_VISIBLE | LBS_STANDARD,
				   13, 72, 150, 70, hwnDlg, (HMENU) 1, 
				   (HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);
      
      hwndCurrentWord = CreateWindow (TEXT("EDIT"), NULL, WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER,
				      13, 146, 150, 20, hwnDlg, (HMENU) IDC_LANGEDIT, 
				      (HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);
      
      
      CheckRadioButton (hwnDlg, IDC_BEFORE, IDC_WHOLEDOC, IDC_AFTER);
      SetDlgItemInt (hwnDlg, IDC_EDITPROPOSALS, 3, FALSE);
      SetDlgItemText (hwnDlg, IDC_EDITIGNORE, currentRejectedchars);
      iLocation = 2;
      SetWindowText (hwndCurrentWord, TEXT(""));
      WIN_DisplayWords ();
      break;
      
    case WM_COMMAND:
      if (LOWORD (wParam) == 1 && HIWORD (wParam) == LBN_SELCHANGE) {
	itemIndex = SendMessage (hwnListWords, LB_GETCURSEL, 0, 0);
	itemIndex = SendMessage (hwnListWords, LB_GETTEXT, itemIndex, (LPARAM) currentWord);
	SetDlgItemText (hwnDlg, IDC_LANGEDIT, currentWord);
      } else if (LOWORD (wParam) == 1 && HIWORD (wParam) == LBN_DBLCLK) {
	if (LB_ERR == (itemIndex = SendMessage (hwnListWords, LB_GETCURSEL, 0, 0L)))
	  break;
	itemIndex = SendMessage (hwnListWords, LB_GETTEXT, itemIndex, (LPARAM) currentWord);
	SetDlgItemText (hwnDlg, IDC_LANGEDIT, currentWord);
	ThotCallback (SpellingBase + ChkrSelectProp, STRING_DATA, currentWord);
	ThotCallback (SpellingBase + ChkrMenuOR, INTEGER_DATA, (CHAR_T*) iLocation);
	ThotCallback (SpellingBase + ChkrFormCorrect, INTEGER_DATA, (CHAR_T*) 3);
	if (iLocation == 3) {
	  CheckRadioButton (hwnDlg, IDC_BEFORE, IDC_WHOLEDOC, IDC_AFTER);
	  iLocation = 2;
	}
	return 0;
      } 
      if (HIWORD (wParam) == EN_UPDATE)
	{
	  if (LOWORD (wParam) == IDC_EDITPROPOSALS)
	    {
	      val = GetDlgItemInt (hwnDlg, IDC_EDITPROPOSALS, &ok, TRUE);
	      if (ok)
		ThotCallback (SpellingBase + ChkrCaptureNC, INTEGER_DATA, (CHAR_T*) val);
	    }
	  else if (LOWORD (wParam) == IDC_EDITIGNORE)
	    {
	      GetDlgItemText (hwnDlg, IDC_EDITIGNORE, currentRejectedchars, sizeof (currentRejectedchars) + 1);
	      ThotCallback (SpellingBase + ChkrSpecial, STRING_DATA, currentRejectedchars);
	    }
	  else if (LOWORD (wParam) == IDC_LANGEDIT) 
	    GetDlgItemText (hwnDlg, IDC_LANGEDIT, currentWord, sizeof (currentWord) + 1);
	}
      
      switch (LOWORD (wParam))
	{
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
	  ThotCallback (SpellingBase + ChkrMenuIgnore, INTEGER_DATA, (CHAR_T*) 0);
	  break;
	  
	case IDC_IGNORE2:
	  ThotCallback (SpellingBase + ChkrMenuIgnore, INTEGER_DATA, (CHAR_T*) 1);
	  break;
	  
	case IDC_IGNORE3: 
	  ThotCallback (SpellingBase + ChkrMenuIgnore, INTEGER_DATA, (CHAR_T*) 2);
	  break;
	  
	case IDC_IGNORE4:
	  ThotCallback (SpellingBase + ChkrMenuIgnore, INTEGER_DATA, (CHAR_T*) 3);
	  break;
	  
	case ID_SKIPNEXT:
	  ThotCallback (SpellingBase + ChkrSelectProp, STRING_DATA, currentWord);
	  ThotCallback (SpellingBase + ChkrMenuOR, INTEGER_DATA, (CHAR_T*) iLocation);
	  ThotCallback (SpellingBase + ChkrFormCorrect, INTEGER_DATA, (CHAR_T*) 1);
	  if (iLocation == 3) {
	    CheckRadioButton (hwnDlg, IDC_BEFORE, IDC_WHOLEDOC, IDC_AFTER);
	    iLocation = 2;
	  }
	  break;
	  
	case ID_SKIPDIC:
	  ThotCallback (SpellingBase + ChkrSelectProp, STRING_DATA, currentWord);
	  ThotCallback (SpellingBase + ChkrMenuOR, INTEGER_DATA, (CHAR_T*) iLocation);
	  ThotCallback (SpellingBase + ChkrFormCorrect, INTEGER_DATA, (CHAR_T*) 2);
	  if (iLocation == 3)
	    {
	      CheckRadioButton (hwnDlg, IDC_BEFORE, IDC_WHOLEDOC, IDC_AFTER);
	      iLocation = 2;
	    }
	  break;
	  
	case ID_REPLACENEXT:
	  ThotCallback (SpellingBase + ChkrSelectProp, STRING_DATA, currentWord);
	  ThotCallback (SpellingBase + ChkrMenuOR, INTEGER_DATA, (CHAR_T*) iLocation);
	  ThotCallback (SpellingBase + ChkrFormCorrect, INTEGER_DATA, (CHAR_T*) 3);
	  if (iLocation == 3)
	    {
	      CheckRadioButton (hwnDlg, IDC_BEFORE, IDC_WHOLEDOC, IDC_AFTER);
	      iLocation = 2;
	    }
	  break;
	  
	case ID_REPLACEDIC:
	  ThotCallback (SpellingBase + ChkrSelectProp, STRING_DATA, currentWord);
	  ThotCallback (SpellingBase + ChkrMenuOR, INTEGER_DATA, (CHAR_T*) iLocation);
	  ThotCallback (SpellingBase + ChkrFormCorrect, INTEGER_DATA, (CHAR_T*) 4);
	  if (iLocation == 3)
	    {
	      CheckRadioButton (hwnDlg, IDC_BEFORE, IDC_WHOLEDOC, IDC_AFTER);
	      iLocation = 2;
	    }
	  break;
	  
	case IDC_WORDBUTTON:
	  GetWindowText (wordButton, currentWord, MAX_WORD_LEN);
	  SetWindowText (hwndCurrentWord, currentWord);
	  break;
	  
	case ID_DONE:
	  EndDialog (hwnDlg, ID_DONE);
	  break;
	  
	case WM_CLOSE:
	case WM_DESTROY:
	  EndDialog (hwnDlg, ID_DONE);
	  break;
	}
      break;
    default: return FALSE;
    }
  return TRUE;
}

/*-----------------------------------------------------------------------
 InitConfirmDlgProc
 ------------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK InitConfirmDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK InitConfirmDlgProc (hwnDlg, msg, wParam, lParam)
ThotWindow   hwndParent;
UINT   msg;
WPARAM wParam;
LPARAM lParam;
#endif /* __STDC__ */
{
  ThotWindow messageWnd;

  switch (msg)
    {
    case WM_INITDIALOG:
      SetWindowText (hwnDlg, wndTitle);
      SetWindowText (GetDlgItem (hwnDlg, ID_CONFIRM), TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
      SetWindowText (GetDlgItem (hwnDlg, IDCANCEL), TtaGetMessage (LIB, TMSG_CANCEL));
      
      messageWnd = CreateWindow (TEXT("STATIC"), message, WS_CHILD | WS_VISIBLE | SS_LEFT,
				 15, 10, 303, 60, hwnDlg, (HMENU) 99, 
				 (HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);
      break; 
      
    case WM_COMMAND:
      switch (LOWORD (wParam))
	{
	case ID_CONFIRM:
	  EndDialog (hwnDlg, ID_CONFIRM);
	  ThotCallback (currentRef, INTEGER_DATA, (CHAR_T*) 1);
	  break;

	case IDCANCEL:
	  EndDialog (hwnDlg, IDCANCEL);
	  ThotCallback (currentRef, INTEGER_DATA, (CHAR_T*) 0);
	  break;
	}
      break;
      
    default: return FALSE;
    }
  return TRUE;
}

/*-----------------------------------------------------------------------
 InitConfirm3LDlgProc
 ------------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK InitConfirm3LDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK InitConfirm3LDlgProc (hwnDlg, msg, wParam, lParam)
ThotWindow   hwndParent;
UINT   msg;
WPARAM wParam;
LPARAM lParam;
#endif /* __STDC__ */
{
    switch (msg)
      {
      case WM_INITDIALOG:
	SetWindowText (hwnDlg, wndTitle);
	SetWindowText (GetDlgItem (hwnDlg, ID_CONFIRM), TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
	if (WithCancel)
	  SetWindowText (GetDlgItem (hwnDlg, IDCANCEL), TtaGetMessage (LIB, TMSG_CANCEL));
	if (message)
	  SetWindowText (GetDlgItem (hwnDlg, IDC_MESSAGE1), message);
	if (message2)
	  SetWindowText (GetDlgItem (hwnDlg, IDC_MESSAGE2), message2);
	if (message3)
	  SetWindowText (GetDlgItem (hwnDlg, IDC_MESSAGE3), message3);
	break; 
	
      case WM_COMMAND:
	switch (LOWORD (wParam))
	  {
	  case ID_CONFIRM:
	    EndDialog (hwnDlg, ID_CONFIRM);
	    ThotCallback (currentRef, INTEGER_DATA, (CHAR_T*) 1);
	    break;
	    
	  case IDCANCEL:
	    EndDialog (hwnDlg, IDCANCEL);
	    ThotCallback (currentRef, INTEGER_DATA, (CHAR_T*) 0);
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
LRESULT CALLBACK ChangeFormatDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK ChangeFormatDlgProc (hwnDlg, msg, wParam, lParam)
ThotWindow   hwndParent;
UINT   msg;
WPARAM wParam;
LPARAM lParam;
#endif /* __STDC__ */
{
  ThotBool ok;	  
  int  val;

  switch (msg) {
  case WM_INITDIALOG:
       SetWindowText (hwnDlg, TtaGetMessage (LIB, TMSG_FORMAT));
	   SetWindowText (GetDlgItem (hwnDlg, IDCALIGNGROUP), TtaGetMessage (LIB, TMSG_ALIGN));
	   SetWindowText (GetDlgItem (hwnDlg, IDC_DEFAULTALIGN), TtaGetMessage (LIB, TMSG_UNCHANGED));

	   SetWindowText (GetDlgItem (hwnDlg, IDC_INDENTGROUP), TtaGetMessage (LIB, TMSG_INDENT));
	   SetWindowText (GetDlgItem (hwnDlg, IDC_INDENTDEFAULT), TtaGetMessage (LIB, TMSG_UNCHANGED));
	   SetWindowText (GetDlgItem (hwnDlg, IDC_INDENTPT), TtaGetMessage (LIB, TMSG_INDENT_PTS));

	   SetWindowText (GetDlgItem (hwnDlg, IDC_JUSTIFGROUP), TtaGetMessage (LIB, TMSG_JUSTIFY));
	   SetWindowText (GetDlgItem (hwnDlg, IDC_JUSTIFYES), TtaGetMessage (LIB, TMSG_LIB_YES));
	   SetWindowText (GetDlgItem (hwnDlg, IDC_JUSTIFNO), TtaGetMessage (LIB, TMSG_LIB_NO));
	   SetWindowText (GetDlgItem (hwnDlg, IDC_JUSTIFDEFAULT), TtaGetMessage (LIB, TMSG_UNCHANGED));

	   SetWindowText (GetDlgItem (hwnDlg, IDC_LINESPACEGROUP), TtaGetMessage (LIB, TMSG_LINE_SPACING));
	   SetWindowText (GetDlgItem (hwnDlg, IDC_LINESPACINGPT), TtaGetMessage (LIB, TMSG_LINE_SPACING_PTS));
       SetWindowText (GetDlgItem (hwnDlg, IDC_SPACINGDEFAULT), TtaGetMessage (LIB, TMSG_UNCHANGED));

	   SetWindowText (GetDlgItem (hwnDlg, ID_APPLY), TtaGetMessage (LIB, TMSG_APPLY));
	   SetWindowText (GetDlgItem (hwnDlg, ID_DONE), TtaGetMessage (LIB, TMSG_DONE));
    if (Align_num  == 0)
      CheckRadioButton (hwnDlg, IDC_LEFT, IDC_DEFAULTALIGN, IDC_LEFT);
    else if (Align_num  == 1)
      CheckRadioButton (hwnDlg, IDC_LEFT, IDC_DEFAULTALIGN, IDC_RIGHT);
    else if (Align_num  == 2)
      CheckRadioButton (hwnDlg, IDC_LEFT, IDC_DEFAULTALIGN, IDC_CENTER);
    
    if (Indent_num == 0)
      CheckRadioButton (hwnDlg, IDC_INDENT1, IDC_INDENTDEFAULT, IDC_INDENT1);
    else if (Indent_num == 1)
      CheckRadioButton (hwnDlg, IDC_INDENT1, IDC_INDENTDEFAULT, IDC_INDENT2);
    
    if (Justification_num == 0)
      CheckRadioButton (hwnDlg, IDC_JUSTIFYES, IDC_JUSTIFDEFAULT, IDC_JUSTIFYES);
    else if (Justification_num == 1)
      CheckRadioButton (hwnDlg, IDC_JUSTIFYES, IDC_JUSTIFDEFAULT, IDC_JUSTIFNO);
    
    if (Line_spacingNum == 0)
      CheckRadioButton (hwnDlg, IDC_SSMALL, IDC_SPACINGDEFAULT, IDC_SSMALL);
    else if (Line_spacingNum == 1)
      CheckRadioButton (hwnDlg, IDC_SSMALL, IDC_SPACINGDEFAULT, IDC_SMEDIUM);
    else if (Line_spacingNum == 2)
      CheckRadioButton (hwnDlg, IDC_SSMALL, IDC_SPACINGDEFAULT, IDC_SLARGE);
    
    SetDlgItemInt (hwnDlg, IDC_INDENTPTEDIT, Indent_value, FALSE);
    SetDlgItemInt (hwnDlg, IDC_LINESPACINGEDIT, Old_lineSp, FALSE);
    break;
    
  case WM_COMMAND:
    if (HIWORD (wParam) == EN_UPDATE) {
      if (LOWORD (wParam) == IDC_INDENTPTEDIT) {
	val = GetDlgItemInt (hwnDlg, IDC_INDENTPTEDIT, &ok, TRUE);
	if (ok)
	  ThotCallback (Num_zoneRecess, INTEGER_DATA, (CHAR_T*) val);
      } else if (LOWORD (wParam) == IDC_LINESPACINGEDIT) {
	val = GetDlgItemInt (hwnDlg, IDC_LINESPACINGEDIT, &ok, TRUE);
	if (ok)
	  ThotCallback (Num_zoneLineSpacing, INTEGER_DATA, (CHAR_T*) val);
      }
    }
    switch (LOWORD (wParam)) {
      /* Alignement menu */
    case ID_APPLY:
      ThotCallback (NumFormPresFormat, INTEGER_DATA, (CHAR_T*) 1);
      break;
      
    case WM_CLOSE:
    case WM_DESTROY:
      EndDialog (hwnDlg, ID_DONE);
      break;
      
    case ID_DONE:
      ThotCallback (NumFormPresFormat, INTEGER_DATA, (CHAR_T*) 0);
      EndDialog (hwnDlg, ID_DONE);
      break;
      
    case IDC_BLEFT:
      ThotCallback (NumMenuAlignment, INTEGER_DATA, (CHAR_T*) 0);
      CheckRadioButton (hwnDlg, IDC_LEFT, IDC_DEFAULTALIGN, IDC_LEFT);
      break;
      
    case IDC_LEFT:
      ThotCallback (NumMenuAlignment, INTEGER_DATA, (CHAR_T*) 0);
      break;
      
    case IDC_BRIGHT:
      CheckRadioButton (hwnDlg, IDC_LEFT, IDC_DEFAULTALIGN, IDC_RIGHT);
      ThotCallback (NumMenuAlignment, INTEGER_DATA, (CHAR_T*) 1);
      break;
      
    case IDC_RIGHT:
      ThotCallback (NumMenuAlignment, INTEGER_DATA, (CHAR_T*) 1);
      break;
      
    case IDC_BCENTER:
      CheckRadioButton (hwnDlg, IDC_LEFT, IDC_DEFAULTALIGN, IDC_CENTER);
      ThotCallback (NumMenuAlignment, INTEGER_DATA, (CHAR_T*) 2);
      break;
      
    case IDC_CENTER:
      ThotCallback (NumMenuAlignment, INTEGER_DATA, (CHAR_T*) 2);
      break;
      
    case IDC_DEFAULTALIGN:
      ThotCallback (NumMenuAlignment, INTEGER_DATA, (CHAR_T*) 3);
      break;
      
      /* Jusitification menu */
    case IDC_JUSTIFYES:
      ThotCallback (NumMenuJustification, INTEGER_DATA, (CHAR_T*) 0);
      break;
      
    case IDC_JUSTIFNO:
      ThotCallback (NumMenuJustification, INTEGER_DATA, (CHAR_T*) 1);
      break;
      
    case IDC_JUSTIFDEFAULT:
      ThotCallback (NumMenuJustification, INTEGER_DATA, (CHAR_T*) 2);
      break;
      
      /* Indent Menu */ 
    case IDC_INDENT1:
      ThotCallback (NumMenuRecessSense, INTEGER_DATA, (CHAR_T*) 0);
      SetDlgItemInt (hwnDlg, IDC_INDENTPTEDIT, WIN_IndentValue, FALSE);
      break;
      
    case IDC_BINDENT1:
      CheckRadioButton (hwnDlg, IDC_INDENT1, IDC_INDENTDEFAULT, IDC_INDENT1);
      ThotCallback (NumMenuRecessSense, INTEGER_DATA, (CHAR_T*) 0);
      SetDlgItemInt (hwnDlg, IDC_INDENTPTEDIT, WIN_IndentValue, FALSE);
      break;
      
    case IDC_INDENT2:
      ThotCallback (NumMenuRecessSense, INTEGER_DATA, (CHAR_T*) 1);
      SetDlgItemInt (hwnDlg, IDC_INDENTPTEDIT, WIN_IndentValue, FALSE);
      break;
      
    case IDC_BINDENT2:
      CheckRadioButton (hwnDlg, IDC_INDENT1, IDC_INDENTDEFAULT, IDC_INDENT2);
      ThotCallback (NumMenuRecessSense, INTEGER_DATA, (CHAR_T*) 1);
      SetDlgItemInt (hwnDlg, IDC_INDENTPTEDIT, WIN_IndentValue, FALSE);
      break;
      
    case IDC_INDENTDEFAULT:
      ThotCallback (NumMenuRecessSense, INTEGER_DATA, (CHAR_T*) 2);
      SetDlgItemInt (hwnDlg, IDC_INDENTPTEDIT, 0, FALSE);
      break;
      
      /* Line spacing menu */
    case IDC_SSMALL:
      ThotCallback (NumMenuLineSpacing, INTEGER_DATA, (CHAR_T*) 0);
      SetDlgItemInt (hwnDlg, IDC_LINESPACINGEDIT, WIN_OldLineSp, FALSE);
      break;
      
    case IDC_BSSMALL:
      CheckRadioButton (hwnDlg, IDC_SSMALL, IDC_SPACINGDEFAULT, IDC_SSMALL);
      ThotCallback (NumMenuLineSpacing, INTEGER_DATA, (CHAR_T*) 0);
      SetDlgItemInt (hwnDlg, IDC_LINESPACINGEDIT, WIN_OldLineSp, FALSE);
      break;
      
    case IDC_SMEDIUM:
      ThotCallback (NumMenuLineSpacing, INTEGER_DATA, (CHAR_T*) 1);
      SetDlgItemInt (hwnDlg, IDC_LINESPACINGEDIT, WIN_OldLineSp, FALSE);
      break;
      
    case IDC_BSMEDIUM:
      CheckRadioButton (hwnDlg, IDC_SSMALL, IDC_SPACINGDEFAULT, IDC_SMEDIUM);
      ThotCallback (NumMenuLineSpacing, INTEGER_DATA, (CHAR_T*) 1);
      SetDlgItemInt (hwnDlg, IDC_LINESPACINGEDIT, WIN_OldLineSp, FALSE);
      break;
      
    case IDC_SLARGE:
      ThotCallback (NumMenuLineSpacing, INTEGER_DATA, (CHAR_T*) 2);
      SetDlgItemInt (hwnDlg, IDC_LINESPACINGEDIT, WIN_OldLineSp, FALSE);
      break;
      
    case IDC_BSLARGE:
      CheckRadioButton (hwnDlg, IDC_SSMALL, IDC_SPACINGDEFAULT, IDC_SLARGE);
      ThotCallback (NumMenuLineSpacing, INTEGER_DATA, (CHAR_T*) 2);
      SetDlgItemInt (hwnDlg, IDC_LINESPACINGEDIT, WIN_OldLineSp, FALSE);
      break;
      
    case IDC_SPACINGDEFAULT:
      ThotCallback (NumMenuLineSpacing, INTEGER_DATA, (CHAR_T*) 3);
      SetDlgItemInt (hwnDlg, IDC_LINESPACINGEDIT, WIN_NormalLineSpacing, FALSE);
      break;
    }
    break;
    
  default: return FALSE;
  }
  return TRUE;
}

/*-----------------------------------------------------------------------
 GreekKeyboardDlgProc
 ------------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK GreekKeyboardDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK GreekKeyboardDlgProc (hwnDlg, msg, wParam, lParam)
ThotWindow   hwndParent;
UINT   msg;
WPARAM wParam;
LPARAM lParam;
#endif /* __STDC__ */
{
  int    car;

  switch (msg)
    {
    case WM_INITDIALOG: 
      GreekPal = hwnDlg;
      SetWindowText (hwnDlg, TtaGetMessage (LIB, TMSG_GREEK_ALPHABET));
      SetWindowText (GetDlgItem (hwnDlg, IDCANCEL), TtaGetMessage (LIB, TMSG_CANCEL));
      break;

    case WM_CLOSE:
    case WM_DESTROY:
      GreekPal = NULL;
      EndDialog (hwnDlg, ID_DONE);
      break;

    case WM_COMMAND:
      switch (LOWORD (wParam))
	{
	case IDCANCEL:
	  GreekPal = NULL;
	  EndDialog (hwnDlg, IDCANCEL);
	  return FALSE;
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
      SetFocus (FrRef[currentFrame]);
      KbdCallbackHandler (hwnDlg, car, "\n");
      break;
    
    default: return FALSE;
    }
  return TRUE;
}

/*-----------------------------------------------------------------------
 AuthentificationDlgProc
 ------------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK AuthentificationDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK AuthentificationDlgProc (hwnDlg, msg, wParam, lParam)
ThotWindow   hwndParent;
UINT   msg;
WPARAM wParam;
LPARAM lParam;
#endif /* __STDC__ */
{
  static ThotWindow hwnNameEdit;
  static ThotWindow hwnPasswdEdit;
  /* the following var is used to set the focus on the name edit box */
  static ThotBool setFirstFocus;

    switch (msg) {
    case WM_INITDIALOG:
      hwnNameEdit = GetDlgItem (hwnDlg, IDC_NAMEEDIT);
      hwnPasswdEdit = GetDlgItem (hwnDlg, IDC_PASSWDEDIT);
	  SetDlgItemText (hwnDlg, IDC_AUTHREALM, string_par1);
	  SetDlgItemText (hwnDlg, IDC_AUTHSERVER, string_par2);
	  SetDlgItemText (hwnDlg, IDC_PASSWDEDIT, TEXT(""));
      SetDlgItemText (hwnDlg, IDC_NAMEEDIT, TEXT(""));

	  setFirstFocus = FALSE;
      break;

    case WM_COMMAND:
      if (HIWORD (wParam) == EN_UPDATE) {
	if (LOWORD (wParam) == IDC_NAMEEDIT) {
		SetFocus (hwnNameEdit);
	} else if (LOWORD (wParam) == IDC_PASSWDEDIT)
	  SetFocus (hwnPasswdEdit);
      }  
      switch (LOWORD (wParam)) {
      case ID_CONFIRM:
	GetDlgItemText (hwnDlg, IDC_NAMEEDIT, Answer_name, sizeof (Answer_name) + 1);
	GetDlgItemText (hwnDlg, IDC_PASSWDEDIT, Answer_password, sizeof (Answer_password) + 1);
	/* don't end the dialogue unless both answer fields have something */
	if (Answer_name[0] == WC_EOS)
	  SetFocus (hwnNameEdit);
	else if (Answer_password[0] == WC_EOS)
	  SetFocus (hwnPasswdEdit);
	else
	  {
	    UserAnswer = 1;
	    EndDialog (hwnDlg, ID_CONFIRM);
	  }
	break;
	
      case IDCANCEL:
    /* we clear the answer fields */
	Answer_name[0] = Answer_password[0] = WC_EOS;
	UserAnswer = 0;
	EndDialog (hwnDlg, IDCANCEL);
	break;
      }
      break;
      
    default:
		if (!setFirstFocus)
		{
		  setFirstFocus = TRUE;
		  SetFocus (hwnNameEdit);
		}
		return FALSE;
    }
    return TRUE;
}

/*-----------------------------------------------------------------------
 BackgroundImageDlgProc
 ------------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK BackgroundImageDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK BackgroundImageDlgProc (hwnDlg, msg, wParam, lParam)
ThotWindow   hwndParent;
UINT   msg;
WPARAM wParam;
LPARAM lParam;
#endif /* __STDC__ */
{
    switch (msg)
      {
      case WM_INITDIALOG:
	SetDlgItemText (hwnDlg, IDC_BGLOCATION, currentPathName);
	CheckRadioButton (hwnDlg, IDC_REPEAT, IDC_NOREPEAT, IDC_REPEAT);
	SetWindowText (hwnDlg, TtaGetMessage (AMAYA, AM_BACKGROUND_IMAGE));
	SetWindowText (GetDlgItem (hwnDlg, IDC_OPENLOCATION), TtaGetMessage (AMAYA, AM_IMAGES_LOCATION));
	SetWindowText (GetDlgItem (hwnDlg, IDC_REPEATMODE), TtaGetMessage (AMAYA, AM_REPEAT_MODE));
	SetWindowText (GetDlgItem (hwnDlg, IDC_REPEAT), TtaGetMessage (AMAYA, AM_REPEAT));
	SetWindowText (GetDlgItem (hwnDlg, IDC_REPEATX), TtaGetMessage (AMAYA, AM_REPEAT_X));
	SetWindowText (GetDlgItem (hwnDlg, IDC_REPEATY), TtaGetMessage (AMAYA, AM_REPEAT_Y));
	SetWindowText (GetDlgItem (hwnDlg, IDC_NOREPEAT), TtaGetMessage (AMAYA, AM_NO_REPEAT));
	SetWindowText (GetDlgItem (hwnDlg, ID_CONFIRM), TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
	SetWindowText (GetDlgItem (hwnDlg, ID_CLEAR), TtaGetMessage (AMAYA, AM_CLEAR));
	SetWindowText (GetDlgItem (hwnDlg, IDC_BROWSE), TEXT("Browse"));
	SetWindowText (GetDlgItem (hwnDlg, IDCANCEL), TtaGetMessage (LIB, TMSG_CANCEL));
	repeatMode = REPEAT;
	break;

      case WM_COMMAND:
	if (HIWORD (wParam) == EN_UPDATE)
	  {
	    GetDlgItemText (hwnDlg, IDC_BGLOCATION, urlToOpen, sizeof (urlToOpen) - 1);
	    ThotCallback (BaseImage + ImageURL, STRING_DATA, urlToOpen);
	  }

	switch (LOWORD (wParam))
	  {
	  case IDC_REPEAT:
	    repeatMode  = REPEAT;
	    break;
	    
	  case IDC_REPEATX:
	    repeatMode  = REPEAT_X;
	    break;
	    
	  case IDC_REPEATY:
	    repeatMode  = REPEAT_Y;
	    break;
	    
	  case IDC_NOREPEAT:
	    repeatMode  = NO_REPEAT;
	    break;
	    
	  case ID_CONFIRM:
	    ThotCallback (BaseImage + RepeatImage, INTEGER_DATA, (CHAR_T*)repeatMode);
	    ThotCallback (BaseImage + FormBackground, INTEGER_DATA, (CHAR_T*)1);
	    EndDialog (hwnDlg, ID_CONFIRM);
	    break;
	    
	  case ID_CLEAR:
	    SetDlgItemText (hwnDlg, IDC_BGLOCATION, TEXT(""));
	    ThotCallback (BaseImage + RepeatImage, INTEGER_DATA, (CHAR_T*)repeatMode);
	    ThotCallback (BaseImage + FormBackground, INTEGER_DATA, (CHAR_T*)2);
	    break;
	    
	  case IDC_BROWSE:
	    OpenFileName.lStructSize       = sizeof (OPENFILENAME);
	    OpenFileName.hwndOwner         = hwnDlg;
	    OpenFileName.hInstance         = hInstance;
	    OpenFileName.lpstrFilter       = (LPTSTR) szFilter;
	    OpenFileName.lpstrCustomFilter = (LPTSTR) NULL;
	    OpenFileName.nMaxCustFilter    = 0L;
	    OpenFileName.nFilterIndex      = 1L;
	    OpenFileName.lpstrFile         = (LPTSTR) szFileName;
	    OpenFileName.nMaxFile          = 256;
	    OpenFileName.lpstrInitialDir   = NULL;
	    OpenFileName.lpstrTitle        = TEXT ("Open a File");
	    OpenFileName.nFileOffset       = 0;
	    OpenFileName.nFileExtension    = 0;
	    OpenFileName.lpstrDefExt       = TEXT ("*.gif");
	    OpenFileName.lCustData         = 0;
	    OpenFileName.Flags             = OFN_SHOWHELP | OFN_HIDEREADONLY;
	    
	    if (GetOpenFileName (&OpenFileName))
	      ustrcpy (urlToOpen, OpenFileName.lpstrFile);
	    
	    SetDlgItemText (hwnDlg, IDC_BGLOCATION, urlToOpen);
	    EndDialog (hwnDlg, ID_CONFIRM);
	    ThotCallback (BaseImage + RepeatImage, INTEGER_DATA, (CHAR_T*)repeatMode);
	    ThotCallback (BaseImage + FormBackground, INTEGER_DATA, (CHAR_T*)1);
	    break;

	  case IDCANCEL:
	    ThotCallback (BaseImage + FormBackground, INTEGER_DATA, (CHAR_T*)0);
	    EndDialog (hwnDlg, IDCANCEL);
	    break;
	  }
	break;
	
      default: return FALSE;
      }
    return TRUE;
}

/*-----------------------------------------------------------------------
 MakeIDDlgProc
 ------------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK MakeIDDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK MakeIDDlgProc (hwnDlg, msg, wParam, lParam)
ThotWindow   hwndParent;
UINT   msg;
WPARAM wParam;
LPARAM lParam;
#endif /* __STDC__ */
{
  switch (msg)
    {
    case WM_INITDIALOG:
      MakeIDHwnd = hwnDlg;
      /* init the dialog's text */
      SetWindowText (hwnDlg, TEXT("ID Handler menu"));
      SetWindowText (GetDlgItem (hwnDlg, ID_DONE), 
		     TtaGetMessage (LIB, TMSG_DONE));
      /* set up/clear the other options */
      /* elem name */
      SetDlgItemText (hwnDlg, IDC_IDELEMNAME, TEXT(""));
      /* status bar */
      SetDlgItemText (hwnDlg, IDC_IDSTATUS, TEXT(""));
      /* radio buttons */
      if (IdApplyToSelection)
	CheckRadioButton (hwnDlg, IDC_IDAPPLYTOSEL, 
			  IDC_IDAPPLYTOSEL, IDC_IDAPPLYTODOC);
      else
	CheckRadioButton (hwnDlg, IDC_IDAPPLYTODOC, 
			  IDC_IDAPPLYTOSEL, IDC_IDAPPLYTODOC);
      break;

    case WM_CLOSE:
    case WM_DESTROY:
      MakeIDHwnd = NULL;
      EndDialog (hwnDlg, ID_DONE);
      break;
      
    case WM_COMMAND:
      if (HIWORD (wParam) == EN_UPDATE)
	{
	  switch (LOWORD (wParam))
	    {
	    case IDC_IDELEMNAME:
	      GetDlgItemText (hwnDlg, IDC_IDELEMNAME, szBuffer,
			      MAX_LENGTH - 1);
	      szBuffer[MAX_LENGTH - 1] = EOS;
	      ThotCallback (BaseDialog + mElemName, STRING_DATA, szBuffer);
	      break;
	    }
	}
      
      switch (LOWORD (wParam))
	{
	  /* radio buttons */
	case IDC_IDAPPLYTODOC:
	  ThotCallback (BaseDialog + mIdUseSelection, INTEGER_DATA, 
			(CHAR_T *) 0);
	  break;

	case IDC_IDAPPLYTOSEL:
	  ThotCallback (BaseDialog + mIdUseSelection, INTEGER_DATA, 
			(CHAR_T *) 1);
	  break;
	  
	  /* action buttons */
	case ID_CREATEID:
	  ThotCallback (BaseDialog + MakeIdMenu, INTEGER_DATA, (CHAR_T *) 1);
	  /* update the status bar with the result of the operation */
	   SetDlgItemText (hwnDlg, IDC_IDSTATUS, IdStatus);
	  break;
	  
	case ID_REMOVEID:
	  ThotCallback (BaseDialog + MakeIdMenu, INTEGER_DATA, (CHAR_T *) 2);
	  /* update the status bar with the result of the operation */
	   SetDlgItemText (hwnDlg, IDC_IDSTATUS, IdStatus);
	  break;
	  
	case ID_DONE:
	  MakeIDHwnd = NULL;
	  EndDialog (hwnDlg, ID_DONE);
	  break;
	}
      break;
      
    default: return FALSE;
    }
  return TRUE;
}

/*-----------------------------------------------------------------------
 CreateAltDlgWindow
 ------------------------------------------------------------------------*/
void CreateAltDlgWindow ()
{
  DialogBox (hInstance, MAKEINTRESOURCE (GETALTERNATEDIALOG), NULL, (DLGPROC) AltDlgProc);
}

/*-----------------------------------------------------------------------
 CreateCSSDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void       CreateCSSDlgWindow (ThotWindow parent, int nb_item, STRING buffer, STRING title, CHAR_T* msg_text)
#else  /* __STDC__ */
void       CreateCSSDlgWindow (parent, nb_item, buffer, title, msg_text)
ThotWindow parent;
int        nb_item;
STRING     buffer;
STRING     title;
STRING     msg_text;
#endif /* __STDC__ */
{
  nbItem     = (UINT)nb_item;
  cssList    = buffer;
  ustrcpy (wndTitle, title);
  if (nbItem == 0)
    /* no entry */
    MessageBox (parent, msg_text, wndTitle, MB_OK | MB_ICONWARNING);
  else 
    DialogBox (hInstance, MAKEINTRESOURCE (CSSDIALOG), parent, (DLGPROC) CSSDlgProc);
}


/*-----------------------------------------------------------------------
 CreateTitleDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void       CreateTitleDlgWindow (ThotWindow parent, STRING title)
#else  /* !__STDC__ */
void       CreateTitleDlgWindow (parent, title)
ThotWindow parent;
STRING     title;
#endif /* __STDC__ */
{  
  ustrcpy (urlToOpen, title);
  ReleaseFocus = FALSE;
  isHref = FALSE;
  text[0] = 0;
  DialogBox (hInstance, MAKEINTRESOURCE (LINKDIALOG), parent, (DLGPROC) TextDlgProc);
}

/*-----------------------------------------------------------------------
 CreateHRefDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void   CreateHRefDlgWindow (ThotWindow parent, STRING HRefValue, int doc_select, int dir_select, int doc_type)
#else  /* !__STDC__ */
void   CreateHReflgWindow (parent, HRefValue, doc_select, dir_select, doc_type)
ThotWindow   parent;
STRING HRefValue;
int    doc_select;
int    dir_select;
int    doc_type;
#endif /* __STDC__ */
{  	
  docSelect = doc_select;
  dirSelect = dir_select;
  ustrcpy (tmpDocName, HRefValue);
    
  if (doc_type == TEXT_FILE)
    szFilter = APPFILENAMEFILTER;
  else if (doc_type == IMG_FILE)
    szFilter = APPIMAGENAMEFILTER;
  else 
    szFilter = APPALLFILESFILTER;

  DialogBox (hInstance, MAKEINTRESOURCE (OPENDOCDIALOG), parent, (DLGPROC) HRefDlgProc);
}

/*-----------------------------------------------------------------------
 CreateHelpDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void       CreateHelpDlgWindow (ThotWindow parent, STRING localname, STRING msg1, STRING msg2)
#else  /* !__STDC__ */
void       CreateHelpDlgWindow (parent, localname, msg1, msg2)
ThotWindow parent;
STRING     localname;
STRING     msg1;
STRING     msg2;
#endif /* __STDC__ */
{  
  ustrcpy (currentPathName, localname);
  ustrcpy (message, msg1);
  ustrcpy (message2, msg2);
  DialogBox (hInstance, MAKEINTRESOURCE (HELPDIALOG), parent, (DLGPROC) HelpDlgProc);
}

/*-----------------------------------------------------------------------
 CreateMathDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void        CreateMathDlgWindow (ThotWindow parent)
#else  /* !__STDC__ */
void        CreateMathDlgWindow (parent)
ThotWindow  parent;
#endif /* __STDC__ */
{  
  if (MathPal)
    SetFocus (MathPal);
  else
    DialogBox (hInstance, MAKEINTRESOURCE (MATHDIALOG), NULL, (DLGPROC) MathDlgProc);
}

/*-----------------------------------------------------------------------
 CreatePrintDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void       CreatePrintDlgWindow (ThotWindow parent, STRING ps_dir)
#else  /* !__STDC__ */
void       CreatePrintDlgWindow (parent, ps_dir)
ThotWindow parent;
STRING     ps_dir;
#endif /* __STDC__ */
{  
  gbAbort            = FALSE;
  ghwndMain          = parent;
  ustrcpy (currentFileToPrint, ps_dir);
  DialogBox (hInstance, MAKEINTRESOURCE (PRINTDIALOG), NULL, (DLGPROC) PrintDlgProc);
  if (!gbAbort)
    {
      EnableWindow  (parent, TRUE);
      DestroyWindow (ghwndAbort);
    }
  TtPrinterDC = NULL;
}

/*-----------------------------------------------------------------------
 CreateTableDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void   CreateTableDlgWindow (int num_cols, int num_rows, int t_border)
#else  /* !__STDC__ */
void   CreateTableDlgWindow (num_cols, num_rows, t_border)
int    num_cols;
int    num_rows;
int    t_border;
#endif /* __STDC__ */
{
  numCols   = num_cols;
  numRows   = num_rows;
  tBorder   = t_border;
  WithBorder = TRUE;
  DialogBox (hInstance, MAKEINTRESOURCE (TABLEDIALOG), NULL, (DLGPROC) TableDlgProc);
}

/*-----------------------------------------------------------------------
 CreateMatrixDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void   CreateMatrixDlgWindow (int num_cols, int num_rows)
#else  /* !__STDC__ */
void   CreateMatrixDlgWindow (num_cols, num_rows)
int    num_cols;
int    num_rows;
#endif /* __STDC__ */
{
  numCols   = num_cols;
  numRows   = num_rows;
  WithBorder = FALSE;
  DialogBox (hInstance, MAKEINTRESOURCE (MATRIXDIALOG), NULL, (DLGPROC) TableDlgProc);
}

/*-----------------------------------------------------------------------
 CreateSearchDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void        CreateSearchDlgWindow (ThotWindow parent, BOOL ok, CHAR_T* title)
#else  /* !__STDC__ */
void        CreateSearchDlgWindow (parent, ok, title)
ThotWindow  parent;
BOOL        ok;
CHAR_T*     title;
#endif /* __STDC__ */
{  
  selectionFound = ok;
  ustrcpy (wndTitle, title);
  DialogBox (hInstance, MAKEINTRESOURCE (SEARCHDIALOG), NULL, (DLGPROC) SearchDlgProc);
}

/*-----------------------------------------------------------------------
 CreateSaveAsDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void   CreateSaveAsDlgWindow (ThotWindow parent, STRING path_name)
#else  /* !__STDC__ */
void   CreateSaveAsDlgWindow (parent, path_name)
ThotWindow   parent;
STRING path_name;
#endif /* __STDC__ */
{  
  ustrcpy (currentPathName, path_name);
  DialogBox (hInstance, MAKEINTRESOURCE (SAVEASDIALOG), parent, (DLGPROC) SaveAsDlgProc);
}

/*-----------------------------------------------------------------------
 CreateOPenDocDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void   CreateOpenDocDlgWindow (ThotWindow parent, STRING title, STRING docName, int doc_select, int dir_select, int doc_type)
#else  /* !__STDC__ */
void   CreateOpenDocDlgWindow (parent, title, docName, doc_select, dir_select, doc_type)
ThotWindow   parent;
STRING title;
STRING docName;
int    doc_select;
int    dir_select;
int    doc_type;
#endif /* __STDC__ */
{  
  docSelect = doc_select;
  dirSelect = dir_select;
  ustrcpy (wndTitle, title);
  ustrcpy (tmpDocName, docName);
  ustrcpy ( urlToOpen, docName);
    
  if (doc_type == TEXT_FILE)
    szFilter = APPFILENAMEFILTER;
  else if (doc_type == IMG_FILE)
    szFilter = APPIMAGENAMEFILTER;
  else 
    szFilter = APPALLFILESFILTER;

  DialogBox (hInstance, MAKEINTRESOURCE (OPENDOCDIALOG), parent, (DLGPROC) OpenDocDlgProc);
}

/*-----------------------------------------------------------------------
 CreateOPenImgDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void CreateOpenImgDlgWindow (ThotWindow parent, STRING imgName, int doc_select, int dir_select, int doc_type)
#else  /* !__STDC__ */
void CreateOpenImgDlgWindow (parent, imgName, doc_select, dir_select, doc_type)
ThotWindow  parent;
STRING imgName;
int   doc_select;
int   dir_select;
int   doc_type;
#endif /* __STDC__ */
{  
  docSelect = doc_select;
  dirSelect = dir_select;
  ustrcpy ( urlToOpen, imgName);
    
  if (doc_type == TEXT_FILE)
    szFilter = APPFILENAMEFILTER;
  else if (doc_type == IMG_FILE)
    szFilter = APPIMAGENAMEFILTER;
  else 
    szFilter = APPALLFILESFILTER;
  
  DialogBox (hInstance, MAKEINTRESOURCE (OPENIMAGEDIALOG), parent, (DLGPROC) OpenImgDlgProc);
}

/*-----------------------------------------------------------------------
 CreateGraphicsDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void  CreateGraphicsDlgWindow (ThotWindow frame)
#else  /* !__STDC__ */
void  CreateGraphicsDlgWindow (frame)
ThotWindow frame;
#endif /* __STDC__ */
{
  if (GraphPal)
    SetFocus (GraphPal);
  else
   DialogBox (hInstance, MAKEINTRESOURCE (GRAPHICSDIALOG), NULL, (DLGPROC) GraphicsDlgProc);
}

/*-----------------------------------------------------------------------
 CreateSaveListDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void       CreateSaveListDlgWindow (ThotWindow parent, int nb_item, STRING save_list)
#else  /* !__STDC__ */
void       CreateSaveListDlgWindow (parent, nb_item, save_list)
ThotWindow parent;
int        nb_item;
STRING     save_list;
#endif /* __STDC__ */
{  
  nbItem      = (UINT)nb_item;
  saveList    = save_list;
  DialogBox (hInstance, MAKEINTRESOURCE (SAVELISTDIALOG), parent, (DLGPROC) SaveListDlgProc);
}

/*-----------------------------------------------------------------------
 CreateCloseDocDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void       CreateCloseDocDlgWindow (ThotWindow parent, STRING msg, ThotBool* save_befor, ThotBool* close_dont_save)
#else  /* !__STDC__ */
void       CreateCloseDocDlgWindow (parent, msg, save_befor, close_dont_save)
ThotWindow parent;
STRING    msg;
ThotBool *save_befor;
ThotBool *close_dont_save;
#endif /* __STDC__ */
{  
  ustrcpy (message, msg);
  DialogBox (hInstance, MAKEINTRESOURCE (CLOSEDOCDIALOG), parent, (DLGPROC) CloseDocDlgProc);
  *save_befor = saveBeforeClose;
  *close_dont_save = closeDontSave;
}

/*-----------------------------------------------------------------------
 CreateLanguageDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void        CreateLanguageDlgWindow (ThotWindow parent, STRING title, STRING msg1, int nb_item, STRING lang_list, STRING msg2, int lang_value, STRING curLang)
#else  /* !__STDC__ */
void        CreateLanguageDlgWindow (parent, title, msg1, nb_item, lang_list, msg2, lang_value, curLang)
ThotWindow  parent;
STRING      title;
STRING      msg1;
int         nb_item;
STRING      lang_list;
STRING      msg2;
int         nmenuLanguage;
int         lang_value;
STRING      curLang;
#endif /* __STDC__ */
{  
  ustrcpy (wndTitle, title);
  ustrcpy (message, msg1);
  ustrcpy (message2, msg2);
  ustrcpy (winCurLang, curLang);
  langList                = lang_list;
  nbItem                  = (UINT)nb_item;
  LangValue               = lang_value;
  DialogBox (hInstance, MAKEINTRESOURCE (LANGUAGEDIALOG), parent, (DLGPROC) LanguageDlgProc);
}

/*-----------------------------------------------------------------------
 CreateCharacterDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void CreateCharacterDlgWindow (ThotWindow parent, int font_num, int font_style, int font_weight, int font_underline, int font_size)
#else  /* !__STDC__ */
void CreateCharacterDlgWindow (parentint font_num, font_style, font_weight, font_underline, font_size)
ThotWindow  parent;
int   font_num;
int   font_style;
int   font_weight;
int   font_underline;
int   font_size;
#endif /* __STDC__ */
{  
    fontNum       = font_num;
    fontStyle     = font_style;
    fontWeight    = font_weight;
    fontUnderline = font_underline;
    fontSize      = font_size;


	DialogBox (hInstance, MAKEINTRESOURCE (CHARACTERSDIALOG), NULL, (DLGPROC) CharacterDlgProc);
}

/*-----------------------------------------------------------------------
 CreateAttributeDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void CreateAttributeDlgWindow (STRING title, int curr_val, int nb_items) 
#else  /* __STDC__ */
void CreateAttributeDlgWindow (title, curr_val, nb_items) 
STRING title;
int   attr_val;
int   nb_items;
STRING buffer;
#endif /* __STDC__ */
{
  ustrcpy (attDlgTitle, title);
  currAttrVal = curr_val;
  attDlgNbItems = nb_items;
  switch (attDlgNbItems)
    {
    case 2: DialogBox (hInstance, MAKEINTRESOURCE (ATTR2ITEMSDIALOG), NULL, (DLGPROC) AttrItemsDlgProc);
      break;
      
    case 3: DialogBox (hInstance, MAKEINTRESOURCE (ATTR3ITEMSDIALOG), NULL, (DLGPROC) AttrItemsDlgProc);
      break;
      
    case 4: DialogBox (hInstance, MAKEINTRESOURCE (ATTR4ITEMSDIALOG), NULL, (DLGPROC) AttrItemsDlgProc);
      break;
      
    case 5: DialogBox (hInstance, MAKEINTRESOURCE (ATTR5ITEMSDIALOG), NULL, (DLGPROC) AttrItemsDlgProc);
      break;
      
    case 6: DialogBox (hInstance, MAKEINTRESOURCE (ATTR6ITEMSDIALOG), NULL, (DLGPROC) AttrItemsDlgProc);
      break;
      
    case 7: DialogBox (hInstance, MAKEINTRESOURCE (ATTR7ITEMSDIALOG), NULL, (DLGPROC) AttrItemsDlgProc);
      break;
       
    case 8: DialogBox (hInstance, MAKEINTRESOURCE (ATTR8ITEMSDIALOG), NULL, (DLGPROC) AttrItemsDlgProc);
      break;
      
    case 9: DialogBox (hInstance, MAKEINTRESOURCE (ATTR9ITEMSDIALOG), NULL, (DLGPROC) AttrItemsDlgProc);
      break;
 
    default:
      break;
    } 
}

/*-----------------------------------------------------------------------
 CreateMCHARDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void       CreateMCHARDlgWindow (ThotWindow parent, STRING math_entity_name) 
#else  /* __STDC__ */
void       CreateMCHARDlgWindow (parent, math_entity_name) 
ThotWindow parent; 
STRING     math_entity_name;
#endif /* __STDC__ */
{
    ustrcpy (mathEntityName, math_entity_name);
    DialogBox (hInstance, MAKEINTRESOURCE (MATH_ENTITY_DLG), NULL, (DLGPROC) MathEntityDlgProc);
    ustrcpy (math_entity_name, mathEntityName);
}

/*-----------------------------------------------------------------------
 CreateRuleDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void CreateRuleDlgWindow (ThotWindow parent, int nb_class, STRING class_list)
#else  /* !__STDC__ */
void CreateRuleDlgWindow (parent, nb_class, class_list)
ThotWindow  parent;
int   nb_class;
STRING class_list;
#endif /* __STDC__ */
{  
  nbClass     = (UINT)nb_class;
  classList   = class_list;
  WithEdit = TRUE;
  DialogBox (hInstance, MAKEINTRESOURCE (CREATERULEDIALOG), parent, (DLGPROC) ApplyClassDlgProc);
}

/*-----------------------------------------------------------------------
 CreateApplyClassDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void CreateApplyClassDlgWindow (ThotWindow parent, int nb_class, STRING class_list)
#else  /* !__STDC__ */
void CreateApplyClassDlgWindow (parent, nb_class, class_list)
ThotWindow   parent;
int    nb_class;
STRING class_list;
#endif /* __STDC__ */
{  
  nbClass     = (UINT)nb_class;
  classList   = class_list;
  WithEdit = FALSE;
  DialogBox (hInstance, MAKEINTRESOURCE (APPLYCLASSDIALOG), NULL/*parent*/, (DLGPROC) ApplyClassDlgProc);
}

/*-----------------------------------------------------------------------
 CreateSpellCheckDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void CreateSpellCheckDlgWindow (ThotWindow parent, STRING label, STRING rejectedChars,
								int spellingBase, int chkrSelectProp, int chkrMenuOR, 
							    int chkrFormCorrect, int chkrMenuIgnore, int chkrCaptureNC, int chkrSpecial)
#else  /* !__STDC__ */
void CreateSpellCheckDlgWindow (parent, label, rejectedChars, spellingBase, 
								chkrSelectProp, chkrMenuOR, chkrFormCorrect, chkrMenuIgnore, chkrCaptureNC, chkrSpecial)
ThotWindow  parent;
STRING label;
STRING rejectedChars;
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
  ChkrMenuOR      = chkrMenuOR;
  ChkrFormCorrect = chkrFormCorrect;
  ChkrMenuIgnore  = chkrMenuIgnore;
  ChkrCaptureNC   = chkrCaptureNC;
  
  ustrcpy (currentLabel, label);
  ustrcpy (currentRejectedchars, rejectedChars); 
  DialogBox (hInstance, MAKEINTRESOURCE (SPELLCHECKDIALOG), NULL, (DLGPROC) SpellCheckDlgProc);
}

/*-----------------------------------------------------------------------
 CreateInitConfirmDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void CreateInitConfirmDlgWindow (ThotWindow parent, int ref, STRING title, STRING msg)
#else  /* !__STDC__ */
void CreateInitConfirmDlgWindow (parent, ref, title, msg)
ThotWindow  parent;
int   ref;
STRING title;
STRING msg;
#endif /* __STDC__ */
{  
  ustrcpy (message, msg);
  ustrcpy (wndTitle, title);
  currentRef = ref;

  DialogBox (hInstance, MAKEINTRESOURCE (INITCONFIRMDIALOG), parent, (DLGPROC) InitConfirmDlgProc);
}

/*-----------------------------------------------------------------------
 CreateInitConfirm3LDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void        CreateInitConfirm3LDlgWindow (ThotWindow parent, int ref, STRING title, STRING msg, STRING msg2, STRING msg3, ThotBool withCancel)
#else  /* !__STDC__ */
void        CreateInitConfirm3LDlgWindow (parent, ref, title, msg, msg2, msg3)
ThotWindow  parent;
int         ref;
STRING      title;
STRING      msg;
STRING      msg2;
STRING      msg3;
ThotBool    withCancel;
#endif /* __STDC__ */
{
  ustrcpy (message, msg);
  if (msg2 && *msg2 != WC_EOS)
    ustrcpy (message2, msg2);
  else
    message2[0] = WC_EOS;
  if (msg3 && *msg3 != WC_EOS)
    ustrcpy (message3, msg3);
  else
    message3[0] = WC_EOS;
  
  ustrcpy (wndTitle, title);
  currentRef = ref;
  
  /* register if the cancel button has to be generated */
  WithCancel = withCancel;
  if (withCancel)
    DialogBox (hInstance, MAKEINTRESOURCE (INITCONFIRM3LDIALOG), parent, (DLGPROC) InitConfirm3LDlgProc);
  else
	DialogBox (hInstance, MAKEINTRESOURCE (INITCONFIRM3LDIALOG1), parent, (DLGPROC) InitConfirm3LDlgProc);
}

/*-----------------------------------------------------------------------
 CreateChangeFormatDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void CreateChangeFormatDlgWindow (int num_zone_recess, int num_zone_line_spacing, int align_num, int indent_value, int indent_num, int justification_num, int old_lineSp, int line_spacingNum)
#else  /* !__STDC__ */
void CreateChangeFormatDlgWindow (num_zone_recess, num_zone_line_spacing, align_num, indent_value, indent_num, justification_num, old_lineSp, line_spacingNum)
int num_zone_recess;
int num_zone_line_spacing;
int align_num;
int indent_value;
int indent_num;
int justification_num;
int old_lineSp;
int line_spacingNum;
ThotWindow  parent;
#endif /* __STDC__ */
{  
  Num_zoneRecess      = num_zone_recess;
  Num_zoneLineSpacing = num_zone_line_spacing;
  Align_num           = align_num; 
  Indent_value        = indent_value;
  Indent_num          = indent_num;
  Justification_num   = justification_num;
  Old_lineSp          = old_lineSp;
  Line_spacingNum     = line_spacingNum;
  
  DialogBox (hInstance, MAKEINTRESOURCE (FORMATDIALOG), NULL, (DLGPROC) ChangeFormatDlgProc);
}

/*-----------------------------------------------------------------------
 CreateGreekKeyboardDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void CreateGreekKeyboardDlgWindow (ThotWindow parent)
#else  /* !__STDC__ */
void CreateGreekKeyboardDlgWindow (parent)
ThotWindow  parent;
#endif /* __STDC__ */
{  
  if (GreekPal)
    SetFocus (GreekPal);
  else
    DialogBox (hInstance, MAKEINTRESOURCE (GALPHABETDIALOG), NULL, (DLGPROC) GreekKeyboardDlgProc);
}

/*-----------------------------------------------------------------------
 CreateAuthentificationDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void         CreateAuthenticationDlgWindow (ThotWindow parent, const STRING realm, STRING server)
#else  /* !__STDC__ */
void         CreateAuthenticationDlgWindow (parent, realm, server)
ThotWindow   parent;
const STRING realm;
STRING       server;
#endif /* __STDC__ */
{  
  string_par1 = realm;
  string_par2 = server;

  DialogBox (hInstance, MAKEINTRESOURCE (AUTHENTIFICATIONDIALOG), parent, (DLGPROC) AuthentificationDlgProc);
}

/*-----------------------------------------------------------------------
 CreateBackgroundImageDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void CreateBackgroundImageDlgWindow (ThotWindow parent, STRING image_location)
#else /* !__STDC__ */
void CreateBackgroundImageDlgWindow (parent, image_location)
ThotWindow  parent;
STRING image_location;
#endif /* __STDC__ */
{
  szFilter         = APPIMAGENAMEFILTER;
  ustrcpy (currentPathName, image_location);
  DialogBox (hInstance, MAKEINTRESOURCE (BGIMAGEDIALOG), parent, (DLGPROC) BackgroundImageDlgProc);
}

/*-----------------------------------------------------------------------
 CreateMakeIDDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void CreateMakeIDDlgWindow (ThotWindow parent)
#else /* !__STDC__ */
void CreateMakeIDDlgWindow (parent)
ThotWindow  parent;
#endif /* __STDC__ */
{
  /* we only use one such dialog at the time */
  if (MakeIDHwnd)
    EndDialog (MakeIDHwnd, ID_DONE);
  DialogBox (hInstance, MAKEINTRESOURCE (MAKEIDMENU), NULL, (DLGPROC) MakeIDDlgProc);
}
#endif /* _WINDOWS */
