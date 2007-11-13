/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2007
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


#include "wx/wx.h"
#include "wx/panel.h"


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



#include "AmayaFrame.h"
#include "AmayaWindow.h"
#include "AmayaCallback.h"
#include "appdialogue_wx_f.h"
#include "appdialogue_wx.h"
#include "message_wx.h"
#include "AmayaPopupList.h"


#define THOT_EXPORT
#define THOT_INITGLOBALVAR
#include "dialogapi_tv.h" /* global variable implementation */
#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "appdialogue_tv.h"
#include "thotcolor_tv.h"
#include "frame_tv.h"
#include "font_tv.h"

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

/*----------------------------------------------------------------------
  TtaCatwidget returns the widget that owns the catalog given in ref
  ----------------------------------------------------------------------*/
ThotWidget TtaCatWidget(int ref)
{
  return (CatWidget (ref));
}

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
  wxMessageDialog messagedialog( NULL,
                                 TtaConvMessageToWX(text), 
                                 TtaConvMessageToWX(TtaGetMessage(LIB, TMSG_LIB_CONFIRM)),
                                 (long) wxOK | wxICON_EXCLAMATION | wxSTAY_ON_TOP);
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
                /* nothing is done here because the menu items widgets are destroyed later */
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
}

/*----------------------------------------------------------------------
  TtaSetPulldownOff suspend le pulldown                           
  ----------------------------------------------------------------------*/
void TtaSetPulldownOff (int ref, ThotMenu parent)
{
}

/*----------------------------------------------------------------------
  TtaSetPulldownOn reactive le pulldown                           
  ----------------------------------------------------------------------*/
void TtaSetPulldownOn (int ref, ThotMenu parent)
{
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

  char                menu_item [1024];
  ThotWidget          menu = NULL;

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
      menu = (ThotWindow)new AmayaPopupList( parent, ref );
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

                sprintf (menu_item, "%s", &text[index + 1]);
                ((AmayaPopupList*)menu)->Append(i, TtaConvMessageToWX(menu_item));
                w = (ThotWidget) i;
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
}


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
}

/*----------------------------------------------------------------------
  TtaSetMenuForm fixe la selection dans un sous-menu de formulaire : 
  The parameter ref donne la re'fe'rence du catalogue.               
  The parameter val de'signe l'entre'e se'lectionne'e.               
  ----------------------------------------------------------------------*/
void TtaSetMenuForm (int ref, int val)
{
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
  wxASSERT_MSG(FALSE, _T("TtaRedrawMenuEntry : to remove"));
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

          if (catalogue->Cat_Type == CAT_DIALOG)
            if (catalogue->Cat_Widget)
              catalogue->Cat_Widget->Destroy();

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
  /* hide the widget */
  if (catalogue->Cat_Type == CAT_DIALOG && catalogue->Cat_Widget)
    {
      catalogue->Cat_Widget->Show( true );
      catalogue->Cat_Widget->Raise( );
    }
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


  /* hide the widget */
  if (catalogue->Cat_Type == CAT_DIALOG)
    if (catalogue->Cat_Widget)
      catalogue->Cat_Widget->Hide();

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
                          /* nothing is done here because the menu will be deleted
                           * when a new menu is created (TtaNewPulldown) */
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
          /* TODO : a faire qd on aura porte les dialogues */
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
      if (catalogue->Cat_Type == CAT_DIALOG || catalogue->Cat_Type == CAT_SCRPOPUP)
        if (catalogue->Cat_Widget)
          {
            catalogue->Cat_Widget->Destroy();

            /* then give focus to canvas */
            //	      TtaRedirectFocus();
          }

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
      /* TODO: a faire qd on aura porte les dialogues */
    }
}

/*----------------------------------------------------------------------
  TtaSetDefaultButton
  Defines the default result for the GTK event Double-click.
  ----------------------------------------------------------------------*/
