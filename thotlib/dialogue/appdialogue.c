/*
 * Copyright (c) 1996 INRIA, All rights reserved
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
 * Handle windows and menu bars of Thot applications
 *
 * Author: I. Vatton (INRIA)
 *         R. Guetari (W3C/INRIA): Windows NT and Window 95 routines
 *
 */

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "modif.h"
#include "appdialogue.h"
#include "dialog.h"
#include "logowindow.h"
#include "application.h"
#include "dialog.h"
#include "document.h"
#include "message.h"
#include "libmsg.h"
#include "LiteClue.h"

#define MAX_ARGS	20
#define MAX_USER_ACTION	100

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "font_tv.h"
#include "edit_tv.h"
#include "frame_tv.h"
#include "units_tv.h"
#include "appdialogue_tv.h"

#include "applicationapi_f.h"
#include "inites_f.h"
#include "LiteClue_f.h"

extern boolean      WithMessages;	/* partage avec le module dialog.c */
extern Pixmap       image;
#ifndef _WIN_PRINT
extern int          appArgc;
extern char       **appArgv;
#endif /* !_WIN_PRINT */
extern ThotWidget   WIN_curWin;
typedef void        (*Thot_ActionProc) ();
typedef struct _CallbackCTX *PtrCallbackCTX;

typedef struct _CallbackCTX
  {
     Thot_ActionProc     callbackProc;
     int                 callbackSet;
     PtrCallbackCTX      callbackNext;
  }
CallbackCTX;

static PtrCallbackCTX firstCallbackAPI;
static int          FreeMenuAction;
static Pixmap       wind_pixmap;

/* LISTES DES MENUS : chaque menu pointe sur une liste d'items.  */
/* Chaque item contient le numero d'entree dans le fichier de    */
/* dialogue (le texte pouvant varier avec la langue utilisee) et */
/* l'indice dans la TABLE DES ACTIONS de l'action associee.      */
/* CsList des menus attache's a la fenetre principale de l'application */
static Menu_Ctl    *MainMenuList;

/* CsList des menus attache's aux frames de documents en ge'ne'ral */
static Menu_Ctl    *DocumentMenuList;

/* CsList des menus attache's aux frames de documents particuliers */
static SchemaMenu_Ctl *SchemasMenuList;

#ifdef _WINDOWS
#define WM_ENTER (WM_USER)

extern TBADDBITMAP ThotTBBitmap;

static WNDPROC lpfnTextZoneWndProc = (WNDPROC) 0;
static BOOL    doSwitchButton = TRUE;

static int FormattedViewXPos = 0;
static int FormattedViewYPos = 0;
int     currentFrame;

HWND  hwndClient ;
HWND  ToolBar ;
HWND  StatusBar;
HWND  logoFrame;
HMENU currentMenu;
#ifdef THOT_TOOLTIPS
int    nCust[MAX_FRAME][30];
static HWND hwndTB;

static int   tipIndex = 0;
static int   strIndex ;

extern int     CommandToString [MAX_BUTTON];
extern char    szTbStrings [4096];
#endif /* THOT_TOOLTIPS */

#define ToolBar_InsertButton(hwnd, idButton, lpButton) \
    (BOOL)SendMessage((hwnd), TB_INSERTBUTTON, (WPARAM)idButton, (LPARAM)(LPTBBUTTON)lpButton)

#if 0
extern HBITMAP appLogo;
extern int     bmpID;
#endif /* 0 */
HMENU hmenu;
int   menu_item ;
#ifdef THOT_TOOLTIPS
#ifdef __STDC__
LPSTR GetString (int frame, int iString)
#else  /* __STDC__ */
LPSTR GetString (frame, iString)
int frame;
int iString;
#endif /* __STDC__ */
{
   int i, cb ;
   LPSTR pString ;

   /* Cycle through to requested string */
   pString = &szTbStrings [0] ;
   for (i = 0 ; i < iString ; i++) {
       cb = lstrlen (pString) ;
       pString += (cb + 1) ;
   }

   return pString ;
}

#ifdef __STDC__
LRESULT ToolBarNotify (int frame, HWND hwnd, WPARAM wParam, LPARAM lParam)
#else  /* __STDC__ */
LRESULT ToolBarNotify (frame, hwnd, wParam, lParam)
int    frame;
HWND   hwnd; 
WPARAM wParam; 
LPARAM lParam;
#endif /* __STDC__ */
{
   LPNMHDR pnmh = (LPNMHDR) lParam ;
   int idCtrl = (int) wParam ;

   /* Allow toolbar to be customized */
   if ((pnmh->code == TBN_QUERYDELETE) || (pnmh->code == TBN_QUERYINSERT))
      return 1 ; /* We always say "yes" */

   /* Provide details of allowable toolbar buttons */
   if (pnmh->code == TBN_GETBUTTONINFO) {
      LPTBNOTIFY ptbn = (LPTBNOTIFY) lParam ;
      int iButton = nCust[frame][ptbn->iItem] ;

      if (iButton != -1) {
         lstrcpy (ptbn->pszText, GetString (frame, ptbn->iItem)) ;
         memcpy (&ptbn->tbButton, FrameTable[frame].Button[iButton], sizeof (TBBUTTON)) ;
         return 1 ;
      }
   }

   return 0 ;
}
#endif /* THOT_TOOLTIPS */

#ifdef __STDC__
LRESULT CALLBACK textZoneProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* __STDC__ */
LRESULT CALLBACK textZoneProc (hwnd, msg, wParam, lParam)
HWND   hwnd; 
UINT   msg; 
WPARAM wParam; 
LPARAM lParam;
#endif /* __STDC__ */
{
    switch (msg) { 
           case WM_KEYDOWN: 
                switch (wParam) { 
                       case VK_RETURN: 
                            SendMessage(GetParent (hwnd), WM_ENTER, 0, 0); 
                            return 0; 
				} 
                break; 
 
           case WM_KEYUP: 
           case WM_CHAR: 
                switch (wParam) { 
                       case VK_RETURN: 
                       return 0; 
				} 
    } 
 
    /* 
     * Call the original window procedure for default 
     * processing. 
     */ 
 
    return CallWindowProc (lpfnTextZoneWndProc, hwnd, msg, wParam, lParam); 
}
#endif /* _WINDOWS */

#include "appli_f.h"
#include "textcommands_f.h"
#include "structcreation_f.h"
#include "scroll_f.h"
#include "boxlocate_f.h"
#include "dialogapi_f.h"
#include "views_f.h"
#include "appdialogue_f.h"
#include "actions_f.h"
#include "callback_f.h"
#include "windowdisplay_f.h"
#include "font_f.h"
#include "absboxes_f.h"
#include "buildboxes_f.h"
#include "input_f.h"
#include "memory_f.h"
#include "structmodif_f.h"
#include "boxparams_f.h"
#include "boxselection_f.h"
#include "structselect_f.h"
#include "thotmsg_f.h"
#include "dialogapi_f.h"
#include "context_f.h"
#include "dictionary_f.h"
#include "viewapi_f.h"
#include "sortmenu_f.h"


/*----------------------------------------------------------------------
   TteInitMenuActions alloue la table des actions.                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TteInitMenus (char *name, int number)
#else  /* __STDC__ */
void                TteInitMenus (name, number)
char               *name;
int                 number;

#endif /* __STDC__ */
{
   int                 i;
   char                namef1[100];
   char                namef2[100];
   char                text[100];
   char                alphabet;

#  ifndef _WINDOWS
   Display            *Dp;
#  endif /* !_WINDOWS */

   /* Initialisation du  contexte serveur */
   FrRef[0] = 0;
#  ifdef _WINDOWS
   FrMainRef [0] = 0;
#  endif /* _WINDOWS */
   InitDocContexts ();


   /* Initialise le dialogue */
   servername = NULL;
#  ifndef _WIN_PRINT
   if (appArgc > 2)
     {
	i = 1;
	while (i < appArgc - 1)
	   if (strcmp (appArgv[i], "-display") != 0)
	      i++;
	   else
	     {
		/* l'argument est "-display" et celui qui suit le nom du display */
		servername = appArgv[i + 1];
		i = appArgc;
	     }
     }
#  endif /* _WIN_PRINT */
#  ifdef _WINDOWS
   WIN_TtaInitDialogue (servername, TtaGetMessage (LIB, TMSG_LIB_CONFIRM),
			   TtaGetMessage (LIB, TMSG_CANCEL), TtaGetMessage (LIB, TMSG_DONE));
#  else  /* _WINDOWS */
   TtaInitDialogue (servername, TtaGetMessage (LIB, TMSG_LIB_CONFIRM),
		    TtaGetMessage (LIB, TMSG_CANCEL), TtaGetMessage (LIB, TMSG_DONE), &app_cont, &Dp);
   if (!RootShell)
     {
	/* Connection au serveur X impossible */
	printf ("*** Not initialized\n");
	printf ("*** Fatal Error: X connexion refused\n");
	exit (1);
     }
   TtDisplay = Dp;
#  endif /* !_WINDOWS */

   /* Definition de la procedure de retour des dialogues */
   TtaDefineDialogueCallback (ThotCallback);

   Dict_Init ();
   ThotInitDisplay (name, 0, 0);
   alphabet = TtaGetAlphabet (TtaGetDefaultLanguage ());
   FontIdentifier (alphabet, 'H', 0, MenuSize, UnPoint, text, namef1);
   FontIdentifier (alphabet, 'H', 1, MenuSize, UnPoint, text, namef2);
   TtaChangeDialogueFonts (namef1, namef2);

   /* reserve les menus de Thot */
   TtaGetReferencesBase (MAX_ThotMenu);

   /* Il faut ajouter les actions internes liees a la structure */
   number += MAX_INTERNAL_CMD;
   /* as well as space for new User's defined actions */
   number += MAX_USER_ACTION;

   MaxMenuAction = number;
   MenuActionList = (Action_Ctl *) TtaGetMemory (number * sizeof (Action_Ctl));
   /* initialisation des equilalents clavier et validation de l'action */
   for (FreeMenuAction = 0; FreeMenuAction < MAX_INTERNAL_CMD; FreeMenuAction++)
     {
	MenuActionList[FreeMenuAction].ActionEquiv = NULL;
	for (i = 0; i < MAX_FRAME; i++)
	   MenuActionList[FreeMenuAction].ActionActive[i] = TRUE;
     }
   for (i = FreeMenuAction;i < MaxMenuAction;i++) {
	MenuActionList[i].ActionName = "";
	MenuActionList[i].Call_Action = NULL;
	MenuActionList[i].User_Action = NULL;
	MenuActionList[i].ActionEquiv = NULL;
   }

   /* Initialisation des actions internes obligatoires */
   MenuActionList[0].ActionName = CST_InsertChar;	/* action InsertChar() */
   MenuActionList[0].Call_Action = (Proc) NULL;
   MenuActionList[0].User_Action = (UserProc) NULL;

   MenuActionList[CMD_DeletePrevChar].ActionName = CST_DeletePrevChar;
   MenuActionList[CMD_DeletePrevChar].Call_Action = (Proc) NULL;
   MenuActionList[CMD_DeletePrevChar].User_Action = (UserProc) NULL;
   MenuActionList[CMD_DeletePrevChar].ActionEquiv = CST_EquivBS;

   MenuActionList[CMD_DeleteSelection].ActionName = CST_DeleteSelection;
   MenuActionList[CMD_DeleteSelection].Call_Action = (Proc) NULL;
   MenuActionList[CMD_DeleteSelection].User_Action = (UserProc) NULL;
   MenuActionList[CMD_DeleteSelection].ActionEquiv = CST_EquivDel;

   MenuActionList[CMD_BackwardChar].ActionName = CST_BackwardChar;
   MenuActionList[CMD_BackwardChar].Call_Action = (Proc) TtcPreviousChar;
   MenuActionList[CMD_BackwardChar].User_Action = (UserProc) NULL;

   MenuActionList[CMD_ForwardChar].ActionName = CST_ForwardChar;
   MenuActionList[CMD_ForwardChar].Call_Action = (Proc) TtcNextChar;
   MenuActionList[CMD_ForwardChar].User_Action = (UserProc) NULL;

   MenuActionList[CMD_PreviousLine].ActionName = CST_PreviousLine;
   MenuActionList[CMD_PreviousLine].Call_Action = (Proc) TtcPreviousLine;
   MenuActionList[CMD_PreviousLine].User_Action = (UserProc) NULL;

   MenuActionList[CMD_NextLine].ActionName = CST_NextLine;
   MenuActionList[CMD_NextLine].Call_Action = (Proc) TtcNextLine;
   MenuActionList[CMD_NextLine].User_Action = (UserProc) NULL;

   MenuActionList[CMD_BeginningOfLine].ActionName = CST_BeginningOfLine;
   MenuActionList[CMD_BeginningOfLine].Call_Action = (Proc) TtcStartOfLine;
   MenuActionList[CMD_BeginningOfLine].User_Action = (UserProc) NULL;

   MenuActionList[CMD_EndOfLine].ActionName = CST_EndOfLine;
   MenuActionList[CMD_EndOfLine].Call_Action = (Proc) TtcEndOfLine;
   MenuActionList[CMD_EndOfLine].User_Action = (UserProc) NULL;

   MenuActionList[CMD_ParentElement].ActionName = CST_ParentElement;
   MenuActionList[CMD_ParentElement].Call_Action = (Proc) TtcParentElement;
   MenuActionList[CMD_ParentElement].User_Action = (UserProc) NULL;

   MenuActionList[CMD_PreviousElement].ActionName = CST_PreviousElement;
   MenuActionList[CMD_PreviousElement].Call_Action = (Proc) TtcPreviousElement;
   MenuActionList[CMD_PreviousElement].User_Action = (UserProc) NULL;

   MenuActionList[CMD_NextElement].ActionName = CST_NextElement;
   MenuActionList[CMD_NextElement].Call_Action = (Proc) TtcNextElement;
   MenuActionList[CMD_NextElement].User_Action = (UserProc) NULL;

   MenuActionList[CMD_ChildElement].ActionName = CST_ChildElement;
   MenuActionList[CMD_ChildElement].Call_Action = (Proc) TtcChildElement;
   MenuActionList[CMD_ChildElement].User_Action = (UserProc) NULL;

   MenuActionList[CMD_PageUp].ActionName = CST_PageUp;
   MenuActionList[CMD_PageUp].Call_Action = (Proc) TtcPageUp;
   MenuActionList[CMD_PageUp].User_Action = (UserProc) NULL;
   MenuActionList[CMD_PageUp].ActionEquiv = CST_EquivPrior;

   MenuActionList[CMD_PageDown].ActionName = CST_PageDown;
   MenuActionList[CMD_PageDown].Call_Action = (Proc) TtcPageDown;
   MenuActionList[CMD_PageDown].User_Action = (UserProc) NULL;
   MenuActionList[CMD_PageDown].ActionEquiv = CST_EquivNext;

   MenuActionList[CMD_PageTop].ActionName = CST_PageTop;
   MenuActionList[CMD_PageTop].Call_Action = (Proc) TtcPageTop;
   MenuActionList[CMD_PageTop].User_Action = (UserProc) NULL;
   MenuActionList[CMD_PageTop].ActionEquiv = CST_EquivHome;

   MenuActionList[CMD_PageEnd].ActionName = CST_PageEnd;
   MenuActionList[CMD_PageEnd].Call_Action = (Proc) TtcPageEnd;
   MenuActionList[CMD_PageEnd].User_Action = (UserProc) NULL;
   MenuActionList[CMD_PageEnd].ActionEquiv = CST_EquivEnd;

   MenuActionList[CMD_CreateElement].ActionName = CST_CreateElement;
   MenuActionList[CMD_CreateElement].Call_Action = (Proc) NULL;
   MenuActionList[CMD_CreateElement].User_Action = (UserProc) NULL;

   MenuActionList[CMD_CopyToClipboard].ActionName = CST_CopyClipboard;
   MenuActionList[CMD_CopyToClipboard].Call_Action = (Proc) TtcCopyToClipboard;
   MenuActionList[CMD_CopyToClipboard].User_Action = (UserProc) NULL;

   MenuActionList[CMD_PasteFromClipboard].ActionName = CST_PasteClipboard;
   MenuActionList[CMD_PasteFromClipboard].Call_Action = (Proc) NULL;
   MenuActionList[CMD_PasteFromClipboard].User_Action = (UserProc) NULL;
}


