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
 * Author: I. Vatton (INRIA)
 *         R. Guetari (W3C/INRIA): Windows NT/95
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
static boolean      AttrFormExists = FALSE;
static boolean      MandatoryAttrFormExists = FALSE;
#ifdef _WINDOWS
static boolean      dlgInitialized = FALSE; 
static char         WIN_buffMenu [MAX_TXT_LEN];
static char         WIN_Lab [1024];
static int          WIN_nbItem;
#  endif /* _WINDOWS */

/* the menu attributes */
static PtrSSchema   AttrStruct[LgMaxAttributeMenu];
static int          AttrNumber[LgMaxAttributeMenu];
static boolean      AttrOblig[LgMaxAttributeMenu];

/* the attribute concerning the displayed input form */
static PtrSSchema   SchCurrentAttr = NULL;
static int          NumCurrentAttr = 0;
static int          ActiveAttr[100];
static int          CurrentAttr;
static int	    MenuAlphaLangValue;
/* return value of the input form */
static int          NumAttrValue;

#define LgMaxAttrText 500
static char         TextAttrValue[LgMaxAttrText];

/* required attributs context */
static PtrAttribute PtrReqAttr;

#ifdef _WINDOWS
#define ID_CONFIRM   1000
#define ID_DONE      1001
#define ID_APPLY     1002
#define ID_DELETE    1003
#define ID_EDITVALUE 1004

static TtAttribute* WIN_pAttr1;
static BOOL         wndRegistered;
static BOOL         wndSheetRegistered;
static BOOL         wndNumAttrRegistered;
static BOOL         WIN_AtNumAttr  = FALSE;
static BOOL         WIN_AtTextAttr = FALSE;
static BOOL         WIN_AtEnumAttr = FALSE;
static BOOL         isForm         = FALSE ;
static PtrAttribute WIN_currAttr;
static char         formRange [100];
static char         attr_text [500];
static int          formValue;
static int          nbDlgItems ;

extern HINSTANCE hInstance;
extern LPCTSTR   iconID;
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

#include "appli_f.h"
#include "tree_f.h"
#include "attrpresent_f.h"
#include "attributes_f.h"
#include "structcreation_f.h"
#include "appdialogue_f.h"
#include "actions_f.h"
#include "callback_f.h"
#include "exceptions_f.h"
#include "memory_f.h"
#include "changeabsbox_f.h"
#include "structselect_f.h"
#include "selectmenu_f.h"
#include "structschema_f.h"
#include "content_f.h"
#include "applicationapi_f.h"
#include "language_f.h"

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
   int                 i, defItem, nbItem, nbLanguages, firstLanguage, length;
   char                bufMenu[MAX_TXT_LEN];
   char                string[MAX_TXT_LEN];
   char               *ptr;
   Language            language;
   Name                languageValue;
   char                Lab[200];
   PtrAttribute        pHeritAttr;
   PtrElement          pElAttr;

   /* c'est l'attribut Langue, on initialise le formulaire Langue */
   languageValue[0] = EOS;
   if (currAttr != NULL)
      if (currAttr->AeAttrText != NULL)
	 strncpy (languageValue, currAttr->AeAttrText->BuContent, MAX_NAME_LENGTH);

   /* cree le formulaire avec les deux boutons Appliquer et Supprimer */
   strcpy (bufMenu, TtaGetMessage (LIB, TMSG_APPLY));
   i = strlen (bufMenu) + 1;
   strcpy (&bufMenu[i], TtaGetMessage (LIB, TMSG_DEL));
#  ifndef _WINDOWS
   TtaNewSheet (NumFormLanguage, TtaGetViewFrame (doc, view),
     TtaGetMessage (LIB, TMSG_LANGUAGE), 2, bufMenu, FALSE, 2, 'L', D_DONE);
#  endif /* !_WINDOWS */
   /* construit le selecteur des Langues */
#  ifdef _WINDOWS
   ptr = &WIN_buffMenu [0];
#  else  /* _WINDOWS */
   ptr = &bufMenu[0];
#  endif /* _WINDOWS */
   nbItem = 0;
   defItem = -1;
   nbLanguages = TtaGetNumberOfLanguages ();
   firstLanguage = TtaGetFirstUserLanguage ();
   for (language = firstLanguage; language < nbLanguages; language++)
     {
	strcpy (string, TtaGetLanguageName (language));
	length = strlen (string);
	if (length > 0)
	  {
	     if (defItem < 0)
	        if (strcasecmp(TtaGetLanguageCode(language), languageValue) == 0)
		  {
		  defItem = nbItem;
		  strcpy (languageValue, string);
		  }
	     nbItem++;
	     strcpy (ptr, string);
	     ptr += length + 1;
	  }
     }

   if (nbItem == 0)
     {
	/* pas de langue definie, on cree une simple zone de saisie de texte */
       MenuAlphaLangValue = 0;
#   ifndef _WINDOWS 
	TtaNewTextForm (NumSelectLanguage, NumFormLanguage,
			TtaGetMessage (LIB, TMSG_LANGUAGE), 30, 1, FALSE);
	TtaSetTextForm (NumFormLanguage, languageValue);
#   endif /* !_WINDOWS */
	
     }
   else
      /* on cree un selecteur */
     {
	if (nbItem >= 6)
	   length = 6;
	else
	   length = nbItem;
#   ifndef _WINDOWS 
	TtaNewSelector (NumSelectLanguage, NumFormLanguage,
		      TtaGetMessage (LIB, TMSG_LANG_OF_EL), nbItem, bufMenu,
			length, NULL, TRUE, TRUE);
#   endif /* !_WINDOWS */
	if (languageValue[0] == EOS || defItem < 0)
	  {
	    TtaSetSelector (NumSelectLanguage, -1, NULL);
	    MenuAlphaLangValue = -1;
	    /* cherche la valeur heritee de l'attribut Langue */
	    strcpy (Lab, TtaGetMessage (LIB, TMSG_INHERITED_LANG));
	    pHeritAttr = GetTypedAttrAncestor (firstSel, 1, NULL, &pElAttr);
	    if (pHeritAttr != NULL)
	      if (pHeritAttr->AeAttrText != NULL)
		{
		  /* the attribute value is a RFC-1766 code. Convert it into */
		  /* a language name */
		  language = TtaGetLanguageIdFromName (pHeritAttr->AeAttrText->BuContent);
		  strcat (Lab, TtaGetLanguageName(language));
	 }
	  }
	else
	   /* initialise le selecteur sur l'entree correspondante a la valeur
	      courante de l'attribut langue. */
	  {
	    TtaSetSelector (NumSelectLanguage, defItem, languageValue);
	    MenuAlphaLangValue = TtaGetLanguageIdFromAlphabet(TtaGetAlphabet (defItem));
	    Lab[0] = EOS;
	  }
     }

   if (MenuAlphaLangValue == -1)
     MenuAlphaLangValue = TtaGetLanguageIdFromAlphabet(TtaGetAlphabet (language));