void TtaSetDefaultButton (int ref, int button)
{
  struct Cat_Context *catalogue;

  if (ref)
    {
      catalogue = CatEntry (ref);
      if (catalogue)
        catalogue->Cat_Default = (unsigned char) button;
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
}

/*----------------------------------------------------------------------
  TtaAttachForm attache le catalogue au formulaire ou au feuillet    
  dont il de'pend. Les catalogues sont cre'e's attache's.            
  ----------------------------------------------------------------------*/
void TtaAttachForm (int ref)
{
}

/*----------------------------------------------------------------------
  TtaDetachForm detache le catalogue au formulaire ou au feuillet    
  dont il de'pend. Les catalogues sont cre'e's attache's.            
  ----------------------------------------------------------------------*/
void TtaDetachForm (int ref)
{
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
}

/*----------------------------------------------------------------------
  TtaActiveSelector rend actif le  se'lecteur.                       
  ----------------------------------------------------------------------*/
void TtaActiveSelector (int ref)
{
}

/*----------------------------------------------------------------------
  TtaDesactiveSelector rend non actif le  se'lecteur.                        
  ----------------------------------------------------------------------*/
void TtaDesactiveSelector (int ref)
{
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
}

/*----------------------------------------------------------------------
  TtaNewLabel cre'e un intitule' constant dans un formulaire :       
  The parameter ref donne la re'fe'rence du catalogue.               
  The parameter text donne l'intitule'.                              
  ----------------------------------------------------------------------*/
void TtaNewLabel (int ref, int ref_parent, char *text)
{
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
}
/*----------------------------------------------------------------------
  TtaSetTextForm initialise une feuille de saisie de texte :         
  The parameter ref donne la re'fe'rence du catalogue.               
  The parameter text donne la valeur initiale.                       
  ----------------------------------------------------------------------*/
void TtaSetTextForm (int ref, char *text)
{
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
}

/*----------------------------------------------------------------------
  TtaSetNumberForm fixe le contenu de la feuille de saisie de texte :        
  The parameter ref donne la re'fe'rence du catalogue.               
  The parameter val donne la valeur initiale.                        
  ----------------------------------------------------------------------*/
void TtaSetNumberForm (int ref, int val)
{
}


/*----------------------------------------------------------------------
  TtaClearTree
  Clears (destroys) the contents of a tree created with TtaNewTreeForm.
  Returns the identifier of the tree widget.
  ----------------------------------------------------------------------*/
ThotWidget TtaClearTree (ThotWidget tree)
     /* add some tree stuff here */
{
  ThotWidget tree_widget = NULL;
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
  return w;
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
      if (ShowCat  && ShowCat->Cat_Widget)
        {
          ShowCat->Cat_Widget->Hide();
        }
    }
}

/*----------------------------------------------------------------------
  TtaSetDialoguePosition me'morise la position actuelle de la souris 
  comme position d'affichage des TtaShowDialogue().               
  ----------------------------------------------------------------------*/
void TtaSetDialoguePosition ()
{
  wxPoint p = wxGetMousePosition();
  ShowX = p.x;
  ShowY = p.y;
}

/*----------------------------------------------------------------------
  TtaShowDialogue active le catalogue de'signe.                      
  ----------------------------------------------------------------------*/
void TtaShowDialogue (int ref, ThotBool remanent)
{
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
}

/*----------------------------------------------------------------------
  TtaWaitShowProcDialogue
  Waits the return of the catalogue that is shown by TtaShowDialogue.
  The dialog has its own callback handler and the function returns
  when the dialogue disappears.
  ----------------------------------------------------------------------*/
void TtaWaitShowProcDialogue ()
{
  TtaWaitShowDialogue ();
}

/*----------------------------------------------------------------------
  TtaWaitShowDialogue attends le retour du catalogue affiche par     
  TtaShowDialogue.                                                   
  ----------------------------------------------------------------------*/
void TtaWaitShowDialogue ()
{
  ThotEvent              event;

  /* a TtaWaitShowDialogue pending */
  CurrentWait = 1;
  while (ShowReturn == 1)
    TtaHandleOneEvent (&event);
  /* Fin de l'attente */
  CurrentWait = 0;
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
