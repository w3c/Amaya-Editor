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
 *
 * Author: I. Vatton (INRIA)
 *
 */
 
#include "thot_gui.h"
#include "thot_sys.h"
#include "constmenu.h"
#include "app.h"
#include "constmedia.h"
#include "typemedia.h"
#include "appdialogue.h"
#include "tree.h"
#include "message.h"
#include "dialog.h"

static boolean      InProcedureQuitThot = FALSE;

#include "appdialogue_f.h"
#include "applicationapi_f.h"
#include "callback_f.h"
#include "closedoc_f.h"
/*----------------------------------------------------------------------
   TtcQuit
   Initializes and shows the quit form.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcQuit (Document document, View view)

#else  /* __STDC__ */
void                TtcQuit (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   int                 doc;
   NotifyEvent         notifyEvt;
   boolean             ok;

   /* Sauf incident, on va quitter Thot */

   if (InProcedureQuitThot)
      return;
   InProcedureQuitThot = TRUE;
   /* envoie le message Exit.Pre */
   notifyEvt.event = TteExit;
   if (!CallEventType (&notifyEvt, TRUE))
      /* l'application accepte de quitter l'editeur */
     {
	ok = TRUE;
	/* parcourt la table des documents, tant que l'utilisateur n'annule */
	/* pas sa commande et qu'on n'a pas de probleme de sauvegarde */
	for (doc = 0; doc < MAX_DOCUMENTS && ok; doc++)
	   if (doc != document)
	      /* traite d'abord tous les autres documents */
	      ok = CloseADocument (doc, document, view);
	/* Traite le document */
	if((int)document != 0)
	  ok = CloseADocument (document, document, view);
	/* si la commande n'a pas ete annulee, on quitte Thot a regrets */
	if (ok)
	  {
	     /* envoie le message Exit.Post */
	     notifyEvt.event = TteExit;
	     CallEventType (&notifyEvt, FALSE);
	     /* quitte definitivement Thot */
	     TtaQuit ();
	  }
     }
   InProcedureQuitThot = FALSE;
}

