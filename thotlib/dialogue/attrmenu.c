/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Warning:
 * This module is part of the Thot library, which was originally
 * developed in French. That's why some comments are still in
 * French, but their translation is in progress and the full module
 * will be available in English in the next release.
 * 
 */

/*
 * User interface for attributes
 *
 * Authors: I. Vatton (INRIA)
 *          R. Guetari (W3C/INRIA) - Unicode and Windows version
 *
 */

#include "thot_gui.h"
#include "thot_sys.h"
#include "dialog.h"
#include "constmedia.h"
#include "typemedia.h"
#include "libmsg.h"
#include "message.h"
#include "constmenu.h"
#include "appdialogue.h"
#ifdef _WINDOWS
#include "winsys.h"
#endif /* _WINDOWS */

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "select_tv.h"
#include "edit_tv.h"
#include "frame_tv.h"
#include "appdialogue_tv.h"

/* flags to show the existence of the TtAttribute forms*/
static ThotBool     AttrFormExists = FALSE;
static ThotBool     MandatoryAttrFormExists = FALSE;
#ifdef _WINDOWS
static CHAR_T       WIN_Lab[1024];
static int          WIN_nbItem;
static CHAR_T       WIN_title[MAX_NAME_LENGTH + 2];

extern WNDPROC      lpfnTextZoneWndProc ;

CHAR_T              WIN_buffMenu[MAX_TXT_LEN];

#ifdef __STDC__
extern LRESULT CALLBACK textZoneProc (HWND, UINT, WPARAM, LPARAM);
#else  /* !__STDC__ */
extern LRESULT CALLBACK textZoneProc ();
#endif /* !__STDC_ */
#endif /* _WINDOWS */


#define LgMaxAttrText 500
/* the menu attributes */
static PtrSSchema   AttrStruct[MAX_MENU * 2];
static int          AttrNumber[MAX_MENU * 2];
static ThotBool     AttrOblig[MAX_MENU * 2];
static ThotBool     AttrEvent[MAX_MENU* 2];
static CHAR_T       TextAttrValue[LgMaxAttrText];
static PtrSSchema   SchCurrentAttr = NULL;
static int          EventMenu[MAX_FRAME];
static int          NumCurrentAttr = 0;
static int          CurrentAttr;
/* return value of the input form */
static int          NumAttrValue;
/* main menu of attributes */
static int          ActiveAttr[MAX_MENU * 2];
/* submenu of event attributes */
static int          AttrEventNumber[MAX_MENU];
static int          ActiveEventAttr[MAX_MENU];


/* required attributs context */
static PtrAttribute PtrReqAttr;

#ifdef _WINDOWS
#define ID_CONFIRM   1000
#define ID_DONE      1001
#define ID_APPLY     1002
#define ID_DELETE    1003
#define ID_EDITVALUE 1004

static HWND         hwnEdit;
static TtAttribute *WIN_pAttr1;
static BOOL         wndRegistered;
static BOOL         wndSheetRegistered;
static BOOL         wndNumAttrRegistered;
static BOOL         WIN_AtNumAttr  = FALSE;
static BOOL         WIN_AtTextAttr = FALSE;
static BOOL         WIN_AtEnumAttr = FALSE;
static BOOL         isForm         = FALSE;
static PtrAttribute WIN_currAttr;
static CHAR_T       formRange[100];
static CHAR_T       Attr_text[LgMaxAttrText];
static int          formValue;
static int          nbDlgItems;
static int          WIN_Language;

extern HINSTANCE hInstance;
extern LPCTSTR   iconID;
extern UINT      subMenuID[MAX_FRAME];
#ifdef __STDC__
extern BOOL RegisterWin95 (CONST WNDCLASS*);

LRESULT CALLBACK InitFormDialogWndProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK InitSheetDialogWndProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK InitNumAttrDialogWndProc (HWND, UINT, WPARAM, LPARAM);
#else  /* __STDC__ */
extern BOOL RegisterWin95 ();

LRESULT CALLBACK InitFormDialogWndProc ();
LRESULT CALLBACK InitSheetDialogWndProc ();
LRESULT CALLBACK InitNumAttrDialogWndProc ();
#endif /* __STDC__ */
#endif /* _WINDOWS */

#include "appdialogue_f.h"
#include "appli_f.h"
#include "applicationapi_f.h"
#include "attributes_f.h"
#include "attrpresent_f.h"
#include "actions_f.h"
#include "callback_f.h"
#include "changeabsbox_f.h"
#include "content_f.h"
#include "displayview_f.h"
#include "exceptions_f.h"
#include "language_f.h"
#include "memory_f.h"
#include "selectmenu_f.h"
#include "structcommands_f.h"
#include "structcreation_f.h"
#include "structmodif_f.h"
#include "structselect_f.h"
#include "structschema_f.h"
#include "tree_f.h"
#include "ustring_f.h"


#ifdef _WINDOWS
#include "wininclude.h"
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
  InitFormLangue
  initializes a form for capturing the values of the Language attribute.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         InitFormLanguage (Document doc, View view, PtrElement firstSel, PtrAttribute currAttr)
#else  /* __STDC__ */
static void         InitFormLanguage (doc, view, firstSel, currAttr)
Document            doc;
View                view;
PtrElement          firstSel;
PtrAttribute        currAttr;

#endif /* __STDC__ */
{
#ifndef _WINDOWS
   CHAR_T              bufMenu[MAX_TXT_LEN];
   int                 i;
#endif /* _WINDOWS */
   CHAR_T              string[MAX_TXT_LEN];
   CHAR_T*             ptr;
   Language            language;
   CHAR_T              languageValue[MAX_TXT_LEN];
   CHAR_T              Lab[200];
   PtrAttribute        pHeritAttr;
   PtrElement          pElAttr;
   int                 defItem, nbItem, nbLanguages, firstLanguage, length;

   /* c'est l'attribut Langue, on initialise le formulaire Langue */
   languageValue[0] = WC_EOS;
   if (currAttr != NULL && currAttr->AeAttrText != NULL)
     ustrncpy (languageValue, currAttr->AeAttrText->BuContent,MAX_NAME_LENGTH);

   /* cree le formulaire avec les deux boutons Appliquer et Supprimer */
#ifdef _WINDOWS
   ptr = &WIN_buffMenu[0];
#else  /* _WINDOWS */
   ustrcpy (bufMenu, TtaGetMessage (LIB, TMSG_APPLY));
   i = ustrlen (bufMenu) + 1;
   ustrcpy (&bufMenu[i], TtaGetMessage (LIB, TMSG_DEL_ATTR));
   TtaNewSheet (NumFormLanguage, TtaGetViewFrame (doc, view),
		TtaGetMessage (LIB, TMSG_LANGUAGE), 2, 
		bufMenu, FALSE, 2, 'L', D_DONE);
   /* construit le selecteur des Langues */
   ptr = &bufMenu[0];
#endif /* _WINDOWS */

   nbItem = 0;
   defItem = -1;
   nbLanguages = TtaGetNumberOfLanguages ();
   firstLanguage = TtaGetFirstUserLanguage ();
   for (language = firstLanguage; language < nbLanguages; language++)
     {
       ustrcpy (string, TtaGetLanguageName (language));
       length = ustrlen (string);
       if (length > 0)
	 {
	   if (defItem < 0 && languageValue[0] != WC_EOS)
	     if (ustrcasecmp (TtaGetLanguageCode(language),languageValue) == 0)
	       {
		 defItem = nbItem;
		 ustrcpy (languageValue, string);
	       }
	   nbItem++;
	   ustrcpy (ptr, string);
	   ptr += length + 1;
	 }
     }

   if (nbItem == 0)
     {
      /* pas de langue definie, on cree une simple zone de saisie de texte */
#ifndef _WINDOWS
      TtaNewTextForm (NumSelectLanguage, NumFormLanguage,
		      TtaGetMessage (LIB, TMSG_LANGUAGE), 30, 1, FALSE);
      TtaSetTextForm (NumFormLanguage, languageValue);
#endif /* !_WINDOWS */
     }
   else
     {
#ifndef _WINDOWS 
       /* on cree un selecteur */
       if (nbItem >= 6)
	 length = 6;
       else
	 length = nbItem;
       TtaNewSelector (NumSelectLanguage, NumFormLanguage,
		       TtaGetMessage (LIB, TMSG_LANG_OF_EL), nbItem, bufMenu,
		       length, NULL, TRUE, TRUE);
#endif /* !_WINDOWS */
       if (languageValue[0] == EOS || defItem < 0)
	 {
#ifndef _WINDOWS
	   TtaSetSelector (NumSelectLanguage, -1, NULL);
#endif /* !_WINDOWS */
	   /* cherche la valeur heritee de l'attribut Langue */
	   ustrcpy (Lab, TtaGetMessage (LIB, TMSG_INHERITED_LANG));
	   pHeritAttr = GetTypedAttrAncestor (firstSel, 1, NULL, &pElAttr);
	   if (pHeritAttr != NULL)
	     if (pHeritAttr->AeAttrText != NULL)
	       {
		 /* the attribute value is a RFC-1766 code. Convert it into */
		 /* a language name */
		 language = TtaGetLanguageIdFromName (pHeritAttr->AeAttrText->BuContent);
		 ustrcat (Lab, TtaGetLanguageName(language));
	       }
	 }
       else
	 /* initialise le selecteur sur l'entree correspondante a la valeur */
	 /* courante de l'attribut langue. */
	 Lab[0] = EOS;
     }

#ifndef _WINDOWS 
   TtaNewLabel (NumLabelHeritedLanguage, NumFormLanguage, Lab);
   /* affiche le formulaire */
   TtaShowDialogue (NumFormLanguage, TRUE);
#else  /* _WINDOWS */
   usprintf (WIN_Lab, TEXT("%s"), Lab);
   WIN_nbItem = nbItem; 
   WIN_Language = language;
#endif /* _WINDOWS */
}

