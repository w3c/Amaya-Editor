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
 * Close commands
 *
 * Author: I. Vatton (INRIA)
 *         R. Guetari (W3C/INRIA) - Windows NT/95 routines
 *
 */
 
#include "thot_gui.h"
#include "thot_sys.h"
#include "constmenu.h"
#include "constmedia.h"
#include "typemedia.h"
#include "app.h"
#include "appdialogue.h"
#include "tree.h"
#include "message.h"
#include "dialog.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "edit_tv.h"
#include "frame_tv.h"
#include "appdialogue_tv.h"

static boolean      CloseDontSave;
static boolean      SaveBeforeClosing;

#ifdef _WINDOWS
extern boolean viewClosed;
#endif /* _WINDOWS */

#include "views_f.h"
#include "appdialogue_f.h"
#include "actions_f.h"
#include "docs_f.h"
#include "viewcommands_f.h"

/*----------------------------------------------------------------------
   CallbackCloseDocMenu 
   handles the callbacks of the CloseDoc menu
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CallbackCloseDocMenu (int ref, int typedata, char *data)
#else  /* __STDC__ */
void                CallbackCloseDocMenu (ref, typedata, data)
int                 ref;
int                 typedata;
char               *data;

#endif /* __STDC__ */
{
#  ifndef _WINDOWS
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
#  endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
   AskToConfirm
   asks the user if he wants to save before closing the pDoc document.
   save takes a TRUE value if the document should be saved.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                AskToConfirm (PtrDocument pDoc, Document document, View view, boolean * confirmation, boolean * save)
#else  /* __STDC__ */
void                AskToConfirm (pDoc, document, view, confirmation, save)
PtrDocument         pDoc;
Document            document;
View                view;
boolean            *confirmation;
boolean            *save;

#endif /* __STDC__ */
{
#  ifndef _WINDOWS
   char                buftext[300];
   char                bufbutton[300];
   int                 i;

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

#  ifdef _WINDOWS
   sprintf (message, buftext);
#  endif /* _WINDOWS */

   /* Feuille de dialogue Fermer */
   strcpy (bufbutton, TtaGetMessage (LIB, TMSG_SAVE_DOC));
   i = strlen (TtaGetMessage (LIB, TMSG_SAVE_DOC)) + 1;
   strcpy (bufbutton + i, TtaGetMessage (LIB, TMSG_CLOSE_DON_T_SAVE));
   TtaNewSheet (NumFormClose, TtaGetViewFrame(document,view), 
		TtaGetMessage (LIB, TMSG_CLOSE_DOC), 2, bufbutton, TRUE, 1, 'L', D_CANCEL);
   /* label indiquant le nom du document a sauver avant de fermer */
   TtaNewLabel (NumLabelSaveBeforeClosing, NumFormClose, buftext);
   /* active le formulaire "Fermer" */
   TtaSetDialoguePosition();
   TtaShowDialogue (NumFormClose, FALSE);
   /* attend le retour de ce formulaire (traite' par CallbackCloseDocMenu) */
   TtaWaitShowDialogue ();
#  else  /* _WINDOWS */
   char    buftext[300];
   boolean save_befor, close_dont_save;
   strcpy (buftext, TtaGetMessage (LIB, TMSG_SAVE_DOC));
   strcat (buftext, " ");
   strcat (buftext, pDoc->DocDName);
   strcat (buftext, " ");
   strcat (buftext, TtaGetMessage (LIB, TMSG_BEFORE_CLOSING));
#  ifndef _WIN_PRINT 
   CreateCloseDocDlgWindow (TtaGetViewFrame(document,view), TtaGetMessage (LIB, TMSG_CLOSE_DOC), buftext, &save_befor, &close_dont_save);
#  endif /* _WIN_PRINT */
   SaveBeforeClosing = save_befor ;
   CloseDontSave     = close_dont_save;
#  endif /* _WINDOWS */
   *save = SaveBeforeClosing;
   *confirmation = !CloseDontSave;
}


/*----------------------------------------------------------------------
   CloseADocument
   closes a document.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             CloseADocument (Document document, Document docform, View viewform)
#else  /* __STDC__ */
boolean             CloseADocument (document, docform, viewform)
Document            document;
Document            docform;
View                viewform;

#endif /* __STDC__ */
{
   PtrDocument         pDoc;
   boolean             save, ok;

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
	     /* veut le sauver. L'utilisateur peut aussi annuler la commande. */
	     if (!pDoc->DocReadOnly && pDoc->DocModified)
	       {
		  /* Faut-il creer le formulaire TtcCloseDocument */
		  if (ThotLocalActions[T_confirmclose] == NULL)
		    {
		       /* Connecte le traitement de la TtcCloseDocument */
		       TteConnectAction (T_confirmclose, (Proc) AskToConfirm);
		       TteConnectAction (T_rconfirmclose, (Proc) CallbackCloseDocMenu);
		    }
		  (*ThotLocalActions[T_confirmclose])(pDoc, docform, viewform, &ok, &save);
		  if (ok)
		     /* pas d'annulation */
		    {
		       if (save)
			  /* l'utilisateur demande a sauver le document */
			  ok = WriteDocument (pDoc, 4);
		       if (ok)
			  /* tout va bien, on ferme ce document */
			  CloseDocument (pDoc);
		    }
	       }
	     else
		CloseDocument (pDoc);
	  }
     }
   return (ok);
}


/*----------------------------------------------------------------------
   TtcCloseDocument
   begins closing of a document.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcCloseDocument (Document document, View view)
#else  /* __STDC__ */
void                TtcCloseDocument (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
#  ifdef _WINDOWS
   viewClosed = TRUE;
#  endif /* _WINDOWS */
   CloseADocument (document, document, view);
}
