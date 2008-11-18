/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2008
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

#ifdef _WX
#include "wx/wx.h"
#include "wx/panel.h"
#endif /* _WX */

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "dialog.h"
#include "application.h"
#include "interface.h"
#include "appdialogue.h"
#include "message.h"
#include "dialogapi.h"

#include "appli_f.h"
#include "font_f.h"
#include "appdialogue_f.h"
#include "memory_f.h"
#include "thotmsg_f.h"
#include "dialogapi_f.h"
#include "callback_f.h"
#include "registry_f.h"

#ifdef _WINGUI
#include "winsys.h"
#include "wininclude.h"
#endif /* _WINGUI */

#ifdef _GTK
#include <gdk/gdkx.h>
#include "gtk-functions.h" /* GTK prototype */
extern int        appArgc;
extern char**     appArgv;
#endif /* _GTK */

#ifdef _WX
#include "AmayaFrame.h"
#include "AmayaWindow.h"
#include "AmayaCallback.h"
#include "appdialogue_wx_f.h"
#include "appdialogue_wx.h"
#include "message_wx.h"
#include "AmayaPopupList.h"
#endif /* _WX */

#define THOT_EXPORT
#define THOT_INITGLOBALVAR
#include "dialogapi_tv.h" /* global variable implementation */
#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "appdialogue_tv.h"
#include "thotcolor_tv.h"
#include "frame_tv.h"
#include "font_tv.h"

#ifdef _WINGUI

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