/*----------------------------------------------------------------------
   TteAddMenuAction ajoute une nouvelle action dans la table des      
   actions d'interface.                                            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TteAddMenuAction (char *actionName, Proc procedure)

#else  /* __STDC__ */
void                TteAddMenuAction (actionName, procedure)
char               *actionName;
Proc                procedure;

#endif /* __STDC__ */
{
   char               *ptr;
   int                 lg;
   int                 i;

   if (actionName == NULL)
      return;			/* pas de nom d'action declare */

   lg = strlen (actionName);
   if (FreeMenuAction < MaxMenuAction && lg != 0)
     {
	/* Alloue une chaine de caractere pour le nom de l'action */
	ptr = TtaGetMemory (lg + 1);
	strcpy (ptr, actionName);
	MenuActionList[FreeMenuAction].ActionName = ptr;
	MenuActionList[FreeMenuAction].Call_Action = procedure;
	MenuActionList[FreeMenuAction].User_Action = (UserProc) NULL;
	MenuActionList[FreeMenuAction].ActionEquiv = NULL;
	/* Cette nouvelle action n'est active pour aucune frame */
	for (i = 0; i < MAX_FRAME; i++)
	   MenuActionList[FreeMenuAction].ActionActive[i] = FALSE;
	FreeMenuAction++;
     }
}

/*----------------------------------------------------------------------
   TteAddUserMenuAction add dynamically an user action in the table of the
   user interface actions. This may override or complete an existing
   built-in action, or override a previously defined user action.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                TteAddUserMenuAction (char *actionName, UserProc procedure,
                                      void *arg)

#else  /* __STDC__ */
int                TteAddUserMenuAction (actionName, procedure, arg)
char               *actionName;
UserProc            procedure;
void               *arg;

#endif /* __STDC__ */
{
   char               *ptr;
   int                 lg;
   int                 i;

   /*
    * We need a name !
    */
   if (actionName == NULL)
      return(-1);
   lg = strlen (actionName);
   if (lg == 0) return(-1);

   /*
    * Search in the menu actions table for a predefined action with this name
    */
   for (i = 0;i < FreeMenuAction;i++) {
       if (!strcmp(MenuActionList[i].ActionName, actionName)) {
           /*
	    * This action already exists, register the user procedure.
	    */
           MenuActionList[i].User_Action = procedure;
           MenuActionList[i].User_Arg = arg;
	   return(0);
       }
   }
   if (procedure == NULL) return(-1);

   /*
    * This action is not registered, try to allocate a new one.
    */
   if (FreeMenuAction < MaxMenuAction && lg != 0)
     {
	/* Dup' the action name string */
	ptr = TtaGetMemory (lg + 1);
	strcpy (ptr, actionName);
	MenuActionList[FreeMenuAction].ActionName = ptr;
	MenuActionList[FreeMenuAction].Call_Action = (Proc) NULL;
	MenuActionList[FreeMenuAction].User_Action = procedure;
	MenuActionList[FreeMenuAction].User_Arg = arg;
	MenuActionList[FreeMenuAction].ActionEquiv = NULL;
	/* desactivate this action for all frames */
	for (i = 0; i < MAX_FRAME; i++)
	   MenuActionList[FreeMenuAction].ActionActive[i] = FALSE;
	FreeMenuAction++;
	return(1);
     }
   return(-1);
}


/*----------------------------------------------------------------------
   FindMenuAction recherche l'action dans la table des actions        
   d'interface.                                                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          FindMenuAction (char *actionName)

#else  /* __STDC__ */
static int          FindMenuAction (actionName)
char               *actionName;

#endif /* __STDC__ */
{
   int                 i;

   for (i = 0; i < MaxMenuAction; i++)
     {
	if (!strcmp (actionName, MenuActionList[i].ActionName))
	   return (i);
     }
   return (i);
}


/*----------------------------------------------------------------------
   TteZeroMenu signale qu'il n'y a pas de menu dans ce type de        
   fenentre.                                                       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TteZeroMenu (WindowType windowtype, char *schemaName)

#else  /* __STDC__ */
void                TteZeroMenu (windowtype, schemaName)
WindowType          windowtype;
char               *schemaName;

#endif /* __STDC__ */
{
   SchemaMenu_Ctl     *ptrschema;
   boolean             ok;

   if (windowtype == DocTypeWindow)
     {
	/* il s'agit d'un menu d'un schema particulier */
	if (SchemasMenuList == NULL)
	  {
	     /* creation et initialisation du contexte specifique au schema */
	     ptrschema = (SchemaMenu_Ctl *) TtaGetMemory (sizeof (SchemaMenu_Ctl));
	     ptrschema->SchemaName = TtaGetMemory (strlen (schemaName) + 1);
	     strcpy (ptrschema->SchemaName, schemaName);
	     ptrschema->SchemaMenu = NULL;
	     ptrschema->NextSchema = NULL;
	     SchemasMenuList = ptrschema;
	  }
	else
	  {
	     ptrschema = SchemasMenuList;
	     ok = strcmp (schemaName, ptrschema->SchemaName);
	     while (!ok && ptrschema->NextSchema != NULL)
	       {
		  ptrschema = ptrschema->NextSchema;
		  ok = strcmp (schemaName, ptrschema->SchemaName);
	       }

	     if (!ok)
	       {
		  /* creation et initialisation du contexte specifique au schema */
		  ptrschema->NextSchema = (SchemaMenu_Ctl *) TtaGetMemory (sizeof (SchemaMenu_Ctl));
		  ptrschema = ptrschema->NextSchema;
		  ptrschema->SchemaName = TtaGetMemory (strlen (schemaName) + 1);
		  strcpy (ptrschema->SchemaName, schemaName);
		  ptrschema->SchemaMenu = NULL;
		  ptrschema->NextSchema = NULL;
	       }
	  }
     }
}


/*----------------------------------------------------------------------
   TteAddMenu ajoute un nouveau menu pour le schema donne. Si le      
   nom de schema est Null, il s'agit des menus pris par defaut.    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TteAddMenu (WindowType windowtype, char *schemaName, int view, int menuID, int itemsNumber, char *menuName)

#else  /* __STDC__ */
void                TteAddMenu (windowtype, schemaName, view, menuID, itemsNumber, menuName)
WindowType          windowtype;
char               *schemaName;
int                 view;
int                 menuID;
int                 itemsNumber;
char               *menuName;

#endif /* __STDC__ */
{
   Menu_Ctl           *ptrmenu = NULL;
   Menu_Ctl           *newmenu;
   SchemaMenu_Ctl     *ptrschema;
   Item_Ctl           *ptr;
   int                 i;
   boolean             ok;

   /* Creation du nouveau menu */
   newmenu = (Menu_Ctl *) TtaGetMemory (sizeof (Menu_Ctl));
   newmenu->MenuID = menuID;
   newmenu->MenuView = view;
   newmenu->ItemsNb = itemsNumber;
   /* Enregistre les menus actifs */
   newmenu->MenuAttr = FALSE;
   newmenu->MenuSelect = FALSE;
   newmenu->MenuHelp = FALSE;
   if (!strcmp (menuName, "MenuAttribute"))
      newmenu->MenuAttr = TRUE;
   else if (!strcmp (menuName, "MenuSelection"))
      newmenu->MenuSelect = TRUE;
   else if (!strcmp (menuName, "MenuHelp"))
      newmenu->MenuHelp = TRUE;

   /* creation et initialisation de la table des items */
   ptr = (Item_Ctl *) TtaGetMemory (itemsNumber * sizeof (Item_Ctl));
   for (i = 0; i < itemsNumber; i++)
     {
	ptr[i].ItemID = -1;
	ptr[i].ItemAction = -1;
	ptr[i].ItemType = ' ';
     }
   newmenu->ItemsList = ptr;
   newmenu->NextMenu = NULL;

   /* Chainage du nouveau menu aux autres menus existants */
   switch (windowtype)
	 {
	    case MainWindow:
	       /* il s'agit d'un des menus principaux */
	       if (MainMenuList == NULL)
		 {
		    MainMenuList = newmenu;
		    ptrmenu = NULL;
		 }
	       else
		  ptrmenu = MainMenuList;	/* simple ajout du menu dans une liste */
	       break;

	    case DocWindow:
	       /* il s'agit d'un des menus pris par defaut */
	       if (DocumentMenuList == NULL)
		 {
		    DocumentMenuList = newmenu;
		    ptrmenu = NULL;
		 }
	       else
		  ptrmenu = DocumentMenuList;	/* simple ajout du menu dans une liste */
	       break;

	    case DocTypeWindow:
	       /* il s'agit d'un menu d'un schema particulier */
	       if (SchemasMenuList == NULL)
		 {
		    /* creation et initialisation du contexte specifique au schema */
		    ptrschema = (SchemaMenu_Ctl *) TtaGetMemory (sizeof (SchemaMenu_Ctl));
		    ptrschema->SchemaName = TtaGetMemory (strlen (schemaName) + 1);
		    strcpy (ptrschema->SchemaName, schemaName);
		    ptrschema->SchemaMenu = newmenu;
		    ptrschema->NextSchema = NULL;
		    ptrmenu = NULL;
		    SchemasMenuList = ptrschema;
		 }
	       else
		 {
		    ptrschema = SchemasMenuList;
		    ok = strcmp (schemaName, ptrschema->SchemaName);
		    while (ok && ptrschema->NextSchema != NULL)
		      {
			 ptrschema = ptrschema->NextSchema;
			 ok = strcmp (schemaName, ptrschema->SchemaName);
		      }

		    if (ok)
		      {
			 /* creation et initialisation du contexte specifique au schema */
			 ptrschema->NextSchema = (SchemaMenu_Ctl *) TtaGetMemory (sizeof (SchemaMenu_Ctl));
			 ptrschema = ptrschema->NextSchema;
			 ptrschema->SchemaName = TtaGetMemory (strlen (schemaName) + 1);
			 strcpy (ptrschema->SchemaName, schemaName);
			 ptrschema->SchemaMenu = newmenu;
			 ptrschema->NextSchema = NULL;
			 ptrmenu = NULL;
		      }
		    else
		       ptrmenu = ptrschema->SchemaMenu;		/* simple ajout du menu dans une liste */
		 }
	       break;
	 }

   if (ptrmenu != NULL)
     {
	/* Ajout du nouveau menu en fin de liste */
	while (ptrmenu->NextMenu != NULL)
	   ptrmenu = ptrmenu->NextMenu;
	ptrmenu->NextMenu = newmenu;
     }
}


/*----------------------------------------------------------------------
   TteAddSubMenu ajoute un sous-menu pour le schema donne.            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TteAddSubMenu (WindowType windowtype, char *schemaName, int menuID, int itemID, int itemsNumber)

#else  /* __STDC__ */
void                TteAddSubMenu (windowtype, schemaName, menuID, itemID, itemsNumber)
WindowType          windowtype;
char               *schemaName;
int                 menuID;
int                 itemID;
int                 itemsNumber;

#endif /* __STDC__ */
{
   Menu_Ctl           *ptrmenu;
   Menu_Ctl           *newmenu;
   SchemaMenu_Ctl     *ptrschema;
   Item_Ctl           *ptr, *ptrItem;
   int                 i, j;

   /* Recherche la bonne liste de menus */
   ptrmenu = NULL;
   switch (windowtype)
	 {
	    case MainWindow:
	       /* il s'agit d'un des menus principaux */
	       if (MainMenuList != NULL)
		  ptrmenu = MainMenuList;
	       break;

	    case DocWindow:
	       /* il s'agit d'un des menus pris par defaut */
	       if (DocumentMenuList != NULL)
		  ptrmenu = DocumentMenuList;
	       break;

	    case DocTypeWindow:
	       /* il s'agit d'un menu d'un schema particulier */
	       ptrschema = SchemasMenuList;
	       while (ptrschema != NULL && strcmp (schemaName, ptrschema->SchemaName))
		  ptrschema = ptrschema->NextSchema;
	       if (ptrschema != NULL)
		  ptrmenu = ptrschema->SchemaMenu;
	       break;
	 }

   /* Recherche le bon menu */
   while (ptrmenu != NULL && menuID != ptrmenu->MenuID)
      ptrmenu = ptrmenu->NextMenu;

   if (ptrmenu != NULL)
     {
	/* recherche l'item dans le menu */
	ptrItem = ptrmenu->ItemsList;
	j = 0;
	while (j < ptrmenu->ItemsNb && ptrItem[j].ItemType != ' ')
	   j++;
	if (j < ptrmenu->ItemsNb)
	  {
	     /* Creation du sous-menu */
	     newmenu = (Menu_Ctl *) TtaGetMemory (sizeof (Menu_Ctl));
	     newmenu->MenuID = 0;
	     newmenu->MenuView = 0;
	     newmenu->ItemsNb = itemsNumber;
	     newmenu->MenuAttr = FALSE;
	     newmenu->MenuSelect = FALSE;
	     newmenu->MenuHelp = FALSE;

	     /* creation et initialisation de la table des items */
	     ptr = (Item_Ctl *) TtaGetMemory (itemsNumber * sizeof (Item_Ctl));
	     for (i = 0; i < itemsNumber; i++)
	       {
		  ptr[i].ItemID = -1;
		  ptr[i].ItemAction = -1;
		  ptr[i].ItemType = ' ';
	       }
	     newmenu->ItemsList = ptr;
	     newmenu->NextMenu = NULL;
	     /* relie le sous-menu a l'item */
	     ptrItem[j].SubMenu = newmenu;
	     ptrItem[j].ItemID = itemID;
	     ptrItem[j].ItemType = 'M';
	  }
     }
}


/*----------------------------------------------------------------------
   TteAddMenuItem ajoute une nouvel item dans un menu.                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TteAddMenuItem (WindowType windowtype, char *schemaName, int menuID, int subMenu, int itemID, char *actionName, char itemType)

#else  /* __STDC__ */
void                TteAddMenuItem (windowtype, schemaName, menuID, subMenu, itemID, actionName, itemType)
WindowType          windowtype;
char               *schemaName;
int                 menuID;
int                 subMenu;
int                 itemID;
char               *actionName;
char                itemType;

#endif /* __STDC__ */
{
   Menu_Ctl           *ptrmenu;
   SchemaMenu_Ctl     *ptrschema;
   Item_Ctl           *ptr;
   int                 i;

   /* Recherche la bonne liste de menus */
   ptrmenu = NULL;
   switch (windowtype)
	 {
	    case MainWindow:
	       /* il s'agit d'un des menus principaux */
	       if (MainMenuList != NULL)
		  ptrmenu = MainMenuList;
	       break;

	    case DocWindow:
	       /* il s'agit d'un des menus pris par defaut */
	       if (DocumentMenuList != NULL)
		  ptrmenu = DocumentMenuList;
	       break;

	    case DocTypeWindow:
	       /* il s'agit d'un menu d'un schema particulier */
	       ptrschema = SchemasMenuList;
	       while (ptrschema != NULL && strcmp (schemaName, ptrschema->SchemaName))
		  ptrschema = ptrschema->NextSchema;
	       if (ptrschema != NULL)
		  ptrmenu = ptrschema->SchemaMenu;
	       break;
	 }

   /* Recherche le menu */
   while (ptrmenu != NULL && menuID != ptrmenu->MenuID)
      ptrmenu = ptrmenu->NextMenu;

   if (ptrmenu != NULL && subMenu != -1)
     {
	/* Recherche l'entree du sous-menu dans le menu */
	i = 0;
	ptr = ptrmenu->ItemsList;
	while (i < ptrmenu->ItemsNb && (ptr[i].ItemID != subMenu))
	   i++;
	if (i < ptrmenu->ItemsNb)
	   ptrmenu = ptr[i].SubMenu;
	else
	   /* on n'a pas trouve le sous-menu */
	   return;
     }

   /* ajoute l'item dans le menu */
   i = 0;
   ptr = ptrmenu->ItemsList;
   while (i < ptrmenu->ItemsNb && ptr[i].ItemType != ' ')
      i++;

   if (i < ptrmenu->ItemsNb)
     {
	ptr[i].ItemID = itemID;
	ptr[i].ItemType = itemType;
	if (actionName != NULL)
	   ptr[i].ItemAction = FindMenuAction (actionName);
     }
}