#  ifndef _WINDOWS 
   TtaNewLabel (NumLabelHeritedLanguage, NumFormLanguage, Lab);

   /* construction du menu alphabet */
   ptr = &bufMenu[0];
   nbItem = 0;
   nbLanguages = TtaGetFirstUserLanguage ();
   for (language = 0; language < nbLanguages; language++)
     {
	strcpy (string, TtaGetLanguageName (language));
	length = strlen (string);
	if (length > 0)
	  {
	    nbItem++;
	    sprintf(ptr, "T%s", string);
	    ptr += length + 2;
	  }
     }
   TtaNewSubmenu (NumMenuAlphaLanguage, NumFormLanguage, 0 ,TtaGetMessage (LIB, TMSG_CHAR_ENCODING), nbItem, bufMenu, NULL, TRUE);
   TtaSetMenuForm (NumMenuAlphaLanguage, MenuAlphaLangValue);	      

   /* affiche le formulaire */
   TtaShowDialogue (NumFormLanguage, TRUE);
#  else  /* _WINDOWS */
   sprintf (WIN_Lab, "%s", Lab);
   WIN_nbItem = nbItem; 
   /*
   CreateLanguageDlgWindow (TtaGetViewFrame (doc, view), TtaGetMessage (LIB, TMSG_LANGUAGE),
	                        TtaGetMessage (LIB, TMSG_LANG_OF_EL), nbItem, bufMenu, Lab, NumMenuAlphaLanguage);
							*/
#  endif /* _WINDOWS */
}

#ifdef _WINDOWS
/*----------------------------------------------------------------------
  WIN_InitFormDialog
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void WIN_InitFormDialog (HWND parent, char* title)
#else  /* __STDC__ */
static void WIN_InitFormDialog (parent, title)
HWND  parent;
char* title	;
#endif /* __STDC__ */
{
   WNDCLASS    wndFormClass ;
   static char szAppName[] = "FormClass" ;
   HWND        hwnFromDialog;
   MSG         msg;

   if (!wndRegistered) {
	  wndRegistered = TRUE;
      wndFormClass.style         = CS_HREDRAW | CS_VREDRAW ;
      wndFormClass.lpfnWndProc   = InitFormDialogWndProc ;
      wndFormClass.cbClsExtra    = 0 ;
      wndFormClass.cbWndExtra    = 0 ;
      wndFormClass.hInstance     = hInstance ;
      wndFormClass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
      wndFormClass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
      wndFormClass.hbrBackground = (HBRUSH) GetStockObject (LTGRAY_BRUSH) ;
      wndFormClass.lpszClassName = szAppName ;
      wndFormClass.lpszMenuName  = NULL ;

      if (IS_WIN95) {
         if (!RegisterWin95 (&wndFormClass))
            return (FALSE);
      } else if (!RegisterClass (&wndFormClass))
             return (FALSE);
   }

   hwnFromDialog = CreateWindow (szAppName, title,
                                 DS_MODALFRAME | WS_POPUP | 
                                 WS_VISIBLE | WS_CAPTION | WS_SYSMENU,
                                 ClickX, ClickY,
                                 335, 200,
                                 parent, NULL, hInstance, NULL) ;

   ShowWindow (hwnFromDialog, SW_SHOWNORMAL) ;
   UpdateWindow (hwnFromDialog) ;

   while (GetMessage (&msg, NULL, 0, 0)) {
         TranslateMessage (&msg) ;
         DispatchMessage (&msg) ;
   }
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
	HWND        hwnTitle;
	HWND        confirmButton;
	HWND        doneButton;
	char*       pBuffer;
	/* char        attr_text [500]; */
	int         i, index;
	static PSTR pWinBuffer;
	static int  txtLength;
	static HWND hwnEdit ;

	if (WIN_currAttr)
	   sprintf (attr_text, WIN_currAttr->AeAttrText->BuContent);
	else
		attr_text [0] = EOS ;

    switch (iMsg) {
	       case WM_CREATE:
			    /* Create static window for the title */
			    hwnTitle = CreateWindow ("STATIC", WIN_pAttr1->AttrName, 
					                     WS_CHILD | WS_VISIBLE | SS_LEFT,
										 10, 10, 100, 15, hwnd, (HMENU) 99, 
										 ((LPCREATESTRUCT) lParam)->hInstance, NULL); 

			    /* Create Edit Window autoscrolled */
				hwnEdit = CreateWindow ("EDIT", attr_text, 
					                    WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL |
										WS_BORDER | ES_LEFT | ES_MULTILINE |
										ES_AUTOHSCROLL | ES_AUTOVSCROLL,
										10, 30, 310, 110, hwnd, (HMENU) 1, ((LPCREATESTRUCT) lParam)->hInstance, NULL);
				/* Create Confirm button */
                confirmButton = CreateWindow ("BUTTON", TtaGetMessage (LIB, TMSG_LIB_CONFIRM), 
                                              WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE,
                                              100, 150, 55, 20, hwnd, 
                                              (HMENU) ID_CONFIRM, ((LPCREATESTRUCT) lParam)->hInstance, NULL) ;

				/* Create Done Button */
				doneButton = CreateWindow ("BUTTON", TtaGetMessage (LIB, TMSG_DONE), 
                                           WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE,
                                           175, 150, 55, 20, hwnd, 
                                           (HMENU) ID_DONE, ((LPCREATESTRUCT) lParam)->hInstance, NULL) ;
 
				return 0;

           case WM_DESTROY :
                PostQuitMessage (0) ;
                return 0 ;

		   case WM_COMMAND:
			    switch (LOWORD (wParam)) {
				       case ID_CONFIRM:
						    txtLength = GetWindowTextLength (hwnEdit);
							if ((pWinBuffer = (PSTR) TtaGetMemory (txtLength + 1))) {
							   GetWindowText (hwnEdit, pWinBuffer, txtLength + 1);
							   index = 0;
							   pBuffer = (char*) TtaGetMemory (txtLength + 1);
							   for (i = 0; i < txtLength; i ++) 
								   if (pWinBuffer [i] != '\r') {
									  pBuffer [index++] = pWinBuffer[i];
								   }

							   pBuffer [index] = EOS;
							} else {
								  pWinBuffer = "";	  
							      pBuffer = "";
							}

							ThotCallback (NumMenuAttrTextNeeded, STRING_DATA, pBuffer);
							ThotCallback (NumMenuAttrRequired, INTEGER_DATA, (char*) 1);
						    DestroyWindow (hwnd);
						    break;
					   case ID_DONE:
							ThotCallback (NumMenuAttrRequired, INTEGER_DATA, (char*) 0);
						    DestroyWindow (hwnd);
						    /* Traitement ID_DONE */
						    break;
				}
			    return 0;
    }
    return DefWindowProc (hwnd, iMsg, wParam, lParam) ;
}