#ifdef _WINDOWS
/*----------------------------------------------------------------------
  WIN_InitFormDialog
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static BOOL WIN_InitFormDialog (HWND parent, STRING title)
#else  /* __STDC__ */
static BOOL WIN_InitFormDialog (parent, title)
HWND  parent;
STRING title	;
#endif /* __STDC__ */
{
   WNDCLASS    wndFormClass;
   STRING      szAppName; 
   HWND        hwnFromDialog;
   MSG         msg;
   int         frame;

   szAppName = TEXT("FormClass");
   if (!wndRegistered)
     {
       wndRegistered = TRUE;
       wndFormClass.style         = CS_HREDRAW | CS_VREDRAW;
       wndFormClass.lpfnWndProc   = InitFormDialogWndProc;
       wndFormClass.cbClsExtra    = 0;
       wndFormClass.cbWndExtra    = 0;
       wndFormClass.hInstance     = hInstance;
       wndFormClass.hIcon         = LoadIcon (NULL, IDI_APPLICATION);
       wndFormClass.hCursor       = LoadCursor (NULL, IDC_ARROW);
       wndFormClass.hbrBackground = (HBRUSH) GetStockObject (LTGRAY_BRUSH);
       wndFormClass.lpszClassName = szAppName;
       wndFormClass.lpszMenuName  = NULL;
       
       if (IS_WIN95)
	 {
	   if (!RegisterWin95 (&wndFormClass))
	     return (FALSE);
	 }
       else if (!RegisterClass (&wndFormClass))
	 return (FALSE);
     }

   hwnFromDialog = CreateWindow (szAppName, title,
                                 DS_MODALFRAME | WS_POPUP | 
                                 WS_VISIBLE | WS_CAPTION | WS_SYSMENU,
                                 ClickX, ClickY,
                                 340, 100,
                                 parent, NULL, hInstance, NULL);

   ShowWindow (hwnFromDialog, SW_SHOWNORMAL);
   UpdateWindow (hwnFromDialog);

   while (GetMessage (&msg, NULL, 0, 0))
     {
       frame = GetFrameNumber (msg.hwnd);
       TranslateMessage (&msg);
       DispatchMessage (&msg);
     }
   return TRUE;
}

/*----------------------------------------------------------------------
 InitFormDialogWndProc
  ----------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK InitFormDialogWndProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
#else  /* __STDC__ */
LRESULT CALLBACK InitFormDialogWndProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
#endif /* __STDC__ */
{
  HWND          hwnTitle;
  HWND          confirmButton;
  HWND          doneButton;
  int           i;
  int           txtLength;
  
  switch (iMsg)
    {
    case WM_CREATE:
      if (WIN_currAttr)
	usprintf (Attr_text, WIN_currAttr->AeAttrText->BuContent);
      else
	Attr_text[0] = EOS;

      /* Create static window for the title */
      hwnTitle = CreateWindow (TEXT("STATIC"), WIN_pAttr1->AttrName, 
			       WS_CHILD | WS_VISIBLE | SS_LEFT,
			       10, 5, 100, 15, hwnd, (HMENU) 99, 
			       ((LPCREATESTRUCT) lParam)->hInstance, NULL); 
      
      /* Create Edit Window autoscrolled */
      hwnEdit = CreateWindow (TEXT("EDIT"), Attr_text, 
			      WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT |
			         ES_AUTOHSCROLL,
			      10, 25, 320, 20, hwnd, (HMENU) 1,
			      ((LPCREATESTRUCT) lParam)->hInstance, NULL);

      if (lpfnTextZoneWndProc == (WNDPROC) 0)
	lpfnTextZoneWndProc = (WNDPROC) SetWindowLong (hwnEdit, GWL_WNDPROC,
						       (DWORD) textZoneProc);
      else
	SetWindowLong (hwnEdit, GWL_WNDPROC, (DWORD) textZoneProc);
      
      /* Create Confirm button */
      confirmButton = CreateWindow (TEXT("BUTTON"),
				    TtaGetMessage (LIB, TMSG_LIB_CONFIRM), 
				    WS_CHILD | BS_DEFPUSHBUTTON | WS_VISIBLE,
				    65, 50, 100, 20, hwnd, 
				    (HMENU) ID_CONFIRM,
				    ((LPCREATESTRUCT)lParam)->hInstance, NULL);
      
      /* Create Done Button */
      doneButton = CreateWindow (TEXT("BUTTON"), TtaGetMessage(LIB, TMSG_DONE),
				 WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE,
				 185, 50, 100, 20, hwnd, 
				 (HMENU) ID_DONE,
				 ((LPCREATESTRUCT) lParam)->hInstance, NULL);
      
      SetFocus (hwnEdit);
      break;
      
    case WM_DESTROY :
      PostQuitMessage (0);
      break;
      
    case WM_ENTER:
      txtLength = GetWindowTextLength (hwnEdit);
      if (txtLength >= LgMaxAttrText)
	txtLength = LgMaxAttrText - 1;
      GetWindowText (hwnEdit, Attr_text, txtLength + 1);
      i = 0;
      while (i < txtLength && Attr_text[i] != __CR__)
	i++;
      if (i < txtLength)
	Attr_text[i] = EOS;
      ThotCallback (NumMenuAttrTextNeeded, STRING_DATA, Attr_text);
      ThotCallback (NumMenuAttrRequired, INTEGER_DATA, (STRING) 1);
      DestroyWindow (hwnd);
      break;
      
    case WM_COMMAND:
      switch (LOWORD (wParam))
	{
	case ID_CONFIRM:
	  txtLength = GetWindowTextLength (hwnEdit);
	  if (txtLength >= LgMaxAttrText)
	    txtLength = LgMaxAttrText - 1;
	  GetWindowText (hwnEdit, Attr_text, txtLength + 1);
	  i = 0;
	  while (i < txtLength && Attr_text[i] != __CR__)
	    i++;
	  if (i < txtLength)
	    Attr_text[i] = EOS;
	  ThotCallback (NumMenuAttrTextNeeded, STRING_DATA, Attr_text);
	  ThotCallback (NumMenuAttrRequired, INTEGER_DATA, (STRING) 1);
	  DestroyWindow (hwnd);
	  break;
	  
	case ID_DONE:
	  ThotCallback (NumMenuAttrRequired, INTEGER_DATA, (STRING) 0);
	  DestroyWindow (hwnd);
	  /* Traitement ID_DONE */
	  break;
	}
      break;
    }
  return DefWindowProc (hwnd, iMsg, wParam, lParam);
}

/*----------------------------------------------------------------------
  WIN_InitSheetDialog
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static BOOL WIN_InitSheetDialog (HWND parent, STRING title)
#else  /* __STDC__ */
static BOOL WIN_InitSheetDialog (parent, title)
HWND  parent;
STRING title	;
#endif /* __STDC__ */
{
   WNDCLASSEX    wndSheetClass;
   STRING        szAppName;
   HWND          hwnSheetDialog;
   MSG           msg;
   int           frame;

   szAppName = TEXT("SheetClass");
   if (!wndSheetRegistered)
     {
       wndSheetRegistered = TRUE;
       wndSheetClass.style         = CS_HREDRAW | CS_VREDRAW;
       wndSheetClass.lpfnWndProc   = InitSheetDialogWndProc;
       wndSheetClass.cbClsExtra    = 0;
       wndSheetClass.cbWndExtra    = 0;
       wndSheetClass.hInstance     = hInstance;
       wndSheetClass.hIcon         = LoadIcon (NULL, IDI_APPLICATION);
       wndSheetClass.hCursor       = LoadCursor (NULL, IDC_ARROW);
       wndSheetClass.hbrBackground = (HBRUSH) GetStockObject (LTGRAY_BRUSH);
       wndSheetClass.lpszMenuName  = NULL;
       wndSheetClass.lpszClassName = szAppName;
       wndSheetClass.cbSize        = sizeof(WNDCLASSEX);
       wndSheetClass.hIconSm       = LoadIcon (hInstance, iconID);
       
       if (!RegisterClassEx (&wndSheetClass))
         return (FALSE);
     }
   
   hwnSheetDialog = CreateWindow (szAppName, title,
				  DS_MODALFRAME | WS_POPUP | WS_VISIBLE |
				      WS_CAPTION | WS_SYSMENU | WS_TABSTOP,
				  ClickX, ClickY,
				  340, 100,
				  parent, NULL, hInstance, NULL);
   
   ShowWindow (hwnSheetDialog, SW_SHOWNORMAL);
   UpdateWindow (hwnSheetDialog);
   
   while (GetMessage (&msg, NULL, 0, 0))
     {
       frame = GetFrameNumber (msg.hwnd);
       TranslateMessage (&msg);
       DispatchMessage (&msg);
     }
   return TRUE;
}