/*----------------------------------------------------------------------
   mmtopixel convertit des mm en pixels.                           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          mmtopixel (int N, int horiz)
#else  /* __STDC__ */
static int          mmtopixel (N, horiz)
int                 N;
int                 horiz;

#endif /* __STDC__ */
{
   if (horiz)
      return (DOT_PER_INCHE * N * 10) / 254;
   else
      return (DOT_PER_INCHE * N * 10) / 254;
}


/*----------------------------------------------------------------------
   BuildSubmenu construit un sous-menu attache' a l'item item     
   du menu ref.                                            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         BuildSubMenu (Menu_Ctl * ptrmenu, int ref, int entry, int frame)
#else  /* __STDC__ */
static void         BuildSubMenu (ptrmenu, ref, entry, frame)
Menu_Ctl           *ptrmenu;
int                 ref;
int                 entry;
int                 frame;

#endif /* __STDC__ */
{
   int                 i, j;
   int                 lg, sref;
   int                 item;
   int                 action;
   char                string[700];
#define MaxEquivLen 200
   char                equiv[MaxEquivLen];
   boolean             withEquiv;
   Item_Ctl           *ptritem;
   char               *ptr;

   /* Construit le sous-menu attache a l'item */
#  ifdef _WINDOWS
   currentFrame = frame;
#  endif /* _WINDOWS */
   item = 0;
   i = 0;
   j = 0;
   withEquiv = FALSE;
   equiv[0] = EOS;
   ptritem = ptrmenu->ItemsList;
   while (item < ptrmenu->ItemsNb)
     {
	/* Regarde si le texte des commandes ne deborde pas */
	ptr = TtaGetMessage (THOT, ptritem[item].ItemID);
	lg = strlen (ptr) + 1;
	if (ptritem[item].ItemType == 'S' && i + 2 < 700)
	  {
	     strcpy (&string[i], "S");
	     i += 2;
	  }
	else if (i + lg < 699)
	  {
	     if (ptritem[item].ItemType == 'D')
		string[i] = 'B';
	     else
		string[i] = ptritem[item].ItemType;
	     strcpy (&string[i + 1], ptr);
	     i += lg + 1;
	  }
	else
	   /* sinon on reduit le nombre d'items */
	   ptrmenu->ItemsNb = item - 1;

	/* traite le contenu de l'item de menu */
	action = ptritem[item].ItemAction;
	if (action != -1)
	  {
	     /* Active l'action correspondante pour cette fenetre */
	     if (MenuActionList[action].ActionEquiv != NULL)
	       {
		  withEquiv = TRUE;
		  lg = strlen (MenuActionList[action].ActionEquiv);
		  if (lg + j < MaxEquivLen)
		    {
		       strcpy (&equiv[j], MenuActionList[action].ActionEquiv);
		       j += lg;
		    }
	       }
	     MenuActionList[action].ActionActive[frame] = TRUE;
	  }
	equiv[j++] = EOS;
	item++;
     }
   sref = ((entry + 1) * MAX_MENU * MAX_ITEM) + ref;
   /* Creation du Pulldown avec ou sans equiv */
   if (withEquiv)
      TtaNewSubmenu (sref, ref, entry, NULL, ptrmenu->ItemsNb, string, equiv, FALSE);
   else
      TtaNewSubmenu (sref, ref, entry, NULL, ptrmenu->ItemsNb, string, NULL, FALSE);
}


/*----------------------------------------------------------------------
   BuildPopdown construit un menu popdown attache' au bouton      
   de menu.                                                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         BuildPopdown (Menu_Ctl * ptrmenu, int ref, ThotWidget button, int frame)
#else  /* __STDC__ */
static void         BuildPopdown (ptrmenu, ref, button, frame)
Menu_Ctl           *ptrmenu;
int                 ref;
ThotWidget          button;
int                 frame;

#endif /* __STDC__ */
{
   int                 i, j;
   int                 lg;
   int                 item;
   int                 action;
   char                string[700];
   char                equiv[MaxEquivLen];
   boolean             withEquiv;
   Item_Ctl           *ptritem;
   char               *ptr;

#  ifdef _WINDOWS
   currentFrame = frame;
#  endif /* _WINDOWS */
   /* Construit le pulldown attache au bouton */
   item = 0;
   i = 0;
   j = 0;
   withEquiv = FALSE;
   equiv[0] = EOS;
   ptritem = ptrmenu->ItemsList;
   while (item < ptrmenu->ItemsNb)
     {
	action = ptritem[item].ItemAction;
	/* Regarde si le texte des commandes ne deborde pas */
	ptr = TtaGetMessage (THOT, ptritem[item].ItemID);
	lg = strlen (ptr) + 1;
	if (ptritem[item].ItemType == 'S' && i + 2 < 700)
	  {
	     strcpy (&string[i], "S");
	     i += 2;
	  }
	else if (i + lg < 699)
	  {
	     if (ptritem[item].ItemType == 'D')
		string[i] = 'B';
	     else
		string[i] = ptritem[item].ItemType;
	     strcpy (&string[i + 1], ptr);
	     i += lg + 1;
	  }
	else
	   /* sinon on reduit le nombre d'items */
	   ptrmenu->ItemsNb = item - 1;

	/* traite le contenu de l'item de menu */
	if (action != -1)
	  {
	     if (ptritem[item].ItemType == 'B' || ptritem[item].ItemType == 'T')
	       {
		  /* Active l'action correspondante pour cette fenetre */
		  if (MenuActionList[action].ActionEquiv != NULL)
		    {
		       withEquiv = TRUE;
		       lg = strlen (MenuActionList[action].ActionEquiv);
		       if (lg + j < MaxEquivLen)
			 {
			    strcpy (&equiv[j], MenuActionList[action].ActionEquiv);
			    j += lg;
			 }
		    }
		  MenuActionList[action].ActionActive[frame] = TRUE;
	       }
	  }
	equiv[j++] = EOS;
	item++;
     }

#  ifdef _WINDOWS
   currentMenu = button ;
#  endif /* _WINDOWS */

   /* Creation du Pulldown avec ou sans equiv */
   if (withEquiv)
      TtaNewPulldown (ref, button, NULL, ptrmenu->ItemsNb, string, equiv);
   else
      TtaNewPulldown (ref, button, NULL, ptrmenu->ItemsNb, string, NULL);

   /* traite les sous-menus de l'item de menu */
   item = 0;
   ptritem = ptrmenu->ItemsList;
   while (item < ptrmenu->ItemsNb)
     {
	action = ptritem[item].ItemAction;
	if (action != -1)
	  {
	     if (ptritem[item].ItemType == 'M')
	       {
		  if (action != 0 && item < MAX_MENU)
		     /* creation du sous-menu */
		     BuildSubMenu (ptritem[item].SubMenu, ref, item, frame);
	       }
	  }
	item++;
     }
}

#ifndef _WIN_PRINT
/*----------------------------------------------------------------------
   TteOpenMainWindow opens the application main window.

   Parameters:
   name: the name to be displayed as the title of the main window.
   logo: the logo pixmap to be displayed in the window or NULL.
   icon: the icon pixmap to be displayed in the window or NULL.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TteOpenMainWindow (char *name, Pixmap logo, Pixmap icon)
#else  /* __STDC__ */
void                TteOpenMainWindow (name, logo, icon)
char               *name;
Pixmap              logo;
Pixmap              icon;

#endif /* __STDC__ */
{
   int                 i, n;
   int                 ref;
   int                 lg;
   char                string[700];
   Menu_Ctl           *ptrmenu;
   char               *ptr;

   /* Creation de la fenetre principale */
   UserErrorCode = 0;
   TtaInitDialogueTranslations (InitTranslations (name));
   TteLoadApplications ();
#  ifndef _WINDOWS
   if (TtDisplay == 0)
     {
	/* Connexion au serveur X impossible */
	TtaError (ERR_cannot_open_main_window);
	exit (1);
     }
   else
#  endif /* !_WINDOWS */
     {
	/* Police de caracteres utilisee dans les menus */
#       ifndef _WINDOWS
       /*DefaultFont = XmFontListCreate ((XFontStruct *) ThotLoadFont ('L', 'H', 0, MenuSize, UnPoint, 0), XmSTRING_DEFAULT_CHARSET);*/
#       endif
	/* Compte le nombre de menus a creer */
	n = 0;
	i = 0;
	ptrmenu = MainMenuList;
	while (ptrmenu != NULL)
	  {
	     n++;
	     /* Regarde si le texte des commandes ne deborde pas */

	     /*
	        ptr = TtaGetMessage(EDIT_DIALOG, ptrmenu->MenuID);
	      */
	     ptr = TtaGetMessage (THOT, ptrmenu->MenuID);
	     lg = strlen (ptr) + 1;
	     if (i + lg < 700)
	       {
		  strcpy (&string[i], ptr);
		  i += lg;
		  ptrmenu = ptrmenu->NextMenu;
	       }
	     else
		/* sinon on reduit le nombre de menus */
		ptrmenu = NULL;
	  }

/**** creation de la fenetre principale ****/
	if (n == 0)
	  {
	     WithMessages = FALSE;
	     TtaInitDialogueWindow (name, NULL, None, None, 0, NULL);
	  }
	else
	  {
	     WithMessages = TRUE;
	     TtaInitDialogueWindow (name, NULL, logo, icon, n, string);
	  }

	/* icone des fenetres de documents */
#       ifndef _WINDOWS
	wind_pixmap = XCreateBitmapFromData (TtDisplay, XDefaultRootWindow (TtDisplay),
		      logowindow_bits, logowindow_width, logowindow_height);
#       endif
/**** creation des menus ****/
	ptrmenu = MainMenuList;
	FrameTable[0].FrMenus = ptrmenu;
	ref = MAX_LocalMenu;	/* reference du menu construit */
	i = 0;
	while (ptrmenu != NULL)
	  {
	     /* Enregistre le widget du menu */
	     FrameTable[0].ActifMenus[i] = TRUE;
	     BuildPopdown (ptrmenu, ref, FrameTable[0].WdMenus[i], 0);
	     ptrmenu = ptrmenu->NextMenu;
	     ref += MAX_ITEM;
	     i++;
	  }

	/* Les autres entrees de menus sont inactives */
	while (i < MAX_MENU)
	  {
	     FrameTable[0].ActifMenus[i] = FALSE;
	     i++;
	  }
     }
}
#endif /* _WIN_PRINT */
/*----------------------------------------------------------------------
   ButtonAction                                                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void APP_ButtonCallback (ThotWidget w, int frame, caddr_t call_d)

#else  /* __STDC__ */
void APP_ButtonCallback (w, frame, call_d)
ThotWidget          w;
int                 frame;
caddr_t             call_d;

#endif /* __STDC__ */
{
   Document            document;
   View                view;
   int                 i;

   i = 0;
   while (i < MAX_BUTTON && FrameTable[frame].Button[i] != w)
      i++;
   if (i < MAX_BUTTON)
     {
#ifdef _WINDOWS
	doSwitchButton = FALSE;
#endif /* _WINDOWS */
	CloseInsertion ();
	FrameToView (frame, &document, &view);
	(*FrameTable[frame].Call_Button[i]) (document, view);
     }
#ifdef _WINDOWS
   doSwitchButton = TRUE;
#endif /* _WINDOWS */
}

#ifndef _WINDOWS
/*----------------------------------------------------------------------
   InitClue

   Initialize the liteClue Widget for the application, handling the
   tooltips on buttons.

   Parameters:
   toplevel: the application toplevel Shell.
  ----------------------------------------------------------------------*/

static ThotWidget liteClue = NULL;

#ifdef __STDC__
void                InitClue (ThotWidget toplevel)
#else  /* __STDC__ */
void                InitClue (toplevel)
ThotWidget          toplevel;

#endif /* __STDC__ */
{
   Arg                 args[MAX_ARGS];
   int                 n;
   int                 wait_ms = 500; /* 500 ms i.e. 1/2 second */
   char                *user_delay;
   ThotColor           bg;

   if (liteClue != NULL) return;
   liteClue = XtVaCreatePopupShell("popup_shell", xcgLiteClueWidgetClass,
                                   toplevel, NULL);

   
   user_delay = TtaGetEnvString("TOOLTIPDELAY");
   if (user_delay != NULL) {
       if (sscanf(user_delay,"%d",&wait_ms) != 1) {
           TtaSetEnvString ("TOOLTIPDELAY", "500", TRUE);
	   wait_ms = 500;
       }
   }
   bg = ColorPixel(ColorNumber("yellow"));
   n = 0;
   XtSetArg (args[n], XtNbackground, bg);
   n++;
   XtSetArg (args[n], XtNfont, DefaultFont);
   n++;
   XtSetArg (args[n], XgcNwaitperiod, wait_ms);
   n++;
   XtSetValues (liteClue, args, n);
}
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
   TtaAddButton

   Adds a new button entry in a document view.
   This function must specify a valid view of a valid document.

   Parameters:
   document: the concerned document.
   view: the concerned view.
   picture: label of the new entry. None creates a space between buttons.
   procedure: procedure to be executed when the new entry is
   selected by the user. Null creates a cascade button.
   info: text to display when the cursor stays on the button.
   Returns index
  ----------------------------------------------------------------------*/
#ifdef _WINDOWS
#ifdef __STDC__
int WIN_TtaAddButton (Document document, View view, int picture, void (*procedure) (), char *info, BYTE type, BOOL state)
#else  /* __STDC__ */
int WIN_TtaAddButton (document, view, picture, procedure, info, type, state)
Document document;
View     view;
int      picture;
void     (*procedure) ();
char*    info;
BYTE     type;
BOOL     state;
#endif /* __STDC__ */
#else  /* !_WINDOWS */
#ifdef __STDC__
int                 TtaAddButton (Document document, View view, Pixmap picture, void (*procedure) (), char *info)
#else  /* __STDC__ */
int                 TtaAddButton (document, view, picture, procedure, info)
Document            document;
View                view;
Pixmap              picture;
void                (*procedure) ();
char               *info;
#endif /* __STDC__ */
#endif /* _WINDOWS */
{
   int                 frame, i, index;

#  ifndef _WINDOWS
   int                 n;
   XmString            title_string;
   Arg                 args[MAX_ARGS];
   ThotWidget          w, row;
#  else  /* _WINDOWS */
   TBBUTTON* w;
#  endif

   UserErrorCode = 0;
   index = 0;
   /* verifie le parametre document */
   if (document == 0 && view == 0)
      TtaError (ERR_invalid_parameter);
   else
     {
	frame = GetWindowNumber (document, view);
	if (frame == 0 || frame > MAX_FRAME)
	   TtaError (ERR_invalid_parameter);
	else if (FrameTable[frame].WdFrame != 0)
	  {
	     i = 0;
	     while (i < MAX_BUTTON && FrameTable[frame].Button[i] != 0)
               i++;
	     if (i < MAX_BUTTON)
	       {

		  /* Insere le nouveau bouton */
#                 ifndef _WINDOWS
		  row = FrameTable[frame].Button[0];
		  n = 0;
		  XtSetArg (args[n], XmNmarginWidth, 0);
		  n++;
		  XtSetArg (args[n], XmNmarginHeight, 0);
		  n++;
		  XtSetArg (args[n], XmNbackground, BgMenu_Color);
		  n++;
		  XtSetArg (args[n], XmNtraversalOn, FALSE);
		  n++;
		  if (picture == None)
		    {
		       /* insere une chaine vide */
		       title_string = XmStringCreateSimple ("  ");
		       XtSetArg (args[n], XmNlabelString, title_string);
		       n++;
		       XtSetArg (args[n], XmNforeground, Black_Color);
		       n++;
		       XtSetArg (args[n], XmNheight, (Dimension) 30);
		       n++;
		       w = XmCreateLabel (row, "Logo", args, n);
		       XtManageChild (w);
		       XmStringFree (title_string);
		    }
		  else
		    {
		       /* insere l'icone du bouton */
		       XtSetArg (args[n], XmNlabelType, XmPIXMAP);
		       n++;
		       XtSetArg (args[n], XmNlabelPixmap, picture);
		       n++;
		       if (procedure == NULL)
			 {
			    w = XmCreateCascadeButton (row, "dialogue", args, n);
			    XtManageChild (w);
			 }
		       else
			 {
			    w = XmCreatePushButton (row, "dialogue", args, n);
			    XtManageChild (w);
			    XtAddCallback (w, XmNactivateCallback, (XtCallbackProc) APP_ButtonCallback, (XtPointer) frame);
			    FrameTable[frame].Call_Button[i] = (Proc) procedure;
			 }
		    }
		  FrameTable[frame].Button[i] = w;
		  index = i;
		  /* force la mise a jour de la fenetre */
		  XtManageChild (row);
#                 else  /* _WINDOWS */
		  index = i;
                  if (procedure) {
                     w = (TBBUTTON*) TtaGetMemory (sizeof (TBBUTTON));
					 if (!w)
                        WinErrorBox (NULL);
					 else {
                         w->iBitmap   = picture;
                         w->idCommand = TBBUTTONS_BASE + i; 
                         w->fsState   = TBSTATE_ENABLED;
                         w->fsStyle   = type;
						 w->bReserved[0] = 0;
						 w->bReserved[1] = 0;
                         w->dwData    = 0;
                         w->iString   = 0;
#                        ifdef THOT_TOOLTIPS
                         CommandToString[tipIndex++] = TBBUTTONS_BASE + i;
                         CommandToString[tipIndex]   = -1;
			             nCust [frame][i] = i;
#                        endif /* THOT_TOOLTIPS */
                         FrameTable[frame].Button[i] = w;
                         FrameTable[frame].Call_Button[i] = (Proc) procedure;

                         ToolBar_InsertButton (WinToolBar[frame], i, w);
                         SendMessage (WinToolBar[frame], TB_ENABLEBUTTON, (WPARAM) (index + TBBUTTONS_BASE), (LPARAM) MAKELONG (state, 0));
					 }
                  } else {
                        w = (TBBUTTON*) TtaGetMemory (sizeof (TBBUTTON));
                        w->iBitmap   = 0;
                        w->idCommand = 0; 
                        w->fsState   = TBSTATE_ENABLED;
                        w->fsStyle   = type;
                        w->dwData    = 0;
                        w->iString   = 0;
                        FrameTable[frame].Button[i] = w;
                        FrameTable[frame].Call_Button[i] = (Proc) procedure;
                        ToolBar_InsertButton (WinToolBar[frame], i, w);
                  }
#                 endif /* _WINDOWS */
                  if (info != NULL) {
#                    ifdef _WINDOWS
#            ifdef THOT_TOOLTIPS
		     strcat (&szTbStrings [strIndex], info);
		     strIndex += (strlen (info) + 1);
#            endif /* THOT_TOOLTIPS */
#                    else  /* !_WINDOWS */
		     XcgLiteClueAddWidget(liteClue, w,  info, strlen(info), 0);
#                    endif /* _WINDOWS */
                  }
	       }
	  }
     }

   TtaHandlePendingEvents ();
   return (index);
}				/*TtaAddButton */