/*----------------------------------------------------------------------
  WIN_InitSheetDialog
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void WIN_InitSheetDialog (HWND parent, char* title)
#else  /* __STDC__ */
static void WIN_InitSheetDialog (parent, title)
HWND  parent;
char* title	;
#endif /* __STDC__ */
{
   WNDCLASSEX    wndSheetClass ;
   static char szAppName[] = "SheetClass" ;
   HWND        hwnSheetDialog;
   MSG         msg;

   if (!wndSheetRegistered) {
	  wndSheetRegistered = TRUE;
      wndSheetClass.style         = CS_HREDRAW | CS_VREDRAW ;
      wndSheetClass.lpfnWndProc   = InitSheetDialogWndProc ;
      wndSheetClass.cbClsExtra    = 0 ;
      wndSheetClass.cbWndExtra    = 0 ;
      wndSheetClass.hInstance     = hInstance ;
      wndSheetClass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
      wndSheetClass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
      wndSheetClass.hbrBackground = (HBRUSH) GetStockObject (LTGRAY_BRUSH) ;
      wndSheetClass.lpszMenuName  = NULL ;
      wndSheetClass.lpszClassName = szAppName ;
      wndSheetClass.cbSize        = sizeof(WNDCLASSEX);
      wndSheetClass.hIconSm       = LoadIcon (hInstance, iconID) ;

	  if (!RegisterClassEx (&wndSheetClass))
         return (FALSE);
   }

   hwnSheetDialog = CreateWindow (szAppName, title,
                                 DS_MODALFRAME | WS_POPUP | 
                                 WS_VISIBLE | WS_CAPTION | WS_SYSMENU,
                                 ClickX, ClickY,
                                 335, 200,
                                 parent, NULL, hInstance, NULL) ;

   ShowWindow (hwnSheetDialog, SW_SHOWNORMAL) ;
   UpdateWindow (hwnSheetDialog) ;

   while (GetMessage (&msg, NULL, 0, 0)) {
         TranslateMessage (&msg) ;
         DispatchMessage (&msg) ;
   }
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
	HWND        hwnTitle;
	HWND        applyButton;
	HWND        deleteButton;
	HWND        doneButton;
	char*       pBuffer;
	int         i, index;
	static PSTR pWinBuffer;
	static int  txtLength;
	static HWND hwnEdit ;

	if (!dlgInitialized) {
       if (WIN_currAttr) 
          sprintf (attr_text, "%s", WIN_currAttr->AeAttrText->BuContent);
       else
          attr_text [0] = EOS ;
       dlgInitialized = TRUE;
    }

    switch (iMsg) {
	       case WM_CREATE:
			    /* Create static window for the title */
			    hwnTitle = CreateWindow ("STATIC", WIN_pAttr1->AttrName, 
					                     WS_CHILD | WS_VISIBLE | SS_LEFT,
										 10, 10, 100, 15, hwnd, (HMENU) 99, 
										 ((LPCREATESTRUCT) lParam)->hInstance, NULL); 

			    /* Create Edit Window autoscrolled */
				hwnEdit = CreateWindow ("EDIT", attr_text, 
					                    WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL |
										WS_BORDER | ES_LEFT | ES_MULTILINE |
										ES_AUTOHSCROLL | ES_AUTOVSCROLL,
										10, 30, 310, 110, hwnd, (HMENU) 1, ((LPCREATESTRUCT) lParam)->hInstance, NULL);
				/* Create Apply button */
                applyButton = CreateWindow ("BUTTON", TtaGetMessage (LIB, TMSG_APPLY), 
                                            WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE,
                                            70, 150, 55, 20, hwnd, 
                                            (HMENU) ID_APPLY, ((LPCREATESTRUCT) lParam)->hInstance, NULL) ;

				/* Create Delete Button */
				deleteButton = CreateWindow ("BUTTON", TtaGetMessage (LIB, TMSG_DEL), 
                                           WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE,
                                           130, 150, 55, 20, hwnd, 
                                           (HMENU) ID_DELETE, ((LPCREATESTRUCT) lParam)->hInstance, NULL) ;
 
				/* Create Done Button */
				doneButton = CreateWindow ("BUTTON", TtaGetMessage (LIB, TMSG_DONE), 
                                           WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE,
                                           190, 150, 55, 20, hwnd, 
                                           (HMENU) ID_DONE, ((LPCREATESTRUCT) lParam)->hInstance, NULL) ;
				return 0;

           case WM_DESTROY :
                PostQuitMessage (0) ;
                return 0 ;

		   case WM_COMMAND:
			    switch (LOWORD (wParam)) {
				       case ID_APPLY:
						    txtLength = GetWindowTextLength (hwnEdit);
							if ((pWinBuffer = (PSTR) TtaGetMemory (txtLength + 1))) {
							   GetWindowText (hwnEdit, pWinBuffer, txtLength + 1);
							   index = 0;
							   pBuffer = (char*) TtaGetMemory (txtLength + 1);
							   for (i = 0; i < txtLength; i ++) 
								   if (pWinBuffer [i] != '\r') {
									  pBuffer [index++] = pWinBuffer[i];
								   }

							   pBuffer [index] = EOS;
							} else {
								  pWinBuffer = "";	  
							      pBuffer = "";
							}

							ThotCallback (NumMenuAttrText, STRING_DATA, pBuffer);
							ThotCallback (NumMenuAttr, INTEGER_DATA, (char*) 1);
						    break;

					   case ID_DELETE:
						    txtLength = GetWindowTextLength (hwnEdit);
							if ((pWinBuffer = (PSTR) TtaGetMemory (txtLength + 1))) {
							   GetWindowText (hwnEdit, pWinBuffer, txtLength + 1);
							   index = 0;
							   pBuffer = (char*) TtaGetMemory (txtLength + 1);
							   for (i = 0; i < txtLength; i ++) 
								   if (pWinBuffer [i] != '\r') {
									  pBuffer [index++] = pWinBuffer[i];
								   }

							   pBuffer [index] = EOS;
							} else {
								  pWinBuffer = "";	  
							      pBuffer = "";
							}

							ThotCallback (NumMenuAttrText, STRING_DATA, pBuffer);
							ThotCallback (NumMenuAttr, INTEGER_DATA, (char*) 2);
						    break;

					   case ID_DONE:
							ThotCallback (NumMenuAttr, INTEGER_DATA, (char*) 0);
						    DestroyWindow (hwnd);
							dlgInitialized = FALSE;
						    break;
				}
			    return 0;
    }
    return DefWindowProc (hwnd, iMsg, wParam, lParam) ;
}