/*----------------------------------------------------------------------
  InitSheetDialogWndProc
  ----------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK InitSheetDialogWndProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
#else  /* __STDC__ */
LRESULT CALLBACK InitSheetDialogWndProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
#endif /* __STDC__ */
{
  HWND          hwnTitle;
  HWND          applyButton;
  HWND          deleteButton;
  HWND          doneButton;
  int           i;
  int           txtLength;

  switch (iMsg)
    {
    case WM_CREATE:
      if (WIN_currAttr) 
	usprintf (Attr_text, TEXT("%s"), WIN_currAttr->AeAttrText->BuContent);
      else
	Attr_text[0] = EOS;

      /* Create static window for the title */
      hwnTitle = CreateWindow (TEXT("STATIC"), WIN_pAttr1->AttrName, 
			       WS_CHILD | WS_VISIBLE | SS_LEFT,
			       10, 5, 100, 15, hwnd, (HMENU) 99, 
			       ((LPCREATESTRUCT) lParam)->hInstance, NULL); 
      
      /* Create Edit Window autoscrolled */
      hwnEdit = CreateWindow (TEXT("EDIT"), Attr_text, 
			      WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT |
			        ES_AUTOHSCROLL | WS_TABSTOP,
			      10, 25, 310, 20, hwnd, (HMENU) 1,
			      ((LPCREATESTRUCT) lParam)->hInstance, NULL);
				
      if (lpfnTextZoneWndProc == (WNDPROC) 0)
	lpfnTextZoneWndProc = (WNDPROC) SetWindowLong (hwnEdit, GWL_WNDPROC,
						       (DWORD) textZoneProc);
      else
	SetWindowLong (hwnEdit, GWL_WNDPROC, (DWORD) textZoneProc);
      
      /* Create Apply button */
      applyButton = CreateWindow (TEXT("BUTTON"),
				  TtaGetMessage (LIB, TMSG_APPLY), 
				  WS_CHILD | BS_DEFPUSHBUTTON | WS_VISIBLE,
				  10, 50, 80, 20, hwnd, (HMENU) ID_APPLY,
				  ((LPCREATESTRUCT) lParam)->hInstance, NULL);
      
      /* Create Delete Button */
      deleteButton = CreateWindow (TEXT("BUTTON"),
				   TtaGetMessage (LIB, TMSG_DEL_ATTR), 
				   WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE,
				   100, 50, 120, 20, hwnd, (HMENU) ID_DELETE,
				   ((LPCREATESTRUCT) lParam)->hInstance, NULL);
      
      /* Create Done Button */
      doneButton = CreateWindow (TEXT("BUTTON"),
				 TtaGetMessage (LIB, TMSG_DONE), 
				 WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE,
				 230, 50, 80, 20, hwnd, (HMENU) ID_DONE,
				 ((LPCREATESTRUCT) lParam)->hInstance, NULL);
      SetFocus (hwnEdit);
      break;

    case WM_ENTER:
      txtLength = GetWindowTextLength (hwnEdit);
      if (txtLength >= LgMaxAttrText)
	txtLength = LgMaxAttrText - 1;
      GetWindowText (hwnEdit, Attr_text, txtLength + 1);
      i = 0;
      while (i < txtLength && Attr_text[i] != __CR__)
	i++;
      if (i < txtLength)
	Attr_text[i] = EOS;
      ThotCallback (NumMenuAttrText, STRING_DATA, Attr_text);
      ThotCallback (NumMenuAttr, INTEGER_DATA, (STRING) 1);
      ThotCallback (NumMenuAttr, INTEGER_DATA, (STRING) 0);
      DestroyWindow (hwnd);
      break;
      
    case WM_DESTROY :
      PostQuitMessage (0);
      break;
      
    case WM_COMMAND:
      switch (LOWORD (wParam))
	{
	case ID_APPLY:
	  txtLength = GetWindowTextLength (hwnEdit);
	  if (txtLength >= LgMaxAttrText)
	    txtLength = LgMaxAttrText - 1;
	  GetWindowText (hwnEdit, Attr_text, txtLength + 1);
	  i = 0;
	  while (i < txtLength && Attr_text[i] != __CR__)
	    i++;
	  if (i < txtLength)
	    Attr_text[i] = EOS;
	  ThotCallback (NumMenuAttrText, STRING_DATA, Attr_text);
	  ThotCallback (NumMenuAttr, INTEGER_DATA, (STRING) 1);
	  break;
	  
	case ID_DELETE:
	  txtLength = GetWindowTextLength (hwnEdit);
	  if (txtLength >= LgMaxAttrText)
	    txtLength = LgMaxAttrText - 1;
	  GetWindowText (hwnEdit, Attr_text, txtLength + 1);
	  i = 0;
	  while (i < txtLength && Attr_text[i] != __CR__)
	    i++;
	  if (i < txtLength)
	    Attr_text[i] = EOS;
	  ThotCallback (NumMenuAttrText, STRING_DATA, Attr_text);
	  ThotCallback (NumMenuAttr, INTEGER_DATA, (STRING) 2);
	  DestroyWindow (hwnd);
	  break;
	  
	case ID_DONE:
	  ThotCallback (NumMenuAttr, INTEGER_DATA, (STRING) 0);
	  DestroyWindow (hwnd);
	  break;
	}
      break;
    }
  return DefWindowProc (hwnd, iMsg, wParam, lParam);
}

/*----------------------------------------------------------------------
  WIN_InitNumAttrDialog
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static BOOL WIN_InitNumAttrDialog (HWND parent, STRING title)
#else  /* __STDC__ */
static BOOL WIN_InitNumAttrDialog (parent, title)
HWND  parent;
STRING title	;
#endif /* __STDC__ */
{
   WNDCLASSEX    wndNumAttrClass;
   static STRING szAppName;
   HWND          hwnNumAttrDialog;
   MSG           msg;
   int           frame;

   szAppName = TEXT("NumAttrClass");
   if (!wndNumAttrRegistered)
     {
       wndNumAttrRegistered = TRUE;
       wndNumAttrClass.style         = CS_HREDRAW | CS_VREDRAW;
       wndNumAttrClass.lpfnWndProc   = InitNumAttrDialogWndProc;
       wndNumAttrClass.cbClsExtra    = 0;
       wndNumAttrClass.cbWndExtra    = 0;
       wndNumAttrClass.hInstance     = hInstance;
       wndNumAttrClass.hIcon         = LoadIcon (NULL, IDI_APPLICATION);
       wndNumAttrClass.hCursor       = LoadCursor (NULL, IDC_ARROW);
       wndNumAttrClass.hbrBackground = (HBRUSH) GetStockObject (LTGRAY_BRUSH);
       wndNumAttrClass.lpszMenuName  = NULL;
       wndNumAttrClass.lpszClassName = szAppName;
       wndNumAttrClass.cbSize        = sizeof(WNDCLASSEX);
       wndNumAttrClass.hIconSm       = LoadIcon (hInstance, iconID);
       
       if (!RegisterClassEx (&wndNumAttrClass))
         return (FALSE);
     }
   
   hwnNumAttrDialog = CreateWindow (szAppName, title,
                                    DS_MODALFRAME | WS_POPUP | 
                                    WS_VISIBLE | WS_CAPTION | WS_SYSMENU,
                                    ClickX, ClickY,
                                    285, 110,
                                    parent, NULL, hInstance, NULL);
   
   ShowWindow (hwnNumAttrDialog, SW_SHOWNORMAL);
   UpdateWindow (hwnNumAttrDialog);
   
   while (GetMessage (&msg, NULL, 0, 0))
     {
       frame = GetFrameNumber (msg.hwnd);
       TranslateMessage (&msg);
       DispatchMessage (&msg);
     }
   return TRUE;
}