/*----------------------------------------------------------------------
   TtaGetButtonCallback

   Get the callback of a button in a document view.
   Returns the callback if it exists
           NULL if it doesn't exists

   Parameters:
   document: the concerned document.
   view: the concerned view.
   index: the index.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void *              TtaGetButtonCallback (Document document, View view, int index)
#else  /* __STDC__ */
void *              TtaGetButtonCallback (document, view, index)
Document            document;
View                view;
int                 index;

#endif /* __STDC__ */
{
   int                 frame;

   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document == 0 && view == 0)
      return(NULL);
   else
     {
	frame = GetWindowNumber (document, view);
	if (frame == 0 || frame > MAX_FRAME)
	   return(NULL);
	else if (FrameTable[frame].WdFrame != 0)
	  {
	     if (index >= MAX_BUTTON || index <= 0
		 || FrameTable[frame].Button[index] == 0)
		return(FrameTable[frame].Call_Button[index]);
	     else
	        return(NULL);
	  }
     }
   return(NULL);
}


/*----------------------------------------------------------------------
   TtaSwitchButton

   Change the status of the button entry in a document view.
   This function must specify a valid view of a valid document.

   Parameters:
   document: the concerned document.
   view: the concerned view.
   index: the index.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaSwitchButton (Document document, View view, int index)
#else  /* __STDC__ */
void                TtaSwitchButton (document, view, index)
Document            document;
View                view;
int                 index;

#endif /* __STDC__ */
{
   int                 frame;

#  ifndef _WINDOWS
   int                 n;
   Pixel               top, bottom;
   Arg                 args[MAX_ARGS];

#  endif

   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document == 0 && view == 0)
      TtaError (ERR_invalid_parameter);
   else
     {
	frame = GetWindowNumber (document, view);
	if (frame == 0 || frame > MAX_FRAME)
	   TtaError (ERR_invalid_parameter);
	else if (FrameTable[frame].WdFrame != 0)
	  {
	     if (index >= MAX_BUTTON || index <= 0
		 || FrameTable[frame].Button[index] == 0)
		TtaError (ERR_invalid_parameter);
	     else
	       {
		  /* Change l'etat du bouton */
#                 ifdef _WINDOWS
		  if (doSwitchButton) {
             if (!SendMessage (WinToolBar[frame], TB_ISBUTTONCHECKED, (WPARAM) (index + TBBUTTONS_BASE), (LPARAM) 0))
                SendMessage (WinToolBar[frame], TB_CHECKBUTTON, (WPARAM) (index + TBBUTTONS_BASE), (LPARAM) MAKELONG (TRUE, 0));
		     else
                SendMessage (WinToolBar[frame], TB_CHECKBUTTON, (WPARAM) (index + TBBUTTONS_BASE), (LPARAM) MAKELONG (FALSE, 0));
		  }
#                 else  /* !_WINDOWS */
		  n = 0;
		  XtSetArg (args[n], XmNtopShadowColor, &top);
		  n++;
		  XtSetArg (args[n], XmNbottomShadowColor, &bottom);
		  n++;
		  XtGetValues (FrameTable[frame].Button[index], args, n);
		  n = 0;
		  XtSetArg (args[n], XmNtopShadowColor, bottom);
		  n++;
		  XtSetArg (args[n], XmNbottomShadowColor, top);
		  n++;
		  XtSetValues (FrameTable[frame].Button[index], args, n);
#                 endif /* _WINDOWS */
	       }
	  }
     }
}


/*----------------------------------------------------------------------
   TtaChangeButton

   Change the button entry in a document view.
   This function must specify a valid view of a valid document.

   Parameters:
   document: the concerned document.
   view: the concerned view.
   index: the index.
   picture: the new icon.
  ----------------------------------------------------------------------*/
#ifdef _WINDOWS
#ifdef __STDC__
void                WIN_TtaChangeButton (Document document, View view, int index, int picture, BOOL state)
#else  /* __STDC__ */
void                WIN_TtaChangeButton (document, view, index, picture, state)
Document            document;
View                view;
int                 index;
int                 picture;
BOOL                state;
#endif /* __STDC__ */
#else  /* !_WINDOWS */
#ifdef __STDC__
void                TtaChangeButton (Document document, View view, int index, Pixmap picture)
#else  /* __STDC__ */
void                TtaChangeButton (document, view, index, picture)
Document            document;
View                view;
int                 index;
Pixmap              picture;

#endif /* __STDC__ */
#endif /* _WINDOWS */
{

#  ifndef _WINDOWS
   Arg                 args[MAX_ARGS];
   int                 n;
#  endif
   int                 frame;

   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document == 0 && view == 0)
      TtaError (ERR_invalid_parameter);
#  ifndef _WINDOWS
   else if (picture == None)
      TtaError (ERR_invalid_parameter);
#  endif /* !_WINDOWS */
   else
     {
	frame = GetWindowNumber (document, view);
	if (frame == 0 || frame > MAX_FRAME)
	   TtaError (ERR_invalid_parameter);
	else if (FrameTable[frame].WdFrame != 0)
	  {
	     if (index >= MAX_BUTTON || index <= 0
		 || FrameTable[frame].Button[index] == 0)
		TtaError (ERR_invalid_parameter);
	     else
	       {
		  /* Insere le nouvel icone */
#                 ifdef _WINDOWS
          SendMessage (WinToolBar[frame], TB_ENABLEBUTTON, (WPARAM) (index + TBBUTTONS_BASE - 1), (LPARAM) MAKELONG (state, 0));
#                 else  /* !_WINDOWS */
		  n = 0;
		  XtSetArg (args[n], XmNlabelPixmap, picture);
		  n++;
		  XtSetValues (FrameTable[frame].Button[index], args, n);
#                 endif /* _WINDOWS */
	       }
	  }
     }
}				/*TtaChangeButton */

/*----------------------------------------------------------------------
   TtcSwitchButtonBar

   Shows the buttonbar in a document view.
   This function must specify a valid view of a valid document.

   Parameters:
   document: identifier of the document.
   view: identifier of the view.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcSwitchButtonBar (Document document, View view)
#else  /* __STDC__ */
void                TtcSwitchButtonBar (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   int                 frame;
#  ifndef _WINDOWS
   Dimension           dy;
   Arg                 args[MAX_ARGS];
   ThotWidget          row;
#  endif
#  ifdef _WINDOWS
   RECT r;
#  endif /* _WINDOWS */

   UserErrorCode = 0;
   frame = 0;
   /* verifie le parametre document */
   if (document == 0 && view == 0)
      TtaError (ERR_invalid_parameter);
   else
     {
	frame = GetWindowNumber (document, view);
	if (frame == 0 || frame > MAX_FRAME)
	  {
	     TtaError (ERR_invalid_parameter);
	     return;
	  }
	else if (FrameTable[frame].WdFrame == 0)
	   return;
     }

#  ifndef _WINDOWS
   row = FrameTable[frame].Button[0];
   XtSetArg (args[0], XmNheight, &dy);
   if (row != 0)
     {
	XtUnmanageChild (XtParent (XtParent (row)));
	if (XtIsManaged (row))
	  {
	     XtGetValues (row, args, 1);
	     XtUnmanageChild (row);
	     dy = -dy;
	     XtUnmanageChild (XtParent (row));
	  }
	else
	  {
	     XtManageChild (row);
	     XtGetValues (row, args, 1);
	  }

	/*FrameResized((int *)w, frame, NULL); */
	XtManageChild (XtParent (row));
	XtManageChild (XtParent (XtParent (row)));
     }
#  else  /* _WINDOWS */
   if (WinToolBar[frame] && IsWindowVisible (WinToolBar[frame])) {
      hmenu = GetMenu (FrMainRef[frame]); 
      CheckMenuItem (hmenu, menu_item, MF_BYCOMMAND | MF_UNCHECKED); 
      ShowWindow (WinToolBar[frame], SW_HIDE);
   } else {
        hmenu = GetMenu (FrMainRef[frame]); 
        CheckMenuItem (hmenu, menu_item, MF_BYCOMMAND | MF_CHECKED); 
        ShowWindow (WinToolBar[frame], SW_SHOW);
   }
   /* Resize other windows */
   GetClientRect (FrMainRef [frame], &r);
   PostMessage (FrMainRef [frame], WM_SIZE, 0, MAKELPARAM (r.right, r.bottom));
#  endif /* _WINDOWS */
   /* force la mise a jour de la fenetre */
   TtaHandlePendingEvents ();
}				/*TtcSwitchButtonBar */


/*----------------------------------------------------------------------
   TextAction                                                      
  ----------------------------------------------------------------------*/
#ifdef _WINDOWS
#ifdef __STDC__
void WIN_APP_TextCallback (HWND w, int frame)
#else  /* __STDC__ */
void WIN_APP_TextCallback (w, rame)
HWND w; 
int  frame;
#endif /* __STDC__ */
#else  /* !_WINDOWS */
#ifdef __STDC__
static void         APP_TextCallback (ThotWidget w, int frame, XmTextVerifyCallbackStruct * call_d)
#else  /* __STDC__ */
static void         APP_TextCallback (w, frame, call_d)
ThotWidget          w;
int                 frame;
XmTextVerifyCallbackStruct *call_d;
#endif /* __STDC__ */
#endif /* _WINDOWS */
{
   Document            document;
   View                view;
   int                 i;
#  ifndef _WINDOWS
   char               *text;
#  else  /* _WINDOWS */
   static char text [1024];
#  endif /* _WINDOWS */

   CloseInsertion ();
   i = 0;
   while (i < MAX_TEXTZONE && FrameTable[frame].Text_Zone[i] != w)
      i++;
   if (i < MAX_TEXTZONE)
     {
	FrameToView (frame, &document, &view);
#   ifndef _WINDOWS
	text = XmTextGetString (w);
#   else  /* _WINDOWS */
	GetWindowText (w, text, sizeof (text) + 1);
#   endif /* _WINDOWS */
	(*FrameTable[frame].Call_Text[i]) (document, view, text);
     }
}

/*----------------------------------------------------------------------
   TtaAddTextZone

   Adds a new textual command in a document view.
   This function must specify a valid view of a valid document.

   Parameters:
   document: the concerned document.
   view: the concerned view.
   label: label of the new entry.
   procedure: procedure to be executed when the new entry is changed by the
   user.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 TtaAddTextZone (Document document, View view, char *label, boolean editable, void (*procedure) ())
#else  /* __STDC__ */
int                 TtaAddTextZone (document, view, label, editable, procedure)
Document            document;
View                view;
char               *label;
boolean             editable;
void                (*procedure) ();

#endif /* __STDC__ */
{
   int                 frame, i;
   ThotWidget          w, row;
#  ifndef _WINDOWS
   int                 n;
   ThotWidget          rowh;
   ThotWidget         *brother;
   XmString            title_string;
   Arg                 args[MAX_ARGS];
#  endif /* _WINDOWS */
#  ifdef _WINDOWS
   RECT       rect;
   ThotWidget wLabel;
#  endif /* _WINDOWS */

   UserErrorCode = 0;
   i = 0;
   w = 0;
   /* verifie le parametre document */
   if (document == 0 && view == 0)
      TtaError (ERR_invalid_parameter);
   else
     {
	frame = GetWindowNumber (document, view);
	if (frame == 0 || frame > MAX_FRAME)
	   TtaError (ERR_invalid_parameter);
	else if (FrameTable[frame].WdFrame != 0)
	  {
	     i = 0;
	     while (i < MAX_TEXTZONE && FrameTable[frame].Text_Zone[i] != 0)
		i++;
	     if (i < MAX_TEXTZONE)
	       {
		  row = FrameTable[frame].Text_Zone[0];
		  /*XtManageChild(row); */
#                 ifndef _WINDOWS
		  XtUnmanageChild (XtParent (XtParent (row)));

		  /* Insere la nouvelle zone de texte */
		  n = 0;
		  XtSetArg (args[n], XmNchildren, &brother);
		  n++;
		  XtGetValues (row, args, n);

		  n = 0;
		  XtSetArg (args[n], XmNmarginWidth, 0);
		  n++;
		  XtSetArg (args[n], XmNmarginHeight, 0);
		  n++;
		  XtSetArg (args[n], XmNbackground, BgMenu_Color);
		  n++;
		  XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);
		  n++;
		  if (brother == NULL)
		    {
		       XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);
		       n++;
		    }
		  else
		    {
		       XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);
		       n++;
		       XtSetArg (args[n], XmNtopWidget, *brother);
		       n++;
		       XtSetArg (args[n], XmNbottomWidget, *brother);
		       n++;
		    }
		  XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);
		  n++;
		  rowh = XmCreateForm (row, "Dialogue", args, n);
		  XtManageChild (rowh);
		  if (label != NULL)
		    {
		       n = 0;
		       XtSetArg (args[n], XmNbackground, BgMenu_Color);
		       n++;
		       XtSetArg (args[n], XmNforeground, Black_Color);
		       n++;
		       XtSetArg (args[n], XmNheight, (Dimension) FontHeight (LargeFontDialogue));
		       n++;
		       XtSetArg (args[n], XmNfontList, DefaultFont);
		       n++;
		       title_string = XmStringCreateSimple (label);
		       XtSetArg (args[n], XmNlabelString, title_string);
		       n++;
		       XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);
		       n++;
		       XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);
		       n++;
		       XtSetArg (args[n], XmNwidth, (Dimension) 60);
		       n++;
		       XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING);
		       n++;
		       w = XmCreateLabel (rowh, "Dialogue", args, n);
		       XtManageChild (w);
		       XmStringFree (title_string);
		    }

		  n = 0;
		  XtSetArg (args[n], XmNbackground, BgMenu_Color);
		  n++;
		  XtSetArg (args[n], XmNforeground, Black_Color);
		  n++;
		  XtSetArg (args[n], XmNeditMode, XmSINGLE_LINE_EDIT);
		  n++;
		  XtSetArg (args[n], XmNtraversalOn, TRUE);
		  n++;
		  XtSetArg (args[n], XmNkeyboardFocusPolicy, XmEXPLICIT);
		  n++;
		  XtSetArg (args[n], XmNsensitive, TRUE);
		  n++;
		  XtSetArg (args[n], XmNeditable, editable);
		  n++;
		  XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);
		  n++;
		  XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);
		  n++;
		  XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);
		  n++;
		  if (label != NULL)
		    {
		       XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET);
		       n++;
		       XtSetArg (args[n], XmNleftWidget, w);
		       n++;
		    }
		  else
		    {
		       XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);
		       n++;
		    }

		  w = XmCreateText (rowh, "Dialogue", args, n);
		  XtManageChild (w);
		  FrameTable[frame].Text_Zone[i] = w;
		  if (procedure != NULL)
		    {
		       XtAddCallback (w, XmNactivateCallback, (XtCallbackProc) APP_TextCallback, (XtPointer) frame);
		       FrameTable[frame].Call_Text[i] = (Proc) procedure;
		    }
		  XtManageChild (row);
		  XtManageChild (XtParent (XtParent (row)));
		  XtManageChild (XtParent (XtParent (XtParent (row))));