LRESULT CALLBACK WndProc        (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ClientWndProc  (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ThotDlgProc    (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK TxtZoneWndProc (HWND, UINT, WPARAM, LPARAM);
extern int main (int, char**);

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

  currentFrame = -1;
  hInstance = hInst;
  nAmayaShow = nShow;
  argc = makeArgcArgv (hInst, &argv, lpCommand);
  main (argc, argv);
  return (TRUE);
}

#endif /* _WINGUI */

/*----------------------------------------------------------------------
  NewCatList Creates a new catalogue list
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
  NewEList: creates a new block of elements
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
  FreeEList: Releases all blocks of elements        
  ----------------------------------------------------------------------*/
static void FreeEList (struct E_List *adbloc)
{
  struct E_List      *cebloc;

  cebloc = adbloc;
  while (cebloc)
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
  CatEntry looks for the catalogue of a given reference ref or a new
  free entry.
  ----------------------------------------------------------------------*/
struct Cat_Context *CatEntry (int ref)
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
  while (adlist && catval == NULL)
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
  if (catval == NULL && catlib)
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

#ifdef _WINGUI
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
#endif /* _WINGUI */

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef _GTK
static ThotBool ConfirmMessageGTK (ThotWidget w, ThotWidget MsgBox)
{
  gtk_widget_hide (MsgBox);
  return TRUE;
}

/*----------------------------------------------------------------------
  Callback inits a form.
  ----------------------------------------------------------------------*/
static void INITform (ThotWidget w, struct Cat_Context *parentCatalogue, caddr_t call_d)
{
  int                 ent;
  struct E_List      *adbloc;
  struct Cat_Context *catalogue;

  /* Affiche le formulaire */
  if (parentCatalogue->Cat_Widget)
    {
      /*** Allume les sous-widgets du formulaire ***/
      adbloc = parentCatalogue->Cat_Entries;
      /* Le premier bloc contient les boutons de la feuille de saisie */
      adbloc = adbloc->E_Next;
      ent = 1;
      while (adbloc->E_ThotWidget[ent])
        {
          /* Il faut sauter les widgets des RowColumns */
          if (adbloc->E_Free[ent] == 'N')
            catalogue = (struct Cat_Context *) adbloc->E_ThotWidget[ent];
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
      if (PopShell)
        {
          gtk_window_set_position (GTK_WINDOW (PopShell), GTK_WIN_POS_MOUSE);
          gtk_widget_show_all (PopShell);
        }
      else
        {
          gtk_window_set_position (GTK_WINDOW (w), GTK_WIN_POS_MOUSE);
          gtk_widget_show_all (GTK_WIDGET(w));
        }
    }
}
#endif /* _GTK */

/*----------------------------------------------------------------------
  warning handler                                                    
  ----------------------------------------------------------------------*/
void MyWarningHandler (char *s)
{
}


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
  CurrentWait = 0;
  ShowReturn = 0;
  ShowX = 100;
  ShowY = 100;
  ActiveFrame = 1;
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
}

/*----------------------------------------------------------------------
  InitDialogueFont initializes the dialogue font
  ----------------------------------------------------------------------*/
void InitDialogueFont ()
{
#ifdef _GTK
  DefaultFont = DialogFont;
#endif /* _GTK */
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
void DisplayConfirmMessage (const char *text)
{
  int                 display_width_px, display_height_px;
  int                 width = 200, height = 100;

  wxMessageDialog messagedialog( NULL,
                                 TtaConvMessageToWX(text), 
                                 TtaConvMessageToWX(TtaGetMessage(LIB, TMSG_LIB_CONFIRM)),
                                 (long) wxOK | wxICON_EXCLAMATION | wxSTAY_ON_TOP);
  wxPoint pos = wxGetMousePosition();
  wxDisplaySize(&display_width_px, &display_height_px);
  if (pos.x + width > display_width_px)
    pos.x = display_width_px - width;
  if (pos.y + height > display_height_px)
    pos.x = display_height_px - height;
  messagedialog.Move(pos);
  messagedialog.ShowModal();
}

/*----------------------------------------------------------------------
  DisplayMessage display the given messge (text) in main window   
  according to is msgType.		                
  - INFO : bellow the previous message.                   
  - OVERHEAD : instead of the previous message.           
  ----------------------------------------------------------------------*/
void DisplayMessage (char *text, int msgType)
{
  wxMessageDialog messagedialog( NULL,
                                 TtaConvMessageToWX(text), 
                                 _T("Info"),
                                 (long) wxOK | wxICON_INFORMATION | wxSTAY_ON_TOP);
  messagedialog.CenterOnScreen();
  messagedialog.ShowModal();
}

/*----------------------------------------------------------------------
  ClearChildren nettoie tous les catalalogues fils du catalogue.     
  ----------------------------------------------------------------------*/
static void ClearChildren (struct Cat_Context *parentCatalogue)
{
  register int        icat;
  struct Cat_Context *catalogue;
  struct Cat_List    *adlist;

  /* Recherche les catalogues qui sont les fils de parentCatalogue */
  adlist = PtrCatalogue;
  while (adlist)
    {
      icat = 0;
      while (icat < MAX_CAT)
        {
          catalogue = &adlist->Cat_Table[icat];
          if (catalogue->Cat_Widget && catalogue->Cat_PtParent == parentCatalogue)
            {
              /* Libere les blocs des entrees */
              if ((catalogue->Cat_Type != CAT_TEXT)
                  && (catalogue->Cat_Type != CAT_SELECT)
                  && (catalogue->Cat_Type != CAT_LABEL)
                  && (catalogue->Cat_Type != CAT_TREE)
                  && (catalogue->Cat_Type != CAT_COMBOBOX))
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
int DestContenuMenu (struct Cat_Context *catalogue)
{
  register int        ent;
  struct E_List      *adbloc;
  ThotWidget          w = NULL;
  
  if (catalogue == NULL)
    return (1);
  else if (catalogue->Cat_Widget == 0)
    return (1);
  else
    {
      if (catalogue->Cat_Type == CAT_LABEL)
        {
          /* Recupere le widget parent du widget detruit */
#ifdef _GTK
          w = GTK_WIDGET(catalogue->Cat_Widget)->parent;
          gtk_widget_destroy (catalogue->Cat_Widget);
#endif /* _GTK */
#ifdef _WINGUI
          w = GetParent (catalogue->Cat_Widget);
          DestroyWindow (w);
#endif /* _WINGUI */
          catalogue->Cat_Widget = 0;
        }
      else
        {
          /* Ce sont des menus */
          adbloc = catalogue->Cat_Entries;
          /* On saute les entrees 0 et 1 */
          ent = 2;
          w = 0;
          if (adbloc)
            /* Liberation de toutes les entrees du menu */
            while (adbloc->E_ThotWidget[ent])
              {
                /* Recuperation du widget parent en sautant le widget titre */
#ifdef _WINGUI
                if (w == 0 && ent)
                  w = GetParent (adbloc->E_ThotWidget[ent]);
                DestroyWindow (adbloc->E_ThotWidget[ent]);
#endif  /* _WINGUI */
#ifdef _GTK
                if (w == 0 && ent)
                  w = GTK_WIDGET(adbloc->E_ThotWidget[ent])->parent;
                
                /* Libere les widgets */
                gtk_widget_hide (GTK_WIDGET(adbloc->E_ThotWidget[ent]));
                gtk_widget_destroy (GTK_WIDGET(adbloc->E_ThotWidget[ent]));
#endif /* _GTK */
#ifdef _WX
                /* nothing is done here because the menu items widgets are destroyed later */
#endif /* _WX */
                adbloc->E_ThotWidget[ent] = (ThotWidget) 0;
                /* Faut-il changer de bloc d'entrees ? */
                ent++;
                if (ent >= C_NUMBER)
                  {
                    if (adbloc->E_Next)
                      adbloc = adbloc->E_Next;
                    ent = 0;
                  }
              }
          /* delete all children */
          if (catalogue->Cat_Type == CAT_POPUP
              || catalogue->Cat_Type == CAT_SCRPOPUP
              || catalogue->Cat_Type == CAT_PULL
              || catalogue->Cat_Type == CAT_MENU)
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
                     char *text, char* equiv, int max_length)
{
#ifndef _WX
  ThotMenu            menu;
  ThotWidget          w;
  struct Cat_Context *catalogue;
  struct E_List      *adbloc;
  register int        count;
  register int        index;
  register int        ent;
  int                 eindex, i;
  ThotBool            rebuilded;
#if defined (_WINGUI) || defined (_GTK) || defined(_WX)
  char                menu_item [1024];
  char                equiv_item [255];
#ifdef _GTK
  GtkWidget          *table;
  ThotWidget          wlabel;
#endif /* _GTK */

  equiv_item[0] = 0;
#endif /* _WINGUI || _GTK || defined(_WX) */
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
      if (catalogue->Cat_Widget)
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
#ifdef _WINGUI
          menu = parent;
#endif  /* _WINGUI */
#ifdef _GTK
          menu = gtk_menu_new ();
          /* 
             peut -�tre rajouter une bouton pour d�tacher le menu ?
             se fait avec gtk_menu_set_tearoff_state;
          */
#endif /* _GTK */
        }
      else
        menu = (ThotMenu) catalogue->Cat_Widget;
      catalogue->Cat_Ref = ref;
      catalogue->Cat_Type = CAT_PULL;
      catalogue->Cat_Button = 'L';
      catalogue->Cat_Data = -1;
      catalogue->Cat_Widget = (ThotWidget) menu;
      catalogue->Cat_ParentWidget = (ThotWidget)parent;
      adbloc = catalogue->Cat_Entries;
#ifdef _WINGUI
      if (parent)
        WIN_AddFrameCatalogue (parent, catalogue);
      if (number == 0)
        /* it's a simple button not a pull-down */
        return;
#endif  /* _WINGUI */
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
#endif /* _GTK */

      /*** Create the menu title ***/
      if (title)
        {
#ifdef _WINGUI
          if (!rebuilded)
            {
              adbloc = NewEList ();
              catalogue->Cat_Entries = adbloc;
              adbloc->E_ThotWidget[0] = (ThotWidget) 0;
              adbloc->E_ThotWidget[1] = (ThotWidget) 0;
            }
#endif  /* _WINGUI */
        }
      else if (!rebuilded)
        {
          adbloc = NewEList ();
          catalogue->Cat_Entries = adbloc;
        }
      
      /* Cree les differentes entrees du menu */
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
                    if (&equiv[eindex] != EOS)
                      {
                        equiv_item[0] = EOS;
                        strcat (equiv_item, &equiv[eindex]);
                      }
                    eindex += strlen (&equiv[eindex]) + 1;
                  }
                if (text[index] == 'B')
                  /*__________________________________________ Creation d'un bouton __*/
                  {
#ifdef _WINGUI 
                    if (equiv_item && equiv_item[0] != EOS)
                      {
                        sprintf (menu_item, "%s\t%s", &text[index + 1], equiv_item); 
                        AppendMenu (menu, MF_STRING | MF_UNCHECKED, ref + i, menu_item);
                        equiv_item[0] = EOS;
                      }
                    else 
                      AppendMenu (menu, MF_STRING | MF_UNCHECKED, ref + i, &text[index + 1]);
                    adbloc->E_ThotWidget[ent] = (ThotWidget) i;
#endif  /* _WINGUI */
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
#endif /* _GTK */
                  }
                else if (text[index] == 'T')
                  /*__________________________________________ Creation d'un toggle __*/
                  {
                    /* un toggle a faux */
#ifdef _WINGUI
                    if (equiv_item && equiv_item[0] != EOS)
                      {
                        sprintf (menu_item, "%s\t%s", &text[index + 1], equiv_item);
                        equiv_item[0] = EOS;
                      }
                    else
                      sprintf (menu_item, "%s", &text[index + 1]);
                    AppendMenu (menu, MF_STRING | MF_UNCHECKED, ref + i, menu_item);
                    adbloc->E_ThotWidget[ent] = (ThotWidget) i;
#endif /* _WINGUI */
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
#endif /* _GTK */
                  }
                else if (text[index] == 'M')
                  /*_______________________________________ Creation d'un sous-menu __*/
                  {
                    /* En attendant le sous-menu on cree un bouton */
#ifdef _WINGUI
                    w = (HMENU) CreatePopupMenu ();
                    subMenuID [currentFrame] = (UINT) w;
                    AppendMenu (menu, MF_POPUP, (UINT) w, &text[index + 1]);
                    adbloc->E_ThotWidget[ent] = w;
#endif /* _WINGUI */
#ifdef _GTK
                    sprintf (menu_item, "%s", &text[index + 1]);
                    w = gtk_menu_item_new_with_label (menu_item);
                    gtk_widget_show_all (w);
                    gtk_menu_append (GTK_MENU (menu),w);
                    adbloc->E_ThotWidget[ent] = w;
#endif /* _GTK */
                  }
                else if (text[index] == 'S')
                  /*_________________________________ Creation d'un separateur __*/
                  {
#ifdef _WINGUI
                    AppendMenu (menu, MF_SEPARATOR, 0, NULL);
                    adbloc->E_ThotWidget[ent] = (ThotWidget) 0;
#endif  /* _WINGUI */
#ifdef _GTK
                    w = gtk_menu_item_new ();
                    gtk_widget_show_all (w);
                    gtk_menu_append (GTK_MENU (menu),w); 
                    adbloc->E_ThotWidget[ent] = w;		 
#endif /* _GTK */
                  }
                else
                  /*____________________________________ Une erreur de construction __*/
                  {
                    TtaDestroyDialogue (ref);
                    /* Type d'entree non defini */
                    TtaError (ERR_invalid_parameter);
                    return;
                  }
                i++;
                ent++;
                index += count + 1;
              }
          }
      /* Attache le pull-down menu au widget passe en parametre */
      if (parent && !rebuilded)
        {	  
#ifdef _GTK
          gtk_menu_item_set_submenu (GTK_MENU_ITEM (parent), menu);
          gtk_object_set_data (GTK_OBJECT(menu), "MenuItem", (gpointer)parent);
          gtk_widget_show_all (parent);
#endif /* _GTK */
        }
    }
#endif /* _WX */
}

#if defined(_GTK) || defined (_WINGUI) || defined(_WX)
/*----------------------------------------------------------------------
  TtaSetPulldownOff suspend le pulldown                           
  ----------------------------------------------------------------------*/
#ifdef _WINGUI
void WIN_TtaSetPulldownOff (int ref, ThotMenu parent, HWND owner)
#endif /* _WINGUI */
#if defined(_GTK) || defined(_WX)
     void TtaSetPulldownOff (int ref, ThotMenu parent)
#endif /* #if defined(_GTK) || defined(_WX) */
{
#ifndef _WX
  struct Cat_Context *catalogue;
#ifdef _WINGUI
  int                 frame;
#endif /* _WINGUI */

  if (ref == 0)
    TtaError (ERR_invalid_reference);
  else if (parent == 0)
    TtaError (ERR_invalid_parent_dialogue);
  else
    {
      catalogue = CatEntry (ref);
      if (catalogue == NULL)
        TtaError (ERR_invalid_reference);
#ifdef _GTK
      else if (catalogue->Cat_Widget)
        {
          gtk_widget_set_sensitive (GTK_WIDGET(parent), FALSE);
          gtk_widget_show_all (GTK_WIDGET(parent));
        }
#endif /* _GTK */
#ifdef _WINGUI
      frame = GetMainFrameNumber (owner);
      EnableMenuItem ((HMENU)WinMenus[frame], (UINT)parent, MF_GRAYED);
      DrawMenuBar (FrMainRef[frame]); 
#endif /* _WINGUI */
    }
#endif /* _WX */
}

/*----------------------------------------------------------------------
  TtaSetPulldownOn reactive le pulldown                           
  ----------------------------------------------------------------------*/
#ifdef _WINGUI
void WIN_TtaSetPulldownOn (int ref, ThotMenu parent, HWND owner)
#endif /* _WINGUI */
#if defined (_GTK) || defined(_WX)
     void TtaSetPulldownOn (int ref, ThotMenu parent)
#endif /* #if defined (_GTK) || defined(_WX) */
{
#ifndef _WX
  struct Cat_Context *catalogue;
  ThotWidget          menu;
#ifdef _WINGUI
  int                 frame;
#endif /* _WINGUI */

  if (ref == 0)
    TtaError (ERR_invalid_reference);
  else if (parent == 0)
    TtaError (ERR_invalid_parent_dialogue);
  else
    {
      catalogue = CatEntry (ref);
      if (catalogue == NULL)
        TtaError (ERR_invalid_reference);
      else if (catalogue->Cat_Widget)
        {
          menu = catalogue->Cat_Widget;
#ifdef _GTK
          gtk_widget_set_sensitive (GTK_WIDGET(parent), TRUE);
          gtk_widget_show_all (GTK_WIDGET(parent));
#endif /* _GTK */
#ifdef _WINGUI
          frame = GetMainFrameNumber (owner);
          EnableMenuItem ((HMENU)WinMenus[frame], (UINT)parent, MF_ENABLED);
          DrawMenuBar (FrMainRef[frame]); 
#endif /* _WINGUI */
        }
    }
#endif /* _WX */
}
#endif /* #if defined(_GTK) || defined (_WINGUI) || defined(_WX) */

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
#if defined (_GTK) || defined(_WINGUI)
  register int        count;
  register int        index;
  register int        ent;
  register int        i;
  int                 eindex;
  ThotBool            rebuilded;
  struct Cat_Context *catalogue;
  struct E_List      *adbloc;
#ifdef _WINGUI
  HMENU               menu;
  HMENU               w;
  int                 nbOldItems, ndx;
#endif /* _WINGUI */
#ifdef _GTK
  GtkWidget          *table;
  ThotWidget          wlabel;
  char                menu_item [1024];
  char                equiv_item [255];
  ThotWidget          menu;
  ThotWidget          w;
#endif /* _GTK */

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
      if (catalogue->Cat_Widget)
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
#ifdef _GTK
      menu = gtk_menu_new ();
      ConnectSignalGTK (GTK_OBJECT (menu), "delete_event",
                        GTK_SIGNAL_FUNC (formKillGTK), (gpointer) catalogue);
      ConnectSignalGTK (GTK_OBJECT (menu), "unmap_event",
                        GTK_SIGNAL_FUNC (formKillGTK), (gpointer) catalogue);
#endif /* _GTK */
      if (!rebuilded)
        {
#ifdef _WINGUI
          menu = CreatePopupMenu ();
#endif /* _WINGUI */
          catalogue->Cat_Widget = menu;
          catalogue->Cat_Ref = ref;
          catalogue->Cat_ParentWidget = (ThotWidget)parent; /* remember the parent widget for the callback */
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
            catalogue->Cat_Button = button;
          else
            button = catalogue->Cat_Button;
        }
      catalogue->Cat_Data = -1;
#ifdef _WINGUI
      if (parent)
        WIN_AddFrameCatalogue (parent, catalogue);
#endif /* _WINGUI */
      
      /*** Cree le titre du menu ***/
      if (title)
        {
          if (!rebuilded)
            {
#ifdef _WINGUI
              adbloc->E_ThotWidget[0] = (ThotWidget) 0;
              adbloc->E_ThotWidget[1] = (ThotWidget) 0;
#endif /* _WINGUI */
            }
        }
      /* Cree les differentes entrees du menu */
#ifdef _WINGUI
      nbOldItems = GetMenuItemCount (menu);
      for (ndx = 0; ndx < nbOldItems; ndx ++)
        if (!DeleteMenu (menu, ref + ndx, MF_BYCOMMAND))
          DeleteMenu (menu, ndx, MF_BYPOSITION);
#endif /* _WINGUI */
      
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
                if (equiv)
                  {
#ifdef _GTK
                    if (&equiv[eindex] != EOS)
                      strcpy (equiv_item, &equiv[eindex]); 
#endif /* _GTK */
                    eindex += strlen (&equiv[eindex]) + 1;
                  }
                if (text[index] == 'B')
                  /*__________________________________________ Creation d'un bouton __*/
                  {
#ifdef _WINGUI
                    AppendMenu (menu, MF_STRING, ref + i, &text[index + 1]);
                    adbloc->E_ThotWidget[ent] = (ThotWidget) i;
#endif /* _WINGUI */
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
#endif /* _GTK */
                  }
                else if (text[index] == 'T')
                  /*__________________________________________ Creation d'un toggle __*/
                  {
#ifdef _WINGUI
                    AppendMenu (menu, MF_STRING | MF_UNCHECKED, ref + i, &text[index + 1]);
                    adbloc->E_ThotWidget[ent] = (ThotWidget) i;
#endif  /* _WINGUI */
#ifdef _GTK
                    /* \t doesn't mean anything to gtk... to we align ourself*/
                    sprintf (menu_item, "%s", &text[index + 1]);
                    if (equiv_item && equiv_item[0])
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
#endif /* _GTK */
                  }
                else if (text[index] == 'M')
                  /*_______________________________________ Creation d'un sous-menu __*/
                  {
                    /* En attendant le sous-menu on cree un bouton */
#ifdef _WINGUI
                    w = (HMENU) CreateMenu ();
                    AppendMenu (menu, MF_POPUP, (UINT) w, (LPCTSTR) (&text[index + 1]));
                    adbloc->E_ThotWidget[ent] = (ThotWidget) w;
#endif  /* _WINGUI */
#ifdef _GTK
                    sprintf (menu_item, "%s", &text[index + 1]);
                    w = gtk_menu_item_new_with_label (menu_item);
                    gtk_widget_show_all (w);
                    gtk_menu_append (GTK_MENU (menu),w);
                    adbloc->E_ThotWidget[ent] = w;
#endif /* _GTK */
                  }
                else if (text[index] == 'S')
                  /*_________________________________ Creation d'un separateur __*/
                  {
#ifdef _WINGUI
                    AppendMenu (menu, MF_SEPARATOR, 0, NULL);
                    adbloc->E_ThotWidget[ent] = (ThotWidget) i;
#endif /* _WINGUI */
#ifdef _GTK
                    w = gtk_menu_item_new ();
                    gtk_widget_show_all (w);
                    gtk_menu_append (GTK_MENU (menu),w); 
                    adbloc->E_ThotWidget[ent] = w;		 
#endif /* _GTK */
                  }
                else
                  /*____________________________________ Une erreur de construction __*/
                  {
                    TtaDestroyDialogue (ref);
                    TtaError (ERR_invalid_parameter);	/* Type d'entree non defini */
                    return;
                  }
                i++;
                ent++;
                index += count + 1;
              }
          }
    }
#endif /* #if defined (_GTK) || defined(_WINGUI)  */
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
  ----
  This dialog is used when the user click on a <select> element (HTML formulary)
  ----------------------------------------------------------------------*/
void TtaNewScrollPopup (int ref, ThotWidget parent, char *title, int number,
                        char *text, char *equiv, ThotBool multipleOptions, char button)
{
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
#ifdef _WINGUI
  HWND                menu;
  HWND                listBox;
#endif /* _WINGUI */
#ifdef _GTK
  char                menu_item [1024];
  GtkWidget          *gtklist;
  GtkWidget          *scr_window = NULL;
  GtkWidget          *event_box;
  GtkWidget          *first = NULL;
  GList              *glist = NULL;
  GtkWidget          *table;
  ThotWidget          wlabel;  
  ThotWidget          menu;  
#endif  /* _GTK */

#ifdef _WX
  char                menu_item [1024];
  ThotWidget          menu = NULL;
#endif /* _WX */

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
  if (catalogue->Cat_Widget)
    {
#ifndef _WX
      if (catalogue->Cat_Type == CAT_SCRPOPUP)
        {
          /* Modification du catalogue */
          DestContenuMenu (catalogue);
          rebuilded = TRUE;
        }
      else
#endif /* _WX */
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
#ifdef _WINGUI
      menu = WIN_InitScrPopup (parent, ref, multipleOptions, number, width, height);
      if (menu)
        listBox = GetDlgItem (menu, 1);
      else
        listBox = NULL;
#endif /* _WINGUI */
#ifdef _WX
      menu = (ThotWindow)new AmayaPopupList( parent, ref );
#endif /* _WX */
#ifdef _GTK      
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
      width = (width) * (gdk_char_width (DialogFont, 'm'));
      height = height * 2 * (gdk_char_height (DialogFont, 'M'));
      gtk_widget_set_usize (scr_window, width, height);
      GTK_WIDGET_UNSET_FLAGS (GTK_SCROLLED_WINDOW (scr_window)->hscrollbar, GTK_CAN_FOCUS);
      GTK_WIDGET_UNSET_FLAGS (GTK_SCROLLED_WINDOW (scr_window)->vscrollbar, GTK_CAN_FOCUS);
      gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scr_window), GTK_POLICY_AUTOMATIC,
                                      GTK_POLICY_AUTOMATIC);
      /* add the scrwindow to its container */
      gtk_container_add (GTK_CONTAINER (event_box), scr_window);
#endif /* _GTK */
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
  catalogue->Cat_ParentWidget = (ThotWidget)parent;
#ifdef _WINGUI
  if (parent)
    WIN_AddFrameCatalogue (parent, catalogue);
#endif /* _WINGUI */
  /* Cree les differentes entrees du menu */
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
            if (equiv)
              {
                if (&equiv[eindex] != EOS)
                  strcpy (equiv_item, &equiv[eindex]); 
                eindex += strlen (&equiv[eindex]) + 1;
              }
            if (text[index] == 'T' || text[index] == 'B' || text[index] == 'M')
              /*__________________________________________ Creation d'un bouton __*/
              {

#ifdef _WINGUI
                /* reserve the first char to indicate selected status */
                text[index] = ' ';
                SendMessage (listBox, LB_INSERTSTRING, i, (LPARAM) &text[index]);
                w = (ThotWidget) i;
#endif /* _WINGUI */
#ifdef _WX
                sprintf (menu_item, "%s", &text[index + 1]);
                ((AmayaPopupList*)menu)->Append(i, TtaConvMessageToWX(menu_item));
                w = (ThotWidget) i;
#endif /* _WX */
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
#endif /* _GTK */
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
#ifdef _WINGUI
  /* remember the catalogue */
  SetProp (menu, "ref", (HANDLE) ref);      
#endif /* _WINGUI */
#ifdef _GTK     
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
                          GTK_SIGNAL_FUNC (ListEventGTK), 
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
      gtk_grab_add (menu);
      GTK_LIST (gtklist)->drag_selection = TRUE;
      gdk_pointer_grab (menu->window, TRUE,
                        (GdkEventMask) (
                                        GDK_BUTTON_PRESS_MASK |
                                        GDK_BUTTON_RELEASE_MASK |
                                        GDK_POINTER_MOTION_MASK ),
                        NULL, NULL, GDK_CURRENT_TIME);
      gtk_widget_grab_focus (menu);
      gdk_keyboard_grab (menu->window, TRUE, GDK_CURRENT_TIME);
      gtk_widget_show (GTK_WIDGET (gtklist)); 
    }
#endif /* _GTK */
}

#ifndef _WX
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

  /* Il faut sauter la 1ere entree allouee a un Row-Column */
  *entry = 1;
  *index = 1;
  /* Le 1er bloc sert aux boutons du feuillet */
  *adbloc = catalogue->Cat_Entries->E_Next;
  /* Recupere le Row-Column racine du formulaire */
  w = (*adbloc)->E_ThotWidget[0];
#ifdef _WINGUI
  row = GetParent (w);
#endif  /* _WINGUI */
#ifdef _GTK
  row = GTK_WIDGET(w->parent);
#endif /* _GTK */
  /*** Recherche une entree libre dans le formulaire ***/
  while ((*adbloc)->E_ThotWidget[*entry])
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
#ifdef _GTK
      if (catalogue->Cat_in_lines)
        w = gtk_hbox_new (FALSE, 5);
      else
        w = gtk_vbox_new (FALSE, 5);
     	gtk_widget_show_all (GTK_WIDGET(w));
      gtk_box_pack_start (GTK_BOX(row), GTK_WIDGET(w), TRUE, TRUE, 0);
#endif /* _GTK */
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
#endif /* _WX */

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
                     int number, ThotIcon * icons, ThotBool horizontal)
{
#ifdef _GTK
  int                 i;
  int                 ent;
  struct Cat_Context *catalogue;
  struct Cat_Context *parentCatalogue;
  struct E_List      *adbloc;
  ThotWidget          menu;
  ThotWidget          w;
  ThotWidget          row;
  ThotWidget          tmpw;

  if (ref == 0)
    {
      TtaError (ERR_invalid_reference);
      return;
    }
  catalogue = CatEntry (ref);
  menu = 0;
  if (catalogue == NULL)
    TtaError (ERR_cannot_create_dialogue);
  else if (catalogue->Cat_Widget)
    TtaError (ERR_invalid_reference);
  else
    {
      catalogue->Cat_React = TRUE;
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
          menu = gtk_vbox_new (FALSE, 0);
          gtk_widget_show_all (menu);
          gtk_container_add (GTK_CONTAINER(w), menu);	     
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
      if (title)
        {
          /* add a label */
          w = gtk_label_new (title);
          gtk_misc_set_alignment (GTK_MISC (w), 0.0, 0.5);
          gtk_widget_show_all (w);
          if (w->style->font == NULL ||
              w->style->font->type != GDK_FONT_FONTSET)
            w->style->font = DefaultFont;
          /*	     gtk_label_set_justify (GTK_LABEL (w), GTK_JUSTIFY_LEFT);*/
          gtk_box_pack_start (GTK_BOX(menu), w, FALSE, FALSE, 0);
          adbloc->E_ThotWidget[0] = w;
	     
          /* add a separator */
          w = gtk_hseparator_new ();
          gtk_widget_show_all (w);
          gtk_box_pack_start (GTK_BOX(menu), w, FALSE, FALSE, 0);
          adbloc->E_ThotWidget[1] = w;
        }

      if (horizontal)
        row = gtk_hbox_new (FALSE, 0);
      else
        row = gtk_vbox_new (FALSE, 0);
      gtk_widget_show_all (row);
      gtk_container_add (GTK_CONTAINER(menu), row);
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
          tmpw = gtk_pixmap_new (((ThotIcon)icons[i])->pixmap, ((ThotIcon )icons[i])->mask);
          w = gtk_button_new ();
          gtk_container_add (GTK_CONTAINER (w), tmpw);
          gtk_widget_show_all (w);
          gtk_box_pack_start (GTK_BOX (row), w, FALSE, FALSE, 0);
          /* Connecte the clicked acton to the button */
          ConnectSignalGTK (GTK_OBJECT(w), "clicked",
                            GTK_SIGNAL_FUNC(/*CallRadioGTK*/CallIconButtonGTK), (gpointer)catalogue);
          adbloc->E_ThotWidget[ent] = w;
          i++;
          ent++;
        }
    }