/*----------------------------------------------------------------------
  InitNumAttrDialogWndProc
  ----------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK InitNumAttrDialogWndProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
#else  /* __STDC__ */
LRESULT CALLBACK InitNumAttrDialogWndProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
#endif /* __STDC__ */
{
  HWND        hwnTitle;
  HWND        hwnRange;
  HWND        applyButton;
  HWND        deleteButton;
  HWND        doneButton;
  BOOL        ok;
  int         val;

  switch (iMsg)
    {
    case WM_CREATE:
      /* Create static window for the title */
      hwnTitle = CreateWindow (TEXT("STATIC"), WIN_pAttr1->AttrName, 
			       WS_CHILD | WS_VISIBLE | SS_LEFT,
			       10, 10, 160, 240, hwnd, (HMENU) 1,
			       ((LPCREATESTRUCT) lParam)->hInstance, NULL); 
      
      /* Create Edit Window autoscrolled */
      hwnEdit = CreateWindow (TEXT("EDIT"), NULL, 
			      WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT |
			         ES_AUTOHSCROLL,
			      10, 30, 120, 20, hwnd, (HMENU) ID_EDITVALUE,
			      ((LPCREATESTRUCT) lParam)->hInstance, NULL);
      SetDlgItemInt (hwnd, ID_EDITVALUE, formValue, TRUE);
      if (lpfnTextZoneWndProc == (WNDPROC) 0)
	lpfnTextZoneWndProc = (WNDPROC) SetWindowLong (hwnEdit, GWL_WNDPROC,
						       (DWORD) textZoneProc);
      else
	SetWindowLong (hwnEdit, GWL_WNDPROC, (DWORD) textZoneProc);
      
      /* Create Apply button */
      applyButton = CreateWindow (TEXT("BUTTON"),
				  TtaGetMessage (LIB, TMSG_APPLY), 
				  WS_CHILD | BS_DEFPUSHBUTTON | WS_VISIBLE,
				  10, 55, 65, 25, hwnd, (HMENU) ID_APPLY,
				  ((LPCREATESTRUCT) lParam)->hInstance, NULL);
      
      /* Create Delete Button */
      deleteButton = CreateWindow (TEXT("BUTTON"),
				   TtaGetMessage (LIB, TMSG_DEL_ATTR), 
				   WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE,
				   80, 55, 120, 25, hwnd, (HMENU) ID_DELETE,
				   ((LPCREATESTRUCT) lParam)->hInstance, NULL);
      
      /* Create Done Button */
      doneButton = CreateWindow (TEXT("BUTTON"),
				 TtaGetMessage (LIB, TMSG_DONE), 
				 WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE,
				 205, 55, 65, 25, hwnd, (HMENU) ID_DONE,
				 ((LPCREATESTRUCT) lParam)->hInstance, NULL);
      SetFocus (hwnEdit);
      break;
      
    case WM_DESTROY :
      PostQuitMessage (0);
      break;

    case WM_ENTER:
      val = GetDlgItemInt (hwnd, ID_EDITVALUE, &ok, TRUE);
      if (ok)
	{
	  ThotCallback (NumMenuAttrNumber, INTEGER_DATA, (STRING) val);
	  ThotCallback (NumMenuAttr, INTEGER_DATA, (STRING) 1);
	}
      break;
      
    case WM_COMMAND:
      switch (LOWORD (wParam))
	{
	case ID_APPLY:
	  val = GetDlgItemInt (hwnd, ID_EDITVALUE, &ok, TRUE);
	  if (ok)
	    {
	      ThotCallback (NumMenuAttrNumber, INTEGER_DATA, (STRING) val);
	      ThotCallback (NumMenuAttr, INTEGER_DATA, (STRING) 1);
	    }
	  break;
	  
	case ID_DELETE:
	  val = GetDlgItemInt (hwnd, ID_EDITVALUE, &ok, TRUE);
	  if (ok)
	    {
	      ThotCallback (NumMenuAttrNumber, INTEGER_DATA, (STRING) val);
	      ThotCallback (NumMenuAttr, INTEGER_DATA, (STRING) 2);
	      DestroyWindow (hwnd);
	    }
	  break;
	  
	case ID_DONE:
	  ThotCallback (NumMenuAttr, INTEGER_DATA, (STRING) 0);
	  DestroyWindow (hwnd);
	  break;
	}
      break;
    }
  return DefWindowProc (hwnd, iMsg, wParam, lParam);
}
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
   MenuValues
   builds the sheet for capturing the values of the attribute defined
   by the pAttr1 rule.
   required specifies if it's a required attribute
   currAttr gives the current value of the attribute
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         MenuValues (TtAttribute * pAttr1, ThotBool required, PtrAttribute currAttr,
				PtrDocument pDoc, int view)
#else  /* __STDC__ */
static void         MenuValues (pAttr1, required, currAttr, pDoc, view)
TtAttribute        *pAttr1;
ThotBool            required;
PtrAttribute        currAttr;
PtrDocument         pDoc;
int                 view;

#endif /* __STDC__ */

{
   int                 i, lgmenu, val;
   int                 form, subform;
   Document            doc;
   CHAR_T              bufMenu[MAX_TXT_LEN];
   CHAR_T              TxtAttrValue[LgMaxAttrText];
#ifndef _WINDOWS
   CHAR_T              title[MAX_NAME_LENGTH + 2];
#else  /* _WINDOWS */
   WIN_pAttr1 = pAttr1;
   WIN_currAttr	= currAttr;
#endif /* _WINDOWS */

   doc = (Document) IdentDocument (pDoc);
   /* detruit la feuille de dialogue et la recree */
   ustrcpy (bufMenu, TtaGetMessage (LIB, TMSG_APPLY));
   i = ustrlen (bufMenu) + 1;
   ustrcpy (&bufMenu[i], TtaGetMessage (LIB, TMSG_DEL_ATTR));
   if (required)
     {
       form = NumMenuAttrRequired;
       if (MandatoryAttrFormExists)
	 {
	   TtaUnmapDialogue (NumMenuAttrRequired);
	   TtaDestroyDialogue (NumMenuAttrRequired);
	 } 
#ifndef _WINDOWS 
       TtaNewForm (NumMenuAttrRequired, TtaGetViewFrame (doc, view),
		   TtaGetMessage (LIB, TMSG_ATTR), FALSE, 2, 'L', D_DONE);
#else  /* _WINDOWS */
       isForm = TRUE;
       WIN_InitFormDialog (TtaGetViewFrame (doc, view),
			   TtaGetMessage (LIB, TMSG_ATTR));
#endif /* _WINDOWS */
       MandatoryAttrFormExists = TRUE;
     }
   else
     {
       form = NumMenuAttr;
       if (AttrFormExists)
	 {
	 TtaUnmapDialogue (NumMenuAttr);
	 TtaDestroyDialogue (NumMenuAttr);
	 } 
#ifndef _WINDOWS
       TtaNewSheet (NumMenuAttr, TtaGetViewFrame (doc, view),
		    TtaGetMessage (LIB, TMSG_ATTR), 2, bufMenu, FALSE, 2,
		    'L', D_DONE);
#else  /* _WINDOWS */
       isForm = FALSE;
#endif /* _WINDOWS */
       AttrFormExists = TRUE;
     }  

#ifdef _WINDOWS
   ustrncpy (WIN_title, pAttr1->AttrName, MAX_NAME_LENGTH);
#else  /* !_WINDOWS */
   ustrncpy (title, pAttr1->AttrName, MAX_NAME_LENGTH);
#endif /* _WINDOWS */
   switch (pAttr1->AttrType)
     {
     case AtNumAttr: /* attribut a valeur numerique */
       subform = form + 1;
#ifndef _WINDOWS
       TtaNewNumberForm (subform, form, title, -MAX_INT_ATTR_VAL,
			 MAX_INT_ATTR_VAL, TRUE);
       TtaAttachForm (subform);
#endif /* !_WINDOWS */
       if (currAttr == NULL)
	 i = 0;
       else
	 i = currAttr->AeAttrValue;
       
#ifndef _WINDOWS
       TtaSetNumberForm (subform, i);
#else /* !_WINDOWS */
       WIN_AtNumAttr  = TRUE;
       WIN_AtTextAttr = FALSE;
       WIN_AtEnumAttr = FALSE;
       usprintf (formRange, TEXT("%d .. %d"), -MAX_INT_ATTR_VAL,
		 MAX_INT_ATTR_VAL); 
       formValue = i;
#endif /* _WINDOWS */
       break;
       
     case AtTextAttr: /* attribut a valeur textuelle */
       subform = form + 2;
#ifndef _WINDOWS
       TtaNewTextForm (subform, form, title, 40, 1, FALSE);
       TtaAttachForm (subform);
       if (currAttr == NULL)
	 TtaSetTextForm (subform, "");
       else if (currAttr->AeAttrText == NULL)
	 TtaSetTextForm (subform, "");
       else
	 {
	   i = LgMaxAttrText - 2;
	   CopyTextToString (currAttr->AeAttrText, TxtAttrValue, &i);
	   TtaSetTextForm (subform, TxtAttrValue);
	 }
       
#else  /* _WINDOWS */
       WIN_AtNumAttr  = FALSE;
       WIN_AtTextAttr = TRUE;
       WIN_AtEnumAttr = FALSE;
#endif /* _WINDOWS */
       break;
       
     case AtEnumAttr: /* attribut a valeurs enumerees */
       subform = form + 3;
       /* cree un menu de toutes les valeurs possibles de l'attribut */
       lgmenu = 0;
       val = 0;
       /* boucle sur les valeurs possibles de l'attribut */
       while (val < pAttr1->AttrNEnumValues)
	 {
#ifdef _WINDOWS 
	   i = ustrlen (pAttr1->AttrEnumValue[val]) + 1; /* for 'B' and EOS */
#else  /* !_WINDOWS */
	   i = ustrlen (pAttr1->AttrEnumValue[val]) + 2; /* for 'B' and EOS */
#endif /* _WINDOWS */
	   if (lgmenu + i < MAX_TXT_LEN)
	     {
#ifndef _WINDOWS
	       bufMenu[lgmenu] = 'B';
	       ustrcpy (&bufMenu[lgmenu + 1], pAttr1->AttrEnumValue[val]);
#else  /* _WINDOWS */
	       ustrcpy (&WIN_buffMenu[lgmenu], pAttr1->AttrEnumValue[val]);
#endif /* _WINDOWS */
	       val++;
	     } 
	   lgmenu += i;
	 }

#ifndef _WINDOWS
       /* cree le menu des valeurs de l'attribut */
       TtaNewSubmenu (subform, form, 0, title, val, bufMenu, NULL, TRUE);
       TtaAttachForm (subform);
       /* initialise le menu avec la valeur courante */
       val = -1;
       if (currAttr != NULL)
	 val = currAttr->AeAttrValue - 1;
       TtaSetMenuForm (subform, val);
#else  /* _WINDOWS */
       nbDlgItems = val;
       WIN_AtNumAttr  = FALSE;
       WIN_AtTextAttr = FALSE;
       WIN_AtEnumAttr = TRUE;
#endif /* _WINDOWS */
       break;

     case AtReferenceAttr: /* attribut reference, on ne fait rien */
       break;
       
     default: break;
     } 
} 