#                 else  /* _WINDOWS */
		          currentFrame = frame;
                  GetClientRect (FrMainRef [frame], &rect);
                  w = CreateWindow ("EDIT", "", WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER | ES_AUTOHSCROLL,
                                    0, 0, 0, 0, FrMainRef[frame], (HMENU) i, hInstance, NULL);

                  FrameTable[frame].Text_Zone[i] = w;
                  FrameTable[frame].Call_Text[i] = (Proc) procedure;

				  if (lpfnTextZoneWndProc == (WNDPROC) 0)
                     lpfnTextZoneWndProc = SetWindowLong (FrameTable[frame].Text_Zone[i], GWL_WNDPROC, (DWORD) textZoneProc);
				  else
				     SetWindowLong (FrameTable[frame].Text_Zone[i], GWL_WNDPROC, (DWORD) textZoneProc);

                  wLabel = CreateWindow ("STATIC", label, WS_CHILD | WS_VISIBLE | SS_LEFT, 
                                         0, 0, 0, 0, FrMainRef[frame], (HMENU) (i + MAX_TEXTZONE), hInstance, NULL);
                  FrameTable[frame].Label[i] = wLabel;
				  /* FrameTable[frame].showLogo = TRUE ; */
                  PostMessage (FrMainRef[frame], WM_SIZE, 0, MAKELPARAM (rect.right, rect.bottom));
#                 endif /* _WINDOWS */
	       }
	     else
		i = 0;
	  }
     }
   /* force la mise a jour de la fenetre */
   TtaHandlePendingEvents ();
   return (i);
}				/*TtaAddTextZone */


/*----------------------------------------------------------------------
   TtaSetTextZone

   Sets the text in text-zone in a document view.
   This function must specify a valid view of a valid document.

   Parameters:
   document: identifier of the document.
   view: identifier of the view.
   index: 

  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaSetTextZone (Document document, View view, int index, char *text)
#else  /* __STDC__ */
void                TtaSetTextZone (document, view, index, text)
Document            document;
View                view;
int                 index;
char               *text;

#endif /* __STDC__ */
{
   int                 frame;
   ThotWidget          w;

   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document == 0 && view == 0 && (index < 1 || index >= MAX_TEXTZONE) && text != NULL)
      TtaError (ERR_invalid_parameter);
   else
     {
	frame = GetWindowNumber (document, view);
	if (frame == 0 || frame > MAX_FRAME)
	   TtaError (ERR_invalid_parameter);
	else if (FrameTable[frame].WdFrame != 0)
	  {
	      /* w = FrameTable[frame].Text_Zone[index]; */
	     /*XtRemoveCallback(w, XmNmodifyVerifyCallback, (XtCallbackProc)APP_TextCallback, (XtPointer)frame); */
#            ifndef _WINDOWS
	     w = FrameTable[frame].Text_Zone[index];
	     if (w != 0)
		XmTextSetString (w, text);
#            else  /* _WINDOWS */
	     w = FrameTable[frame].Text_Zone[index - 1];
	     if (w != 0)
                 SetWindowText (w, text);

#            endif /* _WINDOWS */
	     /*XtAddCallback(w, XmNmodifyVerifyCallback, (XtCallbackProc)APP_TextCallback, (XtPointer)frame); */
	  }
     }
#  ifndef _WINDOWS
   XFlush (TtDisplay);
#  endif /* _WINDOWS */
}


/*----------------------------------------------------------------------
   TtcSwitchCommands

   Shows or hides the commands part in a document view.
   This function must specify a valid view of a valid document.

   Parameters:
   document: identifier of the document.
   view: identifier of the view.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcSwitchCommands (Document document, View view)
#else  /* __STDC__ */
void                TtcSwitchCommands (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   int                 frame;

#  ifndef _WINDOWS
   Dimension           y, dy;
   Arg                 args[MAX_ARGS];
   ThotWidget          row, w;
#  endif

#  ifdef _WINDOWS
   int     index, nbZonesShown = 0;
   boolean itemChecked = FALSE;
   RECT    r;
#  endif /* _WINDOWS */


   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document == 0 && view == 0)
      TtaError (ERR_invalid_parameter);
   else
     {
	frame = GetWindowNumber (document, view);
#   ifdef _WINDOWS
    /* FrameTable[frame].showLogo = !FrameTable[frame].showLogo; */
#   endif /* _WINDOWS */ 
	if (frame == 0 || frame > MAX_FRAME)
	   TtaError (ERR_invalid_parameter);
	else if (FrameTable[frame].WdFrame != 0)
	  {
#         ifndef _WINDOWS
	     row = XtParent (FrameTable[frame].Text_Zone[0]);
	     XtSetArg (args[0], XmNwidth, &dy);
	     if (row != 0)
	       {
		  XtUnmanageChild (XtParent (XtParent (row)));
		  if (XtIsManaged (row))
		    {
		       XtGetValues (row, args, 1);
		       XtUnmanageChild (row);
		       dy = -dy;
		    }
		  else
		    {
		       XtManageChild (row);
		       XtGetValues (row, args, 1);
		    }
		  XFlush (TtDisplay);
		  /* Il faut forcer la reevaluation de la fenetre */
		  w = FrameTable[frame].WdFrame;
		  XtSetArg (args[0], XmNwidth, &y);
		  XtGetValues (row, args, 1);
		  XtSetArg (args[0], XmNwidth, y + dy);
		  XtSetValues (row, args, 1);
		  FrameResized ((int *) w, frame, NULL);
		  XtManageChild (XtParent (XtParent (row)));
	       }
#         else  /* _WINDOWS */
	     for (index = 0; index <  MAX_TEXTZONE; index++) {
		 if (FrameTable[frame].Text_Zone[index] && IsWindowVisible (FrameTable[frame].Text_Zone[index])) {
            if (!itemChecked) {
               hmenu = GetMenu (FrMainRef[frame]); 
               CheckMenuItem (hmenu, menu_item, MF_BYCOMMAND | MF_UNCHECKED); 
               itemChecked = TRUE ;
            }

		    ShowWindow (FrameTable[frame].Label [index], SW_HIDE);
		    ShowWindow (FrameTable[frame].Text_Zone [index], SW_HIDE);
		 } else {
              if (!itemChecked) {
                 hmenu = GetMenu (FrMainRef[frame]); 
                 CheckMenuItem (hmenu, menu_item, MF_BYCOMMAND | MF_CHECKED); 
              }

		      ShowWindow (FrameTable[frame].Label [index], SW_SHOW);
		      ShowWindow (FrameTable[frame].Text_Zone [index], SW_SHOW);
		 }
	     }

             GetClientRect (FrMainRef [frame], &r);
             PostMessage (FrMainRef [frame], WM_SIZE, 0, MAKELPARAM (r.right, r.bottom));
#         endif /* _WINDOWS */
	  }
     }
   /* force la mise a jour de la fenetre */
   TtaHandlePendingEvents ();
}				/*TtcSwitchCommands */


/*----------------------------------------------------------------------
   Evenement sur une frame document.                              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DrawingInput (int *w, int frame, int *infos)
#else  /* __STDC__ */
void                DrawingInput (w, frame, infos)
int                *w;
int                 frame;
int                *infos;

#endif /* __STDC__ */
{
}

