
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */
/* I. Vatton    Mai 1994 */

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

#include "appdialogue.f"
#include "appexec.f"
#include "edit.f"

#ifdef __STDC__
extern boolean      FermeUnDocument (Document, Document, View);
extern void         QuitEditor (void);

#else
extern boolean      FermeUnDocument ();
extern void         QuitEditor ();

#endif

/* ---------------------------------------------------------------------- */
/* | TtcQuit initialise le changement de TtcQuit.                       | */
/* ---------------------------------------------------------------------- */
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
   if (!ThotSendMessage (&notifyEvt, TRUE))
      /* l'application accepte de quitter l'editeur */
     {
	ok = TRUE;
	/* parcourt la table des documents, tant que l'utilisateur n'annule */
	/* pas sa commande et qu'on n'a pas de probleme de sauvegarde */
	for (doc = 0; doc < MAX_DOCUMENTS && ok; doc++)
	   if (doc != document)
	      /* traite d'abord tous les autres documents */
	      ok = FermeUnDocument (doc, document, view);
	/* Traite le document */
	ok = FermeUnDocument (document, document, view);
	/* si la commande n'a pas ete annulee, on quitte Thot a regrets */
	if (ok)
	  {
	     /* envoie le message Exit.Post */
	     notifyEvt.event = TteExit;
	     ThotSendMessage (&notifyEvt, FALSE);
	     /* quitte definitivement Thot */
	     QuitEditor ();
	  }
     }
   InProcedureQuitThot = FALSE;
}
