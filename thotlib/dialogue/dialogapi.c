/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2002
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * Dialogue API routines
 *
 * Authors: I. Vatton (INRIA)
 *          R. Guetari (W3C/INRIA) - Windows version
 *
 */

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "dialog.h"
#include "application.h"
#include "interface.h"
#include "appdialogue.h"
#include "appli_f.h"
#include "font_f.h"
#include "message.h"
#ifdef _WINDOWS
#include "winsys.h"
#include "wininclude.h"
#define GLOBALS_HERE
#endif /* _WINDOWS */
#ifdef _GTK
#include <gdk/gdkx.h>
#endif /* _GTK */

#define THOT_EXPORT extern
#include "appdialogue_tv.h"
#include "thotcolor_tv.h"
#include "frame_tv.h"
#include "font_tv.h"

/* Catalogues structures */
#define CAT_INT        0
#define CAT_TEXT       1
#define CAT_FORM       2
#define CAT_MENU       3
#define CAT_POPUP      4
#define CAT_FMENU      5
#define CAT_SHEET      6
#define CAT_SELECT     7
#define CAT_TMENU      8
#define CAT_LABEL      9
#define CAT_DIALOG    10
#define CAT_PULL      11
#define CAT_ICON      12
#define CAT_SCRPOPUP  13

#define MAX_CAT       20
#define C_NUMBER      15
#define INTEGER_DATA   1
#define STRING_DATA    2
#define MAX_TXTMSG   120
#define MAX_ARGS      20

struct E_List
  {
     struct     E_List* E_Next;         /* CS_List d'entrees suivante         */
     char     E_Free[C_NUMBER];       /* Disponibilite des entrees         */
     char     E_Type[C_NUMBER];       /* CsList des types des entrees      */
     ThotWidget E_ThotWidget[C_NUMBER]; /* ThotWidgets associes aux entrees  */
  };

struct Cat_Context
  {
    int                 Cat_Ref;	/* CsReference appli du catalogue    */
    unsigned char       Cat_Type;	/* Type du catalogue                 */
    unsigned char       Cat_Button;	/* Le bouton qui active              */
    unsigned char       Cat_Default;    /* Defaulft return                   */
    union
    {
      int                Catu_Data;        /* Valeur de retour                  */
      ThotWidget	 Catu_XtWParent;
    } Cat_Union1;
    union
    {
      int                Catu_in_lines;	/* Orientation des formulaires       */
      ThotWidget	 Catu_SelectLabel;
    } Cat_Union2;
    ThotWidget          Cat_Widget;	/* Le widget associe au catalogue    */
    ThotWidget          Cat_Title;	/* Le widget du titre                */
    ThotWidget          Cat_ParentWidget; /* pointer to the widget parent */
    struct Cat_Context *Cat_PtParent;	/* Adresse du catalogue pere         */
    int                 Cat_EntryParent; /* Entree du menu parent            */
    ThotBool            Cat_React;	/* Indicateur reaction immediate     */
    ThotBool            Cat_SelectList; /* Indicateur selecteur = liste      */
    struct E_List      *Cat_Entries;	/* CsList des entrees d'un menu      */
                                        /* ou widget de saisie de texte      */
  };

/* Redefiniton de champs de catalogues dans certains cas */
#define Cat_ListLength  Cat_Union1.Catu_Data
#define Cat_FormPack    Cat_Union1.Catu_Data
#define Cat_Data	Cat_Union1.Catu_Data
#define Cat_XtWParent   Cat_Union1.Catu_XtWParent
#define Cat_in_lines	Cat_Union2.Catu_in_lines
#define Cat_SelectLabel Cat_Union2.Catu_SelectLabel
#define Cat_Focus       Cat_React

struct Cat_List
  {
     struct Cat_List    *Cat_Next;
     struct Cat_Context  Cat_Table[MAX_CAT];
  };

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "frame_tv.h"
#include "appdialogue_tv.h"

#ifdef _GTK
#include "gtk-functions.h" /* GTK prototype */
extern int          appArgc;
extern char**     appArgv;
#endif /*_GTK */

ThotBool            WithMessages = TRUE;
/* First free reference */
static int          FirstFreeRef;
/* List of catalogues */
static struct Cat_List*    PtrCatalogue;
static struct E_List*      PtrFreeE_List;
static int                 NbOccE_List;
static int                 NbLibE_List;
static int                 CurrentWait;
static int                 ShowReturn;
static int                 ShowX, ShowY;
static struct Cat_Context* ShowCat = NULL;
static ThotTranslations    TextTranslations;
static ThotWidget          MainShell, PopShell;


#ifdef _WINDOWS
static HFONT          formFONT;
char                 *iconID;
static OPENFILENAME  OpenFileName;
static int           cyValue = 10;
#else  /* _WINDOWS */
#ifndef _GTK
static XmFontList     formFONT;
static ThotAppContext Def_AppCont;
static Display       *GDp;
#else /* _GTK */
static GdkFont       *formFONT;
#endif /* _GTK */
#endif /* _WINDOWS */

#include "appdialogue_f.h"
#include "memory_f.h"
#include "thotmsg_f.h"

#ifdef _WINDOWS

#define MAX_FRAMECAT 50
typedef struct FrCatalogue {
  struct Cat_Context * Cat_Table[MAX_FRAMECAT];
} FrCatalogue;


FrCatalogue FrameCatList [MAX_FRAME + 1];
LRESULT CALLBACK WndProc        (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ClientWndProc  (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ThotDlgProc    (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK TxtZoneWndProc (HWND, UINT, WPARAM, LPARAM);

static int          nAmayaShow;
/* following variables are declared as extern in frame_tv.h */
HINSTANCE           hInstance = 0;

HBITMAP             WIN_LastBitmap = 0;

typedef struct WIN_Form
{
  HWND Buttons [10]; /* Dialog Button      */
  int  x  [10];      /* Initial x position */
  int  cx [10];      /* Button width       */
} WIN_Form;

static int      bIndex = 0;
static int      bAbsBase = 60;
static WIN_Form formulary;
static BYTE     fVirt;
static char     key;

UINT subMenuID [MAX_FRAME];
extern int main (int, char**);
static struct Cat_Context *CatEntry (int ref);

/*----------------------------------------------------------------------
  GetMenuParentNumber:  returns the Thot window number associated to a     
  given menu.
  ----------------------------------------------------------------------*/
static int GetMenuParentNumber (ThotMenu menu)
{
  int      menuIndex;
  int      iframe = 0;
  int      frame = -1;
  ThotBool found = FALSE;
  
  while (iframe <= MAX_FRAME && !found)
    {
      menuIndex = 0;
      while (menuIndex < MAX_MENU && !found) 
	if (FrameTable[iframe].WdMenus[menuIndex] == menu)
	  {
	    frame = iframe;
	    found = TRUE;
	  }
	else 
	  menuIndex++;
      if (!found)
	iframe++;
    }
  return frame;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
HMENU WIN_GetMenu (int frame)
{
  return (GetMenu (FrMainRef [frame]));
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void WIN_AddFrameCatalogue (ThotWidget parent, struct Cat_Context* catalogue)
{
  int                 iframe;
  int                 twIndex;
  int                 i, j;
  int                 frame;
  ThotBool            found;

  frame = GetMainFrameNumber (parent);
  if (frame == -1)
    frame = GetMenuParentNumber ((ThotMenu) parent);
  if (frame == - 1)
    {
      iframe = 0;
      found = FALSE;
      while (iframe <= MAX_FRAME && !found)
	{
	  i = 0;
	  while (i < MAX_FRAMECAT && !found)
	    {
	      twIndex = 0;
	      while (twIndex < C_NUMBER && !found)
		{
		  if (FrameCatList[iframe].Cat_Table[i] &&
		      FrameCatList[iframe].Cat_Table[i]->Cat_Entries &&
		      FrameCatList[iframe].Cat_Table[i]->Cat_Entries->E_ThotWidget[twIndex] == parent)
		    {
		      found = TRUE;
		      frame = iframe;
		    }
		  else
		    twIndex++;
		} 
	      if (!found)
		i++;
	    }
	  if (!found)
	    iframe++;
	}
    }
  else if (frame > 0 && frame < MAX_FRAME)
    {
      found = FALSE;
      i = 0;
      while (i < MAX_FRAMECAT && FrameCatList[frame].Cat_Table[i] && !found)
	{
	  if (FrameCatList[frame].Cat_Table[i]->Cat_Ref == catalogue->Cat_Ref)
	    {
	      found = TRUE;
	      if (FrameCatList[frame].Cat_Table[i] != catalogue)
		FrameCatList[frame].Cat_Table[i] = catalogue;
	    }
	  else if (FrameCatList[frame].Cat_Table[i]->Cat_Ref > catalogue->Cat_Ref)
	    {
	      /* the new catalogue must be inserted before */
	      j = MAX_FRAMECAT  - 1;
	      while (j >= i)
		{
		  if (FrameCatList[frame].Cat_Table[j])
		    FrameCatList[frame].Cat_Table[j + 1] = FrameCatList[frame].Cat_Table[j];
		  j--;
		}
	      FrameCatList[frame].Cat_Table[i] = catalogue;
	      found = TRUE;
	    }
	  else
	    i++;
	}
      if (i < MAX_FRAMECAT && !found) 
	FrameCatList[frame].Cat_Table[i] = catalogue;
    }
}

/*----------------------------------------------------------------------
  CleanFrameCatList removes all entries (ref = 0) or a specific entry.
  ----------------------------------------------------------------------*/
void CleanFrameCatList (int frame, int ref) 
{
  int         i;
  ThotBool    found;

  if (ref == 0)
    {
      /* clean up all entries */
      for (i = 0; i < MAX_FRAMECAT; i++)
	FrameCatList[frame].Cat_Table[i] = NULL;
    }
  else
    {
      i = 0;
      found = FALSE;
      while (i < MAX_FRAMECAT && FrameCatList[frame].Cat_Table[i] && !found)
	{
	  if (FrameCatList[frame].Cat_Table[i]->Cat_Ref == ref)
	    {
	      /* remove this entry */
	      while (i < MAX_FRAMECAT - 1 && FrameCatList[frame].Cat_Table[i + 1])
		{
		  FrameCatList[frame].Cat_Table[i] = FrameCatList[frame].Cat_Table[i + 1];
		  i++;
		}
	      FrameCatList[frame].Cat_Table[i] = NULL;
	      found = TRUE;
	    }
	  else
	    i++;
	}
    }
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
int makeArgcArgv (HINSTANCE hInst, char*** pArgv, char* cmdLine)
{ 
  int          argc;
  static char *argv[20];
  static char  argv0[MAX_TXT_LEN];
  static char  commandLine [MAX_TXT_LEN];
  char        *ptr;
  char         lookFor = 0;
  enum {
    nowAt_start, 
    nowAt_text
  } nowAt;

  strcpy (commandLine, cmdLine);
  ptr = commandLine;
  *pArgv = argv;
  argc = 0;
  GetModuleFileName (hInst, (LPTSTR)argv0, sizeof (argv0));
  argv[argc++] = argv0;
  for (nowAt = nowAt_start;;)
    {
      if (!*ptr) 
	return (argc);
      if (lookFor)
	{
	  if (*ptr == lookFor)
	    {
	      nowAt = nowAt_start;
	      lookFor = 0;
	      *ptr = 0;   /* remove the quote */
	    }
	  else if (nowAt == nowAt_start)
	    {
	      argv[argc++] = ptr;
	      nowAt = nowAt_text;
	    }
	  ptr++;
	  continue;
        }
      if (*ptr == SPACE || *ptr == TAB)
	{
	  *ptr = 0;
	  ptr++;
	  nowAt = nowAt_start;
	  continue;
        }
      if ((*ptr == '\'' || *ptr == '\"' || *ptr == '`') && nowAt == nowAt_start)
	{
	  lookFor = *ptr;
	  nowAt = nowAt_start;
	  ptr++;
	  continue;
        }
      if (nowAt == nowAt_start)
	{
	  argv[argc++] = ptr;
	  nowAt = nowAt_text;
        }
      ptr++;
    }
}

/*----------------------------------------------------------------------
   WinMain
  ----------------------------------------------------------------------*/
BOOL PASCAL WinMain (HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCommand, int nShow)
{ 
  int        argc;
  char**   argv;

#ifdef IV_DEBUG
   /* Get all memory leak in the debug window after using
   debug and closing the application*/
    #define _CRTDBG_MAP_ALLOC
    #include <stdlib.h>
    #include <crtdbg.h>


	int flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG); 

	flag |= _CRTDBG_ALLOC_MEM_DF  | _CRTDBG_LEAK_CHECK_DF; 
	_CrtSetDbgFlag(flag); 

/*Launch the app in debug mode, follow a strict scenario, then exit.
in the debug output you'll see memory leak dump, and where they were allocated
Relaunch the app,  break near the beginning
then use quickwatch (clc right button on text canvas)
then enter _crtBreakAlloc and click "recalculate"
Instead of the "-1" result, enter the number in the brace
C:\Amaya\thotlib\image\gifhandler.c(1520) : {40493} normal block at 0x01F95C18, 57760 bytes long.
here, you'll enter 40493
Follow the same scenario as before, and the app will break at the right allocation that 
had never been FREED !!
http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vsdebug/html/vxconsettingbreakpointonmemoryallocationnumber.asp
*/


    /* When the count reaches zero !!!
	http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vclib/html/_crt__crtsetreportmode.asp

	_CrtSetReportMode (_CRT_WARN, _CRTDBG_MODE_DEBUG | _CRTDBG_MODE_WNDW );
	_CrtSetReportMode (_CRT_ERROR, _CRTDBG_MODE_DEBUG | _CRTDBG_MODE_WNDW );
	_CrtSetReportMode (_CRT_ASSERT, _CRTDBG_MODE_DEBUG | _CRTDBG_MODE_WNDW );
    */
	/* Opens a Console for debugging purpose*/
	/*InitConsoleDebug();   */


/*	MS Visual Conditional Breakpoints
Set a breakpoint (F9) 
Now, edit breakpoint ( Alt -F9 ) 
Select the breakpoint you just created, and hit the "Condition" tab 
set a condit ion for the breakpoint .
*/

   currentFrame = -1;
   hInstance = hInst;
   nAmayaShow = nShow;
   argc = makeArgcArgv (hInst, &argv, lpCommand);
   main (argc, argv);
   /* Close the debugging console*/
   /*ExitConsoleDebug ();*/
	return (TRUE);
#else /*_DEBUG*/   


  currentFrame = -1;
  hInstance = hInst;
  nAmayaShow = nShow;
  argc = makeArgcArgv (hInst, &argv, lpCommand);
  main (argc, argv);
  return (TRUE);
#endif /*_DEBUG*/
}

/*----------------------------------------------------------------------
  WIN_ListOpenDirectory
  ----------------------------------------------------------------------*/
void WIN_ListOpenDirectory (HWND parent, char *fileName)
{
  char *szFilter;
  char szFileName[256];

  szFilter = APPFILENAMEFILTER;
  OpenFileName.lStructSize = sizeof (OPENFILENAME); 
  OpenFileName.hwndOwner = parent; 
  OpenFileName.hInstance = hInstance; 
  OpenFileName.lpstrFilter = szFilter; 
  OpenFileName.lpstrCustomFilter = NULL; 
  OpenFileName.nMaxCustFilter = 0L; 
  OpenFileName.nFilterIndex = 1L; 
  OpenFileName.lpstrFile = szFileName; 
  OpenFileName.nMaxFile = 256; 
  OpenFileName.lpstrInitialDir = NULL; 
  OpenFileName.lpstrTitle = "Open a File"; 
  OpenFileName.nFileOffset = 0; 
  OpenFileName.nFileExtension = 0; 
  OpenFileName.lpstrDefExt = "*.html"; 
  OpenFileName.lCustData = 0; 
  OpenFileName.Flags = OFN_SHOWHELP | OFN_HIDEREADONLY; 
  if (GetOpenFileName (&OpenFileName))
    strcpy (fileName, OpenFileName.lpstrFile);
}

/*----------------------------------------------------------------------
  WIN_ListSaveDirectory
  ----------------------------------------------------------------------*/
void WIN_ListSaveDirectory (int parentRef, char *title, char *fileName)
{
  struct Cat_Context *parentCatalogue;
  char               *szFilter;
  char                szFileName[256];
  char                szFileTitle[256];

  parentCatalogue = CatEntry (parentRef);
  szFilter = APPFILENAMEFILTER;
  strncpy (szFileName, fileName, 256);
  OpenFileName.lStructSize = sizeof (OPENFILENAME); 
  OpenFileName.hwndOwner = parentCatalogue->Cat_Widget; 
  OpenFileName.lpstrFilter = szFilter;
  OpenFileName.lpstrFile = szFileName; 
  OpenFileName.nMaxFile = sizeof (szFileName); 
  OpenFileName.lpstrFileTitle = szFileTitle; 
  OpenFileName.lpstrTitle = title; 
  OpenFileName.nMaxFileTitle = sizeof (szFileTitle); 
  OpenFileName.lpstrInitialDir = NULL; 
  OpenFileName.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
  if (GetSaveFileName (&OpenFileName))
    strcpy (fileName, OpenFileName.lpstrFile);
}

/*-----------------------------------------------------------------------
 ThotDlgProc
 ------------------------------------------------------------------------*/
LRESULT CALLBACK ThotDlgProc (HWND hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  int        ndx;
    
  switch (msg)
    {
    case WM_CREATE:
      {
	for (ndx = 0; ndx < bIndex; ndx ++)
	  {
	    SetParent (formulary.Buttons[ndx], hwnDlg);
	    ShowWindow (formulary.Buttons[ndx], SW_SHOW);
	  }
	return 0;
      }
    case WM_COMMAND:
      switch (LOWORD (wParam))
	{
	case IDCANCEL: DestroyWindow (hwnDlg);
	  return 0;
	default:       WIN_ThotCallBack (GetParent (hwnDlg), wParam , lParam);
	  return 0;
	}
    default: return (DefWindowProc (hwnDlg, msg, wParam, lParam));
    }
}
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
  Procedure de retour par defaut.
  ----------------------------------------------------------------------*/
static void CallbackError (int ref, int typedata, char *data)
{
  printf ("Toolkit error : No callback procedure ...\n");
}

static void (*CallbackDialogue) () = CallbackError;
/*----------------------------------------------------------------------
   NewCatList Creates a new catalogue list.                           
  ----------------------------------------------------------------------*/
static struct Cat_List *NewCatList ()
{
  register int        i;
  struct Cat_List    *adlist;

  adlist = (struct Cat_List *) TtaGetMemory (sizeof (struct Cat_List));
  adlist->Cat_Next = NULL;
  /* Updates thenumber of available and free catalogues */
  for (i = 0; i < MAX_CAT; i++)
    {
      adlist->Cat_Table[i].Cat_Widget = 0;
      adlist->Cat_Table[i].Cat_Entries = NULL;
    }
  return (adlist);
}

/*----------------------------------------------------------------------
   NewEList: creates a new block of elements.                         
  ----------------------------------------------------------------------*/
static struct E_List *NewEList ()
{
  register int        i;
  struct E_List      *adbloc;

  /* verifies if there is a free block */
  if (PtrFreeE_List == NULL)
    adbloc = (struct E_List *) TtaGetMemory (sizeof (struct E_List));
  else
    {
      adbloc = PtrFreeE_List;
      PtrFreeE_List = adbloc->E_Next;
      NbLibE_List--;
    }
  /* Updates the number of blocks of allocated E_List */
  NbOccE_List++;
  adbloc->E_Next = NULL;
  for (i = 0; i < C_NUMBER; i++)
    adbloc->E_ThotWidget[i] = 0;
  return (adbloc);
}

/*----------------------------------------------------------------------
   FreeEList: Releases all blocks of elements.                        
  ----------------------------------------------------------------------*/
static void FreeEList (struct E_List *adbloc)
{
  struct E_List      *cebloc;

  cebloc = adbloc;
  while (cebloc != NULL)
    {
      NbLibE_List++;
      if (cebloc->E_Next == NULL)
	{
	  cebloc->E_Next = PtrFreeE_List;
	  PtrFreeE_List = adbloc;
	  cebloc = NULL;
	}
      else
	cebloc = cebloc->E_Next;
    }
}

/*----------------------------------------------------------------------
   CatEntry recherche si le catalogue de'signe' par sa re'fe'rence   
   existe de'ja` ou une entre'e libre dans la table des catalogues.   
   Retourne l'adresse du catalogue cre'e' ou NULL.                    
  ----------------------------------------------------------------------*/
static struct Cat_Context *CatEntry (int ref)
{
  register int        icat;
  struct Cat_Context *catlib;
  struct Cat_Context *catval;
  struct Cat_Context *catalogue;
  struct Cat_List    *adlist;

  /* Si la reference depasse la borne declaree */
  if (ref >= FirstFreeRef)
    return (NULL);
  /* Une entree de catalogue libre */
  catlib = NULL;
  /* L'entree qui porte la reference */
  catval = NULL;
  /* Parcours toutes les entrees existantes */
  adlist = PtrCatalogue;
  while (adlist != NULL && catval == NULL)
    {
      icat = 0;
      while (icat < MAX_CAT && catval == NULL)
	{
	  catalogue = &adlist->Cat_Table[icat];
	  if (catalogue->Cat_Widget == 0)
	    {
	      if (catlib == NULL)
		catlib = catalogue;
	    }
	  else if (catalogue->Cat_Ref == ref)
	    /* the catalogue already exists */
	    catval = catalogue;
	  icat++;
	}
      /* On passe au bloc suivant */
      if (adlist->Cat_Next == NULL && catval == NULL && catlib == NULL)
	{
	  /* Cree une nouvelle liste de catalogues */
	  adlist->Cat_Next = NewCatList ();
	}
      adlist = adlist->Cat_Next;
    }
  /* Si le catalogue n'existe pas encore */
  if (catval == NULL && catlib != NULL)
    {
      catlib->Cat_PtParent = NULL;
      catlib->Cat_Default = 1;
      return (catlib);
    }
  else
    return (catval);
}

/*----------------------------------------------------------------------
   Catwidget returns the widget that owns the catalog given in ref
  ----------------------------------------------------------------------*/
ThotWidget CatWidget(int ref)
{
  struct Cat_Context *catalogue;
  
  catalogue  = CatEntry (ref);
  return (catalogue->Cat_Widget);
}
#if !defined(_WINDOWS) && !defined(_GTK)
/*----------------------------------------------------------------------
  Callback for closing a menu
  ----------------------------------------------------------------------*/
static void UnmapMenu (ThotWidget w, struct Cat_Context *catalogue, caddr_t call_d)
{
  struct Cat_Context *icatal;
  
  /* Une attende est peut etre debloquee */
  icatal = catalogue;
  while (icatal->Cat_PtParent != NULL)
    icatal = icatal->Cat_PtParent;
  if (icatal == ShowCat && ShowReturn == 1)
    ShowReturn = 0;
}
#endif /* _GTK && _WINDOWS */

/*----------------------------------------------------------------------
  Callback for a menu button                                         
  ----------------------------------------------------------------------*/
#ifndef _GTK
static void CallMenu (ThotWidget w, struct Cat_Context *catalogue, caddr_t call_d)
#else /* _GTK */
static ThotBool CallMenuGTK (ThotWidget w, struct Cat_Context *catalogue)
#endif /* _GTK */
{
  register int        i;
  register int        index;
  register int        entry;
  struct E_List      *adbloc;
  struct Cat_Context *icatal;
#ifdef _WINDOWS
  int                 ndx;
  UINT                menuEntry;
  int                 nbMenuItem;
#endif /* _WINDOWS */

  /* Une attende est peut etre debloquee */
  icatal = catalogue;
  while (icatal->Cat_PtParent != NULL)
    icatal = icatal->Cat_PtParent;
  if (icatal == ShowCat && ShowReturn == 1)
    ShowReturn = 0;
  /* A menu entry is selected */
  if (catalogue->Cat_Widget != 0)
    {
      if ((int) catalogue->Cat_Widget == -1)
	/*** back to a simple button ***/
	(*CallbackDialogue) (catalogue->Cat_Ref, INTEGER_DATA, 0);
      else
	{
	  adbloc = catalogue->Cat_Entries;
	  entry = -1;
	  index = 0;
	  i = 2;		/* decalage de 2 pour le widget titre */
	  while ((entry == -1) && (adbloc != NULL))
	    {
	      while ((entry == -1) && (i < C_NUMBER))
		{
#ifdef _WINDOWS
		  if (IsMenu ((ThotMenu) (adbloc->E_ThotWidget[i])))
		    {
		      nbMenuItem = GetMenuItemCount ((ThotMenu) (adbloc->E_ThotWidget[i]));
		      for (ndx = 0; ndx < nbMenuItem; ndx++)
			{
			  menuEntry = GetMenuItemID ((ThotMenu) (adbloc->E_ThotWidget[i]), ndx);
			  if (menuEntry == (catalogue->Cat_Ref + (UINT)w))
			    entry = ndx;
			}
		    }
		  else
#endif /* _WINDOWS */
		    if (adbloc->E_ThotWidget[i] == w)
		      entry = index;
		  i++;
		  index++;
		}
	      /* Passe au bloc suivant */
	      adbloc = adbloc->E_Next;
	      i = 0;
	  }
	  /*** Retour de l'entree du menu choisie vers l'application ***/
	  (*CallbackDialogue) (catalogue->Cat_Ref, INTEGER_DATA, entry);
	}
    }
  return TRUE;
}

/*----------------------------------------------------------------------
  Callback pour un bouton du toggle-menu
  ----------------------------------------------------------------------*/
static ThotBool CallToggle (ThotWidget w, struct Cat_Context *catalogue, caddr_t call_d)
{
  register int        i;
  int                 entry;
  int                 ent;
  int                 max;
  struct E_List      *adbloc;

  /* Enregistre la selection d'un toggle button */
  if (catalogue->Cat_Widget != 0)
    {
      adbloc = catalogue->Cat_Entries;
      entry = -1;
      ent = 0;
      max = catalogue->Cat_Data;	/* nombre d'entrees definies */
      i = 2;			/* decalage de 2 pour le widget titre */
      while (entry == -1 && adbloc != NULL && ent <= max)
	{
	  while (entry == -1 && i < C_NUMBER && ent <= max)
	    {
	      if (adbloc->E_ThotWidget[i] == w)
		{
		  entry = 0;
		  /* Bascule la valeur du bouton correspondant a l'entree */
		  adbloc->E_Free[i] = 'Y';
		  /* signale que l'entree est basculee si le menu est reactif */
		  if (catalogue->Cat_React)
		    (*CallbackDialogue) (catalogue->Cat_Ref, INTEGER_DATA, ent);
		}
	      i++;
	      ent++;
	    }
	  /* Passe au bloc suivant */
	  adbloc = adbloc->E_Next;
	  i = 0;
	}
    }
  return TRUE;  
}

/*----------------------------------------------------------------------
  Callback pour un bouton du sous-menu de formulaire
  ----------------------------------------------------------------------*/
static ThotBool CallRadio (ThotWidget w, struct Cat_Context *catalogue, caddr_t call_d)
{
  register int        i;
  register int        index;
  register int        entry;
  struct E_List      *adbloc;

  /* Enregistre la selection d'un toggle button */
  if (catalogue->Cat_Widget != 0)
    {
      adbloc = catalogue->Cat_Entries;
      entry = -1;
      index = 0;
      i = 2;			/* decalage de 2 pour le widget titre */
      while ((entry == -1) && (adbloc != NULL))
	{
	  while ((entry == -1) && (i < C_NUMBER))
	    {
	      if (adbloc->E_ThotWidget[i] == w)
		entry = index;
	      i++;
	      index++;
	    }
	  /* Passe au bloc suivant */
	  adbloc = adbloc->E_Next;
	  i = 0;
	}
      /*** Sauve la valeur de la derniere selection ***/
      catalogue->Cat_Data = entry;
      /* retourne la valeur si le menu est reactif */
      if (catalogue->Cat_React)
	(*CallbackDialogue) (catalogue->Cat_Ref, INTEGER_DATA, entry);
    }
  return TRUE;
  
}

#ifdef _WINDOWS
/*-----------------------------------------------------------------------
 Win_ScrPopupProc The callback handler for the Scroll popup widget
 ------------------------------------------------------------------------*/
LRESULT CALLBACK WIN_ScrPopupProc (HWND hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
 static HWND scrPopupWin;
 HWND   listBox;
 struct Cat_Context *catalogue;
 int    itemIndex, ref;

  switch (msg)
    {
      /* initialize the widget */
    case WM_CREATE:
      {
	HFONT  newFont;
        HWND   listBox;
        DWORD  dwStyle;
        RECT   rect;
	HDC    display;
	TEXTMETRIC  textMetric;
	int    width;
	int    height;

	/* get the rectangle size according to the font metrics (hack) */
	newFont = GetStockObject (DEFAULT_GUI_FONT); 
	if (newFont)
	  SendMessage (hwnDlg, WM_SETFONT, (WPARAM) newFont, MAKELPARAM(FALSE, 0));

	GetWindowRect (hwnDlg, &rect);
	width = rect.right - rect.left;
	height = rect.bottom - rect.top;

	display = GetDC (hwnDlg);
	if (GetTextMetrics (display, &textMetric))
	  {
	    height = height * textMetric.tmHeight;
	    width = width * (textMetric.tmAveCharWidth);
	  }
	else 
	  {
	    /* try to give it some value */
	    height = height * 14;
	    width = width * 14;
	  }
	ReleaseDC (hwnDlg, display);
	/* we change the settings of the current window too */
	SetWindowPos (hwnDlg, NULL, rect.left, rect.top, width, height, SWP_NOZORDER);

	/* create a list box inside the container window */
	scrPopupWin = hwnDlg;
	dwStyle = WS_BORDER | WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL 
	  | LBS_HASSTRINGS | LBS_NOTIFY | LBS_WANTKEYBOARDINPUT;
	/* give it the same size as that of the container */

	listBox = CreateWindowEx (WS_EX_CLIENTEDGE, "LISTBOX", NULL, dwStyle, 0, 0,
				  width, height,
				  hwnDlg, (HMENU) 1, hInstance, NULL);
	/* set the font of the window */
	newFont = GetStockObject (DEFAULT_GUI_FONT); 
	if (newFont)
	  SendMessage (listBox, WM_SETFONT, (WPARAM) newFont, MAKELPARAM(FALSE, 0));
	SetFocus (listBox);
	return 0;
      }
      break;
      /* destroy the widget */
    case WM_CLOSE:
    case WM_DESTROY:
      RemoveProp (hwnDlg, "ref");
      scrPopupWin = NULL;
      if (msg == WM_DESTROY)
	PostQuitMessage (0);
      return 0;
      break;
    case WM_VKEYTOITEM:
      ref = (int) wParam;
      switch ((TCHAR) wParam)
	{
	case VK_RETURN:  /* activate an entry */
	  SendMessage (hwnDlg, WM_COMMAND, MAKEWPARAM (1, LBN_DBLCLK), MAKELPARAM(FALSE, 0));
	  return -2;
	  break;
	case VK_ESCAPE:  /* cancel */
	  DestroyWindow (hwnDlg);
	  return -2;
	  break;
	}
      break;
    case WM_COMMAND:
      if (LOWORD (wParam) == 1)
	{
	  switch (HIWORD (wParam))
	    {
	    case LBN_DBLCLK:  /* activate an entry */
	      {
		listBox = GetDlgItem (hwnDlg, 1);
		itemIndex = SendMessage (listBox, LB_GETCURSEL, 0, 0);
		ref = (int) GetProp (hwnDlg, "ref");
		catalogue = CatEntry (ref);
		CallMenu ((ThotWidget) itemIndex, catalogue, NULL);
		scrPopupWin = NULL;
		DestroyWindow (hwnDlg);
		return 0;
	      }
	      break;
	    case LBN_KILLFOCUS:   /* destroy the window if we click elsewhere */
	      if (scrPopupWin)
		{
		  HWND win;
		  win = GetFocus ();
		  
		  if (win != scrPopupWin && GetParent (win) != scrPopupWin)
		    {
		      scrPopupWin = NULL;
		      DestroyWindow (hwnDlg);
		      return 0;
		    }
		}
	      break;
	    }
	}
      break;
    }
  return (DefWindowProc (hwnDlg, msg, wParam, lParam));
}

/*----------------------------------------------------------------------
  WIN_InitScrPopup
  System calls for creating an empty  scroll popup widget under Windows.
  Returns a pointer to the widget if succesful, NULL otherwise.
  ----------------------------------------------------------------------*/
static HWND WIN_InitScrPopup (ThotWindow parent, int ref,
                              ThotBool multipleOptions, int nbOptions,
			      int width, int height)
{
  WNDCLASS      wndSheetClass;
  LPCSTR        szAppName;
  static ATOM   wndScrPopupRegistered;
  HWND          menu;
  POINT         curPoint;

  szAppName = (LPCSTR) "MYSCRPOPUP";
  /* register the popup scroll widget class if it doesn't exists */
  if (!wndScrPopupRegistered)
    {
      wndSheetClass.style         = CS_HREDRAW | CS_VREDRAW;
      wndSheetClass.lpfnWndProc   = (WNDPROC) WIN_ScrPopupProc;
      wndSheetClass.cbClsExtra    = 0;
      wndSheetClass.cbWndExtra    = 0;
      wndSheetClass.hInstance     = hInstance;
      wndSheetClass.hIcon         = LoadIcon (NULL, IDI_APPLICATION);
      wndSheetClass.hCursor       = LoadCursor (NULL, IDC_ARROW);
      wndSheetClass.hbrBackground = (HBRUSH) GetStockObject (NULL_BRUSH);
      wndSheetClass.lpszMenuName  = NULL;
      wndSheetClass.lpszClassName = szAppName;
      wndScrPopupRegistered = RegisterClass (&wndSheetClass);
      if (!wndScrPopupRegistered)
	return (NULL);
    }
  /* we don't use the multipleOptions variable anymore */
  /* we use a global variable, as I was unable to pass a parameter with CreateWindow */
  /* multipleSel = multipleOptions; */

  /* compute the height and width in pixels, using the system font */
  
  /* create a widget instance at the current cursor position */
  GetCursorPos (&curPoint);
  menu = CreateWindow (szAppName,  NULL,
		       WS_POPUP, curPoint.x, curPoint.y,
		       width, height,
		       parent, NULL, hInstance, NULL);
  if (!menu)
    return NULL;

  /* store the catalogue reference inside the window */
  SetProp (menu, "ref", (HANDLE) ref);

  return menu;
}

/*----------------------------------------------------------------------
   Callback pour un bouton du menu                                    
  ----------------------------------------------------------------------*/
void WIN_ThotCallBack (HWND hWnd, WPARAM wParam, LPARAM lParam)
{
   struct Cat_Context *catalogue;
   struct Cat_Context *nearest;
   int                 i;
   int                 frame;
   int                 ref;
   ThotBool            found;

#ifdef AMAYA_DEBUG
   fprintf (stderr, "Got WIN_ThotCallBack(%X, %X(%d:%d), %X(%d))\n",
	    hWnd, wParam, HIWORD (wParam), LOWORD (wParam), lParam, lParam);
#endif /* AMAYA_DEBUG */
   frame = GetMainFrameNumber (hWnd);
   if (frame > 0 && frame <= MAX_FRAME)
   {
     nearest = NULL;
     ref = LOWORD (wParam);
     if (ref == 0)
       return;
     i = 0;
     found = FALSE;
     while (!found && i < MAX_FRAMECAT && FrameCatList[frame].Cat_Table[i])
       {
	 catalogue = FrameCatList[frame].Cat_Table[i];
	 if (catalogue)
	   {
	     if (catalogue->Cat_Ref == ref)
	       found = TRUE;
	     else if (nearest == NULL)
	       nearest = catalogue;
	     else if (ref >= catalogue->Cat_Ref &&
		      ref - catalogue->Cat_Ref < ref - nearest->Cat_Ref)
	       nearest = catalogue;
	   }
	 i++;
       }
     if (!found)
       catalogue = nearest;
     if (catalogue == NULL)
       return;
     ref = ref - catalogue->Cat_Ref;
     switch (catalogue->Cat_Type)
       {
       case CAT_PULL:
       case CAT_MENU:
       case CAT_POPUP:
       case CAT_SCRPOPUP:
	 CallMenu ((ThotWidget)ref, catalogue, NULL);
	 break;
       case CAT_TMENU:
	 CallToggle ((ThotWidget)ref, catalogue, NULL);
	 break;
       case CAT_SHEET:
       case CAT_FMENU:
	 CallRadio ((ThotWidget)ref, catalogue, NULL);
	 break;
       default:
	 break;
       }
   }
}

#else /* _WINDOWS */
/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void ConfirmMessage (ThotWidget w, ThotWidget MsgBox, caddr_t call_d)
{
#ifndef _GTK
   XtPopdown (MsgBox);
#else /* _GTK */
   gtk_widget_hide (MsgBox);
#endif /* _GTK */
}

/*----------------------------------------------------------------------
  Delete a form
  ----------------------------------------------------------------------*/
#ifndef _GTK
static void formKill (ThotWidget w, struct Cat_Context *catalogue, caddr_t call_d)
#else /* _GTK */
static void formKillGTK (GtkWidget *widget, GdkEvent *event, gpointer data)
#endif /* _GTK */
{
#ifdef _GTK
  struct Cat_Context *catalogue;
  
  catalogue = (struct Cat_Context *) data;
  if (catalogue == NULL || widget == NULL)
    return;
#endif /* _GTK */
  /* Le widget est detruit */
  if (catalogue->Cat_Type == CAT_FORM || catalogue->Cat_Type == CAT_SHEET ||
      catalogue->Cat_Type == CAT_DIALOG || catalogue->Cat_Type == CAT_POPUP ||
      catalogue->Cat_Type == CAT_SCRPOPUP)
    TtaDestroyDialogue (catalogue->Cat_Ref);
}

#ifdef _GTK
/*----------------------------------------------------------------------
  Callback for a scrolled window (click) @JK
  ----------------------------------------------------------------------*/
static gboolean CallPopGTK (GtkWidget *widget, gpointer data)
{
  struct Cat_Context *catalogue;
  GtkWidget          *window;

  catalogue = (struct Cat_Context *) data;  
  window = (GtkWidget *) gtk_object_get_data (GTK_OBJECT (widget), 
					      "window");
  if (window)
    if (GTK_WIDGET_HAS_GRAB (window))
      {
	gtk_grab_remove (window);
	gdk_pointer_ungrab (GDK_CURRENT_TIME);
	gdk_keyboard_ungrab (GDK_CURRENT_TIME);
      }
  CallMenuGTK ((ThotWidget) widget, catalogue);
  return (FALSE);
}

/*----------------------------------------------------------------------
  Callback for a scrolled window (button press)
  ----------------------------------------------------------------------*/
static gboolean scr_popup_button_press (GtkWidget *widget, 
					GdkEventButton *event, 
					gpointer data)
{ 
  return CallPopGTK (widget, data);
}

/*----------------------------------------------------------------------
  Callback for a scrolled window (keypress)
  ----------------------------------------------------------------------*/
static gboolean scr_popup_key_press (GtkWidget *widget, GdkEventKey *event, gpointer data)
{
  if (event->keyval == GDK_Escape) 
    {
      formKillGTK (widget, NULL, data);
      return TRUE;
    }
  else if (event->keyval == GDK_Return
	   || event->keyval == GDK_space)
    return (CallPopGTK (widget, data));
  else
    return FALSE;
}

gboolean listeventGTK (GtkWidget *w, GdkEvent *AllEvent, int data)
{
  GdkEventButton *event;
  int x,y;

  switch (AllEvent->type)
    {
    case GDK_BUTTON_RELEASE:
      {
	event = (GdkEventButton *)AllEvent;
	x = (int) event->x;
	y = (int) event->y;
	if (x < w->allocation.x || 
	      x > w->allocation.x + w->allocation.width ||
	      y > w->allocation.y + w->allocation.height ||
	      y < w->allocation.y)
	  {
	    formKillGTK (w, NULL, (gpointer) data);
	  }
      }
      return FALSE;
    case GDK_LEAVE_NOTIFY:
      /*We could hide the widget there ?*/
      return FALSE;

    default:
      break;
    }
  return FALSE;
}

#if 0
/**
   LIST ALL GTK EVENT ON A WIDGET
 */
gboolean listeventGTK (GtkWidget *w, GdkEvent *AllEvent, int data)
{

  g_print ("\nevent!\t");
  
  switch (AllEvent->type)
    {
    case GDK_CLIENT_EVENT:
      g_print ("expose");
      return FALSE;

    case GDK_EXPOSE:
      g_print ("expose");
      return FALSE;

    case GDK_PROPERTY_NOTIFY:
      g_print ("property notify");
      return FALSE;

    case GDK_BUTTON_PRESS:
      g_print ("button press");
      return FALSE;

    case GDK_BUTTON_RELEASE:
      g_print ("button release");
      return FALSE;

    case GDK_MOTION_NOTIFY:
      g_print ("Motion");
      return FALSE;

    case GDK_FOCUS_CHANGE:
      g_print ("focus");
      return FALSE;

    case GDK_LEAVE_NOTIFY:
      g_print ("leave notify");
      return FALSE;

    case GDK_ENTER_NOTIFY:
      g_print ("enter notify");
      return FALSE;

    case GDK_KEY_PRESS:
      g_print ("keypress");
      return FALSE;
    
    default:
      break;
    }
  return FALSE;
}
#endif /*0*/
#endif /* _GTK */

/*----------------------------------------------------------------------
  ReturnTogglevalues returns switched entries.
  ----------------------------------------------------------------------*/
static void ReturnTogglevalues (struct Cat_Context *catalogue)
{
  register int        i;
  register int        index;
  int                 ent;
  int                 max;
  struct E_List      *adbloc;

  /* Enregistre la selection d'un toggle button */
  if (catalogue->Cat_Widget != 0)
    {
      adbloc = catalogue->Cat_Entries;
      index = 0;
      ent = 0;
      max = catalogue->Cat_Data;	/* nombre d'entrees definies */
      i = 2;			/* decalage de 2 pour le widget titre */
      while (adbloc != NULL)
	{
	  while (i < C_NUMBER && ent < max)
	    {
	      /*** Retour les entrees selectionnees vers l'application ***/
	      if (adbloc->E_Free[i] == 'Y')
		{
		  (*CallbackDialogue) (catalogue->Cat_Ref, INTEGER_DATA, index);
		  adbloc->E_Free[i] = 'N';
		}
	      i++;
	      ent++;
	      index++;
	    }
	  /* next block */
	  adbloc = adbloc->E_Next;
	  i = 0;
	}
    }
}

/*----------------------------------------------------------------------
  Callback inits a form.
  ----------------------------------------------------------------------*/
static void INITform (ThotWidget w, struct Cat_Context *parentCatalogue, caddr_t call_d)
{
  int                 ent;
  struct E_List      *adbloc;
#ifndef _GTK
  Arg                 args[MAX_ARGS];
  register int        n;
#endif /* _GTK */
  struct Cat_Context *catalogue;

  /* Affiche le formulaire */
  if (parentCatalogue->Cat_Widget != 0)
    {
      /*** Allume les sous-widgets du formulaire ***/
      adbloc = parentCatalogue->Cat_Entries;
      /* Le premier bloc contient les boutons de la feuille de saisie */
      adbloc = adbloc->E_Next;
      ent = 1;
      while (adbloc->E_ThotWidget[ent] != 0)
	{
	  /* Il faut sauter les widgets des RowColumns */
	  if (adbloc->E_Free[ent] == 'N')
	    {
	      catalogue = (struct Cat_Context *) adbloc->E_ThotWidget[ent];
#ifndef _GTK
	      if (catalogue->Cat_Widget != 0)
		XtManageChild (catalogue->Cat_Widget);
#endif /* _GTK */
	    }
	  /* Faut-il passer au bloc suivant ? */
	  ent++;
	  if (ent >= C_NUMBER)
	    {
	      ent = 0;
	      if (adbloc->E_Next == NULL)
		break;
	      else
		adbloc = adbloc->E_Next;
	    }
	}
      w = parentCatalogue->Cat_Widget;
#ifndef _GTK
      /*** Positionne le formulaire a la position courante du show ***/
      n = 0;
      XtSetArg (args[n], XmNx, (Position) ShowX);
      n++;
      XtSetArg (args[n], XmNy, (Position) ShowY);
      n++;
      XtSetValues (XtParent (w), args, n);
      XtManageChild (w);
      XtManageChild (XtParent (w));
      if (PopShell != 0)
	{
	  XtSetValues (PopShell, args, n);
	  XtPopup (PopShell, XtGrabNonexclusive);
	}
#else /* _GTK */
      if (PopShell != 0)
	{
	  gtk_window_set_position (GTK_WINDOW (PopShell), GTK_WIN_POS_MOUSE);
	  gtk_widget_show_all (PopShell);
	}
      else
	{
	  gtk_window_set_position (GTK_WINDOW (w), GTK_WIN_POS_MOUSE);
	  gtk_widget_show_all (GTK_WIDGET(w));
	}
#endif /* _GTK */
    }
}

/*----------------------------------------------------------------------
 New input value.
  ----------------------------------------------------------------------*/
static void CallValueSet (ThotWidget w, struct Cat_Context *catalogue, caddr_t call_d)
{
  int               val, val1;
  char              text[11];
  ThotWidget        wtext;

  /* Indication de valeur */
  if (catalogue->Cat_Widget != 0 && catalogue->Cat_Type == CAT_INT)
    {
      catalogue->Cat_Data = 0;
      wtext = catalogue->Cat_Entries->E_ThotWidget[1];
#ifndef _GTK	
      strncpy (text, XmTextGetString (wtext), 10);
#else /* _GTK */
      strncpy (text, gtk_entry_get_text (GTK_ENTRY (wtext)), 10);
#endif /* _GTK */
      text[10] = EOS;
      if (text[0] != EOS)
	{
	  if ((text[0] == '-') && (text[1] == EOS))
	    /* cas ou le caractere - a ete tape, on met val a 0 */
	    val = 0;
	  else
	    sscanf (text, "%d", &val);
	  /* Est-ce une valeur valide ? */
	  if (val < (int) catalogue->Cat_Entries->E_ThotWidget[2])
	    val1 = (int) catalogue->Cat_Entries->E_ThotWidget[2];
	  else if (val > (int) catalogue->Cat_Entries->E_ThotWidget[3])
	    val1 = (int) catalogue->Cat_Entries->E_ThotWidget[3];
	  else
	    val1 = val;	/* valeur inchangee */
	  
	  /* Est-ce qu'il faut changer le contenu du widget ? */
	  if (val != val1)
	    {
	      sprintf (text, "%d", val1);
#ifndef _GTK
	      /* Desactive la procedure de Callback */
	      if (catalogue->Cat_React)
		XtRemoveCallback (wtext, XmNvalueChangedCallback,
				  (XtCallbackProc) CallValueSet, catalogue);
	      XmTextSetString (wtext, text);
	      val = strlen (text);
	      XmTextSetSelection (wtext, val, val, 500);
	      /* Reactive la procedure de Callback */
	      if (catalogue->Cat_React)
		XtAddCallback (wtext, XmNvalueChangedCallback,
			       (XtCallbackProc) CallValueSet, catalogue);
#else /* _GTK */
	      /* Desactive la procedure de Callback */
	      if (catalogue->Cat_React)
		RemoveSignalGTK (GTK_OBJECT(wtext), "changed"); 
	      gtk_entry_set_text (GTK_ENTRY (wtext), text);
	      val = strlen (text);
	      /* Reactive la procedure de Callback */
	      if (catalogue->Cat_React)
		ConnectSignalGTK (GTK_OBJECT(wtext), "changed",
				  GTK_SIGNAL_FUNC(CallValueSet), (gpointer)catalogue);
#endif /* _GTK */
	    }
	  /* retourne la valeur saisie si la feuille de saisie est reactive */
	  if (catalogue->Cat_React)
	    (*CallbackDialogue) (catalogue->Cat_Ref, INTEGER_DATA, val);
	}
    }
}

/*----------------------------------------------------------------------
  ReturnSheet handles a sheet callback.          
  ----------------------------------------------------------------------*/
static void ReturnSheet (struct Cat_Context *parentCatalogue, int entry,
			 struct E_List *adbloc)
{
#ifndef _GTK
  register int        n;
  Arg                 args[MAX_ARGS];
  XmStringTable       strings;
  ThotWidget          wtext;
#else /* _GTK */
  ThotWidget          tmpw;
  gchar              *wtext;
#endif /* _GTK */
  struct Cat_Context *catalogue;
  char                text[100];
  char               *ptr;
  int                 i;
  int                 ent;

  /*** Retour vers l'application ***/
  /*** Eteins les sous-widgets du feuillet si on quitte ***/
  /*** Recupere les retours des sous-catalogues         ***/
  adbloc = adbloc->E_Next;
  ent = 1;
  while (adbloc->E_ThotWidget[ent] != 0)
    {
      /* Il faut sauter les widgets des RowColumns */
      if (adbloc->E_Free[ent] == 'N')
	{
	  catalogue = (struct Cat_Context *) adbloc->E_ThotWidget[ent];
	  if (catalogue->Cat_Widget != 0)
	    {
	      if (entry == 0)
		{
#ifndef _GTK		      
		  XtUnmanageChild (catalogue->Cat_Widget);
#else /* _GTK */		      
		  gtk_widget_hide (GTK_WIDGET(catalogue->Cat_Widget));
#endif /* _GTK */
		}
	      /* Sinon il faut retourner la valeur du sous-catalogue */
	      else
		{
		  if (catalogue->Cat_React)
		    ; /* value already returned */
		  else if (catalogue->Cat_Type == CAT_FMENU)
		    {
		      i = catalogue->Cat_Data; /* a sub-menu */
		      (*CallbackDialogue) (catalogue->Cat_Ref, INTEGER_DATA, i);
		    }
		  else if (catalogue->Cat_Type == CAT_TMENU)
		    ReturnTogglevalues (catalogue); /* a toggle */
		  else if (catalogue->Cat_Type == CAT_INT) /* a number */
		    {
		      CallValueSet (catalogue->Cat_Entries->E_ThotWidget[1], catalogue, NULL);
#ifndef _GTK			  
		      strncpy (text, XmTextGetString (catalogue->Cat_Entries->E_ThotWidget[1]), 10);
#else /* _GTK */
		      strncpy (text, gtk_entry_get_text(GTK_ENTRY(catalogue->Cat_Entries->E_ThotWidget[1])), 10);
#endif /* _GTK */
		      text[10] = EOS;
		      if (text[0] != EOS)
			sscanf (text, "%d", &i);
		      else
			i = 0;
		      (*CallbackDialogue) (catalogue->Cat_Ref, INTEGER_DATA, i);
		    }
		  else if (catalogue->Cat_Type == CAT_TEXT)
		    {
#ifndef _GTK
		      (*CallbackDialogue) (catalogue->Cat_Ref, STRING_DATA,
					   XmTextGetString ((ThotWidget) catalogue->Cat_Entries));
#else /* _GTK */
		      (*CallbackDialogue) (catalogue->Cat_Ref, STRING_DATA, gtk_entry_get_text(GTK_ENTRY(catalogue->Cat_Entries)));
#endif /* _GTK */
		    }
		  else if (catalogue->Cat_Type == CAT_SELECT)
		    {
		      if (catalogue->Cat_SelectList)
			{
#ifndef _GTK
			  text[0] = EOS;
			  n = 0;
			  XtSetArg (args[n], XmNselectedItems, &strings);
			  n++;
			  XtGetValues ((ThotWidget) catalogue->Cat_Entries, args, n);
			  ptr = text;
			  if (strings)
			    XmStringGetLtoR (strings[0], XmSTRING_DEFAULT_CHARSET, &ptr);
			  (*CallbackDialogue) (catalogue->Cat_Ref, STRING_DATA, ptr);
			  if (strings)
			    TtaFreeMemory (ptr);
#else /* _GTK */
			  tmpw = GTK_WIDGET(catalogue->Cat_Entries);
			  if(GTK_LIST(tmpw)->selection)
			    {
			      gtk_label_get(GTK_LABEL(gtk_object_get_data(GTK_OBJECT(GTK_LIST(tmpw)->selection->data), "ListElementLabel")),&ptr);
			      (*CallbackDialogue) (catalogue->Cat_Ref, STRING_DATA, ptr);
			    }
#endif /* _GTK */
			}
		      else
			{
#ifndef _GTK
			  wtext = XmSelectionBoxGetChild ((ThotWidget) catalogue->Cat_Entries, XmDIALOG_TEXT);
			  /* Retourne la valeur dans tous les cas */
			  (*CallbackDialogue) (catalogue->Cat_Ref, STRING_DATA, XmTextGetString (wtext));
#else /* _GTK */
			  tmpw = GTK_WIDGET(catalogue->Cat_Entries);
			  tmpw = GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (tmpw), "EntryZone"));
			  wtext = gtk_entry_get_text (GTK_ENTRY (tmpw));
			  (*CallbackDialogue) (catalogue->Cat_Ref, STRING_DATA, wtext);
#endif /* _GTK */
			}
		    }
		}
	    }
	}
      ent++;
      if (ent >= C_NUMBER)
	{
	  /* next block */
	  ent = 0;
	  if (adbloc->E_Next == NULL)
	    break;
	  else
	    adbloc = adbloc->E_Next;
	}
    }
  /*** On fait disparaitre le formulaire ***/
  if (entry == 0 || parentCatalogue->Cat_Type == CAT_DIALOG || parentCatalogue->Cat_Type == CAT_FORM)
    {
#ifndef _GTK
      XtUnmanageChild (parentCatalogue->Cat_Widget);
      XtUnmanageChild (XtParent (parentCatalogue->Cat_Widget));
#else /* _GTK */
      gtk_widget_hide (parentCatalogue->Cat_Widget);
#endif /* _GTK */ 
      /* Si on en a fini avec la feuille de dialogue */
      catalogue = parentCatalogue;
      while (catalogue->Cat_PtParent != NULL)
	catalogue = catalogue->Cat_PtParent;
      if (catalogue == ShowCat && ShowReturn == 1)
	ShowReturn = 0;
    }
  (*CallbackDialogue) (parentCatalogue->Cat_Ref,INTEGER_DATA, entry);
}

#ifdef _GTK
/*----------------------------------------------------------------------
   CallbackSheet.                                              
  ----------------------------------------------------------------------*/
static void CallEnter (ThotWidget w, struct Cat_Context *parentCatalogue,
		       caddr_t call_d)
{
  struct E_List      *adbloc;
  int                 i;
  int                 entry;

  /* On a selectionne une entree du menu */
  if (parentCatalogue->Cat_Widget != 0)
    {
      adbloc = parentCatalogue->Cat_Entries;
      entry = -1;
      i = 0;
      while (entry == -1 && i < C_NUMBER)
	{
	  if (adbloc->E_ThotWidget[i] == w)
	    entry = i;
	  i++;
	}
      /* Si la feuille de dialogue est detruite cela force l'abandon */
      if (entry == -1 && parentCatalogue->Cat_Type == CAT_SHEET)
	  entry = 0;
      ReturnSheet (parentCatalogue, 1, adbloc);
    }
}
#endif /* _GTK */

/*----------------------------------------------------------------------
   CallbackSheet: a button was clicked.                                              
  ----------------------------------------------------------------------*/
static ThotBool CallSheet (ThotWidget w, struct Cat_Context *parentCatalogue,
		       caddr_t call_d)
{
  struct E_List      *adbloc;
  int                 i;
  int                 entry;

  /* On a selectionne une entree du menu */
  if (parentCatalogue->Cat_Widget != 0)
    {
      adbloc = parentCatalogue->Cat_Entries;
      entry = -1;
      i = 0;
      while (entry == -1 && i < C_NUMBER)
	{
	  if (adbloc->E_ThotWidget[i] == w)
	    entry = i;
	  i++;
	}
      /* Si la feuille de dialogue est detruite cela force l'abandon */
      if (entry == -1 && parentCatalogue->Cat_Type == CAT_SHEET)
	  entry = 0;
      if (entry != -1)
	ReturnSheet (parentCatalogue, entry, adbloc);
    }
  return TRUE;  
}


/*----------------------------------------------------------------------
   Callback de selection dans une liste.                              
  ----------------------------------------------------------------------*/
#ifndef _GTK
static void CallList (ThotWidget w, struct Cat_Context *catalogue, XmListCallbackStruct * infos)
#else /* _GTK */
static ThotBool CallListGTK (ThotWidget w, struct Cat_Context *catalogue)
#endif /* _GTK */
{
#ifndef _GTK
   char              *text = NULL;
   ThotBool           ok;

   if (catalogue->Cat_Widget != 0 && catalogue->Cat_Type == CAT_SELECT)
     {
       ok = XmStringGetLtoR (infos->item, XmSTRING_DEFAULT_CHARSET, &text);
       /* retourne l'entree choisie */
       if (ok && text != NULL)
	 if (text[0] != EOS && text[0] != SPACE)
	   (*CallbackDialogue) (catalogue->Cat_Ref, STRING_DATA, text);
       TtaFreeMemory (text);
     }
#else /* _GTK */
   gchar              *text = NULL;
   ThotWidget         tmpw;

   if (catalogue->Cat_Widget != 0 && catalogue->Cat_Type == CAT_SELECT)
     {
       /* when you select an element in a selector box */
       tmpw = GTK_WIDGET(catalogue->Cat_Entries);
       if (catalogue->Cat_SelectList)
	 {
	   /* just a simple list */
	   if(GTK_LIST(tmpw)->selection)
	     {
	       gtk_label_get(GTK_LABEL(gtk_object_get_data (GTK_OBJECT (GTK_LIST(tmpw)->selection->data), "ListElementLabel")),&text);
	       (*CallbackDialogue) (catalogue->Cat_Ref, STRING_DATA, text);
	     }
	 }
       else
	 {
	   /* a list and a text zone */
	   /* just get the selected text and assigne it to the text zone data */
	   if(GTK_LIST(tmpw)->selection)
	     {
	       gtk_label_get(GTK_LABEL(gtk_object_get_data(GTK_OBJECT(GTK_LIST(tmpw)->selection->data), "ListElementLabel")), &text);
	       tmpw = GTK_WIDGET(gtk_object_get_data (GTK_OBJECT (tmpw), "EntryZone"));
	       gtk_entry_set_text (GTK_ENTRY (tmpw), text);
	     }	      
	 }
     }
   return TRUE;   
#endif /* _GTK */
}

#ifdef _GTK
/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
gboolean DeletePopShell (ThotWidget w, GdkEventButton *bu, gpointer data)
{
  PopShell = 0;
  return FALSE;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
gboolean CallTextEnterGTK (ThotWidget w, GdkEventButton *bu, gpointer data)
{  
  struct Cat_Context *catalogue;
  int                 i;
  
  catalogue = (struct Cat_Context *) data;
  if (bu->button == 1) 
    {
      if (bu->type == GDK_2BUTTON_PRESS) 
	{ 
	  while (catalogue->Cat_PtParent != NULL)
	    catalogue = catalogue->Cat_PtParent;
	  i = catalogue->Cat_Default;
	  (*CallbackDialogue) (catalogue->Cat_Ref, INTEGER_DATA, (char *)i);
	  return TRUE;
	}
    }
  return FALSE;
}
#endif /* _GTK */

/*----------------------------------------------------------------------
  Callback de saisie de texte.
  ----------------------------------------------------------------------*/
#ifndef _GTK
static void CallTextChange (ThotWidget w, struct Cat_Context *catalogue, caddr_t call_d)
#else /* _GTK */
static ThotBool CallTextChangeGTK (ThotWidget w, struct Cat_Context *catalogue)
#endif /* _GTK */
{
#ifndef _GTK
   ThotWidget         wtext;
#endif /* _GTK */
   char              *text = NULL;
    
   if (catalogue->Cat_Widget != 0)
     {
      if (catalogue->Cat_Type == CAT_TEXT)
	{
#ifndef _GTK
	  /* retourne la valeur saisie si la feuille de saisie est reactive */
	  (*CallbackDialogue) (catalogue->Cat_Ref, STRING_DATA,
			       XmTextGetString ((ThotWidget) catalogue->Cat_Entries));
#else /* _GTK */
	  (*CallbackDialogue) (catalogue->Cat_Ref, STRING_DATA,
			       gtk_entry_get_text (GTK_ENTRY (catalogue->Cat_Entries)));
#endif /* _GTK */
	}
      else if (catalogue->Cat_Type == CAT_SELECT)
	{
#ifndef _GTK
	  wtext = XmSelectionBoxGetChild ((ThotWidget) catalogue->Cat_Entries,
					  XmDIALOG_TEXT);
	  /* retourne la valeur saisie si la feuille de saisie est reactive */
	  text = XmTextGetString (wtext);
	  (*CallbackDialogue) (catalogue->Cat_Ref, STRING_DATA, text);
	  TtaFreeMemory (text);
#else /* _GTK */
	  text = gtk_entry_get_text (GTK_ENTRY (w));
	  (*CallbackDialogue) (catalogue->Cat_Ref, STRING_DATA, text);
#endif /* _GTK */
	}
     }
   return TRUE;
}


/*----------------------------------------------------------------------
  Callback pour un bouton du label de selecteur
  ----------------------------------------------------------------------*/
static ThotBool CallLabel (ThotWidget w, struct Cat_Context *catalogue, caddr_t call_d)
{
#ifndef _GTK
  Arg                 args[MAX_ARGS];
  XmString            text;
  char               *str = NULL;

  if (catalogue->Cat_Widget != 0)
    {
      /* Recupere le texte du label */
      XtSetArg (args[0], XmNlabelString, &text);
      XtGetValues (w, args, 1);
      /* Initialise le champ texte */
      XtSetArg (args[0], XmNtextString, text);
      if (catalogue->Cat_SelectList)
	{
	  /* retourne la valeur du label */
	  XmStringGetLtoR (text, XmSTRING_DEFAULT_CHARSET, &str);
	  (*CallbackDialogue) (catalogue->Cat_Ref, STRING_DATA, str);
	  TtaFreeMemory (str);
	}
      else
	XtSetValues ((ThotWidget) catalogue->Cat_Entries, args, 1);
     }
#else /* _GTK */
   gchar *str;

   if (catalogue->Cat_Widget != 0)
     {
       gtk_label_get(GTK_LABEL(gtk_object_get_data (GTK_OBJECT(w),"ButtonLabel")),&str);
       (*CallbackDialogue) (catalogue->Cat_Ref, STRING_DATA, str);
     }
   return TRUE;
#endif /* _GTK */
}

/*----------------------------------------------------------------------
  warning handler                                                    
  ----------------------------------------------------------------------*/
void MyWarningHandler ()
{
}

#ifndef _GTK
/*----------------------------------------------------------------------
  Procedure which controls Motif dialogue colors
  ----------------------------------------------------------------------*/
void ThotXmColorProc (ThotColorStruct *bg, ThotColorStruct *fg,
		      ThotColorStruct *sel, ThotColorStruct *top,
		      ThotColorStruct *bottom)
{
   top->red = RGB_Table[3].red *256;
   top->green = RGB_Table[3].green *256;
   top->blue = RGB_Table[3].blue *256;
   bottom->red = RGB_Table[6].red *256;
   bottom->green = RGB_Table[6].green *256;
   bottom->blue = RGB_Table[6].blue *256;
   fg->red = RGB_Table[7].red *256;
   fg->green = RGB_Table[7].green *256;
   fg->blue = RGB_Table[7].blue *256;
   sel->red = RGB_Table[5].red *256;
   sel->green = RGB_Table[5].green *256;
   sel->blue = RGB_Table[5].blue *256;
}
#endif /* _GTK */
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
   TtaInitDialogue
   Parameters:
   server: nom du serveur X.
   X-Specific stuff :
   app_context: contient au retour l'identification du contexte d'application.
   display:  contient au retour l'identification de l'e'cran.
  ----------------------------------------------------------------------*/
void TtaInitDialogue (char *server, ThotAppContext *app_context)
{
#if !defined(_WINDOWS) && !defined(_GTK)
   int                 n;
   char               *arg;
#endif /* _WINDOWS && GTK */
#ifdef _GTK
   /* char   *ptr; */

   /* Sets the current locale according to the program environment */
   /* ptr = TtaGetEnvString ("ENABLE_MULTIKEY");
      if (ptr && !strcasecmp (ptr, "yes"))*/
   gtk_set_locale ();
   /* initialize everything needed to operate the toolkit and parses some standard command line options */
   if (!gtk_init_check (&appArgc, &appArgv))
     printf ("GUI can't be initialized\n");
   /* initilize the imlib, gtkv2.0 dont use imlib , it uses gdkpixbuf */
   /* _GTK2 gdk_rgb_init(); */
   gdk_imlib_init();
#endif /* _GTK */
   CurrentWait = 0;
   ShowReturn = 0;
   ShowX = 100;
   ShowY = 100;
   /* Initialisation des catalogues */
   PtrCatalogue = NewCatList ();
   NbOccE_List = 0;
   NbLibE_List = 0;
   PtrFreeE_List = NULL;
   /* Initialisation des couleurs et des translations */
   MainShell = 0;
   PopShell = 0;
   /* Pas encore de reference attribuee */
   FirstFreeRef = 0;
#ifdef _WINDOWS
   FrMainRef[0] = 0;
   iconID = "IDI_APPICON";
   /*Window main class */
   RootShell.style = 0;
   RootShell.lpfnWndProc = WndProc;
   RootShell.cbClsExtra = 0;
   RootShell.cbWndExtra = 0;
   RootShell.hInstance = hInstance;
   RootShell.hIcon = LoadIcon (hInstance, iconID);
   RootShell.hCursor = LoadCursor (NULL, IDC_ARROW);
   RootShell.hbrBackground = (HBRUSH) GetStockObject (LTGRAY_BRUSH);
   RootShell.lpszMenuName = "AmayaMain";
   RootShell.lpszClassName = "Amaya";
   RootShell.cbSize = sizeof(WNDCLASSEX);
   RootShell.hIconSm = LoadIcon (hInstance, iconID);
   RegisterClassEx (&RootShell);

   /*Window canvas  class */
   RootShell.style = 
	   /* Handle dblclks*/
	   CS_DBLCLKS |
	   /* Force entire window redraw on H or V resize */
	   /* CS_HREDRAW | CS_VREDRAW | */
	   /* Faster handling of device context*/
	   CS_OWNDC;
   RootShell.lpfnWndProc = ClientWndProc;
   RootShell.cbClsExtra = 0;
   RootShell.cbWndExtra = 0;
   RootShell.hInstance = hInstance;
   RootShell.hIcon = LoadIcon (hInstance, iconID);
   RootShell.hCursor = LoadCursor (NULL, IDC_ARROW);
#ifndef _GL
   RootShell.hbrBackground = (HBRUSH) GetStockObject (LTGRAY_BRUSH);
#else /*_GL*/
   RootShell.hbrBackground = 0;
#endif /*_GL*/
   RootShell.lpszClassName = "ClientWndProc";
   RootShell.lpszMenuName = NULL;
   RootShell.cbSize = sizeof(WNDCLASSEX);
   RootShell.hIconSm = LoadIcon (hInstance, iconID);
   RegisterClassEx (&RootShell);

   /* New windows class : dialog*/
   RootShell.style = 0;
   RootShell.lpfnWndProc = ThotDlgProc;
   RootShell.cbClsExtra = 0;
   RootShell.cbWndExtra = 0;
   RootShell.hInstance = hInstance;
   RootShell.hIcon = LoadIcon (hInstance, iconID);
   RootShell.hCursor = LoadCursor (NULL, IDC_ARROW);
   RootShell.lpszClassName = "WNDIALOGBOX";
   RootShell.lpszMenuName = NULL;
   RootShell.hbrBackground = (HBRUSH) GetStockObject (LTGRAY_BRUSH);
   RootShell.cbSize = sizeof(WNDCLASSEX);
   RootShell.hIconSm = LoadIcon (hInstance, iconID);
   RegisterClassEx (&RootShell);
#else /* _WINDOWS */
#ifndef _GTK
   /* Ouverture de l'application pour le serveur X-ThotWindow */
   RootShell = 0;
   XtToolkitInitialize ();
   n = 0;
   arg = NULL;
   Def_AppCont = XtCreateApplicationContext ();
   GDp = XtOpenDisplay (Def_AppCont, server, "appli", "Dialogue", NULL, 0, &n, &arg);
   if (!GDp)
     /* No server connection */
     return;
   *app_context = Def_AppCont;
   TtDisplay = GDp;
   RootShell = XtAppCreateShell ("", "Dialogue", applicationShellWidgetClass, GDp, NULL, 0);
   /* 28/Nov/2000: Contribution by Johaness Zellner for enabling tear-off menus */
   XmRepTypeInstallTearOffModelConverter ();
   /* redirect handler warnings */
   XtAppSetWarningHandler (*app_context, MyWarningHandler);
   /* Initialisation des options de dialogue */
   DefaultFont = XmFontListCreate (XLoadQueryFont (GDp, "fixed"), XmSTRING_DEFAULT_CHARSET);
   formFONT = XmFontListCreate (XLoadQueryFont (GDp, "fixed"), XmSTRING_DEFAULT_CHARSET);
   XmSetColorCalculation ((XmColorProc) ThotXmColorProc);
#else /* _GTK */
   DefaultFont = gdk_font_load ("fixed");
   formFONT = gdk_font_load ("fixed");
   TextTranslations = NULL;
   TtDisplay = GDK_DISPLAY ();
#endif /* _GTK */
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
   TtaInitDialogueTranslations initialise les translations du         
   dialogue. Ce sont tous les racoursis claviers.                     
  ----------------------------------------------------------------------*/
void TtaInitDialogueTranslations (ThotTranslations translations)
{
   TextTranslations = translations;
}

/*----------------------------------------------------------------------
   TtaChangeDialogueFonts change les polices de caracteres du dialogue.
  ----------------------------------------------------------------------*/
void TtaChangeDialogueFonts (char *menufont, char *formfont)
{ 
#ifndef _WINDOWS
#ifndef _GTK
  if (menufont)
    {
      XmFontListFree (DefaultFont);
      DefaultFont = XmFontListCreate (XLoadQueryFont (GDp, menufont), XmSTRING_DEFAULT_CHARSET);
    }
  if (formfont)
    {
      XmFontListFree (formFONT);
      formFONT = XmFontListCreate (XLoadQueryFont (GDp, formfont), XmSTRING_DEFAULT_CHARSET);
    }
#else /* _GTK */
  if (menufont != NULL)
    DefaultFont = gdk_font_load(menufont);
  if (formfont != NULL)
    formFONT = gdk_font_load(formfont);
#endif /* _GTK */
#endif /* _WINDOWS */
}


/*----------------------------------------------------------------------
   TtaGetReferencesBase re'serve number re'fe'rences pour
   l'application a` partir de la base courante.
   La fonction retourne la base courante.
  ----------------------------------------------------------------------*/
int TtaGetReferencesBase (int number)
{
   int                 base;

   base = FirstFreeRef;
   if (number > 0)
      FirstFreeRef += number;
   return (base);
}

/*----------------------------------------------------------------------
  DisplayConfirmMessage displays the given message (text).
  ----------------------------------------------------------------------*/
void DisplayConfirmMessage (char *text)
{
#ifndef _WINDOWS
#ifndef _GTK
   XmString            title_string, OK_string;
   Arg                 args[MAX_ARGS];
   int                 n;
#endif /* _GTK */
   ThotWidget          row, w;
   ThotWidget          msgbox;

   /* get current position */
   TtaSetDialoguePosition ();
#ifdef _GTK
   /* Create the window message */
   msgbox = gtk_window_new (GTK_WINDOW_TOPLEVEL);
   gtk_widget_realize (GTK_WIDGET(msgbox));
   msgbox->style->font = DefaultFont;
   gtk_window_set_title (GTK_WINDOW (msgbox), TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
   gtk_window_set_policy (GTK_WINDOW (msgbox), TRUE, TRUE, FALSE);
   gtk_widget_set_uposition(GTK_WIDGET(msgbox), ShowX, ShowY);
   gtk_container_set_border_width (GTK_CONTAINER(msgbox), 2);
    /*** Create a Row-Column to add the label and OK button ***/
   row = gtk_vbox_new (FALSE,0);
   gtk_widget_show (GTK_WIDGET(row));
   gtk_container_add (GTK_CONTAINER (msgbox), row);
   /* the label */
   w = gtk_label_new (text);
   gtk_misc_set_alignment (GTK_MISC (w), 0., 0.5);
   gtk_misc_set_padding (GTK_MISC (w), 10., 0.);
   gtk_widget_show (GTK_WIDGET(w));
   gtk_label_set_justify (GTK_LABEL (w), GTK_JUSTIFY_CENTER);
   gtk_box_pack_start (GTK_BOX (row), w, FALSE, FALSE, 0);
   /*** Create the Row-Column that includes OK button ***/
   w = gtk_hbox_new (FALSE,0);
   gtk_widget_show (GTK_WIDGET(w));
   gtk_box_pack_start (GTK_BOX (row), w, FALSE, FALSE, 0);
   row=w;
   /*** Create the OK button ***/
   w = gtk_button_new_with_label (TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
   gtk_widget_show (GTK_WIDGET(w));
   gtk_box_pack_start (GTK_BOX (row), w, FALSE, FALSE, 100);
   ConnectSignalGTK (GTK_OBJECT(w), "clicked", GTK_SIGNAL_FUNC(ConfirmMessage), (gpointer)msgbox);
   gtk_widget_show_all (msgbox);
   gdk_window_raise (msgbox->window);
#else /* _GTK */
   /* C  reate the window message */
   title_string = XmStringCreateSimple (text);
   OK_string = XmStringCreateSimple (TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
   n = 0;
   XtSetArg (args[n], XmNx, (Position) ShowX);
   n++;
   XtSetArg (args[n], XmNy, (Position) ShowY);
   n++;
   XtSetArg (args[n], XmNallowShellResize, TRUE);
   n++;
   XtSetArg (args[n], XmNuseAsyncGeometry, TRUE);
   n++;
   msgbox = XtCreatePopupShell (TtaGetMessage (LIB, TMSG_LIB_CONFIRM), applicationShellWidgetClass,
				RootShell, args, n);
   n = 0;
   XtSetArg (args[n], XmNbackground, BgMenu_Color);
   n++;
   XtSetArg (args[n], XmNfontList, DefaultFont);
   n++;
   XtSetArg (args[n], XmNdialogTitle, OK_string);
   n++;
   XtSetArg (args[n], XmNautoUnmanage, FALSE);
   n++;
   XtSetArg (args[n], XmNmarginWidth, 10);
   n++;
   XtSetArg (args[n], XmNmarginHeight, 0);
   n++;
   XtSetArg (args[n], XmNspacing, 0);
   n++;
   w = XmCreateBulletinBoard (msgbox, "Dialogue", args, n);
   XtManageChild (w);
   /*** Create a Row-Column to add the label and OK button ***/
   n = 0;
   XtSetArg (args[n], XmNbackground, BgMenu_Color);
   n++;
   /*XtSetArg (args[n], XmNadjustLast, FALSE);
     n++;*/
   XtSetArg (args[n], XmNmarginWidth, 0);
   n++;
   XtSetArg (args[n], XmNmarginHeight, 0);
   n++;
   XtSetArg (args[n], XmNspacing, 0);
   n++;
   /*XtSetArg (args[n], XmNpacking, XmPACK_TIGHT);
     n++;*/
   XtSetArg (args[n], XmNorientation, XmVERTICAL);
   n++;
   XtSetArg (args[n], XmNresizeHeight, TRUE);
   n++;
   row = XmCreateRowColumn (w, "Dialogue", args, n);
   XtManageChild (row);
   /* the label */
   n = 0;
   XtSetArg (args[n], XmNalignment, XmALIGNMENT_CENTER);
   n++;
   XtSetArg (args[n], XmNfontList, DefaultFont);
   n++;
   XtSetArg (args[n], XmNbackground, BgMenu_Color);
   n++;
   XtSetArg (args[n], XmNforeground, FgMenu_Color);
   n++;
   XtSetArg (args[n], XmNlabelString, title_string);
   n++;
   w = XmCreateLabel (row, "Thot_MSG", args, n);
   XtManageChild (w);
   /*** Create the Row-Column that includes OK button ***/
   n = 0;
   XtSetArg (args[n], XmNfontList, DefaultFont);
   n++;
   XtSetArg (args[n], XmNbackground, BgMenu_Color);
   n++;
   XtSetArg (args[n], XmNorientation, XmHORIZONTAL);
   n++;
   XtSetArg (args[n], XmNmarginWidth, 100);
   n++;
   XtSetArg (args[n], XmNmarginHeight, 0);
   n++;
   row = XmCreateRowColumn (row, "Dialogue", args, n);
   XtManageChild (row);
   /*** Create the OK button ***/
   n = 0;
   XtSetArg (args[n], XmNbackground, BgMenu_Color);
   n++;
   XtSetArg (args[n], XmNforeground, FgMenu_Color);
   n++;
   XtSetArg (args[n], XmNbottomShadowColor, BgMenu_Color);
   n++;
   XtSetArg (args[n], XmNfontList, DefaultFont);
   n++;
   w = XmCreatePushButton (row, TtaGetMessage (LIB, TMSG_LIB_CONFIRM), args, n);
   XtManageChild (w);
   XtAddCallback (w, XmNactivateCallback, (XtCallbackProc) ConfirmMessage, msgbox);
   /* display the message */
   XtPopup (msgbox, XtGrabNonexclusive);
   XmStringFree (title_string);
   XmStringFree (OK_string);
#endif /* _GTK */
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
   DisplayMessage display the given messge (text) in main window   
   according to is msgType.		                
   - INFO : bellow the previous message.                   
   - OVERHEAD : instead of the previous message.           
  ----------------------------------------------------------------------*/
void DisplayMessage (char *text, int msgType)
{
#ifndef _WINDOWS
   int                 lg;
   int                 n;
   char                buff[500 + 1];
   char               *pointer;

   /* Is the initialisation done ? */
   lg = strlen (text);
   if (MainShell != 0 && WithMessages && lg > 0)
     {
	/* take current messages */
#ifndef _GTK
	strncpy (buff, XmTextGetString (FrameTable[0].WdStatus), 500);
#else /* _GTK */
	strncpy (buff, gtk_entry_get_text (GTK_ENTRY(FrameTable[0].WdStatus)), 500);
#endif /* _GTK */
	n = strlen (buff);
	if (msgType == INFO)
	  {
	     /* is it necessary to suppress one or more messages ? */
	     if (n + lg + 1 >= 500)
	       {
		  /* suppress messages */
		  /* kill until we have 50 free characters */
		  while (n + lg + 1 >= 450)
		    {
		       /* search next New Line */
		       pointer = strchr (buff, '\n');
		       if (pointer == NULL)
			  n = 0;
		       else
			 {
			    strcpy (buff, &pointer[1]);
			    n = strlen (buff);
			 }
		    }
		  /* add message */
		  if (n > 0)
		     strcpy (&buff[n++], "\n");
		  strncpy (&buff[n], text, 500 - n);
		  lg += n;
		  /* copy text */
#ifndef _GTK
		  XmTextSetString (FrameTable[0].WdStatus, buff);
#else /* _GTK */
#ifndef _GTK2
		  if (gtk_text_get_length (GTK_TEXT (FrameTable[0].WdStatus))>0)
		    gtk_editable_delete_text( GTK_EDITABLE (FrameTable[0].WdStatus), 0, -1);
		  gtk_text_insert (GTK_TEXT (FrameTable[0].WdStatus), NULL, NULL, NULL, buff, -1);
#endif /* _GTK2 */
#endif /* _GTK */
	       }
	     else
	       {
		  /* enough space, just add new message at the end */
		  strcpy (buff, "\n");
		  strcat (buff, text);
#ifndef _GTK	     
		  XmTextInsert (FrameTable[0].WdStatus, n, buff);
#else /* _GTK */
#ifndef _GTK2
		  gtk_text_insert (GTK_TEXT (FrameTable[0].WdStatus), NULL, NULL, NULL, buff, -1);
#endif /* _GTK2 */
#endif /* _GTK */
		  lg += n;
	       }
	     /* select the message end to force scroll down */
#ifndef _GTK
	     XmTextSetSelection (FrameTable[0].WdStatus, lg, lg, 500);
#else
	     gtk_editable_select_region(GTK_EDITABLE(FrameTable[0].WdStatus), 0, -1);
#endif
	  }
	else if (msgType == OVERHEAD)
	  {
	     /* search last New Line */
	     while (buff[n] != EOL && n >= 0)
		n--;
	     /* replace last message by the new one */
#ifndef _GTK
	     XmTextReplace (FrameTable[0].WdStatus, n + 1, strlen (buff), text);
#else /* _GTK */
#ifndef _GTK2
	     if (gtk_text_get_length (GTK_TEXT (FrameTable[0].WdStatus))>0)
	       gtk_editable_delete_text( GTK_EDITABLE (FrameTable[0].WdStatus), 0, -1);
	     gtk_text_insert (GTK_TEXT (FrameTable[0].WdStatus), NULL, NULL, NULL, text, -1);
#endif /* _GTK2 */
#endif /* _GTK */
	  }
#ifndef _GTK
	XFlush (GDp);
#else /* _GTK */
#endif /* _GTK */
     }
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
   DefineCallbackDialog de'finit la proce'dure de traitement des      
   retoursde catalogues dans l'application.                           
  ----------------------------------------------------------------------*/
void TtaDefineDialogueCallback (void (*procedure) ())
{
   CallbackDialogue = procedure;
}


/*----------------------------------------------------------------------
   ClearChildren nettoie tous les catalalogues fils du catalogue.     
  ----------------------------------------------------------------------*/
static void ClearChildren (struct Cat_Context *parentCatalogue)
{
   register int        icat;
   struct Cat_Context *catalogue;
   struct Cat_List    *adlist;

   /* Tous les sous-menus ou sous-formulaires sont detruits par MOTIF */
   /* Recherche les catalogues qui sont les fils de parentCatalogue */
   adlist = PtrCatalogue;
   while (adlist != NULL)
     {
	icat = 0;
	while (icat < MAX_CAT)
	  {
	     catalogue = &adlist->Cat_Table[icat];
	     if ((catalogue->Cat_Widget != 0)
		 && (catalogue->Cat_PtParent == parentCatalogue))
	       {
		  /* Libere les blocs des entrees */
		  if ((catalogue->Cat_Type != CAT_TEXT)
		      && (catalogue->Cat_Type != CAT_SELECT)
		      && (catalogue->Cat_Type != CAT_LABEL))
		    {
		       FreeEList (catalogue->Cat_Entries);
		       catalogue->Cat_Entries = NULL;
		    }
		  /* Libere le catalogue */
		  catalogue->Cat_Widget = 0;
		  if ((catalogue->Cat_Type == CAT_POPUP)
		      || (catalogue->Cat_Type == CAT_SCRPOPUP)
		      || (catalogue->Cat_Type == CAT_PULL)
		      || (catalogue->Cat_Type == CAT_MENU)
		      || (catalogue->Cat_Type == CAT_FORM)
		      || (catalogue->Cat_Type == CAT_SHEET)
		      || (catalogue->Cat_Type == CAT_DIALOG))
		     ClearChildren (catalogue);
	       }
	     icat++;
	  }
	/* On passe au bloc suivant */
	adlist = adlist->Cat_Next;
     }
}


/*----------------------------------------------------------------------
   DestContenuMenu de'truit les entre'es du catalogue de'signe' par   
   son index.                                                         
   Retourne un code d'erreur.                                         
  ----------------------------------------------------------------------*/
static int DestContenuMenu (struct Cat_Context *catalogue)
{
   register int        ent;
   struct E_List      *adbloc;
   ThotWidget          w;

   if (catalogue == NULL)
      return (1);
   else if (catalogue->Cat_Widget == 0)
      return (1);
   else
     {
	if (catalogue->Cat_Type == CAT_LABEL)
	  {
	     /* Recupere le widget parent du widget detruit */
#ifndef _WINDOWS
#ifndef _GTK
	     w = XtParent (catalogue->Cat_Widget);
	     XtDestroyWidget (catalogue->Cat_Widget);
#else /* _GTK */
	     w = GTK_WIDGET(catalogue->Cat_Widget)->parent;
	     gtk_widget_destroy (catalogue->Cat_Widget);
#endif /* _GTK */
#else  /* _WINDOWS */
	     w = GetParent (catalogue->Cat_Widget);
	     DestroyWindow (w);
#endif /* _WINDOWS */
	     catalogue->Cat_Widget = 0;
	  }
	else
	  {
	     /* Ce sont des menus */
	     adbloc = catalogue->Cat_Entries;
	     /* On saute les entrees 0 et 1 */
	     ent = 2;
	     w = 0;
	     if (adbloc != NULL)
		/* Liberation de toutes les entrees du menu */
		while (adbloc->E_ThotWidget[ent] != 0)
		  {
		     /* Recuperation du widget parent en sautant le widget titre */
#ifdef _WINDOWS
		     if (w == 0 && ent != 0)
			w = GetParent (adbloc->E_ThotWidget[ent]);
		     DestroyWindow (adbloc->E_ThotWidget[ent]);
#else  /* _WINDOWS */
#ifndef _GTK
		     if (w == 0 && ent != 0)
			w = XtParent (adbloc->E_ThotWidget[ent]);
		     /* Libere les widgets */
		     XtUnmanageChild (adbloc->E_ThotWidget[ent]);
		     XtDestroyWidget (adbloc->E_ThotWidget[ent]);
#else /* _GTK */
		     if (w == 0 && ent != 0)
		       w = GTK_WIDGET(adbloc->E_ThotWidget[ent])->parent;

		     /* Libere les widgets */
		     gtk_widget_hide (GTK_WIDGET(adbloc->E_ThotWidget[ent]));
		     gtk_widget_destroy (GTK_WIDGET(adbloc->E_ThotWidget[ent]));
#endif /* _GTK */
#endif /* _WINDOWS */
		     adbloc->E_ThotWidget[ent] = (ThotWidget) 0;
		     /* Faut-il changer de bloc d'entrees ? */
		     ent++;
		     if (ent >= C_NUMBER)
		       {
			  if (adbloc->E_Next != NULL)
			     adbloc = adbloc->E_Next;
			  ent = 0;
		       }
		  }
	     /* Note que tous les fils sont detruits par MOTIF */
	     if ((catalogue->Cat_Type == CAT_POPUP)
		 || (catalogue->Cat_Type == CAT_SCRPOPUP)
		 || (catalogue->Cat_Type == CAT_PULL)
		 || (catalogue->Cat_Type == CAT_MENU))
	       ClearChildren (catalogue);
	     
	     /* Libere les blocs des entrees */
	     adbloc = catalogue->Cat_Entries->E_Next;
	     FreeEList (adbloc);
	     catalogue->Cat_Entries->E_Next = NULL;
	  }
	/* On memorise le widget parent des entrees a recreer */
	catalogue->Cat_XtWParent = w;
	return (0);
     }
}


/*----------------------------------------------------------------------
   TtaNewPulldown cre'e un pull-down menu :                           
   The parameter ref donne la re'fe'rence pour l'application.         
   The parameter parent identifie le widget pe`re du pull-down menu.  
   The parameter title donne le titre du catalogue.                   
   The parameter number indique le nombre d'entre'es dans le menu.    
   The parameter text contient la liste des intitule's du catalogue.  
   Chaque intitule' commence par un caracte`re qui donne le type de   
   l'entre'e et se termine par un caracte`re de fin de chai^ne \0.    
   S'il n'est pas nul, le parame`tre ] donne les acce'le'rateurs  
   des entre'es du menu.    
   Retourne un code d'erreur.
  ----------------------------------------------------------------------*/
void TtaNewPulldown (int ref, ThotMenu parent, char *title, int number,
		     char *text, char* equiv)
{
  ThotMenu            menu;
  ThotWidget          w;
  struct Cat_Context *catalogue;
  struct E_List      *adbloc;
  register int        count;
  register int        index;
  register int        ent;
  int                 eindex;
  register int        i;
  ThotBool            rebuilded;
#if defined (_WINDOWS) || defined (_GTK)
  char                menu_item [1024];
  char                equiv_item [255];
#else /* _WINDOWS || _GTK */
  Arg                 args[MAX_ARGS];
  XmString            title_string = NULL;
  int                 n = 0;
#endif /* _WINDOWS || _GTK */
#ifdef _GTK
  GtkWidget          *table;
  ThotWidget          wlabel;
#endif /* _GTK */

#if defined (_WINDOWS) || defined (_GTK)
  equiv_item[0] = 0;
#endif /* _WINDOWS || _GTK */
  if (ref == 0)
    {
      TtaError (ERR_invalid_reference);
      return;
    }
  catalogue = CatEntry (ref);
  if (catalogue == NULL)
    TtaError (ERR_cannot_create_dialogue);
  else
    {
      /* Est-ce que le catalogue existe deja ? */
      rebuilded = FALSE;
      if (catalogue->Cat_Widget != 0)
	{
	  if (catalogue->Cat_Type == CAT_PULL)
	    {
	      DestContenuMenu (catalogue); /* Modification of the catalogue */
	      rebuilded = TRUE;
	    }
	  else
	    TtaDestroyDialogue (ref);
	}
      if (parent == 0)
	{
	  TtaError (ERR_invalid_parent_dialogue);
	  return;
	}
      else if (number == 0)
	menu = (ThotMenu) - 1;	/* not a pull-down */
      else if (!rebuilded)
	{
#ifdef _WINDOWS
	  menu = parent;
#else  /* _WINDOWS */
#ifndef _GTK
	  /* Create the menu */
	  n = 0;
	  XtSetArg (args[n], XmNbackground, BgMenu_Color);
	  n++;
	  menu = XmCreatePulldownMenu (XtParent (parent), "Dialogue", args, n);
#else /* _GTK */
	  menu = gtk_menu_new ();
	  /* 
	     peut -être rajouter une bouton pour détacher le menu ?
	     se fait avec gtk_menu_set_tearoff_state;
	  */
#endif /* _GTK */
#endif /* _WINDOWS */
	}
      else
	menu = (ThotMenu) catalogue->Cat_Widget;
      catalogue->Cat_Ref = ref;
      catalogue->Cat_Type = CAT_PULL;
      catalogue->Cat_Button = 'L';
      catalogue->Cat_Data = -1;
      catalogue->Cat_Widget = (ThotWidget) menu;
  	  catalogue->Cat_ParentWidget = parent;
      adbloc = catalogue->Cat_Entries;
#ifdef _WINDOWS
      if (parent)
	WIN_AddFrameCatalogue (parent, catalogue);
      if (number == 0)
	/* it's a simple button not a pull-down */
	return;
#else  /* _WINDOWS */
#ifdef _GTK 
      if (number == 0)
	{
	  /* it's a simple button not a pull-down */
	  if (parent)
	    {
	      gtk_widget_show_all (parent);
	      ConnectSignalGTK (GTK_OBJECT(parent), "activate",
				GTK_SIGNAL_FUNC (CallMenuGTK), (gpointer)catalogue);
	    }
	  return;
	}
#else /* _GTK */
      if (number == 0)
	{
	  /* it's a simple button not a pull-down */
	  if (parent)
	    {
	      XtManageChild (parent);
	      XtAddCallback (parent, XmNcascadingCallback, (XtCallbackProc) CallMenu, catalogue);
	    }
	  return;
	}
#endif /*_GTK */
#endif /* _WINDOWS */
      /*** Create the menu title ***/
      if (title)
	{
#ifndef _GTK
#ifdef _WINDOWS
	  if (!rebuilded)
	    {
	      adbloc = NewEList ();
	      catalogue->Cat_Entries = adbloc;
	      adbloc->E_ThotWidget[0] = (ThotWidget) 0;
	      adbloc->E_ThotWidget[1] = (ThotWidget) 0;
	    }
#else  /* _WINDOWS */
	  if (!rebuilded)
	    {
	      adbloc = NewEList ();
	      catalogue->Cat_Entries = adbloc;
	      n = 0;
	      title_string = XmStringCreateSimple (title);
	      XtSetArg (args[n], XmNlabelString, title_string);
	      n++;
	      XtSetArg (args[n], XmNfontList, DefaultFont);
	      n++;
	      XtSetArg (args[n], XmNbackground, BgMenu_Color);
	      n++;
	      w = XmCreateLabel (menu, "Dialogue", args, n);
	      XtManageChild (w);
	      adbloc->E_ThotWidget[0] = w;
	      n = 0;
	      XtSetArg (args[n], XmNmarginHeight, 0);
	      n++;
	      XtSetArg (args[n], XmNspacing, 0);
	      n++;
	      XtSetArg (args[n], XmNbackground, BgMenu_Color);
	      n++;
	      XtSetArg (args[n], XmNseparatorType, XmSHADOW_ETCHED_OUT);
	      n++;
	      w = XmCreateSeparator (menu, "Dialogue", args, n);
	      XtManageChild (w);
	      adbloc->E_ThotWidget[1] = w;
	    }
	  else if (adbloc->E_ThotWidget[0] != 0)
	    XtSetValues (adbloc->E_ThotWidget[0], args, n);
	  if (!title_string)
	    XmStringFree (title_string);
#endif /* _WINDOWS */
#endif /*_GTK */
	}
      else if (!rebuilded)
	{
	  adbloc = NewEList ();
	  catalogue->Cat_Entries = adbloc;
	}
#if !defined(_WINDOWS) && !defined(_GTK)
      /* Cree les differentes entrees du menu */
      n = 0;
      XtSetArg (args[n], XmNfontList, DefaultFont);
      n++;
      XtSetArg (args[n], XmNmarginWidth, 0);
      n++;
      XtSetArg (args[n], XmNmarginHeight, 0);
      n++;
      XtSetArg (args[n], XmNbackground, BgMenu_Color);
      n++;
      XtSetArg (args[n], XmNforeground, FgMenu_Color);
      n++;
      if (equiv)
	n++;
#endif /* _WINDOWS && _GTK */
      i = 0;
      index = 0;
      eindex = 0;
      ent = 2;
      if (text)
	while (i < number)
	  {
	    count = strlen (&text[index]);	/* Longueur de l'intitule */
	    /* S'il n'y a plus d'intitule -> on arrete */
	    if (count == 0)
	      {
		i = number;
		TtaError (ERR_invalid_parameter);
		return;
	      }
	    else
	      {
		/* Faut-il changer de bloc d'entrees ? */
		if (ent >= C_NUMBER)
		  {
		    adbloc->E_Next = NewEList ();
		    adbloc = adbloc->E_Next;
		    ent = 0;
		  }
		/* Recupere le type de l'entree */
		adbloc->E_Type[ent] = text[index];
		adbloc->E_Free[ent] = 'Y';
		/* Note l'accelerateur */
		if (equiv)
		  {
#ifdef _WINDOWS
		    if (&equiv[eindex] != EOS)
		      strcpy (equiv_item, &equiv[eindex]); 
#else  /* _WINDOWS */
#ifndef _GTK
		    title_string = XmStringCreate (&equiv[eindex], XmSTRING_DEFAULT_CHARSET);
		    XtSetArg (args[n - 1], XmNacceleratorText, title_string);
#else /* _GTK */
		    if (&equiv[eindex] != EOS)
		      strcpy (equiv_item, &equiv[eindex]); 
#endif /* _GTK */
#endif /* _WINDOWS */
		    eindex += strlen (&equiv[eindex]) + 1;
		  }
		if (text[index] == 'B')
		  /*__________________________________________ Creation d'un bouton __*/
		  {
#ifdef _WINDOWS 
		    if (equiv_item && equiv_item[0] != EOS)
		      {
			sprintf (menu_item, "%s\t%s", &text[index + 1], equiv_item); 
			AppendMenu (menu, MF_STRING | MF_UNCHECKED, ref + i, menu_item);
			equiv_item[0] = EOS;
		      }
		    else 
		      AppendMenu (menu, MF_STRING | MF_UNCHECKED, ref + i, &text[index + 1]);
		    adbloc->E_ThotWidget[ent] = (ThotWidget) i;
#else  /* _WINDOWS */
#ifdef _GTK
		    sprintf (menu_item, "%s", &text[index + 1]);
		    /* \t doesn't mean anything to gtk... to we align ourself*/
		    if (equiv_item && equiv_item[0] != EOS)
		      {
			strcat (menu_item, "  ");
			w = gtk_menu_item_new ();
			table = gtk_table_new (1, 3, FALSE);    
			gtk_container_add (GTK_CONTAINER (w), table);  
			wlabel = gtk_label_new (menu_item);
			/*(that's left-justified, right is 1.0, center is 0.5).*/
			/*gtk_label don't seem to like table cell...so*/
			gtk_misc_set_alignment (GTK_MISC(wlabel), 0.0 , 0); 
			gtk_table_attach_defaults (GTK_TABLE(table), wlabel, 0, 1, 0, 1);
			gtk_widget_show (wlabel);
			gtk_label_set_justify (GTK_LABEL(wlabel), GTK_JUSTIFY_LEFT);
			wlabel = gtk_label_new (equiv_item);
			gtk_misc_set_alignment (GTK_MISC(wlabel), 1.0, 0); 
			gtk_table_attach_defaults (GTK_TABLE(table), wlabel, 2, 3, 0, 1);
			gtk_widget_show (wlabel);
			gtk_widget_show (table);
		      }
		    else
		      w = gtk_menu_item_new_with_label (menu_item);
		    ConnectSignalGTK (GTK_OBJECT(w),  "activate",
				      GTK_SIGNAL_FUNC (CallMenuGTK), (gpointer)catalogue);	
		    gtk_widget_show (w);
		    gtk_menu_append (GTK_MENU (menu), w);
		    adbloc->E_ThotWidget[ent] = w;
#else /* _GTK */
		    w = XmCreatePushButton (menu, &text[index + 1], args, n);
		    XtManageChild (w);
		    adbloc->E_ThotWidget[ent] = w;
		    XtAddCallback (w, XmNactivateCallback, (XtCallbackProc) CallMenu, catalogue);
#endif /* _GTK */
#endif /* _WINDOWS */
		  }
		else if (text[index] == 'T')
		  /*__________________________________________ Creation d'un toggle __*/
		  {
		    /* un toggle a faux */
#ifdef _WINDOWS
		    if (equiv_item && equiv_item[0] != EOS)
		      {
			sprintf (menu_item, "%s\t%s", &text[index + 1], equiv_item);
			equiv_item[0] = EOS;
		      }
		    else
		      sprintf (menu_item, "%s", &text[index + 1]);
		    AppendMenu (menu, MF_STRING | MF_UNCHECKED, ref + i, menu_item);
		    adbloc->E_ThotWidget[ent] = (ThotWidget) i;
#else  /* _WINDOWS */
#ifdef _GTK
		    /* \t doesn't mean anything to gtk... to we align ourself*/
		    sprintf (menu_item, "%s", &text[index + 1]);
		    if (equiv_item && equiv_item[0] != EOS)
		      {
			strcat (menu_item, "  ");
			w = gtk_check_menu_item_new ();
			table = gtk_table_new (1, 3, FALSE);    
			gtk_container_add (GTK_CONTAINER (w), table);  
			wlabel = gtk_label_new (menu_item);
			/*(that's left-justified, right is 1.0, center is 0.5).*/
			/*gtk_label don't seem to like table cell...so*/
			gtk_misc_set_alignment (GTK_MISC(wlabel), 0.0 , 0); 
			gtk_table_attach_defaults (GTK_TABLE(table), wlabel, 0, 1, 0, 1);   
			gtk_widget_show (wlabel);
			gtk_label_set_justify (GTK_LABEL(wlabel), GTK_JUSTIFY_LEFT);
			wlabel = gtk_label_new (equiv_item);
			gtk_misc_set_alignment (GTK_MISC(wlabel), 1.0, 0); 
			gtk_table_attach_defaults (GTK_TABLE(table), wlabel, 2, 3, 0, 1);   
			gtk_widget_show (wlabel);
			gtk_widget_show (table);
		      }
		    else
		      w = gtk_check_menu_item_new_with_label (menu_item);
		    gtk_widget_show_all (w);
		    gtk_menu_append (GTK_MENU (menu),w);
		    gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (w), FALSE);
		    gtk_check_menu_item_set_show_toggle (GTK_CHECK_MENU_ITEM (w), TRUE);
		    ConnectSignalGTK (GTK_OBJECT(w), "activate",
				      GTK_SIGNAL_FUNC (CallMenuGTK), (gpointer)catalogue);
		    adbloc->E_ThotWidget[ent] = w;
#else /* _GTK */
		    XtSetArg (args[n], XmNvisibleWhenOff, TRUE);
		    XtSetArg (args[n + 1], XmNselectColor, BgMenu_Color);
		    w = XmCreateToggleButton (menu, &text[index + 1], args, n + 2);
		    XtManageChild (w);
		    adbloc->E_ThotWidget[ent] = w;
		    XtAddCallback (w, XmNvalueChangedCallback, (XtCallbackProc) CallMenu, catalogue);
#endif /* _GTK */
#endif /* _WINDOWS */
		  }
		else if (text[index] == 'M')
		  /*_______________________________________ Creation d'un sous-menu __*/
		  {
		    /* En attendant le sous-menu on cree un bouton */
#ifdef _WINDOWS
		    w = (HMENU) CreatePopupMenu ();
		    subMenuID [currentFrame] = (UINT) w;
		    AppendMenu (menu, MF_POPUP, (UINT) w, &text[index + 1]);
		    adbloc->E_ThotWidget[ent] = w;
#else  /* _WINDOWS */
#ifdef _GTK
		    sprintf (menu_item, "%s", &text[index + 1]);
		    w = gtk_menu_item_new_with_label (menu_item);
		    gtk_widget_show_all (w);
		    gtk_menu_append (GTK_MENU (menu),w);
		    adbloc->E_ThotWidget[ent] = w;
#else /* _GTK */
		    w = XmCreateCascadeButton (menu, &text[index + 1], args, n);
		    adbloc->E_ThotWidget[ent] = w;
#endif /* _GTK */
#endif /* _WINDOWS */
		  }
		else if (text[index] == 'S')
		  /*_________________________________ Creation d'un separateur __*/
		  {
#ifdef _WINDOWS
		    AppendMenu (menu, MF_SEPARATOR, 0, NULL);
		    adbloc->E_ThotWidget[ent] = (ThotWidget) 0;
#else  /* _WINDOWS */
#ifdef _GTK
		    w = gtk_menu_item_new ();
		    gtk_widget_show_all (w);
		    gtk_menu_append (GTK_MENU (menu),w); 
		    adbloc->E_ThotWidget[ent] = w;		 
#else /* _GTK */
		    XtSetArg (args[n], XmNseparatorType, XmSINGLE_DASHED_LINE);
		    w = XmCreateSeparator (menu, "Dialogue", args, n + 1);
		    XtManageChild (w);
		    adbloc->E_ThotWidget[ent] = w;
#endif /* _GTK */
#endif /* _WINDOWS */
		  }
		else
		  /*____________________________________ Une erreur de construction __*/
		  {
			 TtaDestroyDialogue (ref);
			 /* Type d'entree non defini */
			 TtaError (ERR_invalid_parameter);
			 return;
		  }
#ifndef _WINDOWS
#ifndef _GTK
		/* liberation de la string */
		if (equiv != NULL && !title_string)
		  XmStringFree (title_string);
#endif /* _GTK */ /* TODO : verifier cette liberation */
#endif /* _WINDOWS */
		i++;
		ent++;
		index += count + 1;
	      }
	  }
      /* Attache le pull-down menu au widget passe en parametre */
      if (parent != 0 && !rebuilded)
	{
#ifndef _WINDOWS
#ifdef _GTK
	  gtk_menu_item_set_submenu (GTK_MENU_ITEM (parent), menu);
	  gtk_object_set_data (GTK_OBJECT(menu), "MenuItem", (gpointer)parent);
	  gtk_widget_show_all (parent);
#else /* _GTK */
	  n = 0;
	  XtSetArg (args[n], XmNsubMenuId, menu);
	  n++;
	  XtSetValues (parent, args, n);
	  XtManageChild (parent);
#endif /* _GTK */
#endif /* _WINDOWS */
	}
    }
}

/*----------------------------------------------------------------------
   TtaSetPulldownOff suspend le pulldown                           
  ----------------------------------------------------------------------*/
#ifdef _WINDOWS
void WIN_TtaSetPulldownOff (int ref, ThotMenu parent, HWND owner)
#else  /* _WINDOWS */
void TtaSetPulldownOff (int ref, ThotWidget parent)
#endif /* _WINDOWS */
{
   struct Cat_Context *catalogue;
#ifndef _WINDOWS
#ifndef _GTK
   Arg                 args[MAX_ARGS];
#else /* _GTK */
#endif /* _GTK */
#else  /* _WINDOWS */
   int                 frame;
#endif /* _WINDOWS */

   if (ref == 0)
      TtaError (ERR_invalid_reference);
   else if (parent == 0)
      TtaError (ERR_invalid_parent_dialogue);
   else
     {
	catalogue = CatEntry (ref);
	if (catalogue == NULL)
	   TtaError (ERR_invalid_reference);
#ifndef _WINDOWS
#ifndef _GTK
	else if (catalogue->Cat_Widget != 0)
	  {
             XtSetArg (args[0], XmNsubMenuId, NULL);
             XtSetValues (parent, args, 1);
             XtManageChild (parent);
	  }
#else /* _GTK */
	else if (catalogue->Cat_Widget != 0)
	  {
	     gtk_widget_set_sensitive (GTK_WIDGET(parent), FALSE);
	     gtk_widget_show_all (GTK_WIDGET(parent));
	  }
#endif /* _GTK */
#else  /* _WINDOWS */
        frame = GetMainFrameNumber (owner);
        EnableMenuItem ((HMENU)WinMenus[frame], (UINT)parent, MF_GRAYED);
	DrawMenuBar (FrMainRef[frame]); 
#endif /* _WINDOWS */
     }
}

/*----------------------------------------------------------------------
   TtaSetPulldownOn reactive le pulldown                           
  ----------------------------------------------------------------------*/
#ifdef _WINDOWS
void WIN_TtaSetPulldownOn (int ref, ThotMenu parent, HWND owner)
#else  /* _WINDOWS */
void TtaSetPulldownOn (int ref, ThotWidget parent)
#endif /* _WINDOWS */
{
   struct Cat_Context *catalogue;
   ThotWidget          menu;
#ifndef _WINDOWS
#ifndef _GTK
   Arg                 args[MAX_ARGS];
#endif /* _GTK */
#else  /* _WINDOWS */
   int                 frame;
#endif /* _WINDOWS */

   if (ref == 0)
      TtaError (ERR_invalid_reference);
   else if (parent == 0)
      TtaError (ERR_invalid_parent_dialogue);
   else
     {
	catalogue = CatEntry (ref);
	if (catalogue == NULL)
	   TtaError (ERR_invalid_reference);
	else if (catalogue->Cat_Widget != 0)
	  {
	     menu = catalogue->Cat_Widget;
#ifndef _WINDOWS
#ifndef _GTK
             XtSetArg (args[0], XmNsubMenuId, menu);
             XtSetValues (parent, args, 1);
             XtManageChild (parent);
#else /* _GTK */
	     gtk_widget_set_sensitive (GTK_WIDGET(parent), TRUE);
	     gtk_widget_show_all (GTK_WIDGET(parent));
#endif /* _GTK */
#else  /* _WINDOWS */
	     frame = GetMainFrameNumber (owner);
             EnableMenuItem ((HMENU)WinMenus[frame], (UINT)parent, MF_ENABLED);
	     DrawMenuBar (FrMainRef[frame]); 
#endif /* _WINDOWS */
	  }
     }
}


/*----------------------------------------------------------------------
   TtaNewPopup cre'e un pop-up menu :                                 
   The parameter ref donne la re'fe'rence pour l'application.         
   The parameter title donne le titre du catalogue.                   
   The parameter number indique le nombre d'entre'es dans le menu.    
   The parameter text contient la liste des intitule's du catalogue.  
   Chaque intitule' commence par un caracte`re qui donne le type de   
   l'entre'e et se termine par un caracte`re de fin de chai^ne \0.    
   S'il n'est pas nul, le parame`tre equiv donne les acce'le'rateurs  
   des entre'es du menu.                                              
   The parameter button indique le bouton de la souris qui active le  
   menu : 'L' pour left, 'M' pour middle et 'R' pour right.           
  ----------------------------------------------------------------------*/
void TtaNewPopup (int ref, ThotWidget parent, char *title, int number,
		  char *text, char *equiv, char button)
{
  register int        count;
  register int        index;
  register int        ent;
  register int        i;
  int                 eindex;
  ThotBool            rebuilded;
  struct Cat_Context *catalogue;
  struct E_List      *adbloc;
#ifdef _WINDOWS
  HMENU               menu;
  HMENU               w;
  int                 nbOldItems, ndx;
#else /* _WINDOWS */
#ifndef _GTK
  Arg                 args[MAX_ARGS];
  XmString            title_string = NULL;
  int                 n;
#else /* _GTK */
  GtkWidget          *table;
  ThotWidget          wlabel;
  char                menu_item [1024];
  char                equiv_item [255];
#endif /* _GTK */
  ThotWidget          menu;
  ThotWidget          w;
#endif /* _WINDOWS */

#ifdef _GTK
  equiv_item[0] = 0;
#endif /* _GTK */
  if (ref == 0)
    {
      TtaError (ERR_invalid_reference);
      return;
    }
  catalogue = CatEntry (ref);
  menu = 0;
  if (catalogue == NULL)
    TtaError (ERR_cannot_create_dialogue);
  else
    {
      /* Est-ce que le catalogue existe deja ? */
      rebuilded = FALSE;
      if (catalogue->Cat_Widget != 0)
	{
	  if (catalogue->Cat_Type == CAT_POPUP)
	    {
	      /* Modification du catalogue */
	      DestContenuMenu (catalogue);
	      rebuilded = TRUE;
	    }
	  else
	    TtaDestroyDialogue (ref);
	}
#ifndef _WINDOWS
#ifndef _GTK
      if (!rebuilded)
	{
	  /* Creation du Popup Shell pour contenir le menu */
	  n = 0;
	  XtSetArg (args[n], XmNheight, (Dimension) 10);
	  n++;
	  XtSetArg (args[n], XmNwidth, (Dimension) 10);
	  n++;
	  menu = XtCreatePopupShell ("Dialogue", xmMenuShellWidgetClass, RootShell, args, n);
	}
      /* Cree le menu correspondant */
      if (button == 'R')
	XtSetArg (args[0], XmNwhichButton, Button3);
      else if (button == 'M')
	XtSetArg (args[0], XmNwhichButton, Button2);
      else
	{
	  button = 'L';
	  XtSetArg (args[0], XmNwhichButton, Button1);
	}
#else /* _GTK */
      menu = gtk_menu_new ();
      ConnectSignalGTK (GTK_OBJECT (menu), "delete_event",
			GTK_SIGNAL_FUNC (formKillGTK), (gpointer) catalogue);
      ConnectSignalGTK (GTK_OBJECT (menu), "unmap_event",
			GTK_SIGNAL_FUNC (formKillGTK), (gpointer) catalogue);
#endif /* _GTK */
#endif /* _WINDOWS */
      if (!rebuilded)
	{
#ifndef _WINDOWS
#ifndef _GTK
	  n = 1;
	  XtSetArg (args[n], XmNmarginWidth, 0);
	  n++;
	  XtSetArg (args[n], XmNmarginHeight, 0);
	  n++;
	  XtSetArg (args[n], XmNspacing, 0);
	  n++;
	  XtSetArg (args[n], XmNbackground, BgMenu_Color);
	  n++;
	  XtSetArg (args[n], XmNrowColumnType, XmMENU_POPUP);
	  n++;
	  menu = XmCreateRowColumn (menu, "Dialogue", args, n);
	  XtAddCallback (XtParent (menu), XmNpopdownCallback, (XtCallbackProc) UnmapMenu, catalogue);
#endif /* _GTK */
#else  /* _WINDOWS */
	  menu = CreatePopupMenu ();
#endif /* _WINDOWS */
	  catalogue->Cat_Widget = menu;
	  catalogue->Cat_Ref = ref;
	  catalogue->Cat_ParentWidget = parent; /* remember the parent widget for the callback */
	  catalogue->Cat_Type = CAT_POPUP;
	  catalogue->Cat_Button = button;
	  /* Initialisation de la liste des widgets fils */
	  adbloc = NewEList ();
	  catalogue->Cat_Entries = adbloc;
	}
      else
	{
	  /* Mise a jour du menu existant */
	  menu = catalogue->Cat_Widget;
	  adbloc = catalogue->Cat_Entries;
	  /* Si on a change de bouton on met a jour le widget avec args[0] */
	  if (catalogue->Cat_Button != button)
	    {
#if !defined (_WINDOWS) && !defined(_GTK)
	      XtSetValues (menu, args, 1);
#endif /* _WINDOWS && _GTK */
	      catalogue->Cat_Button = button;
	    }
	  else
	    button = catalogue->Cat_Button;
	}
      catalogue->Cat_Data = -1;
#ifdef _WINDOWS
      if (parent != 0)
	WIN_AddFrameCatalogue (parent, catalogue);
#endif /* _WINDOWS */
      
      /*** Cree le titre du menu ***/
      if (title != NULL)
	{
#if !defined (_WINDOWS) && !defined(_GTK)
	  n = 0;
	  title_string = XmStringCreateSimple (title);
	  XtSetArg (args[n], XmNlabelString, title_string);
	  n++;
#endif /* _WINDOWS && GTK */
	  if (!rebuilded)
	    {
#ifdef _WINDOWS
	      adbloc->E_ThotWidget[0] = (ThotWidget) 0;
	      adbloc->E_ThotWidget[1] = (ThotWidget) 0;
#else  /* _WINDOWS */
#ifndef _GTK
	      XtSetArg (args[n], XmNfontList, DefaultFont);
	      n++;
	      XtSetArg (args[n], XmNmarginHeight, 0);
	      n++;
	      XtSetArg (args[n], XmNbackground, BgMenu_Color);
	      n++;
	      XtSetArg (args[n], XmNforeground, FgMenu_Color);
	      n++;
	      w = XmCreateLabel (menu, "Dialogue", args, n);
	      XtManageChild (w);
	      adbloc->E_ThotWidget[0] = w;
	      n = 0;
	      XtSetArg (args[n], XmNbackground, BgMenu_Color);
	      n++;
	      XtSetArg (args[n], XmNseparatorType, XmSHADOW_ETCHED_OUT);
	      n++;
	      w = XmCreateSeparator (menu, "Dialogue", args, n);
	      XtManageChild (w);
	      adbloc->E_ThotWidget[1] = w;
#endif /* _GTK */
#endif /* _WINDOWS */
	    }
#if !defined (_WINDOWS) && !defined(_GTK)
	  else if (adbloc->E_ThotWidget[0] != 0)
	    XtSetValues (adbloc->E_ThotWidget[0], args, n);
	  XmStringFree (title_string);
#endif /* _WINDOWS && _GTK */
	}
      /* Cree les differentes entrees du menu */
#ifndef _WINDOWS
#ifndef _GTK
      n = 0;
      XtSetArg (args[n], XmNfontList, DefaultFont);
      n++;
      XtSetArg (args[n], XmNmarginWidth, 0);
      n++;
      XtSetArg (args[n], XmNmarginHeight, 0);
      n++;
      XtSetArg (args[n], XmNbackground, BgMenu_Color);
      n++;
      XtSetArg (args[n], XmNforeground, FgMenu_Color);
      n++;
      if (equiv != NULL)
	n++;
#endif /* _GTK */
#else /* _WINDOWS */
      nbOldItems = GetMenuItemCount (menu);
      for (ndx = 0; ndx < nbOldItems; ndx ++)
        if (!DeleteMenu (menu, ref + ndx, MF_BYCOMMAND))
	  DeleteMenu (menu, ndx, MF_BYPOSITION);
#endif /* _WINDOWS */
      
      i = 0;
      index = 0;
      eindex = 0;
      ent = 2;
      if (text != NULL)
	while (i < number)
	  {
	    count = strlen (&text[index]);	/* Longueur de l'intitule */
	    /* S'il n'y a plus d'intitule -> on arrete */
	    if (count == 0)
	      {
		i = number;
		TtaError (ERR_invalid_parameter);
		break;
	      }
	    else
	      {
		/* Faut-il changer de bloc d'entrees ? */
		if (ent >= C_NUMBER)
		  {
		    adbloc->E_Next = NewEList ();
		    adbloc = adbloc->E_Next;
		    ent = 0;
		  }
		/* Recupere le type de l'entree */
		adbloc->E_Type[ent] = text[index];
		adbloc->E_Free[ent] = 'Y';
		/* Note l'accelerateur */
		if (equiv != NULL)
		  {
#ifndef _WINDOWS
#ifndef _GTK
		    title_string = XmStringCreate (&equiv[eindex], XmSTRING_DEFAULT_CHARSET);
		    XtSetArg (args[n - 1], XmNacceleratorText, title_string);
#else /* _GTK */
		    if (&equiv[eindex] != EOS)
		      strcpy (equiv_item, &equiv[eindex]); 
#endif /* _GTK */
#endif /* _WINDOWS */
		    eindex += strlen (&equiv[eindex]) + 1;
		  }
		if (text[index] == 'B')
		  /*__________________________________________ Creation d'un bouton __*/
		  {
#ifdef _WINDOWS
		    AppendMenu (menu, MF_STRING, ref + i, &text[index + 1]);
		    adbloc->E_ThotWidget[ent] = (ThotWidget) i;
#else  /* _WINDOWS */
#ifdef _GTK
		    sprintf (menu_item, "%s", &text[index + 1]);
		    /* \t doesn't mean anything to gtk... to we align ourself*/
		    if (equiv_item && equiv_item[0] != EOS)
		      {
			strcat (menu_item, "  ");
			w = gtk_menu_item_new ();
			table = gtk_table_new (1, 3, FALSE);    
			gtk_container_add (GTK_CONTAINER (w), table);  
			wlabel = gtk_label_new(menu_item);
			/*(that's left-justified, right is 1.0, center is 0.5).*/
			/*gtk_label don't seem to like table cell...so*/
			gtk_misc_set_alignment(GTK_MISC(wlabel), 0.0 , 0); 
			gtk_table_attach_defaults (GTK_TABLE(table), wlabel, 0, 1, 0, 1);   
			gtk_widget_show (wlabel);
			gtk_label_set_justify(GTK_LABEL(wlabel), GTK_JUSTIFY_LEFT);
			
			wlabel = gtk_label_new(equiv_item);
			gtk_misc_set_alignment(GTK_MISC(wlabel), 1.0, 0); 
			gtk_table_attach_defaults (GTK_TABLE(table), wlabel, 2, 3, 0, 1);   
			gtk_widget_show (wlabel);
			gtk_widget_show (table);
		      }
		    else
		      w = gtk_menu_item_new_with_label (menu_item);
		    ConnectSignalGTK (GTK_OBJECT(w), "activate",
					GTK_SIGNAL_FUNC (CallMenuGTK), (gpointer)catalogue);	
		    gtk_widget_show (w);
		    gtk_menu_append (GTK_MENU (menu), w);
		    adbloc->E_ThotWidget[ent] = w;
#else /* _GTK */
		    w = XmCreatePushButton (menu, &text[index + 1], args, n);
		    XtManageChild (w);
		    adbloc->E_ThotWidget[ent] = w;
		    XtAddCallback (w, XmNactivateCallback, (XtCallbackProc) CallMenu, catalogue);
#endif /* _GTK */
#endif /* _WINDOWS */
		  }
		else if (text[index] == 'T')
		  /*__________________________________________ Creation d'un toggle __*/
		  {
#ifdef _WINDOWS
		    AppendMenu (menu, MF_STRING | MF_UNCHECKED, ref + i, &text[index + 1]);
		    adbloc->E_ThotWidget[ent] = (ThotWidget) i;
#else  /* _WINDOWS */
#ifdef _GTK
		    /* \t doesn't mean anything to gtk... to we align ourself*/
		    sprintf (menu_item, "%s", &text[index + 1]);
		    if (equiv_item && equiv_item[0] != 0)
		      {
			strcat (menu_item, "  ");
			w = gtk_check_menu_item_new ();
			table = gtk_table_new (1, 3, FALSE);    
			gtk_container_add (GTK_CONTAINER (w), table);  
			wlabel = gtk_label_new(menu_item);
			/*(that's left-justified, right is 1.0, center is 0.5).*/
			/*gtk_label don't seem to like table cell...so*/
			gtk_misc_set_alignment(GTK_MISC(wlabel), 0.0 , 0); 
			gtk_table_attach_defaults (GTK_TABLE(table), wlabel, 0, 1, 0, 1);   
			gtk_widget_show (wlabel);
			gtk_label_set_justify(GTK_LABEL(wlabel), GTK_JUSTIFY_LEFT);
			
			wlabel = gtk_label_new(equiv_item);
			gtk_misc_set_alignment(GTK_MISC(wlabel), 1.0, 0); 
			gtk_table_attach_defaults (GTK_TABLE(table), wlabel, 2, 3, 0, 1);   
			gtk_widget_show (wlabel);
			gtk_widget_show (table);
		      }
		    else
		      w = gtk_check_menu_item_new_with_label (menu_item);
		    gtk_widget_show_all (w);
		    gtk_menu_append (GTK_MENU (menu), w);
		    gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (w), FALSE);
		    gtk_check_menu_item_set_show_toggle (GTK_CHECK_MENU_ITEM (w), TRUE);
		    ConnectSignalGTK (GTK_OBJECT(w), "activate",
				      GTK_SIGNAL_FUNC (CallMenuGTK), (gpointer)catalogue);
		    adbloc->E_ThotWidget[ent] = w;
#else /* _GTK */
		    XtSetArg (args[n], XmNvisibleWhenOff, TRUE);
		    XtSetArg (args[n + 1], XmNselectColor, BgMenu_Color);
		    w = XmCreateToggleButton (menu, &text[index + 1], args, n + 2);
		    XtManageChild (w);
		    adbloc->E_ThotWidget[ent] = w;
		    XtAddCallback (w, XmNvalueChangedCallback, (XtCallbackProc) CallMenu, catalogue);
#endif /* _GTK */
#endif /* _WINDOWS */
		  }
		else if (text[index] == 'M')
		  /*_______________________________________ Creation d'un sous-menu __*/
		  {
		    /* En attendant le sous-menu on cree un bouton */
#ifdef _WINDOWS
		    w = (HMENU) CreateMenu ();
		    AppendMenu (menu, MF_POPUP, (UINT) w, (LPCTSTR) (&text[index + 1]));
		    adbloc->E_ThotWidget[ent] = (ThotWidget) w;
#else  /* _WINDOWS */
#ifdef _GTK
		    sprintf (menu_item, "%s", &text[index + 1]);
		    w = gtk_menu_item_new_with_label (menu_item);
		    gtk_widget_show_all (w);
		    gtk_menu_append (GTK_MENU (menu),w);
		    adbloc->E_ThotWidget[ent] = w;
#else /* _GTK */
		    w = XmCreateCascadeButton (menu, &text[index + 1], args, n);
		    adbloc->E_ThotWidget[ent] = w;
#endif /* _GTK */
#endif /* _WINDOWS */
		  }
		else if (text[index] == 'S')
		  /*_________________________________ Creation d'un separateur __*/
		  {
#ifdef _WINDOWS
		    AppendMenu (menu, MF_SEPARATOR, 0, NULL);
		    adbloc->E_ThotWidget[ent] = (ThotWidget) i;
#else  /* _WINDOWS */
#ifdef _GTK
		    w = gtk_menu_item_new ();
		    gtk_widget_show_all (w);
		    gtk_menu_append (GTK_MENU (menu),w); 
		    adbloc->E_ThotWidget[ent] = w;		 
#else /* _GTK */
		    XtSetArg (args[n], XmNseparatorType, XmSINGLE_DASHED_LINE);
		    w = XmCreateSeparator (menu, "Dialogue", args, n + 1);
		    XtManageChild (w);
		    adbloc->E_ThotWidget[ent] = w;
#endif /* _GTK */
#endif /* _WINDOWS */
		  }
		else
		  /*____________________________________ Une erreur de construction __*/
		  {
		    TtaDestroyDialogue (ref);
		    TtaError (ERR_invalid_parameter);	/* Type d'entree non defini */
		    return;
		  }
#if !defined(_WINDOWS) && !defined(_GTK)
		/* liberation de la string */
		if (equiv != NULL)
		  XmStringFree (title_string);
#endif /* _WINDOWS && _GTK */
		i++;
		ent++;
		index += count + 1;
	      }
	  }
    }
}
/*----------------------------------------------------------------------
   TtaNewScrollPopup cre'e un pop-up menu :                                 
   The parameter ref donne la re'fe'rence pour l'application.         
   The parameter title donne le titre du catalogue.                   
   The parameter number indique le nombre d'entre'es dans le menu.    
   The parameter text contient la liste des intitule's du catalogue.  
   Chaque intitule' commence par un caracte`re qui donne le type de   
   l'entre'e et se termine par un caracte`re de fin de chai^ne \0.    
   S'il n'est pas nul, le parame`tre equiv donne les acce'le'rateurs  
   des entre'es du menu.                                              
   The parameter button indique le bouton de la souris qui active le  
   menu : 'L' pour left, 'M' pour middle et 'R' pour right.           
  ----------------------------------------------------------------------*/
void TtaNewScrollPopup (int ref, ThotWidget parent, char *title, int number,
			char *text, char *equiv, ThotBool multipleOptions, char button)
{
#if defined (_WINDOWS) || defined (_GTK)
  register int        count;
  register int        index;
  register int        ent;
  register int        i;
  int                 eindex;
  int                 height, width, max_entry_len;
  ThotBool            rebuilded;
  struct Cat_Context *catalogue;
  struct E_List      *adbloc;
  char                equiv_item [255];
  ThotWidget          w;
#ifdef _WINDOWS
  HWND                menu;
  HWND                listBox;
#else /* _WINDOWS */
  char                menu_item [1024];
  GtkWidget          *gtklist;
  GtkWidget          *scr_window = NULL;
  GtkWidget          *event_box;
  GtkWidget          *first = NULL;
  GList              *glist = NULL;
  GtkWidget          *table;
  ThotWidget          wlabel;  
  ThotWidget          menu;  
#endif  /* _WINDOWS */

  equiv_item[0] = 0;
  if (ref == 0)
    {
      TtaError (ERR_invalid_reference);
      return;
    }
  if (number <= 0)
    {
      TtaError (ERR_cannot_create_dialogue);
      return;
    }

  catalogue = CatEntry (ref);
  menu = 0;
  if (catalogue == NULL)
    {
      TtaError (ERR_cannot_create_dialogue);
      return;
    }
  /* Est-ce que le catalogue existe deja ? */
  rebuilded = FALSE;
  if (catalogue->Cat_Widget != 0)
    {
      if (catalogue->Cat_Type == CAT_SCRPOPUP)
	{
	  /* Modification du catalogue */
	  DestContenuMenu (catalogue);
	  rebuilded = TRUE;
	}
      else
	TtaDestroyDialogue (ref);
    }
  if (!rebuilded)
    {      
      /* Create a new dialog window for the scrolled window to be
	 packed into. */

      /* compute the widget size in number of chars and lines */
      i = 0;
      max_entry_len = 0;
      index = 0;
      while (i < number)
	{
	  int cur_len;
	  
	  cur_len = strlen (&text[index]);
	  if (max_entry_len < cur_len)
	    {
	      max_entry_len = cur_len;
	      /* limit the number of characters in an entry to 30 chars 
		 (more will require a scroll bar) */
	      if (max_entry_len > 30)
		{
		  max_entry_len = 30;
		  break;
		}
	    }
	  index = index + cur_len + 1;
	  i++;
	}
      width = (max_entry_len + 2);
      if (number < 10)
	height = number;
      else
	height = 10;

#ifdef _WINDOWS
      menu = WIN_InitScrPopup (parent, ref, multipleOptions, number, width, height);
      if (menu)
	listBox = GetDlgItem (menu, 1);
      else
	listBox = NULL;
#else /* _WINDOWS */
      menu =  gtk_window_new (GTK_WINDOW_POPUP);
      
      /* signals */
      ConnectSignalGTK (GTK_OBJECT (menu),
			"destroy",
			GTK_SIGNAL_FUNC (formKillGTK),
			(gpointer) catalogue);
      /* properties */
      gtk_window_set_policy (GTK_WINDOW (menu), TRUE, TRUE, FALSE);
      gtk_container_border_width (GTK_CONTAINER (menu), 0);
      gtk_window_set_position (GTK_WINDOW (menu), GTK_WIN_POS_MOUSE);
      gtk_widget_set_events (menu, GDK_KEY_PRESS_MASK | GDK_LEAVE_NOTIFY_MASK);
      /* create the event box that will be associated with it */
      event_box = gtk_event_box_new ();
      gtk_container_add (GTK_CONTAINER (menu), event_box);
      gtk_widget_show (event_box);
      /* properties */
      gtk_widget_realize (event_box);
      /* change the cursor when we're inside the popup */
      {
	GdkCursor *cursor;
	cursor = gdk_cursor_new (GDK_TOP_LEFT_ARROW);
	gdk_window_set_cursor (event_box->window, cursor);
	gdk_cursor_destroy (cursor);
      }
      /* create the scrolled window that will contain the list widget */
      scr_window = gtk_scrolled_window_new (NULL, NULL);
      gtk_widget_show (scr_window);
      /* set the widget size */
      /* (experience shows that GTK doesn't report a good font height ! Multiplying
	 it by two fixes a bit the problem)  */
      width = (width) * (CharacterWidth ((int) 'm',  FontDialogue));
      height = height * 2 * (CharacterHeight ((int) 'M', FontDialogue));
      gtk_widget_set_usize (scr_window, width, height);
      GTK_WIDGET_UNSET_FLAGS (GTK_SCROLLED_WINDOW (scr_window)->hscrollbar, GTK_CAN_FOCUS);
      GTK_WIDGET_UNSET_FLAGS (GTK_SCROLLED_WINDOW (scr_window)->vscrollbar, GTK_CAN_FOCUS);
      gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scr_window), GTK_POLICY_AUTOMATIC,
				      GTK_POLICY_AUTOMATIC);
      /* add the scrwindow to its container */
      gtk_container_add (GTK_CONTAINER (event_box), scr_window);
     
#endif /* _WINDOWS */
      catalogue->Cat_Widget = menu;
      catalogue->Cat_Ref = ref;
      catalogue->Cat_Type = CAT_SCRPOPUP;
      catalogue->Cat_Button = button;
      /* Initialisation de la liste des widgets fils */
      adbloc = NewEList ();
      catalogue->Cat_Entries = adbloc;
    }
  else
    {
      /* Mise a jour du menu existant */
      menu = catalogue->Cat_Widget;
      adbloc = catalogue->Cat_Entries;
      /* Si on a change de bouton on met a jour le widget avec args[0] */
      if (catalogue->Cat_Button != button)
	catalogue->Cat_Button = button;
      else
	button = catalogue->Cat_Button;
    }
  catalogue->Cat_Data = -1;
  catalogue->Cat_ParentWidget = parent;

#ifdef _WINDOWS
  if (parent != 0)
    WIN_AddFrameCatalogue (parent, catalogue);
#endif /* _WINDOWS */
  /* Cree les differentes entrees du menu */
  i = 0;
  index = 0;
  eindex = 0;
  ent = 2;
  if (text != NULL)
    while (i < number)
      {
	count = strlen (&text[index]);	/* Longueur de l'intitule */
	/* S'il n'y a plus d'intitule -> on arrete */
	if (count == 0)
	  {
	    i = number;
	    TtaError (ERR_invalid_parameter);
	    break;
	  }
	else
	  {
	    /* Faut-il changer de bloc d'entrees ? */
	    if (ent >= C_NUMBER)
	      {
		adbloc->E_Next = NewEList ();
		adbloc = adbloc->E_Next;
		ent = 0;
	      }
	    /* Recupere le type de l'entree */
	    adbloc->E_Type[ent] = text[index];
	    adbloc->E_Free[ent] = 'Y';
		/* Note l'accelerateur */
		if (equiv != NULL)
		  {
		    if (&equiv[eindex] != EOS)
		      strcpy (equiv_item, &equiv[eindex]); 
		    eindex += strlen (&equiv[eindex]) + 1;
		  }
		if (text[index] == 'T' || text[index] == 'B')
		  /*__________________________________________ Creation d'un bouton __*/
		  {
#ifdef _WINDOWS
		    /* reserve the first char to indicate selected status */
		    text[index] = ' ';
		    SendMessage (listBox, LB_INSERTSTRING, i, (LPARAM) &text[index]);
		    w = (ThotWidget) i;
#else /* _WINDOWS */
		    sprintf (menu_item, "%s", &text[index + 1]);
		    /* \t doesn't mean anything to gtk... to we align ourself*/
		    if (equiv_item && equiv_item[0] != EOS)
		      {
			strcat (menu_item, "  ");
			w = gtk_menu_item_new ();
			table = gtk_table_new (1, 3, FALSE);    
			gtk_container_add (GTK_CONTAINER (w), table);  
			wlabel = gtk_label_new(menu_item);
			/*(that's left-justified, right is 1.0, center is 0.5).*/
			/*gtk_label don't seem to like table cell...so*/
			gtk_misc_set_alignment(GTK_MISC(wlabel), 0.0 , 0); 
			gtk_table_attach_defaults (GTK_TABLE(table), wlabel, 0, 1, 0, 1);   
			gtk_widget_show (wlabel);
			gtk_label_set_justify(GTK_LABEL(wlabel), GTK_JUSTIFY_LEFT);
			
			wlabel = gtk_label_new(equiv_item);
			gtk_misc_set_alignment(GTK_MISC(wlabel), 1.0, 0); 
			gtk_table_attach_defaults (GTK_TABLE(table), wlabel, 2, 3, 0, 1);   
			gtk_widget_show (wlabel);
			gtk_widget_show (table);
			
		      }
		    else
		      w = gtk_list_item_new_with_label (menu_item);
		    
		    if (!first)
		      first = w;
		    
		    /* memorize the parent window */
		    gtk_object_set_data (GTK_OBJECT (w), 
					 "window", 
					 (gpointer) menu);
		    
		    /* for debugging, memorize the widget name */
		    gtk_object_set_data (GTK_OBJECT (w), 
					 "item", (gpointer) 
					 (TtaStrdup (menu_item)));


		    /* get the key press */
		    gtk_signal_connect (GTK_OBJECT (w), "key_press_event",
					GTK_SIGNAL_FUNC (scr_popup_key_press),
					(gpointer ) catalogue);

		    /* get the click */
		    
		    ConnectSignalGTK (GTK_OBJECT (w), 
				      "button-press-event",
				      GTK_SIGNAL_FUNC (scr_popup_button_press),
				      (gpointer) catalogue);
		    gtk_widget_show (w);
		    glist = g_list_append (glist, w);
#endif /* _WINDOWS */
		  }
		else
		  /*____________________________________ Une erreur de construction __*/
		  {
		    TtaDestroyDialogue (ref);
		    TtaError (ERR_invalid_parameter);	/* Type d'entree non defini */
		    return;
		  }

		/* add the entry to the catalog list */
		adbloc->E_ThotWidget[ent] = w;
		adbloc->E_Type[ent] = 'T';
		i++;
		ent++;
		index += count + 1;
	      }
	  }
#ifdef _WINDOWS
      /* remember the catalogue */
      SetProp (menu, "ref", (HANDLE) ref);      
#else /* _WINDOWS */
      if (menu && glist)
	{
	  gtklist = gtk_list_new ();
	  gtk_list_append_items (GTK_LIST (gtklist), glist);

	  /* allow multiple items to be selected */
	  if (multipleOptions)
	    gtk_list_set_selection_mode (GTK_LIST (gtklist), GTK_SELECTION_MULTIPLE);

	  /* store the value of the gtklist and the menu inside the popup object (we will
	   use them later in the popup callback) */
	  gtk_object_set_data (GTK_OBJECT (menu), 
			       "window", (gpointer) menu);
	  gtk_object_set_data (GTK_OBJECT (menu), 
			       "gtklist", (gpointer) gtklist);
	  /* We'll use enter notify events to figure out when to transfer
	   * the grab to the list
	   */
	  gtk_widget_set_events (gtklist, 
				 GDK_ALL_EVENTS_MASK);
	  
	  gtk_signal_connect (GTK_OBJECT (menu), "event",
				    GTK_SIGNAL_FUNC (listeventGTK), 
				    (gpointer)catalogue);

	  /* add the gtk list to the scr window */
	  gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scr_window), gtklist);
	  /* tie the scrolling of the list to the scrolled window */
	  gtk_container_set_focus_hadjustment (GTK_CONTAINER (gtklist),
					       gtk_scrolled_window_get_hadjustment
					       (GTK_SCROLLED_WINDOW (scr_window)));
	  gtk_container_set_focus_vadjustment (GTK_CONTAINER (gtklist),
					       gtk_scrolled_window_get_vadjustment
					       (GTK_SCROLLED_WINDOW (scr_window)));
	  /* give the focus to the gtklist */
	  gtk_window_set_focus (GTK_WINDOW (menu), GTK_WIDGET (first));

	  /* show everything */

/* show everything */
	  gtk_grab_add (menu);


	  GTK_LIST (gtklist)->drag_selection = TRUE;
	  gdk_pointer_grab (menu->window, TRUE,
			    GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK |
			    GDK_POINTER_MOTION_MASK,
			    NULL, NULL, GDK_CURRENT_TIME);
 	  gtk_widget_grab_focus (menu);
	  gdk_keyboard_grab (menu->window, TRUE, GDK_CURRENT_TIME);


	  gtk_widget_show (GTK_WIDGET (gtklist)); 
	 
	}
#endif /* _WINDOWS */
#else /* _WINDOWS || GTK*/
      /* this widget is only supported on Windows and GTK */
      TtaError (ERR_cannot_create_dialogue);
#endif /* _WINDOWS || GTK*/
}


/*----------------------------------------------------------------------
   AddInFormulary recherche une entree libre dans le formulaire  
   et cre'e e'ventuellement un nouveau bloc d'entre'es et un nouveau  
   Row-Column.                                                        
   Retourne :                                                         
   + index -> le nume'ro d'entre'e sur l'ensemble des blocs.          
   + entry -> l'index dans le bloc des entre'es concerne'.            
   + adbloc -> le bloc des entre'es concerne'.                        
  ----------------------------------------------------------------------*/
static ThotWidget AddInFormulary (struct Cat_Context *catalogue, int *index,
				  int *entry, struct E_List **adbloc)
{
   ThotWidget          row;
   ThotWidget          w;

#if !defined(_WINDOWS) && !defined(_GTK)
   Arg                 args[MAX_ARGS];
   int                 n;
#endif /* _WINDOWS && _GTK*/

   /* Il faut sauter la 1ere entree allouee a un Row-Column */
   *entry = 1;
   *index = 1;
   /* Le 1er bloc sert aux boutons du feuillet */
   *adbloc = catalogue->Cat_Entries->E_Next;
   /* Recupere le Row-Column racine du formulaire */
   w = (*adbloc)->E_ThotWidget[0];
#ifdef _WINDOWS
   row = GetParent (w);
#else  /* _WINDOWS */
#ifndef _GTK
   row = XtParent (w);
#else /* _GTK */
   row = GTK_WIDGET(w->parent);
#endif /* _GTK */
#endif /* _WINDOWS */
   /*** Recherche une entree libre dans le formulaire ***/
   while ((*adbloc)->E_ThotWidget[*entry] != 0)
     {
	/* Est-ce un widget de Row-Column (n fois le facteur de blocage) ? */
	if (*index % catalogue->Cat_FormPack == 0)
	   w = (*adbloc)->E_ThotWidget[*entry];
	(*entry)++;
	(*index)++;
	if (*entry >= C_NUMBER)
	  {
	     /* Il faut changer de bloc et enventuellement en creer un */
	     if ((*adbloc)->E_Next == NULL)
		(*adbloc)->E_Next = NewEList ();
	     *adbloc = (*adbloc)->E_Next;
	     *entry = 0;
	  }
     }
   /* Faut-il ajouter un nouveau Row-Column ? */
   if (*index % catalogue->Cat_FormPack == 0)
     {
#ifndef _WINDOWS
#ifndef _GTK
	n = 0;
	XtSetArg (args[n], XmNadjustLast, FALSE);
	n++;
	if (catalogue->Cat_in_lines)
	  {
	   XtSetArg (args[n], XmNorientation, XmHORIZONTAL);
	   n++;
	   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);
	   n++;
	  }
	else
	  {
	   XtSetArg (args[n], XmNorientation, XmVERTICAL);
	   n++;
	   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);
	   n++;
	  }
	XtSetArg (args[n], XmNbackground, BgMenu_Color);
	n++;
	XtSetArg (args[n], XmNspacing, 5);
	n++;
	w = XmCreateRowColumn (row, "Dialogue", args, n);
	XtManageChild (w);
#else /* _GTK */
	if (catalogue->Cat_in_lines)
	  w = gtk_hbox_new (FALSE, 5);
	else
	  w = gtk_vbox_new (FALSE, 5);
     	gtk_widget_show_all (GTK_WIDGET(w));
	gtk_box_pack_start (GTK_BOX(row), GTK_WIDGET(w), TRUE, TRUE, 0);
#endif /* _GTK */
#endif /* _WINDOWS */
	(*adbloc)->E_ThotWidget[*entry] = w;
	(*adbloc)->E_Free[*entry] = 'X';
	(*index)++;
	(*entry)++;
	if (*entry >= C_NUMBER)
	  {
	     /* Il faut changer de bloc et enventuellement en creer un */
	     if ((*adbloc)->E_Next == NULL)
		(*adbloc)->E_Next = NewEList ();
	     *adbloc = (*adbloc)->E_Next;
	     *entry = 0;
	  }
     }
   /* il faut mettre a jour l'index */
   *index += C_NUMBER;
   return (w);
}

#ifndef _WINDOWS
/*----------------------------------------------------------------------
   TtaNewIconMenu cre'e un sous-menu :
   The parameter ref donne la re'fe'rence pour l'application.         
   The parameter ref_parent identifie le formulaire pe`re.            
   Le parametre entry de'signe l'entre'e correspondante dans le menu  
   pe`re. The parameter title donne le titre du catalogue.            
   The parameter number indique le nombre d'entre'es dans le menu.    
   The parameter icons contient la liste des icones du catalogue.     
   Tout changement de se'lection dans le sous-menu est imme'diatement 
   signale' a` l'application.                                         
  ----------------------------------------------------------------------*/
void TtaNewIconMenu (int ref, int ref_parent, int entry, char *title,
		     int number, Pixmap * icons, ThotBool horizontal)
{
   int                 i;
   int                 ent;
   struct Cat_Context *catalogue;
   struct Cat_Context *parentCatalogue;
   struct E_List      *adbloc;
   ThotWidget          menu;
   ThotWidget          w;
   ThotWidget          row;
#ifndef _GTK
   int                 n;
   Arg                 args[MAX_ARGS];
   XmString            title_string;
#else /* _GTK */
   ThotWidget          tmpw;
#endif /* _GTK */

   if (ref == 0)
     {
	TtaError (ERR_invalid_reference);
	return;
     }
   catalogue = CatEntry (ref);
   menu = 0;
   if (catalogue == NULL)
      TtaError (ERR_cannot_create_dialogue);
   else if (catalogue->Cat_Widget != 0)
      TtaError (ERR_invalid_reference);
   else
     {
	catalogue->Cat_React = TRUE;
#ifndef _GTK
	title_string = 0;
#endif /* _GTK */
	parentCatalogue = CatEntry (ref_parent);
	/*__________________________________ Le catalogue parent n'existe pas __*/
	if (parentCatalogue == NULL)
	  {
	     TtaError (ERR_invalid_parent_dialogue);
	     return;
	  }
	else if (parentCatalogue->Cat_Widget == 0)
	  {
	     TtaError (ERR_invalid_parent_dialogue);
	     return;
	  }
	/*_________________________________________ Sous-menu d'un formulaire __*/
	else if (parentCatalogue->Cat_Type == CAT_FORM
		 || parentCatalogue->Cat_Type == CAT_SHEET
		 || parentCatalogue->Cat_Type == CAT_DIALOG)
	  {
	     w = AddInFormulary (parentCatalogue, &i, &ent, &adbloc);
#ifndef _GTK
	     /*** Cree un sous-menu d'un formulaire ***/
	     n = 0;
	     XtSetArg (args[n], XmNbackground, BgMenu_Color);
	     n++;
	     XtSetArg (args[n], XmNmarginWidth, 0);
	     n++;
	     XtSetArg (args[n], XmNmarginHeight, 0);
	     n++;
	     XtSetArg (args[n], XmNspacing, 0);
	     n++;
	     menu = XmCreateRowColumn (w, "Dialogue", args, n);
#else /* _GTK */
	     menu = gtk_vbox_new (FALSE, 0);
	     gtk_widget_show_all (menu);
	     gtk_container_add (GTK_CONTAINER(w), menu);	     
#endif /* _GTK */
	     catalogue->Cat_Ref = ref;
	     catalogue->Cat_Type = CAT_FMENU;
	     catalogue->Cat_Data = -1;
	     catalogue->Cat_Widget = menu;
	     catalogue->Cat_PtParent = parentCatalogue;
	     adbloc->E_ThotWidget[ent] = (ThotWidget) (catalogue);
	     adbloc->E_Free[ent] = 'N';
	     catalogue->Cat_EntryParent = i;
	     adbloc = NewEList ();
	     catalogue->Cat_Entries = adbloc;
	  }
	/*** Cree le titre du sous-menu ***/
	if (title != NULL)
	  {
#ifndef _GTK
	     n = 0;
	     title_string = XmStringCreateSimple (title);
	     XtSetArg (args[n], XmNlabelString, title_string);
	     n++;
	     XtSetArg (args[n], XmNfontList, DefaultFont);
	     n++;
	     XtSetArg (args[n], XmNmarginHeight, 0);
	     n++;
	     XtSetArg (args[n], XmNbackground, BgMenu_Color);
	     n++;
	     XtSetArg (args[n], XmNforeground, FgMenu_Color);
	     n++;
	     w = XmCreateLabel (menu, "Dialogue", args, n);
	     XtManageChild (w);
	     adbloc->E_ThotWidget[0] = w;
	     n = 0;
	     XtSetArg (args[n], XmNbackground, BgMenu_Color);
	     n++;
	     XtSetArg (args[n], XmNseparatorType, XmSHADOW_ETCHED_OUT);
	     n++;
	     w = XmCreateSeparator (menu, "Dialogue", args, n);
	     XtManageChild (w);
	     adbloc->E_ThotWidget[1] = w;
	     XmStringFree (title_string);
#else /* _GTK */
	     /* add a label */
	     w = gtk_label_new (title);
	     gtk_misc_set_alignment (GTK_MISC (w), 0.0, 0.5);
	     gtk_widget_show_all (w);
	     w->style->font = DefaultFont;
	     /*	     gtk_label_set_justify (GTK_LABEL (w), GTK_JUSTIFY_LEFT);*/
	     gtk_box_pack_start (GTK_BOX(menu), w, FALSE, FALSE, 0);
	     adbloc->E_ThotWidget[0] = w;
	     
	     /* add a separator */
	     w = gtk_hseparator_new ();
	     gtk_widget_show_all (w);
	     gtk_box_pack_start (GTK_BOX(menu), w, FALSE, FALSE, 0);
	     adbloc->E_ThotWidget[1] = w;
#endif /* _GTK */
	  }
#ifndef _GTK
	/* Cree un Row-Column d'icone dans le Row-Column du formulaire */
	n = 0;
	XtSetArg (args[n], XmNmarginWidth, 0);
	n++;
	XtSetArg (args[n], XmNmarginHeight, 0);
	n++;
	XtSetArg (args[n], XmNspacing, 0);
	n++;
	XtSetArg (args[n], XmNrowColumnType, XmMENU_BAR);
	n++;
	XtSetArg (args[n], XmNbackground, BgMenu_Color);
	n++;
	if (horizontal)
	   XtSetArg (args[n], XmNorientation, XmHORIZONTAL);
	else
	   XtSetArg (args[n], XmNorientation, XmVERTICAL);
	n++;
	row = XmCreateRowColumn (menu, "Dialogue", args, n);
	XtManageChild (row);

	/*** Cree les differentes entrees du sous-menu ***/
	n = 0;
	XtSetArg (args[n], XmNfontList, DefaultFont);
	n++;
	XtSetArg (args[n], XmNmarginWidth, 0);
	n++;
	XtSetArg (args[n], XmNmarginHeight, 0);
	n++;
	XtSetArg (args[n], XmNbackground, BgMenu_Color);
	n++;
	XtSetArg (args[n], XmNlabelType, XmPIXMAP);
	n++;
#else /* _GTK */
	if (horizontal)
	  row = gtk_hbox_new (FALSE, 0);
	else
	  row = gtk_vbox_new (FALSE, 0);
	gtk_widget_show_all (row);
	gtk_container_add (GTK_CONTAINER(menu), row);
#endif /* _GTK */
	i = 0;
	ent = 2;
	while (i < number)
	  {
	     /* Faut-il changer de bloc d'entrees ? */
	     if (ent >= C_NUMBER)
	       {
		  adbloc->E_Next = NewEList ();
		  adbloc = adbloc->E_Next;
		  ent = 0;
	       }
	     /* On ne traite que des entrees de type bouton */
	     adbloc->E_Type[ent] = 'B';
	     adbloc->E_Free[ent] = 'Y';
#ifndef _GTK
	     XtSetArg (args[n], XmNlabelPixmap, icons[i]);
	     w = XmCreateCascadeButton (row, "dialogue", args, n + 1);
	     XtManageChild (w);
	     XtAddCallback (w, XmNactivateCallback, (XtCallbackProc) CallRadio, catalogue);
#else /* _GTK */
	     tmpw = gtk_pixmap_new (((ThotIcon)icons[i])->pixmap, ((ThotIcon )icons[i])->mask);
	     w = gtk_button_new ();
	     gtk_container_add (GTK_CONTAINER (w), tmpw);
	     gtk_widget_show_all (w);
	     gtk_box_pack_start (GTK_BOX (row), w, FALSE, FALSE, 0);
	     /* Connecte the clicked acton to the button */
	     ConnectSignalGTK (GTK_OBJECT(w), "clicked",
			       GTK_SIGNAL_FUNC(CallRadio), (gpointer)catalogue);
#endif /* _GTK */
	     adbloc->E_ThotWidget[ent] = w;
	     i++;
	     ent++;
	  }
     }
}
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
   TtaNewSubmenu cre'e un sous-menu :                                 
   The parameter ref donne la re'fe'rence pour l'application.         
   The parameter ref_parent identifie le formulaire pe`re.            
   Le parametre entry de'signe l'entre'e correspondante dans le menu  
   pe`re. The parameter title donne le titre du catalogue.            
   The parameter number indique le nombre d'entre'es dans le menu.    
   The parameter text contient la liste des intitule's du catalogue.  
   Chaque intitule' commence par un caracte`re qui donne le type de   
   l'entre'e et se termine par un caracte`re de fin de chai^ne \0.    
   S'il n'est pas nul, le parame`tre equiv donne les acce'le'rateurs  
   des entre'es du menu.                                              
   Quand le parame`tre react est vrai, tout changement de se'lection  
   dans le sous-menu est imme'diatement signale' a` l'application.    
  ----------------------------------------------------------------------*/
void TtaNewSubmenu (int ref, int ref_parent, int entry, char *title,
		    int number, char *text, char* equiv, ThotBool react)
{
  ThotWidget          w;
  ThotWidget          row;
  ThotMenu            menu = NULL;
  struct Cat_Context *catalogue;
  struct Cat_Context *parentCatalogue;
  struct E_List      *adbloc;
  char                button = 'L';
  register int        count;
  register int        index;
  int                 eindex;
  int                 i;
  int                 ent;
  ThotBool            rebuilded;
#if defined (_WINDOWS) || defined (_GTK)
  char                menu_item [1024];
  char                equiv_item [255];
#else /* _WINDOWS || _GTK */
  Arg                 args[MAX_ARGS];
  XmString            title_string = NULL;
  int                 n = 0;
#endif /* _WINDOWS || _GTK */
#ifdef _GTK
  GtkWidget          *table;
  ThotWidget          wlabel;
  GSList             *GSListTmp = NULL;
  ThotWidget          tmpw;
  GtkStyle *current_style;
#endif /* _GTK */

#if defined (_WINDOWS) || defined (_GTK)
  equiv_item[0] = EOS;
#endif /* _WINDOWS || _GTK */
  if (ref == 0)
    {
      TtaError (ERR_invalid_reference);
      return;
    } 
  catalogue = CatEntry (ref);
  if (catalogue == NULL)
    TtaError (ERR_cannot_create_dialogue);
  else
    {
      catalogue->Cat_React = react;
      /* Faut-il detruire le catalogue precedent ? */
      rebuilded = FALSE;
      if (catalogue->Cat_Widget)
	{
	  if (catalogue->Cat_Type == CAT_MENU || catalogue->Cat_Type == CAT_FMENU)
	    {
	      DestContenuMenu (catalogue);	/* Modification du catalogue */
	      rebuilded = TRUE;
	    }
	  else
	    /* Modification du catalogue */
	    TtaDestroyDialogue (ref);
	}
      /*======================================> Recherche le catalogue parent */
      parentCatalogue = CatEntry (ref_parent);
      /*__________________________________ Le catalogue parent n'existe pas __*/
      if (parentCatalogue == NULL)
	{
	  TtaError (ERR_invalid_parent_dialogue);
	  return;
	}
      else if (parentCatalogue->Cat_Widget == 0)
	{
	  TtaError (ERR_invalid_parent_dialogue);
	  return;
	}
      else if (parentCatalogue->Cat_Type == CAT_FORM  || 
	       parentCatalogue->Cat_Type == CAT_SHEET || 
	       parentCatalogue->Cat_Type == CAT_DIALOG)
	{
	  /*____________________________________ Sous-menu d'un formulaire __*/
	  if (!rebuilded)
	    {
	      w = AddInFormulary (parentCatalogue, &i, &ent, &adbloc);
#ifdef _WINDOWS
	      menu = w;
#else  /* _WINDOWS */
#ifndef _GTK
	      /*** Cree un sous-menu d'un formulaire ***/
	      n = 0;
	      XtSetArg (args[n], XmNbackground, BgMenu_Color);
	      n++;
	      XtSetArg (args[n], XmNmarginWidth, 0);
	      n++;
	      XtSetArg (args[n], XmNmarginHeight, 0);
	      n++;
	      XtSetArg (args[n], XmNspacing, 0);
	      n++;
	      menu = XmCreateRowColumn (w, "Dialogue", args, n);
#else /* _GTK */
	      menu = gtk_vbox_new (FALSE, 0);
	      gtk_widget_show_all (menu);
	      gtk_container_add (GTK_CONTAINER(w), menu);
#endif /* _GTK */
#endif /* _WINDOWS */
	      catalogue->Cat_Ref = ref;
	      catalogue->Cat_Type = CAT_FMENU;
	      catalogue->Cat_Data = -1;
	      catalogue->Cat_Widget = menu;
	      catalogue->Cat_PtParent = parentCatalogue;
	      adbloc->E_ThotWidget[ent] = (ThotWidget) (catalogue);
	      adbloc->E_Free[ent] = 'N';
	      catalogue->Cat_EntryParent = i;
	      adbloc = NewEList ();
	      catalogue->Cat_Entries = adbloc;
	    }
	  else
	    {
	      /* Sinon on recupere le widget du menu */
	      menu = catalogue->Cat_Widget;
	      adbloc = catalogue->Cat_Entries;
	    } 
	  /*** Cree le titre du sous-menu ***/
	  if (title)
	    {
#ifdef _GTK
	      if (!rebuilded)
		{
		  /* add a label */
		  w = gtk_label_new (title);
		  gtk_misc_set_alignment (GTK_MISC (w), 0.0, 0.5);
		  gtk_widget_show_all (w);
		  w->style->font = DefaultFont;
		  gtk_label_set_justify (GTK_LABEL (w), GTK_JUSTIFY_LEFT);
		  gtk_widget_set_name (w, "Dialogue");
		  gtk_box_pack_start (GTK_BOX(menu), w, FALSE, FALSE, 0);
		  adbloc->E_ThotWidget[0] = w;
		  
		  /* add a separator */
		  w = gtk_hseparator_new ();
		  gtk_widget_show_all (w);
		  gtk_widget_set_name (w, "Dialogue");
		  gtk_box_pack_start (GTK_BOX(menu), w, FALSE, FALSE, 0);
		  adbloc->E_ThotWidget[1] = w;
		}
	      else if (adbloc->E_ThotWidget[0] != 0)
		/* update the title */
		gtk_label_set_text (GTK_LABEL(adbloc->E_ThotWidget[0]), title);
#else /* _GTK */
#ifdef _WINDOWS
	      if (!rebuilded)
		{
		  adbloc->E_ThotWidget[0] = (ThotWidget) 0;
		  adbloc->E_ThotWidget[1] = (ThotWidget) 0;
		} 
#else  /* _WINDOWS */
	      if (!rebuilded)
		{
		  n = 0;
		  title_string = XmStringCreateSimple (title);
		  XtSetArg (args[n], XmNlabelString, title_string);
		  n++;
		  XtSetArg (args[n], XmNfontList, DefaultFont);
		  n++;
		  XtSetArg (args[n], XmNmarginHeight, 0);
		  n++;
		  XtSetArg (args[n], XmNbackground, BgMenu_Color);
		  n++;
		  XtSetArg (args[n], XmNforeground, FgMenu_Color);
		  n++;
		  w = XmCreateLabel (menu, "Dialogue", args, n);
		  XtManageChild (w);
		  adbloc->E_ThotWidget[0] = w;
		  n = 0;
		  XtSetArg (args[n], XmNbackground, BgMenu_Color);
		  n++;
		  XtSetArg (args[n], XmNseparatorType, XmSHADOW_ETCHED_OUT);
		  n++;
		  w = XmCreateSeparator (menu, "Dialogue", args, n);
		  XtManageChild (w);
		  adbloc->E_ThotWidget[1] = w;
		} 
	      else if (adbloc->E_ThotWidget[0] != 0)
		XtSetValues (adbloc->E_ThotWidget[0], args, n);
	      if (!title_string)
		XmStringFree (title_string);
#endif /* _WINDOWS */
#endif /* _GTK */
	    }
	  if (!rebuilded)
	    {
#ifdef _GTK
	      /* create a new vbox */
	      row = gtk_vbox_new (FALSE, 1);
	      gtk_widget_show_all (GTK_WIDGET(row));
	      gtk_widget_set_name (GTK_WIDGET(row), "Dialogue");
	      gtk_box_pack_start (GTK_BOX(menu), GTK_WIDGET(row), FALSE, FALSE, 0);
#else /* _GTK */
#ifndef _WINDOWS
	      /* Cree un Row-Column de Radio dans le Row-Column du formulaire */
	      n = 0;
	      XtSetArg (args[n], XmNradioAlwaysOne, TRUE);
	      n++;
	      XtSetArg (args[n], XmNradioBehavior, TRUE);
	      n++;
	      XtSetArg (args[n], XmNmarginWidth, 0);
	      n++;
	      XtSetArg (args[n], XmNmarginHeight, 0);
	      n++;
	      XtSetArg (args[n], XmNspacing, 0);
	      n++;
	      XtSetArg (args[n], XmNbackground, BgMenu_Color);
	      n++;
	      row = XmCreateRowColumn (menu, "Dialogue", args, n);
	      XtManageChild (row);
#endif /* _WINDOWS */
#endif /* _GTK */
	    }
	  else
	    /* Sinon on recupere le widget parent des entrees */
	    row = catalogue->Cat_XtWParent;
#if !defined(_WINDOWS) && !defined(_GTK)
	  /*** Cree les differentes entrees du sous-menu ***/
	  n = 0;
	  XtSetArg (args[n], XmNindicatorType, XmONE_OF_MANY);
	  n++;
	  XtSetArg (args[n], XmNfontList, DefaultFont);
	  n++;
	  XtSetArg (args[n], XmNmarginWidth, 0);
	  n++;
	  XtSetArg (args[n], XmNmarginHeight, 0);
	  n++;
	  XtSetArg (args[n], XmNbackground, BgMenu_Color);
	  n++;
	  XtSetArg (args[n], XmNforeground, FgMenu_Color);
	  n++;
#endif /* _WINDOWS && _GTK */
	  i = 0;
	  index = 0;
	  eindex = 0;
	  ent = 2;
#ifdef _GTK
	  GSListTmp = NULL;
#endif
	  while (i < number)
	    {
	      count = strlen (&text[index]);	/* Longueur de l'intitule */
	      /* S'il n'y a plus d'intitule -> on arrete */
	      if (count == 0)
		i = number;
	      else
		{ /* Faut-il changer de bloc d'entrees ? */
		  if (ent >= C_NUMBER)
		    {
		      adbloc->E_Next = NewEList ();
		      adbloc = adbloc->E_Next;
		      ent = 0;
		    }
		  /* On ne traite que des entrees de type bouton */
		  adbloc->E_Type[ent] = 'B';
		  adbloc->E_Free[ent] = 'Y';
#ifdef _GTK
		  /* create a radiolist */
		  /* copy the style because every widget share the same style */
		  /* rem: GSListTmp regroupe the widget of the radiolist*/
		  w = gtk_radio_button_new (GSListTmp);
		  gtk_widget_show_all (GTK_WIDGET(w));
		  tmpw = gtk_label_new (&text[index + 1]);
		  gtk_misc_set_alignment (GTK_MISC (tmpw), 0.0, 0.5);
		  gtk_widget_show_all (tmpw);
		  current_style = gtk_style_copy(gtk_widget_get_style(tmpw));
		  current_style->font = DefaultFont;
		  gtk_widget_set_style(tmpw, current_style);
		  gtk_label_set_justify (GTK_LABEL (tmpw), GTK_JUSTIFY_LEFT);
		  gtk_container_add (GTK_CONTAINER(w), tmpw);
		  gtk_object_set_data (GTK_OBJECT(w), "Label", (gpointer)tmpw);
		  gtk_box_pack_start (GTK_BOX(row), GTK_WIDGET(w), FALSE, FALSE, 0);
		  ConnectSignalGTK (GTK_OBJECT(w), "toggled", GTK_SIGNAL_FUNC(CallRadio), (gpointer)catalogue);
		  /* add this element to the radio group */
		  GSListTmp = gtk_radio_button_group (GTK_RADIO_BUTTON (w));
		  adbloc->E_ThotWidget[ent] = w;
#else /* _GTK */
#ifdef _WINDOWS
		  WIN_AddFrameCatalogue (w, catalogue);
		  adbloc->E_ThotWidget[ent] = w;
#else  /* _WINDOWS */
		  w = XmCreateToggleButton (row, &text[index + 1], args, n);
		  XtManageChild (w);
		  XtAddCallback (w, XmNarmCallback, (XtCallbackProc) CallRadio, catalogue);
		  adbloc->E_ThotWidget[ent] = w;
#endif /* _WINDOWS */
#endif /* _GTK */
		  i++;
		  index += count + 1;
		  ent++;
		}
	    }
	}
      else
	{   
	  /*____________________________________________ Sous-menu d'un menu __*/
	  if (parentCatalogue->Cat_Type == CAT_POPUP || 
	      parentCatalogue->Cat_Type == CAT_PULL  || 
	      parentCatalogue->Cat_Type == CAT_MENU)
	    {
	      /* Faut-il reconstruire entierement le menu */
	      if (!rebuilded)
		{
		  /*=========> Recherche l'entree du menu parent corespondante */
		  adbloc = parentCatalogue->Cat_Entries;
		  ent = entry + 2;		/* decalage de 2 pour le widget titre */
		  while (ent >= C_NUMBER)
		    {
		      if (adbloc->E_Next == NULL)
			{
			  TtaError (ERR_invalid_parent_dialogue);
			  return;
			}
		      else
			adbloc = adbloc->E_Next;
		      ent -= C_NUMBER;
		    }
		  if (adbloc->E_Type[ent] == 'M' && adbloc->E_Free[ent] == 'Y')
		    {
		      /* Cree un sous-menu d'un menu */
		      w = parentCatalogue->Cat_Widget;
#ifdef _GTK
		      /* Cree un sous-menu d'un menu */
		      menu = gtk_menu_new ();
		      menu->style->font=DefaultFont;
#else /* _GTK */
#ifndef _WINDOWS
		      n = 0;
		      XtSetArg (args[n], XmNbackground, BgMenu_Color);
		      n++;
		      button = parentCatalogue->Cat_Button;
		      if (button == 'R')
			XtSetArg (args[n], XmNwhichButton, Button3);
		      else if (button == 'M')
			XtSetArg (args[n], XmNwhichButton, Button2);
		      else 
			XtSetArg (args[n], XmNwhichButton, Button1);
		      n++;
		      XtSetArg (args[n], XmNmarginWidth, 0);
		      n++;
		      XtSetArg (args[n], XmNmarginHeight, 0);
		      n++;
		      XtSetArg (args[n], XmNspacing, 0);
		      n++;
		      menu = XmCreatePulldownMenu (w, "Dialogue", args, n);
#endif /* _WINDOWS */
#endif /* _GTK */
		      catalogue->Cat_Button = button;
		      catalogue->Cat_Data = -1;
		      /* Memorise l'entree decalee de 2 pour le widget titre */
		      catalogue->Cat_EntryParent = entry + 2;
		      
		      /*** Relie le sous-menu au bouton du menu ***/
		      w = adbloc->E_ThotWidget[ent];
		      catalogue->Cat_Ref = ref;
		      catalogue->Cat_Type = CAT_MENU;
		      catalogue->Cat_PtParent = parentCatalogue;
		      catalogue->Cat_Widget = menu;
#ifdef _WINDOWS
		      WIN_AddFrameCatalogue (FrMainRef[currentFrame], catalogue);
		      if (!IsMenu (catalogue->Cat_Widget))
			  catalogue->Cat_Widget = w;
#else  /* _WINDOWS */
#ifndef _GTK
		      n = 0;
		      XtSetArg (args[n], XmNsubMenuId, menu);
		      n++;
		      XtSetValues (w, args, n);
		      XtManageChild (w);
#else /* _GTK */
		      /* assign the submenu to the menu bar*/
		      gtk_widget_show_all (w);
		      gtk_menu_item_set_submenu (GTK_MENU_ITEM (w), menu);
#endif /* _GTK */
#endif /* _WINDOWS */
		      adbloc->E_Free[ent] = 'N';
		      adbloc = NewEList ();
		      catalogue->Cat_Entries = adbloc;
		    }
		  else
		    {
		      TtaError (ERR_invalid_parameter);
		      return;
		    } 
		}
	      else
		{ /* On recupere le widget du menu */
		  menu = catalogue->Cat_Widget;
		  adbloc = catalogue->Cat_Entries;
		}
	    }
	  else
	    {
	      /*_________________________________________ Sous-menu non valide __*/
	      TtaError (ERR_invalid_parameter);
	      return;
	    }
	  /*** Cree le titre du sous-menu ***/
	  if (title)
	    {
#ifdef _WINDOWS
	      if (!rebuilded)
		{
		  adbloc->E_ThotWidget[0] = w;
		  adbloc->E_ThotWidget[1] = w;
		}
#else /* _WINDOWS */
#ifndef _GTK
	      if (!rebuilded)
		{
		  n = 0;
		  title_string = XmStringCreateSimple (title);
		  XtSetArg (args[n], XmNlabelString, title_string);
		  n++;
		  XtSetArg (args[n], XmNfontList, DefaultFont);
		  n++;
		  XtSetArg (args[n], XmNmarginHeight, 0);
		  n++;
		  XtSetArg (args[n], XmNbackground, BgMenu_Color);
		  n++;
		  XtSetArg (args[n], XmNforeground, FgMenu_Color);
		  n++;
		  w = XmCreateLabel (menu, "Dialogue", args, n);
		  XtManageChild (w);
		  adbloc->E_ThotWidget[0] = w;
		  n = 0;
		  XtSetArg (args[n], XmNbackground, BgMenu_Color);
		  n++;
		  XtSetArg (args[n], XmNseparatorType, XmSHADOW_ETCHED_OUT);
		  n++;
		  w = XmCreateSeparator (menu, "Dialogue", args, n);
		  XtManageChild (w);
		  adbloc->E_ThotWidget[1] = w;
		}
	      else if (adbloc->E_ThotWidget[0] != 0)
		XtSetValues (adbloc->E_ThotWidget[0], args, n);
	      XmStringFree (title_string);
#else /* _GTK */
	      if (!rebuilded)
		{
		  w = gtk_menu_item_new_with_label (title);
		  gtk_widget_show_all (w);
		  gtk_box_pack_start (GTK_BOX(menu), w, FALSE, FALSE, 0);
		  adbloc->E_ThotWidget[0] = w;
		  w = gtk_hseparator_new ();
		  gtk_widget_show_all (w);
		  gtk_box_pack_start (GTK_BOX(menu), w, FALSE, FALSE, 0);
		  adbloc->E_ThotWidget[1] = w;
		}
#endif /* _GTK */
#endif /* _WINDOWS */
	    } 
	  /* Cree les differentes entrees du sous-menu */
#if !defined(_WINDOWS) && !defined(_GTK)
	  n = 0;
	  XtSetArg (args[n], XmNfontList, DefaultFont);
	  n++;
	  XtSetArg (args[n], XmNmarginWidth, 0);
	  n++;
	  XtSetArg (args[n], XmNmarginHeight, 0);
	  n++;
	  XtSetArg (args[n], XmNbackground, BgMenu_Color);
	  n++;
	  XtSetArg (args[n], XmNforeground, FgMenu_Color);
	  n++;
	  if (equiv)
	    n++;
#endif /* _WINDOWS  && GTK */
	  i = 0;
	  index = 0;
	  eindex = 0;
	  ent = 2;
	  while (i < number)
	    {
	      count = strlen (&text[index]);	/* Longueur de l'intitule */
	      /* S'il n'y a plus d'intitule -> on arrete */
	      if (count == 0)
		{
		  i = number;
		  TtaError (ERR_invalid_parameter);
		  return;
		}
	      else
		{ /* Faut-il changer de bloc d'entrees ? */
		  if (ent >= C_NUMBER)
		    {
		      adbloc->E_Next = NewEList ();
		      adbloc = adbloc->E_Next;
		      ent = 0;
		    }
		  /* Recupere le type de l'entree */
		  adbloc->E_Type[ent] = text[index];
		  adbloc->E_Free[ent] = 'Y';
		  /* Note l'accelerateur */
		  if (equiv)
		    {
#ifdef _WINDOWS
		      if (&equiv[eindex] != EOS)
			strcpy (equiv_item, &equiv[eindex]); 
#else  /* _WINDOWS */
#ifndef _GTK
		      title_string = XmStringCreate (&equiv[eindex], XmSTRING_DEFAULT_CHARSET);
		      XtSetArg (args[n - 1], XmNacceleratorText, title_string);
#else /* _GTK */
		      if (&equiv[eindex] != EOS)
			strcpy (equiv_item, &equiv[eindex]); 
#endif /* _GTK */
#endif /* _WINDOWS */
		      eindex += strlen (&equiv[eindex]) + 1;
		    }
		  if (text[index] == 'B')
		    {
		      /*______________________________________ Creation d'un bouton __*/
#ifdef _WINDOWS
		      if (equiv_item && equiv_item[0] != EOS)
			{
			  sprintf (menu_item, "%s\t%s", &text[index + 1], equiv_item);
			  AppendMenu (w, MF_STRING, ref + i, menu_item);
			  equiv_item[0] = EOS;
			}
		      else
			AppendMenu (w, MF_STRING, ref + i, &text[index + 1]);
		      adbloc->E_ThotWidget[ent] = (ThotWidget) i;
#else  /* _WINDOWS */
#ifdef _GTK
		      sprintf (menu_item, "%s", &text[index + 1]);
		      /* \t doesn't mean anything to gtk... to we align ourself*/
		      if (equiv_item && equiv_item[0] != EOS)
			{
			  strcat (menu_item, "  ");
			  w = gtk_menu_item_new ();
			  table = gtk_table_new (1, 3, FALSE);    
			  gtk_container_add (GTK_CONTAINER (w), table);  
			  wlabel = gtk_label_new(menu_item);
			  /*(that's left-justified, right is 1.0, center is 0.5).*/
			  /*gtk_label don't seem to like table cell...so*/
			  gtk_misc_set_alignment(GTK_MISC(wlabel), 0.0 , 0); 
			  gtk_table_attach_defaults (GTK_TABLE(table), wlabel, 0, 1, 0, 1);   
			  gtk_widget_show (wlabel);
			  gtk_label_set_justify(GTK_LABEL(wlabel), GTK_JUSTIFY_LEFT);
			  
			  wlabel = gtk_label_new(equiv_item);
			  gtk_misc_set_alignment(GTK_MISC(wlabel), 1.0, 0); 
			  gtk_table_attach_defaults (GTK_TABLE(table), wlabel, 2, 3, 0, 1);   
			  gtk_widget_show (wlabel);
			  gtk_widget_show (table);
			}
		      else
			w = gtk_menu_item_new_with_label (menu_item);
		      /*w->style = gtk_style_copy (w->style);
			w->style->font=DefaultFont;*/
		      gtk_widget_show_all (w);
		      gtk_menu_append (GTK_MENU (menu),w);
		      ConnectSignalGTK (GTK_OBJECT(w), "activate",
					GTK_SIGNAL_FUNC (CallMenuGTK), (gpointer)catalogue);
		      adbloc->E_ThotWidget[ent] = w;
#else /* _GTK */
		      w = XmCreatePushButton (menu, &text[index + 1], args, n);
		      XtManageChild (w);
		      adbloc->E_ThotWidget[ent] = w;
		      XtAddCallback (w, XmNactivateCallback, (XtCallbackProc) CallMenu, catalogue);
#endif /* _GTK */
#endif /* _WINDOWS */
		    }
		  else if (text[index] == 'T')
		    {
		      /*________________________________ Creation d'un toggle __*/
#ifdef _WINDOWS
		      if (equiv_item && equiv_item[0] != EOS)
			{
			  sprintf (menu_item, "%s\t%s", &text[index + 1], equiv_item);
			  AppendMenu (w, MF_STRING | MF_UNCHECKED, ref + i, menu_item);
			  equiv_item[0] = EOS;
			}
		      else 
			AppendMenu (w, MF_STRING | MF_UNCHECKED, ref + i, &text[index + 1]);
		      adbloc->E_ThotWidget[ent] = (ThotWidget) i;
		      adbloc->E_ThotWidget[ent + 1] = (ThotWidget) -1;
		      /* WIN_AddFrameCatalogue (FrMainRef [currentFrame], catalogue); */
#else  /* _WINDOWS */
#ifdef _GTK
		      /* create a check menu */
		      /* \t doesn't mean anything to gtk... to we align ourself*/
		      sprintf (menu_item, "%s", &text[index + 1]);
		      if (equiv_item && equiv_item[0] != EOS)
			{
			  strcat (menu_item, "  ");
			  w = gtk_check_menu_item_new ();
			  table = gtk_table_new (1, 3, FALSE);    
			  gtk_container_add (GTK_CONTAINER (w), table);  
			  wlabel = gtk_label_new(menu_item);
			  /*(that's left-justified, right is 1.0, center is 0.5).*/
			  /*gtk_label don't seem to like table cell...so*/
			  gtk_misc_set_alignment(GTK_MISC(wlabel), 0.0 , 0); 
			  gtk_table_attach_defaults (GTK_TABLE(table), wlabel, 0, 1, 0, 1);   
			  gtk_widget_show (wlabel);
			  gtk_label_set_justify(GTK_LABEL(wlabel), GTK_JUSTIFY_LEFT);
			  wlabel = gtk_label_new(equiv_item);
			  gtk_misc_set_alignment(GTK_MISC(wlabel), 1.0, 0); 
			  gtk_table_attach_defaults (GTK_TABLE(table), wlabel, 2, 3, 0, 1);   
			  gtk_widget_show (wlabel);
			  gtk_widget_show (table);
			}
		      else
			w = gtk_check_menu_item_new_with_label (menu_item);
		      gtk_widget_show_all (w);
		      gtk_menu_append (GTK_MENU (menu), w);
		      gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (w), FALSE);
		      adbloc->E_ThotWidget[ent] = w;
		      ConnectSignalGTK (GTK_OBJECT(w), "activate",
					GTK_SIGNAL_FUNC (CallMenuGTK), (gpointer)catalogue);		      
#else /* _GTK */
		      /* un toggle a faux */
		      XtSetArg (args[n], XmNvisibleWhenOff, TRUE);
		      XtSetArg (args[n + 1], XmNselectColor, BgMenu_Color);
		      w = XmCreateToggleButton (menu, &text[index + 1], args, n + 2);
		      XtManageChild (w);
		      adbloc->E_ThotWidget[ent] = w;
		      XtAddCallback (w, XmNvalueChangedCallback, (XtCallbackProc) CallMenu, catalogue);
#endif /* _GTK */
#endif /* _WINDOWS */
		    }
		  else if (text[index] == 'M')
		    {
		      /*_________________________________ Appel d'un sous-menu __*/
#ifndef _WINDOWS
#ifdef _GTK
		      w = gtk_menu_item_new_with_label (&text[index + 1]);
		      gtk_widget_show_all (w);
		      current_style = gtk_style_copy(gtk_widget_get_style(w));
		      current_style->font = DefaultFont;
		      gtk_widget_set_style(w, current_style);

		      adbloc->E_ThotWidget[ent] = w;
#else /* _GTK */
		      w = XmCreateCascadeButton (menu, &text[index + 1], args, n);
		      adbloc->E_ThotWidget[ent] = w;
#endif /* _GTK */
#endif /* _WINDOWS */
		    }
		  else if (text[index] == 'S')
		    {
		      /*_____________________________ Creation d'un separateur __*/
#ifdef _WINDOWS
		      AppendMenu (w, MF_SEPARATOR, 0, NULL);
		      adbloc->E_ThotWidget[ent] = (ThotWidget) 0;
#else  /* _WINDOWS */
#ifdef _GTK
		      w =  gtk_menu_item_new ();
		      current_style = gtk_style_copy(gtk_widget_get_style(w));
		      current_style->font = DefaultFont;
		      gtk_widget_set_style(w, current_style);
		      gtk_widget_show_all (w);
		      gtk_menu_append (GTK_MENU (menu),w);
		      adbloc->E_ThotWidget[ent] = w;
#else /* _GTK */
		      XtSetArg (args[n], XmNseparatorType, XmSINGLE_DASHED_LINE);
		      w = XmCreateSeparator (menu, "Dialogue", args, n + 1);
		      XtManageChild (w);
		      adbloc->E_ThotWidget[ent] = w;
#endif /* _GTK */
#endif /* _WINDOWS */
		    }
		  else
		    {
		      /*___________________________ Une erreur de construction __*/
		      TtaDestroyDialogue (ref);
		      TtaError (ERR_invalid_parameter);	/* Type d'entree non defini */
		      return;
		    } 
		  
		  /* free the string */
#ifndef _WINDOWS
#ifndef _GTK
		  if (equiv != NULL)
		    XmStringFree (title_string);
#endif /* _GTK */
#endif /* _WINDOWS */
		  i++;
		  index += count + 1;
		  ent++;
		}  
	    }
	}
     }
}

#ifndef _WINDOWS
/*----------------------------------------------------------------------
   TtaSetMenuForm fixe la selection dans un sous-menu de formulaire : 
   The parameter ref donne la re'fe'rence du catalogue.               
   The parameter val de'signe l'entre'e se'lectionne'e.               
  ----------------------------------------------------------------------*/
void TtaSetMenuForm (int ref, int val)
{
#ifndef _WINDOWS
   register int        i;
   register int        ent;
   ThotBool            visible;
   struct E_List      *adbloc;
#ifndef _GTK
   register int        n;
   Arg                 args[MAX_ARGS];
#else
   guint               id_toggled;
#endif /* _GTK */
#endif /* _WINDOWS */
   struct Cat_Context *catalogue;

   catalogue = CatEntry (ref);
   if (catalogue == NULL)
      TtaError (ERR_invalid_reference);
   else if (catalogue->Cat_Widget == 0)
      TtaError (ERR_invalid_reference);
   else
     {
	/* Si ce n'est pas un sous-menu de formulaire */
	if (catalogue->Cat_Type != CAT_FMENU)
	  {
	     TtaError (ERR_invalid_reference);
	     return;
	  }

	/* Est-ce que le sous-menu est actuellement affiche */
#ifndef _WINDOWS
#ifndef _GTK
	if (XtIsManaged (catalogue->Cat_Widget))
	   visible = TRUE;
	else
	  {
	     visible = FALSE;
	     XtManageChild (catalogue->Cat_Widget);
	  }
#else /* _GTK */
	if (GTK_WIDGET_VISIBLE (catalogue->Cat_Widget))
	   visible = TRUE;
	else
	  {
	     visible = FALSE;
	     gtk_widget_show_all (catalogue->Cat_Widget);
	  }
#endif /* _GTK */
	/* Positionnement de la valeur de chaque entree */
	adbloc = catalogue->Cat_Entries;
	ent = 0;
	i = 2;			/* decalage de 2 pour le widget titre */
	while (adbloc != NULL)
	  {
	     while (i < C_NUMBER)
	       {
		  if (adbloc->E_ThotWidget[i] == 0)
		     i = C_NUMBER;
		  else if (ent == val)
		    {
#ifndef _GTK
		       n = 0;
		       XtSetArg (args[n], XmNset, TRUE);
		       n++;
		       XtSetValues (adbloc->E_ThotWidget[i], args, n);
#else /* _GTK */

		       id_toggled  = (guint) gtk_object_get_data (GTK_OBJECT (adbloc->E_ThotWidget[i]),
							  "toggled");
		       gtk_signal_handler_block (GTK_OBJECT(adbloc->E_ThotWidget[i]), 
						 id_toggled);
		       gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (adbloc->E_ThotWidget[i]), 
						     TRUE);
#endif /* _GTK */
		    }
		  else
		    {
#ifndef _GTK
		       n = 0;
		       XtSetArg (args[n], XmNset, FALSE);
		       n++;
		       XtSetValues (adbloc->E_ThotWidget[i], args, n);
#else /* _GTK */  

		       id_toggled  = (guint) gtk_object_get_data (GTK_OBJECT (adbloc->E_ThotWidget[i]),
							  "toggled");
		       gtk_signal_handler_block (GTK_OBJECT(adbloc->E_ThotWidget[i]), 
						 id_toggled);
		       gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (adbloc->E_ThotWidget[i]), 
						     FALSE);
#endif /* _GTK */
		    }
		  i++;
		  ent++;
	       }
	     /* Passe au bloc suivant */
	     adbloc = adbloc->E_Next;
	     i = 0;
	  }			/*while */
#ifndef _GTK
	if (!visible)
	   XtUnmanageChild (catalogue->Cat_Widget);
#else /* _GTK */
	adbloc = catalogue->Cat_Entries;
	ent = 0;
	i = 2;			/* decalage de 2 pour le widget titre */
	while (adbloc != NULL)
	  {
	    while (i < C_NUMBER)
	      {
		if (adbloc->E_ThotWidget[i] == 0)
		  i = C_NUMBER;
		else
		  {
		    id_toggled  = (guint) gtk_object_get_data (GTK_OBJECT (adbloc->E_ThotWidget[i]),
							       "toggled");
		    gtk_signal_handler_unblock (GTK_OBJECT(adbloc->E_ThotWidget[i]), 
						id_toggled); 
		  }
		  i++;
		  ent++;
	       }
	     /* Passe au bloc suivant */
	     adbloc = adbloc->E_Next;
	     i = 0;
	  }		
	if (!visible)
	  gtk_widget_hide (catalogue->Cat_Widget);
#endif /* _GTK */
#endif /* _WINDOWS */
	/* La selection de l'utilisateur est desactivee */
	catalogue->Cat_Data = val;
     }
}

/*----------------------------------------------------------------------
   TtaNewToggleMenu cre'e un sous-menu a` choix multiples :           
   The parameter ref donne la re'fe'rence pour l'application.         
   The parameter ref_parent identifie le formulaire pe`re.            
   The parameter title donne le titre du catalogue.                   
   The parameter number indique le nombre d'entre'es dans le menu.    
   The parameter text contient la liste des intitule's du catalogue.  
   Chaque intitule' commence par un caracte`re qui donne le type de   
   l'entre'e et se termine par un caracte`re de fin de chai^ne \0.    
   S'il n'est pas nul, le parame`tre equiv donne les acce'le'rateurs  
   des entre'es du menu.                                              
   Quand le parame`tre react est vrai, tout changement de se'lection  
   dans le sous-menu est imme'diatement signale' a` l'application.    
  ----------------------------------------------------------------------*/
void TtaNewToggleMenu (int ref, int ref_parent, char *title, int number,
		       char *text, char *equiv, ThotBool react)
{
   register int        count;
   register int        index;
   int                 eindex;
   int                 i;
   int                 ent;
   ThotBool            rebuilded;
   struct Cat_Context *catalogue;
   struct Cat_Context *parentCatalogue;
   struct E_List      *adbloc;
#ifndef _GTK
   Arg                 args[MAX_ARGS];
   XmString            title_string;
   int                 n;
#endif /* _GTK */
   ThotWidget          menu;
   ThotWidget          w;
   ThotWidget          row;

   if (ref == 0)
     {
	TtaError (ERR_invalid_reference);
	return;
     }
   catalogue = CatEntry (ref);
   if (catalogue == NULL)
      TtaError (ERR_cannot_create_dialogue);
   else
     {
	catalogue->Cat_React = react;
#ifndef _GTK
	title_string = 0;
#endif /* _GTK */
	/* Faut-il detruire le catalogue precedent ? */
	rebuilded = FALSE;
	if (catalogue->Cat_Widget != 0)
	  {
	   if (catalogue->Cat_Type == CAT_TMENU)
	     {
		DestContenuMenu (catalogue);	/* Modification du catalogue */
		rebuilded = TRUE;
	     }
	   else
	      /* Modification du catalogue */
	      TtaDestroyDialogue (ref);
	  }
	/*======================================> Recherche le catalogue parent */
	if (!rebuilded)
	   parentCatalogue = CatEntry (ref_parent);
	else
	   parentCatalogue = catalogue->Cat_PtParent;
	/*__________________________________ Le catalogue parent n'existe pas __*/
	if (parentCatalogue == NULL)
	   TtaError (ERR_invalid_parent_dialogue);
	else if (parentCatalogue->Cat_Widget == 0)
	   TtaError (ERR_invalid_parent_dialogue);
	/*_________________________________________ Sous-menu d'un formulaire __*/
	else if (parentCatalogue->Cat_Type == CAT_FORM
		 || parentCatalogue->Cat_Type == CAT_SHEET
		 || parentCatalogue->Cat_Type == CAT_DIALOG)
	  {
	     if (!rebuilded)
	       {
		  w = AddInFormulary (parentCatalogue, &i, &ent, &adbloc);
#ifndef _GTK
		  /* Cree un sous-menu d'un formulaire */
		  n = 0;
		  XtSetArg (args[n], XmNbackground, BgMenu_Color);
		  n++;
		  XtSetArg (args[n], XmNmarginWidth, 0);
		  n++;
		  XtSetArg (args[n], XmNmarginHeight, 0);
		  n++;
		  XtSetArg (args[n], XmNspacing, 0);
		  n++;
		  menu = XmCreateRowColumn (w, "Dialogue", args, n);
#else /* _GTK */
		  /* create a new vbox to contain the toggle buttons */
		  menu = gtk_vbox_new (FALSE, 1);
		  gtk_widget_show_all (menu);
		  gtk_widget_set_name (menu, "Dialogue");
		  gtk_container_add (GTK_CONTAINER(w), menu);
#endif /* _GTK */
		  catalogue->Cat_Ref = ref;
		  catalogue->Cat_Type = CAT_TMENU;
		  catalogue->Cat_Widget = menu;
		  catalogue->Cat_PtParent = parentCatalogue;
		  adbloc->E_ThotWidget[ent] = (ThotWidget) catalogue;
		  adbloc->E_Free[ent] = 'N';
		  catalogue->Cat_EntryParent = i;
		  /* prepare le nouveau catalogue */
		  adbloc = NewEList ();
		  catalogue->Cat_Entries = adbloc;
	       }
	     else
	       {
		  menu = catalogue->Cat_Widget;
		  adbloc = catalogue->Cat_Entries;
	       }
	     /*** Cree le titre du sous-menu ***/
	     if (title != NULL)
	       {
#ifndef _GTK
		  n = 0;
		  title_string = XmStringCreateSimple (title);
		  XtSetArg (args[n], XmNlabelString, title_string);
		  n++;
		  if (!rebuilded)
		    {
		       XtSetArg (args[n], XmNfontList, DefaultFont);
		       n++;
		       XtSetArg (args[n], XmNmarginHeight, 0);
		       n++;
		       XtSetArg (args[n], XmNbackground, BgMenu_Color);
		       n++;
		       XtSetArg (args[n], XmNforeground, FgMenu_Color);
		       n++;
		       w = XmCreateLabel (menu, "Dialogue", args, n);
		       XtManageChild (w);
		       adbloc->E_ThotWidget[0] = w;
		       n = 0;
		       XtSetArg (args[n], XmNbackground, BgMenu_Color);
		       n++;
		       XtSetArg (args[n], XmNseparatorType, XmSHADOW_ETCHED_OUT);
		       n++;
		       w = XmCreateSeparator (menu, "Dialogue", args, n);
		       XtManageChild (w);
		       adbloc->E_ThotWidget[1] = w;
		    }
		  else if (adbloc->E_ThotWidget[0] != 0)
		     XtSetValues (adbloc->E_ThotWidget[0], args, n);
		  XmStringFree (title_string);
#else /* _GTK */
		  if (!rebuilded)
		    {
		      /* new label for the title */
		      w = gtk_label_new (title);
		      gtk_misc_set_alignment (GTK_MISC (w), 0.0, 0.5);
		      gtk_widget_show_all (w);
		      gtk_label_set_justify (GTK_LABEL (w), GTK_JUSTIFY_LEFT);
		      gtk_widget_set_name (w, "Dialogue");
		      gtk_box_pack_start (GTK_BOX(menu), w, FALSE, FALSE, 0);
		      adbloc->E_ThotWidget[0] = w;
		      
		      /* new separator */
		      w = gtk_hseparator_new ();
		      gtk_widget_show_all (w);
		      gtk_widget_set_name (w, "Dialogue");
		      gtk_box_pack_start (GTK_BOX(menu), w, FALSE, FALSE, 0);
		      adbloc->E_ThotWidget[1] = w;
		    }
		  else if (adbloc->E_ThotWidget[0] != 0)
		    gtk_label_set_text (GTK_LABEL(adbloc->E_ThotWidget[0]), title);
#endif /* _GTK */
	       }
	     if (!rebuilded)
	       {
#ifndef _GTK
		  /* Cree un Row-Column de Toggle dans le Row-Column du formulaire */
		  n = 0;
		  XtSetArg (args[n], XmNradioAlwaysOne, FALSE);
		  n++;
		  XtSetArg (args[n], XmNmarginWidth, 0);
		  n++;
		  XtSetArg (args[n], XmNmarginHeight, 0);
		  n++;
		  XtSetArg (args[n], XmNspacing, 0);
		  n++;
		  XtSetArg (args[n], XmNbackground, BgMenu_Color);
		  n++;
		  row = XmCreateRowColumn (menu, "Dialogue", args, n);
		  XtManageChild (row);
#else /* _GTK */
		  /* create a new vbox for the list */
		  row = gtk_vbox_new (FALSE, 1);
		  gtk_widget_set_name (GTK_WIDGET(row), "Dialogue");
		  gtk_widget_show_all (GTK_WIDGET(row));
		  gtk_box_pack_start (GTK_BOX(menu), GTK_WIDGET(row),  FALSE, FALSE, 0);
#endif /* _GTK */
	       }
	     else
	       /* Sinon on recupere le widget parent des entrees */
	       row = catalogue->Cat_XtWParent;
	     
	     /* note le nombre d'entrees du toggle */
	     catalogue->Cat_Data = number;	/* recouvre Cat_XtWParent */
#ifndef _GTK
	     /* Cree les differentes entrees du sous-menu */
	     n = 0;
	     XtSetArg (args[n], XmNindicatorType, XmN_OF_MANY);
	     n++;
	     XtSetArg (args[n], XmNfontList, DefaultFont);
	     n++;
	     XtSetArg (args[n], XmNmarginWidth, 0);
	     n++;
	     XtSetArg (args[n], XmNmarginHeight, 0);
	     n++;
	     XtSetArg (args[n], XmNbackground, BgMenu_Color);
	     n++;
	     XtSetArg (args[n], XmNforeground, FgMenu_Color);
	     n++;
	     if (equiv != NULL)
	       {
		  XtSetArg (args[n + 1], XmNacceleratorText, title_string);
		  n++;
	       }
#endif /* _GTK */
	     i = 0;
	     index = 0;
	     eindex = 0;
	     ent = 2;		/* decalage de 2 pour le widget titre */
	     while (i < number)
	       {
		  count = strlen (&text[index]);	/* Longueur de l'intitule */
		  /* S'il n'y a plus d'intitule -> on arrete */
		  if (count == 0)
		    {
		       number = i;

		       /* note le nombre d'entrees du toggle */
		       catalogue->Cat_Data = number;
		       TtaError (ERR_invalid_parameter);
		       return;
		    }
		  else
		    {
		       /* Faut-il changer de bloc d'entrees ? */
		       if (ent >= C_NUMBER)
			 {
			    adbloc->E_Next = NewEList ();
			    adbloc = adbloc->E_Next;
			    ent = 0;
			 }
		       /* E_Free est utilise pour marquer les entrees selectionnes */
		       /* Intialement les entrees sont non selectionnees */
		       adbloc->E_Free[ent] = 'N';

		       /* Note l'accelerateur */
		       if (equiv != NULL)
			 {
#ifndef _GTK
			   title_string = XmStringCreate (&equiv[eindex], XmSTRING_DEFAULT_CHARSET);
#endif /* _GTK */
			   eindex += strlen (&equiv[eindex]) + 1;
			 }
		       /* On accepte toggles, boutons et separateurs */
		       if (text[index] == 'B' || text[index] == 'T')
			 /*____________________________________ Creation d'un bouton __*/
			 {
			   adbloc->E_Type[ent] = 'B';
#ifndef _GTK
			   w = XmCreateToggleButton (row, &text[index + 1], args, n);
		       
			   XtManageChild (w);
			   XtAddCallback (w, XmNarmCallback, (XtCallbackProc) CallToggle, catalogue);
#else /* _GTK */
			   /* add a check button to the list */
			   w = gtk_check_button_new_with_label (&text[index + 1]);
			   gtk_widget_show_all (GTK_WIDGET(w));
			   gtk_box_pack_start (GTK_BOX(row), GTK_WIDGET(w), FALSE, FALSE, 0);
			   ConnectSignalGTK (GTK_OBJECT(w), 
					     "toggled", 
					     GTK_SIGNAL_FUNC (CallToggle), 
					     (gpointer)catalogue);
#endif /* _GTK */
			   adbloc->E_ThotWidget[ent] = w;
			 }
		       else if (text[index] == 'S')
			 /*_______________________________ Creation d'un separateur __*/
			 {
			   adbloc->E_Type[ent] = 'S';
#ifndef _GTK
			    XtSetArg (args[n], XmNseparatorType, XmSINGLE_DASHED_LINE);
			    w = XmCreateSeparator (row, "Dialogue", args, n+1);
			    XtManageChild (w);
#else /* _GTK */
			    /* add a separator to the list */
			    w = gtk_hseparator_new ();
			    gtk_widget_show_all (w);
			    gtk_box_pack_start (GTK_BOX(row), w, FALSE, FALSE, 0);
#endif /* _GTK */
			    adbloc->E_ThotWidget[ent] = w;
			 }
#ifndef _GTK
		       /* liberation de la string */
		       if (equiv != NULL)
			  XmStringFree (title_string);
#endif /* _GTK */
		       i++;
		       index += count + 1;
		       ent++;
		    }
	       }
	  }
	else
	   TtaError (ERR_invalid_parameter);
     }
}
#endif /* _WINDOWS */


/*----------------------------------------------------------------------
   TtaSetToggleMenu fixe la selection dans un toggle-menu :           
   The parameter ref donne la re'fe'rence du catalogue.               
   The parameter val de'signe l'entre'e se'lectionne'e (-1 pour       
   toutes les entre'es). The parameter on indique que le bouton       
   correspondant doit e^tre allume' (on positif) ou e'teint (on nul). 
  ----------------------------------------------------------------------*/
#ifdef _WINDOWS 
void WIN_TtaSetToggleMenu (int ref, int val, ThotBool on, HWND owner)
#else  /* _WINDOWS */
void TtaSetToggleMenu (int ref, int val, ThotBool on)
#endif /* _WINDOWS */
{
#ifdef _WINDOWS 
  struct Cat_Context *catalogue;
  HMENU              hMenu;
  struct E_List      *adbloc;
  int                tmp_val;
  UINT               uCheck;

  catalogue = CatEntry (ref);
  adbloc = catalogue->Cat_Entries;
  if (catalogue == NULL)
    TtaError (ERR_invalid_reference);
  else if (catalogue->Cat_Widget == 0)
    TtaError (ERR_invalid_reference);
  else
    {
      if (on)
	uCheck = MF_CHECKED;
      else
	uCheck = MF_UNCHECKED;
      hMenu = catalogue->Cat_Widget;
      /* the first two entries of the first block
	 are reserved */
      tmp_val = val + 2;
      /* find the correct block for the val entry */
      while (tmp_val >= C_NUMBER)
	{
	  /* the entry is not in the first block,
	     we update the catalog index entry */
	  /* point to the next block */
	  adbloc = adbloc->E_Next;
	  tmp_val = tmp_val - C_NUMBER;
	  /* the first two entries of the first block entry 
	     are reserved */
	}
      if (catalogue->Cat_Type == CAT_SCRPOPUP)
	{
	  HWND listBox;
	  char buffer[MAX_LENGTH];
	  int i;
	  
	  /* the list box widget is inside the Cat_Widget */
	  listBox = GetDlgItem (catalogue->Cat_Widget, 1);
	  /* check the length of the entry to avoid a SIGSEV */
	  i = SendMessage (listBox, LB_GETTEXTLEN, val, (LPARAM) NULL);
	  if (i>1 && i < MAX_LENGTH)
	    {
	      /* we get the text of the entry, add a '>' char and insert it again into
		 the list */
	      SendMessage (listBox, LB_GETTEXT, val, (LPARAM) buffer);
	      SendMessage (listBox, LB_DELETESTRING, val, (LPARAM) NULL);
	      buffer[0] = '>';
	      SendMessage (listBox, LB_INSERTSTRING, val, (LPARAM) buffer);
	    }
	}
      else if (IsMenu (adbloc->E_ThotWidget[tmp_val]))
	{
	  /* change the menu item */
	  if (CheckMenuItem (adbloc->E_ThotWidget[tmp_val], ref + val, uCheck) == 0xFFFFFFFF) 
	    WinErrorBox (NULL, "WIN_TtaSetToggleMenu (1)");
	}
      else if (CheckMenuItem (hMenu, ref + val, uCheck) == 0xFFFFFFFF)
	{
	  /* get the parent menu reference */
	  hMenu = GetMenu (owner);
	  if (CheckMenuItem (hMenu, ref + val, uCheck) == 0xFFFFFFFF) 
	    WinErrorBox (NULL, "WIN_TtaSetToggleMenu (2)");
	}
    }
#else  /* _WINDOWS  */
   ThotWidget          w;
#ifndef _GTK
   Arg                 args[MAX_ARGS];
   register int        n;
#endif /* _GTK */
   register int        i;
   register int        ent;
   ThotBool            visible;
   ThotBool            done;
   struct E_List      *adbloc;
   struct Cat_Context *catalogue;

   catalogue = CatEntry (ref);
   if (catalogue == NULL)
      TtaError (ERR_invalid_reference);
   else if (catalogue->Cat_Widget == 0)
      TtaError (ERR_invalid_reference);
   else
     {
	/* Si ce n'est pas un menu sous-menu */
	if (catalogue->Cat_Type != CAT_TMENU
	    && catalogue->Cat_Type != CAT_MENU
	    && catalogue->Cat_Type != CAT_PULL
	    && catalogue->Cat_Type != CAT_POPUP
	    && catalogue->Cat_Type != CAT_SCRPOPUP)
	  {
	     TtaError (ERR_invalid_reference);
	     return;
	  }
	else if (catalogue->Cat_Type == CAT_TMENU && val > catalogue->Cat_Data)
	  {
	     /* ce n'est pas une entree du toggle */
	     TtaError (ERR_invalid_reference);
	     return;
	  }
	/* Est-ce que le sous-menu est actuellement affiche */
#ifndef _GTK
	else if (XtIsManaged (catalogue->Cat_Widget))
	  visible = TRUE;
	else
	  {
	    visible = FALSE;
	    XtManageChild (catalogue->Cat_Widget);
	  }
#else /* _GTK */	
	else if (GTK_WIDGET_VISIBLE (catalogue->Cat_Widget))
	  visible = TRUE;
	else
	  {
	    visible = FALSE;
	    if (catalogue->Cat_Type != CAT_SCRPOPUP)
	      gtk_widget_show_all (catalogue->Cat_Widget);
	  }
#endif /* _GTK */

	/* Positionnement de la valeur de chaque entree */
	adbloc = catalogue->Cat_Entries;
	ent = 0;
	i = 2;			/* decalage de 2 pour le widget titre */
	done = FALSE;
	while (adbloc != NULL && !done)
	  {
	    while (i < C_NUMBER)
	      {
		/* S'il n'y a pas de bouton  */
		if (adbloc->E_ThotWidget[i] == 0)
		  i = C_NUMBER;
		/* C'est une entree qu'il faut initialiser */
		else if ((ent == val) || (val == -1))
		  {
		    if (catalogue->Cat_Type != CAT_TMENU && adbloc->E_Type[i] != 'T')
		      {
			/* ce n'est pas une entree du toggle */
			TtaError (ERR_invalid_reference);
			return;
		      }
		    else
		      {
			w = adbloc->E_ThotWidget[i];
#ifndef _GTK
			/* retire les callbacks */
			if (catalogue->Cat_Type == CAT_TMENU)
			  XtRemoveCallback (w, XmNarmCallback, (XtCallbackProc) CallToggle, catalogue);
			else
			  XtRemoveCallback (w, XmNvalueChangedCallback, (XtCallbackProc) CallMenu, catalogue);
#endif /* _GTK */
			if (on)
			  /* Bouton allume */
			  {
#ifndef _GTK
			    n = 0;
			    XtSetArg (args[n], XmNset, TRUE);
			    n++;
			    XtSetValues (w, args, n);
#else /* _GTK */
			    /* attribut active is set to the good value */
			    if (catalogue->Cat_Type == CAT_TMENU)
			      {
				RemoveSignalGTK (GTK_OBJECT(w), "toggled");
			      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(w), TRUE);
				ConnectSignalGTK (GTK_OBJECT(w), "toggled",
						  GTK_SIGNAL_FUNC (CallMenuGTK), 
						  (gpointer) catalogue);
			      }
			      /* JK: this is what SG did: GTK_TOGGLE_BUTTON(w)->active = TRUE; */
			    else if (catalogue->Cat_Type == CAT_SCRPOPUP)
			      {
				/* catalogue = GTK_OBJECT(w)->userData; */
				/* RemoveSignalGTK (GTK_OBJECT(w), "activate"); */
				RemoveSignalGTK (GTK_OBJECT(w), "select");
				gtk_list_item_select (GTK_LIST_ITEM (w));
				ConnectSignalGTK (GTK_OBJECT(w), "select",
						  GTK_SIGNAL_FUNC (CallMenuGTK), 
						  (gpointer) catalogue);
			      }
			    else
			      GTK_CHECK_MENU_ITEM(w)->active = TRUE;
#endif /* _GTK */
			  }
			else
			  /* Etat initial du bouton : eteint */
			  {
#ifndef _GTK
			    n = 0;
			    XtSetArg (args[n], XmNset, FALSE);
			    n++;
			    XtSetValues (w, args, n);
#else /* _GTK */			    
			    /* attribut active is set to the good value */
			    if (catalogue->Cat_Type == CAT_TMENU)
			      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(w), FALSE);
			    /* JK: this is what SG did: GTK_TOGGLE_BUTTON(w)->active = FALSE; */
			    else
			      GTK_CHECK_MENU_ITEM(w)->active = FALSE;
#endif /* _GTK */
			  }
#ifndef _GTK
			/* retablit les callbacks */
			if (catalogue->Cat_Type == CAT_TMENU)
			  XtAddCallback (w, XmNarmCallback, (XtCallbackProc) CallToggle, catalogue);
			else
			  XtAddCallback (w, XmNvalueChangedCallback, (XtCallbackProc) CallMenu, catalogue);
#endif /* _GTK */
			 }
		    adbloc->E_Free[i] = 'N';		/* La valeur est la valeur initiale */
		    done = TRUE;
		    break;
		  }
		/* Sinon l'entree garde son etat precedent  */
		  i++;
		  ent++;
	      }		/*while */
	    /* Passe au bloc suivant */
	    adbloc = adbloc->E_Next;
	    i = 0;
	  }			/*while */
#ifndef _GTK
	if (!visible)
	  XtUnmanageChild (catalogue->Cat_Widget);
#else /* _GTK */
	if (!visible && catalogue->Cat_Type != CAT_SCRPOPUP)
	  gtk_widget_hide (catalogue->Cat_Widget);
#endif /* _GTK */
     }
#endif /* _WINDOWS */
}


/*----------------------------------------------------------------------
   TtaChangeMenuEntry modifie l'intitule' texte de l`entre'e entry    
   du menu de'signe' par sa re'fe'rence ref.                          
  ----------------------------------------------------------------------*/
void TtaChangeMenuEntry (int ref, int entry, char *text)
{
   ThotWidget          w;
   struct Cat_Context *catalogue;
   struct E_List      *adbloc;
   int                 ent;
#ifndef _WINDOWS
#ifndef _GTK
   int                 n;
   Arg                 args[MAX_ARGS];
   XmString            title_string;
#endif /* _GTK */
#endif /* _WINDOWS */

   if (ref == 0)
     {
	TtaError (ERR_invalid_reference);
	return;
     }
   catalogue = CatEntry (ref);
   if (catalogue == NULL)
      TtaError (ERR_invalid_reference);
   /* Est-ce qu'il s'agit bien d'un menu ou d'un sous-menu ? */
   else if (catalogue->Cat_Widget == 0
	    || (catalogue->Cat_Type != CAT_MENU
		&& catalogue->Cat_Type != CAT_POPUP
		&& catalogue->Cat_Type != CAT_SCRPOPUP
		&& catalogue->Cat_Type != CAT_PULL
		&& catalogue->Cat_Type != CAT_TMENU
		&& catalogue->Cat_Type != CAT_FMENU))
      TtaError (ERR_invalid_reference);
   else
     {
	/* Recherche l'entree dans le menu ou sous-menu */
	adbloc = catalogue->Cat_Entries;
	ent = entry + 2;	/* decalage de 2 pour le widget titre */
	while (ent >= C_NUMBER)
	  {
	     if (adbloc->E_Next == NULL)
	       {
		  TtaError (ERR_invalid_parameter);
		  return;
	       }
	     else
		adbloc = adbloc->E_Next;
	     ent -= C_NUMBER;
	  }			/*while */
	if (adbloc->E_ThotWidget[ent] == 0)
	   TtaError (ERR_invalid_parameter);
	else
	  {
	     w = adbloc->E_ThotWidget[ent];
#ifndef _WINDOWS
#ifndef _GTK
	     title_string = XmStringCreateSimple (text);
	     n = 0;
	     XtSetArg (args[n], XmNlabelString, title_string);
	     n++;
	     XtSetValues (w, args, n);
	     XtManageChild (w);
	     XmStringFree (title_string);
#else /* _GTK */
	     gtk_label_set_text (GTK_LABEL(w), text);
	     gtk_widget_show_all (w);
#endif /* _GTK */
#endif /* _WINDOWS */
	  }
     }
}

/*----------------------------------------------------------------------
   TtaRedrawMenuEntry modifie la couleur et/ou la police de l'entre'e 
   entry du menu de'signe' par sa re'fe'rence ref.                    
  ----------------------------------------------------------------------*/
void TtaRedrawMenuEntry (int ref, int entry, char *fontname,
			 Pixel color, int activate)
{
  struct Cat_Context *catalogue;
#ifdef _WINDOWS
  HMENU               menu;
#else /* _WINDOWS */
  ThotWidget          w;
  struct E_List      *adbloc;
  int                 ent;
#ifndef _GTK
  int                 n;
  Arg                 args[MAX_ARGS];
  XmFontList          font;
#else /* _GTK */
  ThotWidget          tmpw;
#endif /* _GTK */
#endif /* _WINDOWS */

  if (ref == 0)
    {
      TtaError (ERR_invalid_reference);
      return;
    }
  catalogue = CatEntry (ref);
  if (catalogue == NULL)
    TtaError (ERR_invalid_reference);
  /* Est-ce qu'il s'agit bien d'un menu ou d'un sous-menu ? */
  else if (catalogue->Cat_Widget == 0
	   || (catalogue->Cat_Type != CAT_MENU
	       && catalogue->Cat_Type != CAT_POPUP
	       && catalogue->Cat_Type != CAT_SCRPOPUP
	       && catalogue->Cat_Type != CAT_PULL
           && catalogue->Cat_Type != CAT_TMENU
	       && catalogue->Cat_Type != CAT_FMENU))
    TtaError (ERR_invalid_parameter);
  else
    {
#ifdef _WINDOWS
  menu = catalogue->Cat_Widget;
  if (activate)
    EnableMenuItem (menu, ref + entry, MF_ENABLED);
  else
    EnableMenuItem (menu, ref + entry, MFS_GRAYED);
#else /* _WINDOWS */
      /* Recherche l'entree dans le menu ou sous-menu */
      adbloc = catalogue->Cat_Entries;
      ent = entry + 2;	/* decalage de 2 pour le widget titre */
      while (ent >= C_NUMBER)
	{
	  if (adbloc->E_Next == NULL)
	    {
	      TtaError (ERR_invalid_parameter);
	      return;
	    }
	  else
	    adbloc = adbloc->E_Next;
	  ent -= C_NUMBER;
	}			/*while */
      
      if (adbloc->E_ThotWidget[ent] == 0)
	TtaError (ERR_invalid_parameter);
      else
	{
	  w = adbloc->E_ThotWidget[ent];
#ifndef _GTK
	  /* Recupere si necessaire la couleur par defaut */
	  n = 0;
	  /* Faut-il changer la police de caracteres ? */
	  if (fontname != NULL)
	    font = XmFontListCreate (XLoadQueryFont (GDp, fontname), XmSTRING_DEFAULT_CHARSET);
	  else
	    font = DefaultFont;
	  XtSetArg (args[n], XmNfontList, font);
	  n++;
	  if ((int) color != -1)
	    {
	      /* La couleur imposee par l'application */
	      XtSetArg (args[n], XmNforeground, color);
	      n++;
	       }
	  else
	    {
	      /* La couleur par defaut */
	      XtSetArg (args[n], XmNforeground, FgMenu_Color);
	      n++;
	    }
#else /* _GTK */
	  /* if the widget is a FORM sub menu, then it is a radiolist
	     the label font must be change, and not the radiolist font. 
	     REM: the label could be show with gtk_object_get_data */
	  if (catalogue->Cat_Type == CAT_FMENU)
	    tmpw = GTK_WIDGET(gtk_object_get_data (GTK_OBJECT(w), "Label"));
	  else
	    tmpw = w;
	  if (fontname != NULL)
	    {
	      /* load the new font */
	      tmpw->style->font = (GdkFont *)gdk_font_load (fontname);
	      /*		 gdk_font_ref ((GdkFont *)(w->style->font));*/
	      if(tmpw->style->font == NULL)
		tmpw->style->font = DefaultFont;
	    }
	  else
	    /* keep the default font*/
	    tmpw->style->font = DefaultFont;
	  /* On interprete un changement de couleur comme un set_sensitive */
	  if ((int) color != -1)
	    gtk_widget_set_sensitive (GTK_WIDGET(w), FALSE);
	  else
	    gtk_widget_set_sensitive (GTK_WIDGET(w), TRUE);

#endif /* _GTK */
	  /* Faut-il activer ou desactiver le Callback */
	  if (activate != -1)
	    {
	      if (catalogue->Cat_Type == CAT_POPUP
		  || catalogue->Cat_Type == CAT_SCRPOPUP
		  || catalogue->Cat_Type == CAT_PULL
		  || catalogue->Cat_Type == CAT_MENU)
		{
#ifndef _GTK
		  XtRemoveCallback (w, XmNactivateCallback, (XtCallbackProc) CallMenu, catalogue);
		  if (activate)
		    XtAddCallback (w, XmNactivateCallback, (XtCallbackProc) CallMenu, catalogue);
#endif /* _GTK */
		}
	      else
		/*CAT_FMENU et CAT_TMENU */
#ifndef _GTK
		if (activate)
		  {
		    XtSetArg (args[n], XmNsensitive, TRUE);
		    n++;
		  }
		else
		  {
		    XtSetArg (args[n], XmNsensitive, FALSE);
		    n++;
		  }
#else /* _GTK */	  
	      if (activate)
		gtk_widget_set_sensitive (GTK_WIDGET(w), TRUE);
	      else
		gtk_widget_set_sensitive (GTK_WIDGET(w), FALSE);
#endif /* _GTK */
	    }
#ifndef _GTK
	  XtSetValues (w, args, n);
	  XtManageChild (w);
	  if (fontname != NULL)
	    XmFontListFree (font);
#else /* _GTK */
	  gtk_widget_show_all (GTK_WIDGET(w));
#endif /* _GTK */
	}
#endif /* _WINDOWS */
    }
}


/*----------------------------------------------------------------------
   DestForm de'truit un formulaire ou une feuille de saisie:          
   The parameter ref donne la re'fe'rence du catalogue.               
  ----------------------------------------------------------------------*/
static int DestForm (int ref)
{
  register int        entry;
  struct Cat_Context *catalogue;
  struct Cat_Context *parentCatalogue;
  struct E_List      *adbloc;
  
  catalogue = CatEntry (ref);
  if (catalogue == NULL)
    return (1);
  else if (catalogue->Cat_Widget == 0)
    return (1);
  else
    {
      /*=====================> Detruit une feuille de saisie ou un formulaire*/
      if (catalogue->Cat_Type != CAT_INT
	  && catalogue->Cat_Type != CAT_TEXT
	  && catalogue->Cat_Type != CAT_FORM
	  && catalogue->Cat_Type != CAT_SELECT
	  && catalogue->Cat_Type != CAT_SHEET
	  && catalogue->Cat_Type != CAT_DIALOG)
	return (1);		/* Ce n'est pas le cas : On ne fait rien */
      else
	{
	  /* Il faut liberer l'entree du menu pere qui le reference */
	  parentCatalogue = catalogue->Cat_PtParent;
	  entry = catalogue->Cat_EntryParent;
	  catalogue->Cat_PtParent = NULL;
	  catalogue->Cat_EntryParent = 0;
	  
	  if (parentCatalogue != NULL)
	    {
	      /* Recherche l'entree du menu qui lui correspond */
	      adbloc = parentCatalogue->Cat_Entries;
	      while (entry >= C_NUMBER)
		{
		  entry -= C_NUMBER;
		  if (adbloc->E_Next == NULL)
		    entry = -1;
		  else
		    adbloc = adbloc->E_Next;
		}		/*while */
	      
	      if (entry >= 0)
		{
		  if (parentCatalogue->Cat_Type == CAT_FORM
		      || parentCatalogue->Cat_Type == CAT_SHEET
		      || parentCatalogue->Cat_Type == CAT_DIALOG)
		    /*__________________________________________ Dans un formulaire __*/
		    {
#if !defined(_WINDOWS)  && !defined(_GTK)
		      XtUnmanageChild (catalogue->Cat_Widget);
#endif /* _WINDOWS && _GTK */
		      adbloc->E_ThotWidget[entry] = (ThotWidget) 0;
		      adbloc->E_Free[entry] = 'Y';
		    }
		}
	    }
	  /* Libere le bloc des entrees alloue */
	  if ((catalogue->Cat_Type == CAT_FORM)
	      || (catalogue->Cat_Type == CAT_INT)
	      || (catalogue->Cat_Type == CAT_SHEET)
	      || (catalogue->Cat_Type == CAT_DIALOG))
	    {
	      FreeEList (catalogue->Cat_Entries);
	      catalogue->Cat_Entries = NULL;
	      /* Note que tous les fils sont detruits */
	      if (catalogue->Cat_Type != CAT_INT)
		{
		  ClearChildren (catalogue);
#if !defined(_WINDOWS)  && !defined(_GTK)
		  XtRemoveCallback (catalogue->Cat_Widget, XmNdestroyCallback,
				    (XtCallbackProc) formKill, catalogue);
#endif /* _WINDOWS && _GTK */
		}
	    }
#ifndef _WINDOWS
#ifndef _GTK
	  XtDestroyWidget (catalogue->Cat_Widget);
#else /* _GTK */
	  gtk_widget_destroy (catalogue->Cat_Widget);
#endif /* _GTK */
#endif /* _WINDOWS */
	  /* Libere le catalogue */
	  catalogue->Cat_Widget = 0;
	  return (0);
	}
    }
}

/*----------------------------------------------------------------------
  TtaUnmapDialogue desactive le dialogue s'il est actif.
  ----------------------------------------------------------------------*/
void TtaUnmapDialogue (int ref)
{
  struct Cat_Context *catalogue;
  
  if (ref == 0)
    {
      TtaError (ERR_invalid_reference);
      return;
    }
  catalogue = CatEntry (ref);
  if (catalogue == NULL)
    return;
  else if (catalogue->Cat_Widget == 0)
    return;
#ifndef _WINDOWS
#ifndef _GTK
  else if (XtIsManaged (catalogue->Cat_Widget))
    {
      /* Traitement particulier des formulaires */
	if (catalogue->Cat_Type == CAT_FORM
	    || catalogue->Cat_Type == CAT_SHEET
	    || catalogue->Cat_Type == CAT_DIALOG)
	  XtUnmanageChild (XtParent (catalogue->Cat_Widget));
	XtUnmanageChild (catalogue->Cat_Widget);
    }
#else /* _GTK */
  else if (GTK_WIDGET_VISIBLE(catalogue->Cat_Widget))
    {    
      /* Traitement particulier des formulaires */
      /*      if (catalogue->Cat_Type == CAT_FORM
	  || catalogue->Cat_Type == CAT_SHEET
	  || catalogue->Cat_Type == CAT_DIALOG)*/
	gtk_widget_hide (GTK_WIDGET(catalogue->Cat_Widget));
    }
#endif /* _!GTK */
#endif /* _WINDOWS */
  
  /* Si le catalogue correspond au dernier TtaShowDialogue */
  if (ShowCat != NULL)
    if (ref == ShowCat->Cat_Ref)
      {
	/* Debloque eventuellement une attente active sur le catalogue */
	ShowCat = NULL;
	ShowReturn = 0;
      }
}


/*----------------------------------------------------------------------
  TtaDestroyDialogue de'truit le catalogue de'signe' par ref.
  ----------------------------------------------------------------------*/
void TtaDestroyDialogue (int ref)
{
   register int        entry;
   ThotWidget          w;
   struct E_List      *adbloc;
   struct Cat_Context *catalogue;
   struct Cat_Context *parentCatalogue;
#ifdef _WINDOWS
   int                 nbMenuItems, itNdx;
#else  /* _WINDOWS */
#ifndef _GTK
   int                 n;
   Arg                 args[MAX_ARGS];
#endif /* _GTK */
#endif /* _WINDOWS */

   if (ref == 0)
     {
	TtaError (ERR_invalid_reference);
	return;
     }
   catalogue = CatEntry (ref);
   if (catalogue == NULL)
      return;
   else if (catalogue->Cat_Widget == 0)
      return;
   else
     {
	TtaUnmapDialogue (ref);
	/*===============================================> Detruit un sous-menu */
	if ((catalogue->Cat_Type == CAT_MENU)
	    || (catalogue->Cat_Type == CAT_TMENU)
	    || (catalogue->Cat_Type == CAT_FMENU))
	  {
	     /* Il faut liberer l'entree du menu pere qui le reference */
	     parentCatalogue = catalogue->Cat_PtParent;
	     entry = catalogue->Cat_EntryParent;
	     catalogue->Cat_PtParent = NULL;
	     catalogue->Cat_EntryParent = 0;
	     if (parentCatalogue != NULL)
	       {
		  /* Recherche l'entree du menu qui lui correspond */
		  adbloc = parentCatalogue->Cat_Entries;
		  while (entry >= C_NUMBER)
		    {
		       entry -= C_NUMBER;
		       if (adbloc->E_Next == NULL)
			  entry = -1;
		       else
			  adbloc = adbloc->E_Next;
		    }		/*while */
		  if (entry >= 0)
		    {
		       if (catalogue->Cat_Type == CAT_FMENU
			   || catalogue->Cat_Type == CAT_TMENU)
			 /*_______________________________ Sous-menu d'un formulaire __*/
			 {
			    /* Libere l'entree du sous-menu dans le formulaire */
			    adbloc->E_ThotWidget[entry] = (ThotWidget) 0;
			    adbloc->E_Free[entry] = 'Y';

			 }
		       else
			 /*_____________________________________ Sous-menu d'un menu __*/
			 {
			    if (adbloc->E_Type[entry] == 'M'
				&& adbloc->E_Free[entry] == 'N')
			      {
				/*** Delie le sous-menu du bouton du menu ***/
				 w = adbloc->E_ThotWidget[entry];
#ifdef _WINDOWS
				 nbMenuItems = GetMenuItemCount (w);
				 for (itNdx = 0; itNdx < nbMenuItems; itNdx ++) 
				   if (!DeleteMenu (w, ref + itNdx, MF_BYCOMMAND))
				     if (!DeleteMenu (w, ref + itNdx, MF_BYPOSITION))
				       WinErrorBox (NULL, "TtaDestroyDialogue");
				 /* RemoveMenu (w, ref + itNdx, MF_BYCOMMAND); */
				 DestroyMenu (w);
				 subMenuID [currentFrame] = (UINT)w;
				 /* CHECK  CHECK  CHECK  CHECK  CHECK  CHECK  CHECK */
#endif /* _WINDOWS */
#ifndef _WINDOWS
#ifndef _GTK
				 n = 0;
				 XtSetArg (args[n], XmNsubMenuId, 0);
				 n++;
				 XtSetValues (w, args, n);
				 XtManageChild (w);
#endif /* _GTK */
#endif /* _WINDOWS */
				 adbloc->E_Free[entry] = 'Y';
			      }
			 }
		    }
	       }
	  }
	/*=================================================> Un autre catalogue */
	else if ((catalogue->Cat_Type != CAT_POPUP)
		 && (catalogue->Cat_Type != CAT_SCRPOPUP)
		 && (catalogue->Cat_Type != CAT_PULL)
		 && (catalogue->Cat_Type != CAT_LABEL))
	  {
	     /* C'est surement une destruction de formulaire */
	     if (DestForm (ref) != 0)
		TtaError (ERR_invalid_reference);
#ifndef _WINDOWS
#ifndef _GTK
	     if (PopShell != 0)
	       {
		 XtDestroyWidget (PopShell);
		 PopShell = 0;
	       }
#else /* _GTK */
	     if (PopShell != 0 && GTK_IS_WIDGET (PopShell))
	       gtk_widget_destroy (GTK_WIDGET (PopShell));
	     PopShell = 0;
#endif /* _GTK */
#endif /* _WINDOWS */
	     return;
	  }
	/* Note que tous les fils sont detruits */
	if ((catalogue->Cat_Type == CAT_POPUP)
	    || (catalogue->Cat_Type == CAT_SCRPOPUP)
	    || (catalogue->Cat_Type == CAT_PULL)
	    || (catalogue->Cat_Type == CAT_MENU))
	   ClearChildren (catalogue);
	/* Libere les blocs des entrees */
	FreeEList (catalogue->Cat_Entries);
	catalogue->Cat_Entries = NULL;
#ifndef _WINDOWS
#ifndef _GTK
	if (catalogue->Cat_Type != CAT_PULL)
	   XtDestroyWidget (catalogue->Cat_Widget);
#else /* _GTK */
       	if (catalogue->Cat_Type != CAT_PULL)
	  gtk_widget_destroy (GTK_WIDGET(catalogue->Cat_Widget));
#endif /* _GTK */
#endif /* _WINDOWS */
	/* Libere le catalogue */
	catalogue->Cat_Widget = 0;
     }
}

#ifndef _WINDOWS
/*----------------------------------------------------------------------
   TtaChangeFormTitle change le titre d'un formulaire ou d'une feuille
   de dialogue :
   The parameter ref donne la re'fe'rence du catalogue.    
   Le parame'tre title donne le titre du catalogue.  
  ----------------------------------------------------------------------*/
void TtaChangeFormTitle (int ref, char *title)
{
   struct Cat_Context *catalogue;
#ifndef _GTK
   int                 n;
   Arg                 args[MAX_ARGS];
   XmString            title_string;
#endif /* _GTK */
   if (ref == 0)
     {
	TtaError (ERR_invalid_reference);
	return;
     }
   catalogue = CatEntry (ref);
   if (catalogue == NULL)
      TtaError (ERR_invalid_reference);
   /* Est-ce que le catalogue existe deja ? */
   else if (catalogue->Cat_Widget == 0)
      TtaError (ERR_invalid_reference);
   else
     {
#ifndef _GTK
       /* Set the window title with motif */
       title_string = XmStringCreateSimple (title);
       n = 0;
       XtSetArg (args[n], XmNdialogTitle, title_string);
       n++;
       XtSetValues (catalogue->Cat_Widget, args, n);
#else
       /* Set the window title with GTK */
       gdk_window_set_title(GTK_WIDGET(catalogue->Cat_Widget)->window, title);
#endif /* _GTK */
     }
}

/*----------------------------------------------------------------------
  TtaSetDefaultButton
  Defines the default result for the GTK event Double-click.
  ----------------------------------------------------------------------*/
void TtaSetDefaultButton (int ref, int button)
{
   struct Cat_Context *catalogue;

   if (ref != 0)
     {
       catalogue = CatEntry (ref);
       if (catalogue)
	 catalogue->Cat_Default = (unsigned char) button;
     }
}

/*----------------------------------------------------------------------
  NewSheet
  ----------------------------------------------------------------------*/
static void NewSheet (int ref, ThotWidget parent, char *title, int number,
		      char *text, ThotBool horizontal, int package,
		      char button, int dbutton, int cattype)
{
   int                 ent;
   int                 index;
   int                 count;
   struct Cat_Context *catalogue;
   struct E_List      *adbloc;
   ThotWidget          form;
   ThotWidget          w;
   char               *ptr = NULL;
#ifndef _GTK
   Arg                 args[MAX_ARGS];
   Arg                 argform[1];
   int                 n;
   XmString            title_string, OK_string;
#else
   ThotWidget          tmpw;
#endif /* _GTK */
   ThotWidget          row;

   if (ref == 0)
     {
	TtaError (ERR_invalid_reference);
	return;
     }
   catalogue = CatEntry (ref);
   if (catalogue == NULL)
      TtaError (ERR_cannot_create_dialogue);
   else
     {
	/* Est-ce que le catalogue existe deja ? */
	if (catalogue->Cat_Widget != 0)
	   TtaDestroyDialogue (ref);	/* Reconstruction du catalogue */
	/* Recherche le widget parent */
	if (MainShell == 0 && parent == 0)
	  {
#ifndef _GTK
	    OK_string = XmStringCreateSimple (TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
	    n = 0;
	    XtSetArg (args[n], XmNx, (Position) ShowX);
	    n++;
	    XtSetArg (args[n], XmNy, (Position) ShowY);
	    n++;
	    XtSetArg (args[n], XmNallowShellResize, TRUE);
	    n++;
	    XtSetArg (args[n], XmNuseAsyncGeometry, TRUE);
	    n++;
	    PopShell = XtCreatePopupShell ("", applicationShellWidgetClass, RootShell, args, 0);
#else /* _GTK */
	    PopShell = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	    gtk_widget_realize (PopShell);
	    gtk_window_set_title (GTK_WINDOW (PopShell), TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
	    gtk_widget_set_uposition (GTK_WIDGET (PopShell), ShowX, ShowY);
	    gtk_container_set_border_width (GTK_CONTAINER (PopShell), 10);
	    ConnectSignalGTK (GTK_OBJECT (PopShell),
			      "destroy",
			      GTK_SIGNAL_FUNC (DeletePopShell),
			      (gpointer)NULL);
#endif /* _GTK */
	  }
	/*________________________________________________ Feuillet principal __*/
	else
	  {
	     /* Il n'y a pas de menu parent */
	     catalogue->Cat_PtParent = NULL;
	     catalogue->Cat_EntryParent = 0;
	  }
	/* No focus in sub-menus */
	catalogue->Cat_Focus = FALSE;
	/* Cree la fenetre du formulaire */
	if (parent != 0)
	   w = parent;
	else if (MainShell == 0)
	   w = PopShell;
	else
	   w = MainShell;
#ifndef _GTK
	n = 0;
	XtSetArg (args[n], XmNfontList, DefaultFont);
	n++;
	if (w == PopShell)
	  form = PopShell;
	else
	  form = XtCreateWidget (title, topLevelShellWidgetClass, w, args, n);
	XtAddCallback (form, XmNdestroyCallback, (XtCallbackProc) formKill, catalogue);
	/*** Cree le formulaire dans sa frame ***/
	title_string = XmStringCreateSimple (title);
	XtSetArg (args[n], XmNdialogTitle, title_string);
	n++;
	XtSetArg (args[n], XmNautoUnmanage, FALSE);
	n++;
	XtSetArg (args[n], XmNdefaultPosition, FALSE);
	n++;
	XtSetArg (args[n], XmNbackground, BgMenu_Color);
	n++;
	form = XmCreateBulletinBoard (form, "", args, n);
	XtAddCallback (XtParent (form), XmNpopdownCallback,
		       (XtCallbackProc) CallSheet, catalogue);
	XtAddCallback (XtParent (form), XmNdestroyCallback,
		       (XtCallbackProc) formKill, catalogue);
	XmStringFree (title_string);
#else /* _GTK */
	/* Creation of the window */
	if (w == PopShell)
	  form = PopShell;
	else
	{
	  form = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	  gtk_widget_realize (GTK_WIDGET (form));	
	  gtk_window_set_title (GTK_WINDOW (form), title);
	  gtk_window_set_position (GTK_WINDOW (form), GTK_WIN_POS_MOUSE);
	  ConnectSignalGTK (GTK_OBJECT(form), "delete_event",
			    GTK_SIGNAL_FUNC (formKillGTK), (gpointer) catalogue);
	}
	gtk_container_set_border_width (GTK_CONTAINER (form), 10);
#endif /* _GTK */
	catalogue->Cat_Ref = ref;
	catalogue->Cat_Type = cattype;
	catalogue->Cat_Button = button;
	catalogue->Cat_FormPack = package + 1;
	catalogue->Cat_Widget = form;
	/* Cree le contenu initial du feuillet */
	adbloc = NewEList ();	/* Un bloc supplementaire pour les boutons */
	catalogue->Cat_Entries = adbloc;
	adbloc->E_Next = NewEList ();
	adbloc = adbloc->E_Next;
#ifndef _GTK
	/*** Cree un Row-Column pour mettre les boutons QUIT/... ***/
	/*** en dessous des sous-menus et sous-formulaires.    ***/
	n = 0;
	XtSetArg (args[n], XmNadjustLast, FALSE);
	n++;
	XtSetArg (args[n], XmNmarginWidth, 0);
	n++;
	XtSetArg (args[n], XmNmarginHeight, 0);
	n++;
	XtSetArg (args[n], XmNspacing, 2);
	n++;
	XtSetArg (args[n], XmNbackground, BgMenu_Color);
	n++;
	row = XmCreateRowColumn (form, "Dialogue", args, n);
	XtManageChild (row);

	/*** Cree un Row-Column pour contenir les lignes ou colonnes ***/
	/*** de sous-menus et sous-formulaires.                      ***/
	n = 0;
	XtSetArg (args[n], XmNadjustLast, FALSE);
	n++;
	XtSetArg (args[n], XmNmarginWidth, 0);
	n++;
	XtSetArg (args[n], XmNmarginHeight, 0);
	n++;
	XtSetArg (args[n], XmNspacing, 2);
	n++;
	if (horizontal)
	   XtSetArg (args[n], XmNorientation, XmVERTICAL);
	else
	   XtSetArg (args[n], XmNorientation, XmHORIZONTAL);
	n++;
	XtSetArg (args[n], XmNbackground, BgMenu_Color);
	n++;
	w = XmCreateRowColumn (row, "Dialogue", args, n);
	XtManageChild (w);
	/*** Cree un 1er Row-Column pour contenir les sous-menus/formulaires ***/
	n = 0;
	XtSetArg (args[n], XmNadjustLast, FALSE);
	n++;
	XtSetArg (args[n], XmNspacing, 5);
	n++;
	catalogue->Cat_in_lines = (int) horizontal;
	if (horizontal)
	   XtSetArg (args[n], XmNorientation, XmHORIZONTAL);
	else
	   XtSetArg (args[n], XmNorientation, XmVERTICAL);
	n++;
	XtSetArg (args[n], XmNbackground, BgMenu_Color);
	n++;
	w = XmCreateRowColumn (w, "Dialogue", args, n);
	XtManageChild (w);
#else /* _GTK */
	/* Create the vbox for button & other vbox or hbox*/
	row = gtk_vbox_new (FALSE, 5);
	gtk_widget_show_all (row);
	gtk_container_add(GTK_CONTAINER(form), row);
	/* Create hbox or vbox in the row */
	if (horizontal)
	  w = gtk_vbox_new (FALSE, 5);
	else
	  w = gtk_hbox_new (FALSE, 5);
       	gtk_widget_show_all (GTK_WIDGET(w));
	gtk_box_pack_start (GTK_BOX(row), w,  TRUE, TRUE, 0);
	/* Create hbox or vbox in the last w */
	catalogue->Cat_in_lines = (int) horizontal;
	if (horizontal)
	  tmpw = gtk_hbox_new (FALSE, 5);
	else
	  tmpw = gtk_vbox_new (FALSE, 5);
	gtk_widget_show_all (tmpw);
	gtk_box_pack_start (GTK_BOX(w), tmpw,  TRUE, TRUE, 0);
	w = tmpw;
#endif /* _GTK */
	adbloc->E_ThotWidget[0] = w;
	adbloc->E_Free[0] = 'X';
	if (number < 0)
	   /* il n'y a pas de boutons a engendrer */
	   return;
#ifndef _GTK
	/*** Cree un Row-Column pour contenir les boutons QUIT/... ***/
	n = 0;
	XtSetArg (args[n], XmNadjustLast, FALSE);
	n++;
	XtSetArg (args[n], XmNorientation, XmHORIZONTAL);
	n++;
	XtSetArg (args[n], XmNpacking, XmPACK_TIGHT/*XmPACK_COLUMN*/);
	n++;
	XtSetArg (args[n], XmNbackground, BgMenu_Color);
	n++;
	XtSetArg (args[n], XmNforeground, FgMenu_Color);
	n++;
	XtSetArg (args[n], XmNspacing, 5);
	n++;
	XtSetArg (args[n], XmNnumColumns, number);
	n++;
	row = XmCreateRowColumn (row, "Dialogue", args, n);
	XtManageChild (row);

	/*** Cree les boutons ***/
	n = 0;
	XtSetArg (args[n], XmNfontList, formFONT);
	n++;
	XtSetArg (args[n], XmNbackground, BgMenu_Color);
	n++;
	XtSetArg (args[n], XmNforeground, FgMenu_Color);
	n++;
#else /* _GTK */
	/* Create the hbox for buttons */
	tmpw = gtk_hbox_new (FALSE, 5);
	gtk_widget_show_all (tmpw);
	/*tmpw->style->font=formFONT;*/
	gtk_widget_set_name (tmpw, "Dialogue");
	gtk_box_pack_start (GTK_BOX(row), tmpw, FALSE, FALSE, 0);
	row=tmpw;
#endif /* _GTK */
	adbloc = catalogue->Cat_Entries;
	if (cattype == CAT_SHEET)
	   ent = 1;
	else if (cattype == CAT_FORM)
	  {
	    /*** Cree le bouton de confirmation du formulaire ***/
	     ent = 1;
#ifndef _GTK
	     w = XmCreatePushButton (row, TtaGetMessage (LIB, TMSG_LIB_CONFIRM),
				     args, n);
	     XtManageChild (w);
	     XtAddCallback (w, XmNactivateCallback,
			    (XtCallbackProc) CallSheet, catalogue);
	     /* Definit le bouton de confirmation comme bouton par defaut */
	     XtSetArg (argform[0], XmNdefaultButton, w);
	     XtSetValues (form, argform, 1);
	     adbloc->E_ThotWidget[1] = w;
	     /* Definit le bouton de confirmation comme bouton par defaut */
	     XtSetArg (argform[0], XmNdefaultButton, w);
	     XtSetValues (form, argform, 1);
#else /* _GTK */
	     w = gtk_button_new_with_label (TtaGetMessage(LIB, TMSG_LIB_CONFIRM));
	     GTK_WIDGET_SET_FLAGS (GTK_WIDGET (w), GTK_CAN_DEFAULT);
	     gtk_widget_show_all (GTK_WIDGET (w));
	     gtk_box_pack_start (GTK_BOX(row), w, FALSE, FALSE, 0);
	     ConnectSignalGTK (GTK_OBJECT(w), "clicked",
			       GTK_SIGNAL_FUNC (CallSheet), (gpointer) catalogue);
	     /*ConnectSignalGTK (GTK_OBJECT(w), "activate",
			       GTK_SIGNAL_FUNC (CallSheet), (gpointer) catalogue);*/
	     gtk_widget_grab_default (GTK_WIDGET (w));
	     adbloc->E_ThotWidget[1] = w;
#endif /* _GTK */
	  }
	else
	   ent = 0;
	/*** Cree les autres boutons du feuillet ***/
	index = 0;
	while (ent < C_NUMBER && ent <= number && text != NULL)
	  {
	     count = strlen (&text[index]);	/* Longueur de l'intitule */
	     /* S'il n'y a plus d'intitule -> on arrete */
	     if (count == 0)
		ent = number;
	     else
	       {
		  if (ent == 0)
		     /* Le premier bouton remplace le bouton QUIT */
		     ptr = &text[index];
		  else
		    {
#ifndef _GTK
		       w = XmCreatePushButton (row, &text[index], args, n);
		       XtManageChild (w);
		       XtAddCallback (w, XmNactivateCallback,
				      (XtCallbackProc) CallSheet, catalogue);
#else /* _GTK */
		       w = gtk_button_new_with_label (&text[index]);
		       GTK_WIDGET_SET_FLAGS (GTK_WIDGET (w), GTK_CAN_DEFAULT);
		       gtk_widget_show_all (GTK_WIDGET (w));
		       gtk_box_pack_start (GTK_BOX (row), w, FALSE, FALSE, 0);
		       ConnectSignalGTK (GTK_OBJECT(w), "clicked",
					 GTK_SIGNAL_FUNC (CallSheet),
					 (gpointer) catalogue);
		       /*ConnectSignalGTK (GTK_OBJECT(w), "activate",
					 GTK_SIGNAL_FUNC (CallSheet),
					 (gpointer) catalogue);*/
		       gtk_widget_grab_default (GTK_WIDGET(w));
#endif /* _GTK */
		       adbloc->E_ThotWidget[ent] = w;
		    }
#ifndef _GTK
		  /* Definit le bouton de confirmation comme bouton par defaut */
		  if (index == 0)
		     XtSetArg (argform[0], XmNdefaultButton, w);
		  XtSetValues (form, argform, 1);
#else /* _GTK */
		  if (index == 0 && number > 0)
		    gtk_widget_grab_default (GTK_WIDGET(w));

#endif /* _GTK */
	       }
	     index += count + 1;
	     ent++;
	  }
	if (cattype == CAT_SHEET || cattype == CAT_FORM)
	  /*** Cree le bouton QUIT ***/
	   switch (dbutton)
	     {
	     case D_CANCEL:
#ifndef _GTK
	       w = XmCreatePushButton (row, TtaGetMessage (LIB, TMSG_CANCEL), args, n);
#else /* _GTK */
	       w = gtk_button_new_with_label(TtaGetMessage (LIB, TMSG_CANCEL));
	       GTK_WIDGET_SET_FLAGS (GTK_WIDGET(w), GTK_CAN_DEFAULT);
	       gtk_widget_show_all (w);
	       gtk_box_pack_start (GTK_BOX(row), w, FALSE, FALSE, 5);
#endif /* _GTK */
	       break;
	     case D_DONE:
#ifndef _GTK
	       w = XmCreatePushButton (row,
				       TtaGetMessage (LIB, TMSG_DONE), args, n);
#else /* _GTK */
	       w = gtk_button_new_with_label(TtaGetMessage (LIB, TMSG_DONE));
	       GTK_WIDGET_SET_FLAGS (GTK_WIDGET(w), GTK_CAN_DEFAULT);
	       gtk_widget_show_all (w);
	       gtk_box_pack_start (GTK_BOX(row), w, FALSE, FALSE, 5);
#endif /* _GTK */
	       break;
	     }
	else
#ifndef _GTK
	  w = XmCreatePushButton (row, ptr, args, n);
	XtManageChild (w);
	XtAddCallback (w, XmNactivateCallback,
		       (XtCallbackProc) CallSheet, catalogue);
#else /* _GTK */
	{
	  w = gtk_button_new_with_label(ptr);
	  GTK_WIDGET_SET_FLAGS (GTK_WIDGET (w), GTK_CAN_DEFAULT);
	  gtk_widget_show_all (GTK_WIDGET (w));
	  gtk_box_pack_start (GTK_BOX (row), w, FALSE, FALSE, 5);
	}
	gtk_widget_show_all (GTK_WIDGET (w));
	ConnectSignalGTK (GTK_OBJECT (w), "clicked",
			  GTK_SIGNAL_FUNC (CallSheet), (gpointer)catalogue);
 
#endif /* _GTK */
	/* Range le bouton dans le 1er bloc de widgets */
	adbloc->E_ThotWidget[0] = w;
     }
}

/*----------------------------------------------------------------------
   TtaNewForm cre'e un formulaire :                                   
   The parameter ref donne la reference et parent le ThotWidget pe're 
   Si parent est nul, le menu est attache a la fenetre MainWindow.          
   Le parame'tre title donne le titre du catalogue.            
   The parameter horizontal indique que le formulaire est compose' en 
   lignes (TRUE) ou en colonnes (FALSE).                              
   The parameter package donne le facteur de blocage du formulaire    
   (nombre d'entre'es par ligne ou par colonne).                      
   Le parame'tre button indique le bouton de la souris qui active le  
   menu : 'L' pour left, 'M' pour middle et 'R' pour right.           
  ----------------------------------------------------------------------*/
void TtaNewForm (int ref, ThotWidget parent, char *title,
		 ThotBool horizontal, int package, char button, int dbutton)
{
   NewSheet (ref, parent, title, 0, NULL, horizontal, package,
	     button, dbutton, CAT_FORM);
}

/*----------------------------------------------------------------------
   TtaNewSheet cre'e un feuillet de commande :                        
   le parameter ref donne la reference et parent le ThotWidget pe're 
   Si parent est nul, le menu est attache a la fenetre MainWindow.          
   The parameter title donne le titre du catalogue.            
   The parameter number indique le nombre de boutons ajoute's au      
   bouton 'QUIT' mis par de'faut.                                     
   The parameter text contient la liste des intitule's des boutons    
   ajoute's au bouton 'QUIT'.                                         
   Chaque intitule' se termine par le caracte`re de fin de chai^ne \0.
   The parameter horizontal indique que le formulaire est compose' en 
   lignes (TRUE) ou en colonnes (FALSE).                              
   The parameter package donne le facteur de blocage du formulaire    
   (nombre d'entre'es par ligne ou par colonne).                      
   The parameter button indique le bouton de la souris qui active le  
   menu : 'L' pour left, 'M' pour middle et 'R' pour right.           
  ----------------------------------------------------------------------*/
void TtaNewSheet (int ref, ThotWidget parent, char *title, int number,
		  char *text, ThotBool horizontal, int package,
		  char button, int dbutton)
{
   NewSheet (ref, parent, title, number, text, horizontal, package,
	     button, dbutton, CAT_SHEET);
}


/*----------------------------------------------------------------------
   TtaNewDialogSheet cre'e un feuillet de dialogue :                  
   The parameter ref donne la reference et parent le ThotWidget pe're 
   Si parent est nul, le menu est attache a la fenetre MainWindow.          
   The parameter title donne le titre du catalogue.            
   The parameter number indique le nombre de boutons ajoute's au      
   bouton 'QUIT' mis par de'faut.                                     
   The parameter text contient la liste des intitule's des boutons    
   ajoute's au bouton 'QUIT'.                                         
   Chaque intitule' se termine par le caracte`re de fin de chai^ne \0.
   The parameter horizontal indique que le formulaire est compose' en 
   lignes (TRUE) ou en colonnes (FALSE).                              
   The parameter package donne le facteur de blocage du formulaire    
   (nombre d'entre'es par ligne ou par colonne).                      
   The parameter button indique le bouton de la souris qui active le  
   menu : 'L' pour left, 'M' pour middle et 'R' pour right.           
  ----------------------------------------------------------------------*/
void TtaNewDialogSheet (int ref, ThotWidget parent, char *title,
			int number, char *text, ThotBool horizontal,
			int package, char button)
{
   NewSheet (ref, parent, title, number - 1, text, horizontal, package,
	     button, D_DONE, CAT_DIALOG);
}

/*----------------------------------------------------------------------
   TtaAttachForm attache le catalogue au formulaire ou au feuillet    
   dont il de'pend. Les catalogues sont cre'e's attache's.            
  ----------------------------------------------------------------------*/
void TtaAttachForm (int ref)
{
   int                 entry;
   struct E_List      *adbloc;
   struct Cat_Context *catalogue;
   struct Cat_Context *parentCatalogue;

   if (ref == 0)
     {
	TtaError (ERR_invalid_reference);
	return;
     }

   catalogue = CatEntry (ref);
   if (catalogue == NULL)
      TtaError (ERR_invalid_reference);
   else if (catalogue->Cat_Widget == 0)
      TtaError (ERR_invalid_reference);
   else if ((catalogue->Cat_Type != CAT_FMENU)
	    && (catalogue->Cat_Type != CAT_TMENU)
	    && (catalogue->Cat_Type != CAT_TEXT)
	    && (catalogue->Cat_Type != CAT_LABEL)
	    && (catalogue->Cat_Type != CAT_INT)
	    && (catalogue->Cat_Type != CAT_SELECT))
      TtaError (ERR_invalid_reference);
   else
     {
	/* Attache le catalogue au formulaire designe */
	parentCatalogue = catalogue->Cat_PtParent;
	if (parentCatalogue == NULL)
	  {
	     TtaError (ERR_invalid_parent_dialogue);
	     return;
	  }
	/* Recherche l'entree du menu qui lui correspond */
	adbloc = parentCatalogue->Cat_Entries;
	entry = catalogue->Cat_EntryParent;

	/* Saute au bloc qui contient l'entree recherchee */
	while (entry >= C_NUMBER)
	  {
	     if (adbloc->E_Next == NULL)
	       {
		  TtaError (ERR_invalid_parameter);
		  return;
	       }
	     else
		adbloc = adbloc->E_Next;
	     entry -= C_NUMBER;
	  }
	if (adbloc->E_Free[entry] == 'Y')
	  {
	     /* marque que le sous-menu est attache */
	     adbloc->E_Free[entry] = 'N';
#ifndef _GTK
	     /* affiche le widget sur l'ecran */
	     if (XtIsManaged (parentCatalogue->Cat_Widget))
		XtManageChild (catalogue->Cat_Widget);
#else /* _GTK */
	     if ( GTK_WIDGET_VISIBLE(parentCatalogue->Cat_Widget) )
	       {
		 gdk_window_raise (GTK_WIDGET(catalogue->Cat_Widget)->window);
	       }
#endif /* _GTK */ 
	  }
     }
}

/*----------------------------------------------------------------------
   TtaDetachForm detache le catalogue au formulaire ou au feuillet    
   dont il de'pend. Les catalogues sont cre'e's attache's.            
  ----------------------------------------------------------------------*/
void TtaDetachForm (int ref)
{
   int                 entry;
   struct E_List      *adbloc;
   struct Cat_Context *catalogue;
   struct Cat_Context *parentCatalogue;

   if (ref == 0)
     {
	TtaError (ERR_invalid_reference);
	return;
     }
   catalogue = CatEntry (ref);
   if (catalogue == NULL)
      TtaError (ERR_invalid_reference);
   else if (catalogue->Cat_Widget == 0)
      TtaError (ERR_invalid_reference);
   else if ((catalogue->Cat_Type != CAT_FMENU)
	    && (catalogue->Cat_Type != CAT_TMENU)
	    && (catalogue->Cat_Type != CAT_TEXT)
	    && (catalogue->Cat_Type != CAT_LABEL)
	    && (catalogue->Cat_Type != CAT_INT)
	    && (catalogue->Cat_Type != CAT_SELECT))
      TtaError (ERR_invalid_reference);
   else
     {
	/* Detache le catalogue du formulaire designe */
	parentCatalogue = catalogue->Cat_PtParent;
	if (parentCatalogue == NULL)
	  {
	     TtaError (ERR_invalid_parent_dialogue);
	     return;
	  }
	/* Recherche l'entree du menu qui lui correspond */
	adbloc = parentCatalogue->Cat_Entries;
	entry = catalogue->Cat_EntryParent;
	/* Saute au bloc qui contient l'entree recherchee */
	while (entry >= C_NUMBER)
	  {
	     if (adbloc->E_Next == NULL)
	       {
		  TtaError (ERR_invalid_parameter);
		  return;
	       }
	     else
		adbloc = adbloc->E_Next;
	     entry -= C_NUMBER;
	  }
	if (adbloc->E_Free[entry] == 'N')
	  {
	     /* marque que le sous-menu est detache */
	     adbloc->E_Free[entry] = 'Y';
#ifndef _GTK
	     /* retire le widget de l'ecran */
	     if (XtIsManaged (catalogue->Cat_Widget))
		XtUnmanageChild (catalogue->Cat_Widget);
#else /* _GTK */
	     if ( GTK_WIDGET_VISIBLE(catalogue->Cat_Widget) )
	       gtk_widget_hide (catalogue->Cat_Widget);
#endif /* _GTK */
	  }
     }
}

/*----------------------------------------------------------------------
   TtaNewSizedSelector creates a selector of a given width in a dialogue form:
   The parameter ref donne la re'fe'rence pour l'application.
   The parameter title donne le titre du catalogue.             
   The parameter number donne le nombre d'intitule's.          
   The parameter text contient la liste des intitule's.   
   The parameter height donne le nombre d'intitule's visibles a` la
   fois (hauteur de la fenetree^tre de visualisation).
   The parameter label ajoute un choix supple'mentaire a` la liste
   des choix possibles. Ce choix est affiche' se'pare'ment et donc
   mis en e'vidence. 
   The parameter withText indique s'il faut creer une zone texte.
   Quand le parame`tre react est vrai, tout changement de se'lection
   dans le se'lecteur est imme'diatement signale' a` l'application.
  ----------------------------------------------------------------------*/
void TtaNewSizedSelector (int ref, int ref_parent, char *title,
			  int number, char *text, int width, int height,
			  char *label, ThotBool withText, ThotBool react)
{
   struct Cat_Context *catalogue;
   struct Cat_Context *parentCatalogue;

#ifndef _GTK
   Arg                 args[MAX_ARGS];
   int                 n;
   XmString            title_string;
   XmString           *item;
   ThotWidget          wt;
#else /* _GTK */
   GList              *item = NULL;
   ThotWidget          tmpw, tmpw2;
#endif /* _GTK */
   int                 ent;
   int                 index;
   int                 i;
   ThotWidget          row;
   ThotWidget          w;
   struct E_List      *adbloc;
   ThotBool            rebuilded;

   if (ref == 0)
     {
	TtaError (ERR_invalid_reference);
	return;
     }
   catalogue = CatEntry (ref);
   rebuilded = FALSE;
   if (catalogue == NULL)
     {
	TtaError (ERR_invalid_reference);
	return;
     }
   /* Faut-il detruire le catalogue precedent ? */
   else if (catalogue->Cat_Widget != 0)
     {
	/* Si c'est une mise a jour du selecteur */
	if (catalogue->Cat_Type == CAT_SELECT && (withText != catalogue->Cat_SelectList))
	   rebuilded = TRUE;
	else
	   TtaDestroyDialogue (ref);	/* Modification du catalogue */
     }
   /*______________________________ Regarde si le catalogue parent existe __*/
   if (!rebuilded)
     {
	parentCatalogue = CatEntry (ref_parent);
	if (parentCatalogue == NULL)
	  {
	     TtaError (ERR_invalid_parent_dialogue);
	     return;
	  }
	else if (parentCatalogue->Cat_Widget == 0)
	  {
	     TtaError (ERR_invalid_parent_dialogue);
	     return;
	  }
	else if ((parentCatalogue->Cat_Type != CAT_FORM)
		 && (parentCatalogue->Cat_Type != CAT_SHEET)
		 && (parentCatalogue->Cat_Type != CAT_DIALOG))
	  {
	     TtaError (ERR_invalid_parent_dialogue);
	     return;
	  }
	catalogue->Cat_PtParent = parentCatalogue;
     }
   else
      parentCatalogue = catalogue->Cat_PtParent;
   /* Avec ou sans zone texte */
   catalogue->Cat_SelectList = !withText;
   catalogue->Cat_React = react;
#ifndef _GTK
   if (number == 0)
     {
	/* Cree un selecteur avec une entree a blanc */
	item = (XmString *) TtaGetMemory (sizeof (XmString) * 2);
	item[0] = XmStringCreateLtoR (" ", XmSTRING_DEFAULT_CHARSET);
	number = 1;
     }
   else
     {
	/* Cree les differentes entrees du selecteur */
	item = (XmString *) TtaGetMemory (sizeof (XmString) * (number + 1));
	i = 0;
	index = 0;
	while (i < number && text[index] != EOS)
	  {
	     item[i++] = XmStringCreateLtoR (&text[index], XmSTRING_DEFAULT_CHARSET);
	     index += strlen (&text[index]) + 1;	/* Longueur de l'intitule */
	  }
	number = i;
     }
   item[number] = NULL;
#else /* _GTK */
   /* Ici on ajoute les entrees au selecteur */
   if (number == 0)
     {
       /* Create a blank selector, item is a GList which contain all Label entry */
       tmpw = gtk_list_item_new();
       tmpw2 = gtk_label_new (" ");
       gtk_misc_set_alignment (GTK_MISC (tmpw2), 0.0, 0.5);
       gtk_widget_show_all (GTK_WIDGET (tmpw));
       gtk_widget_show_all (GTK_WIDGET (tmpw2));
       gtk_label_set_justify (GTK_LABEL (tmpw2), GTK_JUSTIFY_LEFT);
       gtk_container_add (GTK_CONTAINER (tmpw), tmpw2);
       gtk_object_set_data (GTK_OBJECT(tmpw), "ListElementLabel", (gpointer) tmpw2);
       item = g_list_append (item, tmpw);
       number = 1;
     }
   else
     {
	/* Cree les differentes entrees du selecteur */
	i = 0;
	index = 0;
	while (i < number && text[index] != EOS)
	  {
	    /* Create a blank selector, item is a GList which contain all Label entry */
	    tmpw = gtk_list_item_new();
	    tmpw2 = gtk_label_new (&text[index]);
	    gtk_misc_set_alignment (GTK_MISC (tmpw2), 0.0, 0.5);
	    gtk_widget_show_all (GTK_WIDGET (tmpw));
	    gtk_widget_show (GTK_WIDGET (tmpw2));
	    gtk_label_set_justify (GTK_LABEL (tmpw2), GTK_JUSTIFY_LEFT);
	    gtk_container_add (GTK_CONTAINER (tmpw), tmpw2);
	    /* put a reference of the label into the list widget */
	    gtk_object_set_data (GTK_OBJECT(tmpw), "ListElementLabel", (gpointer) tmpw2);
	    /* add the new item to the GList */
	    item = g_list_append (item, tmpw);
	    i++;
	    index += strlen (&text[index]) + 1;	/* Longueur de l'intitule */
	  }
	number = i;
     }
#endif /* _GTK */
   /* Faut-il simplement mettre a jour le selecteur ? */
   if (rebuilded)
     {
	/* On met a jour le titre du selecteur */
	if (catalogue->Cat_Title != 0 && title != NULL)
	  {
#ifndef _GTK 
	     n = 0;
	     title_string = XmStringCreateSimple (title);
	     XtSetArg (args[n], XmNlabelString, title_string);
	     n++;
	     XtSetValues (catalogue->Cat_Title, args, n);
	     XtManageChild (catalogue->Cat_Title);
	     XmStringFree (title_string);
#else /* _GTK */
	     /* update the title label */
	     gtk_label_set_text (GTK_LABEL(catalogue->Cat_Title),title);
	     gtk_widget_show (GTK_WIDGET(catalogue->Cat_Title));
#endif /* _GTK */
	  }
	/* On met a jour le label attache au bouton du selecteur */
	if (catalogue->Cat_SelectLabel != 0 && label != NULL)
	  {
#ifndef _GTK
	     n = 0;
	     title_string = XmStringCreateSimple (label);
	     XtSetArg (args[n], XmNlabelString, title_string);
	     n++;
	     XtSetValues (catalogue->Cat_SelectLabel, args, n);
	     XtManageChild (catalogue->Cat_SelectLabel);
	     XmStringFree (title_string);
#else /* _GTK */
	     /* update the selector label*/
	     gtk_label_set_text (GTK_LABEL(gtk_object_get_data (GTK_OBJECT(catalogue->Cat_SelectLabel),"ButtonLabel")), label);
	     gtk_widget_show (GTK_WIDGET (catalogue->Cat_SelectLabel));
#endif /* _GTK */
	  }
	/* On met a jour le selecteur (catalogue->Cat_Entries) */
	catalogue->Cat_ListLength = number;
#ifndef _GTK
	n = 0;
#endif /* _GTK */
	w = (ThotWidget) catalogue->Cat_Entries;
#ifndef _GTK
	if (catalogue->Cat_SelectList)
	  {
	     /* Une simple liste */
	     XtSetArg (args[n], XmNitems, item);
	     n++;
	     XtSetArg (args[n], XmNitemCount, number);
	     n++;

	     /* Detruit l'ancienne liste */
	     XmListDeleteAllItems (w);
	     /* Remplace par la nouvelle liste */
	     XtSetValues (w, args, n);
	  }
	else
	  {
	     /* Un selecteur */
	     XtSetArg (args[n], XmNlistItems, item);
	     n++;
	     XtSetArg (args[n], XmNlistItemCount, number);
	     n++;
	     /*XtSetArg(args[n], XmNlistVisibleItemCount, height); n++; */
	     title_string = XmStringCreateSimple ("");
	     XtSetArg (args[n], XmNselectionLabelString, title_string);
	     n++;

	     /* Detruit l'ancienne liste */
	     wt = XmSelectionBoxGetChild (w, XmDIALOG_LIST);
	     XmListDeleteAllItems (wt);
	     /* Remplace par la nouvelle liste */
	     XtSetValues (w, args, n);
	     XmStringFree (title_string);
	  }
	XtManageChild (w);
#else /* _GTK */
	/* delete the old list elements */
	if ((gint)gtk_object_get_data(GTK_OBJECT(w), "GList")>0)
	  gtk_list_clear_items (GTK_LIST (w), 0, (gint)gtk_object_get_data(GTK_OBJECT (w), "GList"));
	/* add the new list elements */
	gtk_list_append_items (GTK_LIST(w), item);
	/* update the number of list element */
	gtk_object_set_data(GTK_OBJECT(w), "GList", (gpointer)g_list_length (item));
	gtk_widget_show (GTK_WIDGET(w));
#endif /* _GTK */
     }
   else
     {
       /*_______________________________________ C'est un nouveau formulaire __*/
	w = AddInFormulary (parentCatalogue, &i, &ent, &adbloc);
#ifndef _GTK
	/* Cree un sous-menu d'un formulaire */
	/*** Cree un Row-Column dans le Row-Column du formulaire ***/
	n = 0;
	XtSetArg (args[n], XmNbackground, BgMenu_Color);
	n++;
	if (width != 0)
	  {
	    XtSetArg (args[n], XmNwidth, (Dimension) width);
	    n++;
	    XtSetArg (args[n], XmNresizeWidth, FALSE);
	    n++;
	  }
	XtSetArg (args[n], XmNmarginWidth, 0);
	n++;
	XtSetArg (args[n], XmNmarginHeight, 0);
	n++;
	XtSetArg (args[n], XmNspacing, 0);
	n++;
	row = XmCreateRowColumn (w, "Dialogue", args, n);
#else /* _GTK */	
	row = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (row);
	gtk_widget_set_name (row, "Dialogue");
	gtk_container_add (GTK_CONTAINER (w), row);
#endif /* _GTK */
	catalogue->Cat_Ref = ref;
	catalogue->Cat_Type = CAT_SELECT;
	catalogue->Cat_ListLength = number;
	catalogue->Cat_Widget = row;
	adbloc->E_ThotWidget[ent] = (ThotWidget) catalogue;
	adbloc->E_Free[ent] = 'N';
	catalogue->Cat_EntryParent = i;
	catalogue->Cat_Title = 0;
	/*** Cree le titre du selecteur ***/
	if (title != NULL)
	  {
#ifndef _GTK
	     n = 0;
	     title_string = XmStringCreateSimple (title);
	     XtSetArg (args[n], XmNfontList, DefaultFont);
	     n++;
	     XtSetArg (args[n], XmNlabelString, title_string);
	     n++;
	     XtSetArg (args[n], XmNmarginHeight, 0);
	     n++;
	     XtSetArg (args[n], XmNbackground, BgMenu_Color);
	     n++;
	     XtSetArg (args[n], XmNforeground, FgMenu_Color);
	     n++;
	     w = XmCreateLabel (row, "Dialogue", args, n);
	     XtManageChild (w);
	     catalogue->Cat_Title = w;
	     XmStringFree (title_string);
#else /* _GTK */
	     w = gtk_label_new (title);
	     gtk_misc_set_alignment (GTK_MISC (w), 0.0, 0.5);
	     gtk_widget_show (w);
	     w->style->font=DefaultFont;
	     gtk_box_pack_start (GTK_BOX(row), w, FALSE, FALSE, 0);
	     catalogue->Cat_Title = w;
#endif /* _GTK */
	  }
	/*** Cree le label attache au selecteur ***/
	if (label != NULL)
	  {
#ifndef _GTK
	     n = 0;
	     XtSetArg (args[n], XmNfontList, DefaultFont);
	     n++;
	     XtSetArg (args[n], XmNbackground, BgMenu_Color);
	     n++;
	     XtSetArg (args[n], XmNforeground, FgMenu_Color);
	     n++;
	     w = XmCreatePushButton (row, label, args, n);
	     XtManageChild (w);
	     catalogue->Cat_SelectLabel = w;
	     XtAddCallback (w, XmNactivateCallback, (XtCallbackProc) CallLabel, catalogue);
#else /* _GTK */
	     w = gtk_button_new ();
	     gtk_widget_show (w);
	     w->style->font=DefaultFont;
	     tmpw = gtk_label_new (label);
	     gtk_misc_set_alignment (GTK_MISC (tmpw), 0.0, 0.5);
	     gtk_widget_show(tmpw);
	     gtk_label_set_justify (GTK_LABEL (tmpw), GTK_JUSTIFY_LEFT);
	     gtk_object_set_data(GTK_OBJECT(w), "ButtonLabel", 	   
				 (gpointer)tmpw);
	     gtk_container_add(GTK_CONTAINER(w),GTK_WIDGET(tmpw));
	     gtk_box_pack_start (GTK_BOX(row), w, FALSE, FALSE, 0);
	     ConnectSignalGTK (GTK_OBJECT(w), "clicked", GTK_SIGNAL_FUNC (CallLabel), (gpointer)catalogue);
	     catalogue->Cat_SelectLabel = w;
#endif /* _GTK */
	  }
	else
	   catalogue->Cat_SelectLabel = 0;
#ifndef _GTK
	n = 0;
	XtSetArg (args[n], XmNmarginWidth, 0);
	n++;
	XtSetArg (args[n], XmNmarginHeight, 0);
	n++;
	XtSetArg (args[n], XmNspacing, 0);
	n++;
	XtSetArg (args[n], XmNbackground, BgMenu_Color);
	n++;
	XtSetArg (args[n], XmNforeground, FgMenu_Color);
	n++;
	XtSetArg (args[n], XmNfontList, DefaultFont);
	n++;
	if (catalogue->Cat_SelectList)
	  {
	     /* Une simple liste */
	     XtSetArg (args[n], XmNselectionPolicy, XmSINGLE_SELECT);
	     n++;
	     if (width == 0)
	       {
		 XtSetArg (args[n], XmNlistSizePolicy, XmVARIABLE);
		 n++;
	       }
	     else
	       {
		 XtSetArg (args[n], XmNlistSizePolicy, XmCONSTANT);
		 n++;
	       }
	     XtSetArg (args[n], XmNitems, item);
	     n++;
	     XtSetArg (args[n], XmNitemCount, number);
	     n++;
	     XtSetArg (args[n], XmNvisibleItemCount, height);
	     n++;
	     w = XmCreateScrolledList (row, title, args, n);
	     XtManageChild (w);
	     /* Si le selecteur est reactif */
	     if (react)
		XtAddCallback (w, XmNsingleSelectionCallback, (XtCallbackProc) CallList, catalogue);
	  }
	else
	  {
	     /* Un selecteur */
	     XtSetArg (args[n], XmNlistItems, item);
	     n++;
	     XtSetArg (args[n], XmNlistItemCount, number);
	     n++;
	     XtSetArg (args[n], XmNlistVisibleItemCount, height);
	     n++;
	     XtSetArg (args[n], XmNdialogType, XmDIALOG_COMMAND);
	     n++;
	     if (TextTranslations != NULL)
	       {
		  XtSetArg (args[n], XmNtextTranslations, TextTranslations);
		  n++;
	       }
	     title_string = XmStringCreateSimple ("");
	     XtSetArg (args[n], XmNselectionLabelString, title_string);
	     n++;
	     w = XmCreateSelectionBox (row, title, args, n);
	     XtManageChild (w);
	     XmStringFree (title_string);
	     /* Controle la couleur des widgets fils */
	     n = 0;
	     XtSetArg (args[n], XmNbackground, BgMenu_Color);
	     n++;
	     XtSetArg (args[n], XmNforeground, FgMenu_Color);
	     n++;
	     XtSetArg (args[n], XmNfontList, DefaultFont);
	     n++;
	     XtSetArg (args[n], XmNtroughColor, BgMenu_Color);
	     n++;
	     wt = XmSelectionBoxGetChild (w, XmDIALOG_LIST);
	     XtSetValues (wt, args, n);
	     wt = XmSelectionBoxGetChild (w, XmDIALOG_TEXT);
	     XtSetValues (wt, args, n - 1);
	     /* Si le selecteur est reactif */
	     if (react)
		XtAddCallback (wt, XmNvalueChangedCallback, (XtCallbackProc) CallTextChange, catalogue);
	  }
#else /* _GTK */
	if (catalogue->Cat_SelectList)
	  {
	    /* A simple list */
	    /* Create a scrolled window to control the gtklist & gtklist contain the items */
	    tmpw = gtk_scrolled_window_new (NULL, NULL);
	    gtk_widget_show (tmpw);
	    tmpw->style->font=DefaultFont;
	    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (tmpw), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	    gtk_box_pack_start (GTK_BOX(row), tmpw, TRUE, TRUE, 0);
	    gtk_widget_set_usize (tmpw, width, height*30);
	    w = gtk_list_new ();
	    /* tie the scrolling of the list to the scrolled window */
	    gtk_container_set_focus_hadjustment (GTK_CONTAINER (w),
						 gtk_scrolled_window_get_hadjustment 
						 (GTK_SCROLLED_WINDOW (tmpw)));
	    gtk_container_set_focus_vadjustment (GTK_CONTAINER (w),
						 gtk_scrolled_window_get_vadjustment 
						 (GTK_SCROLLED_WINDOW (tmpw)));
	    gtk_widget_show (GTK_WIDGET(w));
	    gtk_list_set_selection_mode (GTK_LIST(w),GTK_SELECTION_SINGLE);
	    gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW(tmpw),w);
	    gtk_list_append_items (GTK_LIST(w), item);
	    gtk_object_set_data (GTK_OBJECT(w), "GList", (gpointer)g_list_length(item));
	    gtk_widget_show_all (tmpw);
	    if (react)
	      {
	      ConnectSignalGTK (GTK_OBJECT(w), "selection_changed",
				GTK_SIGNAL_FUNC(CallListGTK), (gpointer)catalogue);
	      ConnectSignalGTK (GTK_OBJECT(w), "button_release_event",
				GTK_SIGNAL_FUNC (CallTextEnterGTK),  (gpointer)catalogue);
	      }
	    }
	else
	  {
	    /* A list and a text entry zone */
	    tmpw = gtk_scrolled_window_new (NULL, NULL);
	    gtk_widget_show (tmpw);
	    tmpw->style->font=DefaultFont;
	    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (tmpw),
					    GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	    gtk_box_pack_start (GTK_BOX(row), tmpw, TRUE, TRUE, 0);
	    gtk_widget_set_usize (tmpw, width, height*30);
	    w = gtk_list_new ();
	    /* tie the scrolling of the list to the scrolled window */
	    gtk_container_set_focus_hadjustment (GTK_CONTAINER (w),
						 gtk_scrolled_window_get_hadjustment 
						 (GTK_SCROLLED_WINDOW (tmpw)));
	    gtk_container_set_focus_vadjustment (GTK_CONTAINER (w),
						 gtk_scrolled_window_get_vadjustment 
						 (GTK_SCROLLED_WINDOW (tmpw)));
	    gtk_widget_show (GTK_WIDGET(w));
	    gtk_list_set_selection_mode (GTK_LIST(w), GTK_SELECTION_SINGLE);
	    gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW(tmpw),w);
	    gtk_list_append_items (GTK_LIST(w), item);
	    gtk_object_set_data (GTK_OBJECT(w), "GList", (gpointer)g_list_length(item));
	    gtk_widget_show_all (tmpw);
	    /* on connecte le changement de selection pour mettre a jour la zone text */
	    ConnectSignalGTK (GTK_OBJECT(w), "selection_changed",
			      GTK_SIGNAL_FUNC(CallListGTK), (gpointer)catalogue);
	    ConnectSignalGTK (GTK_OBJECT(w), "button_press_event",
			       GTK_SIGNAL_FUNC (CallTextEnterGTK),  (gpointer)catalogue);
	    tmpw = gtk_entry_new ();
	    gtk_widget_show (tmpw);
	    tmpw->style->font = DefaultFont;
	    gtk_box_pack_start (GTK_BOX(row), tmpw, FALSE, FALSE, 0);
	    gtk_object_set_data (GTK_OBJECT(w), "EntryZone", (gpointer)tmpw);
	    if (react)
	      ConnectSignalGTK (GTK_OBJECT(tmpw), "changed",
				GTK_SIGNAL_FUNC(CallTextChangeGTK), (gpointer)catalogue);
	  }
	if (!catalogue->Cat_Focus)
	  {
	    /* first entry in the form */
	    gtk_widget_grab_focus (GTK_WIDGET(w));
	    catalogue->Cat_Focus = TRUE;
	  }
#endif /* _GTK */
	/* Conserve le widget du selecteur dans l'entree Cat_Entries */
	catalogue->Cat_Entries = (struct E_List *) w;
     }
#ifndef _GTK
   /* Libere les XmString allouees */
   i = 0;
   while (item[i] != NULL)
     {
	XmStringFree (item[i]);
	i++;
     }
   TtaFreeMemory ( item);
#endif
}

/*----------------------------------------------------------------------
   TtaNewSelector creates a selector in a dialogue form:
   The parameter ref donne la re'fe'rence pour l'application.
   The parameter title donne le titre du catalogue.             
   The parameter number donne le nombre d'intitule's.          
   The parameter text contient la liste des intitule's.   
   The parameter height donne le nombre d'intitule's visibles a` la
   fois (hauteur de la fenetree^tre de visualisation).
   The parameter label ajoute un choix supple'mentaire a` la liste
   des choix possibles. Ce choix est affiche' se'pare'ment et donc
   mis en e'vidence. 
   The parameter withText indique s'il faut creer une zone texte.
   Quand le parame`tre react est vrai, tout changement de se'lection
   dans le se'lecteur est imme'diatement signale' a` l'application.
  ----------------------------------------------------------------------*/
void TtaNewSelector (int ref, int ref_parent, char *title, int number,
		     char *text, int height, char *label,
		     ThotBool withText, ThotBool react)
{
  TtaNewSizedSelector (ref, ref_parent, title, number, text, 0, height,
		       label, withText, react);
}

/*----------------------------------------------------------------------
   TtaActiveSelector rend actif le  se'lecteur.                       
  ----------------------------------------------------------------------*/
void TtaActiveSelector (int ref)
{
   ThotWidget          w;
   struct Cat_Context *catalogue;
#ifndef _GTK
   Arg                 args[MAX_ARGS];
#endif /* _GTK */

   catalogue = CatEntry (ref);
   if (catalogue == NULL)
      TtaError (ERR_invalid_reference);
   else if (catalogue->Cat_Widget == 0)
      TtaError (ERR_invalid_reference);
   else if (catalogue->Cat_Type != CAT_SELECT)
      TtaError (ERR_invalid_reference);
   else
     {
#ifndef _GTK
	/* Recupere le widget du selecteur */
	w = (ThotWidget) catalogue->Cat_Entries;
	XtSetArg (args[0], XmNsensitive, TRUE);
	XtSetValues (w, args, 1);
	XtManageChild (w);
#else /* _GTK */
	w = (ThotWidget) catalogue->Cat_Entries;
	gtk_widget_set_sensitive (GTK_WIDGET(w), TRUE);
	gtk_widget_show (GTK_WIDGET(w));
#endif /* _GTK */
     }
}

/*----------------------------------------------------------------------
   TtaDesactiveSelector rend non actif le  se'lecteur.                        
  ----------------------------------------------------------------------*/
void TtaDesactiveSelector (int ref)
{
  ThotWidget          w;
   struct Cat_Context *catalogue;
#ifndef _GTK
   Arg                 args[MAX_ARGS];
#endif /* _GTK */

   catalogue = CatEntry (ref);
   if (catalogue == NULL)
      TtaError (ERR_invalid_reference);
   else if (catalogue->Cat_Widget == 0)
      TtaError (ERR_invalid_reference);
   else if (catalogue->Cat_Type != CAT_SELECT)
      TtaError (ERR_invalid_reference);
   else
     {
#ifndef _GTK
	/* Recupere le widget du selecteur */
	w = (ThotWidget) catalogue->Cat_Entries;
	XtSetArg (args[0], XmNsensitive, FALSE);
	XtSetValues (w, args, 1);
	XtManageChild (w);
#else /* _GTK */
	w = (ThotWidget) catalogue->Cat_Entries;
	gtk_widget_show (GTK_WIDGET(w));
	gtk_widget_set_sensitive (GTK_WIDGET(w), FALSE);
#endif /* _GTK */
     }
}

/*----------------------------------------------------------------------
   TtaSetSelector initialise l'entre'e et/ou le texte du se'lecteur : 
   The parameter ref donne la re'fe'rence du catalogue.               
   The parameter entry positif ou nul donne l'index de l'entre'e      
   se'lectionne'e.                                                    
   The parameter text donne le texte si entry vaut -1.                
  ----------------------------------------------------------------------*/
void TtaSetSelector (int ref, int entry, char *text)
{
#ifndef _GTK
   ThotWidget          w;
#endif /* _GTK */
   ThotWidget          wt;
   ThotWidget          select;
   struct Cat_Context *catalogue;

   catalogue = CatEntry (ref);
   wt = 0;
   if (catalogue == NULL)
      TtaError (ERR_invalid_reference);
   else if (catalogue->Cat_Widget != 0)
     {
	/* Recupere le widget du selecteur */
	select = (ThotWidget) catalogue->Cat_Entries;
	if (catalogue->Cat_Type != CAT_SELECT)
	  {
	     TtaError (ERR_invalid_reference);
	     return;
	  }
#ifndef _GTK
	if (!catalogue->Cat_SelectList)
	  {
	     wt = XmSelectionBoxGetChild (select, XmDIALOG_TEXT);
	     /* Si le selecteur est reactif */
	     if (catalogue->Cat_React)
		XtRemoveCallback (wt, XmNvalueChangedCallback, (XtCallbackProc) CallTextChange, catalogue);
	  }
	else if (catalogue->Cat_React)
	   XtRemoveCallback (select, XmNsingleSelectionCallback, (XtCallbackProc) CallList, catalogue);
#else /* _GTK */
	if (!catalogue->Cat_SelectList)
	  {
	    wt = GTK_WIDGET (gtk_object_get_data (GTK_OBJECT(select), "EntryZone"));
	    /* Si le selecteur est reactif */
	    if (catalogue->Cat_React)
	      RemoveSignalGTK (GTK_OBJECT(wt), "changed");  
	  }
	else if (catalogue->Cat_React)
	  {
	    RemoveSignalGTK (GTK_OBJECT(select), "selection_changed"); 
	    RemoveSignalGTK (GTK_OBJECT(select), "button_press_event"); 
	  }
#endif /* _GTK */
	if (entry >= 0 && entry < catalogue->Cat_ListLength)
	  {
#ifndef _GTK
	     /* Initialise l'entree de la liste */
	     if (catalogue->Cat_SelectList)
		XmListSelectPos (select, entry + 1, TRUE);
	     else
	       {
		  w = XmSelectionBoxGetChild (select, XmDIALOG_LIST);
		  XmListSelectPos (w, entry + 1, TRUE);
	       }
#else /* _GTK */
	     gtk_list_select_item (GTK_LIST(select), entry);
#endif /* _GTK */
	  }
	else if (catalogue->Cat_SelectList)
#ifndef _GTK
	   XmListDeselectAllItems (select);
#else /* _GTK */
	   gtk_list_unselect_all (GTK_LIST(select));
#endif /* _GTK */
	else
	  {
#ifndef _GTK
	     /* Initialise le champ texte */
	     if (catalogue->Cat_ListLength != 0)
	       {
		  /* Deselectionne dans la liste */
		  w = XmSelectionBoxGetChild (select, XmDIALOG_LIST);
		  XmListDeselectAllItems (w);
	       }
	     XmTextSetString (wt, text);
#else /* _GTK */
	     /* Initialise le champ texte */
	     if (catalogue->Cat_ListLength != 0)
	       {
		 gtk_list_unselect_all (GTK_LIST(select));
	       }
	     if (!text)
	       gtk_entry_set_text (GTK_ENTRY (wt), "");
	     else
	       gtk_entry_set_text (GTK_ENTRY (wt), text);
#endif /* _GTK */
	  }

	/* Si le selecteur est reactif */
	if (catalogue->Cat_React)
	  {
#ifndef _GTK
	   if (catalogue->Cat_SelectList)
	      XtAddCallback (select, XmNsingleSelectionCallback,
			     (XtCallbackProc) CallList, catalogue);
	   else
	      XtAddCallback (wt, XmNvalueChangedCallback,
			     (XtCallbackProc) CallTextChange, catalogue);
#else /* _GTK */
	   if (catalogue->Cat_SelectList){
	     ConnectSignalGTK (GTK_OBJECT(select),"selection_changed"
			       , GTK_SIGNAL_FUNC(CallListGTK), (gpointer)catalogue);
	     ConnectSignalGTK (GTK_OBJECT(select), "button_press_event",
			       GTK_SIGNAL_FUNC (CallTextEnterGTK),
			       (gpointer)catalogue);
	   }
	   else
	     ConnectSignalGTK (GTK_OBJECT(wt),"changed",
			       GTK_SIGNAL_FUNC(CallTextChangeGTK), (gpointer)catalogue);
#endif /* _GTK */
	  }
     }
}

/*----------------------------------------------------------------------
   TtaNewLabel cre'e un intitule' constant dans un formulaire :       
   The parameter ref donne la re'fe'rence du catalogue.               
   The parameter text donne l'intitule'.                              
  ----------------------------------------------------------------------*/
void TtaNewLabel (int ref, int ref_parent, char *text)
{
#ifndef _GTK
   Arg                 args[MAX_ARGS];
   XmString            title_string;
   int                 n;
#else
   ThotWidget          tmpw;
#endif /* _GTK */
   int                 i;
   int                 ent;
   int                 rebuilded;
   struct E_List      *adbloc;
   ThotWidget          w;
   struct Cat_Context *catalogue;
   struct Cat_Context *parentCatalogue;

   if (ref == 0)
     {
	TtaError (ERR_invalid_reference);
	return;
     }
   /*   title_string = 0;*/
   catalogue = CatEntry (ref);
   rebuilded = 0;
   if (catalogue == NULL)
     {
       TtaError (ERR_cannot_create_dialogue);
       return;
     }
   else if (catalogue->Cat_Widget != 0 && catalogue->Cat_Type == CAT_LABEL)
     {
	/* Modification du catalogue */
	w = catalogue->Cat_Widget;
#ifndef _GTK
	/* Regarde si le widget est affiche */
	if (XtIsManaged (w))
	   rebuilded = 2;
	else
	   rebuilded = 1;
	n = 0;
	title_string = XmStringCreateSimple (text);
	XtSetArg (args[n], XmNlabelString, title_string);
	n++;
	XtSetValues (w, args, n);
	/* Faut-il reafficher le widget ? */
	if (rebuilded == 2)
	   XtManageChild (w);
#else /* _GTK */
	gtk_widget_show (w);
	gtk_label_set_text (GTK_LABEL (w), text);	
#endif /* _GTK */
     }
   else
     {
	if (catalogue->Cat_Widget != 0)
	   /* Le catalogue est a reconstruire completement */
	   TtaDestroyDialogue (ref);
	/*======================================> Recherche le catalogue parent */
	parentCatalogue = CatEntry (ref_parent);
	/*__________________________________ Le catalogue parent n'existe pas __*/
	if (parentCatalogue == NULL)
	  {
	     TtaError (ERR_invalid_parent_dialogue);
	     return;
	  }
	else if (parentCatalogue->Cat_Widget == 0)
	  {
	     TtaError (ERR_invalid_parent_dialogue);
	     return;
	  }
	/*_________________________________________ Sous-menu d'un formulaire __*/
	else if ((parentCatalogue->Cat_Type != CAT_FORM)
		 && (parentCatalogue->Cat_Type != CAT_SHEET)
		 && (parentCatalogue->Cat_Type != CAT_DIALOG))
	  {
	     TtaError (ERR_invalid_parent_dialogue);
	     return;
	  }
	else if (text == NULL)
	  {
	     TtaError (ERR_invalid_parameter);
	     return;
	  }
	/* Recupere le widget parent */
	w = AddInFormulary (parentCatalogue, &i, &ent, &adbloc);
#ifndef _GTK
	/*** Cree l'intitule ***/
	n = 0;
	title_string = XmStringCreateSimple (text);
	XtSetArg (args[n], XmNfontList, DefaultFont);
	n++;
	XtSetArg (args[n], XmNlabelString, title_string);
	n++;
	XtSetArg (args[n], XmNbackground, BgMenu_Color);
	n++;
	XtSetArg (args[n], XmNforeground, FgMenu_Color);
	n++;
	/*XtSetArg (args[n], XmNborderColor, BgMenu_Color);
	n++;*/
	w = XmCreateLabel (w, "Dialogue", args, n);
#else /* _GTK */
	tmpw = gtk_label_new (text);
	gtk_misc_set_alignment (GTK_MISC (tmpw), 0.0, 0.5);
	gtk_widget_show (GTK_WIDGET(tmpw));
	tmpw->style->font=DefaultFont;
	gtk_label_set_justify (GTK_LABEL (tmpw), GTK_JUSTIFY_LEFT);
	gtk_box_pack_start (GTK_BOX(w), GTK_WIDGET(tmpw), FALSE, FALSE, 0);
	/* on fou les couleurs (A FAIRE)*/
	gtk_widget_set_name (tmpw, "Dialogue");
	w=tmpw;
#endif /* _GTK */
	catalogue->Cat_Widget = w;
	catalogue->Cat_Ref = ref;
	catalogue->Cat_Type = CAT_LABEL;
	catalogue->Cat_PtParent = parentCatalogue;
	adbloc->E_ThotWidget[ent] = (ThotWidget) (catalogue);
	adbloc->E_Free[ent] = 'N';
	catalogue->Cat_EntryParent = i;
	catalogue->Cat_Entries = NULL;
     }
#ifndef _GTK
   XmStringFree (title_string);
#endif /* _GTK */
}

/*----------------------------------------------------------------------
   TtaNewButton cree un nouveau bouton dans un formulaire       
   The parameter ref donne la re'fe'rence du catalogue.               
   The parameter text donne l'intitule' du boutton.                              
  ----------------------------------------------------------------------*/
void TtaNewButton (int ref, int ref_parent, char *text)
{
}

/*----------------------------------------------------------------------
   TtaNewTextForm creates a dialogue element to input text:   
   Parameter ref gives the Thot reference.
   Parameter title gives the dialogue title.
   Parameters width and height give the box size. 
   If the parameter react is TRUE, any change in the input box generates a
   callback to the application.
  ----------------------------------------------------------------------*/
void TtaNewTextForm (int ref, int ref_parent, char *title, int width,
 		     int height, ThotBool react)
{
   int                 ent;
   int                 i;
   struct Cat_Context *catalogue;
   struct Cat_Context *parentCatalogue;
   struct E_List      *adbloc;
   ThotWidget          w;
   ThotWidget          row;
#ifndef _GTK
   Arg                 args[MAX_ARGS];
   XmString            title_string;
   int                 n;
#else /* _GTK */
   ThotWidget          tmpw;
#endif /* _GTK */

   if (ref == 0)
     {
	TtaError (ERR_invalid_reference);
	return;
     }
   catalogue = CatEntry (ref);
   if (catalogue == NULL)
      TtaError (ERR_cannot_create_dialogue);
   else
     {
	catalogue->Cat_React = react;
	/* Faut-il detruire le catalogue precedent ? */
	if (catalogue->Cat_Widget != 0)
	   DestForm (ref);	/* Modification du catalogue */
	/*======================================> Recherche le catalogue parent */
	parentCatalogue = CatEntry (ref_parent);
	/*__________________________________ Le catalogue parent n'existe pas __*/
	if (parentCatalogue == NULL)
	  {
	     TtaError (ERR_invalid_parent_dialogue);
	     return;
	  }
	else if (parentCatalogue->Cat_Widget == 0)
	  {
	     TtaError (ERR_invalid_parent_dialogue);
	     return;
	  }
	/*_______________________ Le catalogue parent n'est pas un formulaire __*/
	else if ((parentCatalogue->Cat_Type != CAT_FORM)
		 && (parentCatalogue->Cat_Type != CAT_SHEET)
		 && (parentCatalogue->Cat_Type != CAT_DIALOG))
	  {
	     TtaError (ERR_invalid_parent_dialogue);
	     return;
	  }
	/*_____________________________________________________________ Sinon __*/
	else
	  {
	     /* Cree a l'interieur Row-Column du formulaire */
	     row = AddInFormulary (parentCatalogue, &i, &ent, &adbloc);
#ifndef _GTK
	     row = AddInFormulary (parentCatalogue, &i, &ent, &adbloc);
	     n = 0;
	     XtSetArg (args[n], XmNbackground, BgMenu_Color);
	     n++;
	     XtSetArg (args[n], XmNmarginWidth, 0);
	     n++;
	     XtSetArg (args[n], XmNmarginHeight, 0);
	     n++;
	     XtSetArg (args[n], XmNspacing, 0);
	     n++;
	     row = XmCreateRowColumn (row, "Dialogue", args, n);
	     /*** Cree le titre ***/
	     if (title != NULL)
	       {
		  n = 0;
		  title_string = XmStringCreateSimple (title);
		  XtSetArg (args[n], XmNfontList, DefaultFont);
		  n++;
		  XtSetArg (args[n], XmNlabelString, title_string);
		  n++;
		  XtSetArg (args[n], XmNmarginWidth, 0);
		  n++;
		  XtSetArg (args[n], XmNmarginHeight, 0);
		  n++;
		  XtSetArg (args[n], XmNbackground, BgMenu_Color);
		  n++;
		  XtSetArg (args[n], XmNforeground, FgMenu_Color);
		  n++;
		  w = XmCreateLabel (row, "Dialogue", args, n);
		  XtManageChild (w);
		  XmStringFree (title_string);
	       }		/*if */
	     /* Cree une feuille de saisie de texte */
	     n = 0;
	     if (width == 0)
		XtSetArg (args[n], XmNcolumns, 10);
	     else
		XtSetArg (args[n], XmNcolumns, width);
	     n++;
	     XtSetArg (args[n], XmNbackground, BgMenu_Color);
	     n++;
	     XtSetArg (args[n], XmNforeground, FgMenu_Color);
	     n++;
	     XtSetArg (args[n], XmNfontList, DefaultFont);
	     n++;
	     if (TextTranslations != NULL)
		XtSetArg (args[n], XmNtranslations, TextTranslations);
	     n++;
	     if (height < 2)
	       {
		  XtSetArg (args[n], XmNeditMode, XmSINGLE_LINE_EDIT);
		  n++;
		  /*XtSetArg(args[n], XmNscrollVertical, FALSE); n++; */
		  w = XmCreateText (row, "Dialogue", args, n);
	       }
	     else
	       {
		  XtSetArg (args[n], XmNeditMode, XmMULTI_LINE_EDIT);
		  n++;
		  XtSetArg (args[n], XmNrows, (short) height);
		  n++;
		  w = XmCreateScrolledText (row, "Dialogue", args, n);
	       }
	     XtManageChild (w);
	     /* Si la feuille de saisie est reactive */
	     if (react)
		XtAddCallback (w, XmNvalueChangedCallback, (XtCallbackProc) CallTextChange, catalogue);
#else /* _GTK */
	     /* create the vbox for all the elements */
	     tmpw = gtk_vbox_new (FALSE, 0);
	     gtk_widget_show (GTK_WIDGET(tmpw));
	     tmpw->style->font = DefaultFont;
	     gtk_box_pack_start (GTK_BOX(row), GTK_WIDGET(tmpw), FALSE, FALSE, 0);
	     row = tmpw;
	     /* Create the label title */
	     if (title != NULL)
	       {
		 w = gtk_label_new (title);
		 gtk_misc_set_alignment (GTK_MISC (w), 0.0, 0.5);
		 gtk_widget_show (GTK_WIDGET(w));
		 w->style->font=DefaultFont;
		 gtk_widget_set_name (w, "Dialogue");
		 gtk_box_pack_start (GTK_BOX(row), w, FALSE, FALSE, 2);
	       }	
	     /* new text widget added into the row widget */
	     w = gtk_entry_new ();
	     gtk_widget_show (w);
	     w->style->font=DefaultFont;
	     gtk_box_pack_start (GTK_BOX(row), w, FALSE, FALSE, 2);
	     /* assigne the good size for the widget */
	     if (width == 0)
	       gtk_widget_set_usize (GTK_WIDGET(w),
				     10*gdk_char_width (DefaultFont, 'n'),
				     10+gdk_char_height (DefaultFont, '|'));
	     else
	       gtk_widget_set_usize (GTK_WIDGET(w),
				     (width)*gdk_char_width (DefaultFont, 'n'),
				     10+gdk_char_height (DefaultFont, '|'));
	     if (!parentCatalogue->Cat_Focus)
	       {
		 /* first entry in the form */
		 gtk_widget_grab_focus (GTK_WIDGET(w));
		 parentCatalogue->Cat_Focus = TRUE;
	       }
	     /* if the widget is reactive */
	     if (react)
	       ConnectSignalGTK (GTK_OBJECT(w), "changed",
				 GTK_SIGNAL_FUNC (CallTextChangeGTK), (gpointer)catalogue); 
	     /* report Enter to the form */
	     ConnectSignalGTK (GTK_OBJECT(w), "activate",
				 GTK_SIGNAL_FUNC (CallEnter), (gpointer) parentCatalogue);
#endif /* _GTK */
	     catalogue->Cat_Ref = ref;
	     catalogue->Cat_Type = CAT_TEXT;
	     /* L'entree Cat_Entries contient le numero du widget texte */
	     catalogue->Cat_Entries = (struct E_List *) w;
	     catalogue->Cat_Widget = row;
	     catalogue->Cat_PtParent = parentCatalogue;
	     adbloc->E_ThotWidget[ent] = (ThotWidget) catalogue;
	     adbloc->E_Free[ent] = 'N';
	     catalogue->Cat_EntryParent = i;
	  }
     }
}

/*----------------------------------------------------------------------
   TtaNewPwdForm creates a dialogue element to input password text:   
   Parameter ref gives the Thot reference.
   Parameter title gives the dialogue title.
   Parameters width and height give the box size. 
   If the parameter react is TRUE, any change in the input box generates a
   callback to the application.
  ----------------------------------------------------------------------*/
void TtaNewPwdForm (int ref, int ref_parent, char *title, int width,
 		     int height, ThotBool react)
{
#ifdef _GTK
  struct Cat_Context *catalogue;

  TtaNewTextForm (ref, ref_parent, title, width, height, react);
  catalogue = CatEntry (ref);
  gtk_entry_set_visibility (GTK_ENTRY(catalogue->Cat_Entries), FALSE);
#endif /* GTK */
}
/*----------------------------------------------------------------------
   TtaSetTextForm initialise une feuille de saisie de texte :         
   The parameter ref donne la re'fe'rence du catalogue.               
   The parameter text donne la valeur initiale.                       
  ----------------------------------------------------------------------*/
void TtaSetTextForm (int ref, char *text)
{
#ifndef _GTK
   int                 lg;
#endif /* _GTK */
   struct Cat_Context *catalogue;
   ThotWidget          w;

   catalogue = CatEntry (ref);
   if (catalogue == NULL)
      TtaError (ERR_invalid_reference);
   else if (catalogue->Cat_Widget == 0)
      TtaError (ERR_invalid_reference);
   else if (text == NULL)
      TtaError (ERR_invalid_reference);
   else if (catalogue->Cat_Type != CAT_TEXT)
      TtaError (ERR_invalid_reference);
   else 
     {
        w = (ThotWidget) catalogue->Cat_Entries;
        /* Si la feuille de saisie est reactive */
#ifndef _GTK
        if (catalogue->Cat_React)
	  XtRemoveCallback (w, XmNvalueChangedCallback,
			    (XtCallbackProc) CallTextChange, catalogue);
        XmTextSetString (w, text);
        lg = strlen (text);
        XmTextSetSelection (w, lg, lg, 500);
        /* Si la feuille de saisie est reactive */
        if (catalogue->Cat_React)
	  XtAddCallback (w, XmNvalueChangedCallback,
			 (XtCallbackProc) CallTextChange, catalogue);
#else /* _GTK */
        if (catalogue->Cat_React)
	  RemoveSignalGTK (GTK_OBJECT(w), "changed");  
	gtk_entry_set_text (GTK_ENTRY (w), text);
	/*gtk_editable_select_region(GTK_EDITABLE(w), 0, -1);*/
        if (catalogue->Cat_React)
	  ConnectSignalGTK (GTK_OBJECT(w), "changed",
			    GTK_SIGNAL_FUNC(CallTextChangeGTK), (gpointer)catalogue);
#endif /* _GTK */
     }
}

/*----------------------------------------------------------------------
   TtaNewNumberForm cre'e une feuille de saisie de nombre :           
   The parameter ref donne la re'fe'rence pour l'application.         
   The parameter ref_parent identifie le formulaire pe`re.            
   The parameter title donne le titre du catalogue.                   
   Les parame`tres min et max fixent les bornes valides du nombre.    
   Quand le parame`tre react est vrai, tout changement dans           
   la feuille de saisie est imme'diatement signale' a` l'application. 
  ----------------------------------------------------------------------*/
void TtaNewNumberForm (int ref, int ref_parent, char *title, int min,
		       int max, ThotBool react)
{
   int                 ent;
   int                 i;
   struct Cat_Context *catalogue;
   struct Cat_Context *parentCatalogue;
   struct E_List      *adbloc;
#ifndef _GTK
   Arg                 args[MAX_ARGS];
   int                 n;
   XmString            title_string;
#else /* _GTK */
   ThotWidget          tmpw;
#endif /* _GTK */
   ThotWidget          w;
   ThotWidget          row;
   char                bounds[100];

   if (ref == 0)
     {
	TtaError (ERR_invalid_reference);
	return;
     }
   catalogue = CatEntry (ref);
   if (catalogue == NULL)
      TtaError (ERR_cannot_create_dialogue);
   else
     {
	catalogue->Cat_React = react;

	/* Faut-il detruire le catalogue precedent ? */
	if (catalogue->Cat_Widget != 0)
	   DestForm (ref);	/* Modification du catalogue */

	/*======================================> Recherche le catalogue parent */
	parentCatalogue = CatEntry (ref_parent);
	/*__________________________________ Le catalogue parent n'existe pas __*/
	if (parentCatalogue == NULL)
	  {
	     TtaError (ERR_invalid_parent_dialogue);
	     return;
	  }
	else if (parentCatalogue->Cat_Widget == 0)
	  {
	     TtaError (ERR_invalid_parent_dialogue);
	     return;
	  }
	/*_______________________ Le catalogue parent n'est pas un formulaire __*/
	else if ((parentCatalogue->Cat_Type != CAT_FORM)
		 && (parentCatalogue->Cat_Type != CAT_SHEET)
		 && (parentCatalogue->Cat_Type != CAT_DIALOG))
	  {
	     TtaError (ERR_invalid_parent_dialogue);
	     return;
	  }
	/*_____________________________________________________________ Sinon __*/
	else
	  {
	     row = AddInFormulary (parentCatalogue, &i, &ent, &adbloc);

#ifndef _GTK
	     /* Cree a l'interieur Row-Column du formulaire */
	     n = 0;
	     XtSetArg (args[n], XmNbackground, BgMenu_Color);
	     n++;
	     /*XtSetArg(args[n], XmNpacking, XmPACK_NONE); n++; */
	     XtSetArg (args[n], XmNadjustLast, FALSE);
	     n++;
	     XtSetArg (args[n], XmNmarginWidth, 0);
	     n++;
	     XtSetArg (args[n], XmNmarginHeight, 0);
	     n++;
	     XtSetArg (args[n], XmNspacing, 0);
	     n++;
	     row = XmCreateRowColumn (row, "Dialogue", args, n);
#else /* _GTK */
	     /* add a vbox to contain the elements */
	     tmpw = gtk_vbox_new (FALSE, 0);
	     gtk_widget_show (tmpw);
	     gtk_widget_set_name (tmpw, "Dialogue");
	     gtk_container_add (GTK_CONTAINER(row), tmpw);
	     row=tmpw;
#endif /* _GTK */
	     catalogue->Cat_Widget = row;
	     catalogue->Cat_PtParent = parentCatalogue;
	     adbloc->E_ThotWidget[ent] = (ThotWidget) catalogue;
	     adbloc->E_Free[ent] = 'N';
	     catalogue->Cat_EntryParent = i;
	     catalogue->Cat_Ref = ref;
	     catalogue->Cat_Type = CAT_INT;
	     /* Alloue un bloc pour ranger les widgets scale et text*/
	     adbloc = NewEList ();
	     catalogue->Cat_Entries = adbloc;
	     /*** Cree le titre du sous-menu ***/
	     if (title != NULL)
	       {
#ifndef _GTK
		  n = 0;
		  title_string = XmStringCreateSimple (title);
		  XtSetArg (args[n], XmNlabelString, title_string);
		  n++;
		  XtSetArg (args[n], XmNfontList, DefaultFont);
		  n++;
		  XtSetArg (args[n], XmNmarginWidth, 0);
		  n++;
		  XtSetArg (args[n], XmNmarginHeight, 0);
		  n++;
		  XtSetArg (args[n], XmNbackground, BgMenu_Color);
		  n++;
		  XtSetArg (args[n], XmNforeground, FgMenu_Color);
		  n++;
		  XtSetArg (args[n], XmNadjustLast, FALSE);
		  n++;
		  w = XmCreateLabel (row, "Dialogue", args, n);
		  XtManageChild (w);
		  /*n = 0;
		  XtSetArg (args[n], XmNbackground, BgMenu_Color);
		  n++;
		  XtSetArg (args[n], XmNseparatorType, XmSHADOW_ETCHED_OUT);
		  n++;
		  w = XmCreateSeparator (row, "Dialogue", args, n);
		  XtManageChild (w);*/
		  XmStringFree (title_string);
#else /* _GTK */
		  /* add a title label */
		  w = gtk_label_new (title);
		  gtk_misc_set_alignment (GTK_MISC (w), 0.0, 0.5);
		  gtk_widget_show (w);
		  gtk_label_set_justify (GTK_LABEL (w), GTK_JUSTIFY_LEFT);
		  gtk_widget_set_name (w, "Dialogue");
		  gtk_box_pack_start (GTK_BOX(row), w, FALSE, FALSE, 0);
#endif /* _GTK */
	       }
	     /* Cree une feuille de saisie d'un nombre */
	     if (min < max)
	       {
		 /* Note les bornes de l'echelle */
		 sprintf (bounds, "(%d", min);
		 strcat (&bounds[strlen (bounds)], "..");
		 sprintf (&bounds[strlen (bounds)], "%d)", max);
		 catalogue->Cat_Entries->E_ThotWidget[2] = (ThotWidget) min;
		 catalogue->Cat_Entries->E_ThotWidget[3] = (ThotWidget) max;
		 ent = max;
	       }
	     else
	       {
		 /* Note les bornes de l'echelle */
		 sprintf (bounds, "%d", max);
		 strcat (&bounds[strlen (bounds)], "..");
		 sprintf (&bounds[strlen (bounds)], "%d", min);
		 catalogue->Cat_Entries->E_ThotWidget[2] = (ThotWidget) max;
		 catalogue->Cat_Entries->E_ThotWidget[3] = (ThotWidget) min;
		 ent = min;
	       }
#ifndef _GTK
	     title_string = XmStringCreateSimple (bounds);
	     XtSetArg (args[n], XmNfontList, DefaultFont);
	     n++;
	     XtSetArg (args[n], XmNlabelString, title_string);
	     n++;
	     XtSetArg (args[n], XmNbackground, BgMenu_Color);
	     n++;
	     XtSetArg (args[n], XmNforeground, FgMenu_Color);
	     n++;
	     w = XmCreateLabel (row, "Dialogue", args, n);
	     XtManageChild (w);
	     XmStringFree (title_string);

	     /* Cree une feuille de saisie annexe */
	     n = 0;
	     XtSetArg (args[n], XmNeditMode, XmSINGLE_LINE_EDIT);
	     n++;
	     XtSetArg (args[n], XmNbackground, BgMenu_Color);
	     n++;
	     sprintf (bounds, "%d", min);
	     XtSetArg (args[n], XmNvalue, bounds);
	     n++;
	     if (min < 0)
		i = 1;
	     else
		i = 0;
	     do
	       {
		  ent = ent / 10;
		  i++;
	       }
	     while (ent > 0);
	     XtSetArg (args[n], XmNcolumns, i);
	     n++;
	     XtSetArg (args[n], XmNmaxLength, i + 1);
	     n++;
	     XtSetArg (args[n], XmNfontList, DefaultFont);
	     n++;
	     w = XmCreateText (row, "Dialogue", args, n);
	     XtManageChild (w);
	     if (catalogue->Cat_React)
	       XtAddCallback (w, XmNvalueChangedCallback, (XtCallbackProc) CallValueSet, catalogue);
#else /* _GTK */
	     /* a new label for the title */
	     w = gtk_label_new (bounds);
	     gtk_misc_set_alignment (GTK_MISC (w), 0.0, 0.5);
	     gtk_widget_show (GTK_WIDGET(w));
	     w->style->font = DefaultFont;
	     gtk_label_set_justify (GTK_LABEL (w), GTK_JUSTIFY_LEFT);
	     gtk_box_pack_start (GTK_BOX(row), w, FALSE, FALSE, 0);

	     /* a new text entry which contain 'bounds' text */
	     sprintf (bounds, "%d", min);
	     w = gtk_entry_new ();
	     gtk_widget_set_usize (GTK_WIDGET(w),
				   gdk_string_width (DefaultFont, bounds),
				   8+gdk_char_height(DefaultFont, '|'));
	     gtk_widget_show (w);
	     gtk_box_pack_start (GTK_BOX(row), w, FALSE, FALSE, 0);
	     gtk_entry_set_text (GTK_ENTRY (w), bounds);
	     if (catalogue->Cat_React)
	       ConnectSignalGTK (GTK_OBJECT(w), "changed", GTK_SIGNAL_FUNC(CallValueSet), (gpointer)catalogue);
#endif /* _GTK */
	     catalogue->Cat_Entries->E_ThotWidget[1] = w;
	  }
     }
}

/*----------------------------------------------------------------------
   TtaSetNumberForm fixe le contenu de la feuille de saisie de texte :        
   The parameter ref donne la re'fe'rence du catalogue.               
   The parameter val donne la valeur initiale.                        
  ----------------------------------------------------------------------*/
void TtaSetNumberForm (int ref, int val)
{
   char              text[10];
   int                 lg;
   ThotWidget          wtext;
   struct Cat_Context *catalogue;

   catalogue = CatEntry (ref);
   if (catalogue == NULL)
      TtaError (ERR_invalid_reference);
   else if (catalogue->Cat_Widget == 0)
      TtaError (ERR_invalid_reference);
   else
     {
	if (catalogue->Cat_Type != CAT_INT)
	  {
	     TtaError (ERR_invalid_reference);
	     return;
	  }
	/* Est-ce une valeur valide ? */
	if ((val < (int) catalogue->Cat_Entries->E_ThotWidget[2])
	    || (val > (int) catalogue->Cat_Entries->E_ThotWidget[3]))
	  {
	     TtaError (ERR_invalid_reference);
	     return;
	  }
	wtext = catalogue->Cat_Entries->E_ThotWidget[1];
#ifndef _GTK
	/* Desactive la procedure de Callback */
	if (catalogue->Cat_React)
	  XtRemoveCallback (wtext, XmNvalueChangedCallback, (XtCallbackProc) CallValueSet, catalogue);
	sprintf (text, "%d", val);
	XmTextSetString (wtext, text);
	lg = strlen (text);
	XmTextSetSelection (wtext, lg, lg, 500);
	/* Reactive la procedure de Callback */
	if (catalogue->Cat_React)
	  XtAddCallback (wtext, XmNvalueChangedCallback, (XtCallbackProc) CallValueSet, catalogue);
#else /* _GTK */
	/* Desactive la procedure de Callback */
	if (catalogue->Cat_React)
	  RemoveSignalGTK (GTK_OBJECT(wtext), "changed");  
	sprintf (text, "%d", val);
	gtk_entry_set_text (GTK_ENTRY (wtext), text);
	lg = strlen (text);
	/* Reactive la procedure de Callback */
	if (catalogue->Cat_React)
	  ConnectSignalGTK (GTK_OBJECT(wtext), "changed", GTK_SIGNAL_FUNC(CallValueSet), (gpointer)catalogue);
#endif /* _GTK */
     }
}

/*----------------------------------------------------------------------
   TtaAbortShowDialogue abandonne le TtaShowDialogue.                 
  ----------------------------------------------------------------------*/
void TtaAbortShowDialogue ()
{
  if (ShowReturn == 1)
    {
      /* Debloque l'attente courante */
      ShowReturn = 0;
      /* Invalide le menu ou formulaire courant */
      if (ShowCat  && ShowCat->Cat_Widget != 0)
	{
#ifndef _GTK
	  if (XtIsManaged (ShowCat->Cat_Widget))
	    {
	      /* Traitement particulier des formulaires */
	      if (ShowCat->Cat_Type == CAT_FORM
		  || ShowCat->Cat_Type == CAT_SHEET
		  || ShowCat->Cat_Type == CAT_DIALOG)
		{
		  XtUnmanageChild (XtParent (ShowCat->Cat_Widget));
		  CallSheet (None, ShowCat, NULL);
		}
	      else
		XtUnmanageChild (ShowCat->Cat_Widget);
	    }
#else /* _GTK */  
	  if( GTK_WIDGET_VISIBLE(ShowCat->Cat_Widget) )
	    {
	      /* Traitement particulier des formulaires */
	      if (ShowCat->Cat_Type == CAT_FORM
		  || ShowCat->Cat_Type == CAT_SHEET
		  || ShowCat->Cat_Type == CAT_DIALOG)
		{
		  if (ShowCat->Cat_Widget->parent)
		    gtk_widget_hide (GTK_WIDGET(ShowCat->Cat_Widget->parent));
		  CallSheet (None, ShowCat, NULL);
		}
	      else
		gtk_widget_hide (GTK_WIDGET(ShowCat->Cat_Widget));
	    }
#endif /* _GTK */
	}
     }
}
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
   TtaSetDialoguePosition me'morise la position actuelle de la souris 
   comme position d'affichage des TtaShowDialogue().               
  ----------------------------------------------------------------------*/
void TtaSetDialoguePosition ()
{
#ifndef _WINDOWS
#ifndef _GTK
   ThotWindow          wdum;
   int                 xdum;
   int                 ydum;
#else /* _GTK */
   GdkModifierType     flag_tmp;
#endif /* _GTK */
#ifndef _GTK
   wdum = RootWindow (GDp, DefaultScreen (GDp));
   XQueryPointer (GDp, wdum, &wdum, &wdum, &xdum, &ydum, &ShowX, &ShowY, &xdum);
#else /* _GTK */
   gdk_window_get_pointer((GdkWindow *)(gdk_window_get_toplevels()->data),
			  &ShowX, &ShowY, &flag_tmp);
#endif /* _GTK */
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
   TtaShowDialogue active le catalogue de'signe.                      
  ----------------------------------------------------------------------*/
void TtaShowDialogue (int ref, ThotBool remanent)
{
#ifdef _WINDOWS
  POINT               curPoint;
#else  /* _WINDOWS */
  int                 n;
#ifndef _GTK
  Arg                 args[MAX_ARGS];
#else /* _GTK */
  ThotBool            usedoubleclick;  
#endif /* _GTK */
#endif /* _WINDOWS */
  ThotWidget          w;
  struct Cat_Context *catalogue;

  if (ref == 0)
    {
      TtaError (ERR_invalid_reference);
      return;
    } 
  catalogue = CatEntry (ref);
  if (catalogue == NULL)
    {
      TtaError (ERR_invalid_reference);
      return;
    }
  else
    {
      w = catalogue->Cat_Widget;
      if (w == 0)
	{
	  TtaError (ERR_invalid_reference);
	  return;
	} 
    }
#ifdef _WINDOWS
  if (catalogue->Cat_Type == CAT_POPUP)
    {
      GetCursorPos (&curPoint);
      if (!TrackPopupMenu (w,  TPM_LEFTALIGN, curPoint.x, curPoint.y, 0,
			   catalogue->Cat_ParentWidget, NULL))
	WinErrorBox (WIN_Main_Wd, "TtaShowDialogue (1)");
    }
  else
    {
      ShowWindow (w, SW_SHOWNORMAL);
      UpdateWindow (w);
    }
#else  /* _WINDOWS */
#ifndef _GTK
  if (XtIsManaged (w))
    XMapRaised (GDp, XtWindowOfObject (XtParent (w)));
#else /* _GTK */
  if (GTK_WIDGET_VISIBLE (w))
    {
      gtk_widget_show_all (GTK_WIDGET (w));
      gdk_window_raise (GTK_WIDGET (w)->window);
    }
#endif /* _GTK */
  /*===========> Active un pop-up menu */
  else if (catalogue->Cat_Type == CAT_POPUP || catalogue->Cat_Type == CAT_PULL
	   || catalogue->Cat_Type == CAT_SCRPOPUP)
    {
      /* Faut-il invalider un TtaShowDialogue precedent */
      TtaAbortShowDialogue ();
      /* Memorise qu'un retour sur le catalogue est attendu et */
      /* qu'il peut etre aborte' si et seulement s'il n'est pas remanent */
      if (!remanent)
	{
	  ShowReturn = 1;
	  ShowCat = catalogue;
	}
#ifndef _GTK      
      /*** Positionne le pop-up a la position courante du show ***/
      n = 0;
      XtSetArg (args[n], XmNx, (Position) ShowX);
      n++;
      XtSetArg (args[n], XmNy, (Position) ShowY);
      n++;
      XtSetValues (w, args, n);
      XtManageChild (w);
#else /* _GTK */
      TtaGetEnvBoolean ("ENABLE_DOUBLECLICK", &usedoubleclick);
      if (catalogue->Cat_Button == 'L')
	{
	  if (usedoubleclick)
	    /* prevent to close immediately the popup menu */
	    n = 3;
	  else
	    n = 1;
	}
      else
	n = 3;
      if (catalogue->Cat_Type == CAT_SCRPOPUP)
	{
	  gtk_widget_show_all ((GtkWidget *) w);
	  gtk_widget_grab_focus ((GtkWidget *) w);
	  gdk_keyboard_grab (((GtkWidget *)w)->window, TRUE, GDK_CURRENT_TIME);
	}
      else
	gtk_menu_popup ((GtkMenu *) w, NULL, NULL, NULL, 0, n, (guint32) 0);
#endif /* _GTK */
    } 
  /*===========> Active un formulaire */
  else if ((catalogue->Cat_Type == CAT_FORM ||
	    catalogue->Cat_Type == CAT_SHEET || 
            catalogue->Cat_Type == CAT_DIALOG) &&
	   catalogue->Cat_PtParent == NULL)
    {
      /* Faut-il invalider un TtaShowDialogue precedent */
      TtaAbortShowDialogue ();
      /* Memorise qu'un retour sur le catalogue est attendu et */
      /* qu'il peut etre aborter si et seulement s'il n'est pas remanent */
      if (!remanent)
	{
	  ShowReturn = 1;
	  ShowCat = catalogue;
	}
      /* Pour les feuilles de dialogue force le bouton par defaut */
      if ((catalogue->Cat_Type == CAT_SHEET  || 
	  catalogue->Cat_Type == CAT_DIALOG || 
	  catalogue->Cat_Type == CAT_FORM) &&
	  catalogue->Cat_Entries != NULL)
	{
#ifndef _GTK
	  XtSetArg (args[0], XmNdefaultButton, catalogue->Cat_Entries->E_ThotWidget[1]);
	  XtSetValues (w, args, 1);
#else /* _GTK */
	  if (catalogue->Cat_Entries->E_ThotWidget[1])
	    gtk_widget_grab_default (GTK_WIDGET(catalogue->Cat_Entries->E_ThotWidget[1]));
	  else if (catalogue->Cat_Entries->E_ThotWidget[0])
	    gtk_widget_grab_default (GTK_WIDGET(catalogue->Cat_Entries->E_ThotWidget[0]));
#endif /* _GTK */
	}
      INITform (w, catalogue, NULL);
    }
  else
    TtaError (ERR_invalid_reference);
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
   TtaWaitShowProcDialogue
   Waits the return of the catalogue that is shown by TtaShowDialogue.
   The dialog has its own callback handler and the function returns
   when the dialogue disappears.
  ----------------------------------------------------------------------*/
void TtaWaitShowProcDialogue ()
{
#ifdef _WINDOWS
   ThotEvent              event;

   /* wait until the user selects something */
   while (GetMessage (&event, NULL, 0, 0))
      {
	TranslateMessage (&event);
	DispatchMessage (&event);
      }
#else  /* _WINDOWS */
   TtaWaitShowDialogue ();
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
   TtaWaitShowDialogue attends le retour du catalogue affiche par     
   TtaShowDialogue.                                                   
  ----------------------------------------------------------------------*/
void TtaWaitShowDialogue ()
{
  ThotEvent              event;

#ifdef _WINDOWS
  GetMessage (&event, NULL, 0, 0);
  TranslateMessage (&event);
  DispatchMessage (&event);
#else  /* _WINDOWS */

  /* Un TtaWaitShowDialogue en cours */
  CurrentWait = 1;
  while (ShowReturn == 1)
    {
      TtaFetchOneEvent (&event);
      TtaHandleOneEvent (&event);
    }
  /* Fin de l'attente */
  CurrentWait = 0;
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
   TtaTestWaitShowDialogue retourne Vrai (1) si un TtaWaitShowDialogue        
   est en cours, sinon Faux (0).                                      
  ----------------------------------------------------------------------*/
ThotBool TtaTestWaitShowDialogue ()
{
   return (CurrentWait);
}

#ifndef _WINDOWS
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
   TtaFreeAllCatalogs frees the memory associated with catalogs.                      
  ----------------------------------------------------------------------*/
void TtaFreeAllCatalogs (void)
{
  static struct Cat_List *current;
  
  current = PtrCatalogue;
  while (current)
    {
      PtrCatalogue = current->Cat_Next;
      TtaFreeMemory (current);
      current = PtrCatalogue;
    }
}