#ifndef _WIN_PRINT
/*----------------------------------------------------------------------
   Cree une frame a' la position X,Y et aux dimensions large et       
   haut (s'ils sont positifs).                                        
   Le parametre texte donne le titre de la fenetree^tre.                      
   Le parametre schema donne le nom du sche'ma pour lequel on cre'e   
   la fenetree^tre de document (NULL pour la fenetree^tre application).       
   Le parametre name donne le titre de la fenetree^tre.                       
   Le paramentre doc donne le numero du document.                     
   Retourne :                                                         
   - Le volume affichable dans la fenetre en equivalent caracteres.   
   - L'indice de la fenetre allouee ou 0 en cas d'echec.              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 MakeFrame (char *schema, int view, char *name, int X, int Y, int large, int haut, int *volume, int doc)
#else  /* __STDC__ */
int                 MakeFrame (schema, view, name, X, Y, large, haut, volume, doc)
char               *schema;
int                 view;
char               *name;
int                 X;
int                 Y;
int                 large;
int                 haut;
int                *volume;
int                 doc;
#endif /* __STDC__ */
{
#  ifdef _WINDOWS
   HMENU               menu_bar, w;
#  else  /* _WINDOWS */
   ThotWidget          menu_bar;
   ThotWidget          w, row1, row2, rowv;
   ThotWidget          TheFrame;
   ThotWidget          Wframe;
   ThotWidget          shell;
   Arg                 args[MAX_ARGS], argument[5];
   XmString            title_string;
   Dimension           dx, dy;
   char                string[700];
#  endif /* _WINDOWS */
   ThotWidget          Main_Wd = (ThotWidget) 0;
   ThotWidget          hscrl;
   ThotWidget          vscrl;
   SchemaMenu_Ctl     *SCHmenu;
   Menu_Ctl           *ptrmenu;
   char		      *visiStr, *zoomStr;
   int                 i, n;
   int                 ref;
   int		       visiVal, zoomVal;
   int                 frame;
   boolean             found;

#define MIN_HAUT 100
#define MIN_LARG 200
   w = 0;
#  ifdef _WINDOWS
   hwndClient = 0;
   ToolBar    = 0;
   logoFrame  = 0;
   StatusBar  = 0;
#  endif /* _WINDOWS */

   frame = 0;
   if (schema != NULL)
     {
	/* Allocation d'une entree dans la table des fenetres */
	found = FALSE;
	frame = 1;
	while (frame <= MAX_FRAME && !found)
	  {
	     /* Recherche une frame ouverte vide */
	     found = (FrameTable[frame].FrDoc == 0 && FrameTable[frame].WdFrame != 0);
	     if (!found)
		frame++;
	  }
	if (!found)
	  {
	     frame = 1;
	     while (frame <= MAX_FRAME && !found)
	       {
		  /* Recherche une frame libre */
		  found = (FrameTable[frame].WdFrame == 0);
		  if (!found)
		     frame++;
	       }
	  }

	if (!found)
	   frame = 0;
	else if (FrameTable[frame].WdFrame == 0)
	  {
	     /* il faut creer effectivement la fenetre */
	     FrameTable[frame].FrLeftMargin = 0;
	     FrameTable[frame].FrTopMargin = 0;
	     /* Verification des dimensions */
	     if (large == 0)
		large = 180;	/* largeur en mm */
	     large = mmtopixel (large, 1) + FrameTable[frame].FrLeftMargin;
	     if (haut == 0)
		haut = 240;	/* hauteur en mm */
	     haut = mmtopixel (haut, 0) + FrameTable[frame].FrTopMargin;

#            ifndef _WINDOWS
	     if (large < MIN_LARG)
		dx = (Dimension) MIN_LARG;
	     else
		dx = (Dimension) large;
	     if (haut < MIN_HAUT)
		dy = (Dimension) MIN_HAUT;
	     else
		dy = (Dimension) haut;
#            endif /* _WINDOWS */

	     if (X <= 0)
		X = 92;
	     else
		X = mmtopixel (X, 1);
	     if (Y <= 0)
		Y = 2;
	     else
		Y = mmtopixel (Y, 0);

#            ifdef _WINDOWS
#        if 0
		 if (appLogo == (HBITMAP)0)
            appLogo = (HBITMAP) LoadImage (hInstance, bmpID, IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
#        endif /* 0 */

	     Main_Wd = CreateWindowEx (0L, tszAppName,	    /* window class name       */
				       tszAppName,	    /* window caption          */
				       WS_OVERLAPPEDWINDOW, /* window style            */
				       X,	    /* initial x pos           */
				       Y,	    /* initial y pos           */
				       large,	            /* initial x size          */
				       haut,	            /* initial y size          */
				       NULL,		    /* parent window handle    */
				       NULL,		    /* window menu handle      */
				       hInstance,	    /* program instance handle */
				       NULL);	            /* creation parameters     */

	     if (Main_Wd == 0)
		WinErrorBox (WIN_Main_Wd);
	     else {
#                 ifdef AMAYA_DEBUG
                  fprintf (stderr, "Created Main_Wd %X for %d\n", Main_Wd, frame);
#                 endif /* AMAYA_DEBUG */
                  /* store everything. */
                  FrMainRef[frame]            = Main_Wd;
                  FrRef[frame]                = hwndClient;
                  WinToolBar[frame]           = ToolBar;
                  FrameTable[frame].WdStatus  = StatusBar;
                  /* and show it up. */
                  
                  menu_bar = CreateMenu ();
                  if (!menu_bar) 
                     WinErrorBox (Main_Wd);
                  else 
		       WinMenus[frame] = menu_bar;
	     }
#            endif /* _WINDOWS */

	     /*** Creation la fenetre document ***/
	     n = 0;
#            ifndef _WINDOWS
	     XtSetArg (args[n], XmNdefaultFontList, DefaultFont);
	     n++;
	     sprintf (string, "+%d+%d", X, Y);
	     XtSetArg (args[n], XmNgeometry, (String) string);
	     n++;
	     XtSetArg (args[n], XmNwidth, dx + 4);
	     n++;
	     XtSetArg (args[n], XmNheight, dy + 4);
	     n++;
	     if (wind_pixmap != 0)
	       {
		  /* Creation de la fenetre icone associee */
		  XtSetArg (args[n], XmNiconPixmap, wind_pixmap);
		  n++;
	       }
	     XtSetArg (args[n], XmNmwmDecorations, MWM_DECOR_ALL);
	     n++;
	     XtSetArg (args[n], XmNkeyboardFocusPolicy, XmPOINTER);
	     n++;
	     shell = XtCreatePopupShell (name, applicationShellWidgetClass, RootShell, args, n);

	     n = 0;
	     XtSetArg (args[n], XmNwidth, dx + 4);
	     n++;
	     XtSetArg (args[n], XmNheight, dy + 4);
	     n++;
	     XtSetArg (args[n], XmNbackground, Scroll_Color);
	     n++;
	     XtSetArg (args[n], XmNspacing, 0);
	     n++;
	     XtSetArg (args[n], XmNkeyboardFocusPolicy, XmPOINTER);
	     n++;
	     Main_Wd = XmCreateMainWindow (shell, "Thot_Doc", args, n);

	     XtManageChild (Main_Wd);
	     XtAddCallback (shell, XmNdestroyCallback, (XtCallbackProc) FrameKilled, (XtPointer) frame);
#            endif /* _WINDOWS */

	     /* Recherche la liste des menus a construire */
	     SCHmenu = SchemasMenuList;
	     ptrmenu = NULL;
	     while (SCHmenu != NULL && ptrmenu == NULL)
	       {
		  if (!strcmp (schema, SCHmenu->SchemaName))
		     /* c'est un document d'un type particulier */
		     ptrmenu = SCHmenu->SchemaMenu;
		  else
		     /* schema suivant */
		     SCHmenu = SCHmenu->NextSchema;
	       }
	     if (ptrmenu == NULL)
		/* c'est un document standard */
		ptrmenu = DocumentMenuList;

	     /**** Construction des menus ****/
	     FrameTable[frame].FrMenus = ptrmenu;
	     /* reference du menu construit */
	     ref = frame + MAX_LocalMenu;
	     i = 0;
	     /* Initialise les menus dynamiques */
	     FrameTable[frame].MenuAttr = -1;
	     FrameTable[frame].MenuSelect = -1;
#            ifndef _WINDOWS
	     menu_bar = 0;
#            endif /* !_WINDOWS */ 
	     /*** Parametres de creation des boutons menus ***/
	     n = 0;
#            ifndef _WINDOWS
	     XtSetArg (args[n], XmNbackground, BgMenu_Color);
	     n++;
	     XtSetArg (args[n], XmNforeground, Black_Color);
	     n++;
	     XtSetArg (args[n], XmNfontList, DefaultFont);
	     n++;
	     XtSetArg (args[n], XmNspacing, 0);
	     n++;
	     XtSetArg (args[n], XmNborderWidth, 0);
	     n++;
#            endif /* _WINDOWS */
	     /* saute les menus qui ne concernent pas cette vue */
	     while (ptrmenu != NULL)
	       {
		  /* saute les menus qui ne concernent pas cette vue */
		  if (ptrmenu->MenuView == 0 || ptrmenu->MenuView == view)
		    {
		       if (menu_bar == 0)
			 {
			     /*** La barre des menus ***/
#                           ifndef _WINDOWS
			    XtSetArg (argument[0], XmNbackground, BgMenu_Color);
			    XtSetArg (argument[1], XmNspacing, 0);
			    menu_bar = XmCreateMenuBar (Main_Wd, "Barre_menu", argument, 2);
			    XtManageChild (menu_bar);
#                           endif /* !_WINDOWS */
			 }

		       /* construit le bouton de menu */
#                      ifdef _WINDOWS
		       w = CreateMenu ();
#                      else  /* _WINDOWS */
		       w = XmCreateCascadeButton (menu_bar, TtaGetMessage (THOT, ptrmenu->MenuID), args, n);
#                      endif /* !_WINDOWS */
		       FrameTable[frame].WdMenus[i] = w;
		       FrameTable[frame].ActifMenus[i] = TRUE;
		       /* Evite la construction des menus dynamiques */
		       if (ptrmenu->MenuAttr)
			  FrameTable[frame].MenuAttr = ptrmenu->MenuID;
		       else if (ptrmenu->MenuSelect) 
			    FrameTable[frame].MenuSelect = ptrmenu->MenuID;
		       else 
			    BuildPopdown (ptrmenu, ref, w, frame);
#                      ifdef _WINDOWS
                       AppendMenu (menu_bar, MF_POPUP, (UINT) w, TtaGetMessage (THOT, ptrmenu->MenuID));
#                      else  /* !_WINDOWS */
                       XtManageChild (w);
#                      endif /* !_WINDOWS */
		       /* Enregistre les menus dynamiques */
		       if (ptrmenu->MenuHelp)
			 {
			    /* Cadre a droite le menu help */
#                           ifdef _WINDOWS
#                           else  /* _WINDOWS */
			    XtSetArg (argument[0], XmNmenuHelpWidget, w);
			    XtSetValues (XtParent (w), argument, 1);
#                           endif /* _WINDOWS */
			 }

		    }

		  ptrmenu = ptrmenu->NextMenu;
		  ref += MAX_ITEM;
		  i++;
	       }

	     /* Les autres entrees de menus sont inactives */
	     while (i < MAX_MENU)
	       {
		  FrameTable[frame].ActifMenus[i] = FALSE;
		  i++;
	       }

	     /*** La barre de scroll horizontale ***/
	     n = 0;
#            ifndef _WINDOWS
	     XtSetArg (args[n], XmNbackground, Scroll_Color);
	     n++;
	     XtSetArg (args[n], XmNorientation, XmHORIZONTAL);
	     n++;
	     XtSetArg (args[n], XmNvalue, 0);
	     n++;
	     hscrl = XmCreateScrollBar (Main_Wd, "Scroll", args, n);
	     XtManageChild (hscrl);
	     /*XtAddCallback (hscrl, XmNvalueChangedCallback, (XtCallbackProc) FrameHScrolled, (XtPointer) frame); */
	     XtAddCallback (hscrl, XmNdragCallback, (XtCallbackProc) FrameHScrolled, (XtPointer) frame);
	     XtAddCallback (hscrl, XmNdecrementCallback, (XtCallbackProc) FrameHScrolled, (XtPointer) frame);
	     XtAddCallback (hscrl, XmNincrementCallback, (XtCallbackProc) FrameHScrolled, (XtPointer) frame);
	     XtAddCallback (hscrl, XmNpageDecrementCallback, (XtCallbackProc) FrameHScrolled, (XtPointer) frame);
	     XtAddCallback (hscrl, XmNpageIncrementCallback, (XtCallbackProc) FrameHScrolled, (XtPointer) frame);
	     XtAddCallback (hscrl, XmNtoTopCallback, (XtCallbackProc) FrameHScrolled, (XtPointer) frame);
	     XtAddCallback (hscrl, XmNtoBottomCallback, (XtCallbackProc) FrameHScrolled, (XtPointer) frame);
#            else  /* _WINDOWS */
             hscrl = CreateWindow ("scrollbar", NULL, WS_CHILD | WS_VISIBLE | SBS_HORZ,
                                   0, 0, 0, 0, Main_Wd, (HMENU) frame, hInstance, NULL);

             SetScrollRange (hscrl, SB_CTL, 0, 100, FALSE);
             SetScrollPos (hscrl, SB_CTL, 0, FALSE);
#            endif /* _WINDOWS */
	     /*** La barre de scroll verticale ***/
#            ifndef _WINDOWS

	     n = 0;
	     XtSetArg (args[n], XmNbackground, Scroll_Color);
	     n++;
	     XtSetArg (args[n], XmNorientation, XmVERTICAL);
	     n++;
	     XtSetArg (args[n], XmNvalue, 0);
	     n++;
	     vscrl = XmCreateScrollBar (Main_Wd, "Scroll", args, n);
	     XtManageChild (vscrl);
	     /*XtAddCallback (vscrl, XmNvalueChangedCallback, (XtCallbackProc) FrameVScrolled, (XtPointer) frame); */
	     XtAddCallback (vscrl, XmNdragCallback, (XtCallbackProc) FrameVScrolled, (XtPointer) frame);
	     XtAddCallback (vscrl, XmNdecrementCallback, (XtCallbackProc) FrameVScrolled, (XtPointer) frame);
	     XtAddCallback (vscrl, XmNincrementCallback, (XtCallbackProc) FrameVScrolled, (XtPointer) frame);
	     XtAddCallback (vscrl, XmNpageDecrementCallback, (XtCallbackProc) FrameVScrolled, (XtPointer) frame);
	     XtAddCallback (vscrl, XmNpageIncrementCallback, (XtCallbackProc) FrameVScrolled, (XtPointer) frame);
	     XtAddCallback (vscrl, XmNtoTopCallback, (XtCallbackProc) FrameVScrolled, (XtPointer) frame);
	     XtAddCallback (vscrl, XmNtoBottomCallback, (XtCallbackProc) FrameVScrolled, (XtPointer) frame);
#            else  /* _WINDOWS */
             vscrl = CreateWindow ("scrollbar", NULL, WS_CHILD | WS_VISIBLE | SBS_VERT,
                                   0, 0, 0, 0, Main_Wd, (HMENU) (frame + 1), hInstance, NULL);

             SetScrollRange (vscrl, SB_CTL, 0, 100, FALSE);
             SetScrollPos (vscrl, SB_CTL, 0, FALSE);
#            endif /* _WINDOWS */

	     /* Row vertical pour mettre le logo au dessous des boutons */
#            ifndef _WINDOWS
	     n = 0;
	     XtSetArg (args[n], XmNmarginWidth, 0);
	     n++;
	     XtSetArg (args[n], XmNmarginHeight, 0);
	     n++;
	     XtSetArg (args[n], XmNbackground, BgMenu_Color);
	     n++;
	     XtSetArg (args[n], XmNpacking, XmPACK_TIGHT);
	     n++;
	     XtSetArg (args[n], XmNorientation, XmVERTICAL);
	     n++;
	     XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);
	     n++;
	     rowv = XmCreateRowColumn (Main_Wd, "", args, n);

	     XtManageChild (rowv);

	     Wframe = rowv;
	     /* Row horizontal des boutons */
	     n = 0;
	     XtSetArg (args[n], XmNmarginWidth, 0);
	     n++;
	     XtSetArg (args[n], XmNmarginHeight, 0);
	     n++;
	     XtSetArg (args[n], XmNbackground, BgMenu_Color);
	     n++;
	     XtSetArg (args[n], XmNorientation, XmHORIZONTAL);
	     n++;
	     XtSetArg (args[n], XmNspacing, 0);
	     n++;
	     XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);
	     n++;
	     row1 = XmCreateRowColumn (rowv, "", args, n);

	     for (i = 1; i < MAX_BUTTON; i++)
	       FrameTable[frame].Button[i] = 0;
	     FrameTable[frame].Button[0] = row1;

	     /* Row horizontal pour mettre le logo a gauche des commandes */
	     n = 0;
	     XtSetArg (args[n], XmNmarginWidth, 5);
	     n++;
	     XtSetArg (args[n], XmNmarginHeight, 5);
	     n++;
	     XtSetArg (args[n], XmNbackground, BgMenu_Color);
	     n++;
	     XtSetArg (args[n], XmNwidth, dx);
	     n++;
	     XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);
	     n++;
	     row1 = XmCreateForm (rowv, "", args, n);

	     XtManageChild (row1);

	     /* logo de l'application */
	     if (image != 0)
	       {
		  n = 0;
		  XtSetArg (args[n], XmNbackground, BgMenu_Color);
		  n++;
		  XtSetArg (args[n], XmNlabelType, XmPIXMAP);
		  n++;
		  XtSetArg (args[n], XmNlabelPixmap, image);
		  n++;
		  XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);
		  n++;
		  XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);
		  n++;
		  w = XmCreateLabel (row1, "Logo", args, n);

		  XtManageChild (w);
	       }

	     /*** Creation des zones texte  ***/
	     n = 0;
	     XtSetArg (args[n], XmNmarginWidth, 0);
	     n++;
	     XtSetArg (args[n], XmNmarginHeight, 0);
	     n++;
	     XtSetArg (args[n], XmNbackground, BgMenu_Color);
	     n++;
	     XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);
	     n++;
	     XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);
	     n++;
	     XtSetArg (args[n], XmNkeyboardFocusPolicy, XmPOINTER);
	     n++;

	     if (image != 0)
	       {
		  XtSetArg (args[n], XmNleftOffset, 5);
		  n++;
		  XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET);
		  n++;
		  XtSetArg (args[n], XmNleftWidget, w);
		  n++;
	       }
	     else
	       {
		  XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);
		  n++;
	       }

	     rowv = XmCreateForm (row1, "", args, n);
	     for (i = 1; i < MAX_TEXTZONE; i++)
		FrameTable[frame].Text_Zone[i] = 0;
	     FrameTable[frame].Text_Zone[0] = rowv;

	     /*** Creation de la zone d'affichage du contenu du document ***/
	     n = 0;
	     XtSetArg (args[n], XmNmarginWidth, 0);
	     n++;
	     XtSetArg (args[n], XmNmarginHeight, 0);
	     n++;
	     XtSetArg (args[n], XmNbackground, Scroll_Color);
	     n++;
	     XtSetArg (args[n], XmNkeyboardFocusPolicy, XmPOINTER);
	     n++;
	     XtSetArg (args[n], XmNtraversalOn, TRUE);
	     n++;

	     TheFrame = w = XmCreateFrame (Main_Wd, "Frame", args, n);
	     XtManageChild (w);

	     n = 0;
	     XtSetArg (args[n], XmNbackground, White_Color);
	     n++;
	     XtSetArg (args[n], XmNfontList, DefaultFont);
	     n++;
	     XtSetArg (args[n], XmNmarginWidth, 0);
	     n++;
	     XtSetArg (args[n], XmNmarginHeight, 0);
	     n++;
	     XtSetArg (args[n], XmNkeyboardFocusPolicy, XmPOINTER);
	     n++;
	     w = XmCreateDrawingArea (w, "", args, n);
	     XtManageChild (w);

	     /* Row horizontal pour les messages */
	     n = 0;
	     XtSetArg (args[n], XmNmarginWidth, 0);
	     n++;
	     XtSetArg (args[n], XmNmarginHeight, 0);
	     n++;
	     XtSetArg (args[n], XmNbackground, BgMenu_Color);
	     n++;
	     XtSetArg (args[n], XmNorientation, XmHORIZONTAL);
	     n++;
	     XtSetArg (args[n], XmNkeyboardFocusPolicy, XmPOINTER);
	     n++;
	     row2 = XmCreateRowColumn (Main_Wd, "", args, n);
	     XtManageChild (row2);
	     n = 0;
	     XtSetArg (args[n], XmNbackground, BgMenu_Color);
	     n++;
	     XtSetArg (args[n], XmNforeground, Black_Color);
	     n++;
	     XtSetArg (args[n], XmNheight, (Dimension) FontHeight (LargeFontDialogue));
	     n++;
	     XtSetArg (args[n], XmNfontList, DefaultFont);
	     n++;
	     title_string = XmStringCreateSimple (" ");
	     XtSetArg (args[n], XmNlabelString, title_string);
	     n++;
	     i = CharacterWidth ('M', LargeFontDialogue) * 50;
	     XtSetArg (args[n], XmNwidth, (Dimension) i);
	     n++;
	     FrameTable[frame].WdStatus = XmCreateLabel (row2, "Thot_MSG", args, n);
	     XtManageChild (FrameTable[frame].WdStatus);
	     XmStringFree (title_string);
	     n = 0;
	     XtSetArg (args[n], XmNmessageWindow, row2);
	     n++;
	     XtSetValues (Main_Wd, args, n);

	     n = 0;
	     XtSetArg (args[n], XmNx, (Position) X + 4);
	     n++;
	     XtSetArg (args[n], XmNy, (Position) Y + 4);
	     n++;
	     XtSetValues (shell, args, n);
	     XtPopup (shell, XtGrabNonexclusive);

	     XmMainWindowSetAreas (Main_Wd, menu_bar, Wframe, hscrl, vscrl, TheFrame);
	     XtAddCallback (w, XmNinputCallback, (XtCallbackProc) DrawingInput, (XtPointer) frame);
	     XtAddCallback (w, XmNresizeCallback, (XtCallbackProc) FrameResized, (XtPointer) frame);
	     FrRef[frame] = XtWindowOfObject (w);
#            endif /* !_WINDOWS */
	     FrameTable[frame].WdScrollH = hscrl;
	     FrameTable[frame].WdScrollV = vscrl;

#            ifndef _WINDOWS
/*****	     n = 0;
	     XtSetArg (args[n], XmNwidth, &dx);
	     n++;
	     XtSetArg (args[n], XmNheight, &dy);
	     n++;
	     XtGetValues ((Widget) w, args, n);
*****/	     FrameTable[frame].FrWidth  = (int) dx;
	     FrameTable[frame].FrHeight = (int) dy;
#            else /* _WINDOWS */
	     FrameTable[frame].FrWidth  = (int) large;
	     FrameTable[frame].FrHeight = (int) haut;
#            endif /* _WINDOWS */
	     FrameTable[frame].WdFrame = w;
	  }
	else
	   ChangeFrameTitle (frame, name);
	/* volume en caracteres */
	*volume = GetCharsCapacity (FrameTable[frame].FrWidth * FrameTable[frame].FrHeight);
	FrameTable[frame].FrDoc = doc;
	FrameTable[frame].FrView = view;

	/* get registry default values for zoom and visibility */
	zoomStr = TtaGetEnvString ("ZOOM");
	if (zoomStr == NULL)
	  zoomVal = 0;
	else
	  {
	    zoomVal = atoi (zoomStr);
	    if (zoomVal > 10 || zoomVal < -10)
	      zoomVal = 0;
	  }
	visiStr = TtaGetEnvString ("VISIBILITY");
	if (visiStr == NULL)
	  visiVal = 5;
	else
	  {
	    visiVal = atoi (visiStr);
	    if (visiVal < 0 || visiVal > 10)
	      visiVal = 5;
	  }
	/* Initialise la visibilite et le zoom de la fenetre */
	InitializeFrameParams (frame, visiVal, zoomVal);	
