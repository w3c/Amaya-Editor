/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2004
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * User interface for attributes
 *
 * Authors: I. Vatton (INRIA)
 *          R. Guetari (W3C/INRIA) - Windows version
 *
 */

#ifdef _WX
  #include "wx/wx.h"
#endif /* _WX */

#include "thot_gui.h"
#include "thot_sys.h"
#include "dialog.h"
#include "constmedia.h"
#include "typemedia.h"
#include "libmsg.h"
#include "message.h"
#include "constmenu.h"
#include "appdialogue.h"
#include "dialogapi.h"

#ifdef _WINGUI
  #include "winsys.h"
  #include "resource.h"
  #include "wininclude.h"
#endif /* _WINGUI */

#ifdef _WX
  #include "AmayaAttributePanel.h"
  #include "AmayaWindow.h"
  #include "AmayaPanel.h"
  #include "appdialogue_wx.h"
  #include "AmayaSubPanelManager.h"
#endif /* _WX */

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "select_tv.h"
#include "edit_tv.h"
#include "frame_tv.h"
#include "appdialogue_tv.h"

/* flags to show the existence of the TtAttribute forms*/
static ThotBool     AttrFormExists = FALSE;
static ThotBool     MandatoryAttrFormExists = FALSE;

#define LgMaxAttrText 500
static PtrDocument  DocCurrentAttr = NULL;
static int          NumCurrentAttr = 0;
static PtrSSchema   SchCurrentAttr = NULL;
static char         TextAttrValue[LgMaxAttrText];
static char         LangAttrValue[LgMaxAttrText];
static int          CurrentAttr;
/* return value of the input form */
static int          NumAttrValue;
/* main menu of attributes */
static PtrSSchema   AttrStruct[MAX_MENU * 2];
static int          AttrNumber[MAX_MENU * 2];
static int          ActiveAttr[MAX_MENU * 2];
static ThotBool     AttrOblig[MAX_MENU * 2];
static ThotBool     AttrEvent[MAX_MENU * 2];
/* submenu of event attributes */
static int          AttrEventNumber[MAX_MENU];
static int          ActiveEventAttr[MAX_MENU];
static int          EventMenu[MAX_FRAME];

/* required attributs context */
static PtrAttribute PtrReqAttr;
static PtrDocument  PtrDocOfReqAttr;

#ifdef _WINGUI
#define ID_CONFIRM   1000
#define ID_DONE      1001

extern WNDPROC      lpfnTextZoneWndProc ;
static char         WIN_buffMenu[MAX_TXT_LEN];
static char         WIN_Lab[200];
static char         formRange[100];
static char        *szAppName;
static ThotWindow   hwnEdit;
static TtAttribute *WIN_pAttr;
static ThotBool     wndRegistered;
static ThotBool     wndSheetRegistered;
static ThotBool     wndNumAttrRegistered;
static int          WIN_nbItem;
static int          formValue;
static int          nbDlgItems;
static int          WIN_Language;

extern HINSTANCE hInstance;
extern LPCTSTR   iconID;
extern UINT      subMenuID[MAX_FRAME];
#endif /* _WINGUI */

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
#include "schemas_f.h"
#include "selectmenu_f.h"
#include "structcommands_f.h"
#include "structcreation_f.h"
#include "structmodif_f.h"
#include "structselect_f.h"
#include "structschema_f.h"
#include "tree_f.h"
#include "uconvert_f.h"
#include "dialogapi_f.h"

/*----------------------------------------------------------------------
  InitFormLangue
  initializes a form for capturing the values of the Language attribute.
  ----------------------------------------------------------------------*/
static void InitFormLanguage (Document doc, View view,
			      PtrElement firstSel,
			      PtrAttribute currAttr)
{
   Language            language;
   PtrAttribute        pHeritAttr;
   PtrElement          pElAttr;
   char               *ptr;
   char                languageCode[MAX_TXT_LEN];
   char                label[200];
   int                 defItem, nbItem;
#if defined(_GTK) || defined(_WX)
   char                bufMenu[MAX_TXT_LEN];
   int                 i;
#endif /* _GTK || _WX */

   /* Initialize the language selector */
   languageCode[0] = EOS;
   if (currAttr && currAttr->AeAttrText)
     CopyBuffer2MBs (currAttr->AeAttrText, 0, (unsigned char*)languageCode,
		     MAX_TXT_LEN);
#ifdef _WINGUI
   ptr = GetListOfLanguages (WIN_buffMenu, MAX_TXT_LEN, languageCode,
			     &nbItem, &defItem);
#endif /* _WINGUI */
#ifdef _GTK
   /* generate the form with two buttons Apply Cancel */
   strcpy (bufMenu, TtaGetMessage (LIB, TMSG_APPLY));
   i = strlen (bufMenu) + 1;
   strcpy (&bufMenu[i], TtaGetMessage (LIB, TMSG_DEL_ATTR));
   TtaNewSheet (NumFormLanguage, TtaGetViewFrame (doc, view),
		TtaGetMessage (LIB, TMSG_LANGUAGE), 2, 
		bufMenu, FALSE, 2, 'L', D_DONE);
   /* construit le selecteur des Langues */
   ptr = GetListOfLanguages (bufMenu, MAX_TXT_LEN, languageCode, &nbItem, &defItem);
   if (nbItem > 0)
     {
       /* on cree un selecteur */
       if (nbItem >= 6)
	 i = 6;
       else
	 i = nbItem;
       TtaNewSelector (NumSelectLanguage, NumFormLanguage,
		       TtaGetMessage (LIB, TMSG_LANG_OF_EL), nbItem, bufMenu,
		       i, NULL, TRUE, FALSE);
     }
   if (defItem >= 0)
     TtaSetSelector (NumSelectLanguage, defItem, NULL);
   else if (ptr)
     TtaSetSelector (NumSelectLanguage, -1, ptr);
   else
     TtaSetSelector (NumSelectLanguage, -1, NULL);
#endif /* _GTK */
   
   if (languageCode[0] == EOS)
     {
       /* look for the inherited attribute value Language */
       strcpy (label, TtaGetMessage (LIB, TMSG_INHERITED_LANG));
       pHeritAttr = GetTypedAttrAncestor (firstSel, 1, NULL, &pElAttr);
       if (pHeritAttr && pHeritAttr->AeAttrText)
	 {
	   /* the attribute value is a RFC-1766 code. Convert it into */
	   /* a language name */
	   CopyBuffer2MBs (pHeritAttr->AeAttrText, 0,
			   (unsigned char*)languageCode, MAX_TXT_LEN);
	   language = TtaGetLanguageIdFromName (languageCode);
	   strcat (label, TtaGetLanguageName(language));
	 }
     }
   else
     label[0] = EOS;

#ifdef _WX
   ptr = GetListOfLanguages (bufMenu, MAX_TXT_LEN, languageCode, &nbItem, &defItem);
   AmayaPanelParams p;
   p.param1 = (void*)AmayaAttributePanel::wxATTR_ACTION_SETUPLANG;
   p.param2 = (void*)FALSE;
   p.param3 = (void*)ptr;
   p.param4 = (void*)label;
   p.param5 = (void*)bufMenu;
   p.param6 = (void*)nbItem;
   p.param7 = (void*)defItem;
   AmayaSubPanelManager::GetInstance()->SendDataToPanel( WXAMAYA_PANEL_ATTRIBUTE, p );
#endif /* _WX */

#ifdef _GTK
   TtaNewLabel (NumLabelHeritedLanguage, NumFormLanguage, label);
   /* affiche le formulaire */
   TtaShowDialogue (NumFormLanguage, TRUE);
#endif /* _GTK */
   
#ifdef _WINGUI
   sprintf (WIN_Lab, "%s", label);
   WIN_nbItem = nbItem; 
   WIN_Language = defItem;
#endif /* _WINGUI */
}