/*----------------------------------------------------------------------
   CallbackReqAttrMenu
   handles the callback of the menu which captures the required attributes.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CallbackReqAttrMenu (int ref, int val, STRING txt)
#else  /* __STDC__ */
void                CallbackReqAttrMenu (ref, val, txt)
int                 ref;
int                 val;
STRING              txt;

#endif /* __STDC__ */
{
  int                 length;
  
  switch (ref)
    {
    case NumMenuAttrRequired:
      /* retour de la feuille de dialogue elle-meme */
      /* on detruit cette feuille de dialogue sauf si c'est */
      /* un abandon */
      if (val != 0)
	{
	  TtaDestroyDialogue (NumMenuAttrRequired);
	  MandatoryAttrFormExists = FALSE;
	}
      /* on ne fait rien d'autre : tout a ete fait par les cas */
      /* suivants */
      break;
    case NumMenuAttrNumNeeded:
      /* zone de saisie de la valeur numerique de l'attribut */
      if (val >= -MAX_INT_ATTR_VAL || val <= MAX_INT_ATTR_VAL)
	PtrReqAttr->AeAttrValue = val;
      break;
    case NumMenuAttrTextNeeded:
      /* zonee de saisie du texte de l'attribut */
      if (PtrReqAttr->AeAttrText == NULL)
	GetTextBuffer (&PtrReqAttr->AeAttrText);
      else
	ClearText (PtrReqAttr->AeAttrText);
      CopyStringToText (txt, PtrReqAttr->AeAttrText, &length);
      break;
    case NumMenuAttrEnumNeeded:
      /* menu des valeurs d'un attribut a valeurs enumerees */
      val++;
      PtrReqAttr->AeAttrValue = val;
      break;
    default:
      break;
    }
}

/*----------------------------------------------------------------------
   BuildReqAttrMenu
   builds the form for capturing the value of the required
   attribute as defined by the pRuleAttr rule.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                BuildReqAttrMenu (PtrAttribute pAttr, PtrDocument pDoc)
#else  /* __STDC__ */
void                BuildReqAttrMenu (pAttr, pDoc)
PtrAttribute        pAttr;
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   TtAttribute        *pRuleAttr;

   PtrReqAttr = pAttr;
   pRuleAttr = &pAttr->AeAttrSSchema->SsAttribute[pAttr->AeAttrNum - 1];
   /* toujours lie a la vue 1 du document */
   MenuValues (pRuleAttr, TRUE, NULL, pDoc, 1);
#ifndef _WINDOWS
   TtaShowDialogue (NumMenuAttrRequired, FALSE);
   TtaWaitShowDialogue ();
#endif /* !_WINDOWS */
}


/*----------------------------------------------------------------------
   TteItemMenuAttr 
   sends the AttrMenu.Pre message which indicates that the editor
   is going to add to the Attributes menu an item for the creation
   of an attribute of type (pSS, att) for the pEl element. It 
   returns the answer from the application.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     TteItemMenuAttr (PtrSSchema pSS, int att, PtrElement pEl, PtrDocument pDoc)
#else  /* __STDC__ */
static ThotBool     TteItemMenuAttr (pSS, att, pEl, pDoc)
PtrSSchema          pSS;
int                 att;
PtrElement          pEl;
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   NotifyAttribute     notifyAttr;
   ThotBool            OK;

   notifyAttr.event = TteAttrMenu;
   notifyAttr.document = (Document) IdentDocument (pDoc);
   notifyAttr.element = (Element) pEl;
   notifyAttr.attribute = NULL;
   notifyAttr.attributeType.AttrSSchema = (SSchema) pSS;
   notifyAttr.attributeType.AttrTypeNum = att;
   OK = !CallEventAttribute (&notifyAttr, TRUE);
   return OK;
}


/*----------------------------------------------------------------------
   BuildAttrMenu
   builds the Attributes menu and returns the number of attributes added
   to the composite menu.
   Returns also the number of events attibutes and updates the corresponding
   buffer.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int    BuildAttrMenu (STRING bufMenu, PtrDocument pDoc, int *nbEvent, STRING bufEventAttr)
#else  /* __STDC__ */
static int    BuildAttrMenu (bufMenu, pDoc, nbEvent, bufEventAttr)
STRING        bufMenu;
PtrDocument   pDoc;
int          *nbEvent;
STRING        bufEventAttr;
#endif /* __STDC__ */
{
  PtrDocument         SelDoc;
  PtrElement          firstSel, lastSel, pEl;
  PtrSSchema          pSS;
  PtrSSchema          pSchExt;
  PtrAttribute        pAttrNew;
  SRule              *pRe1;
  TtAttribute        *pAttr;
  CHAR_T                tempBuffer[MAX_NAME_LENGTH + 1];
  int                 i, j, k;
  int                 firstChar, lastChar;
  int                 lgmenu = 0, lgsubmenu;
  int                 att, nbOfEntries;
  ThotBool            selectionOK, isNew;

  nbOfEntries = 0;
  *nbEvent = 0;
  /* demande quelle est la selection courante */
  selectionOK = GetCurrentSelection (&SelDoc, &firstSel, &lastSel, &firstChar,
				     &lastChar);
  if (selectionOK && SelDoc == pDoc)
    /* il y a une selection et elle est dans le document traite' */
    {
      /* cherche les attributs definis dans les differents schemas de */
      /* structure */
      pSS = NULL;
      pEl = firstSel;
      /* cherche tous les schemas de structure utilises par les ascendants */
      /* du premier element selectionne' (lui-meme compris) */
      while (pEl != NULL)
	{
	  /* schema de struct de l'element courant */
	  pSS = pEl->ElStructSchema;
	  if (pSS == NULL)
	    pEl = NULL;
	  else
	    {
	      /* on parcourt toutes les extensions de schema de ce schema */
	      do
		{
		  /* on a deja traite' ce schema de structure ? */
		  isNew = TRUE;
		  for (i = 0; i < nbOfEntries; i++)
		    if (pSS == AttrStruct[i])
		      /* already known */
		      isNew = FALSE;

		  if (isNew)
		    /* the element uses a new structure schema */
		    /* add all global attributes of this schema in the table */
		    {
		      att = 0;
		      while (att < pSS->SsNAttributes &&
			     nbOfEntries - *nbEvent < MAX_MENU)
			{
			  att++;
			  /* skip local attributes */
			  if (pSS->SsAttribute[att - 1].AttrGlobal &&
			    /* and invisible attributes */
			    !AttrHasException (ExcInvisible, att, pSS))
			    /* skip the attribute Langue execpt the first time */
			    if (nbOfEntries == 0 || att != 1)
			      if (TteItemMenuAttr (pSS, att, firstSel, SelDoc))
				{
				  /* keep in mind the structure schema and */
				  /* the attribute number of this new entry */
				  AttrStruct[nbOfEntries] = pSS;
				  AttrNumber[nbOfEntries] = att;
				  AttrOblig[nbOfEntries] = FALSE;
				  /* is it an event attribute */
				  AttrEvent[nbOfEntries] =
				     AttrHasException (ExcEventAttr, att, pSS);
				  if (AttrEvent[nbOfEntries])
				    (*nbEvent)++;
				  nbOfEntries++;
				}
			}
		    }
		  /* next extension schema */
		  pSS = pSS->SsNextExtens;
		}
	      while (pSS != NULL);
	      /* next parent element */
	      pEl = pEl->ElParent;
	    }
	}
      
      /* cherche les attributs locaux du premier element selectionne' */
      pSS = firstSel->ElStructSchema;
      if (pSS != NULL)
	{
	  pRe1 = &pSS->SsRule[firstSel->ElTypeNumber - 1];
	  pSchExt = SelDoc->DocSSchema;
	  do
	    {
	      if (pRe1 != NULL)
		/* prend les attributs locaux definis dans cette regle */
		for (att = 0; att < pRe1->SrNLocalAttrs; att++)
		  if (nbOfEntries - *nbEvent < MAX_MENU &&
		      !AttrHasException (ExcInvisible, pRe1->SrLocalAttr[att],
					 pSS) &&
		      TteItemMenuAttr (pSS, pRe1->SrLocalAttr[att], firstSel,
				       SelDoc))
		    {
		      /* conserve le schema de structure et le numero */
		      /* d'attribut de cette nouvelle entree du menu */
		      AttrStruct[nbOfEntries] = pSS;
		      AttrNumber[nbOfEntries] = pRe1->SrLocalAttr[att];
		      AttrOblig[nbOfEntries] = pRe1->SrRequiredAttr[att];
		      /* is it an event attribute */
		      AttrEvent[nbOfEntries] = AttrHasException (ExcEventAttr,
						 pRe1->SrLocalAttr[att], pSS);
		      if (AttrEvent[nbOfEntries])
			(*nbEvent)++;
		      nbOfEntries++;
		    }
	      /* passe a l'extension suivante du schema du document */
	      pSchExt = pSchExt->SsNextExtens;
	      /* cherche dans cette extension de schema la regle d'extension */
	      /* pour le premier element selectionne' */
	      if (pSchExt != NULL)
		{
		  pSS = pSchExt;
		  pRe1 = ExtensionRule (firstSel->ElStructSchema,
					firstSel->ElTypeNumber, pSchExt);
		}
	    }
	  while (pSchExt != NULL);
	}
      
      /* la table contient tous les attributs applicables aux elements */
      /* selectionnes */
      GetAttribute (&pAttrNew);
      if (nbOfEntries > 0)
	{
	  lgmenu = 0;
	  lgsubmenu = 0;
	  k = 0;
	  j = 0;
	  /* met les noms des attributs de la table dans le menu */
	  for (att = 0; att < nbOfEntries; att++)
	    {
	      pAttr = &AttrStruct[att]->SsAttribute[AttrNumber[att] - 1];
	      
	      pAttrNew->AeAttrSSchema = AttrStruct[att];
	      pAttrNew->AeAttrNum = AttrNumber[att];
	      pAttrNew->AeDefAttr = FALSE;
	      if (pAttr->AttrType == AtEnumAttr && pAttr->AttrNEnumValues == 1)
		/* attribut enumere' a une seule valeur (attribut booleen) */
		usprintf (tempBuffer, TEXT("T%s"), pAttr->AttrName);
	      else
		usprintf (tempBuffer, TEXT("T%s..."), pAttr->AttrName);
	      i = ustrlen (tempBuffer) + 1;
	      if (AttrEvent[att])
		{
		  if (lgsubmenu + i < MAX_TXT_LEN)
		    ustrcpy (&bufEventAttr[lgsubmenu], tempBuffer);
		  lgsubmenu += i;
		  /* mark all active enties*/
		  if (AttributeValue (firstSel, pAttrNew) != NULL)
		    ActiveEventAttr[k] = 1;
		  else
		    ActiveEventAttr[k] = 0;
		  AttrEventNumber[k] = att;
		  k++;
		}
	      else
		{
		  if (lgmenu + i < MAX_TXT_LEN)
		    ustrcpy (&bufMenu[lgmenu], tempBuffer);
		  lgmenu += i;
		  /* mark all active enties*/
		  if (AttributeValue (firstSel, pAttrNew) != NULL)
		    ActiveAttr[j] = 1;
		  else
		    ActiveAttr[j] = 0;
		  j++;
		}
	    }
	}
      DeleteAttribute (NULL, pAttrNew);
      if (*nbEvent > 0)
	{
	  /* add the event entry if needed */
	  usprintf (tempBuffer, TEXT("M%s"), TtaGetMessage (LIB, TMSG_EVENTS));
	  i = ustrlen (tempBuffer) + 1;
	  if (lgmenu + i < MAX_TXT_LEN)
	    {
	      ustrcpy (&bufMenu[lgmenu], tempBuffer);
	      nbOfEntries++;
	    }
	}
    }

  return (nbOfEntries - *nbEvent);
}