#       ifdef _WINDOWS
        SetMenu (Main_Wd, menu_bar);
	ShowWindow (Main_Wd, SW_SHOWNORMAL);
	UpdateWindow (Main_Wd);
        InitCommonControls ();
#       endif /* _WINDOWS */
     }

   return (frame);
}
#endif /* !_WIN_PRINT */

/*----------------------------------------------------------------------
   Si l'entree existe :                                             
   Ferme la fenetre, detruit le fichier et libere l'entree.      
   Libere toutes les boites allouees a la fenetre.                   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DestroyFrame (int frame)
#else  /* __STDC__ */
void                DestroyFrame (frame)
int                 frame;

#endif /* __STDC__ */
{
   ThotWidget          w;
   int                 action;
   int                 ref, i;
   int                 item;
   Menu_Ctl           *ptrmenu;
   Item_Ctl           *ptr;
#  ifdef _WINDOWS
   int                 txtZoneIndex;
#  endif /* _WINDOWS */

   if (ThotLocalActions[T_stopinsert] != NULL)
     (*ThotLocalActions[T_stopinsert]) ();
   w = FrameTable[frame].WdFrame;
   if (w != 0)
     {
	/* Destruction des menus attaches a la fenetre */
	ptrmenu = FrameTable[frame].FrMenus;
	i = 0;
	ref = frame + MAX_LocalMenu;	/* reference du menu construit */
	while (ptrmenu != NULL)
	  {
	     /* saute les menus qui ne concernent pas cette vue */
	     if (ptrmenu->MenuView == 0 || ptrmenu->MenuView == FrameTable[frame].FrView)
	       {
		  FrameTable[frame].WdMenus[i] = 0;
		  TtaDestroyDialogue (ref);
		  item = 0;
		  ptr = ptrmenu->ItemsList;
		  while (item < ptrmenu->ItemsNb)
		    {
		       action = ptr[item].ItemAction;
		       if (action != -1
			   && (ptr[item].ItemType == 'B' || ptr[item].ItemType == 'T'))
			  /* Desactive l'action correspondante pour cette fenetre */
			  MenuActionList[action].ActionActive[frame] = FALSE;
		       item++;
		    }
	       }
	     ptrmenu = ptrmenu->NextMenu;
	     ref += MAX_ITEM;
	     i++;
	  }


	/* Annule eventuellement les formulaires attaches a la fenetre */
	if (ThotLocalActions[T_rszoom] != NULL)
	   (*ThotLocalActions[T_rszoom]) (frame);
	if (ThotLocalActions[T_rsvisibility] != NULL)
	   (*ThotLocalActions[T_rsvisibility]) (frame);
	if (ThotLocalActions[T_rscorrector] != NULL)
	   (*ThotLocalActions[T_rscorrector]) (frame);
	if (ThotLocalActions[T_rsindex] != NULL)
	   (*ThotLocalActions[T_rsindex]) (frame);

#       ifndef _WINDOWS
        XFlushOutput (0);
        /* Detache les procedures de callback */
        XtRemoveCallback (XtParent (XtParent (w)), XmNdestroyCallback, (XtCallbackProc) FrameKilled, (XtPointer) frame);

        XDestroyWindow (TtDisplay, XtWindowOfObject (XtParent (XtParent (XtParent (w)))));

        for (i = 0; i < MAX_BUTTON; i++)
            FrameTable[frame].Button[i] = 0;

#       else  /* _WINDOWS */
        for (txtZoneIndex = 0; txtZoneIndex < MAX_TEXTZONE; txtZoneIndex++) {
            FrameTable[frame].Text_Zone[txtZoneIndex] = 0;
        }
        if (hAccel [frame]) {
           DestroyAcceleratorTable (hAccel [frame]);
		   hAccel [frame] = NULL;
        }
        DestroyWindow (FrMainRef[frame]);
		CleanFrameCatList (frame);

        for (i = 0; i < MAX_BUTTON; i++) {
            TtaFreeMemory (FrameTable[frame].Button[i]);
            FrameTable[frame].Button[i] = 0;
		}

        FrMainRef [0] = 0;
#       endif /* _WINDOWS */
	FrRef[frame] = 0;
	FrameTable[frame].WdFrame = 0;
	FrameTable[frame].FrDoc = 0;
	/* Elimine les evenements ButtonRelease, DestroyNotify, FocusOut */

	ClearConcreteImage (frame);
	ThotFreeFont (frame);	/* On libere les polices de caracteres utilisees */
     }				/*if */
}


/*----------------------------------------------------------------------
   GetMenu_Ctl donne le contexte du menu associe' a` la fenetree^tre. 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static Menu_Ctl    *GetMenu_Ctl (int frame, int menu)
#else  /* __STDC__ */
static Menu_Ctl    *GetMenu_Ctl (frame, menu)
int                 frame;
int                 menu;

#endif /* __STDC__ */
{
   int                 i;
   Menu_Ctl           *ptrmenu;

   ptrmenu = FrameTable[frame].FrMenus;
   i = 0;
   while (i != menu && ptrmenu != NULL)
     {
	ptrmenu = ptrmenu->NextMenu;
	i++;
     }
   return (ptrmenu);
}


/*----------------------------------------------------------------------
   FindMenu recherche le menu menuID dans la fenetree^tre.            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 FindMenu (int frame, int menuID, Menu_Ctl ** ctxmenu)
#else  /* __STDC__ */
int                 FindMenu (frame, menuID, ctxmenu)
int                 frame;
int                 menuID;
Menu_Ctl          **ctxmenu;

#endif /* __STDC__ */
{
   Menu_Ctl           *ptrmenu;
   int                 m;

   /* Recherche le bon menu */
   m = 1;			/* index de menu */
   /* recherche cet item dans la liste des menus de la fenetre */
   ptrmenu = FrameTable[frame].FrMenus;
   while (ptrmenu != NULL && menuID != ptrmenu->MenuID)
     {
	m++;
	ptrmenu = ptrmenu->NextMenu;
     }

   *ctxmenu = ptrmenu;
   if (ptrmenu == NULL)
      return (-1);
   else if (ptrmenu->MenuView != 0 && ptrmenu->MenuView != FrameTable[frame].FrView)
      return (-1);
   else
      return (m);
}


/*----------------------------------------------------------------------
   FindItemMenu recherche le menu, et l'item de la fenetree^tre.              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         FindItemMenu (int frame, int menuID, int itemID, int *menu, int *submenu, int *item, int *action)
#else  /* __STDC__ */
static void         FindItemMenu (frame, menuID, itemID, menu, submenu, item, action)
int                 frame;
int                 menuID;
int                 itemID;
int                *menu;
int                *submenu;
int                *item;
int                *action;
#endif /* __STDC__ */
{
   Menu_Ctl           *ptrmenu, *ptrsmenu;
   Item_Ctl           *ptr;
   int                 i, j, max;
   int                 m, sm;
   boolean             found;

   j = 0;
   i = 0;
   sm = 0;
   m = FindMenu (frame, menuID, &ptrmenu);
   found = (m != -1);
   if (found)
     {
	/* Recherche l'item dans le menu ou un sous-menu */
	ptr = ptrmenu->ItemsList;
	found = FALSE;
	max = ptrmenu->ItemsNb;
	ptrsmenu = NULL;
	while (ptrmenu != NULL && !found)
	  {
	     while (i < max && !found)
	       {
		  j = ptr[i].ItemAction;
		  if (j == -1)
		     i++;	/* separateur */
		  else if (ptr[i].ItemType == 'M')
		    {
		       /* recherche dans le sous-menu */
		       sm = i + 1;
		       ptrsmenu = ptr[i].SubMenu;
		       i = 0;
		       ptr = ptrsmenu->ItemsList;
		       max = ptrsmenu->ItemsNb;
		    }
		  else if (ptr[i].ItemID != itemID)
		     i++;	/* ce n'est pas l'action */
		  else
		     found = TRUE;
	       }

	     /* faut-il sortir du sous-menu ? */
	     if (!found && ptrsmenu != NULL)
	       {
		  /* reprend la recherche dans le menu */
		  i = sm;
		  sm = 0;
		  ptrsmenu = NULL;
		  ptr = ptrmenu->ItemsList;
		  max = ptrmenu->ItemsNb;
	       }
	     else
		/* on a parcouru tout le menu : cet itemID n'existe pas */
		ptrmenu = NULL;
	  }
     }

   /* est-ce que l'on a trouve ? */
   if (found)
     {
	*menu = m;
	*submenu = sm;
	*item = i;
	*action = j;
     }
   else
     {
	*menu = -1;
	*submenu = 0;
	*item = 0;
	*action = -1;
     }
}

/*----------------------------------------------------------------------
   TtaSetMenuOff desactive le menu (1 a n) de la vue du document ou   
   de la fenetre principale (document = 0, view = 0).                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaSetMenuOff (Document document, View view, int menuID)
#else  /* __STDC__ */
void                TtaSetMenuOff (document, view, menuID)
Document            document;
View                view;
int                 menuID;

#endif /* __STDC__ */
{
   ThotWidget          w;
   int                 menu;
   int                 frame;
   int                 ref;
   Menu_Ctl           *ptrmenu;
#  ifndef _WINDOWS
   int                 n; 
#  endif /* !_WINDOWS */

#ifndef _WINDOWS
   XmFontList          font;
   Arg                 args[MAX_ARGS];

#endif

   if (document == 0 && view == 0)
      frame = 0;
   else
      frame = GetWindowNumber (document, view);

   /* Si les parametres sont invalides */
   if (frame > MAX_FRAME)
      return;
   else if ((FrameTable[frame].WdFrame) == 0)
      return;

   menu = FindMenu (frame, menuID, &ptrmenu);
   if (menu != -1)
     {
	menu--;
	if (FrameTable[frame].ActifMenus[menu])
	  {
	     /* Recupere le widget du bouton */
	     w = FrameTable[frame].WdMenus[menu];
	     if (w != 0)
	       {
		  FrameTable[frame].ActifMenus[menu] = FALSE;
		  ref = (menu * MAX_ITEM) + frame + MAX_LocalMenu;
		  /* Desactive */
#         ifdef _WINDOWS
		  WIN_TtaSetPulldownOff (ref, w, TtaGetViewFrame (document, view));
#         else  /* !_WINDOWS */
		  TtaSetPulldownOff (ref, w);
#         endif /* _WINDOWS */

#ifndef _WINDOWS
		  /* Visualise le bouton inactif */
		  if (TtWDepth > 1)
		    {
		       /* Changement de couleur */
		       n = 0;
		       XtSetArg (args[n], XmNforeground, InactiveB_Color);
		       n++;
		       XtSetValues (w, args, n);
		       XtManageChild (w);
		    }
		  else
		    {
		       /* Changement de police de caracteres */
		       font = XmFontListCreate ((XFontStruct *) IFontDialogue, XmSTRING_DEFAULT_CHARSET);
		       n = 0;
		       XtSetArg (args[n], XmNfontList, font);
		       n++;
		       XtSetValues (w, args, n);
		       XtManageChild (w);
		       XmFontListFree (font);
		    }
#endif /* _WINDOWS */
	       }
	  }
     }
}


/*----------------------------------------------------------------------
   TtaSetMenuOn reactive le menu (1 a n) de la vue du document ou     
   de la fenetre principale (document = 0, view = 0).                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaSetMenuOn (Document document, View view, int menuID)
#else  /* __STDC__ */
void                TtaSetMenuOn (document, view, menuID)
Document            document;
View                view;
int                 menuID;

#endif /* __STDC__ */
{
   ThotWidget          w;
   int                 menu;
   int                 frame;
   int                 ref;
   Menu_Ctl           *ptrmenu;

#ifndef _WINDOWS
   Arg                 args[MAX_ARGS];

#endif

   if (document == 0 && view == 0)
      frame = 0;
   else
      frame = GetWindowNumber (document, view);

   /* Si les parametres sont invalides */
   if (frame > MAX_FRAME)
      return;
   else if ((FrameTable[frame].WdFrame) == 0)
      return;

   menu = FindMenu (frame, menuID, &ptrmenu);
   if (menu != -1)
     {
	menu--;
	if (!FrameTable[frame].ActifMenus[menu])
	  {
	     /* Recupere le widget du bouton */
	     w = FrameTable[frame].WdMenus[menu];
	     if (w != 0)
	       {
		  FrameTable[frame].ActifMenus[menu] = TRUE;
		  ref = (menu * MAX_ITEM) + frame + MAX_LocalMenu;
		  /* Desactive */
#         ifdef _WINDOWS
		  WIN_TtaSetPulldownOn (ref, w, TtaGetViewFrame (document, view));
#         else /* !_WINDOWS */
		  TtaSetPulldownOn (ref, w);
#         endif /* _WINDOWS */

#ifndef _WINDOWS
		  /* Visualise le bouton actif */
		  if (TtWDepth > 1)
		    {
		       /* Changement de couleur */
		       XtSetArg (args[0], XmNforeground, Black_Color);
		       XtSetValues (w, args, 1);
		       XtManageChild (w);
		    }
		  else
		    {
		       /* Changement de police de caracteres */
		       XtSetArg (args[0], XmNfontList, DefaultFont);
		       XtSetValues (w, args, 1);
		       XtManageChild (w);
		    }
#endif
	       }
	  }
     }
}


/*----------------------------------------------------------------------
   TtaSetToggleItem positionne l'item du menu de la vue du document   
   ou de la fenetre principale (document = 0, view = 0).   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaSetToggleItem (Document document, View view, int menuID, int itemID, boolean on)
#else  /* __STDC__ */
void                TtaSetToggleItem (document, view, menuID, itemID, on)
Document            document;
View                view;
int                 menuID;
int                 itemID;
boolean             on;
#endif /* __STDC__ */
{
   int                 frame;
   int                 ref;
   int                 menu, submenu;
   int                 item, action;

   if (menuID == 0 || itemID == 0)
      return;
   if (document == 0 && view == 0)
      frame = 0;
   else
      frame = GetWindowNumber (document, view);

   /* Si les parametres sont invalides */
   if (frame > MAX_FRAME)
      return;
   else if ((FrameTable[frame].WdFrame) == 0)
      return;


   /* Recherche les bons indices de menu, sous-menu et item */
   FindItemMenu (frame, menuID, itemID, &menu, &submenu, &item, &action);
   if (menu >= 0 && item >= 0)
     {
	/* on a trouve */
	ref = ((menu - 1) * MAX_ITEM) + frame + MAX_LocalMenu;
	if (submenu != 0)
	   ref += submenu * MAX_MENU * MAX_ITEM;
#   ifdef _WINDOWS
	WIN_TtaSetToggleMenu (ref, item, on, FrMainRef[frame]);
#   else  /* _WINDOWS */
	TtaSetToggleMenu (ref, item, on);
#   endif /* _WINDOWS */
     }
}


/*----------------------------------------------------------------------
   TtaSetItemOff desactive l'item actionName de la vue du document  
   ou de la fenetre principale (document = 0, view = 0).   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaSetItemOff (Document document, View view, int menuID, int itemID)
#else  /* __STDC__ */
void                TtaSetItemOff (document, view, menuID, itemID)
Document            document;
View                view;
int                 menuID;
int                 itemID;