/*----------------------------------------------------------------------
  WIN_InitNumAttrDialog
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void WIN_InitNumAttrDialog (HWND parent, char* title)
#else  /* __STDC__ */
static void WIN_InitNumAttrDialog (parent, title)
HWND  parent;
char* title	;
#endif /* __STDC__ */
{
   WNDCLASSEX    wndNumAttrClass ;
   static char szAppName[] = "NumAttrClass" ;
   HWND        hwnNumAttrDialog;
   MSG         msg;

   if (!wndNumAttrRegistered) {
	  wndNumAttrRegistered = TRUE;
      wndNumAttrClass.style         = CS_HREDRAW | CS_VREDRAW ;
      wndNumAttrClass.lpfnWndProc   = InitNumAttrDialogWndProc ;
      wndNumAttrClass.cbClsExtra    = 0 ;
      wndNumAttrClass.cbWndExtra    = 0 ;
      wndNumAttrClass.hInstance     = hInstance ;
      wndNumAttrClass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
      wndNumAttrClass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
      wndNumAttrClass.hbrBackground = (HBRUSH) GetStockObject (LTGRAY_BRUSH) ;
      wndNumAttrClass.lpszMenuName  = NULL ;
      wndNumAttrClass.lpszClassName = szAppName ;
      wndNumAttrClass.cbSize        = sizeof(WNDCLASSEX);
      wndNumAttrClass.hIconSm       = LoadIcon (hInstance, iconID) ;

      if (!RegisterClassEx (&wndNumAttrClass))
         return (FALSE);
   }

   hwnNumAttrDialog = CreateWindow (szAppName, title,
                                    DS_MODALFRAME | WS_POPUP | 
                                    WS_VISIBLE | WS_CAPTION | WS_SYSMENU,
                                    ClickX, ClickY,
                                    180, 150,
                                    parent, NULL, hInstance, NULL) ;

   ShowWindow (hwnNumAttrDialog, SW_SHOWNORMAL) ;
   UpdateWindow (hwnNumAttrDialog) ;

   while (GetMessage (&msg, NULL, 0, 0)) {
         TranslateMessage (&msg) ;
         DispatchMessage (&msg) ;
   }
}