#ifdef _WINGUI
/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
LRESULT CALLBACK TextAttrProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg)
    {
    case WM_KEYDOWN:
      switch (wParam)
        {
        case VK_RETURN:
          SendMessage(GetParent (hwnd), WM_ENTER, 0, 0);
          return 0;
        }
      break;

    case WM_KEYUP:
    case WM_CHAR:
      switch (wParam)
        {
        case VK_RETURN:
          return 0;
        }
    }

  /* Call the original window procedure for default processing */
  return CallWindowProc (lpfnTextZoneWndProc, hwnd, msg, wParam, lParam);
}

/*----------------------------------------------------------------------
 InitFormDialogWndProc
  ----------------------------------------------------------------------*/
LRESULT CALLBACK InitFormDialogWndProc (ThotWindow hwnd, UINT iMsg,
					WPARAM wParam, LPARAM lParam)
{
  ThotWindow          hwnTitle;
  ThotWindow          confirmButton;
  ThotWindow          doneButton;
  int                 i;
  int                 txtLength;
  
  switch (iMsg)
    {
    case WM_INITDIALOG:
      SetWindowText (hwnd, TtaGetMessage (LIB, TMSG_ATTR));
      WIN_SetDialogfont (hwnd);
      hwnTitle = GetDlgItem (hwnd, IDC_ATTRNAME);
	  SetWindowText (hwnTitle, WIN_pAttr->AttrName);
      WIN_SetDialogfont (hwnTitle);

      hwnEdit = GetDlgItem (hwnd, ID_EDITVALUE);
      WIN_SetDialogfont (hwnEdit);
      SetDlgItemText (hwnd, ID_EDITVALUE, TextAttrValue);
     if (lpfnTextZoneWndProc == (WNDPROC) 0)
       lpfnTextZoneWndProc = (WNDPROC) SetWindowLong (hwnEdit, GWL_WNDPROC,
						      (DWORD) TextAttrProc);
      else
	SetWindowLong (hwnEdit, GWL_WNDPROC, (DWORD) TextAttrProc);
 	  
      /* Confirm button */
      confirmButton = GetDlgItem (hwnd, ID_CONFIRM);
	  SetWindowText (confirmButton, TtaGetMessage (LIB, TMSG_APPLY));
      WIN_SetDialogfont (confirmButton);

      /* Done Button */
      doneButton = GetDlgItem (hwnd, ID_DONE);
	  SetWindowText (doneButton, TtaGetMessage (LIB, TMSG_DONE));
      WIN_SetDialogfont (doneButton);

      SetFocus (hwnEdit);
      return FALSE;
      break;
	  
    case WM_DESTROY :
      PostQuitMessage (0);
      break;
      
    case WM_COMMAND:
      switch (LOWORD (wParam))
	{
	case ID_CONFIRM:
	  txtLength = GetWindowTextLength (hwnEdit);
	  if (txtLength >= LgMaxAttrText)
	    txtLength = LgMaxAttrText - 1;
	  GetWindowText (hwnEdit, TextAttrValue, txtLength + 1);
	  i = 0;
	  while (i < txtLength && TextAttrValue[i] != __CR__)
	    i++;
	  if (i < txtLength)
	    TextAttrValue[i] = EOS;
	  if (PtrReqAttr)
	  {
	    ThotCallback (NumMenuAttrTextNeeded, STRING_DATA, TextAttrValue);
	    ThotCallback (NumMenuAttrRequired, INTEGER_DATA, (char *) 1);
	  }
	  else
	    ThotCallback (NumMenuAttr, INTEGER_DATA, (char *) 1);

	  DestroyWindow (hwnd);
	  break;
	  
	case IDCANCEL:
	case ID_DONE:
	  ThotCallback (NumMenuAttrRequired, INTEGER_DATA, (char *) 1);
	  DestroyWindow (hwnd);
	  /* Traitement ID_DONE */
	  break;
	}
      break;
    default:
      return FALSE;
    }
  return TRUE;
}

/*----------------------------------------------------------------------
  InitSheetDialogWndProc
  ----------------------------------------------------------------------*/
LRESULT CALLBACK InitSheetDialogWndProc (ThotWindow hwnd, UINT iMsg,
					 WPARAM wParam, LPARAM lParam)
{
  ThotWindow      hwnTitle;
  ThotWindow      applyButton;
  ThotWindow      deleteButton;
  ThotWindow      doneButton;
  int             i;
  int             txtLength;

  switch (iMsg)
    {
    case WM_INITDIALOG:
      SetWindowText (hwnd, TtaGetMessage (LIB, TMSG_ATTR));
      WIN_SetDialogfont (hwnd);
      hwnTitle = GetDlgItem (hwnd, IDC_ATTRNAME);
	  SetWindowText (hwnTitle, WIN_pAttr->AttrName);
      WIN_SetDialogfont (hwnTitle);

      hwnEdit = GetDlgItem (hwnd, ID_EDITVALUE);
      WIN_SetDialogfont (hwnEdit);
      SetDlgItemText (hwnd, ID_EDITVALUE, TextAttrValue);
      if (lpfnTextZoneWndProc == (WNDPROC) 0)
       lpfnTextZoneWndProc = (WNDPROC) SetWindowLong (hwnEdit, GWL_WNDPROC,
						      (DWORD) TextAttrProc);
      else
	SetWindowLong (hwnEdit, GWL_WNDPROC, (DWORD) TextAttrProc);
 	  
      /* Apply button */
      applyButton = GetDlgItem (hwnd, ID_APPLY);
	  SetWindowText (applyButton, TtaGetMessage (LIB, TMSG_APPLY));
      WIN_SetDialogfont (applyButton);

      /* Delete Button */
      deleteButton = GetDlgItem (hwnd, ID_DELETE);
	  SetWindowText (deleteButton, TtaGetMessage (LIB, TMSG_DEL_ATTR));
      WIN_SetDialogfont (deleteButton);
 
      /* Done Button */
      doneButton = GetDlgItem (hwnd, ID_DONE);
	  SetWindowText (doneButton, TtaGetMessage (LIB, TMSG_DONE));
      WIN_SetDialogfont (doneButton);

      SetFocus (hwnEdit);
      return FALSE;
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
	  GetWindowText (hwnEdit, TextAttrValue, txtLength + 1);
	  i = 0;
	  while (i < txtLength && TextAttrValue[i] != __CR__)
	    i++;
	  if (i < txtLength)
	    TextAttrValue[i] = EOS;
	  ThotCallback (NumMenuAttr, INTEGER_DATA, (char *) 1);
	  break;
	  
	case ID_DELETE:
	  ThotCallback (NumMenuAttr, INTEGER_DATA, (char *) 2);
	  DestroyWindow (hwnd);
	  break;
	  
	 case IDCANCEL:
	 case ID_DONE:
	   ThotCallback (NumMenuAttr, INTEGER_DATA, (char *) 0);
	   DestroyWindow (hwnd);
	   break;
	}
      break;
    default:
      return FALSE;
    }
  return TRUE;
}

