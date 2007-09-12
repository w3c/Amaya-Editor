/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Windows Dialogue API routines for Amaya
 *
 * Authors:  I. Vatton (W3C/INRIA),
 *           R. Guetari (W3C/INRIA),
 *           J. Kahan (W3C/INRIA)
 *
 */
#ifdef _WINGUI
#include <windows.h>
#include "resource.h"
#include "constmedia.h"
#include "corrmsg.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "constmenu.h"
#include "wininclude.h"

extern ThotWindow  FrRef[MAX_FRAME + 2];
extern int         ActiveFrame;

#ifndef MAX_PROPOSAL_CHKR
#define MAX_PROPOSAL_CHKR 10
#endif /* MAX_PROPOSAL_CHKR */

#ifndef MAX_WORD_LEN
#define MAX_WORD_LEN 30
#endif /* MAX_WORD_LEN */


#define MAX_BUFF 4096
/* JK: I think that some of these widgets, which are general, should
   go into the thotlib, such as IDC_LANGEDIT. We have a dupblication of
   IDC refs between thotlib and Amaya */
#define IDC_EDITRULE      20001
#define IDC_LANGEDIT      20002
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

#define REPEAT_XY             0
#define REPEAT_X              1
#define REPEAT_Y              2
#define NO_REPEAT             3

int                WIN_IndentValue;
int                WIN_OldLineSp;
int                WIN_NormalLineSpacing;

extern HINSTANCE    hInstance;
extern HDC          TtPrinterDC;
extern char         DocToOpen[MAX_LENGTH];
extern ThotBool     TtIsPrinterTrueColor;
extern ThotBool     bUserAbort;
extern ThotBool     WithToC;
extern ThotBool     NumberLinks;
extern ThotBool     PrintURL;
extern ThotBool     IgnoreCSS;
extern HFONT        DialogFont;

ThotWindow          ghwndAbort;
ThotWindow          ghwndMain;
ThotWindow          MakeIDHwnd;
ThotBool            gbAbort;
Document            TmpDoc; /* used to pass the Document id to the
			       callback when setting up a menu */

static HDC          hDC;
static HDC          hMemDC;
static char         UrlToOpen[MAX_LENGTH];
static char         HrefUrl[MAX_LENGTH];
static char         TmpDocName[MAX_LENGTH];
static char         AltText[MAX_LENGTH];
static char         Message[300];
static char         Message2[300];
static char         Message3[300];
static char         WndTitle[100];
static char         CurrentPathName[100];
static char         WinCurLang[100];
static char         WinCurXML[100];
static char         CurFileToPrint[MAX_PATH];
static char         AttDlgTitle[100];
static char         MathEntName[MAX_TXT_LEN];
static char         SzBuffer[MAX_BUFF];
static char        *ClassList;
static char        *ItemList;
static char        *SavList;
static char        *cssList;
static char        *mimeType;
static char        *charSet;
static char        *string_par1;
static char        *string_par2;
static char         text[1024];
static int          repeatMode;
static int          fontNum;
static int          fontStyle;
static int          fontWeight;
static int          fontUnderline;
static int          fontSize;
static int          docSelect;
static int          dirSelect;
static int          currAttrVal;
static int          Num_zoneRecess;
static int          Num_zoneLineSpacing;
static int          Num_lang;
static int          XMLnum;
static int          Align_num;
static int          Indent_value;
static int          Indent_num;
static int          Old_lineSp;
static int          Line_spacingNum;
static int          attDlgNbItems;
static int          numCols;
static int          numRows;
static int          tBorder;
static int          formAlt;
static int          iLocation;
static int          iMode;
static int          indexImgFilter = 1;
static int          indexLinkFilter = 1;
static int          indexFilter = 1;
static UINT         itemIndex;
static UINT         nbClass;
static UINT         NbItem;

static ThotBool	    saveBeforeClose;
static ThotBool     closeDontSave;
static ThotBool     isHref;
static ThotBool     WithEdit;
static ThotBool     WithCancel;
static ThotBool     WithBorder;
static ThotBool     HTMLFormat;
static ThotBool     ReleaseFocus;
static ThotBool     RequiredAttr;

static OPENFILENAME OpenFileName;
static char        *SzFilter;
static ThotWindow   wndCSSList;
static ThotWindow   wndXMLList;
static ThotWindow   wndLangList;
static ThotWindow   wndListRule;
static ThotWindow   wndEditRule;
static ThotWindow   WndClose = NULL;
static ThotWindow   transURLWnd;
static ThotWindow   copyImgWnd;
static ThotWindow   WndSearchEdit;
static ThotWindow   GraphPal = NULL;
static ThotWindow   MathPal = NULL;
static ThotWindow   GreekPal = NULL;
static ThotWindow   CharacterForm = NULL;
static ThotWindow   FormatForm = NULL;
static ThotWindow   PrintForm = NULL;
static ThotWindow   LangForm = NULL;
static ThotWindow   XMLForm = NULL;
static ThotWindow   AttrForm = NULL;
static ThotWindow   SaveAsForm = NULL;
static ThotWindow   MimeTypeDlg = NULL;
static ThotWindow   InitConfirmForm = NULL;
static ThotWindow   DocInfo[DocumentTableLength];

#include "init_f.h"

/*----------------------------------------------------------------------
  DisplayStyleValue: update the property in Style dialog
  ----------------------------------------------------------------------*/
void  DisplayStyleValue (char *property, char *start_value, char *end_value)
{
}

/*----------------------------------------------------------------------
   ReusePrinterDC()
   Call the Windows print dialogue or reuse the previous context.
  ----------------------------------------------------------------------*/