#endif /* _GTK */
}

/*----------------------------------------------------------------------
  TtaNewSubmenu cree un sous-menu :                                 
  The parameter ref donne la reference pour l'application.         
  The parameter ref_parent identifie le formulaire pe`re.            
  Le parametre entry designe l'entree correspondante dans le menu  
  pere. The parameter title donne le titre du catalogue.            
  The parameter number indique le nombre d'entrees dans le menu.    
  The parameter text contient la liste des intitules du catalogue.  
  Chaque intitule commence par un caractere qui donne le type de   
  l'entree et se termine par un caractere de fin de chaine \0.    
  S'il n'est pas nul, le parametre equiv donne les accelerateurs  
  des entrees du menu.                                              
  Quand le parametre react est vrai, tout changement de selection  
  dans le sous-menu est immediatement signale a l'application.    
  ----------------------------------------------------------------------*/
void TtaNewSubmenu (int ref, int ref_parent, int entry, char *title,
                    int number, char *text, char* equiv, int max_length,
                    ThotBool react)
{
#ifndef _WX
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
#if defined (_WINGUI) || defined (_GTK)
  char                menu_item [1024];
  char                equiv_item [255];
#ifdef _GTK
  GtkWidget          *table;
  ThotWidget          wlabel;
  GSList             *GSListTmp = NULL;
  ThotWidget          tmpw;
  GtkStyle *current_style;
#endif /* _GTK */

  equiv_item[0] = EOS;
#endif /* _WINGUI || _GTK */
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
#ifdef _WINGUI
              menu = w;
#endif  /* _WINGUI */
#ifdef _GTK
              menu = gtk_vbox_new (FALSE, 0);
              gtk_widget_show_all (menu);
              gtk_container_add (GTK_CONTAINER(w), menu);
#endif /* _GTK */
              catalogue->Cat_Ref = ref;
              catalogue->Cat_Type = CAT_FMENU;
              catalogue->Cat_Data = -1;
              catalogue->Cat_Widget = (ThotWidget)menu;
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
              menu = (ThotMenu)catalogue->Cat_Widget;
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
                  if (w->style->font == NULL ||
                      w->style->font->type != GDK_FONT_FONTSET)
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
              else if (adbloc->E_ThotWidget[0])
                /* update the title */
                gtk_label_set_text (GTK_LABEL(adbloc->E_ThotWidget[0]), title);
#endif /* _GTK */
#ifdef _WINGUI
              if (!rebuilded)
                {
                  adbloc->E_ThotWidget[0] = (ThotWidget) 0;
                  adbloc->E_ThotWidget[1] = (ThotWidget) 0;
                } 
#endif /* _WINGUI */
            }
          if (!rebuilded)
            {
#ifdef _GTK
              /* create a new vbox */
              row = gtk_vbox_new (FALSE, 1);
              gtk_widget_show_all (GTK_WIDGET(row));
              gtk_widget_set_name (GTK_WIDGET(row), "Dialogue");
              gtk_box_pack_start (GTK_BOX(menu), GTK_WIDGET(row), FALSE, FALSE, 0);
#endif /* _GTK */
            }
          else
            /* Sinon on recupere le widget parent des entrees */
            row = catalogue->Cat_XtWParent;

          /*** Cree les differentes entrees du sous-menu ***/
          i = 0;
          index = 0;
          eindex = 0;
          ent = 2;
#ifdef _GTK
          GSListTmp = NULL;
#endif /* _GTK */
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
                  /* create a radiolist (not using gtk radio group
                     as it implies always a default value.)*/
                  w = gtk_check_button_new ();	
                  gtk_widget_show_all (GTK_WIDGET(w));
                  tmpw = gtk_label_new (&text[index + 1]);
                  gtk_misc_set_alignment (GTK_MISC (tmpw), 0.0, 0.5);
                  gtk_widget_show_all (tmpw);
                  current_style = gtk_style_copy (gtk_widget_get_style (tmpw));
                  if (current_style->font == NULL ||
                      current_style->font->type != GDK_FONT_FONTSET)
                    current_style->font = DefaultFont;
                  gtk_widget_set_style(tmpw, current_style);
                  gtk_label_set_justify (GTK_LABEL (tmpw), GTK_JUSTIFY_LEFT);
                  gtk_container_add (GTK_CONTAINER(w), tmpw);
                  gtk_object_set_data (GTK_OBJECT(w), "Label", (gpointer)tmpw);
                  gtk_box_pack_start (GTK_BOX(row), GTK_WIDGET(w), FALSE, FALSE, 0);
                  ConnectSignalGTK (GTK_OBJECT(w), "toggled", GTK_SIGNAL_FUNC(CallRadioGTK), (gpointer)catalogue);
                  adbloc->E_ThotWidget[ent] = w;
#endif /* _GTK */
#ifdef _WINGUI
                  WIN_AddFrameCatalogue (w, catalogue);
                  adbloc->E_ThotWidget[ent] = w;
#endif  /* _WINGUI */
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
                      catalogue->Cat_Widget = (ThotWidget)menu;
#ifdef _WINGUI
                      WIN_AddFrameCatalogue (FrMainRef[currentFrame], catalogue);
                      if (!IsMenu (catalogue->Cat_Widget))
                        catalogue->Cat_Widget = w;
#endif /* _WINGUI */
#ifdef _GTK
                      /* assign the submenu to the menu bar*/
                      gtk_widget_show_all (w);
                      gtk_menu_item_set_submenu (GTK_MENU_ITEM (w), menu);
#endif /* _GTK */
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
                  menu = (ThotMenu)catalogue->Cat_Widget;
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
#ifdef _WINGUI
              if (!rebuilded)
                {
                  adbloc->E_ThotWidget[0] = w;
                  adbloc->E_ThotWidget[1] = w;
                }
#endif /* _WINGUI */
#ifdef _GTK
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
            }

          /* Cree les differentes entrees du sous-menu */
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
                      if (&equiv[eindex] != EOS)
                        {
                          equiv_item[0] = EOS;
                          strcat (equiv_item, &equiv[eindex]);
                        }
                      eindex += strlen (&equiv[eindex]) + 1;
                    }
                  if (text[index] == 'B')
                    {
                      /*______________________________________ Creation d'un bouton du sous-menu__*/
#ifdef _WINGUI
                      if (equiv_item && equiv_item[0] != EOS)
                        {
                          sprintf (menu_item, "%s\t%s", &text[index + 1], equiv_item);
                          AppendMenu (w, MF_STRING, ref + i, menu_item);
                          equiv_item[0] = EOS;
                        }
                      else
                        AppendMenu (w, MF_STRING, ref + i, &text[index + 1]);
                      adbloc->E_ThotWidget[ent] = (ThotWidget) i;
#endif  /* _WINGUI */
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
#endif /* _GTK */
                    }
                  else if (text[index] == 'T')
                    {
                      /*________________________________ Creation d'un toggle __*/
#ifdef _WINGUI
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
#endif  /* _WINGUI */
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
#endif /* _GTK */
                    }
                  else if (text[index] == 'M')
                    {
                      /*_________________________________ Appel d'un sous-menu __*/
#ifdef _GTK
                      w = gtk_menu_item_new_with_label (&text[index + 1]);
                      gtk_widget_show_all (w);
                      current_style = gtk_style_copy(gtk_widget_get_style(w));
                      if (current_style->font == NULL ||
                          current_style->font->type != GDK_FONT_FONTSET)
                        current_style->font = DefaultFont;
                      gtk_widget_set_style(w, current_style);

                      adbloc->E_ThotWidget[ent] = w;
#endif /* _GTK */
                    }
                  else if (text[index] == 'S')
                    {
                      /*_____________________________ Creation d'un separateur __*/
#ifdef _WINGUI
                      AppendMenu (w, MF_SEPARATOR, 0, NULL);
                      adbloc->E_ThotWidget[ent] = (ThotWidget) 0;
#endif  /* _WINGUI */
#ifdef _GTK
                      w =  gtk_menu_item_new ();
                      current_style = gtk_style_copy(gtk_widget_get_style(w));
                      if (current_style->font == NULL ||
                          current_style->font->type != GDK_FONT_FONTSET)
                        current_style->font = DefaultFont;
                      gtk_widget_set_style(w, current_style);
                      gtk_widget_show_all (w);
                      gtk_menu_append (GTK_MENU (menu),w);
                      adbloc->E_ThotWidget[ent] = w;
#endif /* _GTK */
                    }
                  else
                    {
                      /*___________________________ Une erreur de construction __*/
                      TtaDestroyDialogue (ref);
                      TtaError (ERR_invalid_parameter);	/* Type d'entree non defini */
                      return;
                    } 
		  
                  /* free the string */
                  i++;
                  index += count + 1;
                  ent++;
                }  
            }
        }
    }
#endif /* _WX */
}

/*----------------------------------------------------------------------
  TtaSetMenuForm fixe la selection dans un sous-menu de formulaire : 
  The parameter ref donne la re'fe'rence du catalogue.               
  The parameter val de'signe l'entre'e se'lectionne'e.               
  ----------------------------------------------------------------------*/
