/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
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
#if defined (_WINGUI) || defined(_WX)
  char                menu_item [1024];
  char                equiv_item [255];

  equiv_item[0] = 0;
#endif /* _WINGUI || _WX */
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
                  }
                else if (text[index] == 'S')
                  /*_________________________________ Creation d'un separateur __*/
                  {
#ifdef _WINGUI
                    AppendMenu (menu, MF_SEPARATOR, 0, NULL);
                    adbloc->E_ThotWidget[ent] = (ThotWidget) 0;
#endif  /* _WINGUI */
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
    }
#endif /* _WX */
}

#if defined (_WINGUI) || defined(_WX)
/*----------------------------------------------------------------------
  TtaSetPulldownOff suspend le pulldown                           
  ----------------------------------------------------------------------*/
#ifdef _WINGUI
void WIN_TtaSetPulldownOff (int ref, ThotMenu parent, HWND owner)
#endif /* _WINGUI */
#ifdef _WX
void TtaSetPulldownOff (int ref, ThotMenu parent)
#endif /* _WX */
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
#ifdef _WX
     void TtaSetPulldownOn (int ref, ThotMenu parent)
#endif /* _WX */
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
#ifdef _WINGUI
          frame = GetMainFrameNumber (owner);
          EnableMenuItem ((HMENU)WinMenus[frame], (UINT)parent, MF_ENABLED);
          DrawMenuBar (FrMainRef[frame]); 
#endif /* _WINGUI */
        }
    }
#endif /* _WX */
}
#endif /* _WINGUI || _WX */

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
#ifdef _WINGUI
  register int        count;
  register int        index;
  register int        ent;
  register int        i;
  int                 eindex;
  ThotBool            rebuilded;
  struct Cat_Context *catalogue;
  struct E_List      *adbloc;
  HMENU               menu;
  HMENU               w;
  int                 nbOldItems, ndx;
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
      if (!rebuilded)
        {
          menu = CreatePopupMenu ();
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
      if (parent)
        WIN_AddFrameCatalogue (parent, catalogue);
      
      /*** Cree le titre du menu ***/
      if (title)
        {
          if (!rebuilded)
            {
              adbloc->E_ThotWidget[0] = (ThotWidget) 0;
              adbloc->E_ThotWidget[1] = (ThotWidget) 0;
            }
        }
      /* Cree les differentes entrees du menu */
      nbOldItems = GetMenuItemCount (menu);
      for (ndx = 0; ndx < nbOldItems; ndx ++)
        if (!DeleteMenu (menu, ref + ndx, MF_BYCOMMAND))
          DeleteMenu (menu, ndx, MF_BYPOSITION);
      
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
                    eindex += strlen (&equiv[eindex]) + 1;
                  }
                if (text[index] == 'B')
                  /*__________________________________________ Creation d'un bouton __*/
                  {
                    AppendMenu (menu, MF_STRING, ref + i, &text[index + 1]);
                    adbloc->E_ThotWidget[ent] = (ThotWidget) i;
                  }
                else if (text[index] == 'T')
                  /*__________________________________________ Creation d'un toggle __*/
                  {
                    AppendMenu (menu, MF_STRING | MF_UNCHECKED, ref + i, &text[index + 1]);
                    adbloc->E_ThotWidget[ent] = (ThotWidget) i;
                  }
                else if (text[index] == 'M')
                  /*_______________________________________ Creation d'un sous-menu __*/
                  {
                    /* En attendant le sous-menu on cree un bouton */
                    w = (HMENU) CreateMenu ();
                    AppendMenu (menu, MF_POPUP, (UINT) w, (LPCTSTR) (&text[index + 1]));
                    adbloc->E_ThotWidget[ent] = (ThotWidget) w;
                  }
                else if (text[index] == 'S')
                  /*_________________________________ Creation d'un separateur __*/
                  {
                    AppendMenu (menu, MF_SEPARATOR, 0, NULL);
                    adbloc->E_ThotWidget[ent] = (ThotWidget) i;
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
#endif /* _WINGUI  */
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
#ifdef _WINGUI
  char                menu_item [1024];
  char                equiv_item [255];

  equiv_item[0] = EOS;
#endif /* _WINGUI */
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
            }
          else
            /* Sinon on recupere le widget parent des entrees */
            row = catalogue->Cat_XtWParent;

          /*** Cree les differentes entrees du sous-menu ***/
          i = 0;
          index = 0;
          eindex = 0;
          ent = 2;
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
                    }
                  else if (text[index] == 'M')
                    {
                      /*_________________________________ Appel d'un sous-menu __*/
                    }
                  else if (text[index] == 'S')
                    {
                      /*_____________________________ Creation d'un separateur __*/
#ifdef _WINGUI
                      AppendMenu (w, MF_SEPARATOR, 0, NULL);
                      adbloc->E_ThotWidget[ent] = (ThotWidget) 0;
#endif  /* _WINGUI */
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
#ifdef _WX
      /* TODO: a faire qd on aura porte les dialogues */
#endif /* _WX */
    }
}

/*----------------------------------------------------------------------
  TtaSetDefaultButton
  Defines the default result.
  ----------------------------------------------------------------------*/
void TtaSetDefaultButton (int ref, int button)
{
#ifdef _WX
  struct Cat_Context *catalogue;

  if (ref)
    {
      catalogue = CatEntry (ref);
      if (catalogue)
        catalogue->Cat_Default = (unsigned char) button;
    }
#endif /* _WX */
}

/*----------------------------------------------------------------------
  TtaAbortShowDialogue abandonne le TtaShowDialogue.                 
  ----------------------------------------------------------------------*/
void TtaAbortShowDialogue ()
{
#ifdef _WX
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
void TtaShowDialogue (int ref, ThotBool remanent, ThotBool move)
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
	  
	      if (move)
		  {
            wxPoint pos = wxGetMousePosition();
            wxDisplaySize(&display_width_px, &display_height_px);
            catalogue->Cat_Widget->GetSize (&width, &height);
            if (pos.x + width > display_width_px)
              pos.x = display_width_px - width;
            if (pos.y + height > display_height_px)
              pos.x = display_height_px - height;
			pos.x -= 4;
			pos.y -= 4;
	        catalogue->Cat_Widget->Move(pos);
		  } 
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
#ifdef _WX
  TtaWaitShowDialogue ();
#endif /* _WX */
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
#ifdef _WX
  /* a TtaWaitShowDialogue pending */
  CurrentWait = 1;
  while (ShowReturn == 1)
    TtaHandleOneEvent (&event);
  /* Fin de l'attente */
  CurrentWait = 0;
#endif /* _WX */
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
  register int        icat;
  struct Cat_List    *current, *next;
  struct Cat_Context *catalogue;
  
  current = PtrCatalogue;
  while (current)
    {
	  next = current->Cat_Next;
      icat = 0;
      while (icat < MAX_CAT)
        {
          catalogue = &current->Cat_Table[icat];
          if (catalogue &&
              catalogue->Cat_Type == CAT_DIALOG && catalogue->Cat_Widget)
            catalogue->Cat_Widget->Destroy();
		  {
			catalogue->Cat_Ref = 0;
			catalogue->Cat_Widget = NULL;
		  }
          icat++;
        }
	  if (current == PtrCatalogue)
		// keep an initial list
        current->Cat_Next = NULL;
	  else
        TtaFreeMemory (current);
      current = next;
    }
}