/*----------------------------------------------------------------------
 InitSheetDialogWndProc
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
	static PSTR pWinBuffer;
	static int  txtLength;
	static HWND hwnEdit ;

    switch (iMsg) {
	       case WM_CREATE:
			    /* Create static window for the title */
			    hwnTitle = CreateWindow ("STATIC", WIN_pAttr1->AttrName, 
					                     WS_CHILD | WS_VISIBLE | SS_LEFT,
										 10, 10, 160, 15, hwnd, (HMENU) 1,
										 ((LPCREATESTRUCT) lParam)->hInstance, NULL); 

			    hwnRange = CreateWindow ("STATIC", formRange, 
					                     WS_CHILD | WS_VISIBLE | SS_LEFT,
										 10, 35, 160, 15, hwnd, (HMENU) 2, 
										 ((LPCREATESTRUCT) lParam)->hInstance, NULL); 

			    /* Create Edit Window autoscrolled */
				hwnEdit = CreateWindow ("EDIT", NULL, 
					                    WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
										10, 60, 160, 20, hwnd, (HMENU) ID_EDITVALUE, ((LPCREATESTRUCT) lParam)->hInstance, NULL);
				SetDlgItemInt (hwnd, ID_EDITVALUE, formValue, TRUE);

				/* Create Apply button */
                applyButton = CreateWindow ("BUTTON", TtaGetMessage (LIB, TMSG_APPLY), 
                                            WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE,
                                            10, 90, 50, 20, hwnd, 
                                            (HMENU) ID_APPLY, ((LPCREATESTRUCT) lParam)->hInstance, NULL) ;

				/* Create Delete Button */
				deleteButton = CreateWindow ("BUTTON", TtaGetMessage (LIB, TMSG_DEL), 
                                             WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE,
                                             65, 90, 50, 20, hwnd, 
                                             (HMENU) ID_DELETE, ((LPCREATESTRUCT) lParam)->hInstance, NULL) ;
 
				/* Create Done Button */
				doneButton = CreateWindow ("BUTTON", TtaGetMessage (LIB, TMSG_DONE), 
                                           WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE,
                                           120, 90, 50, 20, hwnd, 
                                           (HMENU) ID_DONE, ((LPCREATESTRUCT) lParam)->hInstance, NULL) ;
				return 0;

           case WM_DESTROY :
                PostQuitMessage (0) ;
                return 0 ;

		   case WM_COMMAND:
			    switch (LOWORD (wParam)) {
				       case ID_APPLY:
						    val = GetDlgItemInt (hwnd, ID_EDITVALUE, &ok, TRUE);
							if (ok) {
						       ThotCallback (NumMenuAttrNumber, INTEGER_DATA, (char*) val);
						       ThotCallback (NumMenuAttr, INTEGER_DATA, (char*) 1);
							}
						    break;

					   case ID_DELETE:
						    val = GetDlgItemInt (hwnd, ID_EDITVALUE, &ok, TRUE);
							if (ok) {
						       ThotCallback (NumMenuAttrNumber, INTEGER_DATA, (char*) val);
						       ThotCallback (NumMenuAttr, INTEGER_DATA, (char*) 2);
							}
						    break;

					   case ID_DONE:
						    ThotCallback (NumMenuAttr, INTEGER_DATA, (char*) 0);
						    DestroyWindow (hwnd);
						    break;
				}
			    return 0;
    }
    return DefWindowProc (hwnd, iMsg, wParam, lParam) ;
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
static void         MenuValues (TtAttribute * pAttr1, boolean required, PtrAttribute currAttr,
				PtrDocument pDoc, int view)
#else  /* __STDC__ */
static void         MenuValues (pAttr1, required, currAttr, pDoc, view)
TtAttribute        *pAttr1;
boolean             required;
PtrAttribute        currAttr;
PtrDocument         pDoc;
int                 view;

#endif /* __STDC__ */

{
  int                 i, lgmenu, val;
  int                 form, subform;
  char                title[MAX_NAME_LENGTH + 2];
  char                bufMenu[MAX_TXT_LEN];
  Document            doc;

#ifdef _WINDOWS
  WIN_pAttr1 = pAttr1;
  WIN_currAttr	= currAttr;
#endif /* _WINDOWS */

  doc = (Document) IdentDocument (pDoc);
  /* detruit la feuille de dialogue et la recree */
  strcpy (bufMenu, TtaGetMessage (LIB, TMSG_APPLY));
  i = strlen (bufMenu) + 1;
  strcpy (&bufMenu[i], TtaGetMessage (LIB, TMSG_DEL));
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
      isForm = TRUE ;
      WIN_InitFormDialog (TtaGetViewFrame (doc, view), TtaGetMessage (LIB, TMSG_ATTR));
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
		   TtaGetMessage (LIB, TMSG_ATTR), 2, bufMenu, FALSE, 2, 'L', D_DONE);
#else  /* _WINDOWS */
      isForm = FALSE ;