/*----------------------------------------------------------------------
  InitNumAttrDialogWndProc
  ----------------------------------------------------------------------*/
LRESULT CALLBACK InitNumAttrDialogWndProc (ThotWindow hwnd, UINT iMsg,
					   WPARAM wParam, LPARAM lParam)
{
  ThotWindow      hwnTitle;
  ThotWindow      applyButton;
  ThotWindow      deleteButton;
  ThotWindow      doneButton;
  ThotBool        ok;
  int             val;

  switch (iMsg)
    {
    case WM_INITDIALOG:
      SetWindowText (hwnd, TtaGetMessage (LIB, TMSG_ATTR));
	  hwnTitle = GetDlgItem (hwnd, IDC_ATTRNAME);
	  SetWindowText (hwnTitle, WIN_pAttr->AttrName);
      WIN_SetDialogfont (hwnTitle);

      hwnEdit = GetDlgItem (hwnd, ID_EDITVALUE);
      SetDlgItemInt (hwnd, ID_EDITVALUE, formValue, TRUE);
	  WIN_SetDialogfont (hwnEdit);
      if (lpfnTextZoneWndProc == (WNDPROC) 0)
	lpfnTextZoneWndProc = (WNDPROC) SetWindowLong (hwnEdit, GWL_WNDPROC,
						       (DWORD) TextAttrProc);
      else
	SetWindowLong (hwnEdit, GWL_WNDPROC, (DWORD) TextAttrProc);

      /* Apply button */
      applyButton = GetDlgItem (hwnd, ID_APPLY);
	  SetWindowText (applyButton, TtaGetMessage (LIB, TMSG_APPLY));
      WIN_SetDialogfont (applyButton);

      /* Delete Button */
      deleteButton = GetDlgItem (hwnd, ID_DELETE);
	  SetWindowText (deleteButton, TtaGetMessage (LIB, TMSG_DEL_ATTR));
      WIN_SetDialogfont (deleteButton);
 
      /* Done Button */
      doneButton = GetDlgItem (hwnd, ID_DONE);
	  SetWindowText (doneButton, TtaGetMessage (LIB, TMSG_DONE));
      WIN_SetDialogfont (doneButton);

      SetFocus (hwnEdit);
      return FALSE;
      break;

    case WM_DESTROY :
      PostQuitMessage (0);
      break;
    
    case WM_COMMAND:
      switch (LOWORD (wParam))
	{
	case ID_APPLY:
	  val = GetDlgItemInt (hwnd, ID_EDITVALUE, &ok, TRUE);
	  if (ok)
	    {
	      ThotCallback (NumMenuAttrNumber, INTEGER_DATA, (char *) val);
	      ThotCallback (NumMenuAttr, INTEGER_DATA, (char *) 1);
	      DestroyWindow (hwnd);
	    }
	  break;
 
    	case ID_DELETE:
	  val = GetDlgItemInt (hwnd, ID_EDITVALUE, &ok, TRUE);
	  if (ok)
	    {
	      ThotCallback (NumMenuAttrNumber, INTEGER_DATA, (char *) val);
	      ThotCallback (NumMenuAttr, INTEGER_DATA, (char *) 2);
	      DestroyWindow (hwnd);
	    }
	  break;
	  
	case IDCANCEL:
	case ID_DONE:
	  ThotCallback (NumMenuAttr, INTEGER_DATA, (char *) 0);
	  DestroyWindow (hwnd);
	  break;
	}
      break;
    default:
      return FALSE;
    }
  return TRUE;
}

#endif /* _WINGUI */

/*----------------------------------------------------------------------
   MenuValues
   builds the dialog box for capturing the values of the attribute defined
   by the pAttr rule.
   required specifies if it's a required attribute
   currAttr gives the current value of the attribute
  ----------------------------------------------------------------------*/
