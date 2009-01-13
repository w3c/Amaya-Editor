/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * Type modification commands
 *
 * Author: I. Vatton (INRIA)
 *
 */
#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "constmenu.h"
#include "appstruct.h"
#include "appdialogue.h"
#include "tree.h"
#include "message.h"
#include "dialog.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "edit_tv.h"
#include "select_tv.h"
#include "appdialogue_tv.h"

#include "appli_f.h"
#include "tree_f.h"
#include "search_f.h"
#include "structcreation_f.h"
#include "creationmenu_f.h"
#include "createabsbox_f.h"
#include "views_f.h"
#include "appdialogue_f.h"
#include "actions_f.h"
#include "exceptions_f.h"
#include "input_f.h"
#include "structmodif_f.h"
#include "structselect_f.h"
#include "structschema_f.h"


/*----------------------------------------------------------------------
   ComposeItemSplit
   Composes in the BufItemSplit buffer the name of the Split item of
   the Edit menu.
  ----------------------------------------------------------------------*/
static void ComposeItemSplit (char *bufItemSplit)
{
  PtrElement     pFirstEl, pLastEl, pList, pPointDiv, pElToDuplicate, pEl;
  int            firstChar, lastChar;
  PtrDocument    pDoc;

   bufItemSplit[0] = EOS;
   /* verifie si la commande Split est valide pour la selection */
   /* courante */
   if (GetCurrentSelection (&pDoc, &pFirstEl, &pLastEl, &firstChar, &lastChar))
     {
	if (!CanSplitElement (pFirstEl, firstChar, TRUE, &pList, &pPointDiv,
			      &pElToDuplicate))
	   CanSplitElement (pFirstEl, firstChar, FALSE, &pList, &pPointDiv,
			    &pElToDuplicate);
	if (pElToDuplicate != NULL && !ElementIsReadOnly (pList) &&
	    !ElementIsReadOnly (pElToDuplicate))
	   /* la commande Split est valide, on compose l'item */
	   /* correspondant du menu Edit */
	  {
	     pEl = pElToDuplicate;
	     /* si c'est un choix, on utilise le type du fils */
	     if (!pEl->ElTerminal && pEl->ElFirstChild != NULL)
		if (pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1]->SrConstruct == CsChoice)
		   pEl = pEl->ElFirstChild;
	     sprintf (bufItemSplit, "%s %s", TtaGetMessage (LIB, TMSG_SPLIT),
		 pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1]->SrName);
	  }
     }
}

/*----------------------------------------------------------------------
   UpdateSplitItem
   updates the title of the Split item of the Edit menu in all the frames
   of document pDoc.
  ----------------------------------------------------------------------*/
void UpdateSplitItem (PtrDocument pDoc)
{
  char              bufItemSplit[MAX_TXT_LEN];

   bufItemSplit[0] = EOS;
   if (pDoc == NULL || pDoc != SelectedDocument)
      return;
   else if (!pDoc->DocReadOnly)
     {
	/* construit l'intitule' de la commande Split en fonction de */
	/* la selection courante */
	ComposeItemSplit (bufItemSplit);
     }
}

/*----------------------------------------------------------------------
   TtcSplit
   splits an element.
  ----------------------------------------------------------------------*/
void TtcSplit (Document document, View view)
{
   /* on essaie d'abord de faire comme la touche Return */
   if (BreakElement (NULL, NULL, 0, TRUE, TRUE))
      return;
   /* puisque ca n'a pas marche', on essaie autre chose */
   BreakElement (NULL, NULL, 0, FALSE, TRUE);
}


/*----------------------------------------------------------------------
   StructEditingingLoadResources
   connects editing structure functions.
  ----------------------------------------------------------------------*/
void StructEditingLoadResources ()
{
   if (ThotLocalActions[T_chsplit] == NULL)
     {
	/* Connecte les actions liees au traitement du split */
	TteConnectAction (T_chsplit, (Proc) UpdateSplitItem);
	TteConnectAction (T_insertpaste, (Proc) CreatePasteIncludeCmd);
	TteConnectAction (T_rcinsertpaste, (Proc) CreatePasteIncludeMenuCallback);
	TteConnectAction (T_rchoice, (Proc) ChoiceMenuCallback);
	EditingLoadResources ();
     }
}