void TtaSetMenuForm (int ref, int val)
{
#ifdef _GTK
  register int        i;
  register int        ent;
  ThotBool            visible;
  struct E_List      *adbloc;
  intptr_t            id_toggled;
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
      if (GTK_WIDGET_VISIBLE (catalogue->Cat_Widget))
        visible = TRUE;
      else
        {
          visible = FALSE;
          gtk_widget_show_all (catalogue->Cat_Widget);
        }

      /* Positionnement de la valeur de chaque entree */
      adbloc = catalogue->Cat_Entries;
      ent = 0;
      i = 2;			/* decalage de 2 pour le widget titre */
      while (adbloc)
        {
          while (i < C_NUMBER)
            {
              if (adbloc->E_ThotWidget[i] == 0)
                i = C_NUMBER;
              else if (ent == val)
                {
                  id_toggled  = (intptr_t) gtk_object_get_data (GTK_OBJECT (adbloc->E_ThotWidget[i]),
                                                                "toggled");
                  gtk_signal_handler_block (GTK_OBJECT(adbloc->E_ThotWidget[i]), 
                                            id_toggled);
                  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (adbloc->E_ThotWidget[i]), 
                                                TRUE);
                }
              else
                {
                  id_toggled  = (intptr_t) gtk_object_get_data (GTK_OBJECT (adbloc->E_ThotWidget[i]),
                                                                "toggled");
                  gtk_signal_handler_block (GTK_OBJECT(adbloc->E_ThotWidget[i]), 
                                            id_toggled);
                  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (adbloc->E_ThotWidget[i]), 
                                                FALSE);
                }
              i++;
              ent++;
            }
          /* Passe au bloc suivant */
          adbloc = adbloc->E_Next;
          i = 0;
        }

      adbloc = catalogue->Cat_Entries;
      ent = 0;
      i = 2;			/* decalage de 2 pour le widget titre */
      while (adbloc)
        {
          while (i < C_NUMBER)
            {
              if (adbloc->E_ThotWidget[i] == 0)
                i = C_NUMBER;
              else
                {
                  id_toggled  = (intptr_t) gtk_object_get_data (GTK_OBJECT (adbloc->E_ThotWidget[i]),
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

      /* La selection de l'utilisateur est desactivee */
      catalogue->Cat_Data = val;
    }
#endif /* _GTK */
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
#if defined(_GTK)
  register int        count;
  register int        index;
  int                 eindex;
  int                 i;
  int                 ent;
  ThotBool            rebuilded;
  struct Cat_Context *catalogue;
  struct Cat_Context *parentCatalogue;
  struct E_List      *adbloc;
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
      /* Faut-il detruire le catalogue precedent ? */
      rebuilded = FALSE;
      if (catalogue->Cat_Widget)
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
              /* create a new vbox to contain the toggle buttons */
              menu = gtk_vbox_new (FALSE, 1);
              gtk_widget_show_all (menu);
              gtk_widget_set_name (menu, "Dialogue");
              gtk_container_add (GTK_CONTAINER(w), menu);
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
          if (title)
            {
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
              else if (adbloc->E_ThotWidget[0])
                gtk_label_set_text (GTK_LABEL(adbloc->E_ThotWidget[0]), title);
            }
          if (!rebuilded)
            {
              /* create a new vbox for the list */
              row = gtk_vbox_new (FALSE, 1);
              gtk_widget_set_name (GTK_WIDGET(row), "Dialogue");
              gtk_widget_show_all (GTK_WIDGET(row));
              gtk_box_pack_start (GTK_BOX(menu), GTK_WIDGET(row),  FALSE, FALSE, 0);
            }
          else
            /* Sinon on recupere le widget parent des entrees */
            row = catalogue->Cat_XtWParent;
	     
          /* note le nombre d'entrees du toggle */
          catalogue->Cat_Data = number;	/* recouvre Cat_XtWParent */
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
                  if (equiv)
                    eindex += strlen (&equiv[eindex]) + 1;

                  /* On accepte toggles, boutons et separateurs */
                  if (text[index] == 'B' || text[index] == 'T')
                    /*____________________________________ Creation d'un bouton __*/
                    {
                      adbloc->E_Type[ent] = 'B';
                      /* add a check button to the list */
                      w = gtk_check_button_new_with_label (&text[index + 1]);
                      gtk_widget_show_all (GTK_WIDGET(w));
                      gtk_box_pack_start (GTK_BOX(row), GTK_WIDGET(w), FALSE, FALSE, 0);
                      ConnectSignalGTK (GTK_OBJECT(w), 
                                        "toggled", 
                                        GTK_SIGNAL_FUNC (CallToggleGTK), 
                                        (gpointer)catalogue);
                      adbloc->E_ThotWidget[ent] = w;
                    }
                  else if (text[index] == 'S')
                    /*_______________________________ Creation d'un separateur __*/
                    {
                      adbloc->E_Type[ent] = 'S';
                      /* add a separator to the list */
                      w = gtk_hseparator_new ();
                      gtk_widget_show_all (w);
                      gtk_box_pack_start (GTK_BOX(row), w, FALSE, FALSE, 0);
                      adbloc->E_ThotWidget[ent] = w;
                    }
           
                  i++;
                  index += count + 1;
                  ent++;
                }
            }
        }
      else
        TtaError (ERR_invalid_parameter);
    }
#endif /* #if defined(_GTK) */
}

/*----------------------------------------------------------------------
  TtaSetToggleMenu fixe la selection dans un toggle-menu :           
  The parameter ref donne la re'fe'rence du catalogue.               
  The parameter val de'signe l'entre'e se'lectionne'e (-1 pour       
  toutes les entre'es). The parameter on indique que le bouton       
  correspondant doit e^tre allume' (on positif) ou e'teint (on nul). 
  ----------------------------------------------------------------------*/
#ifndef _WX
#ifdef _WINGUI 
void WIN_TtaSetToggleMenu (int ref, int val, ThotBool on, HWND owner)
#endif  /* _WINGUI */
#if defined(_GTK)
     void TtaSetToggleMenu (int ref, int val, ThotBool on)
#endif /* #if defined(_GTK) */
{
#ifdef _WINGUI 
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
        /* change the menu item */
        CheckMenuItem (adbloc->E_ThotWidget[tmp_val], ref + val, uCheck);
      else if (CheckMenuItem (hMenu, ref + val, uCheck) == 0xFFFFFFFF)
        {
          /* get the parent menu reference */
          hMenu = GetMenu (owner);
          CheckMenuItem (hMenu, ref + val, uCheck);
        }
    }
#endif /* _WINGUI  */

#if defined(_GTK)
  ThotWidget          w;
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
      else if (GTK_WIDGET_VISIBLE (catalogue->Cat_Widget))
        visible = TRUE;
      else
        {
          visible = FALSE;
          if (catalogue->Cat_Type != CAT_SCRPOPUP)
            gtk_widget_show_all (catalogue->Cat_Widget);
        }

      /* Positionnement de la valeur de chaque entree */
      adbloc = catalogue->Cat_Entries;
      ent = 0;
      i = 2;			/* decalage de 2 pour le widget titre */
      done = FALSE;
      while (adbloc && !done)
        {
          while (i < C_NUMBER)
            {
              /* S'il n'y a pas de bouton  */
              if (adbloc->E_ThotWidget[i] == 0)
                i = C_NUMBER;
              /* C'est une entree qu'il faut initialiser */
              else if (ent == val || val == -1)
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
                      if (on)
                        /* Bouton allume */
                        {
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
                              RemoveSignalGTK (GTK_OBJECT(w), "select");
                              gtk_list_item_select (GTK_LIST_ITEM (w));
                              ConnectSignalGTK (GTK_OBJECT(w), "select",
                                                GTK_SIGNAL_FUNC (CallMenuGTK), 
                                                (gpointer) catalogue);
                            }
                          else
                            GTK_CHECK_MENU_ITEM(w)->active = TRUE;
                        }
                      else
                        /* Etat initial du bouton : eteint */
                        {
                          /* attribut active is set to the good value */
                          if (catalogue->Cat_Type == CAT_TMENU)
                            gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(w), FALSE);
                          /* JK: this is what SG did: GTK_TOGGLE_BUTTON(w)->active = FALSE; */
                          else
                            GTK_CHECK_MENU_ITEM(w)->active = FALSE;
                        }
                    }
                  adbloc->E_Free[i] = 'N';  /* La valeur est la valeur initiale */
                  done = TRUE;
                  break;
                }
              /* Sinon l'entree garde son etat precedent  */
              i++;
              ent++;
            }
          /* Passe au bloc suivant */
          adbloc = adbloc->E_Next;
          i = 0;
        }

      if (!visible && catalogue->Cat_Type != CAT_SCRPOPUP)
        gtk_widget_hide (catalogue->Cat_Widget);
    }
#endif /* #if defined(_GTK) */
}
#endif /* _WX */

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
#ifdef _GTK
          gtk_label_set_text (GTK_LABEL(w), text);
          gtk_widget_show_all (w);
#endif /* _GTK */
        }
    }
}

/*----------------------------------------------------------------------
  TtaRedrawMenuEntry modifie la couleur et/ou la police de l'entre'e 
  entry du menu de'signe' par sa re'fe'rence ref.                    
  ----------------------------------------------------------------------*/
void TtaRedrawMenuEntry (int ref, int entry, char *fontname,
                         ThotColor color, int activate)
{
#ifdef _WX
  wxASSERT_MSG(FALSE, _T("TtaRedrawMenuEntry : to remove"));
#endif /* _WX */

#ifndef _WX
  struct Cat_Context *catalogue;
#ifdef _WINGUI
  HMENU               menu;
#endif /* _WINGUI */
#ifdef _GTK
  ThotWidget          w;
  struct E_List      *adbloc;
  int                 ent;
  ThotWidget          tmpw;
#endif /* _GTK */

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
#ifdef _WINGUI
      menu = catalogue->Cat_Widget;
      if (activate)
        EnableMenuItem (menu, ref + entry, MF_ENABLED);
      else
        EnableMenuItem (menu, ref + entry, MFS_GRAYED);
#endif /* _WINGUI */

#ifdef _WX
      wxMenu * p_menu = NULL;
      if (catalogue->Cat_Type == CAT_PULL)
        {
          // a menu
          p_menu = (wxMenu *)catalogue->Cat_Widget;
        }
      else if (catalogue->Cat_Type == CAT_MENU)
        {
          // a sub menu
          wxMenuItem * p_menu_item = (wxMenuItem*)catalogue->Cat_Widget;
          p_menu = p_menu_item->GetSubMenu();
        }

      if (p_menu)
        if (activate)
          p_menu->Enable( ref + entry, TRUE );
        else
          p_menu->Enable( ref + entry, FALSE );
#endif /* _WX */

#ifdef _GTK
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
        }
      
      if (adbloc->E_ThotWidget[ent] == 0)
        TtaError (ERR_invalid_parameter);
      else
        {
          w = adbloc->E_ThotWidget[ent];
          /* if the widget is a FORM sub menu, then it is a radiolist
             the label font must be change, and not the radiolist font. 
             REM: the label could be show with gtk_object_get_data */
          if (catalogue->Cat_Type == CAT_FMENU)
            tmpw = GTK_WIDGET(gtk_object_get_data (GTK_OBJECT(w), "Label"));
          else
            tmpw = w;
          if (fontname)
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
    
          /* Faut-il activer ou desactiver le Callback */
          if (activate != -1)
            {
              if (activate)
                gtk_widget_set_sensitive (GTK_WIDGET(w), TRUE);
              else
                gtk_widget_set_sensitive (GTK_WIDGET(w), FALSE);
            }
          gtk_widget_show_all (GTK_WIDGET(w));
        }
#endif /* _GTK */
    }
#endif /* _WX */
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
	  
          if (parentCatalogue)
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
                      adbloc->E_ThotWidget[entry] = (ThotWidget) 0;
                      adbloc->E_Free[entry] = 'Y';
                    }
                }
            }
          /* Libere le bloc des entrees alloue */
          if (catalogue->Cat_Type == CAT_FORM
              || catalogue->Cat_Type == CAT_INT
              || catalogue->Cat_Type == CAT_SHEET
              || catalogue->Cat_Type == CAT_DIALOG)
            {
              FreeEList (catalogue->Cat_Entries);
              catalogue->Cat_Entries = NULL;
              /* Note que tous les fils sont detruits */
              if (catalogue->Cat_Type != CAT_INT)
                ClearChildren (catalogue);
            }

#ifdef _GTK
          gtk_widget_destroy (catalogue->Cat_Widget);
#endif /* _GTK */

#ifdef _WX
          if (catalogue->Cat_Type == CAT_DIALOG)
            if (catalogue->Cat_Widget)
              catalogue->Cat_Widget->Destroy();
#endif /* #ifdef _WX */

          /* Libere le catalogue */
          catalogue->Cat_Widget = 0;
          return (0);
        }
    }
}

/*----------------------------------------------------------------------
  TtaRaiseDialogue raises the dialogue if it exists,
  ----------------------------------------------------------------------*/
ThotBool TtaRaiseDialogue (int ref)
{
  struct Cat_Context *catalogue;
  
  if (ref == 0)
    {
      TtaError (ERR_invalid_reference);
      return FALSE;
    }
  catalogue = CatEntry (ref);
  if (catalogue == NULL || catalogue->Cat_Widget == 0)
    return FALSE;
#ifdef _WX
  /* hide the widget */
  if (catalogue->Cat_Type == CAT_DIALOG && catalogue->Cat_Widget)
    {
      catalogue->Cat_Widget->Show( true );
      catalogue->Cat_Widget->Raise( );
    }
#endif /* _WX */
  return TRUE;
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

#ifdef _GTK
  else if (GTK_WIDGET_VISIBLE(catalogue->Cat_Widget))
    {    
      /* Traitement particulier des formulaires */
      /*      if (catalogue->Cat_Type == CAT_FORM
              || catalogue->Cat_Type == CAT_SHEET
              || catalogue->Cat_Type == CAT_DIALOG)*/
      gtk_widget_hide (GTK_WIDGET(catalogue->Cat_Widget));
    }
#endif /* _GTK */

#ifdef _WX
  /* hide the widget */
  if (catalogue->Cat_Type == CAT_DIALOG)
    if (catalogue->Cat_Widget)
      catalogue->Cat_Widget->Hide();
#endif /* _WX */

  /* Si le catalogue correspond au dernier TtaShowDialogue */
  if (ShowCat)
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
#ifdef _WINGUI
  int                 nbMenuItems, itNdx;
#endif /* _WINGUI */

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
      if (catalogue->Cat_Type == CAT_MENU
          || catalogue->Cat_Type == CAT_TMENU
          || catalogue->Cat_Type == CAT_FMENU)
        {
          /* Il faut liberer l'entree du menu pere qui le reference */
          parentCatalogue = catalogue->Cat_PtParent;
          entry = catalogue->Cat_EntryParent;
          catalogue->Cat_PtParent = NULL;
          catalogue->Cat_EntryParent = 0;
          if (parentCatalogue)
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
#ifdef _WX
                          /* nothing is done here because the menu will be deleted
                           * when a new menu is created (TtaNewPulldown) */
#endif /* _WX */
#ifdef _WINGUI
                          nbMenuItems = GetMenuItemCount (w);
                          for (itNdx = 0; itNdx < nbMenuItems; itNdx ++) 
                            if (!DeleteMenu (w, ref + itNdx, MF_BYCOMMAND))
                              DeleteMenu (w, ref + itNdx, MF_BYPOSITION);
                          /* RemoveMenu (w, ref + itNdx, MF_BYCOMMAND); */
                          DestroyMenu (w);
                          subMenuID [currentFrame] = (UINT)w;
                          /* CHECK  CHECK  CHECK  CHECK  CHECK  CHECK  CHECK */
#endif /* _WINGUI */
                          adbloc->E_Free[entry] = 'Y';
                        }
                    }
                }
            }
        }
      /*=================================================> Un autre catalogue */
      else if (catalogue->Cat_Type != CAT_POPUP
               && catalogue->Cat_Type != CAT_SCRPOPUP
               && catalogue->Cat_Type != CAT_PULL
               && catalogue->Cat_Type != CAT_LABEL
               && catalogue->Cat_Type != CAT_TREE
               && catalogue->Cat_Type != CAT_COMBOBOX)
        {
          /* C'est surement une destruction de formulaire */
          if (DestForm (ref))
            TtaError (ERR_invalid_reference);
#ifdef _GTK
          if (PopShell && GTK_IS_WIDGET (PopShell))
            gtk_widget_destroy (GTK_WIDGET (PopShell));
          PopShell = 0;
#endif /* _GTK */
#ifdef _WX
          /* TODO : a faire qd on aura porte les dialogues */
#endif /* #ifdef _WX */
          return;
        }
      /* Note que tous les fils sont detruits */
      if (catalogue->Cat_Type == CAT_POPUP
          || catalogue->Cat_Type == CAT_SCRPOPUP
          || catalogue->Cat_Type == CAT_PULL
          || catalogue->Cat_Type == CAT_MENU)
        ClearChildren (catalogue);
      /* Libere les blocs des entrees */
      FreeEList (catalogue->Cat_Entries);
      catalogue->Cat_Entries = NULL;