/*----------------------------------------------------------------------
   UpdateAttrMenu                                                       
   Updates the Attributes menu of all open frames belonging to document
   pDoc.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                UpdateAttrMenu (PtrDocument pDoc)
#else  /* __STDC__ */
void                UpdateAttrMenu (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */
{
#ifndef _GTK
  Document            document;
  Menu_Ctl           *pMenu;
  CHAR_T                bufMenuAttr[MAX_TXT_LEN];
  CHAR_T                bufEventAttr[MAX_TXT_LEN];
  int                 view, menu, menuID;
  int                 frame, ref, nbEvent;
  int                 nbItemAttr, i, max;

  /* Compose le menu des attributs */
  if (pDoc == SelectedDocument && !pDoc->DocReadOnly)
    nbItemAttr = BuildAttrMenu (bufMenuAttr, pDoc, &nbEvent, bufEventAttr);
  else
    {
      nbItemAttr = 0;
      nbEvent = 0;
    }

  document = (Document) IdentDocument (pDoc);
  /* Traite toutes les vues de l'arbre principal */
  for (view = 1; view <= MAX_VIEW_DOC; view++)
    {
      frame = pDoc->DocViewFrame[view - 1];
      if (frame != 0 && FrameTable[frame].MenuAttr != -1)
	{
#ifdef _WINDOWS 
	  currentFrame = frame;
#endif /* _WINDOWS */
	  menuID = FrameTable[frame].MenuAttr;
	  menu = FindMenu (frame, menuID, &pMenu) - 1;
	  ref = (menu * MAX_ITEM) + frame + MAX_LocalMenu;
	  if (pDoc != SelectedDocument || nbItemAttr == 0)
	    {
	      /* le menu Attributs contient au moins un attribut */
	      TtaSetMenuOff (document, view, menuID);
	      TtaDestroyDialogue (ref);
	    }
	  else
	    {
#ifdef _WINDOWS
	      int nbOldItems = GetMenuItemCount (FrameTable[frame].WdMenus[menu]);
	      for (i = 0; i < nbOldItems; i ++)
		{
		  if (!DeleteMenu (FrameTable[frame].WdMenus[menu], ref + i,
				   MF_BYCOMMAND))
		    DeleteMenu (FrameTable[frame].WdMenus[menu], i,
				MF_BYPOSITION);
		}
#endif /* _WINDOWS */
	      if (EventMenu[frame - 1] != 0)
		{
		  /* destroy the submenu event */
		  TtaDestroyDialogue (EventMenu[frame - 1]);
#ifdef _WINDOWS
		  if (subMenuID[frame])
		    {
		      DeleteMenu (FrameTable[frame].WdMenus[menu], subMenuID[frame], MF_BYCOMMAND);
		    }
		  else 
		    subMenuID[frame] = 0;
#endif /* _WINDOWS */
		  EventMenu[frame - 1] = 0;
		}
	      TtaNewPulldown (ref, FrameTable[frame].WdMenus[menu], NULL,
			      nbItemAttr, bufMenuAttr, NULL);
	      if (nbEvent != 0)
		{
		  /* there is a submenu of event attributes */
		  EventMenu[frame - 1] = (nbItemAttr * MAX_MENU * MAX_ITEM) + ref;
		  TtaNewSubmenu (EventMenu[frame - 1], ref, nbItemAttr - 1,
				 NULL, nbEvent, bufEventAttr, NULL, FALSE);
		  /* post active attributes */
		  for (i = 0; i < nbEvent; i++)
#ifdef _WINDOWS
		    WIN_TtaSetToggleMenu (EventMenu[frame - 1], i,
		       (ThotBool) (ActiveEventAttr[i] == 1), FrMainRef[frame]);
#else  /* !_WINDOWS */
		  TtaSetToggleMenu (EventMenu[frame - 1], i, (ActiveEventAttr[i] == 1));
#endif /* _WINDOWS */
		}

	      /* post active attributes */
	      max = nbItemAttr;
	      if (nbEvent != 0)
		/* except the submenu entry */
		max--;
	      for (i = 0; i < max; i++)
#ifdef _WINDOWS
		WIN_TtaSetToggleMenu (ref, i, (ThotBool) (ActiveAttr[i] == 1), FrMainRef[frame]);
#else  /* !_WINDOWS */
	      TtaSetToggleMenu (ref, i, (ActiveAttr[i] == 1));
#endif /* _WINDOWS */
	      TtaSetMenuOn (document, view, menuID);
	    }
	}
    }

  /* Traite toutes les vues des arbres associes */
  for (view = 1; view <= MAX_ASSOC_DOC; view++)
    {
      frame = pDoc->DocAssocFrame[view - 1];
      if (frame != 0 && FrameTable[frame].MenuAttr != -1)
	{
	  menuID = FrameTable[frame].MenuAttr;
	  menu = FindMenu (frame, menuID, &pMenu) - 1;
	  ref = (menu * MAX_ITEM) + frame + MAX_LocalMenu;
	  if (pDoc != SelectedDocument || nbItemAttr == 0)
	    {
	      /* le menu Attributs contient au moins un attribut */
	      TtaSetMenuOff (document, view, menu);
	      TtaDestroyDialogue (ref);
	    }
	  else
	    {
	      TtaNewPulldown (ref, FrameTable[frame].WdMenus[menu], NULL,
			      nbItemAttr, bufMenuAttr, NULL);
	      /* marque les attributs actifs */
	      for (i = 0; i < nbItemAttr; i++)
#ifdef _WINDOWS
		WIN_TtaSetToggleMenu (ref, i, (ThotBool)(ActiveAttr[i] == 1), FrMainRef[frame]);
#else  /* !_WINDOWS */
	      TtaSetToggleMenu (ref, i, (ActiveAttr[i] == 1));
#endif /* _WINDOWS */
	      TtaSetMenuOn (document, view, menu);
	    }
	}
    }
#endif /* _GTK */
}

/*----------------------------------------------------------------------
   AttachAttrToElem attachs the attribute to the element
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void	AttachAttrToElem (PtrAttribute pAttr, PtrElement pEl, PtrDocument pDoc)
#else  /* __STDC__ */
static void	AttachAttrToElem (pAttr, pEl, pDoc)
PtrAttribute pAttr;
PtrElement pEl;
PtrDocument pDoc;

#endif /* __STDC__ */
{
   Language            lang;
   PtrAttribute        pAttrAsc;
   PtrElement          pElAttr;

   /* On ne traite pas les marques de page */
   if (!pEl->ElTerminal || pEl->ElLeafType != LtPageColBreak)
     {
        if (pAttr->AeAttrNum == 1)
   	/* c'est l'attribut langue */
          {
   	  /* change la langue de toutes les feuilles de texte du sous-arbre */
   	  /* de l'element */
   	  if (pAttr->AeAttrText != NULL)
   	     lang = TtaGetLanguageIdFromName (pAttr->AeAttrText->BuContent);
   	  else
   	     /* c'est une suppression de l'attribut Langue */
   	    {
   	       lang = TtaGetDefaultLanguage ();		/* langue par defaut */
   	       /* on cherche si un ascendant porte l'attribut Langue */
   	       if (pEl->ElParent != NULL)
   		 pAttrAsc = GetTypedAttrAncestor (pEl->ElParent, 1, NULL,
						  &pElAttr);
   	       else
   		 pAttrAsc = GetTypedAttrAncestor (pEl->ElParent, 1, NULL,
						  &pElAttr);

   	       if (pAttrAsc != NULL)
   		  /* un ascendant definit la langue, on prend cette langue */
   		  if (pAttrAsc->AeAttrText != NULL)
   		     lang = TtaGetLanguageIdFromName (pAttrAsc->AeAttrText->BuContent);
   	    }
   	  ChangeLanguage (pDoc, pEl, lang, FALSE);
          }

        /* met la nouvelle valeur de l'attribut dans l'element et */
        /* applique les regles de presentation de l'attribut a l'element */
        AttachAttrWithValue (pEl, pDoc, pAttr);
        if (ThotLocalActions[T_attrtable] != NULL)
   	(*ThotLocalActions[T_attrtable])
   	   (pEl, pAttr, pDoc);	/* cas particulier des tableaux */

     }
}


/*----------------------------------------------------------------------
   AttachAttrToRange applique l'attribut pAttr a une partie de document
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         AttachAttrToRange (PtrAttribute pAttr, int lastChar, int firstChar, PtrElement pLastSel, PtrElement pFirstSel, PtrDocument pDoc, ThotBool reDisplay)
#else  /* __STDC__ */
static void         AttachAttrToRange (pAttr, lastChar, firstChar, pLastSel, pFirstSel, pDoc, reDisplay)
PtrAttribute        pAttr;
int                 lastChar;
int                 firstChar;
PtrElement          pLastSel;
PtrElement          pFirstSel;
PtrDocument         pDoc;
ThotBool		    reDisplay;
#endif /* __STDC__ */
{
   PtrElement          pEl;
   int                 i;

   /* eteint d'abord la selection */
   TtaClearViewSelections ();
   /* Coupe les elements du debut et de la fin de la selection s'ils */
   /* sont partiellement selectionnes */
   IsolateSelection (pDoc, &pFirstSel, &pLastSel, &firstChar, &lastChar, TRUE);
   /* start an operation sequence in editing history */
   if (ThotLocalActions[T_openhistory] != NULL)
     (*ThotLocalActions[T_openhistory]) (pDoc, pFirstSel, pLastSel, firstChar,
					 lastChar);
   /* parcourt les elements selectionnes */
   pEl = pFirstSel;
   while (pEl != NULL)
     {
	AttachAttrToElem (pAttr, pEl, pDoc);
	/* cherche l'element a traiter ensuite */
	pEl = NextInSelection (pEl, pLastSel);
     }
   /* si c'est un changement de langue qui s'applique a la racine de */
   /* l'arbre principal du document, on change aussi la langue de */
   /* tous les autres arbre de ce document */
   if (pAttr->AeAttrNum == 1)	/* attribut Langue = 1 */
      if (pFirstSel == pDoc->DocRootElement)
	{
	   for (i = 1; i <= MAX_PARAM_DOC; i++)
	      if (pDoc->DocParameters[i - 1] != NULL)
		 AttachAttrToElem (pAttr, pDoc->DocParameters[i - 1], pDoc);
	   for (i = 1; i <= MAX_ASSOC_DOC; i++)
	      if (pDoc->DocAssocRoot[i - 1] != NULL)
		 AttachAttrToElem (pAttr, pDoc->DocAssocRoot[i - 1], pDoc);
	}
   /* close the editing sequence */
   if (ThotLocalActions[T_closehistory] != NULL)
	(*ThotLocalActions[T_closehistory]) (pDoc);
   /* parcourt a nouveau les elements selectionnes pour fusionner les */
   /* elements voisins de meme type ayant les memes attributs, reaffiche */
   /* toutes les vues et retablit la selection */
   if (reDisplay)
     SelectRange (pDoc, pFirstSel, pLastSel, firstChar, lastChar);
}

/*----------------------------------------------------------------------
   CallbackValAttrMenu
   handles the callback of the form which captures the attribute values.
   Applies to the selected elements the attributes chosen by the user.
   ref: reference to the dialogue element who called back this function
   valmenu: selected or captured value in this dialogue element
   valtexte: pointer to the captured text in this dialogue element
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CallbackValAttrMenu (int ref, int valmenu, STRING valtext)
#else  /* __STDC__ */
void                CallbackValAttrMenu (ref, valmenu, valtext)
int                 ref;
int                 valmenu;
STRING              valtext;

#endif /* __STDC__ */
{
  PtrDocument         SelDoc;
  PtrElement          firstSel, lastSel;
  PtrAttribute        pAttrNew;
  DisplayMode         dispMode = DeferredDisplay;
  Document            doc = 0;
  int                 firstChar, lastChar;
  int                 lg, act;
  ThotBool            lock = TRUE;

  act = 1; /* apply by default */
  switch (ref)
    {
    case NumMenuAttrNumber:
      /* valeur d'un attribut numerique */
      NumAttrValue = valmenu;
      act = 0;
      break;
    case NumMenuAttrText:
      /* valeur d'un attribut textuel */
      ustrncpy (TextAttrValue, valtext, LgMaxAttrText);
      act = 0;
      break;
    case NumMenuAttrEnum:
      /* numero de la valeur d'un attribut enumere' */
      NumAttrValue = valmenu + 1;
      break;
    case NumMenuAttr:
      /* retour de la feuille de dialogue elle-meme */
      if (valmenu == 0 || valmenu == 2)
	/* fermeture de la feuille de dialogue */
	{
	  /* on detruit la feuille de dialogue */
	  TtaDestroyDialogue (NumMenuAttr);
	  AttrFormExists = FALSE;
	}
      act = valmenu;
      break;
    }

  if (act > 0)
    {
      /* ce n'est pas une simple fermeture de la feuille de dialogue */
      /* demande quelle est la selection courante */
      if (GetCurrentSelection (&SelDoc, &firstSel, &lastSel, &firstChar,
			       &lastChar))
	/* il y a bien une selection */
	{
	  GetAttribute (&pAttrNew);
	  if (SchCurrentAttr != NULL)
	    {
	      /* lock tables formatting */
	      if (ThotLocalActions[T_islock])
		{
		  (*ThotLocalActions[T_islock]) (&lock);
		  if (!lock)
		    {
		      doc = IdentDocument (SelDoc);
		      dispMode = TtaGetDisplayMode (doc);
		      if (dispMode == DisplayImmediately)
			TtaSetDisplayMode (doc, DeferredDisplay);
		      /* table formatting is not loked, lock it now */
		      (*ThotLocalActions[T_lock]) ();
		    }
		}
	      pAttrNew->AeAttrSSchema = SchCurrentAttr;
	      pAttrNew->AeAttrNum = NumCurrentAttr;
	      pAttrNew->AeDefAttr = FALSE;
	      pAttrNew->AeAttrType = SchCurrentAttr->SsAttribute[NumCurrentAttr - 1].AttrType;
	      
	      switch (pAttrNew->AeAttrType)
		{
		case AtNumAttr:
		  if (act == 2)
		    /* Supprimer l'attribut */
		    pAttrNew->AeAttrValue = MAX_INT_ATTR_VAL + 1;
		  else
		    /* la valeur saisie devient la valeur courante */
		    pAttrNew->AeAttrValue = NumAttrValue;
		  /* applique les attributs a la partie selectionnee */
		  AttachAttrToRange (pAttrNew, lastChar, firstChar, lastSel,
				     firstSel, SelDoc, TRUE);
		  break;
		    
		case AtTextAttr:
		  if (act == 2)
		    /* suppression de l'attribut */
		    pAttrNew->AeAttrText = NULL;
		  else
		    {
		      /* la valeur saisie devient la valeur courante */
		      if (pAttrNew->AeAttrText == NULL)
			GetTextBuffer (&(pAttrNew->AeAttrText));
		      else
			ClearText (pAttrNew->AeAttrText);
		      CopyStringToText (TextAttrValue, pAttrNew->AeAttrText,
					&lg);
		    }
		  /* applique les attributs a la partie selectionnee */
		  AttachAttrToRange (pAttrNew, lastChar, firstChar, lastSel,
				     firstSel, SelDoc, TRUE);
		  break;
		  
		case AtReferenceAttr:		    
		  break;
		  
		case AtEnumAttr:
		  if (act == 2)
		    /* suppression de l'attribut */
		    pAttrNew->AeAttrValue = 0;
		  else
		    /* la valeur choisie devient la valeur courante */
		    pAttrNew->AeAttrValue = NumAttrValue;
		  /* applique les attributs a la partie selectionnee */
		  AttachAttrToRange (pAttrNew, lastChar, firstChar, lastSel,
				     firstSel, SelDoc, TRUE);
		  break;
		  
		default:
		  break;
		}
	      if (!lock)
		{
		  /* unlock table formatting */
		  (*ThotLocalActions[T_unlock]) ();
		  if (dispMode == DisplayImmediately)
		    TtaSetDisplayMode (doc, DisplayImmediately);
		}
	      UpdateAttrMenu (SelDoc);
	    }
	  DeleteAttribute (NULL, pAttrNew);
	}
    }
}


/*----------------------------------------------------------------------
   CallbackAttrMenu 
   handles the callbacks of the "Attributes" menu: creates a
   form to capture the value of the chosen attribute.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CallbackAttrMenu (int refmenu, int att, int frame)
#else  /* __STDC__ */
void                CallbackAttrMenu (refmenu, att, frame)
int                 refmenu;
int                 att;
int                 frame;

#endif /* __STDC__ */
{
  TtAttribute        *pAttr;
  PtrAttribute        pAttrNew, currAttr;
  PtrDocument         SelDoc;
  PtrElement          firstSel, lastSel, AssocCreated;
  PtrReference        Ref;
  Document            doc;
  View                view;
  int                 item, i;
  int                 firstChar, lastChar;
#ifdef _WINDOWS
  int                 currAttrVal = -1;
#endif /* _WINDOWS */

  FrameToView (frame, &doc, &view);
  item = att;
  /* get the right entry in the attributes list */
  if (refmenu == EventMenu[frame - 1])
    att = AttrEventNumber[att];
  else
    {
      i = 0;
      while (i <= att)
	{
	  if (AttrEvent[i])
	    att++;
	  i++;
	}
    }
  if (att >= 0)
    if (GetCurrentSelection (&SelDoc, &firstSel, &lastSel, &firstChar,
			     &lastChar))
      {
	GetAttribute (&pAttrNew);
	pAttrNew->AeAttrSSchema = AttrStruct[att];
	pAttrNew->AeAttrNum = AttrNumber[att];
	pAttrNew->AeDefAttr = FALSE;
	pAttr = &AttrStruct[att]->SsAttribute[AttrNumber[att] - 1];
	pAttrNew->AeAttrType = pAttr->AttrType;
	if (pAttr->AttrType == AtReferenceAttr)
	  {
	    AssocCreated = NULL;
	    /* attache un bloc reference a l'attribut */
	    GetReference (&Ref);
	    pAttrNew->AeAttrReference = Ref;
	    pAttrNew->AeAttrReference->RdElement = NULL;
	    pAttrNew->AeAttrReference->RdAttribute = pAttrNew;
	    /* demande a l'utilisateur l'element reference' */
	    if (LinkReference (firstSel, pAttrNew, SelDoc, &AssocCreated))
	      if (ThotLocalActions[T_checkextens] != NULL)
		(*ThotLocalActions[T_checkextens])
		  (pAttrNew, firstSel, lastSel, FALSE);
	    if (AssocCreated != NULL)
	      {
		CreateAllAbsBoxesOfEl (AssocCreated, SelDoc);
		AbstractImageUpdated (SelDoc);
	      }
	    /* applique l'attribut a la partie selectionnee */
	    AttachAttrToRange (pAttrNew, lastChar, firstChar, lastSel,
			       firstSel, SelDoc, TRUE);
	    UpdateAttrMenu (SelDoc);
	  }
	else
	  {
	    /* cherche la valeur de cet attribut pour le premier element */
	    /* selectionne' */
	    currAttr = AttributeValue (firstSel, pAttrNew);
#ifdef _WINDOWS
	    if (currAttr == NULL)
	      currAttrVal = -1;
	    else 
	      currAttrVal = currAttr->AeAttrValue;
#endif /* _WINDOWS */
	    if (pAttrNew->AeAttrNum == 1)
	      {
		InitFormLanguage (doc, view, firstSel, currAttr);
		/* memorise l'attribut concerne' par le formulaire */
		SchCurrentAttr = pAttrNew->AeAttrSSchema;
		NumCurrentAttr = 1;
		/* restaure l'etat courant du toggle */
#ifdef _WINDOWS
		CreateLanguageDlgWindow (TtaGetViewFrame (doc, view), 
					 TtaGetMessage (LIB, TMSG_LANGUAGE),
					 TtaGetMessage (LIB, TMSG_LANG_OF_EL), 
					 WIN_nbItem, WIN_buffMenu, WIN_Lab, 
					 (int)WIN_Language, TtaGetLanguageName (WIN_Language));
#else /* _WINDOWS */
		if (ActiveAttr[item] == 0)
		  TtaSetToggleMenu (refmenu, item, FALSE);
		else
		  TtaSetToggleMenu (refmenu, item, TRUE);
#endif /* _WINDOWS */
	      }
	    else if (pAttr->AttrType == AtEnumAttr &&
		     pAttr->AttrNEnumValues == 1)
	      /* attribut enumere' a une seule valeur(attribut booleen) */
	      {
		if (currAttr == NULL)
		  /* le premier element selectionne' n'a pas cet */
		  /* attribut. On le lui met */
		  pAttrNew->AeAttrValue = 1;
		else
		  /* suppression de l'attribut */
		  pAttrNew->AeAttrValue = 0;
		/* applique l'operation a la partie selectionnee */
		AttachAttrToRange (pAttrNew, lastChar, firstChar, lastSel,
				   firstSel, SelDoc, TRUE);
	      }
	    else
	      {
		/* construit le formulaire de saisie de la valeur de */
		/* l'attribut */
		MenuValues (pAttr, FALSE, currAttr, SelDoc, view);
		/* memorise l'attribut concerne' par le formulaire */
		SchCurrentAttr = AttrStruct[att];
		NumCurrentAttr = AttrNumber[att];
		/* restaure l'etat courant du toggle */
#ifndef _WINDOWS 
		if (ActiveAttr[item] == 0)
		  TtaSetToggleMenu (refmenu, item, FALSE);
		else
		  TtaSetToggleMenu (refmenu, item, TRUE);
#endif /* _WINDOWS */
		/* et memorise l'attribut en cours de traitement */
		CurrentAttr = att;
		/* affiche le formulaire */
#ifndef _WINDOWS
		TtaShowDialogue (NumMenuAttr, TRUE);
#else  /* _WINDOWS */
		if (WIN_AtNumAttr) 
		  WIN_InitNumAttrDialog (TtaGetViewFrame (doc, view),
					 TtaGetMessage (LIB, TMSG_ATTR));
		else if (WIN_AtTextAttr && !isForm) 
		  WIN_InitSheetDialog (TtaGetViewFrame (doc, view),
				       TtaGetMessage (LIB, TMSG_ATTR));
		else if (WIN_AtEnumAttr) 
                  CreateAttributeDlgWindow (WIN_title, currAttrVal,nbDlgItems);
#endif /* _WINDOWS */
	      }
	    DeleteAttribute (NULL, pAttrNew);
	  }
      }
}

/*----------------------------------------------------------------------
   CallbackLanguageMenu
   handles the callbacks of the Language form.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CallbackLanguageMenu (int ref, int val, STRING txt)
#else  /* __STDC__ */
void                CallbackLanguageMenu (ref, val, txt)
int                 ref;
int                 val;
STRING              txt;

#endif /* __STDC__ */
{
  Language		i;

  switch (ref)
    {
    case NumSelectLanguage:
      /* retour de la langue choisie par l'utilisateur */
      if (txt == NULL)
	TextAttrValue[0] = WC_EOS;
      else
	{
	  i = TtaGetLanguageIdFromName (txt);
	  ustrncpy (TextAttrValue, TtaGetLanguageCode (i), LgMaxAttrText);
	}
#ifndef _WINDOWS 
      TtaNewLabel (NumLabelHeritedLanguage, NumFormLanguage, "");
#endif /* _WINDOWS */
      CallbackValAttrMenu (NumMenuAttr, 1, NULL);
      break;
    case NumFormLanguage:
      /* retour du formulaire lui-meme */
      switch (val)
	{
	case 0:
	  /* abandon du formulaire */
	  break;
	case 1:
	case 2:
	  /* appliquer la nouvelle valeur */
	  CallbackValAttrMenu (NumMenuAttr, val, NULL);
	  break;
	}
      break;
    }
}

/*----------------------------------------------------------------------
   AttributeMenuLoadResources
   connects the local actions.
  ----------------------------------------------------------------------*/
void                AttributeMenuLoadResources ()
{
  int i;

  if (ThotLocalActions[T_chattr] == NULL)
    {
      /* Connecte les actions de selection */
      TteConnectAction (T_chattr, (Proc) UpdateAttrMenu);
      TteConnectAction (T_rattr, (Proc) CallbackAttrMenu);
      TteConnectAction (T_rattrval, (Proc) CallbackValAttrMenu);
      TteConnectAction (T_rattrlang, (Proc) CallbackLanguageMenu);
      TteConnectAction (T_attrreq, (Proc) BuildReqAttrMenu);
      TteConnectAction (T_rattrreq, (Proc) CallbackReqAttrMenu);
      for (i = 0; i < MAX_FRAME; i++)
	EventMenu[i] = 0;
    }
}