#endif /* _WINDOWS */
      AttrFormExists = TRUE;
    }

   strncpy (title, pAttr1->AttrName, MAX_NAME_LENGTH);
   switch (pAttr1->AttrType)
     {
     case AtNumAttr:
       /* attribut a valeur numerique */
       subform = form + 1;
#ifndef _WINDOWS
       TtaNewNumberForm (subform, form, title, -MAX_INT_ATTR_VAL, MAX_INT_ATTR_VAL, TRUE);
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
       sprintf (formRange, "%d .. %d", -MAX_INT_ATTR_VAL, MAX_INT_ATTR_VAL); 
       formValue = i;
#endif /* _WINDOWS */
       break;

     case AtTextAttr:
       /* attribut a valeur textuelle */
       subform = form + 2;
#ifndef _WINDOWS
       TtaNewTextForm (subform, form, title, 40, 3, FALSE);
       TtaAttachForm (subform);
       if (currAttr == NULL)
	 TtaSetTextForm (subform, "");
       else if (currAttr->AeAttrText == NULL)
	 TtaSetTextForm (subform, "");
       else
	 TtaSetTextForm (subform, currAttr->AeAttrText->BuContent);
#else  /* _WINDOWS */
       WIN_AtNumAttr  = FALSE;
       WIN_AtTextAttr = TRUE;
       WIN_AtEnumAttr = FALSE;
#endif /* _WINDOWS */
       break;

     case AtEnumAttr:
       /* attribut a valeurs enumerees */
       subform = form + 3;
       /* cree un menu de toutes les valeurs possibles de l'attribut */
       lgmenu = 0;
       val = 0;
       /* boucle sur les valeurs possibles de l'attribut */
       while (val < pAttr1->AttrNEnumValues)
	 {
	   i = strlen (pAttr1->AttrEnumValue[val]) + 2;	/* for 'B' and EOS */
	   if (lgmenu + i < MAX_TXT_LEN)
	     {
	       bufMenu[lgmenu] = 'B';
	       strcpy (&bufMenu[lgmenu + 1], pAttr1->AttrEnumValue[val]);
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

     case AtReferenceAttr:
       /* attribut reference, on ne fait rien */
       break;

     default:
       break;
     }
}


/*----------------------------------------------------------------------
   CallbackReqAttrMenu
   handles the callback of the menu which captures the required attributes.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CallbackReqAttrMenu (int ref, int val, char *txt)
#else  /* __STDC__ */
void                CallbackReqAttrMenu (ref, val, txt)
int                 ref;
int                 val;
char               *txt;

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
#  ifndef _WINDOWS
   TtaShowDialogue (NumMenuAttrRequired, FALSE);
#  endif /* !_WINDOWS */
   TtaWaitShowDialogue ();
}


/*----------------------------------------------------------------------
   TteItemMenuAttr 
   sends the AttrMenu.Pre message which indicates that the editor
   is going to add to the Attributes menu an item for the creation
   of an attribute of type (pSS, att) for the pEl element. It 
   returns the answer from the application.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean      TteItemMenuAttr (PtrSSchema pSS, int att, PtrElement pEl, PtrDocument pDoc)
#else  /* __STDC__ */
static boolean      TteItemMenuAttr (pSS, att, pEl, pDoc)
PtrSSchema          pSS;
int                 att;
PtrElement          pEl;
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   NotifyAttribute     notifyAttr;
   boolean             OK;

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
   builds the Attributes menu and returns the number of
   attributes added to the composite menu.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 BuildAttrMenu (char *bufMenu, PtrDocument pDoc, int ActiveAttr[])
