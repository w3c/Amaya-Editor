/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2001
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Close commands
 *
 * Authors: I. Vatton (INRIA)
 *          R. Guetari (W3C/INRIA) - Unicode and Windows version
 *
 */

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmenu.h"
#include "constmedia.h"
#include "typemedia.h"
#include "appstruct.h"
#include "appdialogue.h"
#include "tree.h"
#include "message.h"
#include "dialog.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "edit_tv.h"
#include "frame_tv.h"
#include "appdialogue_tv.h"

static ThotBool     CloseDontSave;
static ThotBool     SaveBeforeClosing;

#include "actions_f.h"
#include "appdialogue_f.h"
#include "displayview_f.h"
#include "documentapi_f.h"
#include "docs_f.h"
#include "views_f.h"

#ifdef _WINDOWS
#include "wininclude.h"
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
   CallbackCloseDocMenu 
   handles the callbacks of the CloseDoc menu
  ----------------------------------------------------------------------*/
void CallbackCloseDocMenu (int ref, int typedata, char *data)
{
#ifndef _WINDOWS
  switch ((int) data)
    {
    case 0:
      /* abandon du formulaire */
      break;
    case 1:
      /* sauver avant de fermer */
      CloseDontSave = FALSE;
      SaveBeforeClosing = TRUE;
      break;
    case 2:
      /* fermer sans sauver */
      CloseDontSave = FALSE;
      break;
    }
  TtaDestroyDialogue (NumFormClose);
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
   AskToConfirm
   asks the user if he wants to save before closing the pDoc document.
   save takes a TRUE value if the document should be saved.
  ----------------------------------------------------------------------*/
void AskToConfirm (PtrDocument pDoc, Document document, View view,
		   ThotBool *confirmation, ThotBool *save)
{
  char              buftext[300];
#ifndef _WINDOWS
  char              bufbutton[300];
  int                 i;
#endif /* _WINDOWS */

  CloseDontSave = TRUE;
  SaveBeforeClosing = FALSE;
  /* le document a ete modifie', on propose de le sauver */
  /* initialise le label du formulaire "Fermer" en y mettant le nom */
  /* du document */
  strcpy (buftext, TtaGetMessage (LIB, TMSG_SAVE_DOC));
  strcat (buftext, " ");
  strcat (buftext, pDoc->DocDName);
  strcat (buftext, " ");
  strcat (buftext, TtaGetMessage (LIB, TMSG_BEFORE_CLOSING));

#ifndef _WINDOWS
  /* Feuille de dialogue Fermer */
  strcpy (bufbutton, TtaGetMessage (LIB, TMSG_SAVE_DOC));
  i = strlen (TtaGetMessage (LIB, TMSG_SAVE_DOC)) + 1;
  strcpy (bufbutton + i, TtaGetMessage (LIB, TMSG_CLOSE_DON_T_SAVE));
  TtaNewSheet (NumFormClose, TtaGetViewFrame(document,view), 
	       TtaGetMessage (LIB, TMSG_CLOSE_DOC), 2, bufbutton, TRUE, 1,
	       'L', D_CANCEL);
  /* label indiquant le nom du document a sauver avant de fermer */
  TtaNewLabel (NumLabelSaveBeforeClosing, NumFormClose, buftext);
  /* active le formulaire "Fermer" */
  TtaSetDialoguePosition();
  TtaShowDialogue (NumFormClose, FALSE);
  /* attend le retour de ce formulaire (traite' par CallbackCloseDocMenu) */
  TtaWaitShowDialogue ();
#else  /* _WINDOWS */
  CreateCloseDocDlgWindow (TtaGetViewFrame(document,view), buftext,
			   &SaveBeforeClosing, &CloseDontSave);
#endif /* _WINDOWS */
  *save = SaveBeforeClosing;
  *confirmation = !CloseDontSave;
}


/*----------------------------------------------------------------------
   CloseADocument
   closes a document.
  ----------------------------------------------------------------------*/
ThotBool            CloseADocument (Document document, Document docform,
				    View viewform)
{
  PtrDocument         pDoc;
  ThotBool            save, ok;

  ok = TRUE;
  if (document == 0)
    return FALSE;
  else
    {
      pDoc = LoadedDocument[document - 1];
      if (pDoc != NULL)
	/* il y a un document pour cette entree de la table */
	{
	  /* si le document a ete modifie', demande a l'utilisateur s'il */
	  /* veut le sauver. L'utilisateur peut aussi annuler la commande.*/
	  if (!pDoc->DocReadOnly && pDoc->DocModified)
	    {
	      /* Faut-il creer le formulaire TtcCloseDocument */
	      if (ThotLocalActions[T_confirmclose] == NULL)
		{
		/* Connecte le traitement de la TtcCloseDocument */
		TteConnectAction (T_confirmclose, (Proc) AskToConfirm);
		TteConnectAction (T_rconfirmclose, (Proc)CallbackCloseDocMenu);
		}
	      AskToConfirm (pDoc, docform, viewform, &ok, &save);
	      if (ok)
		/* pas d'annulation */
		{
		  if (save && ThotLocalActions[T_writedocument])
		    /* l'utilisateur demande a sauver le document */
		    ok = (* (Func)ThotLocalActions[T_writedocument]) (pDoc, 4);
		  if (ok)
		    /* tout va bien, on ferme ce document */
		    TCloseDocument (pDoc);
		}
	    }
	  else
	    TCloseDocument (pDoc);
	}
    }
  return (ok);
}

/*----------------------------------------------------------------------
   TtcCloseDocument
   begins closing of a document.
  ----------------------------------------------------------------------*/
void                TtcCloseDocument (Document document, View view)
{
  CloseADocument (document, document, view);
}