static void MenuValues (TtAttribute * pAttr, ThotBool required,
			PtrAttribute currAttr, PtrDocument pDoc, int view)
{
   Document          doc;
   char             *tmp;
   char              bufMenu[MAX_TXT_LEN];
   char             *title = NULL;
   int               i, lgmenu, val, buttons;
   int               form, subform;

#ifdef _WINGUI
   WIN_pAttr = pAttr;
#endif /* _WINGUI */
   doc = (Document) IdentDocument (pDoc);
   buttons = 0;
   strcpy (bufMenu, TtaGetMessage (LIB, TMSG_APPLY));
   buttons++;
   if (required)
     {
       form = NumMenuAttrRequired;
       if (MandatoryAttrFormExists)
	 TtaDestroyDialogue (NumMenuAttrRequired);
       MandatoryAttrFormExists = TRUE;
      }
   else
     {
       form = NumMenuAttr;
       if (AttrFormExists)
	 TtaDestroyDialogue (NumMenuAttr);
       AttrFormExists = TRUE;
       i = strlen (bufMenu) + 1;
       strcpy (&bufMenu[i], TtaGetMessage (LIB, TMSG_DEL_ATTR));
       buttons++;
     }
#ifdef _GTK
   TtaNewSheet (form, TtaGetViewFrame (doc, view),
		TtaGetMessage (LIB, TMSG_ATTR), buttons, bufMenu, FALSE, 2,
		'L', D_DONE);
#endif /* _GTK */

   title = (char *)TtaGetMemory (strlen (pAttr->AttrName) + 2);
   strcpy (title, pAttr->AttrName);
   switch (pAttr->AttrType)
     {
     case AtNumAttr: /* attribut a valeur numerique */
       {
	 if (required)
	   subform = NumMenuAttrNumNeeded;
	 else
	   subform = form + 1;
#ifdef _GTK
	 TtaNewNumberForm (subform, form, title, -MAX_INT_ATTR_VAL,
			   MAX_INT_ATTR_VAL, FALSE);
	 TtaAttachForm (subform);
#endif /* _GTK */
	 if (currAttr == NULL)
	   i = 0;
	 else
	   i = currAttr->AeAttrValue;
#ifdef _GTK
	 /* initialize the input area only when an attribute already exists */
	 if (currAttr)
	   TtaSetNumberForm (subform, i);
#endif /* _GTK */
#ifdef _WINGUI
	 sprintf (formRange, "%d .. %d", -MAX_INT_ATTR_VAL, MAX_INT_ATTR_VAL); 
	 formValue = i;
	 DialogBox (hInstance, MAKEINTRESOURCE (NUMATTRDIALOG), NULL, 
		    (DLGPROC) InitNumAttrDialogWndProc);
#endif /* _WINGUI */
#ifdef _WX
	 
	 AmayaPanelParams p;
	 p.param1 = (void*)AmayaAttributePanel::wxATTR_ACTION_SETUPNUM;
	 p.param2 = (void*)required;
	 p.param3 = (void*)i;
	 AmayaSubPanelManager::GetInstance()->SendDataToPanel( WXAMAYA_PANEL_ATTRIBUTE, p );
#endif /* _WX */
       }
       break;
       
     case AtTextAttr: /* attribut a valeur textuelle */
       {
	 if (required)
	   subform = NumMenuAttrTextNeeded;
	 else
	   subform = form + 2;
	 if (currAttr && currAttr->AeAttrText)
	   {
	     i = LgMaxAttrText - 2;
	     i = CopyBuffer2MBs (currAttr->AeAttrText, 0,
				 (unsigned char*)TextAttrValue, i);
	     /* convert to the dialogue encoding */
	     tmp = (char *)TtaConvertMbsToByte ((unsigned char *)TextAttrValue,
						TtaGetDefaultCharset ());
	     strcpy (TextAttrValue, tmp);
	     TtaFreeMemory (tmp);
	   }
	 else
	   TextAttrValue[0] = EOS;
#ifdef _WX
	 AmayaPanelParams p;
	 p.param1 = (void*)AmayaAttributePanel::wxATTR_ACTION_SETUPTEXT;
	 p.param2 = (void*)required;
	 p.param3 = (void*)TextAttrValue;
	 AmayaSubPanelManager::GetInstance()->SendDataToPanel( WXAMAYA_PANEL_ATTRIBUTE, p );
#endif /* _WX */
#ifdef _GTK
	 TtaNewTextForm (subform, form, title, 40, 1, FALSE);
	 TtaAttachForm (subform);
	 TtaSetTextForm (subform, TextAttrValue);       
#endif /* _GTK */
#ifdef _WINGUI
     if (required)
       DialogBox (hInstance, MAKEINTRESOURCE (REQATTRDIALOG), NULL, 
		         (DLGPROC) InitFormDialogWndProc);
	  else
	   DialogBox (hInstance, MAKEINTRESOURCE (TEXTATTRDIALOG), NULL, 
		         (DLGPROC) InitSheetDialogWndProc);
#endif /* _WINGUI */
       }
       break;
       
     case AtEnumAttr: /* attribut a valeurs enumerees */
       {
	 if (required)
	   subform = NumMenuAttrEnumNeeded;
	 else
	   subform = form + 3;
	 /* cree un menu de toutes les valeurs possibles de l'attribut */
	 lgmenu = 0;
	 val = 0;
	 /* boucle sur les valeurs possibles de l'attribut */
	 while (val < pAttr->AttrNEnumValues)
	   {
#if defined(_WINGUI)
	     i = strlen (pAttr->AttrEnumValue[val]) + 1; /* for EOS */
	     if (lgmenu + i < MAX_TXT_LEN)
	       {
		 strcpy (&WIN_buffMenu[lgmenu], pAttr->AttrEnumValue[val]);
		 val++;
	       } 
#endif /* _WINGUI */
#ifdef _GTK
	     i = strlen (pAttr->AttrEnumValue[val]) + 2; /* for 'B' and EOS */
	     if (lgmenu + i < MAX_TXT_LEN)
	       {
		 bufMenu[lgmenu] = 'B';
		 strcpy (&bufMenu[lgmenu + 1], pAttr->AttrEnumValue[val]);
		 val++;
	       } 
#endif /* _GTK */
#if defined(_WX)
	     i = strlen (pAttr->AttrEnumValue[val]) + 1; /* for EOS */
	     if (lgmenu + i < MAX_TXT_LEN)
	       {
		 strcpy (&bufMenu[lgmenu], pAttr->AttrEnumValue[val]);
		 val++;
	       } 
#endif /* _WX */
	     lgmenu += i;
	   }
	 /* current value */
	 i = 0;
	 if (currAttr != NULL && currAttr->AeAttrValue > 0)
	   i = currAttr->AeAttrValue - 1;
	 if (PtrReqAttr)
	   PtrReqAttr->AeAttrValue = i + 1;
#ifdef _WX
	 AmayaPanelParams p;
	 p.param1 = (void*)AmayaAttributePanel::wxATTR_ACTION_SETUPENUM;
	 p.param2 = (void*)required;
	 p.param3 = (void*)bufMenu;
	 p.param4 = (void*)val;
	 p.param5 = (void*)i;
	 AmayaSubPanelManager::GetInstance()->SendDataToPanel( WXAMAYA_PANEL_ATTRIBUTE, p );
#endif /* _WX */
#ifdef _GTK
	 /* cree le menu des valeurs de l'attribut */
	 TtaNewSubmenu (subform, form, 0, title, val, bufMenu, NULL, 0, FALSE);
	 TtaAttachForm (subform);
	 TtaSetMenuForm (subform, i);
#endif /* _GTK */
#ifdef _WINGUI
	 nbDlgItems = val;
	 CreateAttributeDlgWindow (pAttr->AttrName, i+1, nbDlgItems,
				   WIN_buffMenu, required);
#endif /* _WINGUI */
       }
       break;
     default: break;
   }

   if (title != NULL)
     TtaFreeMemory (title);
}

/*----------------------------------------------------------------------
   CallbackReqAttrMenu
   handles the callback of the menu which captures the required attributes.
  ----------------------------------------------------------------------*/