#ifdef _GTK
      if (catalogue->Cat_Type != CAT_PULL)
        gtk_widget_destroy (GTK_WIDGET(catalogue->Cat_Widget));
#endif /* _GTK */

#ifdef _WX
      if (catalogue->Cat_Type == CAT_DIALOG || catalogue->Cat_Type == CAT_SCRPOPUP)
        if (catalogue->Cat_Widget)
          {
            catalogue->Cat_Widget->Destroy();

            /* then give focus to canvas */
            //	      TtaRedirectFocus();
          }
#endif /* _WX */

      /* Libere le catalogue */
      catalogue->Cat_Widget = 0;
    }
}

/*----------------------------------------------------------------------
  TtaChangeFormTitle change le titre d'un formulaire ou d'une feuille
  de dialogue :
  The parameter ref donne la re'fe'rence du catalogue.    
  Le parame'tre title donne le titre du catalogue.  
  ----------------------------------------------------------------------*/
void TtaChangeFormTitle (int ref, char *title)
{
  struct Cat_Context *catalogue;
   
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
#ifdef _GTK
      /* Set the window title with GTK */
      gdk_window_set_title(GTK_WIDGET(catalogue->Cat_Widget)->window, title);
#endif /* _GTK */
#ifdef _WX
      /* TODO: a faire qd on aura porte les dialogues */
#endif /* #ifdef _WX */
    }
}

/*----------------------------------------------------------------------
  TtaSetDefaultButton
  Defines the default result for the GTK event Double-click.
  ----------------------------------------------------------------------*/
void TtaSetDefaultButton (int ref, int button)
{
#if defined(_GTK) || defined(_WX)
  struct Cat_Context *catalogue;

  if (ref)
    {
      catalogue = CatEntry (ref);
      if (catalogue)
        catalogue->Cat_Default = (unsigned char) button;
    }
#endif /* #if defined(_GTK) || defined(_WX) */
}

#ifndef _WX
/*----------------------------------------------------------------------
  NewSheet
  ----------------------------------------------------------------------*/
static void NewSheet (int ref, ThotWidget parent, char *title, int number,
                      char *text, ThotBool horizontal, int package,
                      char button, int dbutton, int cattype)
{
#ifdef _GTK
  int                 ent;
  int                 index;
  int                 count;
  struct Cat_Context *catalogue;
  struct E_List      *adbloc;
  ThotWidget          form;
  ThotWidget          w;
  char               *ptr = NULL;
  ThotWidget          tmpw;
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
      if (catalogue->Cat_Widget)
        TtaDestroyDialogue (ref);	/* Reconstruction du catalogue */
      /* Recherche le widget parent */
      if (MainShell == 0 && parent == 0)
        {
          PopShell = gtk_window_new (GTK_WINDOW_TOPLEVEL);
          gtk_widget_realize (PopShell);
          gtk_window_set_title (GTK_WINDOW (PopShell), TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
          gtk_widget_set_uposition (GTK_WIDGET (PopShell), ShowX, ShowY);
          gtk_container_set_border_width (GTK_CONTAINER (PopShell), 10);
          ConnectSignalGTK (GTK_OBJECT (PopShell),
                            "destroy",
                            GTK_SIGNAL_FUNC (DeletePopShell),
                            (gpointer)NULL);
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
      if (parent)
        w = parent;
      else if (MainShell == 0)
        w = PopShell;
      else
        w = MainShell;

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
      adbloc->E_ThotWidget[0] = w;
      adbloc->E_Free[0] = 'X';
      if (number < 0)
        /* il n'y a pas de boutons a engendrer */
        return;

      /* Create the hbox for buttons */
      tmpw = gtk_hbox_new (FALSE, 5);
      gtk_widget_show_all (tmpw);
      gtk_widget_set_name (tmpw, "Dialogue");
      gtk_box_pack_start (GTK_BOX(row), tmpw, FALSE, FALSE, 0);
      row=tmpw;
      adbloc = catalogue->Cat_Entries;
      if (cattype == CAT_SHEET)
        ent = 1;
      else if (cattype == CAT_FORM)
        {
          /*** Cree le bouton de confirmation du formulaire ***/
          ent = 1;
          w = gtk_button_new_with_label (TtaGetMessage(LIB, TMSG_LIB_CONFIRM));
          GTK_WIDGET_SET_FLAGS (GTK_WIDGET (w), GTK_CAN_DEFAULT);
          gtk_widget_show_all (GTK_WIDGET (w));
          gtk_box_pack_start (GTK_BOX(row), w, FALSE, FALSE, 0);
          ConnectSignalGTK (GTK_OBJECT(w), "clicked",
                            GTK_SIGNAL_FUNC (CallSheetGTK), (gpointer) catalogue);
          gtk_widget_grab_default (GTK_WIDGET (w));
          adbloc->E_ThotWidget[1] = w;
        }
      else
        ent = 0;
      /*** Cree les autres boutons du feuillet ***/
      index = 0;
      while (ent < C_NUMBER && ent <= number && text)
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
                  w = gtk_button_new_with_label (&text[index]);
                  GTK_WIDGET_SET_FLAGS (GTK_WIDGET (w), GTK_CAN_DEFAULT);
                  gtk_widget_show_all (GTK_WIDGET (w));
                  gtk_box_pack_start (GTK_BOX (row), w, FALSE, FALSE, 0);
                  ConnectSignalGTK (GTK_OBJECT(w), "clicked",
                                    GTK_SIGNAL_FUNC (CallSheetGTK),
                                    (gpointer) catalogue);
                  gtk_widget_grab_default (GTK_WIDGET(w));
                  adbloc->E_ThotWidget[ent] = w;
                }
              if (index == 0 && number > 0)
                gtk_widget_grab_default (GTK_WIDGET(w));
            }
          index += count + 1;
          ent++;
        }
      if (cattype == CAT_SHEET || cattype == CAT_FORM)
        /*** Cree le bouton QUIT ***/
        switch (dbutton)
          {
          case D_CANCEL:
            w = gtk_button_new_with_label(TtaGetMessage (LIB, TMSG_CANCEL));
            GTK_WIDGET_SET_FLAGS (GTK_WIDGET(w), GTK_CAN_DEFAULT);
            gtk_widget_show_all (w);
            gtk_box_pack_start (GTK_BOX(row), w, FALSE, FALSE, 5);
            break;
          case D_DONE:
            w = gtk_button_new_with_label(TtaGetMessage (LIB, TMSG_DONE));
            GTK_WIDGET_SET_FLAGS (GTK_WIDGET(w), GTK_CAN_DEFAULT);
            gtk_widget_show_all (w);
            gtk_box_pack_start (GTK_BOX(row), w, FALSE, FALSE, 5);
            break;
          case D_DISCARD:
            w = gtk_button_new_with_label(TtaGetMessage (LIB, TMSG_DISCARD));
            GTK_WIDGET_SET_FLAGS (GTK_WIDGET(w), GTK_CAN_DEFAULT);
            gtk_widget_show_all (w);
            gtk_box_pack_start (GTK_BOX(row), w, FALSE, FALSE, 5);
            break;
          }
      else
        {
          w = gtk_button_new_with_label(ptr);
          GTK_WIDGET_SET_FLAGS (GTK_WIDGET (w), GTK_CAN_DEFAULT);
          gtk_widget_show_all (GTK_WIDGET (w));
          gtk_box_pack_start (GTK_BOX (row), w, FALSE, FALSE, 5);
        }
      gtk_widget_show_all (GTK_WIDGET (w));
      ConnectSignalGTK (GTK_OBJECT (w), "clicked",
                        GTK_SIGNAL_FUNC (CallSheetGTK), (gpointer)catalogue);
      /* Range le bouton dans le 1er bloc de widgets */
      adbloc->E_ThotWidget[0] = w;
    }
#endif /* _GTK */
}
#endif /* _WX */

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
#if defined(_GTK)
  NewSheet (ref, parent, title, 0, NULL, horizontal, package,
            button, dbutton, CAT_FORM);
#endif /* #if defined(_GTK) */
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
#if defined(_GTK)
  NewSheet (ref, parent, title, number, text, horizontal, package,
            button, dbutton, CAT_SHEET);
#endif /* #if defined(_GTK) */
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
#if defined(_GTK)
  NewSheet (ref, parent, title, number - 1, text, horizontal, package,
            button, D_DONE, CAT_DIALOG);
#endif /* defined(_GTK) */
}

/*----------------------------------------------------------------------
  TtaAttachForm attache le catalogue au formulaire ou au feuillet    
  dont il de'pend. Les catalogues sont cre'e's attache's.            
  ----------------------------------------------------------------------*/
void TtaAttachForm (int ref)
{
#if defined(_GTK)
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
  else if (catalogue->Cat_Type != CAT_FMENU
           && catalogue->Cat_Type != CAT_TMENU
           && catalogue->Cat_Type != CAT_TEXT
           && catalogue->Cat_Type != CAT_LABEL
           && catalogue->Cat_Type != CAT_TREE
           && catalogue->Cat_Type != CAT_COMBOBOX
           && catalogue->Cat_Type != CAT_INT
           && catalogue->Cat_Type != CAT_SELECT)
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
          if ( GTK_WIDGET_VISIBLE(parentCatalogue->Cat_Widget) )
            gdk_window_raise (GTK_WIDGET(catalogue->Cat_Widget)->window);
        }
    }
#endif /* #if defined(_GTK) */
}

/*----------------------------------------------------------------------
  TtaDetachForm detache le catalogue au formulaire ou au feuillet    
  dont il de'pend. Les catalogues sont cre'e's attache's.            
  ----------------------------------------------------------------------*/
void TtaDetachForm (int ref)
{
#if defined(_GTK)
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
  else if (catalogue->Cat_Type != CAT_FMENU
           && catalogue->Cat_Type != CAT_TMENU
           && catalogue->Cat_Type != CAT_TEXT
           && catalogue->Cat_Type != CAT_LABEL
           && catalogue->Cat_Type != CAT_TREE
           && catalogue->Cat_Type != CAT_COMBOBOX
           && catalogue->Cat_Type != CAT_INT
           && catalogue->Cat_Type != CAT_SELECT)
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

          if (GTK_WIDGET_VISIBLE (catalogue->Cat_Widget))
            gtk_widget_hide (catalogue->Cat_Widget);
        }
    }
#endif /* #if defined(_GTK) */
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
#if defined(_GTK)
  struct Cat_Context *catalogue;
  struct Cat_Context *parentCatalogue;
  GList              *item = NULL;
  ThotWidget          tmpw, tmpw2;
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
  else if (catalogue->Cat_Widget)
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
      else if (parentCatalogue->Cat_Type != CAT_FORM
               && parentCatalogue->Cat_Type != CAT_SHEET
               && parentCatalogue->Cat_Type != CAT_DIALOG)
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

  /* Faut-il simplement mettre a jour le selecteur ? */
  if (rebuilded)
    {
      /* On met a jour le titre du selecteur */
      if (catalogue->Cat_Title && title)
        {
          /* update the title label */
          gtk_label_set_text (GTK_LABEL(catalogue->Cat_Title),title);
          gtk_widget_show (GTK_WIDGET(catalogue->Cat_Title));
        }
      /* On met a jour le label attache au bouton du selecteur */
      if (catalogue->Cat_SelectLabel && label)
        {
          /* update the selector label*/
          gtk_label_set_text (GTK_LABEL(gtk_object_get_data (GTK_OBJECT(catalogue->Cat_SelectLabel),"ButtonLabel")), label);
          gtk_widget_show (GTK_WIDGET (catalogue->Cat_SelectLabel));
        }
      /* On met a jour le selecteur (catalogue->Cat_Entries) */
      catalogue->Cat_ListLength = number;
      w = (ThotWidget) catalogue->Cat_Entries;
      /* delete the old list elements */
      if ((intptr_t)gtk_object_get_data(GTK_OBJECT(w), "GList") > 0)
        gtk_list_clear_items (GTK_LIST (w), 0, (intptr_t)gtk_object_get_data(GTK_OBJECT (w), "GList"));
      /* add the new list elements */
      gtk_list_append_items (GTK_LIST(w), item);
      /* update the number of list element */
      gtk_object_set_data(GTK_OBJECT(w), "GList", (gpointer)g_list_length (item));
      gtk_widget_show (GTK_WIDGET(w));
    }
  else
    {
      /*_______________________________________ C'est un nouveau formulaire __*/
      w = AddInFormulary (parentCatalogue, &i, &ent, &adbloc);
      row = gtk_vbox_new (FALSE, 0);
      gtk_widget_show (row);
      gtk_widget_set_name (row, "Dialogue");
      gtk_container_add (GTK_CONTAINER (w), row);
      catalogue->Cat_Ref = ref;
      catalogue->Cat_Type = CAT_SELECT;
      catalogue->Cat_ListLength = number;
      catalogue->Cat_Widget = row;
      adbloc->E_ThotWidget[ent] = (ThotWidget) catalogue;
      adbloc->E_Free[ent] = 'N';
      catalogue->Cat_EntryParent = i;
      catalogue->Cat_Title = 0;
      /*** Cree le titre du selecteur ***/
      if (title)
        {
          w = gtk_label_new (title);
          gtk_misc_set_alignment (GTK_MISC (w), 0.0, 0.5);
          gtk_widget_show (w);
          w->style->font=DefaultFont;
          gtk_box_pack_start (GTK_BOX(row), w, FALSE, FALSE, 0);
          catalogue->Cat_Title = w;
        }
      /*** Cree le label attache au selecteur ***/
      if (label)
        {
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
          ConnectSignalGTK (GTK_OBJECT(w), "clicked",
                            GTK_SIGNAL_FUNC (CallLabel), (gpointer)catalogue);
          catalogue->Cat_SelectLabel = w;
        }
      else
        catalogue->Cat_SelectLabel = 0;

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
          if (tmpw->style->font == NULL ||
              tmpw->style->font->type != GDK_FONT_FONTSET)
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
      /* Conserve le widget du selecteur dans l'entree Cat_Entries */
      catalogue->Cat_Entries = (struct E_List *) w;
    }