void ReusePrinterDC ()
{
  int        orientation, paper;

  if (TtaGetPrinterDC (TRUE, &orientation, &paper))
    {
      ThotCallback (BasePrint + PPrinterName, STRING_DATA, CurFileToPrint);
      ThotCallback (BasePrint + FormPrint, INTEGER_DATA, (char *)1);
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
LRESULT CALLBACK AltDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam,
			     LPARAM lParam)
{
  switch (msg)
    {
    case WM_INITDIALOG:
      SetWindowText (hwnDlg, TtaGetMessage (AMAYA, AM_ALT));
      SetWindowText (GetDlgItem (hwnDlg, IDC_ALTTEXT),
		     TtaGetMessage (AMAYA, AM_ALT));
      SetWindowText (GetDlgItem (hwnDlg, ID_CONFIRM),
		     TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
      SetWindowText (GetDlgItem (hwnDlg, ID_DONE),
		     TtaGetMessage (LIB, TMSG_DONE));
      SetDlgItemText (hwnDlg, IDC_GETALT, "");
      break;
      
    case WM_COMMAND:
      switch (LOWORD (wParam))
	{
	case ID_CONFIRM:
	case ID_DONE:
	  GetDlgItemText (hwnDlg, IDC_GETALT, AltText, sizeof (AltText) - 1);
	  ThotCallback (BaseImage + ImageAlt, STRING_DATA, AltText);
	  if (!AltText || AltText[0] == 0)
	    MessageBox (hwnDlg, TtaGetMessage (AMAYA, AM_ALT_MISSING),
			TtaGetMessage (AMAYA, AM_ALT), MB_OK | MB_ICONERROR);
	  else 
	    EndDialog (hwnDlg, ID_CONFIRM);
	  ThotCallback (BaseImage + FormAlt, STRING_DATA, (char *)1);
	  break;
	}
      break;
    default:
      return FALSE;
    }
  return TRUE;
}

/*-----------------------------------------------------------------------
 XMLDlgProc
 ------------------------------------------------------------------------*/
LRESULT CALLBACK XMLDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam,
							 LPARAM lParam)
{
  int             index = 0;
  UINT            i = 0;

  switch (msg)
    {
    case WM_INITDIALOG:
      /* get the default GUI font */
      /* destroy the focus of the previous open dialog */
      XMLForm = hwnDlg;
      SetWindowText (hwnDlg, TtaGetMessage (LIB, TMSG_EL_TYPE));
	  SetWindowText (GetDlgItem (hwnDlg, ID_APPLY), TtaGetMessage (LIB, TMSG_INSERT));
	  SetWindowText (GetDlgItem (hwnDlg, ID_DONE), TtaGetMessage (LIB, TMSG_DONE));
	  SetWindowText (GetDlgItem (hwnDlg, IDC_XMLELEM),
			 TtaGetMessage (LIB, TMSG_EL_TYPE));
      wndXMLList = GetDlgItem (hwnDlg, IDC_XMLLIST);
      /* set the font of the window */
      WIN_SetDialogfont (wndXMLList);
      SendMessage (wndXMLList, LB_RESETCONTENT, 0, 0);
    while (i < NbItem && ItemList[index] != EOS)
      {
	SendMessage (wndXMLList, LB_INSERTSTRING, i, (LPARAM) &ItemList[index]); 
	index += strlen (&ItemList[index]) + 1;/* Longueur de l'intitule */
	i++;
      }
    SetWindowText (GetDlgItem (hwnDlg, IDC_XMLEDIT), "");
	XMLnum = -1;
    break;

    case WM_CLOSE:
    case WM_DESTROY:
      XMLForm = NULL;
      EndDialog (hwnDlg, ID_DONE);
      break;

    case WM_COMMAND:
      if (LOWORD (wParam) == 1 || HIWORD (wParam) == LBN_SELCHANGE ||
	  HIWORD (wParam) == LBN_DBLCLK)
	{
	  if (HIWORD (wParam) == LBN_SELCHANGE)
	    {
	      XMLnum = SendMessage (wndXMLList, LB_GETCURSEL, 0, 0);
	      XMLnum = SendMessage (wndXMLList, LB_GETTEXT, XMLnum,
				      (LPARAM) WinCurXML);
	      SetWindowText (GetDlgItem (hwnDlg, IDC_XMLEDIT), WinCurXML);
	    }
	  else if (HIWORD (wParam) == LBN_DBLCLK)
	    {
	      if (LB_ERR == (XMLnum = SendMessage (wndLangList, LB_GETCURSEL, 0, 0L)))
		break;
	      XMLnum = SendMessage (wndLangList, LB_GETTEXT, XMLnum,
				      (LPARAM) WinCurLang);
	    }
	  SetDlgItemText (hwnDlg, IDC_XMLEDIT, WinCurXML);
	  ThotCallback (NumSelectElemToBeCreated, STRING_DATA, WinCurXML);
	  if (HIWORD (wParam) == LBN_DBLCLK)
	    {
		  /* confirm the selection */
	      ThotCallback (NumFormElemToBeCreated, INTEGER_DATA, (char*) 1);
	      XMLForm = NULL;
	      EndDialog (hwnDlg, ID_DONE);
	      return FALSE;
	    }
	}
      
      switch (LOWORD (wParam))
	{
	case ID_APPLY:
	  GetDlgItemText (hwnDlg, IDC_XMLEDIT, WinCurXML, sizeof (WinCurXML) - 1);
	  if (WinCurXML[0] != EOS)
	    ThotCallback (NumSelectElemToBeCreated, STRING_DATA, WinCurXML);
	  ThotCallback (NumFormElemToBeCreated, INTEGER_DATA, (char*) 1);
	  if (WinCurXML[0] != EOS)
	  {
	  /* reinitialize the list */
      wndXMLList = GetDlgItem (hwnDlg, IDC_XMLLIST);
    while (i < NbItem && ItemList[index] != EOS)
      {
		if (i == NbItem - 1)
	SendMessage (wndXMLList, LB_INSERTSTRING, i, (LPARAM) &ItemList[index]); 
	index += strlen (&ItemList[index]) + 1;
	i++;
      }
    SetWindowText (GetDlgItem (hwnDlg, IDC_XMLEDIT), "");
	XMLnum = -1;
	  }
 	  break;
	case ID_DONE:
	  ThotCallback (NumFormElemToBeCreated, INTEGER_DATA, (char*) 0);
	  XMLForm = NULL;
	  EndDialog (hwnDlg, ID_DONE);
	  break;	  
	case WM_CLOSE:
	case WM_DESTROY:
	  XMLForm = NULL;
	  EndDialog (hwnDlg, ID_DONE);
	  break;
	}
      break;
    default: return FALSE;
    }
  return TRUE;
}

/*-----------------------------------------------------------------------
 CSSDlgProc
 ------------------------------------------------------------------------*/
LRESULT CALLBACK CSSDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam,
			     LPARAM lParam)
{
  int             index = 0;
  UINT            i = 0;

  switch (msg)
    {
    case WM_INITDIALOG:
      SetWindowText (hwnDlg, WndTitle);
      SetWindowText (GetDlgItem (hwnDlg, ID_CONFIRM),
		  TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
      SetWindowText (GetDlgItem (hwnDlg, ID_DONE),
		  TtaGetMessage (LIB, TMSG_DONE));
      SetWindowText (GetDlgItem (hwnDlg, IDC_CSSFILES),
		  TtaGetMessage (AMAYA, AM_CSS_FILE));
      wndCSSList = GetDlgItem (hwnDlg, IDC_CSSLIST);
      /* set the font of the window */
      WIN_SetDialogfont (wndCSSList);
      SendMessage (wndCSSList, LB_RESETCONTENT, 0, 0);
      while (i < NbItem && cssList[index] != EOS)
	{
	  SendMessage (wndCSSList, LB_INSERTSTRING, i, (LPARAM) &cssList[index]); 
	  index += strlen (&cssList[index]) + 1;	/* entry length */
	  i++;
	}

      itemIndex = SendMessage (wndCSSList, LB_SETCURSEL, (WPARAM)0, (LPARAM)0);
      SetDlgItemText (hwnDlg, IDC_CSSEDIT, SzBuffer);
      break;

    case WM_CLOSE:
    case WM_DESTROY:
      EndDialog (hwnDlg, ID_DONE);
      break;

    case WM_COMMAND:
      if (LOWORD (wParam) == IDC_CSSLIST && HIWORD (wParam) == LBN_SELCHANGE)
	{
	itemIndex = SendMessage (wndCSSList, LB_GETCURSEL, 0, 0);
	itemIndex = SendMessage (wndCSSList, LB_GETTEXT, itemIndex, (LPARAM) SzBuffer);
	SetDlgItemText (hwnDlg, IDC_CSSEDIT, SzBuffer);
	ThotCallback (BaseCSS + CSSSelect, STRING_DATA, SzBuffer);
	}

      switch (LOWORD (wParam))
	{
	case ID_CONFIRM:
	  ThotCallback (BaseCSS + CSSForm, INTEGER_DATA, (char*) 1);
	  EndDialog (hwnDlg, ID_CONFIRM);
	  break;

	case IDCANCEL:
	case ID_DONE:
	  ThotCallback (BaseCSS + CSSForm, INTEGER_DATA, (char*) 0);
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
LRESULT CALLBACK TextDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam,
							  LPARAM lParam)
{
  switch (msg)
    {
    case WM_INITDIALOG:
      if (isHref)
	{
	  SetWindowText (hwnDlg, TtaGetMessage (AMAYA, AM_ATTRIBUTE));
	  SetWindowText (GetDlgItem (hwnDlg, IDC_URL_TEXT),
		  TtaGetMessage (AMAYA, AM_HREF_VALUE));
	}
      else
	{
	  SetWindowText (hwnDlg, TtaGetMessage (AMAYA, AM_CHANGE_TITLE));
	  SetWindowText (GetDlgItem (hwnDlg, IDC_URL_TEXT),
		  TtaGetMessage (AMAYA, AM_TITLE));
	}
      SetWindowText (GetDlgItem (hwnDlg, ID_CONFIRM), 
		  TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
      SetWindowText (GetDlgItem (hwnDlg, ID_DONE), TtaGetMessage (LIB, TMSG_CANCEL));
      SetDlgItemText (hwnDlg, IDC_URLEDIT, UrlToOpen);      
      SetFocus (GetDlgItem (hwnDlg, IDC_URLEDIT));
      break;
      
    case WM_CLOSE:
    case WM_DESTROY:
      EndDialog (hwnDlg, ID_DONE);
      break;
      
    case WM_COMMAND:
      switch (LOWORD (wParam))
	{
	case ID_CONFIRM:
	  GetDlgItemText (hwnDlg, IDC_URLEDIT, UrlToOpen, sizeof (UrlToOpen) - 1);
	  if (isHref)
	    {
	      AttrHREFvalue = TtaGetMemory (strlen (UrlToOpen) + 1);
	      strcpy (AttrHREFvalue, UrlToOpen);
	      ThotCallback (BaseDialog + AttrHREFForm, INTEGER_DATA, (char*) 1);
	    }
	  else
	    {
	      ThotCallback (BaseDialog + TitleText, STRING_DATA, UrlToOpen);
	      ThotCallback (BaseDialog + TitleForm, INTEGER_DATA, (char*) 1);
	    }
	  EndDialog (hwnDlg, ID_CONFIRM);
	  break;
      
	case ID_DONE:
	  if (isHref)
	    ThotCallback (BaseDialog + AttrHREFForm, INTEGER_DATA, (char*) 0);
	  else
	    ThotCallback (BaseDialog + TitleForm, INTEGER_DATA, (char*) 0);
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
LRESULT CALLBACK HRefDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam,
							  LPARAM lParam)
{
  switch (msg)
    {
    case WM_INITDIALOG:
      SetWindowText (hwnDlg, TtaGetMessage (AMAYA, AM_ATTRIBUTE));
      SetWindowText (GetDlgItem (hwnDlg, IDC_URLMESSAGE),
		  TtaGetMessage (AMAYA, AM_LOCATION));
      SetWindowText (GetDlgItem (hwnDlg, ID_CONFIRM),
		  TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
      SetWindowText (GetDlgItem (hwnDlg, IDC_BROWSE), TtaGetMessage (AMAYA, AM_BROWSE));
      SetWindowText (GetDlgItem (hwnDlg, IDC_CLICK), TtaGetMessage (AMAYA, AM_CLICK));
      SetWindowText (GetDlgItem (hwnDlg, IDC_CLEAR), TtaGetMessage (AMAYA, AM_CLEAR));
      SetWindowText (GetDlgItem (hwnDlg, IDCANCEL), TtaGetMessage (LIB, TMSG_CANCEL));
      
	  if (TmpDocName[0] != EOS)
		  SetDlgItemText (hwnDlg, IDC_GETURL, TmpDocName);
	  else
		  SetDlgItemText (hwnDlg, IDC_GETURL, "");
      HrefUrl[0] = 0;

	  /* set the default focus and return FALSE to validate it */
	  SetFocus (GetDlgItem (hwnDlg, IDC_GETURL));
	  return FALSE;

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
	  ThotCallback (BaseDialog + AttrHREFForm, INTEGER_DATA, (char*)1);
	  EndDialog (hwnDlg, ID_CONFIRM);
	  break;
	case IDC_BROWSE:
	  OpenFileName.lStructSize       = sizeof (OPENFILENAME);
	  OpenFileName.hwndOwner         = hwnDlg;
	  OpenFileName.hInstance         = hInstance;
	  OpenFileName.lpstrFilter       = (LPTSTR) SzFilter;
	  OpenFileName.lpstrCustomFilter = (LPTSTR) NULL;
	  OpenFileName.nMaxCustFilter    = 0L;
	  OpenFileName.nFilterIndex      = indexLinkFilter;
	  OpenFileName.lpstrFile         = (LPTSTR) TmpDocName;
	  OpenFileName.nMaxFile          = MAX_LENGTH;
	  OpenFileName.lpstrInitialDir   = (LPTSTR) DirectoryName;
	  OpenFileName.lpstrTitle        = TtaGetMessage (AMAYA, AM_BROWSE);
	  OpenFileName.nFileOffset       = 0;
	  OpenFileName.nFileExtension    = 0;
	  OpenFileName.lpstrDefExt       = "html";
	  OpenFileName.lCustData         = 0;
	  OpenFileName.Flags             = OFN_SHOWHELP | OFN_HIDEREADONLY;
	  
	  if (GetOpenFileName (&OpenFileName))
	  {
	    strcpy (HrefUrl, OpenFileName.lpstrFile);
		indexLinkFilter = OpenFileName.nFilterIndex;
	  }
	  
	  SetDlgItemText (hwnDlg, IDC_GETURL, HrefUrl);
	  if (HrefUrl[0] != EOS)
	    {
	      ThotCallback (BaseDialog + AttrHREFText, STRING_DATA, HrefUrl);
	      EndDialog (hwnDlg, ID_CONFIRM);
	      ThotCallback (BaseDialog + AttrHREFForm, INTEGER_DATA, (char*) 1);
	    }
	  break;
	  
	case IDC_CLICK:
	  EndDialog (hwnDlg, IDCANCEL);
	  ThotCallback (BaseDialog + AttrHREFForm, INTEGER_DATA, (char*) 3);
	  break;
	  
	case IDC_CLEAR:
	  ThotCallback (BaseDialog + AttrHREFForm, INTEGER_DATA, (char*) 4);
	  TmpDocName[0] = EOS;
	  SetDlgItemText (hwnDlg, IDC_GETURL, "");
	  break;
	  
	case IDCANCEL:
	  ThotCallback (BaseDialog + AttrHREFForm, INTEGER_DATA, (char*) 0);
	  HrefUrl[0] = EOS;
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
LRESULT CALLBACK HelpDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam,
							  LPARAM lParam)
{
  switch (msg)
    {
    case WM_INITDIALOG:
      SetWindowText (GetDlgItem (hwnDlg, IDC_VERSION), CurrentPathName);
      SetWindowText (GetDlgItem (hwnDlg, IDC_ABOUT1), Message);
      SetWindowText (GetDlgItem (hwnDlg, IDC_ABOUT2), Message2);
      SetWindowText (GetDlgItem (hwnDlg, ID_CONFIRM),
		  TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
      break;

    case WM_CLOSE:
    case WM_DESTROY:
      EndDialog (hwnDlg, ID_CONFIRM);
      break;
      
    case WM_COMMAND:
      switch (LOWORD (wParam))
	{
	case ID_CONFIRM:
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
LRESULT CALLBACK MathDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam,
							  LPARAM lParam)
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
	  ThotCallback (MathsDialogue + MenuMaths, INTEGER_DATA, (char*)0);
	  break;
	case IDC_ROOT:
	  ThotCallback (MathsDialogue + MenuMaths, INTEGER_DATA, (char*)1);
	  break;
	case IDC_SROOT:
	  ThotCallback (MathsDialogue + MenuMaths, INTEGER_DATA, (char*)2);
	  break;
	case IDC_DIV:
	  ThotCallback (MathsDialogue + MenuMaths, INTEGER_DATA, (char*)3);
	  break;
	case IDC_POWIND:
	  ThotCallback (MathsDialogue + MenuMaths, INTEGER_DATA, (char*)4);
	  break;
	case IDC_IND:
	  ThotCallback (MathsDialogue + MenuMaths, INTEGER_DATA, (char*)5);
	  break;
	case IDC_POW:
	  ThotCallback (MathsDialogue + MenuMaths, INTEGER_DATA, (char*)6);
	  break;
	case IDC_UPDN:
	  ThotCallback (MathsDialogue + MenuMaths, INTEGER_DATA, (char*)7);
	  break;
	case IDC_DOWN:
	  ThotCallback (MathsDialogue + MenuMaths, INTEGER_DATA, (char*)8);
	  break;
	case IDC_UP:
	  ThotCallback (MathsDialogue + MenuMaths, INTEGER_DATA, (char*)9);
	  break;
	case IDC_PAREXP:
	  ThotCallback (MathsDialogue + MenuMaths, INTEGER_DATA, (char*)10);
	  break;
	case IDC_UDLR:
	  ThotCallback (MathsDialogue + MenuMaths, INTEGER_DATA, (char*)11);
	  break;
	case IDC_MATRIX:
	  ThotCallback (MathsDialogue + MenuMaths, INTEGER_DATA, (char*)12);
	  break;
	case IDC_SYM:
	  ThotCallback (MathsDialogue + MenuMaths, INTEGER_DATA, (char*)13);
	  break;
	}
      SetFocus (FrRef[ActiveFrame]);
      break;
      
    default:
      return (FALSE);
    }
  return TRUE;
}
	
/*-----------------------------------------------------------------------
 PrintDlgProc
 ------------------------------------------------------------------------*/
LRESULT CALLBACK PrintDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam,
			       LPARAM lParam)
{
  int      orientation, paper;

  switch (msg)
    {
    case WM_INITDIALOG:
	  PrintForm = hwnDlg;
      SetWindowText (hwnDlg, TtaGetMessage (LIB, TMSG_LIB_PRINT));
      SetWindowText (GetDlgItem (hwnDlg, ID_PRINT),
		     TtaGetMessage (LIB, TMSG_BUTTON_PRINT));
      SetWindowText (GetDlgItem (hwnDlg, IDCANCEL),
		     TtaGetMessage (LIB, TMSG_CANCEL));
      SetWindowText (GetDlgItem (hwnDlg, IDC_OPTIONS),
		     TtaGetMessage (LIB, TMSG_OPTIONS));
      SetWindowText (GetDlgItem (hwnDlg, IDC_TABOFCONTENTS),
		     TtaGetMessage (AMAYA, AM_PRINT_TOC));
      SetWindowText (GetDlgItem (hwnDlg, IDC_LINKS),
		     TtaGetMessage (AMAYA, AM_NUMBERED_LINKS));
      SetWindowText (GetDlgItem (hwnDlg, IDC_PRINTURL),
		     TtaGetMessage (AMAYA, AM_PRINT_URL));
      SetWindowText (GetDlgItem (hwnDlg, IDC_IGNORE_CSS),
		     TtaGetMessage (AMAYA, AM_WITH_CSS));
      CheckDlgButton (hwnDlg, IDC_PRINTURL, PrintURL);
      CheckDlgButton (hwnDlg, IDC_IGNORE_CSS, IgnoreCSS);
      CheckDlgButton (hwnDlg, IDC_TABOFCONTENTS, WithToC);
      CheckDlgButton (hwnDlg, IDC_LINKS, NumberLinks);
      break;
    
    case WM_COMMAND:
      switch (LOWORD (wParam))
	{
	case IDC_TABOFCONTENTS:
	  ThotCallback (BasePrint + PrintOptions, INTEGER_DATA, (char*) 1);
	  break;
	case IDC_LINKS:
	  ThotCallback (BasePrint + PrintOptions, INTEGER_DATA, (char*) 2);
	  break;
	case IDC_PRINTURL:
	  ThotCallback (BasePrint + PrintOptions, INTEGER_DATA, (char*) 3);
	  break;
	case IDC_IGNORE_CSS:
	  ThotCallback (BasePrint + PrintOptions, INTEGER_DATA, (char*) 4);
	  break;
	case ID_PRINT:
	  ThotCallback (BasePrint + PrintSupport, INTEGER_DATA, (char*) 0);
	  PrintForm = NULL;
	  EndDialog (hwnDlg, ID_PRINT);
	  if (TtaGetPrinterDC (FALSE, &orientation, &paper))
	    {
	      EnableWindow (ghwndMain, FALSE);
	      ThotCallback (BasePrint + PaperOrientation, INTEGER_DATA, (char*) orientation);
	      ThotCallback (BasePrint + PaperFormat, INTEGER_DATA, (char*) paper);
	      ThotCallback (BasePrint + PPrinterName, STRING_DATA,
			    CurFileToPrint);
	      ThotCallback (BasePrint + FormPrint, INTEGER_DATA, (char*)1);
	    }
	  break;
	case IDCANCEL:
	  ThotCallback (BasePrint + FormPrint, INTEGER_DATA, (char*)0);
	  PrintForm = NULL;
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
LRESULT CALLBACK TableDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam,
			       LPARAM lParam)
{
  ThotBool ok;
  int      val;

  switch (msg)
  {
    case WM_INITDIALOG:
      SetWindowText (hwnDlg, TtaGetMessage (LIB, TMSG_BUTTON_TABLE));
      SetWindowText (GetDlgItem (hwnDlg, IDC_NUMCOL),
		     TtaGetMessage (AMAYA, AM_COLS));
      SetWindowText (GetDlgItem (hwnDlg, IDC_NUMROWS),
		     TtaGetMessage (AMAYA, AM_ROWS));
      if (WithBorder)
	SetWindowText (GetDlgItem (hwnDlg, IDC_BORDER),
		       TtaGetMessage (AMAYA, AM_BORDER));
      SetWindowText (GetDlgItem (hwnDlg, ID_CONFIRM),
		     TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
      SetWindowText (GetDlgItem (hwnDlg, IDCANCEL),
		     TtaGetMessage (LIB, TMSG_CANCEL));
	
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
		ThotCallback (BaseDialog + TableCols, INTEGER_DATA, (char*) val);
	    }
	  else if (LOWORD (wParam) == IDC_NUMROWSEDIT)
	    {
	      val = GetDlgItemInt (hwnDlg, IDC_NUMROWSEDIT, &ok, TRUE);
	      if (ok)
		ThotCallback (BaseDialog + TableRows, INTEGER_DATA, (char*) val);
	    }
	  else if (LOWORD (wParam) == IDC_BORDEREDIT)
	    {
	      val = GetDlgItemInt (hwnDlg, IDC_BORDEREDIT, &ok, TRUE);
	      if (ok)
		ThotCallback (BaseDialog + TableBorder, INTEGER_DATA, (char*) val);
	    }
	}
      
      switch (LOWORD (wParam))
	{
	case ID_CONFIRM:
	  UserAnswer = 1;
	  ThotCallback (BaseDialog + TableForm, INTEGER_DATA, (char*) 1);
	  EndDialog (hwnDlg, ID_CONFIRM);
	  break;
	case IDCANCEL:
	  UserAnswer = 0;
	  ThotCallback (BaseDialog + TableForm, INTEGER_DATA, (char*) 0);
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
  AttrItemsDlgProc
 ------------------------------------------------------------------------*/
LRESULT CALLBACK AttrItemsDlgProc (ThotWindow hwnDlg, UINT msg,
				   WPARAM wParam, LPARAM lParam)
{
  int        ndx = 0;
  int        i   = 0;
  int        cxChar;
  int        cyChar;
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
      /* store the window ID to be able to destroy it from elsewhere */
      AttrForm = hwnDlg;
      GetClientRect (hwnDlg, &rect);
      hDC = GetDC (hwnDlg);
      SelectObject (hDC, DialogFont);
      GetTextMetrics (hDC, &tm);
      cxChar = tm.tmAveCharWidth;
      cyChar = tm.tmHeight + tm.tmExternalLeading;
      ReleaseDC (hwnDlg, hDC);
      
      SetWindowText (hwnDlg, TtaGetMessage (LIB, TMSG_ATTR));
      SetWindowText (GetDlgItem (hwnDlg, ID_APPLY),
		     TtaGetMessage (LIB, TMSG_APPLY));
      if (RequiredAttr)
	DestroyWindow (GetDlgItem (hwnDlg, ID_DELETE));
      else
        SetWindowText (GetDlgItem (hwnDlg, ID_DELETE),
		       TtaGetMessage (LIB, TMSG_DEL_ATTR));
      SetWindowText (GetDlgItem (hwnDlg, ID_DONE),
		     TtaGetMessage (LIB, TMSG_DONE));
      radio1 = CreateWindow ("BUTTON", &ItemList[ndx],
			     WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 2 * cxChar,
			     cyChar * (1+2*i), 20 * cxChar, 7 * cyChar / 4, hwnDlg,
			     (HMENU) OPT1, (HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);
      /* set the font of the window */
      WIN_SetDialogfont (radio1);
      ndx += strlen (&ItemList[ndx]) + 1;
      i++;
      radio2 = CreateWindow ("BUTTON", &ItemList[ndx],
			     WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 2 * cxChar,
			     cyChar * (1+2*i), 20 * cxChar, 7 * cyChar / 4, hwnDlg,
			     (HMENU) OPT2, (HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);
      /* set the font of the window */
      WIN_SetDialogfont (radio2);
      ndx += strlen (&ItemList[ndx]) + 1;
      i++;
      if (attDlgNbItems > 2)
	{
	  radio3 = CreateWindow ("BUTTON", &ItemList[ndx],
				 WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 2 * cxChar,
				 cyChar * (1+2*i), 20 * cxChar, 7 * cyChar / 4, hwnDlg,
				 (HMENU) OPT3, (HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);
	  /* set the font of the window */
	  WIN_SetDialogfont (radio3);
	  ndx += strlen (&ItemList[ndx]) + 1;
	  i++;
	  if (attDlgNbItems > 3)
	    {	  
	      radio4 = CreateWindow ("BUTTON", &ItemList[ndx],
				     WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 2 * cxChar,
				     cyChar * (1+2*i), 20 * cxChar, 7 * cyChar / 4, hwnDlg,
				     (HMENU) OPT4, (HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);
	      /* set the font of the window */
	      WIN_SetDialogfont (radio4);
	      ndx += strlen (&ItemList[ndx]) + 1;
	      i++;
	      if (attDlgNbItems > 4)
		{
		  radio5 = CreateWindow ("BUTTON", &ItemList[ndx],
					 WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 2 * cxChar,
					 cyChar * (1+2*i), 20 * cxChar, 7 * cyChar / 4, hwnDlg,
					 (HMENU) OPT5, (HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);
		  /* set the font of the window */
		  WIN_SetDialogfont (radio5);
		  ndx += strlen (&ItemList[ndx]) + 1;
		  i++;
		  if (attDlgNbItems > 5)
		    {
		      radio6 = CreateWindow ("BUTTON", &ItemList[ndx],
					     WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 2 * cxChar,
					     cyChar * (1+2*i), 20 * cxChar, 7 * cyChar / 4, hwnDlg,
					     (HMENU) OPT6, (HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);
		      /* set the font of the window */
		      WIN_SetDialogfont (radio6);
		      ndx += strlen (&ItemList[ndx]) + 1;
		      i++;
		      if (attDlgNbItems > 6)
			{	  
			  radio7 = CreateWindow ("BUTTON", &ItemList[ndx],
						 WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 2 * cxChar,
						 cyChar * (1+2*i), 20 * cxChar, 7 * cyChar / 4, hwnDlg,
						 (HMENU) OPT7, (HINSTANCE) GetWindowLong (hwnDlg,
											  GWL_HINSTANCE), NULL);
			  /* set the font of the window */
			  WIN_SetDialogfont (radio7);
			  ndx += strlen (&ItemList[ndx]) + 1;
			  i++;
			  if (attDlgNbItems > 7)
			    {
			      radio8 = CreateWindow ("BUTTON", &ItemList[ndx],
						     WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 2 * cxChar,
						     cyChar * (1+2*i), 20 * cxChar, 7 * cyChar / 4, hwnDlg,
						     (HMENU) OPT8, (HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);
			      /* set the font of the window */
			      WIN_SetDialogfont (radio8);
			      ndx += strlen (&ItemList[ndx]) + 1;
			      i++;
			      if (attDlgNbItems > 8)
				{
				  radio9 = CreateWindow ("BUTTON", &ItemList[ndx],
							 WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 2 * cxChar,
							 cyChar * (1+2*i), 20 * cxChar, 7 * cyChar / 4, hwnDlg,
							 (HMENU) OPT9, (HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);
				  /* set the font of the window */
				  WIN_SetDialogfont (radio9);
				  ndx += strlen (&ItemList[ndx]) + 1;
				  i++;
				}
			    }
			}
		    }
		}
	    }
	}
      groupBx = CreateWindow ("BUTTON", AttDlgTitle,
			      WS_CHILD | WS_VISIBLE | BS_GROUPBOX, cxChar, 0,
			      rect.right - (2 * cxChar), i * (2 * cyChar) + cyChar, hwnDlg,
			      (HMENU) 1, (HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);
      /* set the font of the window */
      WIN_SetDialogfont (groupBx);
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
	      
    case WM_CLOSE:
    case WM_DESTROY:
      AttrForm = NULL;
      if (RequiredAttr)
	  ThotCallback (NumMenuAttrRequired, INTEGER_DATA, (char*) 0);
     else
	ThotCallback (NumMenuAttr, INTEGER_DATA, (char*) 0);
      EndDialog (hwnDlg, IDCANCEL);
      break;

    case WM_COMMAND:
      if (RequiredAttr)
	i = NumMenuAttrEnumNeeded;
      else
	i = NumMenuAttrEnum;
      switch (LOWORD (wParam))
	{
	case OPT1:
	  iLocation = 0;
	  ThotCallback (i, INTEGER_DATA, (char*) iLocation);
	  break;
	case OPT2:
	  iLocation = 1;
	  ThotCallback (i, INTEGER_DATA, (char*) iLocation);
	  break;
	case OPT3:
	  iLocation = 2;
	  ThotCallback (i, INTEGER_DATA, (char*) iLocation);
	  break;
	case OPT4:
	  iLocation = 3;
	  ThotCallback (i, INTEGER_DATA, (char*) iLocation);
	  break;
	case OPT5:
	  iLocation = 4;
	  ThotCallback (i, INTEGER_DATA, (char*) iLocation);
	  break;
	case OPT6:
	  iLocation = 5;
	  ThotCallback (i, INTEGER_DATA, (char*) iLocation);
	  break;
	case OPT7:
	  iLocation = 6;
	  ThotCallback (i, INTEGER_DATA, (char*) iLocation);
	  break;
	case OPT8:
	  iLocation = 7;
	  ThotCallback (i, INTEGER_DATA, (char*) iLocation);
	  break;
	case OPT9:
	  iLocation = 8;
	  ThotCallback (i, INTEGER_DATA, (char*) iLocation);
	  break;
	case ID_APPLY:
	  if (RequiredAttr)
	  {
	    ThotCallback (NumMenuAttrRequired, INTEGER_DATA, (char*) 1);
        EndDialog (hwnDlg, IDCANCEL);
	  }
	  else
	    ThotCallback (NumMenuAttr, INTEGER_DATA, (char*) 1);
	  break;
	case ID_DELETE:
	  ThotCallback (NumMenuAttrEnum, INTEGER_DATA, (char*) -1);
	  ThotCallback (NumMenuAttr, INTEGER_DATA, (char*) 2);
	  AttrForm = NULL;
	  break;
      
	case ID_DONE:
	  AttrForm = NULL;
	  if (RequiredAttr)
	      ThotCallback (NumMenuAttrRequired, INTEGER_DATA, (char*) 0);
	  else
	    ThotCallback (NumMenuAttr, INTEGER_DATA, (char*) 0);
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
 CharsetDlgProc
 ------------------------------------------------------------------------*/
LRESULT CALLBACK CharsetDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam,
					 		     LPARAM lParam)
{
  int             index = 0;
  UINT            i = 0; 
  static ThotWindow   wndMTlist;
  
  switch (msg)
    {
    case WM_INITDIALOG:
      /* get the default GUI font */
      SetWindowText (hwnDlg, TtaGetMessage (AMAYA, AM_SELECT_CHARSET));
      SetWindowText (GetDlgItem (hwnDlg, ID_APPLY), TtaGetMessage (LIB, TMSG_APPLY));
      SetWindowText (GetDlgItem (hwnDlg, IDCANCEL), TtaGetMessage (LIB, TMSG_CANCEL));
	  if (!strcmp (UserCharset, "us-ascii"))
        CheckRadioButton (hwnDlg, IDC_USASCII, IDC_ISOL1, IDC_USASCII);
	  if (!strcmp (UserCharset, "iso-8859-1"))
        CheckRadioButton (hwnDlg, IDC_USASCII, IDC_ISOL1, IDC_ISOL1);
	  else
        CheckRadioButton (hwnDlg, IDC_USASCII, IDC_ISOL1, IDC_UTF8);
      SetFocus (GetDlgItem (hwnDlg, IDC_USASCII));
      return FALSE;
      break;
      
    case WM_CLOSE:
    case WM_DESTROY:
      EndDialog (hwnDlg, ID_DONE);
      break;
      
    case WM_COMMAND:      
      switch (LOWORD (wParam))
	{		  
	case IDC_USASCII:
	  strcpy (SaveFormTmp, "us-ascii");
	  break;
	case IDC_UTF8:
	  strcpy (SaveFormTmp, "UTF-8");
	  break;
	case IDC_ISOL1:
	  strcpy (SaveFormTmp, "iso-8859-1");
	  break;

	case ID_APPLY:
	  EndDialog (hwnDlg, ID_DONE);
	  break;
	  
	case IDCANCEL:
	case WM_CLOSE:
	  SaveFormTmp[0] = EOS;
	  EndDialog (hwnDlg, ID_DONE);
	  break;
	}
      break;
    default: return FALSE;
    }
  return TRUE;
}

/*-----------------------------------------------------------------------
 MimeTypeDlgStatus
 Updates the status bar of the MimeType dialog
 ------------------------------------------------------------------------*/
void MimeTypeDlgStatus (char *msg)
{
  if (MimeTypeDlg && msg)
    SetWindowText (GetDlgItem (MimeTypeDlg, IDC_STATUS), msg);
}

/*-----------------------------------------------------------------------
 MimeTypeDlgProc
 ------------------------------------------------------------------------*/
LRESULT CALLBACK MimeTypeDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam,
				  LPARAM lParam)
{
  int             index = 0;
  UINT            i = 0; 
  static ThotWindow   wndMTlist;

  switch (msg)
    {
    case WM_INITDIALOG:
      /* get the default GUI font */
      MimeTypeDlg = hwnDlg;
      SetWindowText (hwnDlg, TtaGetMessage (AMAYA, AM_SELECT_MIMETYPE));
      SetWindowText (GetDlgItem (hwnDlg, ID_APPLY), TtaGetMessage (LIB, TMSG_APPLY));
      SetWindowText (GetDlgItem (hwnDlg, IDCANCEL), TtaGetMessage (LIB, TMSG_CANCEL));
      
      wndMTlist = CreateWindow ("listbox", NULL,
				WS_CHILD | WS_VISIBLE | LBS_STANDARD | WS_VSCROLL | WS_TABSTOP,
				5, 5, 310, 100, hwnDlg, (HMENU) 1, 
				(HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);
      /* set the font of the window */
      WIN_SetDialogfont (wndMTlist);
      SendMessage (wndMTlist, LB_RESETCONTENT, 0, 0);
      while (i < NbItem && mimeType[index] != '\0')
	{
	  SendMessage (wndMTlist, LB_INSERTSTRING, i, (LPARAM) &mimeType[index]); 
	  index += strlen (&mimeType[index]) + 1;/* Longueur de l'intitule */
	  i++;
	}
      /* select the first list item */
      /* SendMessage (wndMTlist, LB_SETCURSEL, 0, 0); */
      /* put the focus on the first list item */
	  SetDlgItemText (hwnDlg, IDC_MTEDIT, UserMimeType);
      SetFocus (GetDlgItem (hwnDlg, IDC_MTEDIT));
      return FALSE;
      break;
      
    case WM_CLOSE:
    case WM_DESTROY:
      /* ThotCallback (BaseDialog + MimeTypeForm, INTEGER_DATA, (char *) 0); */
      MimeTypeDlg = NULL;
      EndDialog (hwnDlg, ID_DONE);
      break;
      
    case WM_COMMAND:
      if (LOWORD (wParam) == 1)
	{
	  if (HIWORD (wParam) == LBN_SELCHANGE)
	    {
	      itemIndex = SendMessage (wndMTlist, LB_GETCURSEL, 0, 0);
	      itemIndex = SendMessage (wndMTlist, LB_GETTEXT, itemIndex,
				       (LPARAM) SzBuffer);
	    }
	  else if (HIWORD (wParam) == LBN_DBLCLK)
	    {
	      if (LB_ERR == (itemIndex = SendMessage (wndMTlist, LB_GETCURSEL, 0, 0L)))
		break;
	      itemIndex = SendMessage (wndMTlist, LB_GETTEXT, itemIndex,
				       (LPARAM) SzBuffer);
	    }
	  SetDlgItemText (hwnDlg, IDC_MTEDIT, SzBuffer);
	  if (HIWORD (wParam) == LBN_DBLCLK)
	    {
	      GetDlgItemText (hwnDlg, IDC_MTEDIT, SaveFormTmp, sizeof (SaveFormTmp) - 1);
	      if (SaveFormTmp[0] != EOS)
		{
		  MimeTypeDlg = NULL;
		  EndDialog (hwnDlg, ID_DONE);
		}
	      break;
	    }
	}
      
      switch (LOWORD (wParam))
	{	  	  
	case ID_APPLY:
	  GetDlgItemText (hwnDlg, IDC_MTEDIT, SaveFormTmp, sizeof (SaveFormTmp) - 1);
   	  ThotCallback (BaseDialog + MimeTypeForm, INTEGER_DATA, (char *) 1);
	  if (SaveFormTmp[0] != EOS)
	    {
	      MimeTypeDlg = NULL;
	      EndDialog (hwnDlg, ID_DONE);
	    }
	  break;
	  
	case IDCANCEL:
	case WM_CLOSE:
 	  ThotCallback (BaseDialog + MimeTypeForm, INTEGER_DATA, (char *) 0);
 	  MimeTypeDlg = NULL;
	  EndDialog (hwnDlg, ID_DONE);
	  break;
	}
      break;
    default: return FALSE;
    }
  return TRUE;
}

/*-----------------------------------------------------------------------
 SaveAsDlgStatus
 Updates the status bar of the SaveAs dialog
 ------------------------------------------------------------------------*/
void SaveAsDlgStatus (char *msg)
{
  if (SaveAsForm && msg)
    SetWindowText (GetDlgItem (SaveAsForm, IDC_STATUS), msg);
}


/*----------------------------------------------------------------------
  WIN_ListSaveDirectory
  ----------------------------------------------------------------------*/
void WIN_ListSaveDirectory (ThotWindow hwnDlg, char *title, char *fileName)
{
  char               *szFilter;
  char                szFileTitle[256];

  szFilter = APPFILENAMEFILTER;
  TtaExtractName (fileName, SavePath, DocumentName);
  OpenFileName.lStructSize = sizeof (OPENFILENAME); 
  OpenFileName.hwndOwner = hwnDlg; 
  OpenFileName.lpstrFilter = szFilter;
  OpenFileName.lpstrFile = DocumentName; 
  OpenFileName.nMaxFile = MAX_LENGTH; 
  OpenFileName.lpstrFileTitle = szFileTitle; 
  OpenFileName.lpstrTitle = title; 
  OpenFileName.nMaxFileTitle = sizeof (szFileTitle); 
  OpenFileName.lpstrInitialDir = SavePath; 
  OpenFileName.Flags = OFN_SHOWHELP | OFN_HIDEREADONLY;
  if (GetSaveFileName (&OpenFileName))
    strcpy (fileName, OpenFileName.lpstrFile);
}

/*-----------------------------------------------------------------------
 SaveAsDlgProc
 ------------------------------------------------------------------------*/
LRESULT CALLBACK SaveAsDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam,
				LPARAM lParam)
{
  char buff[500];

  switch (msg)
    {
    case WM_INITDIALOG:
      SaveAsForm = hwnDlg;
      SetWindowText (hwnDlg, TtaGetMessage (AMAYA, AM_SAVE_AS));
      SetWindowText (GetDlgItem (hwnDlg, IDC_DOCLOCATION), 
		     TtaGetMessage (AMAYA, AM_DOC_LOCATION));
      SetWindowText (GetDlgItem (hwnDlg, ID_CONFIRM),
		     TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
      SetWindowText (GetDlgItem (hwnDlg, ID_CLEAR), TtaGetMessage (AMAYA, AM_CLEAR));
      SetWindowText (GetDlgItem (hwnDlg, IDC_BROWSE), TtaGetMessage (AMAYA, AM_BROWSE));
      SetWindowText (GetDlgItem (hwnDlg, IDCANCEL), TtaGetMessage (LIB, TMSG_CANCEL));
      SetDlgItemText (hwnDlg, IDC_EDITDOCSAVE, CurrentPathName);
      
	  if (HTMLFormat ||
		  DocumentTypes[SavingDocument] == docMath ||
	      DocumentTypes[SavingDocument] == docSVG ||
          DocumentTypes[SavingDocument] == docXml)
	  {
	    SetWindowText (GetDlgItem (hwnDlg, IDC_COPYIMG),
			 TtaGetMessage (AMAYA, AM_BCOPY_IMAGES));
	    SetWindowText (GetDlgItem (hwnDlg, IDC_TRANSFORMURL),
			 TtaGetMessage (AMAYA, AM_BTRANSFORM_URL));
	    SetWindowText (GetDlgItem (hwnDlg, IDC_IMGLOCATION),
			 TtaGetMessage (AMAYA, AM_IMAGES_LOCATION));
	    copyImgWnd = GetDlgItem (hwnDlg, IDC_COPYIMG);
	    transURLWnd = GetDlgItem (hwnDlg, IDC_TRANSFORMURL);
	  }

      if (HTMLFormat)
	  {
	    SetWindowText (GetDlgItem (hwnDlg, IDC_OUTPUTGROUP),
			 TtaGetMessage (LIB, TMSG_DOCUMENT_FORMAT));
	    SetWindowText (GetDlgItem (hwnDlg, IDC_HTML), "HTML");
	    SetWindowText (GetDlgItem (hwnDlg, IDC_XML), "XML");
	    SetWindowText (GetDlgItem (hwnDlg, IDC_TEXT),
			   TtaGetMessage (AMAYA, AM_TEXT));
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
	    if (!SaveAsText && CopyImages)
	      CheckRadioButton (hwnDlg, IDC_COPYIMG, IDC_COPYIMG, IDC_COPYIMG);
	  }
	  else if (DocumentTypes[SavingDocument] == docMath ||
               DocumentTypes[SavingDocument] == docXml)
	      EnableWindow (copyImgWnd, FALSE);

	  if (UpdateURLs)
	      CheckRadioButton (hwnDlg, IDC_TRANSFORMURL, IDC_TRANSFORMURL, IDC_TRANSFORMURL);

      /* mime type */
      _snprintf (buff, 500, "MIME type: %s", 
		 UserMimeType[0] != EOS ? UserMimeType : TtaGetMessage (AMAYA, AM_UNKNOWN));
      SetDlgItemText (hwnDlg, IDC_MIMETYPE, buff);
      SetDlgItemText (hwnDlg, ID_CHANGEMIMETYPE,  TtaGetMessage (AMAYA, AM_CHANGE));
      /* charset */
      _snprintf (buff, 500, "Charset: %s", 
		 UserCharset[0] != EOS ? UserCharset : TtaGetMessage (AMAYA, AM_UNKNOWN));
      SetDlgItemText (hwnDlg, IDC_CHARSET, buff);
      SetDlgItemText (hwnDlg, ID_CHANGECHARSET, TtaGetMessage (AMAYA, AM_CHANGE));

      /* set the default focus and return FALSE to validate it */
      SetFocus (GetDlgItem (hwnDlg, IDC_EDITDOCSAVE));
      return FALSE;
      break;
    
    case WM_COMMAND:
      if (HIWORD (wParam) == EN_UPDATE)
	{
	  if (LOWORD (wParam) == IDC_EDITDOCSAVE)
	    {
	      GetDlgItemText (hwnDlg, IDC_EDITDOCSAVE, UrlToOpen, sizeof (UrlToOpen) - 1);
	      ThotCallback (BaseDialog + NameSave, STRING_DATA, UrlToOpen);
	    }
	  else if (LOWORD (wParam) == IDC_EDITIMGSAVE)
	    {
	      GetDlgItemText (hwnDlg, IDC_EDITIMGSAVE, UrlToOpen, sizeof (UrlToOpen) - 1);
	      ThotCallback (BaseDialog + ImgDirSave, STRING_DATA, UrlToOpen);
	    }
	}

      switch (LOWORD (wParam))
	{
	case IDC_HTML:
	  EnableWindow (transURLWnd, TRUE);
	  EnableWindow (copyImgWnd, TRUE);
	  ThotCallback (BaseDialog + RadioSave, INTEGER_DATA, (char*) 0);
	  SetDlgItemText (hwnDlg, IDC_EDITDOCSAVE, DocToOpen);
	  return 0;
	  
	case IDC_XML:
	  EnableWindow (transURLWnd, TRUE);
	  EnableWindow (copyImgWnd, TRUE);
	  ThotCallback (BaseDialog + RadioSave, INTEGER_DATA, (char*) 1);
	  SetDlgItemText (hwnDlg, IDC_EDITDOCSAVE, DocToOpen);
	  return 0;
	  
	case IDC_TEXT:
	  EnableWindow (transURLWnd, FALSE);
	  EnableWindow (copyImgWnd, FALSE);
	  
	  ThotCallback (BaseDialog + RadioSave, INTEGER_DATA, (char*) 2);
	  SetDlgItemText (hwnDlg, IDC_EDITDOCSAVE, DocToOpen);
	  return 0;
	  
	case IDC_COPYIMG:
	  ThotCallback (BaseDialog + ToggleSave, INTEGER_DATA, (char*) 0);
	  break;
	  
	case IDC_TRANSFORMURL:
	  ThotCallback (BaseDialog + ToggleSave, INTEGER_DATA, (char*) 1);
	  break;

  	case ID_CHANGECHARSET:
	  ThotCallback (BaseDialog + SaveForm, INTEGER_DATA, (char*) 4);
	  if (SaveFormTmp[0] != EOS)
	  {
	    _snprintf (buff, 500, "Charset: %s", UserCharset);
	    SetDlgItemText (hwnDlg, IDC_CHARSET, buff);	
	  }
	  break;

	case ID_CHANGEMIMETYPE:
	  ThotCallback (BaseDialog + SaveForm, INTEGER_DATA, (char*) 5);
	  if (SaveFormTmp[0] != EOS)
	  {
	    _snprintf (buff, 500, "MIME type: %s", UserMimeType);	  
	    SetDlgItemText (hwnDlg, IDC_MIMETYPE, buff);
	  }
	  break;
	  
	case ID_CLEAR:
	  SetDlgItemText (hwnDlg, IDC_EDITDOCSAVE, "");
	  SetDlgItemText (hwnDlg, IDC_EDITIMGSAVE, "");
	  ThotCallback (BaseDialog + SaveForm, INTEGER_DATA, (char*) 3);
	  break;
	  
	case IDC_BROWSE:
      /* by default keep the same document name */
	  WIN_ListSaveDirectory (hwnDlg, TtaGetMessage (AMAYA, AM_SAVE_AS), UrlToOpen);
	  SetDlgItemText (hwnDlg, IDC_EDITDOCSAVE, UrlToOpen);
	  ThotCallback (BaseDialog + NameSave, STRING_DATA, UrlToOpen);
	  break;
	  
	case IDCANCEL:
	  EndDialog (hwnDlg, IDCANCEL);
	  ThotCallback (BaseDialog + SaveForm, INTEGER_DATA, (char*) 0);
	  SaveAsForm = NULL;
	  break;
	  
	case ID_CONFIRM:
	  EndDialog (hwnDlg, ID_CONFIRM);
	  ThotCallback (BaseDialog + SaveForm, INTEGER_DATA, (char*) 1);
	  SaveAsForm = NULL;
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
LRESULT CALLBACK OpenDocDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam,
								 LPARAM lParam)
{
  switch (msg)
    {
    case WM_INITDIALOG:
      SetWindowText (hwnDlg, TtaGetMessage (AMAYA, AM_OPEN_URL));
      SetWindowText (GetDlgItem (hwnDlg, IDC_URLMESSAGE),
		     TtaGetMessage (AMAYA, AM_LOCATION));
      SetWindowText (GetDlgItem (hwnDlg, ID_CONFIRM),
		     TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
      SetWindowText (GetDlgItem (hwnDlg, IDC_BROWSE), TtaGetMessage (AMAYA, AM_BROWSE));
      SetWindowText (GetDlgItem (hwnDlg, IDC_CLEAR), TtaGetMessage (AMAYA, AM_CLEAR));
      SetWindowText (GetDlgItem (hwnDlg, IDCANCEL), TtaGetMessage (LIB, TMSG_CANCEL));

      SetWindowText (hwnDlg, WndTitle);
      SetDlgItemText (hwnDlg, IDC_GETURL, UrlToOpen);
	  /* put the focus on the first dialog item */
	  SetFocus (GetDlgItem (hwnDlg, IDC_GETURL));
	  return FALSE;
      break;

    case WM_COMMAND:
      if (HIWORD (wParam) == EN_UPDATE)
	{
	  if (LOWORD (wParam) == IDC_GETURL)
	    {
	      GetDlgItemText (hwnDlg, IDC_GETURL, UrlToOpen, sizeof (UrlToOpen) - 1);
	      if (UrlToOpen[0] != EOS)
		ThotCallback (BaseDialog + URLName, STRING_DATA, UrlToOpen);
	    }
	}

    switch (LOWORD (wParam))
      {
      case ID_CONFIRM:
	ThotCallback (BaseDialog + OpenForm, INTEGER_DATA, (char*)1);
	EndDialog (hwnDlg, ID_CONFIRM);
	break;

      case IDC_BROWSE:
	OpenFileName.lStructSize       = sizeof (OPENFILENAME);
	OpenFileName.hwndOwner         = hwnDlg;
	OpenFileName.hInstance         = hInstance;
	OpenFileName.lpstrFilter       = (LPTSTR) SzFilter;
	OpenFileName.lpstrCustomFilter = (LPTSTR) NULL;
	OpenFileName.nMaxCustFilter    = 0L;
	OpenFileName.nFilterIndex      = indexFilter;
	OpenFileName.lpstrFile         = (LPTSTR) TmpDocName;
	OpenFileName.nMaxFile          = MAX_LENGTH;
	OpenFileName.lpstrInitialDir   = (LPTSTR) DirectoryName;
	OpenFileName.lpstrTitle        = TtaGetMessage (AMAYA, AM_OPEN_URL);
	OpenFileName.nFileOffset       = 0;
	OpenFileName.nFileExtension    = 0;
	OpenFileName.lpstrDefExt       = "html";
	OpenFileName.lCustData         = 0;
	OpenFileName.Flags             = OFN_SHOWHELP | OFN_HIDEREADONLY;
	
	if (GetOpenFileName (&OpenFileName))
	{
	  strcpy (UrlToOpen, OpenFileName.lpstrFile);
      indexFilter = OpenFileName.nFilterIndex;
      strcpy (DirectoryName, OpenFileName.lpstrInitialDir);
	SetDlgItemText (hwnDlg, IDC_GETURL, UrlToOpen);
	if (UrlToOpen[0] != EOS)
	  {
	    ThotCallback (BaseDialog + URLName, STRING_DATA, UrlToOpen);
	    EndDialog (hwnDlg, ID_CONFIRM);
	    ThotCallback (BaseDialog + OpenForm, INTEGER_DATA, (char*) 1);
	  }
	}
	break;

      case IDC_CLEAR:
	ThotCallback (BaseDialog + OpenForm, INTEGER_DATA, (char*) 3);
	TmpDocName[0] = EOS;
	SetDlgItemText (hwnDlg, IDC_GETURL, "");
	break;
      
      case IDCANCEL:
	ThotCallback (BaseDialog + OpenForm, INTEGER_DATA, (char*) 0);
	UrlToOpen[0] = EOS;
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
LRESULT CALLBACK OpenImgDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam,
								 LPARAM lParam)
{
  switch (msg)
    {
    case WM_INITDIALOG:
      SetWindowText (hwnDlg, TtaGetMessage (LIB, TMSG_BUTTON_IMG));
      SetWindowText (GetDlgItem (hwnDlg, IDC_URLMESSAGE),
		     TtaGetMessage (AMAYA, AM_LOCATION));
      SetWindowText (GetDlgItem (hwnDlg, ID_CONFIRM),
		     TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
      SetWindowText (GetDlgItem (hwnDlg, IDC_BROWSE), TtaGetMessage (AMAYA, AM_BROWSE));
      SetWindowText (GetDlgItem (hwnDlg, IDCANCEL), TtaGetMessage (LIB, TMSG_CANCEL));
      
      SetDlgItemText (hwnDlg, IDC_GETURL, UrlToOpen);
      UrlToOpen[0] = EOS;
      if (RequiredAttr)
	{
	  SetWindowText (GetDlgItem (hwnDlg, IDC_ALTMESSAGE),
			 TtaGetMessage (AMAYA, AM_ALT));
	  SetDlgItemText (hwnDlg, IDC_GETALT, ImgAlt);
	}
      else
	{
	  DestroyWindow (GetDlgItem (hwnDlg, IDC_ALTMESSAGE));
	  DestroyWindow (GetDlgItem (hwnDlg, IDC_GETALT));
	}
      break;
      
    case WM_COMMAND:
      switch (LOWORD (wParam))
	{
	case ID_CONFIRM:
	  GetDlgItemText (hwnDlg, IDC_GETURL, UrlToOpen, sizeof (UrlToOpen) - 1);
	  if (RequiredAttr)
	    {
	      /* ALT attribute required */
	      GetDlgItemText (hwnDlg, IDC_GETALT, AltText, sizeof (AltText) - 1);
	      if (!AltText || AltText[0] == 0)
		MessageBox (hwnDlg, TtaGetMessage (AMAYA, AM_ALT_MISSING),
			    TtaGetMessage (LIB, TMSG_BUTTON_IMG),
			    MB_OK | MB_ICONERROR);
	      else 
		{
		  ThotCallback (BaseImage + ImageAlt, STRING_DATA, AltText);
		  ThotCallback (BaseImage + ImageURL, STRING_DATA, UrlToOpen);
		  ThotCallback (BaseImage + FormImage, INTEGER_DATA, (char*) 1);
		  EndDialog (hwnDlg, ID_CONFIRM);
		}
	    }
	  break;
	    
	case IDC_BROWSE:
	  OpenFileName.lStructSize       = sizeof (OPENFILENAME);
	  OpenFileName.hwndOwner         = hwnDlg;
	  OpenFileName.hInstance         = hInstance;
	  OpenFileName.lpstrFilter       = (LPTSTR) SzFilter;
	  OpenFileName.lpstrCustomFilter = (LPTSTR) NULL;
	  OpenFileName.nMaxCustFilter    = 0L;
	  OpenFileName.nFilterIndex      = indexImgFilter;
	  OpenFileName.lpstrFile         = (LPTSTR) TmpDocName;
	  OpenFileName.nMaxFile          = MAX_LENGTH;
	  OpenFileName.lpstrInitialDir   = (LPTSTR) DirectoryImage;
	  OpenFileName.lpstrTitle        = TtaGetMessage (AMAYA, AM_BROWSE);
	  OpenFileName.nFileOffset       = 0;
	  OpenFileName.nFileExtension    = 0;
	  OpenFileName.lpstrDefExt       = "";
	  OpenFileName.lCustData         = 0;
	  OpenFileName.Flags             = OFN_SHOWHELP | OFN_HIDEREADONLY;
	  
	  if (GetOpenFileName (&OpenFileName))
	  {
	    strcpy (UrlToOpen, OpenFileName.lpstrFile);
		indexImgFilter = OpenFileName.nFilterIndex;
        strcpy (DirectoryImage, OpenFileName.lpstrInitialDir);
	  }
	  
	  SetDlgItemText (hwnDlg, IDC_GETURL, UrlToOpen);
	  break;
	  
	case IDCANCEL:
	  ThotCallback (BaseImage + FormImage, INTEGER_DATA, (char*) 0);
	  UrlToOpen[0] = 0;
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
LRESULT CALLBACK GraphicsDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam,
								  LPARAM lParam)
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
      SetFocus (FrRef[ActiveFrame]);
      switch (LOWORD (wParam))
	{
	case ID_DONE:
	  GraphPal = NULL;
	  EndDialog (hwnDlg, ID_DONE);
	  break;	 
	  
	case IDC_GLINE:
	  ThotCallback (GraphDialogue + MenuGraph, INTEGER_DATA, (char*)0);
	  break;
	  
	case IDC_GRECT:
	  ThotCallback (GraphDialogue + MenuGraph, INTEGER_DATA, (char*)1);
	  break;
	  
	case IDC_GRRECT:
	  ThotCallback (GraphDialogue + MenuGraph, INTEGER_DATA, (char*)2);
	  break;

	case IDC_GCIRCLE:
	  ThotCallback (GraphDialogue + MenuGraph, INTEGER_DATA, (char*)3);
	  break;
	  
	case IDC_GELLIPSE:
	  ThotCallback (GraphDialogue + MenuGraph, INTEGER_DATA, (char*)4);
	  break;
	  
	case IDC_GPOLYLINE:
	  ThotCallback (GraphDialogue + MenuGraph, INTEGER_DATA, (char*)5);
	  break;
	  
	case IDC_GCPOLYLINE:
	  ThotCallback (GraphDialogue + MenuGraph, INTEGER_DATA, (char*)6);
	  break;
	  
	case IDC_GCURVE:
	  ThotCallback (GraphDialogue + MenuGraph, INTEGER_DATA, (char*)7);
	  break;
	  
	case IDC_GCCURVE:
	  ThotCallback (GraphDialogue + MenuGraph, INTEGER_DATA, (char*)8);
	  break;
	  
	case IDC_GALPHA1:
	  ThotCallback (GraphDialogue + MenuGraph, INTEGER_DATA, (char*)9);
	  break;
	  
	case IDC_GALPHA2:
	  ThotCallback (GraphDialogue + MenuGraph, INTEGER_DATA, (char*)10);
	  break;
	  
	case IDC_GGROUP:
	  ThotCallback (GraphDialogue + MenuGraph, INTEGER_DATA, (char*)11);
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
LRESULT CALLBACK SaveListDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam,
								  LPARAM lParam)
{
  static ThotWindow  wndSavList;
  int                index = 0;
  UINT               i = 0;

  switch (msg)
    {
    case WM_INITDIALOG:
      /* get the default GUI font */
      SetWindowText (hwnDlg, TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
      SetWindowText (GetDlgItem (hwnDlg, IDC_MSG),
		     TtaGetMessage (AMAYA, AM_WARNING_SAVE_OVERWRITE));
      SetWindowText (GetDlgItem (hwnDlg, ID_CONFIRM),
		     TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
      SetWindowText (GetDlgItem (hwnDlg, IDCANCEL),
		     TtaGetMessage (LIB, TMSG_CANCEL));
      
      wndSavList = CreateWindow ("listbox", NULL,
		  WS_CHILD | WS_VISIBLE | LBS_STANDARD,
				  10, 30, 260, 180, hwnDlg, (HMENU) 1, 
				  (HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);
      
      /* set the font of the window */
      WIN_SetDialogfont (wndSavList);
      SendMessage (wndSavList, LB_RESETCONTENT, 0, 0);
      while (i < NbItem && SavList[index] != EOS)
	{
	  SendMessage (wndSavList, LB_INSERTSTRING, i, (LPARAM) &SavList[index]); 
	  index += strlen (&SavList[index]) + 1;	/* Longueur de l'intitule */
	  i++;
	}
      break;
      
    case WM_COMMAND:
      if (LOWORD (wParam) == 1 && HIWORD (wParam) == LBN_SELCHANGE)
	{
	  itemIndex = SendMessage (wndSavList, LB_GETCURSEL, 0, 0);
	  itemIndex = SendMessage (wndSavList, LB_GETTEXT, itemIndex, (LPARAM) SzBuffer);
	  SetDlgItemText (hwnDlg, IDC_LANGEDIT, SzBuffer);
	}
      switch (LOWORD (wParam))
	{
	case ID_CONFIRM:
	  ThotCallback (BaseDialog + ConfirmSave, INTEGER_DATA, (char*)1);
	  EndDialog (hwnDlg, ID_CONFIRM);
	  SendMessage (SaveAsForm, WM_DESTROY, 0, 0);
	  break;
	  
	case IDCANCEL:
	  ThotCallback (BaseDialog + ConfirmSave, INTEGER_DATA, (char*)0);
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
LRESULT CALLBACK CloseDocDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam,
								  LPARAM lParam)
{
  switch (msg)
    {
    case WM_INITDIALOG:
      SetWindowText (hwnDlg, TtaGetMessage (LIB, TMSG_CLOSE_DOC));
      SetWindowText (GetDlgItem (hwnDlg, IDC_CLOSEMSG), Message);
      SetWindowText (GetDlgItem (hwnDlg, ID_SAVEDOC),
		  TtaGetMessage (LIB, TMSG_SAVE_DOC));
      SetWindowText (GetDlgItem (hwnDlg, IDC_DONTSAVE),
		  TtaGetMessage (LIB, TMSG_CLOSE_DON_T_SAVE));
      SetWindowText (GetDlgItem (hwnDlg, IDCANCEL), TtaGetMessage (LIB, TMSG_CANCEL));
      WndClose = hwnDlg;
      /* set the font of the window */
      WIN_SetDialogfont (WndClose);
      break;
      
    case WM_CLOSE:
    case WM_DESTROY:
	  if (WndClose == hwnDlg)
		{
        WndClose = NULL;
        EndDialog (hwnDlg, IDCANCEL);
		}
	  else
		return FALSE;
      break;

    case WM_COMMAND:
      switch (LOWORD (wParam))
	{
	case IDCANCEL:
	  ThotCallback (NumFormClose, INTEGER_DATA, (char*)0);
      WndClose = NULL;
	  EndDialog (hwnDlg, IDCANCEL);
	  break;
	  
	case ID_SAVEDOC:
	  ThotCallback (NumFormClose, INTEGER_DATA, (char*)1);
      WndClose = NULL;
	  EndDialog (hwnDlg, IDCANCEL);
	  break;
	  
	case IDC_DONTSAVE:

	  ThotCallback (NumFormClose, INTEGER_DATA, (char*)2);
      WndClose = NULL;
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
 LanguageDlgProc
 ------------------------------------------------------------------------*/
LRESULT CALLBACK LanguageDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam,
								  LPARAM lParam)
{
  int             index = 0;
  UINT            i = 0;

  switch (msg)
    {
    case WM_INITDIALOG:
      /* get the default GUI font */
      /* destroy the focus of the previous open dialog */
      LangForm = hwnDlg;
      SetWindowText (hwnDlg, TtaGetMessage (LIB, TMSG_LANGUAGE));
	  SetWindowText (GetDlgItem (hwnDlg, ID_APPLY), TtaGetMessage (LIB, TMSG_APPLY));
	  SetWindowText (GetDlgItem (hwnDlg, ID_DELETE),
		  TtaGetMessage (LIB, TMSG_DEL_ATTR));
	  SetWindowText (GetDlgItem (hwnDlg, ID_DONE), TtaGetMessage (LIB, TMSG_DONE));
	  SetWindowText (GetDlgItem (hwnDlg, IDC_LANGELEM),
			 TtaGetMessage (LIB, TMSG_LANG_OF_EL));
	  SetWindowText (GetDlgItem (hwnDlg, IDC_INHERITEDLANG), Message2);
      
      wndLangList = GetDlgItem (hwnDlg, IDC_LANGLIST),
      /* set the font of the window */
      WIN_SetDialogfont (wndLangList);
      SendMessage (wndLangList, LB_RESETCONTENT, 0, 0);
    while (i < NbItem && ItemList[index] != EOS)
      {
	/* keep in mind the current selected entry */
	if (*WinCurLang && !strcmp (WinCurLang, &ItemList[index]))
	  Num_lang = index;
	SendMessage (wndLangList, LB_INSERTSTRING, i, (LPARAM) &ItemList[index]); 
	index += strlen (&ItemList[index]) + 1;/* Longueur de l'intitule */
	i++;
      }
    if (Num_lang != -1)
      {
        SendMessage (wndLangList, LB_SETCURSEL, (WPARAM)Num_lang, (LPARAM)0);
	Num_lang = SendMessage (wndLangList, LB_GETTEXT, Num_lang,
				(LPARAM) WinCurLang);
      }
    SetWindowText (GetDlgItem (hwnDlg, IDC_LNGEDIT), WinCurLang);
    break;

    case WM_CLOSE:
    case WM_DESTROY:
      LangForm = NULL;
      EndDialog (hwnDlg, ID_DONE);
      break;

    case WM_COMMAND:
      if (LOWORD (wParam) == 1 || HIWORD (wParam) == LBN_SELCHANGE ||
	  HIWORD (wParam) == LBN_DBLCLK)
	{
	  if (HIWORD (wParam) == LBN_SELCHANGE)
	    {
	      Num_lang = SendMessage (wndLangList, LB_GETCURSEL, 0, 0);
	      Num_lang = SendMessage (wndLangList, LB_GETTEXT, Num_lang,
				      (LPARAM) WinCurLang);
	      SetWindowText (GetDlgItem (hwnDlg, IDC_LNGEDIT), WinCurLang);
	    }
	  else if (HIWORD (wParam) == LBN_DBLCLK)
	    {
	      if (LB_ERR == (Num_lang = SendMessage (wndLangList, LB_GETCURSEL, 0, 0L)))
		break;
	      Num_lang = SendMessage (wndLangList, LB_GETTEXT, Num_lang,
				      (LPARAM) WinCurLang);
	    }
	  SetDlgItemText (hwnDlg, IDC_LANGEDIT, WinCurLang);
	  ThotCallback (NumSelectLanguage, STRING_DATA, WinCurLang);
	  if (HIWORD (wParam) == LBN_DBLCLK)
	    {
	      ThotCallback (NumFormLanguage, INTEGER_DATA, (char*) 1);
	      LangForm = NULL;
	      EndDialog (hwnDlg, ID_DONE);
	      return FALSE;
	    }
	}
      
      switch (LOWORD (wParam))
	{
	case ID_APPLY:
	  ThotCallback (NumFormLanguage, INTEGER_DATA, (char*) 1);
	  break;
	  
	case ID_DELETE:
	  ThotCallback (NumFormLanguage, INTEGER_DATA, (char*) 2);
	  break;
	  
	case ID_DONE:
	  ThotCallback (NumFormLanguage, INTEGER_DATA, (char*) 0);
	  LangForm = NULL;
	  EndDialog (hwnDlg, ID_DONE);
	  break;
	  
	case WM_CLOSE:
	case WM_DESTROY:
	  LangForm = NULL;
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
LRESULT CALLBACK CharacterDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam,
								   LPARAM lParam)
{
  switch (msg)
    {
    case WM_INITDIALOG:
	  CharacterForm = hwnDlg;
      SetWindowText (hwnDlg, TtaGetMessage (LIB, TMSG_CHAR));
      SetWindowText (GetDlgItem (hwnDlg, IDC_FONTFAMILYGROUP),
		  TtaGetMessage (LIB, TMSG_FONT_FAMILY));
      SetWindowText (GetDlgItem (hwnDlg, IDC_TIMES), "Times");
      SetWindowText (GetDlgItem (hwnDlg, IDC_HELVETICA), "Helvetica");
      SetWindowText (GetDlgItem (hwnDlg, IDC_COURIER), "Courrier");
      SetWindowText (GetDlgItem (hwnDlg, IDC_DEFAULTFAMILY),
		  TtaGetMessage (LIB, TMSG_UNCHANGED));
      
      SetWindowText (GetDlgItem (hwnDlg, IDC_CHARSTYLEGROUP),
		  TtaGetMessage (LIB, TMSG_STYLE));
      SetWindowText (GetDlgItem (hwnDlg, IDC_ROMAN), TtaGetMessage (LIB, TMSG_ROMAN));
      SetWindowText (GetDlgItem (hwnDlg, IDC_ITALIC), TtaGetMessage (LIB, TMSG_ITALIC));
      SetWindowText (GetDlgItem (hwnDlg, IDC_OBLIQUE),
		  TtaGetMessage (LIB, TMSG_OBLIQUE));
      SetWindowText (GetDlgItem (hwnDlg, IDC_DEFAULTSTYLE),
		  TtaGetMessage (LIB, TMSG_UNCHANGED));
      
      SetWindowText (GetDlgItem (hwnDlg, IDC_UNDERLINEGROUP),
		  TtaGetMessage (LIB, TMSG_LINE));
      SetWindowText (GetDlgItem (hwnDlg, IDC_NORMAL), TtaGetMessage (LIB, TMSG_NORMAL));
      SetWindowText (GetDlgItem (hwnDlg, IDC_UNDERLINE),
		  TtaGetMessage (LIB, TMSG_UNDERLINE));
      SetWindowText (GetDlgItem (hwnDlg, IDC_OVERLINE),
		  TtaGetMessage (LIB, TMSG_OVERLINE));
      SetWindowText (GetDlgItem (hwnDlg, IDC_CROSSOUT),
		  TtaGetMessage (LIB, TMSG_CROSS_OUT));
      SetWindowText (GetDlgItem (hwnDlg, IDC_DEFAULTUNDERLINE), TtaGetMessage (LIB, TMSG_UNCHANGED));
      
      SetWindowText (GetDlgItem (hwnDlg, IDC_BOLDGROUP), TtaGetMessage (LIB, TMSG_BOLDNESS));
      SetWindowText (GetDlgItem (hwnDlg, IDC_NORMALBOLD), TtaGetMessage (LIB, TMSG_NOT_BOLD));
      SetWindowText (GetDlgItem (hwnDlg, IDC_BOLD), TtaGetMessage (LIB, TMSG_BOLD));
      SetWindowText (GetDlgItem (hwnDlg, IDC_DEFAULTBOLD), TtaGetMessage (LIB, TMSG_UNCHANGED));
      
      SetWindowText (GetDlgItem (hwnDlg, IDC_BODYSIZEGROUP),
		  TtaGetMessage (LIB, TMSG_BODY_SIZE_PTS));
      SetWindowText (GetDlgItem (hwnDlg, IDC_06PT), " 6 pt");
      SetWindowText (GetDlgItem (hwnDlg, IDC_08PT), " 8 pt");
      SetWindowText (GetDlgItem (hwnDlg, IDC_10PT), "10 pt");
      SetWindowText (GetDlgItem (hwnDlg, IDC_12PT), "12 pt");
      SetWindowText (GetDlgItem (hwnDlg, IDC_14PT), "14 pt");
      SetWindowText (GetDlgItem (hwnDlg, IDC_16PT), "16 pt");
      SetWindowText (GetDlgItem (hwnDlg, IDC_20PT), "20 pt");
      SetWindowText (GetDlgItem (hwnDlg, IDC_24PT), "24 pt");
      SetWindowText (GetDlgItem (hwnDlg, IDC_30PT), "30 pt");
      SetWindowText (GetDlgItem (hwnDlg, IDC_40PT), "40 pt");
      SetWindowText (GetDlgItem (hwnDlg, IDC_60PT), "60 pt");
      SetWindowText (GetDlgItem (hwnDlg, IDC_DEFAULTSIZE),
		  TtaGetMessage (LIB, TMSG_UNCHANGED));
      
      SetWindowText (GetDlgItem (hwnDlg, ID_APPLY), TtaGetMessage (LIB, TMSG_APPLY));
      SetWindowText (GetDlgItem (hwnDlg, ID_DONE), TtaGetMessage (LIB, TMSG_DONE));
      
      switch (fontNum) {
      case 1:  CheckRadioButton (hwnDlg, IDC_TIMES, IDC_DEFAULTFAMILY, IDC_TIMES);
	break;
      case 2:  CheckRadioButton (hwnDlg, IDC_TIMES, IDC_DEFAULTFAMILY, IDC_HELVETICA);
	break;
      case 3:  CheckRadioButton (hwnDlg, IDC_TIMES, IDC_DEFAULTFAMILY, IDC_COURIER);
	break;
      default: CheckRadioButton (hwnDlg, IDC_TIMES, IDC_DEFAULTFAMILY,
				   IDC_DEFAULTFAMILY);
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
    case WM_CLOSE:
    case WM_DESTROY:
	  CharacterForm = NULL;
	  EndDialog (hwnDlg, ID_DONE);
	  break;
    case WM_COMMAND:
      switch (LOWORD (wParam)) {
      case ID_DONE:
	ThotCallback (NumFormPresChar, INTEGER_DATA, (char*) 0);
	CharacterForm = NULL;
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
	ThotCallback (NumMenuCharFontStyle, INTEGER_DATA, (char*) 0);
	break;
      case IDC_ITALIC:
	ThotCallback (NumMenuCharFontStyle, INTEGER_DATA, (char*) 1);
	break;
      case IDC_OBLIQUE:
	ThotCallback (NumMenuCharFontStyle, INTEGER_DATA, (char*) 2);
	break;
      case IDC_DEFAULTSTYLE:
	ThotCallback (NumMenuCharFontStyle, INTEGER_DATA, (char*) 3);
	break;
      case IDC_NORMALBOLD:
	ThotCallback (NumMenuCharFontWeight, INTEGER_DATA, (char*) 0);
	break;
      case IDC_BOLD:
	ThotCallback (NumMenuCharFontWeight, INTEGER_DATA, (char*) 1);
	break;
      case IDC_DEFAULTBOLD:
	ThotCallback (NumMenuCharFontWeight, INTEGER_DATA, (char*) 2);
	break;
      case IDC_NORMAL:
	ThotCallback (NumMenuUnderlineType, INTEGER_DATA, (char*) 0);
	break;
      case IDC_UNDERLINE:
	ThotCallback (NumMenuUnderlineType, INTEGER_DATA, (char*) 1);
	break;
	  case IDC_OVERLINE:
	ThotCallback (NumMenuUnderlineType, INTEGER_DATA, (char*) 2);
	break;
      case IDC_CROSSOUT:
	ThotCallback (NumMenuUnderlineType, INTEGER_DATA, (char*) 3);
	break;
      case IDC_DEFAULTUNDERLINE:
	ThotCallback (NumMenuUnderlineType, INTEGER_DATA, (char*) 4);
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
      
    default:
      return FALSE;
    }
  return TRUE;
}


/*-----------------------------------------------------------------------
 ------------------------------------------------------------------------*/
LRESULT CALLBACK MathEntityDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam,
									LPARAM lParam)
{
  switch (msg)
    {
    case WM_INITDIALOG:
      SetWindowText (hwnDlg, TtaGetMessage (AMAYA, AM_MEntity));
      SetWindowText (GetDlgItem (hwnDlg, IDC_ENTITY_NAME),
		  TtaGetMessage (AMAYA, AM_MATH_ENTITY_NAME));
      SetWindowText (GetDlgItem (hwnDlg, ID_CONFIRM),
		  TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
      SetWindowText (GetDlgItem (hwnDlg, IDCANCEL), TtaGetMessage (LIB, TMSG_CANCEL));
	  SetFocus (GetDlgItem (hwnDlg, IDC_EDIT_NAME));
      break;
      
    case WM_COMMAND:
      switch (LOWORD (wParam))
	{
	case ID_CONFIRM:
	  GetDlgItemText (hwnDlg, IDC_EDIT_NAME, MathEntName,
		  sizeof (MathEntName) - 1);
	  ThotCallback (BaseDialog + MathEntityText, STRING_DATA, MathEntName);
	  ThotCallback (BaseDialog + MathEntityForm, INTEGER_DATA, (char*) 1);
	  EndDialog (hwnDlg, ID_CONFIRM);
	  break;
	  
	case IDCANCEL:
	  EndDialog (hwnDlg, IDCANCEL);
	  ThotCallback (BaseDialog + MathEntityForm, INTEGER_DATA, (char*) 0);
	}
    default:
      return FALSE; 
    }
  return TRUE;
}

/*-----------------------------------------------------------------------
 ApplyClassDlgProc
 ------------------------------------------------------------------------*/
LRESULT CALLBACK ApplyClassDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam,
				    LPARAM lParam)
{
  int             index = 0;
  UINT            i = 0;

  switch (msg)
    {
    case WM_INITDIALOG:
      /* get the default GUI font */
      if (WithEdit)
	{
	  SetWindowText (hwnDlg, TtaGetMessage (AMAYA, AM_DEF_CLASS));
	  SetWindowText (GetDlgItem (hwnDlg, ID_CONFIRM),
			 TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
	}
      else
	{
	  SetWindowText (hwnDlg, TtaGetMessage (LIB, TMSG_APPLY_CLASS));
	  SetWindowText (GetDlgItem (hwnDlg, ID_CONFIRM),
			 TtaGetMessage (LIB, TMSG_APPLY));
	}
      SetWindowText (GetDlgItem (hwnDlg, ID_DONE), TtaGetMessage (LIB, TMSG_DONE));
      wndListRule = CreateWindow ("listbox", NULL,
				  WS_CHILD | WS_VISIBLE | LBS_STANDARD,
				  10, 10, 200, 120, hwnDlg, (HMENU) 1, 
				  (HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE),
				  NULL);
      
       /* set the font of the window */
      WIN_SetDialogfont (wndListRule);
      SendMessage (wndListRule, LB_RESETCONTENT, 0, 0);
      while (i < nbClass && ClassList[index] != EOS)
	{
	  SendMessage (wndListRule, LB_INSERTSTRING, i, (LPARAM) &ClassList[index]); 
	  index += strlen (&ClassList[index]) + 1;	/* entry length */
	  i++;
	}
      if (WithEdit)
	{
	  wndEditRule = CreateWindow ("EDIT", NULL,
				      WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER,
				      10, 130, 200, 30, hwnDlg, (HMENU) IDC_EDITRULE, 
				      (HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);
	  /* set the font of the window */
	  WIN_SetDialogfont (wndEditRule);
	  SetDlgItemText (hwnDlg, IDC_EDITRULE, ClassList);
	}
      break;

    case WM_COMMAND:
      if (LOWORD (wParam) == 1 && HIWORD (wParam) == LBN_SELCHANGE)
	{
	  itemIndex = SendMessage (wndListRule, LB_GETCURSEL, 0, 0);
	  itemIndex = SendMessage (wndListRule, LB_GETTEXT, itemIndex, (LPARAM) SzBuffer);
	  SetDlgItemText (hwnDlg, IDC_EDITRULE, SzBuffer);
	  if (WithEdit)
	  ThotCallback (BaseDialog + ClassSelect, STRING_DATA, SzBuffer);
	  else
	  ThotCallback (BaseDialog + AClassSelect, STRING_DATA, SzBuffer);
	}
      else if (LOWORD (wParam) == 1 && HIWORD (wParam) == LBN_DBLCLK)
	{
	  if (LB_ERR == (itemIndex = SendMessage (wndListRule, LB_GETCURSEL, 0, 0L)))
	    break;
	  itemIndex = SendMessage (wndListRule, LB_GETTEXT, itemIndex, (LPARAM) SzBuffer);
	  SetDlgItemText (hwnDlg, IDC_EDITRULE, SzBuffer);
	  if (WithEdit)
	  {
	    ThotCallback (BaseDialog + ClassSelect, STRING_DATA, SzBuffer);
	    ThotCallback (BaseDialog + ClassForm, INTEGER_DATA, (char*) 1);
	  }
	  else
	  {
	    ThotCallback (BaseDialog + AClassSelect, STRING_DATA, SzBuffer);
	    ThotCallback (BaseDialog + AClassForm, INTEGER_DATA, (char*) 1);
	  }
	  EndDialog (hwnDlg, ID_CONFIRM);
	  return FALSE;
	}
      else if (WithEdit && HIWORD (wParam) == EN_UPDATE)
	{
	  GetDlgItemText (hwnDlg, IDC_EDITRULE, SzBuffer, sizeof (SzBuffer) - 1);
	  ThotCallback (BaseDialog + ClassSelect, STRING_DATA, SzBuffer);
	}

      switch (LOWORD (wParam))
	{
	case ID_CONFIRM:
	  if (WithEdit)
	  {
	    ThotCallback (BaseDialog + ClassForm, INTEGER_DATA, (char*) 1);
	    EndDialog (hwnDlg, ID_CONFIRM);
	  }
	  else
	  {
	    ThotCallback (BaseDialog + AClassForm, INTEGER_DATA, (char*) 1);
      return FALSE;
	  }
	  break;
	  
	case ID_DONE:
	  if (WithEdit)
	    ThotCallback (BaseDialog + ClassForm, INTEGER_DATA, (char*) 0);
	  else
	    ThotCallback (BaseDialog + AClassForm, INTEGER_DATA, (char*) 0);
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
 InitConfirmDlgProc
 ------------------------------------------------------------------------*/
LRESULT CALLBACK InitConfirmDlgProc (ThotWindow hwnDlg, UINT msg,
				     WPARAM wParam, LPARAM lParam)
{
  ThotWindow      messageWnd;
  char           *ptr;

  switch (msg)
    {
    case WM_INITDIALOG:
	  InitConfirmForm = hwnDlg;
      /* get the default GUI font */
      ptr = TtaGetMessage (LIB, TMSG_LIB_CONFIRM);
      SetWindowText (hwnDlg, ptr);
      SetWindowText (GetDlgItem (hwnDlg, ID_CONFIRM), Message2);
	if (strcmp (Message2, ptr))
	  /* generate a button show */
      SetWindowText (GetDlgItem (hwnDlg, ID_MIDDLE), Message3);
	SetWindowText (GetDlgItem (hwnDlg, IDCANCEL), TtaGetMessage (LIB, TMSG_DISCARD));
      messageWnd = CreateWindow ("STATIC", Message,
				 WS_CHILD | WS_VISIBLE | SS_LEFT,
				 10, 5, 500, 15, hwnDlg, (HMENU) 99, 
				 (HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE),
				 NULL);
       /* set the font of the window */
      WIN_SetDialogfont (messageWnd);
      break; 
      
    case WM_COMMAND:
      switch (LOWORD (wParam))
	{
	case ID_CONFIRM:
      InitConfirmForm = NULL;
      EndDialog (hwnDlg, ID_CONFIRM);
	  ThotCallback (BaseDialog + ConfirmForm, INTEGER_DATA, (char*) 1);
	  break;
	case ID_MIDDLE:
      InitConfirmForm = NULL;
	  EndDialog (hwnDlg, ID_CONFIRM);
	  ThotCallback (BaseDialog + ConfirmForm, INTEGER_DATA, (char*) 2);
	  break;
	case IDCANCEL:
      InitConfirmForm = NULL;
	  EndDialog (hwnDlg, IDCANCEL);
	  ThotCallback (BaseDialog + ConfirmForm, INTEGER_DATA, (char*) 0);
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
LRESULT CALLBACK InitConfirm3LDlgProc (ThotWindow hwnDlg, UINT msg,
				       WPARAM wParam, LPARAM lParam)
{
    switch (msg)
      {
      case WM_INITDIALOG:
	SetWindowText (hwnDlg, WndTitle);
	SetWindowText (GetDlgItem (hwnDlg, ID_CONFIRM), WndTitle);
	if (WithCancel)
	  SetWindowText (GetDlgItem (hwnDlg, IDCANCEL),
	  TtaGetMessage (LIB, TMSG_CANCEL));
	if (Message)
	  SetWindowText (GetDlgItem (hwnDlg, IDC_MESSAGE1), Message);
	if (Message2)
	  SetWindowText (GetDlgItem (hwnDlg, IDC_MESSAGE2), Message2);
	if (Message3)
	  SetWindowText (GetDlgItem (hwnDlg, IDC_MESSAGE3), Message3);
	break; 
	
      case WM_COMMAND:
	switch (LOWORD (wParam))
	  {
	  case ID_CONFIRM:
	    EndDialog (hwnDlg, ID_CONFIRM);
	    if (WithCancel)
	      ThotCallback (BaseDialog + ConfirmForm, INTEGER_DATA, (char*) 1);
	    else
	      ThotCallback (BaseDialog + ConfirmForm, INTEGER_DATA, (char*) 0);
	    break;
	    
	  case IDCANCEL:
	    EndDialog (hwnDlg, IDCANCEL);
	    ThotCallback (BaseDialog + ConfirmForm, INTEGER_DATA, (char*) 0);
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
LRESULT CALLBACK ChangeFormatDlgProc (ThotWindow hwnDlg, UINT msg,
				      WPARAM wParam, LPARAM lParam)
{
  ThotBool ok;	  
  int  val;

  switch (msg)
  {
  case WM_INITDIALOG:
	   FormatForm = hwnDlg;	
	   SetWindowText (hwnDlg, TtaGetMessage (LIB, TMSG_FORMAT));
	   SetWindowText (GetDlgItem (hwnDlg, IDCALIGNGROUP),
			  TtaGetMessage (LIB, TMSG_ALIGN));
	   SetWindowText (GetDlgItem (hwnDlg, IDC_DEFAULTALIGN),
			  TtaGetMessage (LIB, TMSG_UNCHANGED));

	   SetWindowText (GetDlgItem (hwnDlg, IDC_INDENTGROUP),
			  TtaGetMessage (LIB, TMSG_INDENT));
	   SetWindowText (GetDlgItem (hwnDlg, IDC_INDENTDEFAULT),
			  TtaGetMessage (LIB, TMSG_UNCHANGED));
	   SetWindowText (GetDlgItem (hwnDlg, IDC_INDENTPT),
			  TtaGetMessage (LIB, TMSG_INDENT_PTS));

	   SetWindowText (GetDlgItem (hwnDlg, IDC_LINESPACEGROUP),
			  TtaGetMessage (LIB, TMSG_LINE_SPACING));
	   SetWindowText (GetDlgItem (hwnDlg, IDC_LINESPACINGPT),
			  TtaGetMessage (LIB, TMSG_LINE_SPACING_PTS));
	   SetWindowText (GetDlgItem (hwnDlg, IDC_SPACINGDEFAULT),
			  TtaGetMessage (LIB, TMSG_UNCHANGED));

	   SetWindowText (GetDlgItem (hwnDlg, ID_APPLY),
			  TtaGetMessage (LIB, TMSG_APPLY));
	   SetWindowText (GetDlgItem (hwnDlg, ID_DONE),
			  TtaGetMessage (LIB, TMSG_DONE));
    if (Align_num  == 0)
      CheckRadioButton (hwnDlg, IDC_LEFT, IDC_DEFAULTALIGN, IDC_LEFT);
    else if (Align_num  == 1)
      CheckRadioButton (hwnDlg, IDC_LEFT, IDC_DEFAULTALIGN, IDC_RIGHT);
    else if (Align_num  == 2)
      CheckRadioButton (hwnDlg, IDC_LEFT, IDC_DEFAULTALIGN, IDC_CENTER);
    else if (Align_num  == 3)
      CheckRadioButton (hwnDlg, IDC_LEFT, IDC_DEFAULTALIGN, IDC_JUSTIFY);
    
    if (Indent_num == 0)
      CheckRadioButton (hwnDlg, IDC_INDENT1, IDC_INDENTDEFAULT, IDC_INDENT1);
    else if (Indent_num == 1)
      CheckRadioButton (hwnDlg, IDC_INDENT1, IDC_INDENTDEFAULT, IDC_INDENT2);
    
    if (Line_spacingNum == 0)
      CheckRadioButton (hwnDlg, IDC_SSMALL, IDC_SPACINGDEFAULT, IDC_SSMALL);
    else if (Line_spacingNum == 1)
      CheckRadioButton (hwnDlg, IDC_SSMALL, IDC_SPACINGDEFAULT, IDC_SMEDIUM);
    else if (Line_spacingNum == 2)
      CheckRadioButton (hwnDlg, IDC_SSMALL, IDC_SPACINGDEFAULT, IDC_SLARGE);
    
    SetDlgItemInt (hwnDlg, IDC_INDENTPTEDIT, Indent_value, FALSE);
    SetDlgItemInt (hwnDlg, IDC_LINESPACINGEDIT, Old_lineSp, FALSE);
    break;
    
    case WM_CLOSE:
    case WM_DESTROY:
 	  FormatForm = NULL;	
	  EndDialog (hwnDlg, ID_DONE);
      break;
	case WM_COMMAND:
    if (HIWORD (wParam) == EN_UPDATE)
	{
      if (LOWORD (wParam) == IDC_INDENTPTEDIT)
	  {
	  val = GetDlgItemInt (hwnDlg, IDC_INDENTPTEDIT, &ok, TRUE);
	  if (ok)
	    ThotCallback (Num_zoneRecess, INTEGER_DATA, (char*) val);
      }
	  else if (LOWORD (wParam) == IDC_LINESPACINGEDIT)
	  {
	  val = GetDlgItemInt (hwnDlg, IDC_LINESPACINGEDIT, &ok, TRUE);
      if (ok)
	    ThotCallback (Num_zoneLineSpacing, INTEGER_DATA, (char*) val);
      }
    }
    switch (LOWORD (wParam))
	{
      /* Alignement menu */
    case ID_APPLY:
      ThotCallback (NumFormPresFormat, INTEGER_DATA, (char*) 1);
      break;

	case ID_DONE:
      ThotCallback (NumFormPresFormat, INTEGER_DATA, (char*) 0);
	  FormatForm = NULL;
      EndDialog (hwnDlg, ID_DONE);
      break;
      
    case IDC_BLEFT:
      ThotCallback (NumMenuAlignment, INTEGER_DATA, (char*) 0);
      CheckRadioButton (hwnDlg, IDC_LEFT, IDC_DEFAULTALIGN, IDC_LEFT);
      break;
      
    case IDC_LEFT:
      ThotCallback (NumMenuAlignment, INTEGER_DATA, (char*) 0);
      break;
      
    case IDC_BRIGHT:
      CheckRadioButton (hwnDlg, IDC_LEFT, IDC_DEFAULTALIGN, IDC_RIGHT);
      ThotCallback (NumMenuAlignment, INTEGER_DATA, (char*) 1);
      break;
      
    case IDC_RIGHT:
      ThotCallback (NumMenuAlignment, INTEGER_DATA, (char*) 1);
      break;
      
    case IDC_BCENTER:
      CheckRadioButton (hwnDlg, IDC_LEFT, IDC_DEFAULTALIGN, IDC_CENTER);
      ThotCallback (NumMenuAlignment, INTEGER_DATA, (char*) 2);
      break;
      
    case IDC_CENTER:
      ThotCallback (NumMenuAlignment, INTEGER_DATA, (char*) 2);
      break;
      
    case IDC_BJUSTIFY:
      CheckRadioButton (hwnDlg, IDC_LEFT, IDC_DEFAULTALIGN, IDC_JUSTIFY);
      ThotCallback (NumMenuAlignment, INTEGER_DATA, (char*) 3);
      break;
      
    case IDC_JUSTIFY:
      ThotCallback (NumMenuAlignment, INTEGER_DATA, (char*) 3);
      break;
      
    case IDC_DEFAULTALIGN:
      ThotCallback (NumMenuAlignment, INTEGER_DATA, (char*) 4);
      break;

      /* Indent Menu */ 
    case IDC_INDENT1:
      ThotCallback (NumMenuRecessSense, INTEGER_DATA, (char*) 0);
      SetDlgItemInt (hwnDlg, IDC_INDENTPTEDIT, WIN_IndentValue, FALSE);
      break;
      
    case IDC_BINDENT1:
      CheckRadioButton (hwnDlg, IDC_INDENT1, IDC_INDENTDEFAULT, IDC_INDENT1);
      ThotCallback (NumMenuRecessSense, INTEGER_DATA, (char*) 0);
      SetDlgItemInt (hwnDlg, IDC_INDENTPTEDIT, WIN_IndentValue, FALSE);
      break;
      
    case IDC_INDENT2:
      ThotCallback (NumMenuRecessSense, INTEGER_DATA, (char*) 1);
      SetDlgItemInt (hwnDlg, IDC_INDENTPTEDIT, WIN_IndentValue, FALSE);
      break;
      
    case IDC_BINDENT2:
      CheckRadioButton (hwnDlg, IDC_INDENT1, IDC_INDENTDEFAULT, IDC_INDENT2);
      ThotCallback (NumMenuRecessSense, INTEGER_DATA, (char*) 1);
      SetDlgItemInt (hwnDlg, IDC_INDENTPTEDIT, WIN_IndentValue, FALSE);
      break;
      
    case IDC_INDENTDEFAULT:
      ThotCallback (NumMenuRecessSense, INTEGER_DATA, (char*) 2);
      SetDlgItemInt (hwnDlg, IDC_INDENTPTEDIT, 0, FALSE);
      break;
      
      /* Line spacing menu */
    case IDC_SSMALL:
      ThotCallback (NumMenuLineSpacing, INTEGER_DATA, (char*) 0);
      SetDlgItemInt (hwnDlg, IDC_LINESPACINGEDIT, WIN_OldLineSp, FALSE);
      break;
      
    case IDC_BSSMALL:
      CheckRadioButton (hwnDlg, IDC_SSMALL, IDC_SPACINGDEFAULT, IDC_SSMALL);
      ThotCallback (NumMenuLineSpacing, INTEGER_DATA, (char*) 0);
      SetDlgItemInt (hwnDlg, IDC_LINESPACINGEDIT, WIN_OldLineSp, FALSE);
      break;
      
    case IDC_SMEDIUM:
      ThotCallback (NumMenuLineSpacing, INTEGER_DATA, (char*) 1);
      SetDlgItemInt (hwnDlg, IDC_LINESPACINGEDIT, WIN_OldLineSp, FALSE);
      break;
      
    case IDC_BSMEDIUM:
      CheckRadioButton (hwnDlg, IDC_SSMALL, IDC_SPACINGDEFAULT, IDC_SMEDIUM);
      ThotCallback (NumMenuLineSpacing, INTEGER_DATA, (char*) 1);
      SetDlgItemInt (hwnDlg, IDC_LINESPACINGEDIT, WIN_OldLineSp, FALSE);
      break;
      
    case IDC_SLARGE:
      ThotCallback (NumMenuLineSpacing, INTEGER_DATA, (char*) 2);
      SetDlgItemInt (hwnDlg, IDC_LINESPACINGEDIT, WIN_OldLineSp, FALSE);
      break;
      
    case IDC_BSLARGE:
      CheckRadioButton (hwnDlg, IDC_SSMALL, IDC_SPACINGDEFAULT, IDC_SLARGE);
      ThotCallback (NumMenuLineSpacing, INTEGER_DATA, (char*) 2);
      SetDlgItemInt (hwnDlg, IDC_LINESPACINGEDIT, WIN_OldLineSp, FALSE);
      break;
      
    case IDC_SPACINGDEFAULT:
      ThotCallback (NumMenuLineSpacing, INTEGER_DATA, (char*) 3);
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
LRESULT CALLBACK GreekKeyboardDlgProc (ThotWindow hwnDlg, UINT msg,
				       WPARAM wParam, LPARAM lParam)
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
      SetFocus (FrRef[ActiveFrame]);
      KbdCallbackHandler (hwnDlg, car, "\n");
      break;
    
    default: return FALSE;
    }
  return TRUE;
}

/*-----------------------------------------------------------------------
 AuthentificationDlgProc
 ------------------------------------------------------------------------*/
LRESULT CALLBACK AuthentificationDlgProc (ThotWindow hwnDlg, UINT msg,
					  WPARAM wParam, LPARAM lParam)
{
  static ThotWindow hwnNameEdit;
  static ThotWindow hwnPasswdEdit;
  /* the following var is used to set the focus on the name edit box */
  static ThotBool setFirstFocus;
  char *ptr, *label;

    switch (msg)
	{
    case WM_INITDIALOG:
	  /* initialize the dialog messages */
      SetWindowText (hwnDlg, TtaGetMessage (AMAYA, AM_GET_AUTHENTICATION));
      SetWindowText (GetDlgItem (hwnDlg, IDC_TAUTHINFO), 
		  TtaGetMessage (AMAYA, AM_GET_AUTHENTICATION_MSG));

      ptr = TtaGetMessage (AMAYA, AM_AUTHENTICATION_REALM);
      label = TtaGetMemory (((string_par1) ? strlen (string_par1) : 0)
			      + ((string_par2) ? strlen (string_par2) : 0)
			      + strlen (ptr)
			      + 20); /*a bit more than enough memory */
      if (label)
	{
	  sprintf (label, ptr,
		    ((string_par1) ? string_par1 : ""));
	  SetWindowText (GetDlgItem (hwnDlg, IDC_TAUTHREALM), label);
	  ptr = TtaGetMessage (AMAYA, AM_AUTHENTICATION_SERVER);
	  sprintf (label, ptr,
		    ((string_par2) ? string_par2 : ""));
	  SetWindowText (GetDlgItem (hwnDlg, IDC_TAUTHSERVER), label);
	  TtaFreeMemory (label);
	}
      SetWindowText (GetDlgItem (hwnDlg, IDC_TNAMEEDIT),
		  TtaGetMessage (AMAYA, AM_NAME));
      SetWindowText (GetDlgItem (hwnDlg, IDC_TPASSWDEDIT),
		  TtaGetMessage (AMAYA, AM_PASSWORD));
      SetWindowText (GetDlgItem (hwnDlg, ID_CONFIRM),
		  TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
      SetWindowText (GetDlgItem (hwnDlg, IDCANCEL), TtaGetMessage (LIB, TMSG_CANCEL));
      hwnNameEdit = GetDlgItem (hwnDlg, IDC_NAMEEDIT);
      hwnPasswdEdit = GetDlgItem (hwnDlg, IDC_PASSWDEDIT);
      SetDlgItemText (hwnDlg, IDC_PASSWDEDIT, "");
      SetDlgItemText (hwnDlg, IDC_NAMEEDIT, "");
      
      setFirstFocus = FALSE;
      break;

    case WM_COMMAND:
      if (HIWORD (wParam) == EN_UPDATE)
	  {
	    if (LOWORD (wParam) == IDC_NAMEEDIT)
		  SetFocus (hwnNameEdit);
	    else if (LOWORD (wParam) == IDC_PASSWDEDIT)
	      SetFocus (hwnPasswdEdit);
      }  
      switch (LOWORD (wParam))
	  {
      case ID_CONFIRM:
	GetDlgItemText (hwnDlg, IDC_NAMEEDIT, Answer_name, sizeof (Answer_name) + 1);
	GetDlgItemText (hwnDlg, IDC_PASSWDEDIT,
		Answer_password, sizeof (Answer_password) + 1);
	/* don't end the dialogue unless both answer fields have something */
	if (Answer_name[0] == EOS)
	  SetFocus (hwnNameEdit);
	else if (Answer_password[0] == EOS)
	  SetFocus (hwnPasswdEdit);
	else
	  {
	    UserAnswer = 1;
	    EndDialog (hwnDlg, ID_CONFIRM);
	  }
	break;
	
      case IDCANCEL:
    /* we clear the answer fields */
	Answer_name[0] = Answer_password[0] = EOS;
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
LRESULT CALLBACK BackgroundImageDlgProc (ThotWindow hwnDlg, UINT msg,
										 WPARAM wParam, LPARAM lParam)
{
    switch (msg)
      {
      case WM_INITDIALOG:
	SetDlgItemText (hwnDlg, IDC_BGLOCATION, CurrentPathName);
	CheckRadioButton (hwnDlg, IDC_REPEAT, IDC_NOREPEAT, IDC_REPEAT);
	SetWindowText (hwnDlg, TtaGetMessage (AMAYA, AM_BACKGROUND_IMAGE));
	SetWindowText (GetDlgItem (hwnDlg, IDC_OPENLOCATION),
		TtaGetMessage (AMAYA, AM_IMAGES_LOCATION));
	SetWindowText (GetDlgItem (hwnDlg, IDC_REPEATMODE),
		TtaGetMessage (AMAYA, AM_REPEAT_MODE));
	SetWindowText (GetDlgItem (hwnDlg, IDC_REPEAT), TtaGetMessage (AMAYA, AM_REPEAT));
	SetWindowText (GetDlgItem (hwnDlg, IDC_REPEATX),
		TtaGetMessage (AMAYA, AM_REPEAT_X));
	SetWindowText (GetDlgItem (hwnDlg, IDC_REPEATY),
		TtaGetMessage (AMAYA, AM_REPEAT_Y));
	SetWindowText (GetDlgItem (hwnDlg, IDC_NOREPEAT),
		TtaGetMessage (AMAYA, AM_NO_REPEAT));
	SetWindowText (GetDlgItem (hwnDlg, ID_CONFIRM),
		TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
	SetWindowText (GetDlgItem (hwnDlg, ID_CLEAR),
		TtaGetMessage (AMAYA, AM_CLEAR));
	SetWindowText (GetDlgItem (hwnDlg, IDC_BROWSE), TtaGetMessage (AMAYA, AM_BROWSE));
	SetWindowText (GetDlgItem (hwnDlg, IDCANCEL), TtaGetMessage (LIB, TMSG_CANCEL));
	repeatMode = REPEAT_XY;
	break;

      case WM_COMMAND:
	if (HIWORD (wParam) == EN_UPDATE)
	  {
	    GetDlgItemText (hwnDlg, IDC_BGLOCATION, UrlToOpen, sizeof (UrlToOpen) - 1);
	    ThotCallback (BaseImage + ImageURL, STRING_DATA, UrlToOpen);
	  }

	switch (LOWORD (wParam))
	  {
	  case IDC_REPEAT:
	    repeatMode  = REPEAT_XY;
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
	    ThotCallback (BaseImage + RepeatImage, INTEGER_DATA, (char*)repeatMode);
	    ThotCallback (BaseImage + FormBackground, INTEGER_DATA, (char*)1);
	    EndDialog (hwnDlg, ID_CONFIRM);
	    break;
	    
	  case ID_CLEAR:
	    SetDlgItemText (hwnDlg, IDC_BGLOCATION, "");
	    ThotCallback (BaseImage + RepeatImage, INTEGER_DATA, (char*)repeatMode);
	    ThotCallback (BaseImage + FormBackground, INTEGER_DATA, (char*)2);
	    break;
	    
	  case IDC_BROWSE:
	    OpenFileName.lStructSize       = sizeof (OPENFILENAME);
	    OpenFileName.hwndOwner         = hwnDlg;
	    OpenFileName.hInstance         = hInstance;
	    OpenFileName.lpstrFilter       = (LPTSTR) SzFilter;
	    OpenFileName.lpstrCustomFilter = (LPTSTR) NULL;
	    OpenFileName.nMaxCustFilter    = 0L;
	    OpenFileName.nFilterIndex      = indexImgFilter;
	    OpenFileName.lpstrFile         = (LPTSTR) TmpDocName;
	    OpenFileName.nMaxFile          = MAX_LENGTH;
	    OpenFileName.lpstrInitialDir   = (LPTSTR) DirectoryName;
	    OpenFileName.lpstrTitle        = TtaGetMessage (AMAYA, AM_BACKGROUND_IMAGE);
	    OpenFileName.nFileOffset       = 0;
	    OpenFileName.nFileExtension    = 0;
	    OpenFileName.lpstrDefExt       = "";
	    OpenFileName.lCustData         = 0;
	    OpenFileName.Flags             = OFN_SHOWHELP | OFN_HIDEREADONLY;
	    
	    if (GetOpenFileName (&OpenFileName))
		{
	      strcpy (UrlToOpen, OpenFileName.lpstrFile);
		  indexImgFilter = OpenFileName.nFilterIndex;
		}
	    
	    SetDlgItemText (hwnDlg, IDC_BGLOCATION, UrlToOpen);
	    EndDialog (hwnDlg, ID_CONFIRM);
	    ThotCallback (BaseImage + RepeatImage, INTEGER_DATA, (char*)repeatMode);
	    ThotCallback (BaseImage + FormBackground, INTEGER_DATA, (char*)1);
	    break;

	  case IDCANCEL:
	    ThotCallback (BaseImage + FormBackground, INTEGER_DATA, (char*)0);
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
LRESULT CALLBACK MakeIDDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam,
				LPARAM lParam)
{
  switch (msg)
    {
    case WM_INITDIALOG:
      MakeIDHwnd = hwnDlg;
      /* init the dialog's text */
      SetWindowText (hwnDlg, TtaGetMessage (AMAYA, ADD_REMOVE_ID));
      SetWindowText (GetDlgItem (hwnDlg, ID_CREATEID), 
		     TtaGetMessage (AMAYA, ADD_ID));
      SetWindowText (GetDlgItem (hwnDlg, ID_REMOVEID), 
		     TtaGetMessage (AMAYA, REMOVE_ID));
      SetWindowText (GetDlgItem (hwnDlg, ID_DONE), 
		     TtaGetMessage (LIB, TMSG_DONE));
      /* set up/clear the other options */
      /* elem name */
      SetDlgItemText (hwnDlg, IDC_TIDELEMNAME,
		      TtaGetMessage (AMAYA, ENTER_ELEMENT_NAME));
      SetDlgItemText (hwnDlg, IDC_IDELEMNAME, IdElemName);
      SetDlgItemText (hwnDlg, IDC_IDAPPLYTODOC,
		      TtaGetMessage (LIB, TMSG_IN_WHOLE_DOC));
      SetDlgItemText (hwnDlg, IDC_IDAPPLYTOSEL,
		      TtaGetMessage (LIB, TMSG_WITHIN_SEL));
      /* status bar */
      SetDlgItemText (hwnDlg, IDC_IDSTATUS, "");
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
	      GetDlgItemText (hwnDlg, IDC_IDELEMNAME, SzBuffer,
			      MAX_LENGTH - 1);
	      SzBuffer[MAX_LENGTH - 1] = EOS;
	      ThotCallback (BaseDialog + mElemName, STRING_DATA, SzBuffer);
	      break;
	    }
	}
      
      switch (LOWORD (wParam))
	{
	  /* radio buttons */
	case IDC_IDAPPLYTODOC:
	  ThotCallback (BaseDialog + mIdUseSelection, INTEGER_DATA, 
			(char *) 0);
	  break;

	case IDC_IDAPPLYTOSEL:
	  ThotCallback (BaseDialog + mIdUseSelection, INTEGER_DATA, 
			(char *) 1);
	  break;
	  
	  /* action buttons */
	case ID_CREATEID:
	  ThotCallback (BaseDialog + MakeIdMenu, INTEGER_DATA, (char *) 1);
	  /* update the status bar with the result of the operation */
	   SetDlgItemText (hwnDlg, IDC_IDSTATUS, IdStatus);
	  break;
	  
	case ID_REMOVEID:
	  ThotCallback (BaseDialog + MakeIdMenu, INTEGER_DATA, (char *) 2);
	  /* update the status bar with the result of the operation */
	   SetDlgItemText (hwnDlg, IDC_IDSTATUS, IdStatus);
	  break;
	  
	case IDCANCEL:
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
 ResetDocInfo
 Clears the value of the window handler for a given doc info window.
 ------------------------------------------------------------------------*/
static void ResetDocInfo (ThotWindow hwnDlg)
{
  Document doc;

  for (doc = 0; doc < DocumentTableLength ; doc++)
  {
    if (DocInfo[doc] == hwnDlg)
	{
        DocInfo[doc] = NULL;
 	    break;
	}
  }
}

/*-----------------------------------------------------------------------
 DocumentInfoDlgProc
 ------------------------------------------------------------------------*/
LRESULT CALLBACK DocumentInfoDlgProc (ThotWindow hwnDlg, UINT msg,
				      WPARAM wParam, LPARAM lParam)
{
  Document    doc;
  char     *content;
  
  switch (msg)
    {
    case WM_INITDIALOG:
	  /* copy the doc id parameter (from the global variable) */
	  doc = TmpDoc;
      DocInfo[doc] = hwnDlg;
      /* init the dialog's text */
      SetWindowText (hwnDlg, "Document Information");
      SetWindowText (GetDlgItem (hwnDlg, ID_DONE), 
		     TtaGetMessage (LIB, TMSG_DONE));

      /* set up the other fields */

      /* document URL */
      SetDlgItemText (hwnDlg, IDC_DIURL, TtaGetMessage (AMAYA, AM_HREF_VALUE));
      if (DocumentURLs[doc] != NULL)
	content = DocumentURLs[doc];
      else
	content = TtaGetMessage (AMAYA, AM_UNKNOWN);
      SetDlgItemText (hwnDlg, IDC_DIURL_VAL, content);

      /* DocumentType */
      SetDlgItemText (hwnDlg, IDC_DIDTYPE, "Document Type");
      content = DocumentTypeString (doc);
      if (!content)
	content = TtaGetMessage (AMAYA, AM_UNKNOWN);
      SetDlgItemText (hwnDlg, IDC_DIDTYPE_VAL, content);

      /* MIME type */
      SetDlgItemText (hwnDlg, IDC_DICONTENTTYPE, "MIME Type");
      if (DocumentMeta[doc] && DocumentMeta[doc]->content_type != NULL)
	content  = DocumentMeta[doc]->content_type;
      else
	content = TtaGetMessage (AMAYA, AM_UNKNOWN);
      SetDlgItemText (hwnDlg, IDC_DICONTENTTYPE_VAL, content);

      /* charset */
      SetDlgItemText (hwnDlg, IDC_DICHARSET, "Charset");
      if (DocumentMeta[doc] && DocumentMeta[doc]->charset != NULL)
	content = DocumentMeta[doc]->charset;
      else
	content = TtaGetMessage (AMAYA, AM_UNKNOWN);
      SetDlgItemText (hwnDlg, IDC_DICHARSET_VAL, content);

      /* content length */
      SetDlgItemText (hwnDlg, IDC_DICONTENTLEN, "Content Length");
      if (DocumentMeta[doc] && DocumentMeta[doc]->content_length != NULL)
	content = DocumentMeta[doc]->content_length;
      else
	content = TtaGetMessage (AMAYA, AM_UNKNOWN);
      SetDlgItemText (hwnDlg, IDC_DICONTENTLEN_VAL, content);

	  /* content location */
      SetDlgItemText (hwnDlg, IDC_DICONTENTLOC, "Content Location");
      if (DocumentMeta[doc] && DocumentMeta[doc]->full_content_location != NULL)
	     content = DocumentMeta[doc]->full_content_location;
      else
	     content = TtaGetMessage (AMAYA, AM_UNKNOWN);
      SetDlgItemText (hwnDlg, IDC_DICONTENTLOC_VAL, content);
      break;

    case WM_CLOSE:
    case WM_DESTROY:
	  ResetDocInfo (hwnDlg);
      EndDialog (hwnDlg, ID_DONE);
      break;
      
    case WM_COMMAND:
      
      switch (LOWORD (wParam))
	{
	  /* action buttons */
	case IDCANCEL:
	case ID_DONE:
	  ResetDocInfo (hwnDlg);
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
  DialogBox (hInstance, MAKEINTRESOURCE (GETALTERNATEDIALOG), NULL,
	  (DLGPROC) AltDlgProc);
}

/*-----------------------------------------------------------------------
 CreateXMLDlgWindow
 ------------------------------------------------------------------------*/
void CreateXMLDlgWindow (ThotWindow parent, int nb_item, char *buffer,
			 ThotBool withText)
{
  NbItem     = (UINT)nb_item;
  ItemList    = buffer;
  if (XMLForm)
	  /* the dialog box is already open */
	  return;
  if (NbItem == 0)
    /* no entry */
    MessageBox (parent, TtaGetMessage (LIB, TMSG_NO_ELEMENT),
		        TtaGetMessage (LIB, TMSG_EL_TYPE),
                MB_OK | MB_ICONWARNING);
  else if (withText)
    DialogBox (hInstance, MAKEINTRESOURCE (XMLDIALOG), NULL,
	       (DLGPROC) XMLDlgProc);
  else 
    DialogBox (hInstance, MAKEINTRESOURCE (XMLDIALOG1), NULL,
	       (DLGPROC) XMLDlgProc);
}

/*-----------------------------------------------------------------------
 CreateCSSDlgWindow
 ------------------------------------------------------------------------*/
void CreateCSSDlgWindow (ThotWindow parent, int nb_item, char *buffer,
			 char *title, char *msg_text)
{
  NbItem     = (UINT)nb_item;
  cssList    = buffer;
  strcpy (WndTitle, title);
  if (NbItem == 0)
    /* no entry */
    MessageBox (parent, msg_text, WndTitle, MB_OK | MB_ICONWARNING);
  else 
    DialogBox (hInstance, MAKEINTRESOURCE (CSSDIALOG), parent,
	       (DLGPROC) CSSDlgProc);
}

/*-----------------------------------------------------------------------
 CreateTitleDlgWindow
 ------------------------------------------------------------------------*/
void       CreateTitleDlgWindow (ThotWindow parent, char *title)
{  
  strcpy (UrlToOpen, title);
  ReleaseFocus = FALSE;
  isHref = FALSE;
  text[0] = 0;
  DialogBox (hInstance, MAKEINTRESOURCE (LINKDIALOG), parent,
	     (DLGPROC) TextDlgProc);
}

/*-----------------------------------------------------------------------
 CreateHRefDlgWindow
 ------------------------------------------------------------------------*/
void CreateHRefDlgWindow (ThotWindow parent, char *HRefValue,
			  int doc_select, int dir_select, int doc_type)
{
  docSelect = doc_select;
  dirSelect = dir_select;
  strcpy (TmpDocName, HRefValue);
    
  if (doc_type == docCSS)
    SzFilter = APPCSSNAMEFILTER;
  else if (doc_type == docJavascript)
    SzFilter = APPJAVASCRIPTNAMEFILTER;
  else 
    SzFilter = APPALLFILESFILTER;

  DialogBox (hInstance, MAKEINTRESOURCE (CREATEHREF), parent,
	     (DLGPROC) HRefDlgProc);
}

/*-----------------------------------------------------------------------
 CreateHelpDlgWindow
 ------------------------------------------------------------------------*/
void CreateHelpDlgWindow (ThotWindow parent, char *localname, char *msg1,
			  char *msg2)
{  
  strcpy (CurrentPathName, localname);
  strcpy (Message, msg1);
  strcpy (Message2, msg2);
  DialogBox (hInstance, MAKEINTRESOURCE (HELPDIALOG), parent,
	     (DLGPROC) HelpDlgProc);
}

/*-----------------------------------------------------------------------
 CreateMathDlgWindow
 ------------------------------------------------------------------------*/
void CreateMathDlgWindow (ThotWindow parent)
{
  if (MathPal)
    SetFocus (MathPal);
  else
    DialogBox (hInstance, MAKEINTRESOURCE (MATHDIALOG), NULL, (DLGPROC) MathDlgProc);
}

/*-----------------------------------------------------------------------
 CreatePrintDlgWindow
 ------------------------------------------------------------------------*/
void CreatePrintDlgWindow (ThotWindow parent, char *ps_dir)
{  
  gbAbort            = FALSE;
  ghwndMain          = parent;
  strcpy (CurFileToPrint, ps_dir);

  if (PrintForm)
    SetFocus (PrintForm);
  else
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
void  CreateTableDlgWindow (int num_cols, int num_rows, int t_border)
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
void  CreateMatrixDlgWindow (int num_cols, int num_rows)
{
  numCols   = num_cols;
  numRows   = num_rows;
  WithBorder = FALSE;
  DialogBox (hInstance, MAKEINTRESOURCE (MATRIXDIALOG), NULL, (DLGPROC) TableDlgProc);
}

/*-----------------------------------------------------------------------
 CreateCharsetDlgWindow
 ------------------------------------------------------------------------*/
void CreateCharsetDlgWindow (ThotWindow parent)
{
   DialogBox (hInstance, MAKEINTRESOURCE (CHARSETDIALOG), SaveAsForm, (DLGPROC) CharsetDlgProc);
}

/*-----------------------------------------------------------------------
 CreateMimeTypeDlgWindow
 ------------------------------------------------------------------------*/
void CreateMimeTypeDlgWindow (ThotWindow parent, int nb_item, char *mimetype_list)
{
   mimeType                = mimetype_list;
   NbItem                  = (UINT)nb_item;
   DialogBox (hInstance, MAKEINTRESOURCE (MIMETYPEDIALOG), SaveAsForm, (DLGPROC) MimeTypeDlgProc);
}

/*-----------------------------------------------------------------------
 CreateSaveAsDlgWindow
 ------------------------------------------------------------------------*/
void  CreateSaveAsDlgWindow (ThotWindow parent, char *path_name)
{
  HTMLFormat = (DocumentTypes[SavingDocument] != docText &&
		DocumentTypes[SavingDocument] != docCSS &&
		DocumentTypes[SavingDocument] != docMath &&
		DocumentTypes[SavingDocument] != docSVG &&
		DocumentTypes[SavingDocument] != docImage &&
		DocumentTypes[SavingDocument] != docXml);
  strcpy (CurrentPathName, path_name);
  if (HTMLFormat)
    DialogBox (hInstance, MAKEINTRESOURCE (SAVEASDIALOG), parent,
	(DLGPROC) SaveAsDlgProc);
  else if (DocumentTypes[SavingDocument] == docMath ||
	   DocumentTypes[SavingDocument] == docSVG ||
	   DocumentTypes[SavingDocument] == docXml)
    DialogBox (hInstance, MAKEINTRESOURCE (SAVEASDIALOG2), parent,
	(DLGPROC) SaveAsDlgProc);
  else
    DialogBox (hInstance, MAKEINTRESOURCE (SAVEASDIALOG1), parent,
	(DLGPROC) SaveAsDlgProc);
}

/*-----------------------------------------------------------------------
 CreateOpenDocDlgWindow
 ------------------------------------------------------------------------*/
void CreateOpenDocDlgWindow (ThotWindow parent, char *title, char *url,
			     char *docName, int doc_select, int dir_select,
			     DocumentType doc_type)
{
  docSelect = doc_select;
  dirSelect = dir_select;
  strcpy (WndTitle, title);
  strcpy (TmpDocName, docName);
  strcpy ( UrlToOpen, url);
  
  if (doc_type == docHTML)
    SzFilter = APPHTMLNAMEFILTER;
  else if (doc_type == docMath)
    SzFilter = APPMATHNAMEFILTER;
  else if (doc_type == docSVG)
    SzFilter = APPSVGNAMEFILTER;
  else if (doc_type == docCSS)
    SzFilter = APPCSSNAMEFILTER;
  else if (doc_type == docImage)
    SzFilter = APPIMAGENAMEFILTER;
  else if (doc_type == docImage)
    SzFilter = APPIMAGENAMEFILTER;
  else if (doc_type == docLibrary)
	SzFilter = APPLIBRARYNAMEFILTER;
  else 
    SzFilter = APPFILENAMEFILTER;

  DialogBox (hInstance, MAKEINTRESOURCE (OPENDOCDIALOG), parent,
	  (DLGPROC) OpenDocDlgProc);
}

/*-----------------------------------------------------------------------
 CreateOPenImgDlgWindow
 The parameter getAlt is tRUE when the ALT attribute is required.
 ------------------------------------------------------------------------*/
void CreateOpenImgDlgWindow (ThotWindow parent, char *imgName, int doc_select,
			     int dir_select, int doc_type, ThotBool getAlt)
{  
  docSelect = doc_select;
  dirSelect = dir_select;
  strcpy ( UrlToOpen, imgName);    
  if (doc_type == docImage)
    SzFilter = APPIMAGENAMEFILTER;
  else 
    SzFilter = APPALLFILESFILTER;
  if (getAlt)
    {
      RequiredAttr = TRUE;
      strcpy (AltText, ImgAlt);
    }
  else
    RequiredAttr = FALSE;
  DialogBox (hInstance, MAKEINTRESOURCE (OPENIMAGEDIALOG), parent,
	  (DLGPROC) OpenImgDlgProc);
}

/*-----------------------------------------------------------------------
 CreateGraphicsDlgWindow
 ------------------------------------------------------------------------*/
void  CreateGraphicsDlgWindow (ThotWindow frame)
{
  if (GraphPal)
    SetFocus (GraphPal);
  else
   DialogBox (hInstance, MAKEINTRESOURCE (GRAPHICSDIALOG), NULL,
   (DLGPROC) GraphicsDlgProc);
}

/*-----------------------------------------------------------------------
 CreateSaveListDlgWindow
 ------------------------------------------------------------------------*/
void CreateSaveListDlgWindow (ThotWindow parent, int nb_item, char *save_list)
{  
  NbItem      = (UINT)nb_item;
  SavList    = save_list;
  DialogBox (hInstance, MAKEINTRESOURCE (SAVELISTDIALOG), parent,
	  (DLGPROC) SaveListDlgProc);
}

/*-----------------------------------------------------------------------
 CreateCloseDocDlgWindow
 ------------------------------------------------------------------------*/
void CreateCloseDocDlgWindow (ThotWindow parent, char *msg)
{  
  strcpy (Message, msg);
  DialogBox (hInstance, MAKEINTRESOURCE (CLOSEDOCDIALOG), parent,
	  (DLGPROC) CloseDocDlgProc);
}

/*-----------------------------------------------------------------------
 CreateLanguageDlgWindow
 ------------------------------------------------------------------------*/
void CreateLanguageDlgWindow (ThotWindow parent, int nb_item,
			      char *lang_list, char *msg, int lang_value)
{
  if (LangForm)
	  EndDialog (LangForm, ID_DONE); 

   strcpy (Message2, msg);
   Num_lang = lang_value;
   if (lang_value >= 0)
	   /* there is a selected language */
	   strcpy (WinCurLang, TtaGetLanguageName (lang_value));
   else
	   WinCurLang[0] = EOS;
   ItemList = lang_list;
   NbItem = (UINT)nb_item;
   DialogBox (hInstance, MAKEINTRESOURCE (LANGUAGEDIALOG), NULL,
	   (DLGPROC) LanguageDlgProc);
}

/*-----------------------------------------------------------------------
 CreateCharacterDlgWindow
 ------------------------------------------------------------------------*/
void CreateCharacterDlgWindow (ThotWindow parent, int font_num, int font_style,
			       int font_weight, int font_underline, int font_size)
{  
    fontNum       = font_num;
    fontStyle     = font_style;
    fontWeight    = font_weight;
    fontUnderline = font_underline;
    fontSize      = font_size;

  if (CharacterForm)
    SetFocus (CharacterForm);
  else
	DialogBox (hInstance, MAKEINTRESOURCE (CHARACTERSDIALOG), NULL,
	(DLGPROC) CharacterDlgProc);
}

/*-----------------------------------------------------------------------
 CreateAttributeDlgWindow
 ------------------------------------------------------------------------*/
void CreateAttributeDlgWindow (char *title, int curr_val, int nb_items,
			       char *item_list, ThotBool required) 
{
  /* destroy the precent attribute menu */
  if (AttrForm)
    {
      EndDialog (AttrForm, ID_DONE);
      AttrForm = NULL;
    }
  strcpy (AttDlgTitle, title);
  ItemList = item_list;
  currAttrVal = curr_val;
  attDlgNbItems = nb_items;
  RequiredAttr = required;
  switch (attDlgNbItems)
    {
    case 2:
      DialogBox (hInstance, MAKEINTRESOURCE (ATTR2ITEMSDIALOG), NULL, 
		 (DLGPROC) AttrItemsDlgProc);
      break;
    case 3:
      DialogBox (hInstance, MAKEINTRESOURCE (ATTR3ITEMSDIALOG), NULL,
		 (DLGPROC) AttrItemsDlgProc);
      break;
    case 4:
      DialogBox (hInstance, MAKEINTRESOURCE (ATTR4ITEMSDIALOG), NULL,
		 (DLGPROC) AttrItemsDlgProc);
      break;
    case 5:
      DialogBox (hInstance, MAKEINTRESOURCE (ATTR5ITEMSDIALOG), NULL,
		 (DLGPROC) AttrItemsDlgProc);
      break;
    case 6:
	  DialogBox (hInstance, MAKEINTRESOURCE (ATTR6ITEMSDIALOG), NULL,
		     (DLGPROC) AttrItemsDlgProc);
      break;
    case 7:
	  DialogBox (hInstance, MAKEINTRESOURCE (ATTR7ITEMSDIALOG), NULL,
		     (DLGPROC) AttrItemsDlgProc);
      break;
    case 8:
	  DialogBox (hInstance, MAKEINTRESOURCE (ATTR8ITEMSDIALOG), NULL,
		     (DLGPROC) AttrItemsDlgProc);
      break;
    case 9:
	  DialogBox (hInstance, MAKEINTRESOURCE (ATTR9ITEMSDIALOG), NULL,
		     (DLGPROC) AttrItemsDlgProc);
      break;
    default:
      break;
    } 
}

/*-----------------------------------------------------------------------
 CreateMCHARDlgWindow
 ------------------------------------------------------------------------*/
void CreateMCHARDlgWindow (ThotWindow parent, char *math_entity_name) 
{
  strcpy (MathEntName, math_entity_name);
  DialogBox (hInstance, MAKEINTRESOURCE (MATH_ENTITY_DLG), NULL,
	     (DLGPROC) MathEntityDlgProc);
  strcpy (math_entity_name, MathEntName);
}

/*-----------------------------------------------------------------------
 CreateRuleDlgWindow
 ------------------------------------------------------------------------*/
void CreateRuleDlgWindow (ThotWindow parent, int nb_class, char *class_list)
{  
  nbClass     = (UINT)nb_class;
  ClassList   = class_list;
  WithEdit = TRUE;
  DialogBox (hInstance, MAKEINTRESOURCE (CREATERULEDIALOG), parent, (DLGPROC) ApplyClassDlgProc);
}

/*-----------------------------------------------------------------------
 CreateApplyClassDlgWindow
 ------------------------------------------------------------------------*/
void CreateApplyClassDlgWindow (ThotWindow parent, int nb_class, char *class_list)
{  
  nbClass     = (UINT)nb_class;
  ClassList   = class_list;
  WithEdit = FALSE;
  DialogBox (hInstance, MAKEINTRESOURCE (APPLYCLASSDIALOG), NULL,
	     (DLGPROC) ApplyClassDlgProc);
}

/*-----------------------------------------------------------------------
 CreateInitConfirmDlgWindow
 ------------------------------------------------------------------------*/
void CreateInitConfirmDlgWindow (ThotWindow parent, char *extrabutton,
								 char *confirmbutton, char *label)
{
  strcpy (Message, label);

  if (InitConfirmForm)
    SetFocus (InitConfirmForm);
  else
  {
    if (confirmbutton && confirmbutton[0] != EOS)
      strcpy (Message2, confirmbutton);
    else
      strcpy (Message2, TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
    if (extrabutton && extrabutton[0] != EOS)
    {
      /* a message with 3 buttons */
      strcpy (Message3, extrabutton); 
      DialogBox (hInstance, MAKEINTRESOURCE (INITCONFIRMDIALOG1), parent,
                 (DLGPROC) InitConfirmDlgProc);
    }
    else
      DialogBox (hInstance, MAKEINTRESOURCE (INITCONFIRMDIALOG), parent,
                 (DLGPROC) InitConfirmDlgProc);
  }
}

/*-----------------------------------------------------------------------
 CreateInitConfirm3LDlgWindow
 ------------------------------------------------------------------------*/
void CreateInitConfirm3LDlgWindow (ThotWindow parent, char *title,
				   char *msg, char *msg2, char *msg3,
				   ThotBool withCancel)
{
  strcpy (Message, msg);
  if (msg2 && *msg2 != EOS)
    strcpy (Message2, msg2);
  else
    Message2[0] = EOS;
  if (msg3 && *msg3 != EOS)
    strcpy (Message3, msg3);
  else
    Message3[0] = EOS;
  strcpy (WndTitle, title);
  /* register if the cancel button has to be generated */
  WithCancel = withCancel;
  if (withCancel)
    DialogBox (hInstance, MAKEINTRESOURCE (INITCONFIRM3LDIALOG), parent,
	       (DLGPROC) InitConfirm3LDlgProc);
  else
	DialogBox (hInstance, MAKEINTRESOURCE (INITCONFIRM3LDIALOG1), parent,
		   (DLGPROC) InitConfirm3LDlgProc);
}

/*-----------------------------------------------------------------------
 CreateChangeFormatDlgWindow
 ------------------------------------------------------------------------*/
void CreateChangeFormatDlgWindow (int num_zone_recess, int num_zone_line_spacing,
				  int align_num, int indent_value, int indent_num,
				  int old_lineSp, int line_spacingNum)
{  
  Num_zoneRecess      = num_zone_recess;
  Num_zoneLineSpacing = num_zone_line_spacing;
  Align_num           = align_num; 
  Indent_value        = indent_value;
  Indent_num          = indent_num;
  Old_lineSp          = old_lineSp;
  Line_spacingNum     = line_spacingNum;
  
  if (FormatForm)
    SetFocus (FormatForm);
  else
	DialogBox (hInstance, MAKEINTRESOURCE (FORMATDIALOG), NULL, (DLGPROC) ChangeFormatDlgProc);
}

/*-----------------------------------------------------------------------
 CreateGreekKeyboardDlgWindow
 ------------------------------------------------------------------------*/
void CreateGreekKeyboardDlgWindow (ThotWindow parent)
{  
  if (GreekPal)
    SetFocus (GreekPal);
  else
    DialogBox (hInstance, MAKEINTRESOURCE (GALPHABETDIALOG), NULL, (DLGPROC) GreekKeyboardDlgProc);
}

/*-----------------------------------------------------------------------
 CreateAuthentificationDlgWindow
 ------------------------------------------------------------------------*/
void CreateAuthenticationDlgWindow (ThotWindow parent, const char *realm,
				    char *server)
{  
  string_par1 = (char *) realm;
  string_par2 = server;

  DialogBox (hInstance, MAKEINTRESOURCE (AUTHENTIFICATIONDIALOG), parent, (DLGPROC) AuthentificationDlgProc);
}

/*-----------------------------------------------------------------------
 CreateBackgroundImageDlgWindow
 ------------------------------------------------------------------------*/
void CreateBackgroundImageDlgWindow (ThotWindow parent, char *image_location)
{
  SzFilter = APPIMAGENAMEFILTER;
  strcpy (CurrentPathName, image_location);
  DialogBox (hInstance, MAKEINTRESOURCE (BGIMAGEDIALOG), parent, (DLGPROC) BackgroundImageDlgProc);
}

/*-----------------------------------------------------------------------
 CreateMakeIDDlgWindow
 ------------------------------------------------------------------------*/
void CreateMakeIDDlgWindow (ThotWindow parent)
{
  /* we only use one such dialog at the time */
  if (MakeIDHwnd)
    EndDialog (MakeIDHwnd, ID_DONE);
  DialogBox (hInstance, MAKEINTRESOURCE (MAKEIDMENU), NULL, (DLGPROC) MakeIDDlgProc);
}

/*-----------------------------------------------------------------------
 CreateAuthentificationDlgWindow
 ------------------------------------------------------------------------*/
void CreateDocumentInfoDlgWindow (ThotWindow parent, const Document doc)
{  
  if (DocInfo[doc])
    SetFocus (DocInfo[doc]);
  else
    {
        /* copy the value so that we can find it in the callback */
      TmpDoc = doc;
      DialogBox (hInstance, MAKEINTRESOURCE (DOCINFOMENU), NULL, (DLGPROC) DocumentInfoDlgProc);
    }
}
#endif /* _WINGUI */