#else  /* __STDC__ */
int                 BuildAttrMenu (bufMenu, pDoc, ActiveAttr)
char               *bufMenu;
PtrDocument         pDoc;
int                 ActiveAttr[];
#endif /* __STDC__ */
{
   int                 i;
   PtrDocument         SelDoc;
   PtrElement          firstSel, lastSel, pEl;
   int                 firstChar, lastChar;
   boolean             selectionOK, isNew;
   int                 lgmenu;
   PtrSSchema          pSS;
   PtrAttribute        pAttrNew;
   int                 att, nbOfEntries;
   SRule              *pRe1;
   TtAttribute        *pAt2;
   char                tempBuffer[MAX_NAME_LENGTH + 1];
   PtrSSchema          pSchExt;

   nbOfEntries = 0;
   /* demande quelle est la selection courante */
   selectionOK = GetCurrentSelection (&SelDoc, &firstSel, &lastSel, &firstChar, &lastChar);
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
	     pSS = pEl->ElStructSchema;	/* schema de struct de l'element courant */
	     if (pSS == NULL)
		pEl = NULL;
	     else
		{
	        /* on parcourt toutes les extensions de schema de ce schema */
	        do
	          {
		  /* on a deja traite' ce schema de structure ? */
		  isNew = TRUE;
		  for (i = 1; i <= nbOfEntries; i++)	/* parcourt la table */
		     if (pSS == AttrStruct[i - 1])	/* deja dans la table */
			isNew = FALSE;
		  if (isNew)
		     /* l'element utilise un schema de structure pas encore */
		     /* rencontre' */
		     /* met tous les attributs globaux de ce schema dans la table */
		    {
		       att = 0;
		       while (att < pSS->SsNAttributes && nbOfEntries < LgMaxAttributeMenu
			      && nbOfEntries < MAX_ENTRY - 1)
			 {
			    att++;
			    /* on saute les attributs locaux */
			    if (pSS->SsAttribute[att - 1].AttrGlobal)
			       /* on saute les attributs ayant l'exception Invisible */
			       if (!AttrHasException (ExcInvisible, att, pSS))
				  /* on saute l'attribut Langue, sauf la 1ere fois */
				  if (nbOfEntries == 0 || att != 1)
				     if (TteItemMenuAttr (pSS, att, firstSel, SelDoc))
				       {
					  nbOfEntries++;
					  /* conserve le schema de structure et le numero */
					  /* d'attribut de cette nouvelle entree du menu */
					  AttrStruct[nbOfEntries - 1] = pSS;
					  AttrNumber[nbOfEntries - 1] = att;
					  AttrOblig[nbOfEntries - 1] = FALSE;
				       }
			 }
		    }
		  /* passe a l'extension de schema suivante */
		  pSS = pSS->SsNextExtens;
	          }
	        while (pSS != NULL);
	        /* passe a l'element ascendant */
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
		for (att = 1; att <= pRe1->SrNLocalAttrs; att++)
		   if (nbOfEntries < LgMaxAttributeMenu && nbOfEntries < MAX_ENTRY - 1)
		      if (!AttrHasException (ExcInvisible, pRe1->SrLocalAttr[att - 1], pSS))
			 if (TteItemMenuAttr (pSS, pRe1->SrLocalAttr[att - 1], firstSel,
					      SelDoc))
			   {
			      nbOfEntries++;
			      /* conserve le schema de structure et le numero */
			      /* d'attribut de cette nouvelle entree du menu */
			      AttrStruct[nbOfEntries - 1] = pSS;
			      AttrNumber[nbOfEntries - 1] = pRe1->SrLocalAttr[att - 1];
			      AttrOblig[nbOfEntries - 1] = pRe1->SrRequiredAttr[att - 1];
			   }
	     /* passe a l'extension suivante du schema du document */
	     pSchExt = pSchExt->SsNextExtens;
	     /* cherche dans cette extension de schema la regle d'extension pour */
	     /* le premier element selectionne' */
	     if (pSchExt != NULL)
	       {
		  pSS = pSchExt;
		  pRe1 = ExtensionRule (firstSel->ElStructSchema, firstSel->ElTypeNumber,
					pSchExt);
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
	     /* met les noms des attributs de la table dans le menu */
	     for (att = 0; att < nbOfEntries; att++)
	       {
		  pAt2 = &AttrStruct[att]->SsAttribute[AttrNumber[att] - 1];

		  pAttrNew->AeAttrSSchema = AttrStruct[att];
		  pAttrNew->AeAttrNum = AttrNumber[att];
		  pAttrNew->AeDefAttr = FALSE;
		  if (pAt2->AttrType == AtEnumAttr && pAt2->AttrNEnumValues == 1)
		     /* attribut enumere' a une seule valeur (attribut booleen) */
		     sprintf (tempBuffer, "T%s", pAt2->AttrName);
		  else
		     sprintf (tempBuffer, "T%s...", pAt2->AttrName);
		  /* met une etoile devant le nom de l'attribut s'il a une */
		  /* valeur pour la selection courante */
		  if (AttributeValue (firstSel, pAttrNew) != NULL)
		     ActiveAttr[att] = 1;
		  else
		     ActiveAttr[att] = 0;
		  i = strlen (tempBuffer) + 1;
		  if (lgmenu + i < MAX_TXT_LEN)
		     strcpy (&bufMenu[lgmenu], tempBuffer);
		  lgmenu += i;
	       }
	  }
	DeleteAttribute (NULL, pAttrNew);
     }
   return nbOfEntries;
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
   int                 nbItemAttr, i;
   char                bufMenuAttr[MAX_TXT_LEN];
   int                 view, menu, menuID;
   int                 frame, ref;
   Document            document;
   Menu_Ctl           *pMenu;

   /* Compose le menu des attributs */
   if (pDoc == SelectedDocument && !pDoc->DocReadOnly)
      nbItemAttr = BuildAttrMenu (bufMenuAttr, pDoc, ActiveAttr);
   else
      nbItemAttr = 0;

   document = (Document) IdentDocument (pDoc);
   /* Traite toutes les vues de l'arbre principal */
   for (view = 1; view <= MAX_VIEW_DOC; view++)
     {
	frame = pDoc->DocViewFrame[view - 1];
	if (frame != 0 && FrameTable[frame].MenuAttr != -1)
	  {
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
		   for (i = 0; i < nbOldItems; i ++) {
                       RemoveMenu (FrameTable[frame].WdMenus[menu], ref + i, MF_BYCOMMAND) ;
		   }
#endif /* _WINDOWS */
		  TtaNewPulldown (ref, FrameTable[frame].WdMenus[menu], NULL,
				  nbItemAttr, bufMenuAttr, NULL);
		  /* marque les attributs actifs */
		  for (i = 0; i < nbItemAttr; i++)
#            ifdef _WINDOWS
		     WIN_TtaSetToggleMenu (ref, i, (ActiveAttr[i] == 1), FrMainRef [frame]);
#            else  /* !_WINDOWS */
		     TtaSetToggleMenu (ref, i, (ActiveAttr[i] == 1));
#            endif /* _WINDOWS */
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
#            ifdef _WINDOWS
		     WIN_TtaSetToggleMenu (ref, i, (ActiveAttr[i] == 1), FrMainRef [frame]);
#            else  /* !_WINDOWS */
		     TtaSetToggleMenu (ref, i, (ActiveAttr[i] == 1));
#            endif /* _WINDOWS */
		  TtaSetMenuOn (document, view, menu);
	       }
	  }
     }
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
void                CallbackValAttrMenu (int ref, int valmenu, char *valtext)
#else  /* __STDC__ */
void                CallbackValAttrMenu (ref, valmenu, valtext)
int                 ref;
int                 valmenu;
char               *valtext;