#endif /* #if defined(_GTK) */
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
#if defined(_GTK)
  TtaNewSizedSelector (ref, ref_parent, title, number, text, 0, height,
                       label, withText, react);
#endif /* #if defined(_GTK) */
}

/*----------------------------------------------------------------------
  TtaActiveSelector rend actif le  se'lecteur.                       
  ----------------------------------------------------------------------*/
void TtaActiveSelector (int ref)
{
#if defined(_GTK)
  ThotWidget          w;
  struct Cat_Context *catalogue;

  catalogue = CatEntry (ref);
  if (catalogue == NULL)
    TtaError (ERR_invalid_reference);
  else if (catalogue->Cat_Widget == 0)
    TtaError (ERR_invalid_reference);
  else if (catalogue->Cat_Type != CAT_SELECT)
    TtaError (ERR_invalid_reference);
  else
    {
      w = (ThotWidget) catalogue->Cat_Entries;
      gtk_widget_set_sensitive (GTK_WIDGET(w), TRUE);
      gtk_widget_show (GTK_WIDGET(w));
    }
#endif /* #if defined(_GTK) */
}

/*----------------------------------------------------------------------
  TtaDesactiveSelector rend non actif le  se'lecteur.                        
  ----------------------------------------------------------------------*/
void TtaDesactiveSelector (int ref)
{
#if defined(_GTK)
  ThotWidget          w;
  struct Cat_Context *catalogue;

  catalogue = CatEntry (ref);
  if (catalogue == NULL)
    TtaError (ERR_invalid_reference);
  else if (catalogue->Cat_Widget == 0)
    TtaError (ERR_invalid_reference);
  else if (catalogue->Cat_Type != CAT_SELECT)
    TtaError (ERR_invalid_reference);
  else
    {
      w = (ThotWidget) catalogue->Cat_Entries;
      gtk_widget_show (GTK_WIDGET(w));
      gtk_widget_set_sensitive (GTK_WIDGET(w), FALSE);
    }
#endif /* #if defined(_GTK) */
}

/*----------------------------------------------------------------------
  TtaSetSelector initialise l'entre'e et/ou le texte du se'lecteur : 
  The parameter ref donne la re'fe'rence du catalogue.               
  The parameter entry positif ou nul donne l'index de l'entre'e      
  se'lectionne'e.                                                    
  The parameter text donne le texte si entry vaut -1.                
  ----------------------------------------------------------------------*/
void TtaSetSelector (int ref, int entry, const char *text)
{
#if defined(_GTK)
  ThotWidget          wt;
  ThotWidget          select;
  struct Cat_Context *catalogue;

  catalogue = CatEntry (ref);
  wt = 0;
  if (catalogue == NULL)
    TtaError (ERR_invalid_reference);
  else if (catalogue->Cat_Widget)
    {
      /* Recupere le widget du selecteur */
      select = (ThotWidget) catalogue->Cat_Entries;
      if (catalogue->Cat_Type != CAT_SELECT)
        {
          TtaError (ERR_invalid_reference);
          return;
        }

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

      if (entry >= 0 && entry < catalogue->Cat_ListLength)
        gtk_list_select_item (GTK_LIST(select), entry);
      else if (catalogue->Cat_SelectList)
        gtk_list_unselect_all (GTK_LIST(select));
      else
        {
          /* Initialise le champ texte */
          if (catalogue->Cat_ListLength)
            gtk_list_unselect_all (GTK_LIST(select));
          if (!text)
            gtk_entry_set_text (GTK_ENTRY (wt), "");
          else
            gtk_entry_set_text (GTK_ENTRY (wt), text);
        }

      /* Si le selecteur est reactif */
      if (catalogue->Cat_React)
        {
          if (catalogue->Cat_SelectList)
            {
              ConnectSignalGTK (GTK_OBJECT(select),"selection_changed"
                                , GTK_SIGNAL_FUNC(CallListGTK), (gpointer)catalogue);
              ConnectSignalGTK (GTK_OBJECT(select), "button_press_event",
                                GTK_SIGNAL_FUNC (CallTextEnterGTK),
                                (gpointer)catalogue);
            }
          else
            ConnectSignalGTK (GTK_OBJECT(wt),"changed",
                              GTK_SIGNAL_FUNC(CallTextChangeGTK), (gpointer)catalogue);
        }
    }
#endif /* #if defined(_GTK) */
}

#ifndef _WX
/*----------------------------------------------------------------------
  NewLabel
  Common code for both TtaNewLabel and TtaNewPaddedLabel.
  ----------------------------------------------------------------------*/
static void NewLabel (int ref, int ref_parent, char *text, int padding)
{
#if defined(_GTK)
  ThotWidget          tmpw;
  int                 width, height;
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
  else if (catalogue->Cat_Widget && catalogue->Cat_Type == CAT_LABEL)
    {
      /* Modification du catalogue */
      w = catalogue->Cat_Widget;
      gtk_widget_show (w);
      gtk_label_set_text (GTK_LABEL (w), text);	
    }
  else
    {
      if (catalogue->Cat_Widget)
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
      else if (parentCatalogue->Cat_Type != CAT_FORM
               && parentCatalogue->Cat_Type != CAT_SHEET
               && parentCatalogue->Cat_Type != CAT_DIALOG)
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
      tmpw = gtk_label_new (text);
      gtk_misc_set_alignment (GTK_MISC (tmpw), 0.0, 0.5);
      gtk_widget_show (GTK_WIDGET(tmpw));
      tmpw->style->font=DefaultFont;
      gtk_label_set_justify (GTK_LABEL (tmpw), GTK_JUSTIFY_LEFT);
      i = strlen (text);
      if (padding != 0 && i < padding)
        {
          i = padding;
          width = i * (gdk_char_width (DialogFont, 'm'));
          /* seems that 1 tells GTK to compute its own values */
          height = 1;
          gtk_widget_set_usize (tmpw, width, height);
        }
      gtk_box_pack_start (GTK_BOX(w), GTK_WIDGET(tmpw), FALSE, FALSE, 0);
      /* on fou les couleurs (A FAIRE)*/
      gtk_widget_set_name (tmpw, "Dialogue");
      w=tmpw;
      catalogue->Cat_Widget = w;
      catalogue->Cat_Ref = ref;
      catalogue->Cat_Type = CAT_LABEL;
      catalogue->Cat_PtParent = parentCatalogue;
      adbloc->E_ThotWidget[ent] = (ThotWidget) (catalogue);
      adbloc->E_Free[ent] = 'N';
      catalogue->Cat_EntryParent = i;
      catalogue->Cat_Entries = NULL;
    }
#endif /* #if defined(_GTK) */
}
#endif /* _WX */

/*----------------------------------------------------------------------
  TtaNewLabel cre'e un intitule' constant dans un formulaire :       
  The parameter ref donne la re'fe'rence du catalogue.               
  The parameter text donne l'intitule'.                              
  ----------------------------------------------------------------------*/
void TtaNewLabel (int ref, int ref_parent, const char *text)
{
#if defined(_GTK)
  NewLabel (ref, ref_parent, text, 0);
#endif /* #if defined(_GTK) */
}

/*----------------------------------------------------------------------
  TtaNewPaddedLabel cre'e un intitule' constant dans un formulaire :       
  The parameter ref donne la re'fe'rence du catalogue.               
  The parameter text donne l'intitule'.        
  Padding says how many extra characters to add if the text is inferior
  to it.                      
  ----------------------------------------------------------------------*/
void TtaNewPaddedLabel (int ref, int ref_parent, char *text, int padding)
{
#if defined(_GTK)
  NewLabel (ref, ref_parent, text, padding);
#endif /* #if defined(_GTK) */
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
#if defined(_GTK)
  int                 ent;
  int                 i;
  struct Cat_Context *catalogue;
  struct Cat_Context *parentCatalogue;
  struct E_List      *adbloc;
  ThotWidget          w;
  ThotWidget          row;
  ThotWidget          tmpw;

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
      if (catalogue->Cat_Widget)
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
      else if (parentCatalogue->Cat_Type != CAT_FORM
               && parentCatalogue->Cat_Type != CAT_SHEET
               && parentCatalogue->Cat_Type != CAT_DIALOG)
        {
          TtaError (ERR_invalid_parent_dialogue);
          return;
        }
      /*_____________________________________________________________ Sinon __*/
      else
        {
          /* adjust the height size */
          if (height < 2)
            height = 1;

          /* Cree a l'interieur Row-Column du formulaire */
          row = AddInFormulary (parentCatalogue, &i, &ent, &adbloc);
          /* create the vbox for all the elements */
          tmpw = gtk_vbox_new (FALSE, 0);
          gtk_widget_show (GTK_WIDGET(tmpw));
          if (tmpw->style->font == NULL ||
              tmpw->style->font->type != GDK_FONT_FONTSET)
            tmpw->style->font = DefaultFont;
          gtk_box_pack_start (GTK_BOX(row), GTK_WIDGET(tmpw), FALSE, FALSE, 0);
          row = tmpw;
          /* Create the label title */
          if (title)
            {
              w = gtk_label_new (title);
              gtk_misc_set_alignment (GTK_MISC (w), 0.0, 0.5);
              gtk_widget_show (GTK_WIDGET(w));
              w->style->font=DefaultFont;
              gtk_widget_set_name (w, "Dialogue");
              gtk_box_pack_start (GTK_BOX(row), w, FALSE, FALSE, 2);
            }	
          /* new text widget added into the row widget */
          if (height == 1)
            w = gtk_entry_new ();
          else
            w = gtk_text_new (NULL, NULL);
          gtk_widget_show (w);
          w->style->font=DefaultFont;
          gtk_box_pack_start (GTK_BOX(row), w, FALSE, FALSE, 2);
          /* assigne the good size for the widget */
          if (width == 0)
            gtk_widget_set_usize (GTK_WIDGET(w),
                                  10*gdk_char_width (DefaultFont, 'n'),
                                  10+height*gdk_char_height (DefaultFont, '|'));
          else
            gtk_widget_set_usize (GTK_WIDGET(w),
                                  (width)*gdk_char_width (DefaultFont, 'n'),
                                  10+height*gdk_char_height (DefaultFont, '|'));

          /* extra things to do for the textedit */
          if (height > 1)
            {
              gtk_text_set_editable (GTK_TEXT(w), TRUE);
              gtk_text_set_word_wrap (GTK_TEXT(w), TRUE);
            }

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
                            GTK_SIGNAL_FUNC (CallSheetGTK), (gpointer) parentCatalogue);

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
#endif /* #if defined(_GTK) */
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
#if defined(_GTK)
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
      if (catalogue->Cat_React)
        RemoveSignalGTK (GTK_OBJECT(w), "changed");  
      if (GTK_IS_ENTRY (w))
        gtk_entry_set_text (GTK_ENTRY (w), text);
      else 
        gtk_text_insert (GTK_TEXT(w), NULL, NULL, NULL, text, -1);
      /*gtk_editable_select_region(GTK_EDITABLE(w), 0, -1);*/
      if (catalogue->Cat_React)
        ConnectSignalGTK (GTK_OBJECT(w), "changed",
                          GTK_SIGNAL_FUNC(CallTextChangeGTK), (gpointer)catalogue);
    }
#endif /* #if defined(_GTK) */
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
#if defined(_GTK)
  int                 ent;
  int                 i;
  struct Cat_Context *catalogue;
  struct Cat_Context *parentCatalogue;
  struct E_List      *adbloc;
  ThotWidget          tmpw;
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
      if (catalogue->Cat_Widget)
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
      else if (parentCatalogue->Cat_Type != CAT_FORM
               && parentCatalogue->Cat_Type != CAT_SHEET
               && parentCatalogue->Cat_Type != CAT_DIALOG)
        {
          TtaError (ERR_invalid_parent_dialogue);
          return;
        }
      /*_____________________________________________________________ Sinon __*/
      else
        {
          row = AddInFormulary (parentCatalogue, &i, &ent, &adbloc);
          /* add a vbox to contain the elements */
          tmpw = gtk_vbox_new (FALSE, 0);
          gtk_widget_show (tmpw);
          gtk_widget_set_name (tmpw, "Dialogue");
          gtk_container_add (GTK_CONTAINER(row), tmpw);
          row=tmpw;
       
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
          if (title)
            {
              /* add a title label */
              w = gtk_label_new (title);
              gtk_misc_set_alignment (GTK_MISC (w), 0.0, 0.5);
              gtk_widget_show (w);
              gtk_label_set_justify (GTK_LABEL (w), GTK_JUSTIFY_LEFT);
              gtk_widget_set_name (w, "Dialogue");
              gtk_box_pack_start (GTK_BOX(row), w, FALSE, FALSE, 0);
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
       
          /* a new label for the title */
          w = gtk_label_new (bounds);
          gtk_misc_set_alignment (GTK_MISC (w), 0.0, 0.5);
          gtk_widget_show (GTK_WIDGET(w));
          if (w->style->font == NULL ||
              w->style->font->type != GDK_FONT_FONTSET)
            w->style->font = DefaultFont;
          gtk_label_set_justify (GTK_LABEL (w), GTK_JUSTIFY_LEFT);
          gtk_box_pack_start (GTK_BOX(row), w, FALSE, FALSE, 0);

          /* a new text entry which contain an empty text */
          bounds[0] = EOS;
          w = gtk_entry_new ();
          gtk_widget_set_usize (GTK_WIDGET(w),
                                gdk_string_width (DefaultFont, bounds),
                                8+gdk_char_height(DefaultFont, '|'));
          gtk_widget_show (w);
          gtk_box_pack_start (GTK_BOX(row), w, FALSE, FALSE, 0);
          gtk_entry_set_text (GTK_ENTRY (w), bounds);
          if (catalogue->Cat_React)
            ConnectSignalGTK (GTK_OBJECT(w), "changed", GTK_SIGNAL_FUNC(CallValueSet), (gpointer)catalogue);
          catalogue->Cat_Entries->E_ThotWidget[1] = w;
        }
    }
