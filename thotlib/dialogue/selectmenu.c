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
 * Handle selection commands.
 *
 * Authors: I. Vatton (INRIA)
 *          C. Roisin (INRIA) - Columns and pages
 *
 */

#include "thot_gui.h"
#include "thot_sys.h"
#include "message.h"
#include "constmenu.h"
#include "constmedia.h"
#include "dialog.h"
#include "typemedia.h"
#include "appdialogue.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "edit_tv.h"
#include "select_tv.h"
#include "frame_tv.h"
#include "appdialogue_tv.h"

#include "appli_f.h"
#include "tree_f.h"
#include "attrmenu_f.h"
#include "search_f.h"
#include "createabsbox_f.h"
#include "views_f.h"
#include "appdialogue_f.h"
#include "actions_f.h"
#include "viewcommands_f.h"
#include "input_f.h"
#include "keyboards_f.h"
#include "structmodif_f.h"
#include "structselect_f.h"
#include "applicationapi_f.h"


/*----------------------------------------------------------------------
   BuildSelectMenu compose dans le buffer BufMenu le menu       
   Selection et retourne le nombre d'entrees de ce menu.         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 BuildSelectMenu (CHAR_T BufMenu[MAX_TXT_LEN])
#else  /* __STDC__ */
int                 BuildSelectMenu (BufMenu)
CHAR_T                BufMenu[MAX_TXT_LEN];

#endif /* __STDC__ */
{
   int                 i;
   PtrElement          pEl;
   STRING              NomElem;

   /* element englobant non cache' */
   i = 0;
   ustrcpy (&BufMenu[i], "B^ ");
   pEl = SelMenuParentEl;
   if (pEl != NULL && pEl->ElStructSchema != NULL)
      ustrcat (&BufMenu[i], pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrName);
   i += ustrlen (&BufMenu[i]) + 1;

   /* element selectionable precedent */
   ustrcpy (&BufMenu[i], "B< ");
   pEl = SelMenuPreviousEl;
   if (pEl != NULL && pEl->ElStructSchema != NULL)
     {
	NomElem = pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrName;
	ustrcat (&BufMenu[i], NomElem);
     }
   i += ustrlen (&BufMenu[i]) + 1;

   /* element selectionable suivant */
   ustrcpy (&BufMenu[i], "B> ");
   pEl = SelMenuNextEl;
   if (pEl != NULL && pEl->ElStructSchema != NULL)
     {
	NomElem = pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrName;
	ustrcat (&BufMenu[i], NomElem);
     }
   i += ustrlen (&BufMenu[i]) + 1;

   /* cherche le type du 1er element englobe' selectionable */
   ustrcpy (&BufMenu[i], "B_ ");
   pEl = SelMenuChildEl;
   if (pEl != NULL && pEl->ElStructSchema != NULL)
     {
	NomElem = pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrName;
	ustrcat (&BufMenu[i], NomElem);
     }
   i += ustrlen (&BufMenu[i]) + 1;
   return (4);
}

/*----------------------------------------------------------------------
   UpdateSelectMenu                                                     
   Met a jour le menu de Selection                                   
   - de toutes les frames ouvertes du document pDoc                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                UpdateSelectMenu (PtrDocument pDoc)
#else  /* __STDC__ */
void                UpdateSelectMenu (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   int                 NbItemSel;
   CHAR_T                BufMenuSel[MAX_TXT_LEN];
   int                 vue, menu, menuID;
   int                 frame, ref;
   Document            document;
   Menu_Ctl           *ptrmenu;

   if (pDoc == SelectedDocument)
      NbItemSel = BuildSelectMenu (BufMenuSel);
   else
      NbItemSel = 0;

   document = (Document) IdentDocument (pDoc);
   /* Traite toutes les vues de l'arbre principal */
   for (vue = 1; vue <= MAX_VIEW_DOC; vue++)
     {
	frame = pDoc->DocViewFrame[vue - 1];
	if (frame != 0 && FrameTable[frame].MenuSelect != -1)
	  {
	     menuID = FrameTable[frame].MenuSelect;
	     menu = FindMenu (frame, menuID, &ptrmenu) - 1;
	     ref = (menu * MAX_ITEM) + frame + MAX_LocalMenu;
	     if (NbItemSel == 0)
	       {
		  /* le menu Selection contient au moins un item */
		  TtaSetMenuOff (document, vue, menuID);
		  TtaDestroyDialogue (ref);
	       }
	     else
	       {
		  TtaNewPulldown (ref, FrameTable[frame].WdMenus[menu], NULL,
				  NbItemSel, BufMenuSel, NULL);
		  TtaSetMenuOn (document, vue, menuID);
	       }
	  }
     }

   /* Traite toutes les vues des arbres associes */
   for (vue = 1; vue <= MAX_ASSOC_DOC; vue++)
     {
	frame = pDoc->DocAssocFrame[vue - 1];
	if (frame != 0 && FrameTable[frame].MenuSelect != -1)
	  {
	     menuID = FrameTable[frame].MenuSelect;
	     menu = FindMenu (frame, menuID, &ptrmenu) - 1;
	     ref = (menu * MAX_ITEM) + frame + MAX_LocalMenu;
	     if (NbItemSel == 0)
	       {
		  /* le menu Selection contient au moins un item */
		  TtaSetMenuOff (document, vue, menu);
		  TtaDestroyDialogue (ref);
	       }
	     else
	       {
		  TtaNewPulldown (ref, FrameTable[frame].WdMenus[menu], NULL,
				  NbItemSel, BufMenuSel, NULL);
		  TtaSetMenuOn (document, vue, menu);
	       }
	  }
     }
}


/*----------------------------------------------------------------------
   CallbackAttrMenu traite les retours du menu 'Attributs':        
   cree un formulaire pour saisir la valeur de l'attribut choisi.  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CallbackSelectMenu (int refmenu, int val, int frame)
#else  /* __STDC__ */
void                CallbackSelectMenu (refmenu, val, frame)
int                 refmenu;
int                 val;
int                 frame;

#endif /* __STDC__ */
{
   SelectAround (val);
}

/*----------------------------------------------------------------------
   SelectionLoadResources connecte les fonctions de selection.          
  ----------------------------------------------------------------------*/
void                SelectionMenuLoadResources ()
{
   if (ThotLocalActions[T_chselect] == NULL)
     {
	/* Connecte les actions de selection */
	TteConnectAction (T_chselect, (Proc) UpdateSelectMenu);
	TteConnectAction (T_rselect, (Proc) CallbackSelectMenu);
     }
}
