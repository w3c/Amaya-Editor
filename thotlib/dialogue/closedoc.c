/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
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
#include "registry_f.h"
#ifdef _WINGUI
  #include "wininclude.h"
#endif /* _WINGUI */

/*----------------------------------------------------------------------
   CallbackCloseDocMenu 
   handles the callbacks of the CloseDoc menu
  ----------------------------------------------------------------------*/
void CallbackCloseDocMenu (int ref, int typedata, char *data)
{
  switch ((long int) data)
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
#ifdef _GTK
   TtaDestroyDialogue (NumFormClose);
#endif /* _GTK */
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
#ifdef _GTK
  char              bufbutton[300];
  int                 i;
#endif /* _GTK */

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

#ifdef _GTK
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
  TtaShowDialogue (NumFormClose, FALSE, TRUE);
  /* attend le retour de ce formulaire (traite' par CallbackCloseDocMenu) */
  TtaWaitShowDialogue ();
#endif /* _GTK */
#ifdef _WINGUI
  CreateCloseDocDlgWindow (TtaGetViewFrame(document,view), buftext);
#endif /* _WINGUI */
  *save = SaveBeforeClosing;
  *confirmation = !CloseDontSave;
}


/*----------------------------------------------------------------------
   CloseADocument
   closes a document.
  ----------------------------------------------------------------------*/
ThotBool CloseADocument (Document document, Document docform, View viewform)
{
  PtrDocument         pDoc;
  ThotBool            save, ok, always_replace;

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
	  /* if (!pDoc->DocReadOnly && pDoc->DocModified) */
	  TtaGetEnvBoolean ("IGNORE_UPDATES", &always_replace);
	  if (!always_replace && pDoc->DocModified)
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
		    ok = (* (Func2)ThotLocalActions[T_writedocument]) (
			(void *)pDoc,
			(void *)4);
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
void TtcCloseDocument (Document document, View view)
{
  CloseADocument (document, document, view);
}