#endif /* #if defined(_GTK) */
}

/*----------------------------------------------------------------------
  TtaSetNumberForm fixe le contenu de la feuille de saisie de texte :        
  The parameter ref donne la re'fe'rence du catalogue.               
  The parameter val donne la valeur initiale.                        
  ----------------------------------------------------------------------*/
void TtaSetNumberForm (int ref, int val)
{
#if defined(_GTK)
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
      if (val < (intptr_t) catalogue->Cat_Entries->E_ThotWidget[2]
          || val > (intptr_t) catalogue->Cat_Entries->E_ThotWidget[3])
        {
          TtaError (ERR_invalid_reference);
          return;
        }
      wtext = catalogue->Cat_Entries->E_ThotWidget[1];
      /* Desactive la procedure de Callback */
      if (catalogue->Cat_React)
        RemoveSignalGTK (GTK_OBJECT(wtext), "changed");  
      sprintf (text, "%d", val);
      gtk_entry_set_text (GTK_ENTRY (wtext), text);
      lg = strlen (text);
      /* Reactive la procedure de Callback */
      if (catalogue->Cat_React)
        ConnectSignalGTK (GTK_OBJECT(wtext), "changed", GTK_SIGNAL_FUNC(CallValueSet), (gpointer)catalogue);
    }
#endif /* #if defined(_GTK) */
}


#ifdef _GTK
typedef void Treecbf (ThotWidget w, ThotBool state, void *user_data);

/*----------------------------------------------------------------------
  TreeRecUnselectCBF
  Clears all selected entries in tree thru a cbf function, except
  for node_skip.
  ----------------------------------------------------------------------*/
static void TreeRecUnselectCBF (GtkCTree *tree, GtkCTreeNode *node, gpointer data)
{
  GtkCTreeNode *node_skip =  GTK_CTREE_NODE (data);
  Treecbf  *cbf;
  void *user_data;

  if (node != node_skip) 
    {
      cbf = (Treecbf *) gtk_object_get_data (GTK_OBJECT(tree), "cbf");
      user_data = gtk_ctree_node_get_row_data (tree, node);
      gtk_ctree_unselect (GTK_CTREE (tree), GTK_CTREE_NODE (node));
      if (cbf)
        (*cbf) ((ThotWidget) node, FALSE, user_data);
    }
}

/*----------------------------------------------------------------------
  TreeItemSelect
  Invokes the user callback if registred and gives the selected/
  deselected state
  ----------------------------------------------------------------------*/
static ThotBool TreeItemSelect (GtkCTree *tree, GtkCTreeNode *node, gint col,
                                gpointer data)
{
  Treecbf  *cbf;
  void *user_data;
  ThotBool state; 
  gpointer has_control, has_multiple;

  /* for some strange reason, we get col == -1 when doing a recursive operation.
     Ignoring this seems to work well. */
  if (col == -1)
    return FALSE;

  /* we can't assign state directly because gpointer > ThotBool. We do
     an arithmetic comparition instead */
  state = (data == 0) ? FALSE : TRUE;
  
  cbf = (Treecbf *) gtk_object_get_data (GTK_OBJECT(tree), "cbf");
  user_data = gtk_ctree_node_get_row_data (tree, node);

  has_multiple = gtk_object_get_data (GTK_OBJECT (tree), "multiple");
  if (has_multiple)
    has_control = gtk_object_get_data (GTK_OBJECT (tree), "control");

  if (has_multiple && !has_control)
    {
      /* remove all the previous selections */
      gtk_clist_freeze (GTK_CLIST (tree));
      /* clear all the entries except the one that generated the event */
      gtk_ctree_post_recursive (GTK_CTREE (tree), NULL, 
                                GTK_CTREE_FUNC (TreeRecUnselectCBF), (gpointer) node);
      gtk_clist_thaw (GTK_CLIST (tree));
    }
  if (cbf)
    (*cbf) ((ThotWidget) node, state, user_data);
  return FALSE;
}

/*----------------------------------------------------------------------
  TreeItemKeyPress
  Detects if a control or F2 multiple selection modifier key is being
  used in the tree widget. If this is the case, it stores this state
  inside the tree widget.
  ----------------------------------------------------------------------*/
static ThotBool TreeItemKeyPress (ThotWidget w, GdkEventKey *ev,
                                  gpointer data)
{
  ThotBool state; 

  if (!GTK_IS_CTREE (w)
      || (ev->type != GDK_KEY_PRESS && ev->type != GDK_KEY_RELEASE))
    return FALSE;

  if (ev->keyval == GDK_Control_L || ev->keyval == GDK_F2)
    {
      /* we can't assign state directly because gpointer > ThotBool. We do
         an arithmetic comparition instead */
      /* update the state indicator */
      state = (data == 0) ? FALSE : TRUE;
      if (state)
        {
          gtk_object_set_data (GTK_OBJECT(w), "control", (gpointer) 1);
        }
      else
        {
          gtk_object_remove_data (GTK_OBJECT(w), "control");
        }
    }
  return FALSE;
}

/*----------------------------------------------------------------------
  TreeFocus
  Detects if the widget has lost focus and if yes, removes the
  "control key pressed" state from the widget.
  ----------------------------------------------------------------------*/
static ThotBool TreeFocus (ThotWidget w, GdkEventFocus *ev,
                           gpointer data)
{
  ThotBool state; 
  
  /* we can't assign state directly because gpointer > ThotBool. We do
     an arithmetic comparition instead */
  state = (data == 0) ? FALSE : TRUE;

  if (!state)
    {
      /* delete the state indicator */
      gtk_object_remove_data (GTK_OBJECT(w), "control");
    }

  return FALSE;
}
#endif /* GTK */

/*----------------------------------------------------------------------
  TtaClearTree
  Clears (destroys) the contents of a tree created with TtaNewTreeForm.
  Returns the identifier of the tree widget.
  ----------------------------------------------------------------------*/
ThotWidget TtaClearTree (ThotWidget tree)
     /* add some tree stuff here */
{
  ThotWidget tree_widget = NULL;
#ifdef _GTK
  GList *children;

  if (!tree || !GTK_IS_SCROLLED_WINDOW (tree))
    return NULL;

  /* get the viewport */
  children = gtk_container_children (GTK_CONTAINER (tree));
  if (!children)
    return NULL;

  /* get the tree */
  tree_widget = GTK_WIDGET(children->data);
  g_list_free (children);

  if (tree_widget && GTK_IS_CTREE (tree_widget))
    gtk_clist_clear (GTK_CLIST (tree_widget));
  else
    return NULL;
#endif /* _GTK */
  return (tree_widget);
}

/*----------------------------------------------------------------------
  TtaAddTreeItem
  parent points to the parent of the tree. If it's NULL, it's
  the first item.
  sibling points to the immediate sibling of this item. If it's NULL
  the item will be added as the first child of parent.
  item_label gives the text that will be visible on the widget.
  selected and expanded gives info on how to display the item.
  user_data is what the user wants to feed to the callback function.
  Returns the reference of the new widget.
  ----------------------------------------------------------------------*/
ThotWidget TtaAddTreeItem (ThotWidget tree, ThotWidget parent,
                           ThotWidget sibling, char *item_label, 
                           ThotBool selected, ThotBool expanded, 
                           void *user_data)
{
  ThotWidget tree_item = NULL;
#ifdef _GTK
  gchar *ctree_label[1];

  ctree_label[0]= (item_label) ?  (gchar *) item_label : (gchar *) "";
  tree_item = (ThotWidget) gtk_ctree_insert_node ((GtkCTree *) tree,
                                                  (GtkCTreeNode *) parent,
                                                  (GtkCTreeNode *) sibling,
                                                  ctree_label,
                                                  5,
                                                  NULL,
                                                  NULL,
                                                  NULL,
                                                  NULL,
                                                  FALSE,
                                                  expanded);

  if (selected)
    {
      RemoveSignalGTK (GTK_OBJECT(tree), "tree_select_row");
      gtk_ctree_select (GTK_CTREE(tree), GTK_CTREE_NODE(tree_item));
      ConnectSignalGTK (GTK_OBJECT (tree), "tree_select_row",
                        GTK_SIGNAL_FUNC (TreeItemSelect), (gpointer) TRUE);
    }

  /* memorize client data */
  gtk_ctree_node_set_row_data (GTK_CTREE (tree),
                               GTK_CTREE_NODE(tree_item), 
                               (gpointer) user_data);
#endif /* _GTK */
  return (tree_item);
}


/*----------------------------------------------------------------------
  TtaNewTreeForm
  The parameter ref gives the catalog reference
  The paramet ref_parent gives the parents reference
  The parameter label gives the form's label
  The parameter multiple says if mutliple selections are allowed inside
  the tree.
  The Parameter callback gives the callback function.
  Returns the pointer of the widget that was created or NULL.
  ----------------------------------------------------------------------*/