#endif /* __STDC__ */
{
   int                 frame;
   int                 ref;
   int                 menu, submenu;
   int                 item;
   int                 action;
   char                fontname[100];
   char                text[20];
#  ifdef _WINDOWS
   HMENU               hMenu;
#  endif /* _WINDOWS */

   /* Si les parametres sont invalides */
   if (document == 0 && view == 0)
      frame = 0;
   else
      frame = GetWindowNumber (document, view);
   if (frame > MAX_FRAME)
      return;
   else if ((FrameTable[frame].WdFrame) == 0)
      return;

   /* Recherche les bons indices de menu, sous-menu et item */
   FindItemMenu (frame, menuID, itemID, &menu, &submenu, &item, &action);
   if (action > 0)
      /* l'action existe et le menu est actif */
      if (MenuActionList[action].ActionActive[frame])
	{
	   /* desactive l'action pour la fenetre */
	   MenuActionList[action].ActionActive[frame] = FALSE;
	   /* desactive l'entree de menu */
	   ref = ((menu - 1) * MAX_ITEM) + frame + MAX_LocalMenu;
	   if (submenu != 0)
	      ref += submenu * MAX_MENU * MAX_ITEM;
#      ifdef _WINDOWS
       hMenu = GetMenu (TtaGetViewFrame (document, view));
	   EnableMenuItem (hMenu, ref + item, MFS_GRAYED);
#      else  /* !_WINDOWS */
	   if (TtWDepth > 1)
	      TtaRedrawMenuEntry (ref, item, NULL, InactiveB_Color, 0);
	   else
	     {
		FontIdentifier ('L', 'T', 2, 11, UnPoint, text, fontname);
		TtaRedrawMenuEntry (ref, item, fontname, -1, 0);
	     }
#      endif /* _WINDOWS */
	}
}


/*----------------------------------------------------------------------
   TtaSetItemOn active l'item actionName de la vue du document      
   ou de la fenetre principale (document = 0, view = 0).   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaSetItemOn (Document document, View view, int menuID, int itemID)
#else  /* __STDC__ */
void                TtaSetItemOn (document, view, menuID, itemID)
Document            document;
View                view;
int                 menuID;
int                 itemID;

#endif /* __STDC__ */
{
   int                 frame;
   int                 ref;
   int                 menu, submenu;
   int                 item;
   int                 action;

   /* Si les parametres sont invalides */
   if (document == 0 && view == 0)
      frame = 0;
   else
      frame = GetWindowNumber (document, view);
   if (frame > MAX_FRAME)
      return;
   else if ((FrameTable[frame].WdFrame) == 0)
      return;

   /* Recherche les bons indices de menu, sous-menu et item */
   FindItemMenu (frame, menuID, itemID, &menu, &submenu, &item, &action);
   if (action > 0)
      /* l'action existe et le menu est actif */
      if (!MenuActionList[action].ActionActive[frame])
	{
	   /* reactive l'action pour la fenetre */
	   MenuActionList[action].ActionActive[frame] = TRUE;
	   /* reactive l'entree de menu */
	   ref = ((menu - 1) * MAX_ITEM) + frame + MAX_LocalMenu;
	   if (submenu != 0)
	      ref += submenu * MAX_MENU * MAX_ITEM;
	   TtaRedrawMenuEntry (ref, item, NULL, -1, 1);
	}
}


/*----------------------------------------------------------------------
   TtaSetCallback

   Specifies the procedure that is called when the user activates a set of forms
   and/or menus created by the application.
   The parameter set gives the number of forms and/or menus created by the 
   application managed by this porcedure.
   This function must be called before processing any event, only if the
   application uses the Dialogue tool kit for generating specific forms or menus.
   This function replaces the DefineCallback function in the Dialogue tool kit.
   This procedure is called with three parameters: the menu or form reference,
   data type, and data value.

   void callbakProcedure(reference, datatype, data)
   int reference;
   int datatype;
   char *data; 

   Parameter:
   callbakProcedure: the application callback procedure.
   set: the number of forms and/or menus managed.
   Return:
   reference of the first form or menu.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 TtaSetCallback (void (*callbakProcedure) (), int set)
#else  /* __STDC__ */
int                 TtaSetCallback (callbakProcedure)
void                (*callbakProcedure) ();
int                 set;

#endif /* __STDC__ */
{
   PtrCallbackCTX      ctxCallback;

   UserErrorCode = 0;
   if (firstCallbackAPI == NULL)
     {
	/* le premier bloc de callback */
	firstCallbackAPI = (PtrCallbackCTX) TtaGetMemory (sizeof (CallbackCTX));
	ctxCallback = firstCallbackAPI;
     }
   else
     {
	ctxCallback = firstCallbackAPI;
	while (ctxCallback->callbackNext != NULL)
	   ctxCallback = ctxCallback->callbackNext;
	ctxCallback->callbackNext = (PtrCallbackCTX) TtaGetMemory (sizeof (CallbackCTX));
	ctxCallback = ctxCallback->callbackNext;
     }

   ctxCallback->callbackProc = callbakProcedure;
   ctxCallback->callbackSet = set;
   ctxCallback->callbackNext = NULL;
   return (TtaGetReferencesBase (set));
}				/*TtaSetCallback */


/*----------------------------------------------------------------------
   ThotCallback ge`re tous les retours du dialogue de Thot.        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ThotCallback (int ref, int typedata, char *data)

#else  /* __STDC__ */
void                ThotCallback (ref, typedata, data)
int                 ref;
int                 typedata;
char               *data;

#endif /* __STDC__ */

{
   int                 frame, item;
   int                 menu, base;
   int                 menuThot;
   Menu_Ctl           *ptrmenu;
   int                 action, i, j;
   Document            document;
   View                view;
   PtrCallbackCTX      ctxCallback;

   /* Termine l'insertion courante s'il y en a une */
#  ifdef _WINDOWS
   menu_item = ref + (int) data;
#  endif /* _WINDOWS */
   CloseInsertion ();

   if (ref >= MAX_ThotMenu)
     {
	if (firstCallbackAPI == NULL)
	   return;		/* pas de callback definis */
	else
	  {
	     /* recherche le bon callback */
	     ctxCallback = firstCallbackAPI;
	     base = MAX_ThotMenu;
	     while (ref >= base + ctxCallback->callbackSet)
	       {
		  if (ctxCallback->callbackNext == NULL)
		     return;
		  else
		    {
		       base = base + ctxCallback->callbackSet;
		       ctxCallback = ctxCallback->callbackNext;
		    }
	       }
	     (*ctxCallback->callbackProc) (ref, typedata, data);
	  }
     }
   else if (ref < MAX_LocalMenu)

/*** Action interne et optionnelle de l''eur ***/
      switch (ref)
	    {
	       case NumMenuInsert:
		  (*ThotLocalActions[T_rcinsertpaste]) (TRUE, FALSE, (int) data + 1);
		  break;
	       case NumMenuPaste:
		  (*ThotLocalActions[T_rcinsertpaste]) (FALSE, TRUE, (int) data + 1);
		  break;
	       case NumMenuInclude:
		  (*ThotLocalActions[T_rcinsertpaste]) (FALSE, FALSE, (int) data + 1);
		  break;
	       case NumMenuElChoice:
		  (*ThotLocalActions[T_rchoice]) ((int) data + 1, NULL);
		  break;
	       case NumSelectNatureName:
		  (*ThotLocalActions[T_rchoice]) (0, (char *) data);
		  break;
	       case NumMenuCreateReferenceElem:
		  /* Pop-up menu 'Creation element reference'' */
		  (*ThotLocalActions[T_raskfornew]) ((int) data);
		  break;
               case NumMenuChangeType:
                  /* Pop-up menu 'ChangeType' */
                  (*ThotLocalActions[T_rchangetype]) ((int)data);
                  break;
               case NumMenuSurround:
                  /* Pop-up menu 'Surround' */
                  (*ThotLocalActions[T_rsurround]) ((int)data);
                  break;

	       case NumMenuAttrRequired:
	       case NumMenuAttrNumNeeded:
	       case NumMenuAttrTextNeeded:
	       case NumMenuAttrEnumNeeded:
		  (*ThotLocalActions[T_rattrreq]) (ref, (int) data, data);
		  break;
	       case NumMenuAttr:
	       case NumMenuAttrNumber:
	       case NumMenuAttrText:
	       case NumMenuAttrEnum:
		  (*ThotLocalActions[T_rattrval]) (ref, (int) data, data);
		  break;

	       case NumSelectLanguage:
		 (*ThotLocalActions[T_rattrlang]) (ref, 0, data);
		  break;
	       case NumFormLanguage:
	       case NumMenuAlphaLanguage:
		 (*ThotLocalActions[T_rattrlang]) (ref, (int) data, NULL);
		  break;

	       case NumFormCreateDoc:
	       case NumZoneDocNameToCreate:
	       case NumZoneDocDirToCreate:
	       case NumSelDocClassToCreate:
		  (*ThotLocalActions[T_createdoc]) (ref, typedata, data);
		  break;
	       case NumFormConfirm:
		  (*ThotLocalActions[T_confirmcreate]) (ref, typedata, data);
		  break;
	       case NumFormOpenDoc:
	       case NumZoneDirOpenDoc:
	       case NumSelDoc:
	       case NumZoneDocNameToOpen:
		  (*ThotLocalActions[T_opendoc]) (ref, typedata, data);
		  break;
	       case NumSelectImportClass:
	       case NumFormImportClass:
		  (*ThotLocalActions[T_import]) (ref, typedata, data);
		  break;
	       case NumFormPresentationSchema:
	       case NumZonePresentationSchema:
		  (*ThotLocalActions[T_presentation]) (ref, typedata, data);
		  break;
	       case NumFormClose:
		  (*ThotLocalActions[T_rconfirmclose]) (ref, typedata, data);
		  break;

	       case NumMenuZoom:
		  (*ThotLocalActions[T_chzoom]) (ref, typedata, data);
		  break;
	       case NumMenuVisibility:
		  (*ThotLocalActions[T_chvisibility]) (ref, typedata, data);
		  break;
	       case NumMenuViewsToOpen:
		  (*ThotLocalActions[T_openview]) (ref, typedata, data);
		  break;

	       case NumZoneFirstPage:
	       case NumZoneLastPage:
               case NumZoneNbOfCopies:
               case NumZoneReduction:
               case NumMenuNbPagesPerSheet:
	       case NumMenuViewsToPrint:
		   (*ThotLocalActions[T_rextprint]) (ref, (int) data, NULL);
                   break;
               case NumFormPrint:
	       case NumMenuOptions:
	       case NumMenuSupport:
	       case NumMenuPaperFormat:
		  (*ThotLocalActions[T_rprint]) (ref, (int) data, NULL);
		  break;
	       case NumZonePrinterName:
		  (*ThotLocalActions[T_rprint]) (ref, 0, data);
		  break;

	       case NumZoneDocNameTooSave:
	       case NumZoneDirDocToSave:
	       case NumMenuFormatDocToSave:
	       case NumMenuCopyOrRename:
	       case NumFormSaveAs:
		  (*ThotLocalActions[T_rsavedoc]) (ref, typedata, data);
		  break;
	       case NumFormPresChar:
	       case NumFormPresFormat:
	       case NumFormPresGraphics:
	       case NumFormColors:
	       case NumMenuCharFamily:
	       case NumMenuStyleChar:
	       case NumMenuCharFontSize:
	       case NumMenuUnderlineType:
	       case NumMenuUnderlineWeight:
	       case NumMenuAlignment:
	       case NumMenuJustification:
	       case NumMenuWordBreak:
	       case NumZoneRecess:
	       case NumMenuRecessSense:
	       case NumZoneLineSpacing:
	       case NumMenuLineSpacing:
	       case NumMenuStrokeStyle:
	       case NumZoneStrokeWeight:
	       case NumToggleWidthUnchanged:
	       case NumTogglePatternUnchanged:
	       case NumToggleForegroundUnchanged:
	       case NumToggleBackgroundUnchanged:
		  (*ThotLocalActions[T_present]) (ref, (int) data, NULL);
		  break;
	       case NumSelectPattern:
	       case NumSelectForegroundColor:
	       case NumSelectBackgroundColor:
		  (*ThotLocalActions[T_present]) (ref, 0, data);
		  break;
	       case NumFormPresentStandard:
	       case NumMenuPresentStandard:
		  (*ThotLocalActions[T_presentstd]) (ref, (int) data);
		  break;
	       case NumZoneSearchPage:
	       case NumFormSearchPage:
		  (*ThotLocalActions[T_searchpage]) (ref, (int) data);
		  break;
               case NumFormSearchEmptyElement:
                  (*ThotLocalActions[T_searchemptyelt]) (ref, (int) data);
                  break;
               case NumFormSearchReference:
                  (*ThotLocalActions[T_searchrefto]) (ref, (int) data);
                  break;
               case NumFormSearchEmptyReference:
                  (*ThotLocalActions[T_searchemptyref]) (ref, (int) data);
                  break;
	       case NumFormSearchText:
	       case NumMenuReplaceMode:
	       case NumToggleUpperEqualLower:
	       case NumMenuSearchNature:
		  /* sous-menu mode de remplacement */
		  (*ThotLocalActions[T_searchtext]) (ref, (int) data, NULL);
		  break;
	       case NumZoneTextSearch:
	       case NumZoneTextReplace:
	       case NumSelTypeToSearch:
	       case NumSelAttributeToSearch:
		  /* zone de saisie du texte de remplacement */
		  (*ThotLocalActions[T_searchtext]) (ref, 0, data);
		  break;
	       case NumMenuOrSearchText:
		  (*ThotLocalActions[T_locatesearch]) (ref, (int) data);
		  break;

	       default:
		  if (ref >= NumMenuAttrName && ref <= NumMenuAttrName + MAX_ITEM)
		     /* retour du menu des attributs */
		    {
		       TtaSetDialoguePosition ();
		       (*ThotLocalActions[T_rattr]) (ref, (int) data, ActiveFrame);
		    }
                  if (ref >= NumMenuPresNature && ref <= NumMenuPresNature + NbMaxMenuPresNature)
                     /* retour des menus de changement de schema de presentation */
                       if (ThotLocalActions[T_rchangepres] != NULL)
                         (*ThotLocalActions[T_rchangepres]) (ref, (int)data);
		  break;
	    }
   else
     {
/*** Action attachee au retour du dialoque de l'application ***/
	/* Calcule les indices menu, item et frame */
	/* ref = (((item+1) * MAX_MENU + menu) * MAX_ITEM) + frame + MAX_LocalMenu */
	j = ref - MAX_LocalMenu;
	i = j / MAX_ITEM;
	frame = j - (i * MAX_ITEM);	/* reste de la division */
	item = i / MAX_MENU;
	menu = i - (item * MAX_MENU);	/* reste de la division */
	if (frame == 0)
	  {
	     document = 0;
	     view = 0;
	  }
	else
	  {
	     FrameToView (frame, &document, &view);
	     if (document == 0)
		return;
	     menuThot = FindMenu (frame, FrameTable[frame].MenuAttr, &ptrmenu) - 1;
	     if (menu == menuThot)
	       {
		  /* traitement du menu attribut */
		  TtaSetDialoguePosition ();
		  (*ThotLocalActions[T_rattr]) (ref, (int) data, frame);
		  return;
	       }
	     menuThot = FindMenu (frame, FrameTable[frame].MenuSelect, &ptrmenu) - 1;
	     if (menu == menuThot)
	       {
		  /* traitement du menu selection */
		  TtaSetDialoguePosition ();
		  (*ThotLocalActions[T_rselect]) (ref, (int) data + 1, frame);
		  return;
	       }
	  }

	/* Appel de l'action */
	ptrmenu = GetMenu_Ctl (frame, menu);
	action = 0;
	if (ptrmenu != NULL)
	  {
	     if (item != 0)
	       {
		  item--;
		  if (item < ptrmenu->ItemsNb && ptrmenu->ItemsList != NULL)
		     ptrmenu = ptrmenu->ItemsList[item].SubMenu;
	       }
	  }
	if (ptrmenu != NULL)
	  {
	     if ((int) data < ptrmenu->ItemsNb && ptrmenu->ItemsList != NULL)
		action = ptrmenu->ItemsList[(int) data].ItemAction;
	     /*action = GetActionItem(frame, menu, (int)data); */
	     if (action > 0)
		/* l'action existe et le menu est actif */
		if (MenuActionList[action].ActionActive[frame]) {
		   if (MenuActionList[action].User_Action != NULL) {
		       if (((*MenuActionList[action].User_Action) (
		              MenuActionList[action].User_Arg, document, view)) &&
                           (MenuActionList[action].Call_Action != NULL))
		           (*MenuActionList[action].Call_Action) (document, view);
		   } else
		       (*MenuActionList[action].Call_Action) (document, view);
		}
	  }
     }
}				/*ThotCallback */
/* End Of Module Thot */