void CallbackReqAttrMenu (int ref, int val, char *txt)
{
  int      length;

  switch (ref)
    {
    case NumMenuAttrRequired:
      /* retour de la feuille de dialogue elle-meme */
      /* on detruit cette feuille de dialogue sauf si c'est */
      /* un abandon */
      if (PtrReqAttr == NULL)
	{
	  TtaDestroyDialogue (NumMenuAttrRequired);
	  MandatoryAttrFormExists = FALSE;
	  if (AttrFormExists)
	  {
	    ThotCallback (NumMenuAttr, INTEGER_DATA, (char *)1);
		AttrFormExists = FALSE;
	  }
	}
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
      if (PtrReqAttr == NULL)
	NumAttrValue = val;
      else if (val >= -MAX_INT_ATTR_VAL || val <= MAX_INT_ATTR_VAL)
	PtrReqAttr->AeAttrValue = val;
      break;
    case NumMenuAttrTextNeeded:
      /* zone de saisie du texte de l'attribut */
      if (PtrReqAttr == NULL)
	  {
	strncpy (TextAttrValue, txt, LgMaxAttrText);
	  }
      else
	{
	  if (PtrReqAttr->AeAttrText == NULL)
	    GetTextBuffer (&PtrReqAttr->AeAttrText);
	  else
	    ClearText (PtrReqAttr->AeAttrText);
	  CopyStringToBuffer ((unsigned char*)txt, PtrReqAttr->AeAttrText, &length);
	}
      break;
    case NumMenuAttrEnumNeeded:
      /* menu des valeurs d'un attribut a valeurs enumerees */
      val++;
      if (PtrReqAttr == NULL)
	NumAttrValue = val;
      else
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
void BuildReqAttrMenu (PtrAttribute pAttr, PtrDocument pDoc)
{
   PtrTtAttribute        pRuleAttr;

   PtrReqAttr = pAttr;
   PtrDocOfReqAttr = pDoc;
   pRuleAttr = pAttr->AeAttrSSchema->SsAttribute->TtAttr[pAttr->AeAttrNum - 1];
   /* toujours lie a la vue 1 du document */
   MenuValues (pRuleAttr, TRUE, NULL, pDoc, 1);
#if defined(_GTK) || defined(_WX)
   TtaShowDialogue (NumMenuAttrRequired, FALSE);
   TtaWaitShowDialogue ();
#endif /* #if defined(_GTK) || defined(_WX) */
}

/*----------------------------------------------------------------------
   TteItemMenuAttr 
   sends the AttrMenu.Pre message which indicates that the editor
   is going to add to the Attributes menu an item for the creation
   of an attribute of type (pSS, att) for the pEl element. It 
   returns the answer from the application.
  ----------------------------------------------------------------------*/
static ThotBool TteItemMenuAttr (PtrSSchema pSS, int att, PtrElement pEl,
				 PtrDocument pDoc)
{
   NotifyAttribute     notifyAttr;
   ThotBool            OK;

   notifyAttr.event = TteAttrMenu;
   notifyAttr.document = (Document) IdentDocument (pDoc);
   notifyAttr.element = (Element) pEl;
   notifyAttr.attribute = NULL;
   notifyAttr.info = 0; /* not sent by undo */
   notifyAttr.attributeType.AttrSSchema = (SSchema) pSS;
   notifyAttr.attributeType.AttrTypeNum = att;
   OK = !CallEventAttribute (&notifyAttr, TRUE);
   return OK;
}


/*----------------------------------------------------------------------
   BuildAttrMenu
   builds the Attributes menu and returns the number of attributes added
   to the menu.
   Returns also the number of events attibutes and updates the corresponding
   buffer.
  ----------------------------------------------------------------------*/
static int BuildAttrMenu (char *bufMenu, PtrDocument pDoc, int *nbEvent,
			  char *bufEventAttr)
{
  PtrDocument         SelDoc;
  PtrElement          firstSel, lastSel;
  PtrSSchema          pSS;
  PtrSSchema          pSchExt;
  PtrAttribute        pAttr;
  PtrSRule            pRe1;
  PtrTtAttribute      pAt;
  char                tempBuffer[100];
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
  if (selectionOK && firstSel == lastSel && firstSel->ElParent == NULL)
    /* the Document element is selected. It can't accept any attribute */
    selectionOK = FALSE;
  if (selectionOK && ElementIsReadOnly (firstSel))
    /* the selected element is read-only. Don't change its attributes */
    selectionOK = FALSE;
  if (selectionOK && SelDoc == pDoc && firstSel)
    /* il y a une selection et elle est dans le document traite' */
    {
      /* cherche les attributs globaux definis dans le schema de structure */
      /* du premier element selectionne' et dans les extrensions de ce schema*/
      pSS = firstSel->ElStructSchema;/* schema de struct de l'element courant*/
      if (pSS)
	{
	  /* on parcourt toutes les extensions de ce schema de structure */
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
		      if (pSS->SsAttribute->TtAttr[att - 1]->AttrGlobal &&
			  /* and invisible attributes */
			  !AttrHasException (ExcInvisible, att, pSS))
			/* skip the attribute Language execpt the first time */
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
	}

      /* cherche les attributs locaux du premier element selectionne' */
      pSS = firstSel->ElStructSchema;
      if (pSS != NULL)
	{
	  pRe1 = pSS->SsRule->SrElem[firstSel->ElTypeNumber - 1];
	  pSchExt = SelDoc->DocSSchema;
	  do
	    {
	      if (pRe1 != NULL)
		/* prend les attributs locaux definis dans cette regle */
		for (att = 0; att < pRe1->SrNLocalAttrs; att++)
		  if (!pSS->SsAttribute->TtAttr[pRe1->SrLocalAttr->Num[att] - 1]->AttrGlobal)
		   if (nbOfEntries - *nbEvent < MAX_MENU &&
		       !AttrHasException (ExcInvisible,
					  pRe1->SrLocalAttr->Num[att], pSS) &&
		       TteItemMenuAttr (pSS, pRe1->SrLocalAttr->Num[att],
					firstSel, SelDoc))
		      {
		      /* conserve le schema de structure et le numero */
		      /* d'attribut de cette nouvelle entree du menu */
		      AttrStruct[nbOfEntries] = pSS;
		      AttrNumber[nbOfEntries] = pRe1->SrLocalAttr->Num[att];
		      AttrOblig[nbOfEntries] = pRe1->SrRequiredAttr->Bln[att];
		      /* is it an event attribute */
		      AttrEvent[nbOfEntries] = AttrHasException (ExcEventAttr,
					     pRe1->SrLocalAttr->Num[att], pSS);
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
      /* add attributes attached to the element that are not yet in
	 the table */
      pAttr = firstSel->ElFirstAttr;
      while (pAttr)
	{
	  isNew = TRUE;
	  for (att = 0; att < nbOfEntries && isNew; att++)
	    {
	      if (pAttr->AeAttrNum == AttrNumber[att] &&
		  pAttr->AeAttrSSchema == AttrStruct[att])
		isNew = FALSE;
            }
	  if (isNew)
	    {
	      if (nbOfEntries - *nbEvent < MAX_MENU &&
		  !AttrHasException (ExcInvisible, pAttr->AeAttrNum,
				     pAttr->AeAttrSSchema) &&
		  TteItemMenuAttr (pAttr->AeAttrSSchema, pAttr->AeAttrNum,
				   firstSel, SelDoc))
		{
		  /* conserve le schema de structure et le numero */
		  /* d'attribut de cette nouvelle entree du menu */
		  AttrStruct[nbOfEntries] = pAttr->AeAttrSSchema;
		  AttrNumber[nbOfEntries] = pAttr->AeAttrNum;
		  AttrOblig[nbOfEntries] = FALSE;
		  /* is it an event attribute? */
		  AttrEvent[nbOfEntries] = AttrHasException (ExcEventAttr,
				       pAttr->AeAttrNum, pAttr->AeAttrSSchema);
		  if (AttrEvent[nbOfEntries])
		    (*nbEvent)++;
		  nbOfEntries++;
		}
	    }
          pAttr = pAttr->AeNext;
	}

      /* build the menu according to the table */
      GetAttribute (&pAttr);
      if (nbOfEntries > 0)
	{
	  lgmenu = 0;
	  lgsubmenu = 0;
	  k = 0;
	  j = 0;
	  /* met les noms des attributs de la table dans le menu */
	  for (att = 0; att < nbOfEntries; att++)
	    {
	      pAt = AttrStruct[att]->SsAttribute->TtAttr[AttrNumber[att]-1];
	      pAttr->AeAttrSSchema = AttrStruct[att];
	      pAttr->AeAttrNum = AttrNumber[att];
	      pAttr->AeDefAttr = FALSE;
	      if (pAt->AttrType == AtEnumAttr && pAt->AttrNEnumValues == 1)
		/* attribut enumere' a une seule valeur (attribut booleen) */
		sprintf (tempBuffer, "T%s", pAt->AttrName);
	      else
		sprintf (tempBuffer, "T%s...", pAt->AttrName);
	      i = strlen (tempBuffer) + 1;
	      if (AttrEvent[att])
		{
		  if (lgsubmenu + i < MAX_TXT_LEN)
		    strcpy (&bufEventAttr[lgsubmenu], tempBuffer);
		  lgsubmenu += i;
		  /* mark all active enties*/
		  if (AttributeValue (firstSel, pAttr) != NULL)
		    ActiveEventAttr[k] = 1;
		  else
		    ActiveEventAttr[k] = 0;
		  AttrEventNumber[k] = att;
		  k++;
		}
	      else
		{
		  if (lgmenu + i < MAX_TXT_LEN)
		    strcpy (&bufMenu[lgmenu], tempBuffer);
		  lgmenu += i;
		  /* mark all active enties*/
		  if (AttributeValue (firstSel, pAttr) != NULL)
		    ActiveAttr[j] = 1;
		  else
		    ActiveAttr[j] = 0;
		  j++;
		}
	    }
	}
      DeleteAttribute (NULL, pAttr);

      if (*nbEvent > 0)
	{
	  /* add the event entry if needed */
	  sprintf (tempBuffer, "M%s", TtaGetMessage (LIB, TMSG_EVENTS));
	  i = strlen (tempBuffer) + 1;
	  if (lgmenu + i < MAX_TXT_LEN)
	    {
	      strcpy (&bufMenu[lgmenu], tempBuffer);
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
void UpdateAttrMenu (PtrDocument pDoc)
{
  Document            document;
  Menu_Ctl           *pMenu;
  char                bufMenuAttr[MAX_TXT_LEN];
  char                bufEventAttr[MAX_TXT_LEN];
  int                 view, menu, menuID;
  int                 frame, ref, nbEvent;
  int                 nbItemAttr, i, max;
#ifdef _WINGUI
  int                 nbOldItems;
#endif /* _WINGUI */

#ifdef _WX
  /* do nothing if the attribute dialog is not updatable (auto refresh checkbox activate) */
  if (!AmayaSubPanelManager::GetInstance()->IsActive(WXAMAYA_PANEL_ATTRIBUTE))
    {
      AmayaSubPanelManager::GetInstance()->ShouldBeUpdated(WXAMAYA_PANEL_ATTRIBUTE);
      return;
    }
#endif /* _WX */

  /* Compose le menu des attributs */
  if (pDoc == SelectedDocument && !pDoc->DocReadOnly)
    nbItemAttr = BuildAttrMenu (bufMenuAttr, pDoc, &nbEvent, bufEventAttr);
  else
    {
      nbItemAttr = 0;
      nbEvent = 0;
    }

#ifdef _WX
  /* update the attribute dialog */
  AmayaPanelParams p;
  p.param1 = (void*)AmayaAttributePanel::wxATTR_ACTION_LISTUPDATE;
  p.param2 = (void*)bufMenuAttr;
  p.param3 = (void*)nbItemAttr;
  p.param4 = (void*)ActiveAttr;
  p.param5 = (void*)bufEventAttr;
  p.param6 = (void*)nbEvent;
  p.param7 = (void*)ActiveEventAttr;
  AmayaSubPanelManager::GetInstance()->SendDataToPanel( WXAMAYA_PANEL_ATTRIBUTE, p );
#endif /* _WX */

#ifndef _WX
  /* Now update the menu widget */
  document = (Document) IdentDocument (pDoc);
  /* Traite toutes les vues de l'arbre principal */
  for (view = 1; view <= MAX_VIEW_DOC; view++)
    {
      frame = pDoc->DocViewFrame[view - 1];
      if (frame != 0 && FrameTable[frame].MenuAttr != -1)
	{
#ifdef _WINGUI 
	  currentFrame = frame;
#endif /* _WINGUI */
	  menuID = FrameTable[frame].MenuAttr;
	  menu = FindMenu (frame, menuID, &pMenu) - 1;
	  ref = (menu * MAX_ITEM) + frame + MAX_LocalMenu;
	  if (pDoc != SelectedDocument || nbItemAttr == 0)
	    {
	      /* le menu Attributs contient au moins un attribut */
	      TtaSetMenuOff (document, view, menuID);
	      TtaDestroyDialogue (ref);
#ifdef _WINGUI
	      /* Remove that reference in the window list of catalogues */
	      CleanFrameCatList (frame, ref);
#endif /* _WINGUI */
	    }
	  else
	    {
#ifdef _WINGUI
	      nbOldItems = GetMenuItemCount (FrameTable[frame].WdMenus[menu]);
	      for (i = 0; i < nbOldItems; i ++)
		{
		  if (!DeleteMenu (FrameTable[frame].WdMenus[menu], ref + i,
				   MF_BYCOMMAND))
		    DeleteMenu (FrameTable[frame].WdMenus[menu], i,
				MF_BYPOSITION);
		}
#endif /* _WINGUI */
	      if (EventMenu[frame - 1] != 0)
		{
		  /* destroy the submenu event */
		  TtaDestroyDialogue (EventMenu[frame - 1]);
#ifdef _WINGUI
		  /* Remove that reference in the window list of catalogues */
		  CleanFrameCatList (frame, EventMenu[frame - 1]);
		  if (subMenuID[frame])
		    DeleteMenu (FrameTable[frame].WdMenus[menu], subMenuID[frame],
				MF_BYCOMMAND);
		  else 
		    subMenuID[frame] = 0;
#endif /* _WINGUI */
		  EventMenu[frame - 1] = 0;
		}
	      TtaNewPulldown (ref, FrameTable[frame].WdMenus[menu], NULL,
			      nbItemAttr, bufMenuAttr, NULL, 0);
	      if (nbEvent != 0)
		{
		  /* there is a submenu of event attributes */
		  EventMenu[frame - 1] = (nbItemAttr * MAX_MENU * MAX_ITEM) + ref;
		  TtaNewSubmenu (EventMenu[frame - 1], ref, nbItemAttr - 1,
				 NULL, nbEvent, bufEventAttr, NULL, 0, FALSE);
		  
		  /* post active attributes */
#ifdef _WINGUI
		  for (i = 0; i < nbEvent; i++)
		    WIN_TtaSetToggleMenu (EventMenu[frame - 1], i,
					  (ThotBool) (ActiveEventAttr[i] == 1),
					  FrMainRef[frame]);
#endif /* _WINGUI */
#if defined(_GTK)
		  for (i = 0; i < nbEvent; i++)
		    TtaSetToggleMenu (EventMenu[frame - 1], i,
				      (ActiveEventAttr[i] == 1));
#endif /* #if defined(_GTK) */
		}
	      
	      /* post active attributes */
	      max = nbItemAttr;
	      if (nbEvent != 0)
		/* except the submenu entry */
		max--;
	      for (i = 0; i < max; i++)
#ifdef _WINGUI
		WIN_TtaSetToggleMenu (ref, i, (ThotBool) (ActiveAttr[i] == 1),
				      FrMainRef[frame]);
#endif /* _WINGUI */
#if defined(_GTK)
	        TtaSetToggleMenu (ref, i, (ActiveAttr[i] == 1));
#endif /* #if defined(_GTK) */
	      TtaSetMenuOn (document, view, menuID);
	    }
	}
    }
#endif /* !_WX */
}

/*----------------------------------------------------------------------
   AttachAttrToElem attachs the attribute to the element
  ----------------------------------------------------------------------*/
static void AttachAttrToElem (PtrAttribute pAttr, PtrElement pEl, PtrDocument pDoc)
{
  Language            lang;
  PtrAttribute        pAttrAsc;
  PtrElement          pElAttr;
  char                text[100];

  /* On ne traite pas les marques de page */
  if (!pEl->ElTerminal || pEl->ElLeafType != LtPageColBreak)
    {
      if (pAttr->AeAttrNum == 1)
   	/* c'est l'attribut langue */
	{
   	  /* change la langue de toutes les feuilles de texte du sous-arbre */
   	  /* de l'element */
   	  if (pAttr->AeAttrText)
	    {
	      CopyBuffer2MBs (pAttr->AeAttrText, 0, (unsigned char*)text, 99);
	      lang = TtaGetLanguageIdFromName (text);
	    }
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
	      
	      if (pAttrAsc && pAttrAsc->AeAttrText)
		{
		  /* un ascendant definit la langue, on prend cette langue */
		  CopyBuffer2MBs (pAttrAsc->AeAttrText, 0, (unsigned char*)text, 99);
		  lang = TtaGetLanguageIdFromName (text);
		}
   	    }
   	  ChangeLanguage (pDoc, pEl, lang, FALSE);
	}
      
      /* met la nouvelle valeur de l'attribut dans l'element et */
      /* applique les regles de presentation de l'attribut a l'element */
      AttachAttrWithValue (pEl, pDoc, pAttr, TRUE);
    }
}


/*----------------------------------------------------------------------
   AttachAttrToRange applique l'attribut pAttr a une partie de document
  ----------------------------------------------------------------------*/
static void AttachAttrToRange (PtrAttribute pAttr, int lastChar, 
			       int firstChar, PtrElement pLastSel, 
			       PtrElement pFirstSel, PtrDocument pDoc,
			       ThotBool reDisplay)
{
   PtrElement          pEl;

   /* eteint d'abord la selection */
   TtaClearViewSelections ();
   /* Coupe les elements du debut et de la fin de la selection s'ils */
   /* sont partiellement selectionnes */
   IsolateSelection (pDoc, &pFirstSel, &pLastSel, &firstChar, &lastChar, TRUE);
   /* start an operation sequence in editing history */
   if (ThotLocalActions[T_openhistory] != NULL)
     (*(Proc6)ThotLocalActions[T_openhistory]) (
		(void *)pDoc,
		(void *)pFirstSel,
		(void *)pLastSel,
		(void *)NULL,
		(void *)firstChar,
		(void *)lastChar);
   /* parcourt les elements selectionnes */
   pEl = pFirstSel;
   while (pEl != NULL)
     {
	AttachAttrToElem (pAttr, pEl, pDoc);
	/* cherche l'element a traiter ensuite */
	pEl = NextInSelection (pEl, pLastSel);
     }
   /* close the editing sequence */
   if (ThotLocalActions[T_closehistory] != NULL)
	(*(Proc1)ThotLocalActions[T_closehistory]) ((void *)pDoc);
   /* parcourt a nouveau les elements selectionnes pour fusionner les */
   /* elements voisins de meme type ayant les memes attributs, reaffiche */
   /* toutes les vues et retablit la selection */
   if (reDisplay)
     {
       GetCurrentSelection (&pDoc, &pFirstSel, &pLastSel, &firstChar,
			    &lastChar);
       if (pFirstSel && pFirstSel->ElStructSchema)
	 SelectRange (pDoc, pFirstSel, pLastSel, firstChar, lastChar);
     }
}

/*----------------------------------------------------------------------
   CallbackValAttrMenu
   handles the callback of the form which captures the attribute values.
   Applies to the selected elements the attributes chosen by the user.
   ref: reference to the dialogue element that called this function
   valmenu: selected or captured value in this dialogue element
   valtext: pointer to the captured text in this dialogue element
  ----------------------------------------------------------------------*/
void CallbackValAttrMenu (int ref, int valmenu, char *valtext)
{
  PtrDocument         SelDoc;
  PtrElement          firstSel, lastSel;
  PtrAttribute        pAttrNew;
  DisplayMode         dispMode = DeferredDisplay;
  Document            doc = 0;
  PtrSRule            pSRule;
  char               *tmp;
  int                 firstChar, lastChar, att;
  int                 act;
  ThotBool            lock = TRUE;

  act = 0; /* apply by default */
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
	{
	  /* on detruit la feuille de dialogue */
	  TtaDestroyDialogue (NumMenuAttr);
	  AttrFormExists = FALSE;
	}
      act = valmenu;
      break;
    }

  /* demande quelle est la selection courante */
  if (!GetCurrentSelection (&SelDoc, &firstSel, &lastSel, &firstChar,
			    &lastChar))
    /* no selection. quit */
    return;

  if (act == 2)
    /* the user wants to delete the attribute. Is it a mandatory attribute
       for the selected element ? */
    if (SchCurrentAttr == firstSel->ElStructSchema)
      {
	/* get the structure rule defining the selected element */
	pSRule = firstSel->ElStructSchema->SsRule->SrElem[firstSel->ElTypeNumber - 1];
	/* look for the attribute in the list of allowed attributes for this
	   element type */
        for (att = 0; att < pSRule->SrNLocalAttrs; att++)
	  if (pSRule->SrLocalAttr->Num[att] == NumCurrentAttr)
	    /* this is the attribute of interest */
	    {
	      if (pSRule->SrRequiredAttr->Bln[att])
		/* mandatory attribute */
		act = 0;
	      /* stop */
	      att = pSRule->SrNLocalAttrs;
	    }
      }

  if (act > 0)
    {
      /* ce n'est pas une simple fermeture de la feuille de dialogue */
	{
	  /* on ne fait rien si le document ou` se trouve la selection
	     n'utilise pas le schema de structure qui definit l'attribut */
	  if (SchCurrentAttr &&
	      GetSSchemaForDoc (SchCurrentAttr->SsName, SelDoc))
	    {
	      /* lock tables formatting */
	      TtaGiveTableFormattingLock (&lock);
	      if (!lock)
		{
		  doc = IdentDocument (SelDoc);
		  dispMode = TtaGetDisplayMode (doc);
		  if (dispMode == DisplayImmediately)
		    TtaSetDisplayMode (doc, DeferredDisplay);
		  /* table formatting is not locked, lock it now */
		  TtaLockTableFormatting ();
		}

	      GetAttribute (&pAttrNew);
	      if (NumCurrentAttr == 1)
	        pAttrNew->AeAttrSSchema = firstSel->ElStructSchema;
	      else
	        pAttrNew->AeAttrSSchema = SchCurrentAttr;
	      pAttrNew->AeAttrNum = NumCurrentAttr;
	      pAttrNew->AeDefAttr = FALSE;
	      pAttrNew->AeAttrType = SchCurrentAttr->SsAttribute->TtAttr[NumCurrentAttr - 1]->AttrType;
	      
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
		      tmp = (char *)TtaConvertByteToMbs ((unsigned char *)TextAttrValue, TtaGetDefaultCharset ());
		      CopyMBs2Buffer ((unsigned char *)tmp, pAttrNew->AeAttrText, 0, strlen (tmp));
		      TtaFreeMemory (tmp);
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
		  TtaUnlockTableFormatting ();
		  if (dispMode == DisplayImmediately)
		    TtaSetDisplayMode (doc, DisplayImmediately);
		}
	      UpdateAttrMenu (SelDoc);
	      DeleteAttribute (NULL, pAttrNew);
	    }
	}
    }
}

/*----------------------------------------------------------------------
   CallbackAttrMenu 
   handles the callbacks of the "Attributes" menu: creates a
   form to capture the value of the chosen attribute.
  ----------------------------------------------------------------------*/
void CallbackAttrMenu (int refmenu, int att, int frame)
{
  PtrTtAttribute      pAttr;
  PtrAttribute        pAttrNew, currAttr;
  PtrDocument         SelDoc;
  PtrElement          firstSel, lastSel;
  PtrReference        Ref;
  PtrSRule            pSRule;
  Document            doc;
  View                view;
  int                 item, i;
  int                 firstChar, lastChar;
  ThotBool            mandatory;

  FrameToView (frame, &doc, &view);
  item = att;
  /* get the right entry in the attributes list */
#if _WX
  /* on wxWidgets, attributs is not a menu but a dialog, this dialog do not have reference */
  /* here we must simulate the default behaviour */
  if (refmenu == -1)
    {
      /* this is the events attribut menu */
      refmenu = EventMenu[frame - 1];
    }
#endif /* _WX */
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
	pAttr = AttrStruct[att]->SsAttribute->TtAttr[AttrNumber[att] - 1];
	pAttrNew->AeAttrType = pAttr->AttrType;
	if (pAttr->AttrType == AtReferenceAttr)
	  {
	    /* attache un bloc reference a l'attribut */
	    GetReference (&Ref);
	    pAttrNew->AeAttrReference = Ref;
	    pAttrNew->AeAttrReference->RdElement = NULL;
	    pAttrNew->AeAttrReference->RdAttribute = pAttrNew;
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
	      /* that's the language attribute */
	      {
		InitFormLanguage (doc, view, firstSel, currAttr);
		/* memorise l'attribut concerne' par le formulaire */
		SchCurrentAttr = pAttrNew->AeAttrSSchema;
		NumCurrentAttr = 1;
		DocCurrentAttr = LoadedDocument[doc - 1];
		/* restaure l'etat courant du toggle */
#ifdef _WINGUI
		CreateLanguageDlgWindow (TtaGetViewFrame (doc, view), 
					 WIN_nbItem, WIN_buffMenu, WIN_Lab, 
					 (int)WIN_Language);
#endif /* _WINGUI */
#if defined(_GTK)  || defined(_WX)
		if (ActiveAttr[item] == 0)
		  TtaSetToggleMenu (refmenu, item, FALSE);
		else
		  TtaSetToggleMenu (refmenu, item, TRUE);
#endif /* #if defined(_GTK) || defined(_WX) */
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
		/* is this attribute mandatory ? */
		mandatory = FALSE;
		/* get the structure rule defining the selected element */
		pSRule = firstSel->ElStructSchema->SsRule->SrElem[firstSel->ElTypeNumber - 1];
		/* look for the attribute in the list of allowed attributes
		   for this element type */
		for (i = 0; i < pSRule->SrNLocalAttrs; i++)
		  if (pSRule->SrLocalAttr->Num[i] == AttrNumber[att])
		    /* this is the attribute of interest */
		    {
		      mandatory = pSRule->SrRequiredAttr->Bln[i];
		      /* stop */
		      i = pSRule->SrNLocalAttrs;
		    }

		/* construit le formulaire de saisie de la valeur de */
		/* l'attribut */
		PtrReqAttr = NULL;
		PtrDocOfReqAttr = NULL;
		if (mandatory)
			/* the callback of required attribute should call
			the standard callback attribute */
			AttrFormExists = TRUE;
		/* memorise l'attribut concerne' par le formulaire */
		SchCurrentAttr = AttrStruct[att];
		NumCurrentAttr = AttrNumber[att];
		DocCurrentAttr = LoadedDocument[doc - 1];
		/* register the current attribut */
		CurrentAttr = att;
		MenuValues (pAttr, mandatory, currAttr, SelDoc, view);
		/* restore the toggle state */
#if defined(_GTK) || defined(_WX)
		if (ActiveAttr[item] == 0)
		  TtaSetToggleMenu (refmenu, item, FALSE);
		else
		  TtaSetToggleMenu (refmenu, item, TRUE);
		/* display the form */
		if (mandatory)
		  TtaShowDialogue (NumMenuAttrRequired, TRUE);
		else
		  TtaShowDialogue (NumMenuAttr, TRUE);
#endif /* #if defined(_GTK) || defined(_WX) */
	      }
	    DeleteAttribute (NULL, pAttrNew);
	  }
      }
}

/*----------------------------------------------------------------------
   CallbackLanguageMenu
   handles the callbacks of the Language form.
  ----------------------------------------------------------------------*/
void CallbackLanguageMenu (int ref, int val, char *txt)
{
  ThotBool   doit;
  char       TmpTextAttrValue[LgMaxAttrText];
  int        TmpNumCurrentAttr;

  doit = FALSE;
  switch (ref)
    {
    case NumSelectLanguage:
      /* current language name */
      if (txt == NULL)
	LangAttrValue[0] = EOS;
      else
	strncpy (LangAttrValue, txt, LgMaxAttrText);
      break;
    case NumFormLanguage:
      switch (val)
	{
	case 0:
	  /* cancel */
	  break;
	case 1:
	  /* apply the new value if it's a valid language */
	  strcpy (TmpTextAttrValue, LangAttrValue);
	  strcpy (LangAttrValue, TtaGetLanguageCodeFromName (TmpTextAttrValue));
	  doit = LangAttrValue[0] != EOS;
#ifdef _GTK
	  if (doit)
	    TtaNewLabel (NumLabelHeritedLanguage, NumFormLanguage, "");
#endif /* _GTK */
 	  break;
	case 2:
	  /* remove the current value */
	  doit = TRUE;
	  break;
	}
      if (doit)
	{
	  /* temporary change of TextAttrValue and NumCurrentAttr */
	  strcpy (TmpTextAttrValue, TextAttrValue);
	  strcpy (TextAttrValue, LangAttrValue);
	  TmpNumCurrentAttr = NumCurrentAttr;
	  NumCurrentAttr = 1;
	  CallbackValAttrMenu (NumMenuAttr, val, NULL);
	  strcpy (TextAttrValue, TmpTextAttrValue);
	  NumCurrentAttr = TmpNumCurrentAttr;
	}
      break;
    }
}

/*----------------------------------------------------------------------
   CloseAttributeDialogues
   Closes all dialogue boxes related with attribute input that are
   associated with document pDoc.
  ----------------------------------------------------------------------*/
void CloseAttributeDialogues (PtrDocument pDoc)
{
 if (PtrDocOfReqAttr == pDoc)
   {
     TtaUnmapDialogue (NumMenuAttrRequired);
     TtaDestroyDialogue (NumMenuAttrRequired);
     PtrReqAttr = NULL;
     PtrDocOfReqAttr = NULL;
   }
 if (DocCurrentAttr == pDoc)
   {
     if (NumCurrentAttr == 1)
       {
	 TtaUnmapDialogue (NumFormLanguage);
	 TtaDestroyDialogue (NumFormLanguage);
       }
     else
       {
	 TtaDestroyDialogue (NumMenuAttrRequired);
	 TtaDestroyDialogue (NumMenuAttr);
       }
     SchCurrentAttr = NULL;
     DocCurrentAttr = NULL;
   }
}

/*----------------------------------------------------------------------
   AttributeMenuLoadResources
   connects the local actions.
  ----------------------------------------------------------------------*/
void AttributeMenuLoadResources ()
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