ThotWidget TtaNewTreeForm (int ref, int ref_parent, char *label, 
                           ThotBool multiple, void *callback)
{
  ThotWidget          tree = NULL;

#ifdef _GTK
  /* general stuff, move it up when adding win32 */
  int                 i;
  int                 ent;
  int                 width, height;
  int                 rebuilded;
  struct E_List      *adbloc;
  ThotWidget          w, tmpw;
  struct Cat_Context *catalogue;
  struct Cat_Context *parentCatalogue;
  /* end of general info */

  if (ref == 0)
    {
      TtaError (ERR_invalid_reference);
      return NULL;
    }

  catalogue = CatEntry (ref);
  rebuilded = 0;
  if (catalogue == NULL)
    {
      TtaError (ERR_cannot_create_dialogue);
      return NULL;
    }
  else if (catalogue->Cat_Widget && catalogue->Cat_Type == CAT_TREE)
    {
      /* Modification du catalogue */
      w = catalogue->Cat_Widget;
      gtk_widget_show_all (w);
      if (label)
        gtk_label_set_text (GTK_LABEL (w), label);	
    }
  else
    {
      if (catalogue->Cat_Widget)
        /* Le catalogue est a reconstruire completement */
        TtaDestroyDialogue (ref);
      /*======================================> Recherche le catalogue parent */
      parentCatalogue = CatEntry (ref_parent);
      /*__________________________________ Le catalogue parent n'existe pas __*/
      if (parentCatalogue == NULL)
        {
          TtaError (ERR_invalid_parent_dialogue);
          return NULL;
        }
      else if (parentCatalogue->Cat_Widget == 0)
        {
          TtaError (ERR_invalid_parent_dialogue);
          return NULL;
        }
      /*_________________________________________ Sous-menu d'un formulaire __*/
      else if (parentCatalogue->Cat_Type != CAT_FORM
               && parentCatalogue->Cat_Type != CAT_SHEET
               && parentCatalogue->Cat_Type != CAT_DIALOG)
        {
          TtaError (ERR_invalid_parent_dialogue);
          return NULL;
        }
      /* Recupere le widget parent */
      w = AddInFormulary (parentCatalogue, &i, &ent, &adbloc);
      /* this is necessary for getting the key release anywhere on the widget,
         even if the pointer is elsewhere */
      if (parentCatalogue && parentCatalogue->Cat_Widget)
        gtk_widget_add_events (GTK_WIDGET(parentCatalogue->Cat_Widget), 
                               GDK_KEY_RELEASE_MASK);
      if (label)
        {
          tmpw = gtk_label_new (label);
          gtk_misc_set_alignment (GTK_MISC (tmpw), 0.0, 0.5);
          tmpw->style->font=DefaultFont;
          gtk_label_set_justify (GTK_LABEL (tmpw), GTK_JUSTIFY_LEFT);
          gtk_box_pack_start (GTK_BOX(w), GTK_WIDGET(tmpw), FALSE, FALSE, 0);
          gtk_widget_set_name (tmpw, "Dialogue");
        }
       
      /* add some tree stuff here */
      {
        GtkWidget *scrolled_window;
	 
        scrolled_window = gtk_scrolled_window_new (NULL, NULL);
        gtk_scrolled_window_set_policy
          (GTK_SCROLLED_WINDOW(scrolled_window),
           GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
        gtk_box_pack_start (GTK_BOX(w), scrolled_window,
                            TRUE, TRUE, 0);
        /*
          How to say this?
          GTK_WIDGET_UNSET_FLAGS (GTK_SCROLLED_WINDOW (scrolled_window), GTK_CAN_FOCUS);
        */
        w = scrolled_window;
        /* make it at least 6 lines high and 15 chars long */
        width = 1;
        /* width =  150 * (gdk_char_width (DialogFont, 'm')); */
        height = 10 * (gdk_char_height (DialogFont, 'M'));
        gtk_widget_set_usize (GTK_WIDGET(w), width, height);
        gtk_widget_show (w);

        tree = gtk_ctree_new (1, 0);
        gtk_clist_set_selection_mode (GTK_CLIST(tree),
                                      (multiple) ? GTK_SELECTION_MULTIPLE :
                                      GTK_SELECTION_SINGLE);
        /* set  all the extra events we want */
        if (multiple)
          {
            gtk_object_set_data (GTK_OBJECT(tree), "multiple", (gpointer) 1);
            gtk_widget_add_events (GTK_WIDGET(tree), GDK_KEY_RELEASE_MASK);
            ConnectSignalGTK (GTK_OBJECT (tree),  "key_press_event",
                              GTK_SIGNAL_FUNC (TreeItemKeyPress), (gpointer) TRUE);
            ConnectSignalGTK (GTK_OBJECT (tree),  "key_release_event",
                              GTK_SIGNAL_FUNC (TreeItemKeyPress), (gpointer) FALSE);
            ConnectSignalAfterGTK (GTK_OBJECT (tree),  "focus_in_event",
                                   GTK_SIGNAL_FUNC (TreeFocus), (gpointer) TRUE);
            ConnectSignalAfterGTK (GTK_OBJECT (tree),  "focus_out_event",
                                   GTK_SIGNAL_FUNC (TreeFocus), (gpointer) FALSE);
          }
        ConnectSignalAfterGTK (GTK_OBJECT (tree), "tree_select_row",
                               GTK_SIGNAL_FUNC (TreeItemSelect), (gpointer) TRUE);
        ConnectSignalAfterGTK (GTK_OBJECT (tree), "tree_unselect_row",
                               GTK_SIGNAL_FUNC (TreeItemSelect), (gpointer) FALSE);

        /* make it be auto-sort, but I probably want to do this elsewhere  */
        gtk_clist_set_sort_type (GTK_CLIST (tree), GTK_SORT_ASCENDING);
        gtk_clist_set_auto_sort (GTK_CLIST (tree), TRUE);
        gtk_container_add (GTK_CONTAINER (w), 
                           GTK_WIDGET (tree));
        gtk_widget_show (tree);
      }	 
      if (!parentCatalogue->Cat_Focus)
        {
          /* first entry in the form */
          gtk_widget_grab_focus (GTK_WIDGET(tree));
          parentCatalogue->Cat_Focus = TRUE;
        }
      catalogue->Cat_Widget = w;
      catalogue->Cat_Ref = ref;
      catalogue->Cat_Type = CAT_TREE;
      catalogue->Cat_PtParent = parentCatalogue;
      adbloc->E_ThotWidget[ent] = (ThotWidget) (catalogue);
      adbloc->E_Free[ent] = 'N';
      catalogue->Cat_EntryParent = i;
      catalogue->Cat_Entries = NULL;
      /* memorize the callback */
      gtk_object_set_data (GTK_OBJECT(tree),
                           "cbf", 
                           (gpointer) callback);
    }
#endif /* GTK */
  return tree;
}

/*----------------------------------------------------------------------
  TtaInitComboBox
  Initializes the labels in a combo box.
  The parameter w points to a combo box widget.
  The parameter nb_items says how many items are in the list
  The parametes item_labels gives the list of labels.
  ----------------------------------------------------------------------*/
void TtaInitComboBox (ThotWidget w, int nb_items, char *item_labels[])
{
#ifdef _GTK
  int i;
  GtkCombo  *combo;
  GtkWidget *item;

  if (!w || !GTK_IS_COMBO (w) || nb_items == 0 || !item_labels)
    return;
  
  combo = GTK_COMBO (w);

  /* clear the precedent list */
  gtk_list_clear_items (GTK_LIST (combo->list), 0, -1);
  /* make a new one */
  /* initialize it */
  for (i=0; i < nb_items; i++)
    {
      item = gtk_list_item_new_with_label (item_labels[i]);
      gtk_widget_show (item);
      gtk_container_add (GTK_CONTAINER (combo->list), item);
    }
#endif /* _GTK */
}

/*----------------------------------------------------------------------
  TtaNewComboBox
  The parameter ref gives the catalog reference
  The parameter ref_parent gives the parents reference
  The parameter label gives the form's label
  The Parameter callback gives the callback function.
  Returns the pointer of the widget that was created or NULL.
  ----------------------------------------------------------------------*/
ThotWidget TtaNewComboBox (int ref, int ref_parent, char *label,
                           ThotBool react)
{
  ThotWidget          w = NULL;

#ifdef _GTK
  /* general stuff, move it up when adding win32 */
  int                 i;
  int                 ent;
  int                 rebuilded;
  struct E_List      *adbloc;
  ThotWidget          tmpw;
  struct Cat_Context *catalogue;
  struct Cat_Context *parentCatalogue;
  /* end of general info */

  if (ref == 0)
    {
      TtaError (ERR_invalid_reference);
      return NULL;
    }

  catalogue = CatEntry (ref);
  rebuilded = 0;
  if (catalogue == NULL)
    {
      TtaError (ERR_cannot_create_dialogue);
      return NULL;
    }
  else if (catalogue->Cat_Widget && catalogue->Cat_Type == CAT_COMBOBOX)
    {
      /* Modification du catalogue */
      w = catalogue->Cat_Widget;
      gtk_widget_show_all (w);
      if (label)
        gtk_label_set_text (GTK_LABEL (w), label);	
    }
  else
    {
      if (catalogue->Cat_Widget)
        /* Le catalogue est a reconstruire completement */
        TtaDestroyDialogue (ref);
      /*======================================> Recherche le catalogue parent */
      parentCatalogue = CatEntry (ref_parent);
      /*__________________________________ Le catalogue parent n'existe pas __*/
      if (parentCatalogue == NULL)
        {
          TtaError (ERR_invalid_parent_dialogue);
          return NULL;
        }
      else if (parentCatalogue->Cat_Widget == 0)
        {
          TtaError (ERR_invalid_parent_dialogue);
          return NULL;
        }
      /*_________________________________________ Sous-menu d'un formulaire __*/
      else if (parentCatalogue->Cat_Type != CAT_FORM
               && parentCatalogue->Cat_Type != CAT_SHEET
               && parentCatalogue->Cat_Type != CAT_DIALOG)
        {
          TtaError (ERR_invalid_parent_dialogue);
          return NULL;
        }
      /* Recupere le widget parent */
      w = AddInFormulary (parentCatalogue, &i, &ent, &adbloc);
      /* this is necessary for getting the key release anywhere on the widget,
         even if the pointer is elsewhere */
      if (parentCatalogue && parentCatalogue->Cat_Widget)
        gtk_widget_add_events (GTK_WIDGET(parentCatalogue->Cat_Widget), 
                               GDK_KEY_RELEASE_MASK);
      if (label)
        {
          tmpw = gtk_label_new (label);
          gtk_misc_set_alignment (GTK_MISC (tmpw), 0.0, 0.5);
          tmpw->style->font=DefaultFont;
          gtk_label_set_justify (GTK_LABEL (tmpw), GTK_JUSTIFY_LEFT);
          gtk_box_pack_start (GTK_BOX(w), GTK_WIDGET(tmpw), FALSE, FALSE, 0);
          gtk_widget_set_name (tmpw, "Dialogue");
        }
       
      {
        GtkCombo *combo;
        combo = GTK_COMBO (gtk_combo_new ());

        gtk_box_pack_start (GTK_BOX(w), GTK_WIDGET(combo),
                            TRUE, TRUE, 0);

        /* 
        ** set up the behavior for the combo box 
        */

        /* use arrows to select entries */
        gtk_combo_set_use_arrows (combo, TRUE);
        gtk_combo_set_use_arrows_always (combo, TRUE);

        /* use arrows to select entries */
        gtk_combo_set_case_sensitive (combo, TRUE);
        gtk_combo_set_value_in_list (combo, TRUE, FALSE);
	 
        gtk_widget_show (GTK_WIDGET (combo));

        /* connect the signals we're interested in */
        ConnectSignalGTK (GTK_OBJECT(combo->entry), "changed",
                          GTK_SIGNAL_FUNC(ComboBoxGTK), (gpointer) catalogue);
        w = GTK_WIDGET (combo);
      }	 
      if (!parentCatalogue->Cat_Focus)
        {
          /* first entry in the form */
          gtk_widget_grab_focus (GTK_WIDGET(GTK_COMBO(w)->entry));
          parentCatalogue->Cat_Focus = TRUE;
        }
      catalogue->Cat_Widget = w;
      catalogue->Cat_Ref = ref;
      catalogue->Cat_Type = CAT_COMBOBOX;
      catalogue->Cat_PtParent = parentCatalogue;
      catalogue->Cat_React = react;
      adbloc->E_ThotWidget[ent] = (ThotWidget) (catalogue);
      adbloc->E_Free[ent] = 'N';
      catalogue->Cat_EntryParent = i;
      catalogue->Cat_Entries = NULL;
    }
#endif /* GTK */

  return w;
}

/*----------------------------------------------------------------------
  TtaAbortShowDialogue abandonne le TtaShowDialogue.                 
  ----------------------------------------------------------------------*/
void TtaAbortShowDialogue ()
{
#if defined(_WX)
  if (ShowReturn == 1)
    {
      /* Debloque l'attente courante */
      ShowReturn = 0;
      /* Invalide le menu ou formulaire courant */
      if (ShowCat  && ShowCat->Cat_Widget)
        {
          ShowCat->Cat_Widget->Hide();
        }
    }
#endif /* _WX */

#if defined(_GTK)
  if (ShowReturn == 1)
    {
      /* Debloque l'attente courante */
      ShowReturn = 0;
      /* Invalide le menu ou formulaire courant */
      if (ShowCat  && ShowCat->Cat_Widget)
        {
          if (GTK_WIDGET_VISIBLE(ShowCat->Cat_Widget))
            {
              /* Traitement particulier des formulaires */
              if (ShowCat->Cat_Type == CAT_FORM
                  || ShowCat->Cat_Type == CAT_SHEET
                  || ShowCat->Cat_Type == CAT_DIALOG)
                {
                  if (ShowCat->Cat_Widget->parent)
                    gtk_widget_hide (GTK_WIDGET(ShowCat->Cat_Widget->parent));
                  CallSheetGTK (None, ShowCat);
                }
              else
                gtk_widget_hide (GTK_WIDGET(ShowCat->Cat_Widget));
            }
        }
    }
#endif /* #if defined(_GTK) */
}

/*----------------------------------------------------------------------
  TtaSetDialoguePosition me'morise la position actuelle de la souris 
  comme position d'affichage des TtaShowDialogue().               
  ----------------------------------------------------------------------*/
void TtaSetDialoguePosition ()
{
#ifdef _WX
  wxPoint p = wxGetMousePosition();
  ShowX = p.x;
  ShowY = p.y;
#endif /* _WX */
}

/*----------------------------------------------------------------------
  TtaShowDialogue active le catalogue de'signe.                      
  ----------------------------------------------------------------------*/
void TtaShowDialogue (int ref, ThotBool remanent)
{
#ifdef _WINGUI
  POINT               curPoint;
#endif  /* _WINGUI */
  ThotWidget          w;
  struct Cat_Context *catalogue;
  int                 display_width_px, display_height_px;
  int                 width, height;

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

#ifdef _WX
  if (catalogue->Cat_Type == CAT_DIALOG)
    {
      if (catalogue->Cat_Widget)
        {
          /* Faut-il invalider un TtaShowDialogue precedent */
          if (ShowCat && ShowCat != catalogue)
            TtaAbortShowDialogue ();
          /* Memorise qu'un retour sur le catalogue est attendu et */
          /* qu'il peut etre aborter si et seulement s'il n'est pas remanent */
          if (!remanent)
            {
              ShowReturn = 1;
              ShowCat = catalogue;
            }
	  
          wxPoint pos = wxGetMousePosition();
          wxDisplaySize(&display_width_px, &display_height_px);
          catalogue->Cat_Widget->GetSize (&width, &height);
          if (pos.x + width > display_width_px)
            pos.x = display_width_px - width;
          if (pos.y + height > display_height_px)
            pos.x = display_height_px - height;
          catalogue->Cat_Widget->Move(pos);
          catalogue->Cat_Widget->Show();
          catalogue->Cat_Widget->Raise();
        }
    }
  else if (catalogue->Cat_Type == CAT_SCRPOPUP)
    {
      /* this is a HTML form selector */

      /* this is the code to use with AmayaPopupList2.cpp 
       * it has a better look but there is a annoying bug in wxwidgets :
       * I didn't found how to activate an element in the bottom part of the popuplist
       * So maybe this code is to try later when the bug will be fixed on wxWidgets side */
      /*
        if (!remanent)
        {
        ShowReturn = 1;
        ShowCat = catalogue;
        }        
        AmayaPopupList * p_popup = wxDynamicCast(catalogue->Cat_Widget, AmayaPopupList);
        wxASSERT(p_popup);
        p_popup->Popup();
      */

      /* this is the code to use with AmayaPopupList.cpp */
      wxWindow * p_parent = wxDynamicCast(catalogue->Cat_ParentWidget, wxWindow);
      AmayaPopupList * p_popup = (AmayaPopupList*)catalogue->Cat_Widget;
      wxASSERT(p_popup);
      wxPoint pos = wxGetMousePosition();
      p_parent->PopupMenu(p_popup, p_parent->ScreenToClient(pos));
    }      
#endif /* _WX */

#ifdef _WINGUI
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
#endif  /* _WINGUI */
}

/*----------------------------------------------------------------------
  TtaWaitShowProcDialogue
  Waits the return of the catalogue that is shown by TtaShowDialogue.
  The dialog has its own callback handler and the function returns
  when the dialogue disappears.
  ----------------------------------------------------------------------*/
void TtaWaitShowProcDialogue ()
{
#ifdef _WINGUI
  ThotEvent              event;

  /* wait until the user selects something */
  while (GetMessage (&event, NULL, 0, 0))
    {
      TranslateMessage (&event);
      DispatchMessage (&event);
    }
#endif  /* _WINGUI */
#if defined(_GTK) || defined(_WX)
  TtaWaitShowDialogue ();
#endif /* #if defined(_GTK) | _WX */
}

/*----------------------------------------------------------------------
  TtaWaitShowDialogue attends le retour du catalogue affiche par     
  TtaShowDialogue.                                                   
  ----------------------------------------------------------------------*/
void TtaWaitShowDialogue ()
{
  ThotEvent              event;

#ifdef _WINGUI
  GetMessage (&event, NULL, 0, 0);
  TranslateMessage (&event);
  DispatchMessage (&event);
#endif  /* _WINGUI */
#if defined(_GTK) || defined(_WX)
  /* a TtaWaitShowDialogue pending */
  CurrentWait = 1;
  while (ShowReturn == 1)
    TtaHandleOneEvent (&event);
  /* Fin de l'attente */
  CurrentWait = 0;
#endif /* _GTK || _WX */
}

/*----------------------------------------------------------------------
  TtaTestWaitShowDialogue retourne Vrai (1) si un TtaWaitShowDialogue        
  est en cours, sinon Faux (0).                                      
  ----------------------------------------------------------------------*/
ThotBool TtaTestWaitShowDialogue ()
{
  return (CurrentWait != 0);
}

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