#endif /* __STDC__ */
{
  PtrDocument         SelDoc;
  PtrElement          firstSel, lastSel;
  int                 firstChar, lastChar;
  PtrAttribute        pAttrNew;
  int                 lg, act;

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
      strncpy (TextAttrValue, valtext, LgMaxAttrText);
      act = 0;
      break;
    case NumMenuAttrEnum:
      /* numero de la valeur d'un attribut enumere' */
      NumAttrValue = valmenu + 1;
      break;
    case NumMenuAttr:
      /* retour de la feuille de dialogue elle-meme */
      if (valmenu == 0)
	/* fermeture de la feuille de dialogue */
	{
	  /* on detruit la feuille de dialogue */
	  TtaDestroyDialogue (NumMenuAttr);
	  AttrFormExists = FALSE;
	  act = 0;
	}
      else
	act = valmenu;
      break;
    }

  if (act > 0)
    {
      /* ce n'est pas une simple fermeture de la feuille de dialogue */
      /* demande quelle est la selection courante */
      if (GetCurrentSelection (&SelDoc, &firstSel, &lastSel, &firstChar, &lastChar))
	/* il y a bien une selection */
	{
	  GetAttribute (&pAttrNew);
	  if (SchCurrentAttr != NULL)
	    {
	      pAttrNew->AeAttrSSchema = SchCurrentAttr;
	      pAttrNew->AeAttrNum = NumCurrentAttr;
	      pAttrNew->AeDefAttr = FALSE;
	      pAttrNew->AeAttrType = SchCurrentAttr->
		SsAttribute[NumCurrentAttr - 1].AttrType;
	      
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
		  AttachAttrToRange (pAttrNew, lastChar, firstChar, lastSel, firstSel,
				     SelDoc, TRUE);
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
		      CopyStringToText (TextAttrValue,
					pAttrNew->AeAttrText, &lg);
		    }
		  /* applique les attributs a la partie selectionnee */
		  AttachAttrToRange (pAttrNew, lastChar, firstChar, lastSel, firstSel,
				     SelDoc, TRUE);
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
		  AttachAttrToRange (pAttrNew, lastChar, firstChar, lastSel, firstSel,
				     SelDoc, TRUE);
		  break;
		  
		default:
		  break;
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
   int                 firstChar, lastChar;
   PtrReference        Ref;
   Document            doc;
   View                view;

   FrameToView (frame, &doc, &view);

   if (att >= 0)
      if (GetCurrentSelection (&SelDoc, &firstSel, &lastSel, &firstChar, &lastChar))
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
		if (pAttrNew->AeAttrNum == 1)
		  {
		     InitFormLanguage (doc, view, firstSel, currAttr);
		     /* memorise l'attribut concerne' par le formulaire */
		     SchCurrentAttr = pAttrNew->AeAttrSSchema;
		     NumCurrentAttr = 1;
		     /* restaure l'etat courant du toggle */
#            ifdef _WINDOWS
             /* DeleteAttribute (NULL, pAttrNew); */
             CreateLanguageDlgWindow (TtaGetViewFrame (doc, view), TtaGetMessage (LIB, TMSG_LANGUAGE),
	                        TtaGetMessage (LIB, TMSG_LANG_OF_EL), WIN_nbItem, WIN_buffMenu, WIN_Lab, NumMenuAlphaLanguage);
#            else /* _WINDOWS */
		     if (ActiveAttr[att] == 0)
			TtaSetToggleMenu (refmenu, att, FALSE);
		     else
			TtaSetToggleMenu (refmenu, att, TRUE);
#           endif /* _WINDOWS */
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
#            ifndef _WINDOWS 
		     if (ActiveAttr[att] == 0)
                TtaSetToggleMenu (refmenu, att, FALSE);
		     else
                TtaSetToggleMenu (refmenu, att, TRUE);
#            endif /* _WINDOWS */
		     /* et memorise l'attribut en cours de traitement */
		     CurrentAttr = att;
		     /* affiche le formulaire */
#            ifndef _WINDOWS
		     TtaShowDialogue (NumMenuAttr, TRUE);
#            else  /* _WINDOWS */
			 if (WIN_AtNumAttr) {
		        WIN_InitNumAttrDialog (TtaGetViewFrame (doc, view), TtaGetMessage (LIB, TMSG_ATTR));
			 } else if (WIN_AtTextAttr && !isForm) {
		            WIN_InitSheetDialog (TtaGetViewFrame (doc, view), TtaGetMessage (LIB, TMSG_ATTR));
			 } else if (WIN_AtEnumAttr) {
				    if (nbDlgItems == 3)
                       CreateAlign1DlgWindow (TtaGetViewFrame (doc, view));
					else if (nbDlgItems == 5)
                         CreateAlign2DlgWindow (TtaGetViewFrame (doc, view));
			 }
#            endif /* _WINDOWS */
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
void                CallbackLanguageMenu (int ref, int val, char *txt)
#else  /* __STDC__ */
void                CallbackLanguageMenu (ref, val, txt)
int                 ref;
int                 val;
char               *txt;

#endif /* __STDC__ */
{
  Language		i;

  switch (ref)
    {
    case NumSelectLanguage:
      /* retour de la langue choisie par l'utilisateur */
      if (txt == NULL)
	TextAttrValue[0] = EOS;
      else
	{
	  i = TtaGetLanguageIdFromName (txt);
	  strncpy (TextAttrValue, TtaGetLanguageCode (i), LgMaxAttrText);
	  i = TtaGetLanguageIdFromAlphabet(TtaGetAlphabet (i));
	  if ((int)i != MenuAlphaLangValue)
	    {
	      TtaSetMenuForm (NumMenuAlphaLanguage, (int)i);
	      MenuAlphaLangValue = (int)i;
	    }
	}
#  ifndef _WINDOWS 
   TtaNewLabel (NumLabelHeritedLanguage, NumFormLanguage, "");
#  endif /* _WINDOWS */

      CallbackValAttrMenu (NumMenuAttr, 1, NULL);
      break;
    case NumMenuAlphaLanguage:
      /* retour de l'alphabet choisi par l'utilisateur */
      strncpy (TextAttrValue, TtaGetLanguageCode ((int)val), LgMaxAttrText);
      TtaSetSelector (NumSelectLanguage, -1, TtaGetMessage (LIB, TMSG_NO_LANGUAGE));
      CallbackValAttrMenu (NumMenuAttr, 1, NULL);
      MenuAlphaLangValue =  (int)val;
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
   if (ThotLocalActions[T_chattr] == NULL)
     {
	/* Connecte les actions de selection */
	TteConnectAction (T_chattr, (Proc) UpdateAttrMenu);
	TteConnectAction (T_rattr, (Proc) CallbackAttrMenu);
	TteConnectAction (T_rattrval, (Proc) CallbackValAttrMenu);
	TteConnectAction (T_rattrlang, (Proc) CallbackLanguageMenu);
	TteConnectAction (T_attrreq, (Proc) BuildReqAttrMenu);
	TteConnectAction (T_rattrreq, (Proc) CallbackReqAttrMenu);
     }
}
